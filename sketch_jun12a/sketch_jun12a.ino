#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

char auth[] = "MASUKKAN_AUTH_TOKEN_ANDA"; // Masukkan Auth Token dari aplikasi Blynk
char ssid[] = "NAMA_WIFI_ANDA";           // Masukkan nama WiFi Anda
char pass[] = "PASSWORD_WIFI_ANDA";      // Masukkan password WiFi Anda

// Pin definitions
const int analogPin = 34;
const int servoPin = 18;
const int trigPin = 5;
const int echoPin = 4;

// Constants 
const int LIGHT_THRESHOLD = 2000;
const int SERVO_OPEN_ANGLE = 180;
const int SERVO_CLOSED_ANGLE = 0;
const int FOOD_EMPTY_DISTANCE = 30;
const int FOOD_FULL_DISTANCE = 5;

Servo myServo;
BlynkTimer timer;

float getFoodLevel() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;

  float distanceCm = duration * 0.0343 / 2;
  
  float foodLevel = map(distanceCm, FOOD_EMPTY_DISTANCE, FOOD_FULL_DISTANCE, 0, 100);
  return constrain(foodLevel, 0, 100);
}

void mainTask() {
  int lightValue = analogRead(analogPin);
  
  float foodLevel = getFoodLevel();
  
  if (lightValue < LIGHT_THRESHOLD && foodLevel < 10) {
    myServo.write(SERVO_OPEN_ANGLE);
    delay(1000); 
    myServo.write(SERVO_CLOSED_ANGLE);
  }

  Blynk.virtualWrite(V1, foodLevel);  
  Blynk.virtualWrite(V2, lightValue); 
  
  Serial.print("Light: ");
  Serial.print(lightValue);
  Serial.print(" | Food Level: ");
  if (foodLevel == -1) {
    Serial.println("Error reading");
  } else {
    Serial.print(foodLevel);
    Serial.println("%");
  }
}

void setup() {
  Serial.begin(115200);
  
  Blynk.begin(auth, ssid, pass);
  
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2400);
  myServo.write(SERVO_CLOSED_ANGLE);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  timer.setInterval(5000L, mainTask);
}

void loop() {
  Blynk.run();
  timer.run();
}

void loop() {
  Blynk.run();
  timer.run();
}
