#ifndef ACEBOTT_ARM_CONTROLLER_H
#define ACEBOTT_ARM_CONTROLLER_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include <ACB.h>

// ================================
// ACEBOTT Arm Controller
// Gripper (Pin 4) + Shoulder (Pin 26)
// Red/Black Object Pickup
// ================================

// Servo Pins
#define GRIPPER_PIN     4     // Gripper servo pin
#define SHOULDER_PIN    26    // Shoulder servo pin

// ACB Motor Pins (for chassis)
#define ACB_MOTOR_M1_FWD 12
#define ACB_MOTOR_M1_BCK 13
#define ACB_MOTOR_M2_FWD 14
#define ACB_MOTOR_M2_BCK 15

// Servo Positions
#define GRIPPER_OPEN    0
#define GRIPPER_CLOSED  90

#define SHOULDER_UP     180   // Reach up
#define SHOULDER_MID    90    // Rest/mid
#define SHOULDER_DOWN   20    // Pickup position

// Motor Speed Constants
#define MOTOR_FULL  255
#define MOTOR_HALF  128
#define MOTOR_SLOW  64

// Detection constants
enum ObjectColor {
    COLOR_NONE  = 0,
    COLOR_RED   = 1,
    COLOR_BLACK = 2
};

class AceBotArmController {
private:
    Servo gripperServo;
    Servo shoulderServo;
    ACB acb;

    bool initialized;
    ObjectColor lastDetectedColor;

public:
    AceBotArmController();

    bool init();

    // Motor control (chassis)
    void moveForward(int speed);
    void moveBackward(int speed);
    void turnLeft(int speed);
    void turnRight(int speed);
    void stopMotors();

    // Arm control (servos)
    void setGripper(int angle);
    void setShoulder(int angle);
    void gripperOpen();
    void gripperClose();
    void shoulderUp();
    void shoulderDown();
    void shoulderMid();

    // Pickup sequences
    void pickupRed();
    void avoidBlack();
    void returnHome();

    // Status
    bool isInitialized();
    void printStatus();
};

#endif