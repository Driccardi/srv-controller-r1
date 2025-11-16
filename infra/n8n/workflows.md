# n8n Workflows

## `/cabin/telemetry`
- HTTP webhook receives JSON payload from ESP32.
- Validate device token/id, enforce schema.
- Insert payload into `telemetry` table (including `system_status`, `firmware_version`, `last_command_id`, and `relay_states`), update `devices.last_seen_at`.
- For each scalar in the payload (temperatures, voltages, cistern levels, heater states, etc.) upsert a `device_metrics` row (with alarm limits/unit metadata) and append a `telemetry_measurements` row so dashboards can chart the trend.
- Run rule set:
  - Queue pump start if cistern below minimum.
  - Queue heater adjustments based on temperature thresholds.
  - Trigger alerts if voltages exceed safe limits.
- Raise alerts when per-metric alarm bands are exceeded, respecting the `alarm_low` / `alarm_high` limits stored next to each metric.
- Update operational settings when needed (e.g., confirm the `telemetry_interval_minutes` value so the firmware can adjust the wake/sleep timer without reflashing).

## `/cabin/commands`
- HTTP webhook authenticates device and returns pending commands ordered by `queued_at`.
- Marks commands as "reserved" to avoid duplicate delivery.
- Includes metadata for each command (id, type, parameters) plus redundancy helpers (`priority`, `expires_at`, `redundancy_key`, `requires_ack`).
- Recommended command categories:
  - **Heaters & pumps:** zone heaters, circulation pumps, battery heaters with manual overrides and hold-until timestamps.
  - **Generator control:** manual start/stop, voltage windows for auto-start, heartbeat watchdog timers.
  - **Water system:** cistern fill bands, pump redundancy state, winter flush/purge routines.
  - **Lighting scenes:** day/night/emergency/low-voltage scenes, brightness trims, scheduled programs.
  - **Solar actuators:** two-axis tracker mode, manual jog, snow-dump cycle.
  - **Settings sync:** telemetry cadence or other key/value configuration pushes so the device stays consistent with the `settings` table.

## `/cabin/commands/ack` (planned)
- Device reports command execution result (success/failure, timestamps).
- Workflow updates `commands.executed` and inserts row into `command_executions`.

## `/cabin/firmware/check` (planned)
- Device sends current firmware version + hardware revision.
- Workflow compares against `firmware_versions` table.
- If newer version is approved, respond with download URL + checksum.

## Alerting & Notifications
- When automation rules trigger critical actions (generator start, low battery), send notifications via email/SMS.
- Capture manual approvals in n8n to gate high-risk commands.
- Build notification nodes that summarize telemetry anomalies (alarm band violations, repeated failsafe status) so operators know when redundancy features are active.

## Settings Management
- Provide an administrative workflow that writes key/value pairs to the `settings` table.
- At minimum seed `telemetry_interval_minutes = 5` so devices share a consistent default cadence.
- Changes to settings should trigger notifications so operators know when wake/sleep intervals or other global behaviors shift.

## Node JSON Exports
The `infra/n8n/nodes/` directory contains ready-to-import workflow JSON for each major integration point:

| Workflow | File |
| --- | --- |
| Telemetry ingest, validation, persistence, and alert fan-out | `infra/n8n/nodes/cabin-telemetry-ingest.json` |
| Pending command retrieval plus reservation | `infra/n8n/nodes/cabin-commands-dispatch.json` |
| Command acknowledgements and execution audit trail | `infra/n8n/nodes/cabin-commands-ack.json` |
| Firmware update eligibility check + auditing | `infra/n8n/nodes/cabin-firmware-check.json` |
| Manual settings administration with notification | `infra/n8n/nodes/cabin-settings-admin.json` |
| Scheduled alarm-band scanning and notifications | `infra/n8n/nodes/cabin-alerting-router.json` |
| Telemetry interpreter agent with memory + OpenAI chat node | `infra/n8n/nodes/cabin-telemetry-agent.json` |
| Daily summarizer agent with 24-hour rollups | `infra/n8n/nodes/cabin-daily-summary-agent.json` |
| Command reliability auditor agent | `infra/n8n/nodes/cabin-command-auditor.json` |

Each export includes placeholder credential names (e.g., `Cabin Postgres`) and webhook paths that align with the documented API surface so they can be imported directly into n8n and wired to environment-specific secrets.

## Agentic workflows
- **Telemetry Interpreter** (`infra/n8n/agents/telemetry-interpreter.md` + `cabin-telemetry-agent.json`): Cron trigger every five minutes pulls the latest 20 thermal metrics, attaches scratchpad memory, invokes an OpenAI Chat node, and fans out any command suggestions into the `commands` table while emailing urgent alerts.
- **Daily Operations Summarizer** (`infra/n8n/agents/daily-summarizer.md` + `cabin-daily-summary-agent.json`): 07:00 cron aggregates the previous 24 hours of telemetry and command history, feeds the agent with prior-day memory, and emails a markdown digest to operators.
- **Command Reliability Auditor** (`infra/n8n/agents/command-auditor.md` + `cabin-command-auditor.json`): Hourly cron scans for stuck commands, lets the agent decide which to retry or cancel, bumps priority/redundancy keys via Postgres, and escalates repeated failures through email alerts.
