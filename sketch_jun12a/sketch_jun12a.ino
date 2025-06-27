#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include "time.h"

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

// Pengaturan Waktu
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200;
const int   daylightOffset_sec = 0;

Servo myServo;
BlynkTimer timer;

int feedHour = -1;
int feedMinute = -1;
bool hasFedToday = false; 

void dispenseFood() {
  Serial.println("Memberi pakan sesuai jadwal...");
  myServo.write(SERVO_OPEN_ANGLE);
  delay(1000);
  myServo.write(SERVO_CLOSED_ANGLE);
  Serial.println("Pakan selesai diberikan.");
}

BLYNK_WRITE(V4) {
  TimeInputParam t(param);
  
  if (t.hasStartTime()) {
    feedHour = t.getStartHour();
    feedMinute = t.getStartMinute();
    Serial.print("Jadwal pakan diatur ke: ");
    Serial.print(feedHour);
    Serial.print(":");
    Serial.println(feedMinute);
  } else {
    feedHour = -1;
    feedMinute = -1;
    Serial.println("Jadwal pakan dinonaktifkan.");
  }
}

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
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Gagal mendapatkan waktu");
    return;
  }
  
  int lightValue = analogRead(analogPin);
  float foodLevel = getFoodLevel();
  Blynk.virtualWrite(V1, foodLevel);
  Blynk.virtualWrite(V2, lightValue);
  
  if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
    hasFedToday = false;
  }
  
  if (!hasFedToday && timeinfo.tm_hour == feedHour && timeinfo.tm_min == feedMinute) {
    dispenseFood();
    hasFedToday = true;
  }

  if (lightValue < LIGHT_THRESHOLD && foodLevel < 10) {
    dispenseFood();
  }
  
  Serial.print("Waktu: ");
  Serial.print(&timeinfo, "%H:%M:%S");
  Serial.print(" | Light: ");
  Serial.print(lightValue);
  Serial.print(" | Food Level: ");
  Serial.print(foodLevel);
  Serial.println("%");
}

void setup() {
  Serial.begin(115200);
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
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
