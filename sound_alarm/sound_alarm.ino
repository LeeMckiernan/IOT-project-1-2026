// Sound Sensor + Buzzer Alarm
// Arduino UNO R4 WiFi + Firestore Logging
#include <WiFiS3.h>
#include <ArduinoJson.h>
#include "rgb_lcd.h" //Grove LCD library

rgb_lcd lcd; //Create LCD object

// ── Config ────────────────────────────────────
const char* WIFI_SSID            = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD        = "YOUR_WIFI_PASSWORD";
const char* FIRESTORE_HOST       = "firestore.googleapis.com";
const char* FIRESTORE_PROJECT_ID = "YOUR_FIREBASE_PROJECT_ID";
const char* FIRESTORE_COLLECTION = "alarm_events";

// ── Pins & Threshold ──────────────────────────
const int soundPin   = A0;   // Sound sensor
const int buzzerPin  = 4;    // Buzzer on D4
// Thresholds (adjust after testing)
int quietLevel = 200;
int mediumLevel = 400;
int loudLevel = 600;

// ── Globals ───────────────────────────────────
int soundValue   = 0;
int eventCounter = 0;

WiFiSSLClient wifiClient;

// ── Helpers ───────────────────────────────────
float adcToDb(int adcValue) {
  if (adcValue <= 0) return 30.0;
  float ratio = (float)adcValue / 1023.0;
  return 30.0 + (60.0 * log10(1.0 + ratio * 9.0) / log10(10.0));
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Connected!");
  } else {
    Serial.println(" Failed. Running in offline mode.");
  }
}

void logToFirestore(float peakDb) {
  if (WiFi.status() != WL_CONNECTED) return;

  eventCounter++;

  StaticJsonDocument<256> doc;
  JsonObject fields = doc.createNestedObject("fields");

  fields["event_id"]["integerValue"] = String(eventCounter);
  fields["timestamp"]["stringValue"] = "uptime+" + String(millis() / 1000) + "s";
  fields["peak_db"]["doubleValue"]   = peakDb;

  char body[256];
  serializeJson(doc, body);

  String path = "/v1/projects/";
  path += FIRESTORE_PROJECT_ID;
  path += "/databases/(default)/documents/";
  path += FIRESTORE_COLLECTION;

  if (!wifiClient.connect(FIRESTORE_HOST, 443)) {
    Serial.println("Firestore connection failed.");
    return;
  }

  wifiClient.print("POST "); wifiClient.print(path); wifiClient.println(" HTTP/1.1");
  wifiClient.print("Host: "); wifiClient.println(FIRESTORE_HOST);
  wifiClient.println("Content-Type: application/json");
  wifiClient.println("Connection: close");
  wifiClient.print("Content-Length: "); wifiClient.println(strlen(body));
  wifiClient.println();
  wifiClient.print(body);

  unsigned long timeout = millis();
  while (wifiClient.available() == 0 && millis() - timeout < 5000);
  if (wifiClient.available()) {
    String status = wifiClient.readStringUntil('\n');
    Serial.println("Firestore: " + status);
  }
  wifiClient.stop();
}

// ── Setup ─────────────────────────────────────
void setup() {
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Sound Sensor Ready...");
  connectWiFi();

  //---------------------------- LCD setup ---------------------------
  lcd.begin(16, 2); // 16 characters and 2 rows
  lcd.setRGB(0,255,0); //Green backlight on startup

  //Startup message
  lcd.print("Smart Sound");
  lcd.setCursor(0, 1);
  lcd.print("Starting.......");
  delay(2000);


  //clear dispaly before the main loop
  lcd.clear();
}

// ── Loop ──────────────────────────────────────
void loop() {
  soundValue = analogRead(soundPin);

  Serial.print("Sound Level: ");
  Serial.println(soundValue);

  if (soundValue < quietLevel) {
    lcd.print("Status: Quiet   ");

    setColor(0, 255, 0);
    
    float peakDb = adcToDb(soundValue);
    logToFirestore(peakDb);
  }
  else if (soundValue < mediumLevel) {
    lcd.print("Status: Medium  ");

    float peakDb = adcToDb(soundValue);
    logToFirestore(peakDb);
  }
  else if (soundValue < loudLevel) {
    lcd.print("Status: Loud    ");

    float peakDb = adcToDb(soundValue);
    logToFirestore(peakDb);
  }
  else {
    lcd.print("Status: TOO LOUD");

    float peakDb = adcToDb(soundValue);
    logToFirestore(peakDb);
  }

  delay(80);
}
