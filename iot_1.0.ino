#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* WIFI_SSID = "NxtWave_Te@m";
const char* WIFI_PASS = "Nxtwave@KKH@2026";

// Webhook URL
const char* WEBHOOK_URL = "https://beerbhadra.app.n8n.cloud/webhook/ESP/Intruder";

// Hardware pins
const int IR_SENSOR_PIN = 14;
const int LED_PIN = 2;

// Timing variables
bool lastSensorState = HIGH;
unsigned long lastDetectionTime = 0;
unsigned long debounceDelay = 1000; // 1 second debounce

void setup() {
  Serial.begin(115200);

  // Setup pins
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Connect to WiFi
  connectToWiFi();

  Serial.println("Intruder Detection System Ready!");

  // Read initial sensor state
  lastSensorState = digitalRead(IR_SENSOR_PIN);
}

void loop() {
  // Read sensor
  bool currentSensorState = digitalRead(IR_SENSOR_PIN);

  // Check if obstacle detected (HIGH to LOW change)
  if (currentSensorState == LOW && lastSensorState == HIGH) {

    // Check if enough time has passed since last detection
    if (millis() - lastDetectionTime > debounceDelay) {

      Serial.println("Motion detected!");
      digitalWrite(LED_PIN, HIGH); // Turn on LED

      // Send alert
      if (sendAlert()) {
        Serial.println("✅ Alert sent successfully");
      } else {
        Serial.println("⚠️ Failed to send alert");
      }

      lastDetectionTime = millis();
      digitalWrite(LED_PIN, LOW); // Turn off LED
    }
  }

  lastSensorState = currentSensorState;
  delay(100);
}

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());
}

bool sendAlert() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  HTTPClient http;
  http.begin(WEBHOOK_URL);
  http.addHeader("Content-Type", "application/json");

  // Create JSON payload
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["message"] = "🚨 Intruder detected! 🚨";
  jsonDoc["device_id"] = "ESP32_Sensor";
  jsonDoc["location"] = "NIAT_ROBOTICS_LAB";

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  int httpResponseCode = http.POST(jsonString);
  http.end();

  return (httpResponseCode == 200 || httpResponseCode == 201);
}
