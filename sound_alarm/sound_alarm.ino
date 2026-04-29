// Sound Alarm — Arduino UNO R4 WiFi
// State machine: IDLE → ALARM → log to Firestore → IDLE
//
// Libraries needed (install via Library Manager):
//   ArduinoJson, Grove LCD RGB Backlight (search "rgb_lcd")
//
// Firestore rules must allow unauthenticated writes to the "events" collection,
// or update the rules to: allow write: if true;  (for the events path)
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "rgb_lcd.h"
#include <time.h>

// ── Config ────────────────────────────────────────────────────────────────────
const char* WIFI_SSID            = "IOT-MPSK";
const char* WIFI_PASSWORD        = "rhgbnvfv";
const char* FIRESTORE_HOST       = "firestore.googleapis.com";
const char* FIRESTORE_PROJECT_ID = "iot-dashboard-b0b46";
const char* FIRESTORE_API_KEY    = "AIzaSyAIdrzuEPa4ELiRhP1meKWEZkjJKVwV1lg";
const char* DEVICE_NAME          = "Arduino-UNO-R4";

// ── Pins ──────────────────────────────────────────────────────────────────────
const int SOUND_PIN  = A0;
const int BUZZER_PIN = 4;

// ── Thresholds ────────────────────────────────────────────────────────────────
const int  ALARM_THRESHOLD   = 500;   // ADC value that triggers alarm
const int  SILENCE_THRESHOLD = 300;   // ADC value considered quiet (hysteresis)
const long ALARM_HOLD_MS     = 2000;  // Stay in alarm at least this long after sound drops

// ── Grove LCD RGB Backlight V5.0 (16×2) ──────────────────────────────────────
rgb_lcd lcd;

// ── State machine ─────────────────────────────────────────────────────────────
enum State { IDLE, ALARM };
State state = IDLE;

// ── Alarm tracking ────────────────────────────────────────────────────────────
unsigned long alarmStartMs = 0;
unsigned long lastLoudMs   = 0;
int           peakAdc      = 0;

// ── NTP time tracking ─────────────────────────────────────────────────────────
unsigned long ntpEpoch  = 0;
unsigned long ntpMillis = 0;
bool          timeSynced = false;

WiFiSSLClient wifiClient;
WiFiUDP       udp;

// ── ADC → dB ─────────────────────────────────────────────────────────────────
float adcToDb(int adc) {
  if (adc <= 0) return 30.0f;
  return 30.0f + 60.0f * log10f(1.0f + (adc / 1023.0f) * 9.0f);
}

// ── LCD draw ──────────────────────────────────────────────────────────────────
// Normal: green backlight. Alarm: red backlight.
void drawLcd(float db, bool alarm) {
  lcd.clear();
  if (alarm) {
    lcd.setRGB(255, 0, 0);
    lcd.setCursor(0, 0);
    lcd.print("!! ALARM !!     ");
    lcd.setCursor(0, 1);
    lcd.print(db, 1);
    lcd.print(" dB         ");
  } else {
    lcd.setRGB(0, 255, 0);
    lcd.setCursor(0, 0);
    lcd.print("Sound Level     ");
    lcd.setCursor(0, 1);
    lcd.print(db, 1);
    lcd.print(" dB         ");
  }
}

// ── NTP sync ─────────────────────────────────────────────────────────────────
void syncNTP() {
  byte packet[48] = {};
  packet[0] = 0b11100011; // LI=0, Version=4, Mode=3 (client)
  packet[2] = 6;
  packet[3] = 0xEC;
  packet[12] = 49; packet[13] = 0x4E; packet[14] = 49; packet[15] = 52;

  IPAddress ntpIP;
  WiFi.hostByName("pool.ntp.org", ntpIP);
  udp.begin(2390);
  udp.beginPacket(ntpIP, 123);
  udp.write(packet, 48);
  udp.endPacket();

  delay(1000);
  if (udp.parsePacket() >= 48) {
    udp.read(packet, 48);
    unsigned long secs = (unsigned long)packet[40] << 24 |
                         (unsigned long)packet[41] << 16 |
                         (unsigned long)packet[42] <<  8 |
                         (unsigned long)packet[43];
    ntpEpoch  = secs - 2208988800UL; // NTP epoch → Unix epoch
    ntpMillis = millis();
    timeSynced = true;
    Serial.println("NTP sync OK");
  } else {
    Serial.println("NTP sync failed — timestamps will be invalid");
  }
  udp.stop();
}

unsigned long nowEpoch() {
  return ntpEpoch + (millis() - ntpMillis) / 1000UL;
}

String epochToRFC3339(unsigned long epoch) {
  time_t t = (time_t)epoch;
  struct tm* gmt = gmtime(&t);
  char buf[25];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02dZ",
    gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday,
    gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
  return String(buf);
}

// ── Firestore log ─────────────────────────────────────────────────────────────
// Writes one document to the "events" collection matching the dashboard schema:
//   time (timestampValue), db (doubleValue), duration (doubleValue), deviceName (stringValue)
void logToFirestore(float peakDb, float durationSec, const String& timestamp) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected — skipping log");
    return;
  }

  StaticJsonDocument<512> doc;
  JsonObject fields = doc.createNestedObject("fields");
  fields["time"]["timestampValue"]    = timestamp;
  fields["db"]["doubleValue"]         = peakDb;
  fields["duration"]["doubleValue"]   = durationSec;
  fields["deviceName"]["stringValue"] = DEVICE_NAME;

  char body[512];
  int bodyLen = serializeJson(doc, body, sizeof(body));

  String path = "/v1/projects/";
  path += FIRESTORE_PROJECT_ID;
  path += "/databases/(default)/documents/events?key=";
  path += FIRESTORE_API_KEY;

  if (!wifiClient.connect(FIRESTORE_HOST, 443)) {
    Serial.println("Firestore connect failed");
    return;
  }

  wifiClient.print("POST "); wifiClient.print(path); wifiClient.println(" HTTP/1.1");
  wifiClient.print("Host: "); wifiClient.println(FIRESTORE_HOST);
  wifiClient.println("Content-Type: application/json");
  wifiClient.println("Connection: close");
  wifiClient.print("Content-Length: "); wifiClient.println(bodyLen);
  wifiClient.println();
  wifiClient.print(body);

  unsigned long t = millis();
  while (!wifiClient.available() && millis() - t < 8000);
  if (wifiClient.available()) {
    Serial.println("Firestore: " + wifiClient.readStringUntil('\n'));
  }
  wifiClient.stop();
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.begin(16, 2);
  lcd.setRGB(0, 255, 0);
  lcd.setCursor(0, 0);
  lcd.print("Smart Sound");
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("WiFi");
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.status() == WL_CONNECTED ? " connected" : " failed");

  if (WiFi.status() == WL_CONNECTED) syncNTP();

  lcd.clear();
}

// ── Loop ──────────────────────────────────────────────────────────────────────
void loop() {
  int adc = analogRead(SOUND_PIN);
  float db = adcToDb(adc);

  if (state == IDLE) {
    drawLcd(db, false);

    if (adc >= ALARM_THRESHOLD) {
      state        = ALARM;
      alarmStartMs = millis();
      lastLoudMs   = millis();
      peakAdc      = adc;
      digitalWrite(BUZZER_PIN, HIGH);
      Serial.println("ALARM triggered");
    }

  } else { // ALARM
    if (adc > peakAdc) peakAdc = adc;
    if (adc >= SILENCE_THRESHOLD) lastLoudMs = millis();

    drawLcd(db, true);

    // Return to idle when quiet AND hold time has elapsed
    if (adc < SILENCE_THRESHOLD && (millis() - lastLoudMs) >= (unsigned long)ALARM_HOLD_MS) {
      digitalWrite(BUZZER_PIN, LOW);

      float peakDb      = adcToDb(peakAdc);
      float durationSec = (millis() - alarmStartMs) / 1000.0f;
      String ts         = timeSynced ? epochToRFC3339(nowEpoch()) : "1970-01-01T00:00:00Z";

      Serial.print("Logging — db="); Serial.print(peakDb);
      Serial.print(" duration="); Serial.print(durationSec);
      Serial.print("s time="); Serial.println(ts);

      logToFirestore(peakDb, durationSec, ts);

      state = IDLE;
      Serial.println("IDLE");
    }
  }

  delay(100);
}
