-- Enable required extensions
CREATE EXTENSION IF NOT EXISTS timescaledb;
CREATE EXTENSION IF NOT EXISTS influxdb;

-- Create the food_levels table
CREATE TABLE IF NOT EXISTS food_levels (
    time        TIMESTAMPTZ       NOT NULL,
    food_level  DOUBLE PRECISION  NOT NULL,
    light_value INTEGER          NOT NULL
);

-- Convert the table to a hypertable
SELECT create_hypertable('food_levels', 'time');

-- Create indexes for better query performance
CREATE INDEX IF NOT EXISTS idx_food_levels_time ON food_levels (time DESC);
CREATE INDEX IF NOT EXISTS idx_food_levels_food_level ON food_levels (food_level);

-- Enable InfluxDB line protocol
SELECT influxdb_enable_line_protocol(); 