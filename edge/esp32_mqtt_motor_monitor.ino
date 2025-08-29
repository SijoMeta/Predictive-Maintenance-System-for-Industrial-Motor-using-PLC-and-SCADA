/*
  esp32_mqtt_motor_monitor.ino
  Simulated motor telemetry: temperature & vibration -> publish via MQTT
  Libraries: PubSubClient, WiFi
*/

#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASS";
const char* mqtt_server = "BROKER_IP"; // e.g., 192.168.1.10
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
int publishInterval = 2000; // ms

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32MotorMonitor-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe("motor/control/#"); // listen for remote commands if any
    } else {
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle remote start/stop commands if needed
  // For demo, we won't implement control — only telemetry
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

float simulateTemp() {
  // base 45°C, occasional spikes
  float base = 45.0;
  float noise = (random(-100,100))/100.0;
  if (random(0,1000) > 990) noise += random(50,120)/10.0; // random spike
  return base + noise;
}

float simulateVibration() {
  // base 0.02 g, increases during fault
  float base = 0.02;
  float noise = random(0,50)/10000.0;
  if (random(0,1000) > 995) noise += random(20,80)/1000.0; // spike
  return base + noise;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > publishInterval) {
    lastMsg = now;

    float temp = simulateTemp();
    float vib = simulateVibration();
    int motorStatus = 1; // 1 = running (for demo)

    char payload[200];
    snprintf(payload, sizeof(payload),
      "{\"motor_id\":\"M1\",\"timestamp\":%lu,\"temp\":%.2f,\"vibration\":%.4f,\"status\":%d}",
      millis(), temp, vib, motorStatus
    );

    client.publish("plant/motor/M1/telemetry", payload);

    Serial.println(payload);
  }
}
