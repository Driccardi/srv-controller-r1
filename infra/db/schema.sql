-- Draft Postgres schema for Swift Cabin Controller
CREATE TABLE devices (
    id UUID PRIMARY KEY,
    name TEXT NOT NULL,
    hardware_revision TEXT,
    firmware_version TEXT,
    last_seen_at TIMESTAMPTZ,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE telemetry (
    id BIGSERIAL PRIMARY KEY,
    device_id UUID REFERENCES devices(id),
    captured_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    payload JSONB NOT NULL
);

CREATE TABLE commands (
    id UUID PRIMARY KEY,
    device_id UUID REFERENCES devices(id),
    command_type TEXT NOT NULL,
    command_body JSONB NOT NULL,
    queued_at TIMESTAMPTZ NOT NULL DEFAULT now(),
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
