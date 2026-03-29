#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

// ====================================================
// Ultrasonic Sensor HC-SR04
// Distance Measurement for Robot Boundary Detection
// ====================================================

// Sensor Pins
#define TRIG_PIN 5      // Trigger pin
#define ECHO_PIN 18     // Echo pin

// Distance Thresholds
#define BOUNDARY_DISTANCE_CM 10     // 10cm trigger
#define MAX_DISTANCE_CM 400         // Max measurable
#define MIN_DISTANCE_CM 2           // Min measurable

// Measurement modes
#define SAFE_DISTANCE 30       // Safe distance to maintain
#define APPROACH_DISTANCE 15   // Start approaching
#define PICKUP_DISTANCE 10     // Ready to pickup

enum DistanceZone {
  ZONE_CLEAR = 0,      // > 30cm
  ZONE_APPROACH = 1,   // 15-30cm
  ZONE_PICKUP = 2,     // < 10cm
  ZONE_BLOCKED = 3     // Obstacle in path
};

class UltrasonicSensor {
private:
  float lastDistance;
  unsigned long lastMeasurementTime;
  unsigned long measurementInterval;
  
public:
  UltrasonicSensor();
  
  void init();
  float getDistance();                    // Returns distance in CM
  float getSmoothedDistance();            // Averaged reading
  DistanceZone getDistanceZone();
  bool isBoundaryDetected();              // 10cm trigger
  
  void printDistance();
  void printStatus();
};

#endif