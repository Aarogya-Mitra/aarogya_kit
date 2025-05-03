#include <MAX30100_PulseOximeter.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// === CONFIGURATION ===
#define ONE_WIRE_BUS 5                   // DS18B20 data pin
#define REPORTING_PERIOD_MS 1000        // Report every 1 second

const char* ssid = ".....";
const char* password = ".....";

// === SENSOR OBJECTS ===
PulseOximeter pox;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
WebServer server(80);

// === DATA VARIABLES ===
float BPM = 0.0;
float SpO2 = 0.0;
float bodyTemperature = 0.0;
uint32_t tsLastReport = 0;
bool isPoxInitialized = false;
unsigned long lastRetryTime = 0;
const unsigned long retryInterval = 5000;  // Retry every 5 seconds

// === CALLBACK ===
void onBeatDetected() {
  Serial.println("Beat detected!");
}

// === HTML JSON ENDPOINT ===
void handleSensorData() {
  String json = "{";
  json += "\"temperature\":" + String(bodyTemperature, 2) + ",";
  json += "\"bpm\":" + String(BPM, 2) + ",";
  json += "\"spo2\":" + String(SpO2, 2);
  json += "}";
  server.send(200, "application/json", json);
}

// === SETUP ===
void setup() {
  Serial.begin(115200);

  // WiFi Setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Web Server setup
  server.on("/data", handleSensorData);
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });
  server.begin();
  Serial.println("HTTP server started");

  // Initialize DS18B20
  sensors.begin();

  // Try initializing MAX30100
  Serial.print("Initializing MAX30100... ");
  isPoxInitialized = pox.begin();
  if (isPoxInitialized) {
    Serial.println("SUCCESS");
    pox.setOnBeatDetectedCallback(onBeatDetected);
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  } else {
    Serial.println("FAILED. Will retry in loop.");
  }
}

// === LOOP ===
void loop() {
  // Retry sensor init if not initialized
  if (!isPoxInitialized && millis() - lastRetryTime > retryInterval) {
    Serial.print("Retrying MAX30100 initialization... ");
    isPoxInitialized = pox.begin();
    if (isPoxInitialized) {
      Serial.println("SUCCESS");
      pox.setOnBeatDetectedCallback(onBeatDetected);
      pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    } else {
      Serial.println("FAILED");
    }
    lastRetryTime = millis();
  }

  // Update and read only if initialized
  if (isPoxInitialized) {
    pox.update();

    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      sensors.requestTemperatures();               // Blocking read
      bodyTemperature = sensors.getTempFByIndex(0);

      BPM = pox.getHeartRate();
      SpO2 = pox.getSpO2();

      Serial.printf("BPM: %.2f | SpO2: %.2f%% | Temp: %.2fÂ°F\n", BPM, SpO2, bodyTemperature);
      Serial.println("*********************************");

      tsLastReport = millis();
    }
  } else {
    Serial.println("MAX30100 not initialized. Skipping sensor read.");
  }

  server.handleClient();  // Serve web requests
}
