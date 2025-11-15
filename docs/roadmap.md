# Roadmap

## Phase 0 – Documentation & Planning
- Establish repository structure (firmware, infra, docs).
- Capture hardware inventory, architecture, and control requirements.
- Define telemetry schema and command vocabulary at a high level.

## Phase 1 – Firmware MVP
- Bring up Waveshare ESP32-S3 in PlatformIO/Arduino with relay + sensor pin mappings.
- Implement wake/sleep cycle, telemetry payload, and basic command polling over HTTPS.
- Integrate DHT sensor, cistern ultrasonic level, and voltage sensing.
- Add configuration file for Wi-Fi credentials, device id, and polling cadence.

## Phase 2 – Cloud Workflows & Database
- Create Postgres schema for devices, telemetry, commands, command_executions, and firmware_versions.
- Build n8n workflows for `/cabin/telemetry` and `/cabin/commands` endpoints with validation and queuing.
- Add command acknowledgement endpoint and n8n-based automation rules (e.g., auto-fill cistern).

## Phase 3 – Advanced Control & OTA
- Add RS485 JKBMS integration for 48 V battery metrics.
- Implement generator start/stop safety interlocks and watchdog timers.
- Introduce OTA firmware update workflow with signed binaries and rollback slot management.
- Deploy optional local LCD/UI for manual overrides and safe-mode.

## Phase 4 – Hardening & Deployment
- End-to-end soak testing with simulated sensor inputs and relay loads.
- Add alerting hooks (email/SMS) from n8n when faults or thresholds trigger.
- Document maintenance procedures, firmware release process, and recovery steps.
