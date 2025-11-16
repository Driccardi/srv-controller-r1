# GPIO Pinout Reference

The Waveshare ESP32-S3 relay board exposes plenty of spare GPIOs alongside the six onboard relays. The table below captures the
initial assignments for the Cabin Controller bring-up so wiring, firmware constants, and test sketches stay aligned.

| Function | Label | GPIO | Direction | Notes |
| -------- | ----- | ---- | --------- | ----- |
| Room 1 temperature/humidity | ROOM1_DHT | GPIO4 | Digital bidirectional (single-wire) | DHT11 sensor located in the main living area. Requires 4.7 kΩ pull-up to 3.3 V on the data line. |
| Room 2 temperature/humidity | ROOM2_DHT | GPIO5 | Digital bidirectional (single-wire) | DHT11 sensor near the mechanical room to monitor heater performance. Shares the same 3.3 V rail as Room 1. |
| Backup battery voltage sense | BACKUP_VBAT_ADC | GPIO1 (ADC1_CH0) | Analog input | Precision divider (e.g., 56 kΩ / 10 kΩ) scales the 12 V reserve pack to < 3.3 V for the ADC. Must remain on an ADC-capable pin (GPIO1–GPIO20), so it cannot be moved to GPIO39. |
| Main 48 V pack sense (redundant) | MAIN_PACK_ADC | GPIO6 (ADC1_CH5) | Analog input | High-impedance divider (≥220 kΩ top leg) mirrors the cabin battery bank. Analog data supplements RS485 telemetry. |
| RS485 TX | RS485_TXD | GPIO17 | Digital output (UART) | Feeds the isolated RS485 transceiver that talks to the JKBMS. |
| RS485 RX | RS485_RXD | GPIO18 | Digital input (UART) | Returns data from the battery BMS. |
| RS485 DE/RE | RS485_DE | GPIO21 | Digital output | Enables the RS485 driver during transmissions; held low for receive. |
| Relay channels 0-5 | RELAY[0..5] | GPIO33–GPIO38 | Digital outputs | Factory-wired to the onboard relays. Active level is board-dependent; default firmware drives them LOW for off during boot. |
| Future LCD SDA | I2C_SDA | GPIO12 | Digital bidirectional | Shared I²C bus for the DFROBOT keypad/LCD shield or other expansion sensors. |
| Future LCD SCL | I2C_SCL | GPIO11 | Digital bidirectional | Clock line for the local UI bus. |

## Wiring Notes
- The two DHT11 sensors share 3.3 V and ground but must have individual data lines. Keep the cables under ~2 m or use shielded
  cable to minimize errors.
- Both voltage dividers should use high-value resistors to limit current draw. Add a 0.1 µF capacitor from the ADC node to ground
  to tame noise and create a quick RC filter.
- The backup 12 V battery is recharged by a dedicated DC-DC charger tied to an auxiliary solar panel. The charger output feeds the
  ESP32 VIN rail via ideal diodes so the controller seamlessly rides through a main-bus failure.
- RS485 differential pairs should be twisted and shielded, with the transceiver reference tied to the cabin battery negative to
  minimize noise.
- Leave the spare GPIOs (e.g., 7–10, 13–16) accessible for future peripherals such as snow-dump actuators for the planned two-axis
  solar controllers.

## Analog-capable GPIOs

Only GPIO1 through GPIO20 on the ESP32-S3 support ADC readings. GPIO1–GPIO10 map to ADC1 channels 0–9, while GPIO11–GPIO20 map to ADC2 channels 0–9. Pins numbered above 20 (e.g., GPIO33–GPIO39) do **not** offer analog sampling, so voltage dividers and other sensors that require ADC measurements must be wired to one of the lower-numbered GPIOs.
