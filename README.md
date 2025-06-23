# Aquarium IoT TimescaleDB Setup

This repository contains the setup for TimescaleDB to store aquarium IoT data.

## Prerequisites

- Docker and Docker Compose installed
- Basic knowledge of SQL and database management

## Setup Instructions

1. Start the TimescaleDB container:
   ```bash
   docker-compose up -d
   ```

2. Wait for the container to start (about 30 seconds)

3. Run the setup script:
   ```bash
   docker exec -i timescaledb psql -U aquarium -d aquarium < setup.sql
   ```

## Configuration

The default configuration is:
- Database name: aquarium
- Username: aquarium
- Password: aquarium123
- PostgreSQL port: 5432
- InfluxDB line protocol port: 8086

## Updating Arduino Code

Update these values in your Arduino sketch:
```cpp
const char* serverUrl = "http://YOUR_SERVER_IP:8086";
const char* dbName = "aquarium";
const char* measurement = "food_level";
```

Replace `YOUR_SERVER_IP` with your computer's IP address.

## Querying Data

You can query the data using standard SQL:

```sql
-- Get latest food level
SELECT * FROM food_levels ORDER BY time DESC LIMIT 1;

-- Get average food level for the last hour
SELECT AVG(food_level) 
FROM food_levels 
WHERE time > NOW() - INTERVAL '1 hour';

-- Get light levels above threshold
SELECT * 
FROM food_levels 
WHERE light_value > 2000 
ORDER BY time DESC;
```

## Backup and Restore

To backup the database:
```bash
docker exec timescaledb pg_dump -U aquarium aquarium > backup.sql
```

To restore from backup:
```bash
docker exec -i timescaledb psql -U aquarium -d aquarium < backup.sql
``` 