# API Payloads

This document captures the JSON structures shared between the cabin device and the cloud control plane. The payloads are intentionally verbose so the cloud side can validate safety constraints and n8n can fan data out to dashboards, alerts, and automation rules.

## Telemetry (`POST /cabin/telemetry`)

```json
{
  "device_id": "2f4fb8e5-4c86-4877-9689-5a5c6117d7c0",
  "captured_at": "2024-01-25T03:15:00Z",
  "firmware_version": "0.0.1",
  "system_status": "nominal",
  "uptime_seconds": 112,
  "relay_states": {
    "generator_latch": "off",
    "battery_heater": "on",
    "cistern_fill_pump": "off",
    "lighting_scene": "night"
  },
  "environment": {
    "external_temp_c": -5.2,
    "external_humidity_pct": 55,
    "room_temps_c": {
      "main_room": 18.6,
      "battery_bay": 7.1
    }
  },
  "power": {
    "battery_voltage_v": 52.1,
    "battery_temp_c": 5.4,
    "battery_heater_status": "heating",
    "generator_status": "standby",
    "generator_runtime_minutes": 0,
    "panel_input_v": 0
  },
  "water": {
    "cistern_level_cm": 73,
    "cistern_percent_full": 61,
    "well_pump_status": "idle",
    "backup_pump_status": "idle"
  },
  "bms": {
    "soc_pct": 84,
    "alarms": []
  },
  "last_command_id": "8e5219cd-a90c-4ca8-8d4a-c195c544da30",
  "last_command_payload": {
    "type": "lighting_scene",
    "parameters": {
      "scene": "night"
    }
  },
  "settings": {
    "telemetry_interval_minutes": 5
  }
}
```

### Notes
- `system_status` conveys coarse device health (e.g., `nominal`, `degraded`, `failsafe`).
- `relay_states` enumerates the logical loads the firmware knows about; they map to DIN relays or SSRs.
- `environment`, `power`, and `water` sections aggregate the raw sensors.
- `last_command_payload` is echoed back so the cloud can confirm delivery/intent, even if downstream acks fail.
- Any sensor not available for a wake cycle may be omitted; n8n should default missing nodes to `null` in the DB.

## Commands (`GET /cabin/commands` response)

```json
{
  "device_id": "2f4fb8e5-4c86-4877-9689-5a5c6117d7c0",
  "timestamp": "2024-01-25T03:16:00Z",
  "commands": [
    {
      "id": "a93545ee-09a1-4d1a-b5de-598e0e80a1f7",
      "type": "heater_control",
      "parameters": {
        "zone": "main_room",
        "state": "on",
        "hold_until": "2024-01-25T09:00:00Z"
      },
      "redundancy_key": "heater_control:main_room",
      "priority": 10,
      "expires_at": "2024-01-25T04:30:00Z"
    },
    {
      "id": "8f48b960-93b1-4881-9c73-e2d28570d4c6",
      "type": "generator_window",
      "parameters": {
        "start_voltage_v": 48.0,
        "stop_voltage_v": 53.5,
        "manual_state": "auto"
      },
      "redundancy_key": "generator_window",
      "priority": 50,
      "expires_at": null
    },
    {
      "id": "686397b4-66a0-4800-80df-37aa199bb6f7",
      "type": "water_fill_band",
      "parameters": {
        "fill_min_pct": 45,
        "fill_max_pct": 80,
        "backup_pump": "enabled"
      },
      "redundancy_key": "cistern_band",
      "priority": 20,
      "expires_at": null
    },
    {
      "id": "dd80cbd4-d138-49c6-9b68-8ac2867640be",
      "type": "lighting_scene",
      "parameters": {
        "scene": "emergency",
        "dim_level_pct": 40
      },
      "redundancy_key": "lighting_scene",
      "priority": 5,
      "expires_at": null
    },
    {
      "id": "a32f6d70-7473-4ea8-bfbb-e428a1f5f1fe",
      "type": "solar_actuator",
      "parameters": {
        "axis_mode": "auto",
        "snow_dump": false
      },
      "redundancy_key": "solar_actuator",
      "priority": 5,
      "expires_at": null
    }
  ]
}
```

### Notes
- Commands are idempotent; `redundancy_key` lets the firmware drop duplicates if a previous wake cycle already executed the latest intent.
- `priority` guides execution ordering on-device when multiple actions arrive simultaneously.
- `expires_at` prevents stale automation instructions from executing after the conditions have changed.
- Suggested command vocabulary (extensible):
  - `heater_control` – per-zone heater or circulation pump overrides.
  - `generator_manual` – start/stop the generator immediately.
  - `generator_window` – configure battery voltage thresholds for auto-start/stop.
  - `water_fill_band` – configure cistern fill bands and pump redundancy.
  - `water_flush` – purge lines for winterization.
  - `battery_heater` – force heater on/off or thermostat mode.
  - `lighting_scene` – pick scene (day/night/emergency/low_voltage) and dimming level.
  - `lighting_schedule` – push schedule blocks for scenes.
  - `solar_actuator` – future two-axis tracker configuration or snow-dump cycle.
  - `settings_sync` – send config key/value pairs (e.g., telemetry cadence) when `settings` table updates.

## Command Acknowledgements (`POST /cabin/commands/ack`)

Even though the endpoint is future work, reserve the payload structure now for telemetry correlation:

```json
{
  "device_id": "2f4fb8e5-4c86-4877-9689-5a5c6117d7c0",
  "command_id": "a93545ee-09a1-4d1a-b5de-598e0e80a1f7",
  "status": "succeeded",
  "completed_at": "2024-01-25T03:16:07Z",
  "details": {
    "notes": "heater toggled on",
    "latency_ms": 820
  }
}
```

Telemetry packets should still echo `last_command_id` and `last_command_payload` so the cloud can reconcile even if the acknowledgement payload fails to arrive.
