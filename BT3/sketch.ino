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
String TOPIC_CMD;

const int LED_PIN = 14;
unsigned long lastMillis = 0;

float docNhiet() { return 27.0 + random(0, 50) / 10.0; }
float docAm() { return 55.0 + random(0, 200) / 10.0; }

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Received message on topic: ");
  Serial.print(topic);
  Serial.print(" | Payload: ");
  Serial.println(message);

  if (String(topic) == TOPIC_CMD) {
    if (message == "1" || message.equalsIgnoreCase("on")) {
      digitalWrite(LED_PIN, HIGH);
    } else if (message == "0" || message.equalsIgnoreCase("off")) {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void noiBroker() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT...");
    String clientId = "ESP32-SubClient-" + String(random(0xffff), HEX);
    if (mqtt.connect(clientId.c_str())) {
      Serial.println("connected");
      mqtt.subscribe(TOPIC_CMD.c_str());
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  mqtt.setServer(mqtt_server, 1883);
  mqtt.setCallback(callback);

  TOPIC_TELE = "aet2022/" + String(SV_ID) + "/telemetry";
  TOPIC_CMD  = "aet2022/" + String(SV_ID) + "/cmd/led";
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

    char buffer[128];
    serializeJson(doc, buffer);
    mqtt.publish(TOPIC_TELE.c_str(), buffer);
  }
}
