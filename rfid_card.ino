#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SS_PIN     5
#define RST_PIN    27
#define BUZZER_PIN 4
#define LED_PIN    2      // ESP32 onboard LED on most 30-pin boards

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define SCREEN_ADDRESS 0x3C   // try 0x3D if the screen stays blank

MFRC522 rfid(SS_PIN, RST_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ============ SAME FOR EVERYONE - set once, share with the class ============
const char* WIFI_SSID     = "NxtWave_Te@m";
const char* WIFI_PASSWORD = "Nxtwave@KKH@2026";

// Paste your n8n Webhook node's "Production URL" here - no separate
// server.py needed anymore, the ESP32 talks to n8n directly.
const char* N8N_WEBHOOK_URL = "https://beerbhadra.app.n8n.cloud/webhook/ESP/RFID";
// ==============================================================================

// ============ EDIT ME - each student fills in their OWN details ============
String MY_NAME     = "Nxtwave Attendance System";
String MY_CARD_UID = "2C1DF506";  // tap your card in Phase 1 to discover this
// ==============================================================================

bool isClockedIn = false;
unsigned long lastWifiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 5000;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  SPI.begin();
  rfid.PCD_Init();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Wire.begin(21, 22);  // SDA, SCL
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED not found - check wiring/address.");
  }
  showMessage("Starting...", "");

  connectToWiFi();

  Serial.println("Ready.");
  if (MY_CARD_UID == "UNKNOWN") {
    Serial.println("MY_CARD_UID not set yet - tap your card to discover it.");
    showMessage("Tap card to", "discover UID");
  } else {
    showIdleScreen();
  }
}

void loop() {
  String uid;
  if (readRFID(uid)) {

    if (MY_CARD_UID == "UNKNOWN") {
      Serial.print("Detected UID: "); Serial.println(uid);
      Serial.println("Copy this into MY_CARD_UID, then re-upload.");
      showMessage("UID found:", uid);
      feedback(3);
      delay(2000);
      showMessage("Tap card to", "discover UID");
      return;
    }

    if (uid != MY_CARD_UID) {
      Serial.println("This isn't the registered card for this device - ignoring.");
      showMessage("Wrong card!", "Not registered");
      feedback(3);
      delay(1500);
      showIdleScreen();
      return;
    }

    handleScan();
  }

  if (millis() - lastWifiCheck > WIFI_CHECK_INTERVAL) {
    lastWifiCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      showMessage("WiFi lost...", "Reconnecting");
      connectToWiFi();
      showIdleScreen();
    }
  }
}

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  showMessage("Connecting to", "WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected. IP: "); Serial.println(WiFi.localIP());
    showMessage("WiFi Connected", WiFi.localIP().toString());
    delay(1000);
  } else {
    Serial.println("Wi-Fi connect failed, will retry in background.");
    showMessage("WiFi Failed", "Retrying later");
    delay(1000);
  }
}

bool readRFID(String &uid) {
  if (!rfid.PICC_IsNewCardPresent()) return false;
  if (!rfid.PICC_ReadCardSerial())   return false;

  uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  return true;
}

void handleScan() {
  String eventType = isClockedIn ? "CLOCK_OUT" : "CLOCK_IN";
  isClockedIn = !isClockedIn;

  Serial.print("Name: "); Serial.println(MY_NAME);
  Serial.print("Event: "); Serial.println(eventType);

  if (eventType == "CLOCK_IN") {
    feedback(1);
    showMessage(MY_NAME, "CLOCKED IN");
  } else {
    feedback(2);
    showMessage(MY_NAME, "CLOCKED OUT");
  }

  bool sent = sendEvent(eventType);
  delay(1200);
  showStatusResult(sent);
  delay(1000);
  showIdleScreen();
}

// Sends the event straight to n8n. n8n handles: logging (for the
// dashboard), Telegram, and email - all in one workflow.
bool sendEvent(String eventType) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No Wi-Fi - event NOT sent.");
    return false;
  }

  StaticJsonDocument<256> doc;
  doc["uid"]        = MY_CARD_UID;
  doc["name"]       = MY_NAME;
  doc["event_type"] = eventType;

  String payload;
  serializeJson(doc, payload);

  HTTPClient http;
  http.begin(N8N_WEBHOOK_URL);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(payload);

  bool success = (httpCode > 0 && httpCode < 300);

  if (httpCode > 0) {
    Serial.print("n8n responded: "); Serial.println(httpCode);
  } else {
    Serial.print("POST failed: "); Serial.println(http.errorToString(httpCode));
  }
  http.end();
  return success;
}

// ---------------- OLED helper functions ----------------

// Two-line generic message
void showMessage(String line1, String line2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println(line1);
  display.setCursor(0, 30);
  display.println(line2);
  display.display();
}

// Idle screen shown between scans
void showIdleScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(MY_NAME);

  display.setCursor(0, 16);
  display.println(isClockedIn ? "Status: IN" : "Status: OUT");

  display.setCursor(0, 32);
  display.println(WiFi.status() == WL_CONNECTED ? "WiFi: Connected" : "WiFi: Disconnected");

  display.setCursor(0, 50);
  display.println("Tap card to scan");

  display.display();
}

// Shown right after trying to send an event
void showStatusResult(bool sent) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(sent ? "Sent to n8n" : "Send FAILED");
  display.setCursor(0, 36);
  display.println(sent ? "OK" : "Check WiFi/n8n");
  display.display();
}

// ---------------- Buzzer + LED feedback (synced) ----------------
// Plays `count` beeps and blinks the LED `count` times, together.
// 1 = clock in, 2 = clock out, 3 = wrong card / discovery mode.
void feedback(int count) {
  for (int i = 0; i < count; i++) {
    tone(BUZZER_PIN, 1000);
    digitalWrite(LED_PIN, HIGH);
    delay(180);
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, LOW);
    delay(150);
  }
}