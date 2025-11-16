# Command Reliability Auditor Agent

## Objective
Keep the remote cabin reliable by scanning the `commands` table for items that are stuck, duplicated, or conflicting. The auditor ensures each queued action has a matching acknowledgement and that redundant commands are issued when the primary channel fails.

## Scope
- Runs hourly (or via manual trigger) across all devices.
- Reviews commands with `status` in (`queued`, `sent`) older than 10 minutes or that have expired.
- Looks for conflicting directives (e.g., simultaneous `generator_on` and `generator_off`).

## Inputs
- Pending command rows with metadata (priority, redundancy_key, expires_at).
- Device heartbeat (last telemetry timestamp) to distinguish offline devices from execution failures.
- Memory bucket `command_auditor_context` storing the last set of remediated command IDs.

## Tools
1. Postgres read/write.
2. Slack/email notifier for human-required interventions.
3. Command buffer write access to clone/resend commands with incremented redundancy counters.

## Actions
- **Resend** – Recreate a command with `priority='high'` and `redundancy_key` suffix `-retry` when acknowledgement is missing but the device is online.
- **Cancel** – Update conflicting or expired commands to `status='cancelled'` and document the reason in `command_executions`.
- **Escalate** – Email operators if the same command fails more than twice within an hour.

## Output Format
```json
{
  "summary": "text",
  "rescuedCommandIds": ["uuid"],
  "cancelledCommandIds": ["uuid"],
  "alerts": [
    {"severity": "warning", "subject": "Generator command stuck", "message": "..."}
  ]
}
```

Ensure all updates are idempotent and logged.
