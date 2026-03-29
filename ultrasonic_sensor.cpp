#include "ultrasonic_sensor.h"

UltrasonicSensor::UltrasonicSensor() 
  : lastDistance(0), lastMeasurementTime(0), measurementInterval(100) {
}

void UltrasonicSensor::init() {
  Serial.println("[Ultrasonic] Initializing HC-SR04 sensor...");
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Test measurement
  delay(500);
  getDistance();
  
  Serial.println("[Ultrasonic] ✓ Sensor ready");
  Serial.printf("[Ultrasonic] Boundary trigger: %d cm\n", BOUNDARY_DISTANCE_CM);
}

float UltrasonicSensor::getDistance() {
  // Send 10us pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure echo time (with 30ms timeout)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  
  // Calculate distance (speed of sound = 343 m/s)
  // distance (cm) = (duration (microseconds) / 2) / 29.1
  float distance = (duration / 2.0) / 29.1;
  
  // Validate measurement
  if (distance < MIN_DISTANCE_CM || distance > MAX_DISTANCE_CM) {
    distance = lastDistance;  // Use previous valid reading
  }
  
  lastDistance = distance;
  lastMeasurementTime = millis();
  
  return distance;
}

float UltrasonicSensor::getSmoothedDistance() {
  // Average of 3 measurements for stability
  float total = 0;
  for (int i = 0; i < 3; i++) {
    total += getDistance();
    delay(50);
  }
  return total / 3.0;
}

DistanceZone UltrasonicSensor::getDistanceZone() {
  float distance = getDistance();
  
  if (distance >= SAFE_DISTANCE) {
    return ZONE_CLEAR;
  }
  else if (distance >= APPROACH_DISTANCE && distance < SAFE_DISTANCE) {
    return ZONE_APPROACH;
  }
  else if (distance > 0 && distance < APPROACH_DISTANCE) {
    return ZONE_PICKUP;
  }
  else {
    return ZONE_BLOCKED;
  }
}

bool UltrasonicSensor::isBoundaryDetected() {
  float distance = getSmoothedDistance();
  
  if (distance <= BOUNDARY_DISTANCE_CM && distance > 0) {
    Serial.printf("[Ultrasonic] ⚠ BOUNDARY DETECTED: %.1f cm\n", distance);
    return true;
  }
  return false;
}

void UltrasonicSensor::printDistance() {
  float distance = getDistance();
  
  Serial.printf("[Ultrasonic] Distance: %.1f cm", distance);
  
  DistanceZone zone = getDistanceZone();
  const char* zones[] = {"CLEAR", "APPROACH", "PICKUP", "BLOCKED"};
  Serial.printf(" | Zone: %s\n", zones[zone]);
}

void UltrasonicSensor::printStatus() {
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║   ULTRASONIC SENSOR STATUS         ║");
  Serial.println("╠════════════════════════════════════╣");
  Serial.printf("║ Trig Pin: %d                         ║\n", TRIG_PIN);
  Serial.printf("║ Echo Pin: %d                         ║\n", ECHO_PIN);
  Serial.printf("║ Last Distance: %.1f cm               ║\n", lastDistance);
  Serial.printf("║ Boundary Trigger: %d cm              ║\n", BOUNDARY_DISTANCE_CM);
  Serial.println("╚════════════════════════════════════╝\n");
}