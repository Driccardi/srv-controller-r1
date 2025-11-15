# n8n Workflows

## `/cabin/telemetry`
- HTTP webhook receives JSON payload from ESP32.
- Validate device token/id, enforce schema.
- Insert payload into `telemetry` table, update `devices.last_seen_at`.
- Run rule set:
  - Queue pump start if cistern below minimum.
  - Queue heater adjustments based on temperature thresholds.
  - Trigger alerts if voltages exceed safe limits.
- Update operational settings when needed (e.g., confirm the `telemetry_interval_minutes` value so the firmware can adjust the wake/sleep timer without reflashing).

## `/cabin/commands`
- HTTP webhook authenticates device and returns pending commands ordered by `queued_at`.
- Marks commands as "reserved" to avoid duplicate delivery.
- Includes metadata for each command (id, type, parameters).

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

## Settings Management
- Provide an administrative workflow that writes key/value pairs to the `settings` table.
- At minimum seed `telemetry_interval_minutes = 5` so devices share a consistent default cadence.
- Changes to settings should trigger notifications so operators know when wake/sleep intervals or other global behaviors shift.
