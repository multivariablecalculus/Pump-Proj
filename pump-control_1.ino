#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Pump_Controller";
const char* password = "12345678";

ESP8266WebServer server(80);

const int pumpRelayPin = D1;
const int topProbePin = D6;
const int bottomProbePin = D5;

bool PumpAuto = true;
bool PumpState = false;
int lastTop = -1;
int lastBottom = -1;

void setup() {
  delay(500);
  Serial.begin(115200);
  Serial.println("\n\n[BOOT] Booting...");

  pinMode(pumpRelayPin, OUTPUT);
  pinMode(topProbePin, INPUT_PULLUP);
  pinMode(bottomProbePin, INPUT_PULLUP);
  digitalWrite(pumpRelayPin, LOW);

  Serial.print("[AP] Starting Access Point: ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("[AP] IP Address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/on", handlePumpOn);
  server.on("/off", handlePumpOff);
  server.on("/auto", handleAuto);
  server.on("/man", handleMan);
  server.on("/status", handleStatus);

  server.begin();
  Serial.println("[HTTP] Web server started.");
}

void loop() {
  server.handleClient();

  bool top = !digitalRead(topProbePin);
  bool bottom = !digitalRead(bottomProbePin);

  if (top != lastTop || bottom != lastBottom) {
    Serial.print("[STATUS] TOP (D6): ");
    Serial.print(top);
    Serial.print(" | BOTTOM (D5): ");
    Serial.println(bottom);
    lastTop = top;
    lastBottom = bottom;
  }

  if (PumpAuto) {
    if (top && bottom) {
      pumpControl(false);
    } else if (!bottom) {
      pumpControl(true);
    }
  }
}

void handleRoot() {
  server.send(200, "text/plain", "Pump Controller Ready");
}

void handlePumpOn() {
  if (!PumpAuto) {
    pumpControl(true);
    server.send(200, "text/plain", "Pump turned ON manually");
  } else {
    server.send(200, "text/plain", "Auto mode active. Switch to manual.");
  }
}

void handlePumpOff() {
  if (!PumpAuto) {
    pumpControl(false);
    server.send(200, "text/plain", "Pump turned OFF manually");
  } else {
    server.send(200, "text/plain", "Auto mode active. Switch to manual.");
  }
}

void handleAuto() {
  PumpAuto = true;
  server.send(200, "text/plain", "Pump Mode changed to AUTO");
}

void handleMan() {
  PumpAuto = false;
  server.send(200, "text/plain", "Pump Mode changed to MANUAL");
}

void handleStatus() {
  bool top = !digitalRead(topProbePin);
  bool bottom = !digitalRead(bottomProbePin);

  String status = String(top) + String(bottom);
  server.send(200, "text/plain", status);
}

void pumpControl(bool state) {
  PumpState = state;
  digitalWrite(pumpRelayPin, state ? HIGH : LOW);
}
