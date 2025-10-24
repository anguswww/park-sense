# 🅿️ ParkSense - IoT Parking Occupancy System
## ⚠️ Work In Progress ⚠️

**ParkSense** is an IoT-based parking management system that detects whether parking spots are occupied and reports that data to a cloud database in real time.  
Each parking sensor (real or simulated) publishes data via **MQTT**, which is then collected by a backend service and stored in **InfluxDB** for visualisation in tools like**Grafana**.

## 📁 Project Structure

```
.
├── firmware/        # ESP32 firmware (PlatformIO)
├── simulator/       # Python script that simulates multiple sensors
└── server/          # MQTT → InfluxDB collector service
```

### **firmware/**
Contains the embedded C++ code for ESP32 microcontrollers equipped with ultrasonic or IR sensors. Each device connects to WiFi, reads parking spot occupancy data, and publishes status updates to the MQTT broker. Built using PlatformIO for easy dependency management and deployment to physical hardware.

### **simulator/**
A Python-based virtual sensor environment that mimics multiple ESP32 devices for testing without physical hardware. Generates realistic parking occupancy events and publishes them to MQTT using the same message format as real sensors. Useful for development, load testing, and demonstration purposes.

### **server/**
The backend data collector service written in Python. Subscribes to MQTT topics, processes incoming sensor messages, and writes time-series data to InfluxDB. Acts as the bridge between the IoT layer (sensors/MQTT) and the persistence/visualization layer (InfluxDB/Grafana).
