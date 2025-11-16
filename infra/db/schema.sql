-- Draft Postgres schema for Swift Cabin Controller
CREATE TABLE devices (
    id UUID PRIMARY KEY,
    name TEXT NOT NULL,
    device_type TEXT NOT NULL DEFAULT 'esp32_s3',
    hardware_revision TEXT,
    firmware_version TEXT,
    last_seen_at TIMESTAMPTZ,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE telemetry (
    id BIGSERIAL PRIMARY KEY,
    device_id UUID REFERENCES devices(id),
    captured_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    firmware_version TEXT,
    system_status TEXT,
    last_command_id UUID,
    last_command_payload JSONB,
    relay_states JSONB,
    payload JSONB NOT NULL
);

CREATE TABLE device_metrics (
    id UUID PRIMARY KEY,
    device_id UUID REFERENCES devices(id),
    metric_key TEXT NOT NULL,
    metric_type TEXT NOT NULL,
    unit TEXT,
    alarm_low NUMERIC,
    alarm_high NUMERIC,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    UNIQUE(device_id, metric_key)
);

CREATE TABLE telemetry_measurements (
    id BIGSERIAL PRIMARY KEY,
    telemetry_id BIGINT REFERENCES telemetry(id) ON DELETE CASCADE,
    device_metric_id UUID REFERENCES device_metrics(id),
    device_id UUID REFERENCES devices(id),
    captured_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    value_numeric NUMERIC,
    value_text TEXT,
    status TEXT,
    CHECK (value_numeric IS NOT NULL OR value_text IS NOT NULL)
);

CREATE TABLE commands (
    id UUID PRIMARY KEY,
    device_id UUID REFERENCES devices(id),
    command_type TEXT NOT NULL,
    command_body JSONB NOT NULL,
    queued_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    priority SMALLINT NOT NULL DEFAULT 0,
    expires_at TIMESTAMPTZ,
    redundancy_key TEXT,
    requires_ack BOOLEAN NOT NULL DEFAULT TRUE,
    executed BOOLEAN NOT NULL DEFAULT FALSE
);

CREATE TABLE command_executions (
    id BIGSERIAL PRIMARY KEY,
    command_id UUID REFERENCES commands(id),
    device_id UUID REFERENCES devices(id),
    status TEXT NOT NULL,
    details JSONB,
    reported_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE firmware_versions (
    version TEXT PRIMARY KEY,
    build_url TEXT NOT NULL,
    released_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    checksum TEXT
);

CREATE TABLE settings (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL,
    description TEXT,
    updated_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

-- Example seed value:
-- INSERT INTO settings (key, value, description)
-- VALUES ('telemetry_interval_minutes', '5', 'Default wake interval for cabin devices');
