# 🅿️ ParkSense - IoT Parking Occupancy System
## ⚠️ Work In Progress ⚠️

**ParkSense** is an IoT-based parking management system that detects whether parking spots are occupied and reports that data to a cloud database in real time.  
Each parking sensor (real or simulated) publishes data via **MQTT**, which is then collected by a backend service and stored in **InfluxDB** for visualisation in tools like **Grafana**.

## 📁 Project Structure

```
.
├── firmware/        # ESP32 firmware (PlatformIO)
├── simulator/       # Python script that simulates multiple sensors
└── server/          # MQTT → InfluxDB collector service
```

### **firmware/**
Contains the embedded C++ code for ESP32 microcontrollers equipped with ultrasonic or IR sensors. Each device connects to WiFi, reads parking spot occupancy data, and publishes status updates to the MQTT broker. Built using PlatformIO for easy dependency management and deployment to physical hardware.

#### Setup & Installation:
Install PlatformIO (as a VSCode extension or CLI).
Open the firmware/ folder in PlatformIO.
Edit secrets.h to include your WiFi and MQTT credentials:

```cpp
const char* WIFI_SSID = "SSID";
const char* WIFI_PASS = "Password";
const char* MQTT_USER = "user";
const char* MQTT_PASS = "pass";
```
Connect your ESP32 board via USB.
Build and upload the firmware using PlatformIO.
Open the Serial Monitor to observe occupancy messages being published.

### **simulator/**
A Python-based virtual sensor environment that mimics multiple ESP32 devices for testing without physical hardware. Generates realistic parking occupancy events and publishes them to MQTT using the same message format as real sensors. Useful for development, load testing, and demonstration purposes.

#### Setup & Run:
Create a Python virtual environment:
```bash
python -m venv .venv
source .venv/bin/activate    # Windows: .venv\Scripts\activate
```

Install dependencies:
```bash
pip install -r requirements.txt
# or if using pyproject.toml with uv:
uv sync
```

Create a .env file in simulator/ with your broker details:
```bash
MQTT_BROKER=broker.emqx.io
MQTT_PORT=1883
MQTT_USERNAME=
MQTT_PASSWORD=
TOPIC_ROOT=parkSenseUTS/msgOut/occupied
ROWS=A-D
SPOTS_PER_ROW=20
PUBLISH_INTERVAL_SEC=1
SINE_PERIOD_SEC=60
NOISE_AMPLITUDE=0.15
OCCUPANCY_THRESHOLD=0.5
CLIENT_PREFIX=mqtt-sim
```

Run the simulator:
```bash
uv run main.py       # if using uv
# or
python main.py
```

### **server/**
The backend data collector service written in Python. Subscribes to MQTT topics, processes incoming sensor messages, and writes time-series data to InfluxDB. Acts as the bridge between the IoT layer (sensors/MQTT) and the persistence/visualization layer (InfluxDB/Grafana).
