# scada_server.py
# Simple MQTT subscriber to log telemetry to CSV and plot basic trends.
import json
import csv
import time
from datetime import datetime
import os
import threading

import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

BROKER = "BROKER_IP"  # set this
PORT = 1883
TOPIC = "plant/motor/+/telemetry"
CSV_FILE = "motor_telemetry.csv"

lock = threading.Lock()

def ensure_csv():
    if not os.path.exists(CSV_FILE):
        with open(CSV_FILE, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['timestamp','motor_id','temp','vibration','status'])

def on_connect(client, userdata, flags, rc):
    print("Connected with result code", rc)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    payload = msg.payload.decode()
    try:
        data = json.loads(payload)
        ts = datetime.now().isoformat()
        motor = data.get('motor_id','M1')
        temp = data.get('temp',0.0)
        vib = data.get('vibration',0.0)
        status = data.get('status',0)
        with lock:
            with open(CSV_FILE,'a',newline='') as f:
                writer = csv.writer(f)
                writer.writerow([ts,motor,temp,vib,status])
        print(f"[{ts}] {motor} T={temp} V={vib}")
    except Exception as e:
        print("JSON parse error:", e, payload)

def plot_trends():
    while True:
        time.sleep(30)  # update every 30s
        try:
            with lock:
                df = pd.read_csv(CSV_FILE, parse_dates=['timestamp'])
            if df.empty:
                continue
            df['timestamp'] = pd.to_datetime(df['timestamp'])
            last = df.set_index('timestamp').last('10min')
            if last.empty:
                continue
            plt.figure(figsize=(8,3))
            plt.plot(last.index, last['temp'], label='Temp (°C)')
            plt.plot(last.index, last['vibration'], label='Vibration (g)')
            plt.legend()
            plt.tight_layout()
            plt.savefig('latest_trend.png')
            plt.close()
            print("Saved latest_trend.png")
        except Exception as e:
            print("Plot error", e)

def main():
    ensure_csv()
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKER, PORT, 60)
    t = threading.Thread(target=plot_trends, daemon=True)
    t.start()
    client.loop_forever()

if __name__ == "__main__":
    main()
