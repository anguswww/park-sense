# 📁 deployment/

This directory contains the production deployment configuration for the Park-Sense parking occupancy monitoring system.

## Overview

The deployment uses Docker Compose to orchestrate multiple services that collect, store, and process parking sensor data in real-time.

## Architecture

- **ESP32 Simulator** (`esp32-simulator/`): Simulates IoT parking sensors that publish occupancy data via MQTT
- **Mosquitto** (`mosquitto/`): MQTT broker that receives sensor data from ESP32 devices
- **Telegraf** (`telegraf/`): Collects MQTT messages and writes them to InfluxDB
- **Docker Compose** (`docker-compose.yaml`): Orchestrates all services

## Requirements

- Docker
- Docker Compose

## Configuration

Before deploying, you'll need to configure environment variables and service settings.

### Environment Variables

1. Copy the example environment file:
```bash
cp .example.env .env
```

2. Edit `.env` and update the following values:

**InfluxDB Configuration:**
- `INFLUX_TOKEN`: Generate a token from your InfluxDB instance (Settings → API Tokens)
- `INFLUX_USERNAME` / `INFLUX_PASSWORD`: Set secure credentials for InfluxDB admin
- `INFLUX_ORG`: Organization name (default: ParkSenseOrg)
- `INFLUX_BUCKET`: Bucket name for storing data (default: ParkSenseBucket)
- `INFLUX_RETENTION`: Data retention period (default: 168h = 7 days)

**MQTT Configuration:**
- `MQTT_USERNAME` / `MQTT_PASSWORD`: Set credentials for MQTT broker access. These are not used in the current configuration.


### Docker Compose

Review and modify `docker-compose.yaml` if needed:
- Update volume mounts for persistent storage
- Adjust port mappings if there are conflicts
- Configure network settings for your infrastructure
- Update service resource limits based on your hardware

## Quick Start
```bash
# Configure environment
cp .example.env .env
nano .env  # Update with your values

# Start all services
docker-compose up -d

# View logs
docker-compose logs -f

# Stop services
docker-compose down
```

## Data Flow

1. ESP32 sensors (or simulator) publish occupancy events to MQTT topics
2. Mosquitto broker receives and distributes messages
3. Telegraf subscribes to MQTT topics and transforms data
4. Data is written to InfluxDB for storage and querying
5. Grafana (or other tools) query InfluxDB for visualization

## Configuration Files

- `mosquitto/mosquitto.conf`: MQTT broker settings
- `telegraf/telegraf.conf`: Data collection and transformation rules
- `esp32-simulator/`: Sensor simulation service
- `.example.env`: Template for environment variables