#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "TEN_WIFI_CUA_BAN";
const char* password = "MAT_KHAU_WIFI";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient mqtt(espClient);

const char* SV_ID = "nhom-01";
String TOPIC_TELE;

unsigned long lastMillis = 0;

float docNhiet() { return 27.0 + random(0, 50) / 10.0; }
float docAm() { return 55.0 + random(0, 200) / 10.0; }

void noiBroker() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT Broker...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (mqtt.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  mqtt.setServer(mqtt_server, 1883);
  TOPIC_TELE = "aet2022/" + String(SV_ID) + "/telemetry";
}

void loop() {
  if (!mqtt.connected()) {
    noiBroker();
  }
  mqtt.loop();

  if (millis() - lastMillis >= 2000) {
    lastMillis = millis();

    JsonDocument doc;
    doc["temp"] = docNhiet();
    doc["hum"] = docAm();
    doc["uptime"] = millis() / 1000;
    doc["rssi"] = WiFi.RSSI();

    char buffer[128];
    serializeJson(doc, buffer);

    mqtt.publish(TOPIC_TELE.c_str(), buffer);
    Serial.println("Published telemetry to MQTT");
  }
}
