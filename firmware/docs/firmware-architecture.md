# Firmware Architecture Draft

## Runtime Lifecycle
1. Wake from deep sleep (timer or external interrupt). The default timer is 5 minutes, matching the `telemetry_interval_minutes` value distributed from the cloud settings service.
2. Initialize sensors (DHT, HC-SR04, voltage ADC) and relay GPIOs.
3. Gather telemetry snapshot and format JSON payload.
4. Connect to Wi-Fi and POST telemetry to `/cabin/telemetry` (HTTPS).
5. Issue GET `/cabin/commands` to fetch pending actions.
6. Execute commands with safety checks; queue acknowledgements (future `/cabin/commands/ack`).
7. Optionally check OTA endpoint for updates.
8. Return to deep sleep with configurable interval.

## Modules (planned)
- `sensors/`
  - `dht_sensor.h/.cpp` – abstract DHT11/22 readings.
  - `ultrasonic_sensor.h/.cpp` – HC-SR04 wrapper with calibration offsets.
  - `voltage_sensor.h/.cpp` – ADC scaling and filtering.
  - Future `rs485_bms.h/.cpp` for JKBMS frames.
- `relays/`
  - `relay_manager.h/.cpp` – maps logical loads (heater, pump, generator) to GPIOs.
  - `latching_driver.h/.cpp` – pulse/direction control for DIN latching relays.
  - `failsafe.h/.cpp` – implements watchdog timers and state verification.
- `net/`
  - `http_client.h/.cpp` – thin wrapper around WiFiClientSecure with retry logic.
  - `telemetry_payload.h/.cpp` – builds JSON and validates schema.
  - `command_client.h/.cpp` – handles GET commands + ack flow.
  - `ota_client.h/.cpp` – handles firmware update negotiation (future).

## Configuration
- Use `config.h` for compile-time defaults (device id, sensor pins, sleep interval). Ship with a 5-minute telemetry cadence that can be overridden by values fetched from the `settings` table via command payloads.
- Persist runtime overrides (Wi-Fi creds, schedule) in NVS for OTA updates without rebuilds.

## Testing Strategy
- Unit test sensor conversions and relay sequencing using PlatformIO Unity tests.
- Hardware-in-the-loop smoke tests for relay actuation and sensor readings before field deployment.
- Simulate cloud responses with mocked HTTP endpoints for regression testing.
