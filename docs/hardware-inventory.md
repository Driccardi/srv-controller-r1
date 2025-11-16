# Hardware Inventory

## Core Controller
- **ESP32-S3 Waveshare 6-channel relay board** – primary MCU with integrated USB-C, 6 relays, and expansion headers. Ships with 16 MB of QSPI flash that the firmware will partition for program storage plus ESP-IDF/Arduino NVS (non-volatile storage) blocks so configuration and last-known relay states survive power loss without any add-on memory.
- **DIN-rail latching relays** – interface the ESP32 outputs to high-current loads such as heaters, generator switches, and pumps.
- **Omron G3MB SSRs** – solid-state relays for small AC loads (lighting, signal lamps).
- **4-channel relay board** – development accessory for bench testing smaller loads.
- **12 V backup battery subsystem** – compact AGM/LiFePO4 pack that powers the ESP32 board and critical sensors during a full
  cabin power failure. It is permanently wired to the controller input rails.
- **DC-DC charger + auxiliary solar panel** – keeps the backup battery topped via an isolated charger fed from a dedicated
  trickle solar panel so the controller can continue reporting even when the main PV array or chargeverter is offline.

## Sensors
| System | Sensor | Notes |
| ------ | ------ | ----- |
| Cabin climate | DHT11/22 | Provide ambient temperature + humidity for heater automation. |
| Cistern level | HC-SR04 ultrasonic | Mounted at top of tank, requires weather-proofing and stable mounting. |
| Battery/panel voltage | Voltage divider / sensor module | Measures 48 V battery bank via scaled input and isolation. Lands on a dedicated ADC GPIO for independent verification of RS485 readings. |
| Battery telemetry | RS485 → UART (future) | Interfaces with JKBMS-compatible BMS for SOC, alarms, and current data. |
| Backup battery monitor | Precision voltage divider | Samples the 12 V reserve pack so firmware can alarm before the lifeline power source is depleted. |

## Controlled Subsystems
- **48 V battery bank (JKBMS via RS485):** monitor voltages, SOC, and alarms. Future plan to gate charging/discharging relays based on thresholds. RS485 remains the authoritative data channel for SOC/state while the analog tap adds redundancy.
- **48 V measurement tap (analog GPIO):** a high-value voltage divider on a spare ADC input supplies a quick sanity reading of the main pack even while RS485 telemetry is offline.
- **Generator (2-wire start):** DIN-rail latching relays emulate manual toggle switch to start/stop backup generator. Auxiliary contacts confirm state.
- **Heating systems (propane + electric heaters):** temperature thresholds drive relays for heaters and circulation pumps. Include fail-safe timers.
- **Cistern water system:**
  - Primary well pump fills cistern when level drops below minimum and power budget allows.
  - Backup pump engages if primary fails or for winterization flushes.
  - Solenoid valves manage fill/usage isolation.
- **Solar chargeverter (AC output):** not directly controlled by ESP32 but monitored via voltage sensing to infer generation status.

## Optional Local UI
- **DFROBOT I²C LCD keypad shield** – attaches via I²C, provides 16x2 LCD and five-button keypad for local overrides, menu navigation, and safe-mode operations.

## Optional Persistence Add-ons
- **I²C FRAM / SD modules (future/optional):** only needed if we later decide to cache high-volume logs or firmware artifacts locally. Normal configuration/state checkpoints live in the onboard flash-backed NVS blocks so the system already rides through full power loss.

## Power and Charging Notes
- **RS485-managed 48 V system:** all authoritative limits and alarms flow through the JKBMS/RS485 link while the ADC voltage tap gives firmware a rapid way to cross-check values or trigger alarms if the bus goes silent.
- **Auxiliary solar trickle + DC-DC charger:** a small panel and charger maintain the 12 V reserve battery, ensuring the ESP32 and radios stay alive long enough to execute recovery scripts even when the main chargeverter is offline.
