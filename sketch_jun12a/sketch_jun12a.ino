#include <ESP32Servo.h>

// Pin definitions
const int analogPin = 34;    // MH sensor analog output
const int servoPin = 18;     // Servo control pin
const int trigPin = 5;       // HC-SR04 Trigger pin
const int echoPin = 4;       // HC-SR04 Echo pin

// Constants
const int LIGHT_THRESHOLD = 2000;  // Threshold for light sensor (0-4095)
const int SERVO_OPEN_ANGLE = 180;  // Angle to dispense food
const int SERVO_CLOSED_ANGLE = 0;  // Angle when closed
const int FOOD_EMPTY_DISTANCE = 30; // Distance in cm when food container is empty
const int FOOD_FULL_DISTANCE = 5;  // Distance in cm when food container is full

Servo myServo;

void setup() {
  Serial.begin(115200);
  
  // Initialize servo
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2400);
  myServo.write(SERVO_CLOSED_ANGLE);  // Start with servo closed

  // Initialize HC-SR04 pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

float getFoodLevel() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;  // Error reading

  float distanceCm = duration * 0.0343 / 2;
  
  // Convert distance to food level percentage
  float foodLevel = map(distanceCm, FOOD_EMPTY_DISTANCE, FOOD_FULL_DISTANCE, 0, 100);
  return constrain(foodLevel, 0, 100);
}

void sendData(float foodLevel, int lightValue) {
  // Send data in CSV format: timestamp,food_level,light_value
  Serial.print(millis());
  Serial.print(",");
  Serial.print(foodLevel);
  Serial.print(",");
  Serial.println(lightValue);
}

void loop() {
  // Read light sensor
  int lightValue = analogRead(analogPin);
  
  // Get food level
  float foodLevel = getFoodLevel();
  
  // If light is high OR food is 0%, rotate servo to dispense food
  if (lightValue < LIGHT_THRESHOLD && foodLevel < 10) {
    myServo.write(SERVO_OPEN_ANGLE);
    delay(1000);  // Keep open for 1 second
    myServo.write(SERVO_CLOSED_ANGLE);
  }

  // Send data through Serial
  sendData(foodLevel, lightValue);

  // Print debug information
  Serial.print("Light: ");
  Serial.print(lightValue);
  Serial.print(" | Food Level: ");
  if (foodLevel == -1) {
    Serial.println("Error reading");
  } else {
    Serial.print(foodLevel);
    Serial.println("%");
  }

  delay(5000);  // Wait 5 seconds between readings
}
