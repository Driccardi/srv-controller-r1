# Hardware Inventory

## Core Controller
- **ESP32-S3 Waveshare 6-channel relay board** – primary MCU with integrated USB-C, 6 relays, and expansion headers.
- **DIN-rail latching relays** – interface the ESP32 outputs to high-current loads such as heaters, generator switches, and pumps.
- **Omron G3MB SSRs** – solid-state relays for small AC loads (lighting, signal lamps).
- **4-channel relay board** – development accessory for bench testing smaller loads.

## Sensors
| System | Sensor | Notes |
| ------ | ------ | ----- |
| Cabin climate | DHT11/22 | Provide ambient temperature + humidity for heater automation. |
| Cistern level | HC-SR04 ultrasonic | Mounted at top of tank, requires weather-proofing and stable mounting. |
| Battery/panel voltage | Voltage divider / sensor module | Measures 48 V battery bank via scaled input and isolation. |
| Battery telemetry | RS485 → UART (future) | Interfaces with JKBMS-compatible BMS for SOC, alarms, and current data. |

## Controlled Subsystems
- **48 V battery bank (JKBMS via RS485):** monitor voltages, SOC, and alarms. Future plan to gate charging/discharging relays based on thresholds.
- **Generator (2-wire start):** DIN-rail latching relays emulate manual toggle switch to start/stop backup generator. Auxiliary contacts confirm state.
- **Heating systems (propane + electric heaters):** temperature thresholds drive relays for heaters and circulation pumps. Include fail-safe timers.
- **Cistern water system:**
  - Primary well pump fills cistern when level drops below minimum and power budget allows.
  - Backup pump engages if primary fails or for winterization flushes.
  - Solenoid valves manage fill/usage isolation.
- **Solar chargeverter (AC output):** not directly controlled by ESP32 but monitored via voltage sensing to infer generation status.

## Optional Local UI
- **DFROBOT I²C LCD keypad shield** – attaches via I²C, provides 16x2 LCD and five-button keypad for local overrides, menu navigation, and safe-mode operations.
