# Swift Cabin Controller

An ESP32-S3 based remote cabin controller that supervises key off-grid systems such as heating, water, energy storage, and backup generation. This repository currently focuses on documentation and scaffolding for firmware, infrastructure, and hardware planning.

## Repository layout

- `firmware/` – Arduino/PlatformIO firmware skeleton plus technical notes.
- `infra/` – Database schema drafts and n8n workflow notes.
- `docs/` – High-level architecture, hardware inventory, roadmap, and API payload definitions.

The firmware will target a Waveshare ESP32-S3 6-channel relay board paired with auxiliary sensors (DHT, HC-SR04, voltage monitors) and DIN-rail relays for larger loads. Telemetry and command/control will be orchestrated through n8n and Postgres, with devices waking every 5 minutes by default (configured via the new `settings` table). See `docs/api-payloads.md` for the JSON contracts that describe telemetry fields (temperatures, voltages, generator status, etc.) and the corresponding command vocabulary, and `docs/gpio-pinout.md` for the physical wiring map that the firmware now follows.
