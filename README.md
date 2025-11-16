# Swift Cabin Controller

An ESP32-S3 based remote cabin controller that supervises key off-grid systems such as heating, water, energy storage, and backup generation. This repository currently focuses on documentation and scaffolding for firmware, infrastructure, and hardware planning.

## Repository layout

- `firmware/` – Arduino/PlatformIO firmware skeleton plus technical notes.
- `infra/` – Database schema drafts, n8n workflow notes, and AI agent instruction packs.
- `docs/` – High-level architecture, hardware inventory, roadmap, and API payload definitions.

The firmware will target a Waveshare ESP32-S3 6-channel relay board paired with auxiliary sensors (DHT, HC-SR04, voltage monitors) and DIN-rail relays for larger loads. Telemetry and command/control will be orchestrated through n8n and Postgres, with devices waking every 5 minutes by default (configured via the new `settings` table). See `docs/api-payloads.md` for the JSON contracts that describe telemetry fields (temperatures, voltages, generator status, etc.) and the corresponding command vocabulary, and `docs/gpio-pinout.md` for the physical wiring map that the firmware now follows.

## Agentic automation layer

The `infra/n8n/agents/` directory captures the system prompts for the new AI helpers that run inside n8n:

- **Telemetry Interpreter** – Scans the last 20 telemetry readings for temperature and voltage anomalies, queues remedial commands (battery heater, room heaters, load shed), and escalates emergencies via email.
- **Daily Operations Summarizer** – Generates a once-per-day markdown brief that highlights min/max telemetry values, automation actions, and outstanding risks.
- **Command Reliability Auditor** – Monitors the `commands` table for stuck or conflicting items, resubmits intent when needed, and notifies operators when retries exceed policy.

Their accompanying workflow exports live under `infra/n8n/nodes/` so they can be imported directly into n8n and wired to the existing telemetry/command pipelines.
