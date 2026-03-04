// Sound Sensor + Buzzer Alarm
// Arduino UNO R4 WiFi

const int soundPin = A0;     // Sound sensor
const int buzzerPin = 4;     // Buzzer on D4

int soundValue = 0;
int threshold = 100;   // Adjust after testing

void setup() {
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Sound Sensor Ready...");
}

void loop() {
  soundValue = analogRead(soundPin);
  
  Serial.print("Sound Level: ");
  Serial.println(soundValue);

  if (soundValue > threshold) {   // Too loud
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Too Loud!");
    delay(1500);
  } else {
    digitalWrite(buzzerPin, LOW);
  }

  delay(80);
}
