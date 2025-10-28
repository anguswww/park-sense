#!/usr/bin/env python3

import os
import asyncio
import json
import random
import math
from dotenv import load_dotenv
from gmqtt import Client as MQTTClient
import time

load_dotenv()

# --- Config from .env ---
BROKER = os.getenv("MQTT_BROKER", "mosquitto")
PORT = int(os.getenv("MQTT_PORT", 1883))
USERNAME = os.getenv("MQTT_USERNAME", "")
PASSWORD = os.getenv("MQTT_PASSWORD", "")
ROWS = "A-D"
SPOTS_PER_ROW = 20
TOPIC_ROOT = "parkSenseUTS/msgOut/occupied"
PUBLISH_INTERVAL = 1.0
SINE_PERIOD = float(os.getenv("SINE_PERIOD_SEC", 60.0))
NOISE = float(os.getenv("NOISE_AMPLITUDE", 0.15))
THRESHOLD = float(os.getenv("OCCUPANCY_THRESHOLD", 0.5))
CLIENT_PREFIX = "mqtt-sim"

def expand_rows(spec):
    if "-" in spec:
        a, b = spec.split("-")
        return [chr(c) for c in range(ord(a), ord(b)+1)]
    return [spec]

def spot_id(row, idx):
    return f"{row}{idx}"

async def simulate_device(row, idx):
    device_id = spot_id(row, idx)
    topic = f"{TOPIC_ROOT}/{device_id}"

    client_id = f"{CLIENT_PREFIX}-{device_id}-{random.randint(0,1_000_000)}"
    client = MQTTClient(client_id)
    if USERNAME:
        client.set_auth_credentials(USERNAME, PASSWORD)

    await client.connect(BROKER, PORT)
    phase_offset = random.uniform(0, 2*math.pi)

    # --- initial publish ---
    t = time.time()
    # probability of being occupied based on sine-wave + noise
    # 0.5*(1+sin(...)) scales sine wave to [0,1], noise is added
    prob = 0.5*(1+math.sin(2*math.pi*(t/SINE_PERIOD)+phase_offset)) + random.uniform(-NOISE, NOISE)
    occupied = int(prob >= THRESHOLD)
    client.publish(topic, json.dumps({"device_id": device_id, "occupied": occupied, "ts": int(time.time())}))
    print(f"[{device_id}] initial occupied={occupied}")

    while True:
        t = time.time()
        # probability of being occupied based on sine-wave + random noise
        prob = 0.5*(1+math.sin(2*math.pi*(t/SINE_PERIOD)+phase_offset)) + random.uniform(-NOISE, NOISE)
        new_occupied = int(prob >= THRESHOLD)
        if new_occupied != occupied:
            occupied = new_occupied
            client.publish(topic, json.dumps({"device_id": device_id, "occupied": occupied, "ts": int(time.time())}))
            print(f"[{device_id}] changed -> occupied={occupied}")
        # sleep with jitter
        await asyncio.sleep(PUBLISH_INTERVAL * random.uniform(0.7,1.3))

async def main():
    devices = [(r, i) for r in expand_rows(ROWS) for i in range(1, SPOTS_PER_ROW+1)]

    # --- batch initial start for all devices ---
    tasks = []
    for row, idx in devices:
        tasks.append(asyncio.create_task(simulate_device(row, idx)))
        await asyncio.sleep(0.05)  # tiny stagger to avoid broker overload

    await asyncio.gather(*tasks)

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("Simulator stopped")
