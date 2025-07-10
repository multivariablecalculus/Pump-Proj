#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "pump_controller";
const char* password = "password";

ESP8266WebServer server(80);

const int pumpRelayPin = D1;
const int topProbePin = D2;
const int bottomProbePin = D3;

bool PumpAuto = false;
bool PumpState = false;

void setup() {
  // 🛫🏬

  Serial.begin(115200);
  Serial.println("\n\n[BOOT] Booting...");

  pinMode(pumpRelayPin, OUTPUT);
  pinMode(topProbePin, INPUT_PULLUP);
  pinMode(bottomProbePin, INPUT_PULLUP);
  digitalWrite(pumpRelayPin, LOW);
  pinMode(LED_BUILTIN,OUTPUT);

  Serial.print("[WIFI] Connecting to: ");
  Serial.println(ssid);
  WiFi.softAP(ssid, password);

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }

  // Serial.println("\n[WIFI] Connected!");
  // Serial.print("[WIFI] IP address: ");
  // Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handlePumpOn);
  server.on("/off", handlePumpOff);
  server.on("/auto", handleAuto);
  server.on("/man", handleMan);
  server.on("/status", handleStatus);

  server.begin();

}

void loop() {
  server.handleClient();

  if (PumpAuto) {
    bool top = digitalRead(topProbePin);
    bool bottom = digitalRead(bottomProbePin);

    if (top == 0) {
      pumpControl(false);
      digitalWrite(LED_BUILTIN , HIGH );
    } else if (bottom == 1) {
      pumpControl(true);
      digitalWrite(LED_BUILTIN ,LOW);
    }
  }
}

void handleRoot() {
  server.send(200, "text/plain", "Pump Controller Ready");
  Serial.println("****************************** handleRoot ");
}

void handlePumpOn() {
  if (!PumpAuto) {
    pumpControl(true);
    server.send(200, "text/plain", "Pump turned ON manually");
    Serial.println("****************************** handlePumpOn ");
    digitalWrite(LED_BUILTIN,LOW);
  } else {
    server.send(200, "text/plain", "Auto mode active. Switch to manual.");
  }
}

void handlePumpOff() {
  if (!PumpAuto) {
    pumpControl(false);
    server.send(200, "text/plain", "Pump turned OFF manually");
    Serial.println("****************************** handlePumpOff ");
    digitalWrite(LED_BUILTIN,HIGH);


  } else {
    server.send(200, "text/plain", "Auto mode active. Switch to manual.");
  }
}

void handleAuto() {
  PumpAuto = true;
  server.send(200, "text/plain", "Pump Mode changed to AUTO");
  Serial.println("****************************** handleAuto ");

}

void handleMan() {
  PumpAuto = false;
  server.send(200, "text/plain", "Pump Mode changed to MANUAL");
  Serial.println("****************************** handleMan ");

}

void handleStatus() {
  bool top = digitalRead(topProbePin);
  bool bottom = digitalRead(bottomProbePin);

  String status = String(top) + String(bottom);
  Serial.println(status);
  server.send(200, "text/plain", status);
  Serial.println("****************************** handleStatus ");

}

void pumpControl(bool state) {
  PumpState = state;
  digitalWrite(pumpRelayPin, state ? LOW : HIGH);
}
