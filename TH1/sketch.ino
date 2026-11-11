#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "TEN_WIFI_CUA_BAN";
const char* password = "MAT_KHAU_WIFI";

WebServer server(80);

float docNhiet() {
  return 27.0 + random(0, 50) / 10.0;
}

float docAm() {
  return 55.0 + random(0, 200) / 10.0;
}

void handleSensor() {
  JsonDocument doc;
  doc["temp"] = docNhiet();
  doc["hum"] = docAm();
  doc["uptime"] = millis() / 1000;
  doc["id"] = "B25DCCN001";
  doc["rssi"] = WiFi.RSSI();

  String out;
  serializeJson(doc, out);
  
  server.send(200, "application/json", out);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/api/sensor", HTTP_GET, handleSensor);
  server.begin();
}

void loop() {
  server.handleClient();
}
