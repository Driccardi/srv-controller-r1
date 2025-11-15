# Architecture Overview

The Swift Cabin Controller coordinates sensors, relays, and cloud workflows to safely automate an off-grid cabin.

## Cabin Device
- **Hardware platform:** Waveshare ESP32-S3 board with six onboard relays and headers for auxiliary sensors.
- **Power model:** Device wakes from deep sleep every 5 minutes (default), performs telemetry/command exchange, then returns to deep sleep to save battery. The interval is managed remotely via the cloud `settings` table so operators can lengthen or shorten the cadence without reflashing firmware.
- **Sensors:**
  - DHT11/22 for temperature/humidity in key cabin zones.
  - HC-SR04 ultrasonic sensor aimed at the cistern for water level.
  - Voltage divider module to monitor 48 V battery bank and solar input (isolated sensing).
  - Future RS485-to-UART adapter for JKBMS battery telemetry.
- **Actuators:**
  - 6 onboard relays for light-duty loads or contactor coils.
  - DIN-rail latching relays for generator start/stop, heaters, pumps, and valves.
  - Solid-state relays (Omron G3MB) for smaller AC loads.
- **Networking:** Wi-Fi client using HTTPS REST APIs to communicate with the cloud control plane.

## Cloud Control Plane
- **n8n workflows** provide REST webhooks for telemetry ingestion, command retrieval, acknowledgements, and future OTA firmware negotiation.
- **Postgres database** persists devices, telemetry, command queues, firmware metadata, and global settings (e.g., telemetry interval) for each cabin.
- **CI/CD integration** publishes signed firmware binaries after human approval; n8n notifies devices of new builds.

## Data Flows
1. **Telemetry Upload** – ESP32 sends JSON payload (temps, tank levels, voltages, relay states) to `/cabin/telemetry`. Workflow validates and writes to `telemetry` table, updates device heartbeat, and evaluates automation rules.
2. **Command Fetch** – ESP32 immediately requests `/cabin/commands` to pull queued actions (e.g., start generator, toggle heater). Commands include idempotent identifiers for auditing.
3. **Execution/Ack (future)** – Device applies commands, reports success/failure via `/cabin/commands/ack` for traceability.
4. **Firmware Check (future)** – Device queries `/cabin/firmware/check` to determine whether an OTA update should be downloaded.

## Safety Considerations
- Latching relays with auxiliary contacts provide verification of generator state.
- Local LCD keypad (future) enables on-site overrides and safe-mode operation if cloud connectivity is lost.
- Automation rules will require human approval for high-impact actions such as generator starts until end-to-end testing is complete.
