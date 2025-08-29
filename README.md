# Predictive Maintenance System for Industrial Motor (PLC + SCADA + Edge IoT)

**Brief:**  
This project implements a predictive maintenance (PdM) solution for an industrial motor. Local motor control and protection is handled by a PLC (ladder logic). An ESP32 edge node collects vibration (simulated), temperature, and motor status and publishes telemetry via MQTT. A SCADA HMI subscribes to telemetry tags, provides real-time visualization, alarms, and trend logging for early fault detection.

**Key features**
- PLC ladder logic for motor start/stop, overload interlock, and emergency stop.
- SCADA HMI tags & screen design (mock JSON) for visualization, alarms, and manual override.
- Edge IoT node (ESP32) publishing temperature & vibration telemetry over MQTT.
- Simple SCADA server script to receive telemetry, log CSV, and generate trend plots.
- Offline-capable: PLC handles safety-critical actions; edge/SCADA provide analytics.

**Tech stack**
- PLC simulation / ladder logic (text)
- SCADA (HMI design – screenshots / JSON)
- ESP32 (Arduino) for edge sensor simulation
- MQTT broker (e.g., Mosquitto) for telemetry
- Python (paho-mqtt, pandas, matplotlib) for server logging & trend plotting

**How to run (quick)**
1. Start MQTT broker (Mosquitto).
2. Run `server/scada_server.py` to collect telemetry and log data.
3. Flash `edge/esp32_mqtt_motor_monitor.ino` to ESP32 (or run simulated Python publisher).
4. Import HMI tag JSON into your SCADA tool or show the JSON + screenshots in the repo for evaluation.

**Deliverables**
- Ladder logic (plc/ladder_logic.txt)
- Edge code (edge/esp32_mqtt_motor_monitor.ino)
- SCADA tag & screen JSON (scada/*.json)
- Server script to log telemetry (server/scada_server.py)
- Report & presentation (docs/)
