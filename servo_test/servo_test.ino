#include <ESP32Servo.h>

const int servoPin = 18;
Servo myServo;

void setup() {
  Serial.begin(115200);
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2400);
}

void loop() {
  // Sweep from 0 to 180
  for (int angle = 0; angle <= 180; angle += 10) {
    myServo.write(angle);
    Serial.print("Servo angle: ");
    Serial.println(angle);
    delay(500);
  }
  // Sweep from 180 to 0
  for (int angle = 180; angle >= 0; angle -= 10) {
    myServo.write(angle);
    Serial.print("Servo angle: ");
    Serial.println(angle);
    delay(500);
  }
} 