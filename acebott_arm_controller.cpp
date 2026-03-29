#include "acebott_arm_controller.h"

AceBotArmController::AceBotArmController()
    : initialized(false), lastDetectedColor(COLOR_NONE) {
}

bool AceBotArmController::init() {
    Serial.println("[AceBotArm] Initializing ACEBOTT Arm Controller...");

    try {
        // Initialize ACB (motor driver)
        acb.begin();
        Serial.println("[AceBotArm] ✓ ACB library initialized");

        // Initialize servos
        gripperServo.attach(GRIPPER_PIN, 500, 2500);
        shoulderServo.attach(SHOULDER_PIN, 500, 2500);

        Serial.println("[AceBotArm] ✓ Servos attached");

        delay(500);

        // Set initial positions
        gripperServo.write(GRIPPER_OPEN);
        shoulderServo.write(SHOULDER_UP);

        delay(1000);

        // Initialize motor pins
        pinMode(ACB_MOTOR_M1_FWD, OUTPUT);
        pinMode(ACB_MOTOR_M1_BCK, OUTPUT);
        pinMode(ACB_MOTOR_M2_FWD, OUTPUT);
        pinMode(ACB_MOTOR_M2_BCK, OUTPUT);

        stopMotors();

        initialized = true;
        Serial.println("[AceBotArm] ✓ ACEBOTT Arm ready");
        printStatus();

        return true;
    }
    catch (...) {
        Serial.println("[AceBotArm] ✗ Initialization failed");
        initialized = false;
        return false;
    }
}

// ===== MOTOR CONTROL =====

void AceBotArmController::moveForward(int speed) {
    if (!initialized) return;

    speed = constrain(speed, 0, 255);

    digitalWrite(ACB_MOTOR_M1_FWD, HIGH);
    digitalWrite(ACB_MOTOR_M1_BCK, LOW);
    digitalWrite(ACB_MOTOR_M2_FWD, HIGH);
    digitalWrite(ACB_MOTOR_M2_BCK, LOW);

    analogWrite(ACB_MOTOR_M1_FWD, speed);
    analogWrite(ACB_MOTOR_M2_FWD, speed);

    Serial.printf("[AceBotArm] ► FORWARD (speed: %d)\n", speed);
}

void AceBotArmController::moveBackward(int speed) {
    if (!initialized) return;

    speed = constrain(speed, 0, 255);

    digitalWrite(ACB_MOTOR_M1_FWD, LOW);
    digitalWrite(ACB_MOTOR_M1_BCK, HIGH);
    digitalWrite(ACB_MOTOR_M2_FWD, LOW);
    digitalWrite(ACB_MOTOR_M2_BCK, HIGH);

    analogWrite(ACB_MOTOR_M1_BCK, speed);
    analogWrite(ACB_MOTOR_M2_BCK, speed);

    Serial.printf("[AceBotArm] ◄ BACKWARD (speed: %d)\n", speed);
}

void AceBotArmController::turnLeft(int speed) {
    if (!initialized) return;

    speed = constrain(speed, 0, 255);

    digitalWrite(ACB_MOTOR_M1_FWD, LOW);
    digitalWrite(ACB_MOTOR_M1_BCK, HIGH);
    digitalWrite(ACB_MOTOR_M2_FWD, HIGH);
    digitalWrite(ACB_MOTOR_M2_BCK, LOW);

    analogWrite(ACB_MOTOR_M1_BCK, speed);
    analogWrite(ACB_MOTOR_M2_FWD, speed);

    Serial.printf("[AceBotArm] ↙ LEFT (speed: %d)\n", speed);
}

void AceBotArmController::turnRight(int speed) {
    if (!initialized) return;

    speed = constrain(speed, 0, 255);

    digitalWrite(ACB_MOTOR_M1_FWD, HIGH);
    digitalWrite(ACB_MOTOR_M1_BCK, LOW);
    digitalWrite(ACB_MOTOR_M2_FWD, LOW);
    digitalWrite(ACB_MOTOR_M2_BCK, HIGH);

    analogWrite(ACB_MOTOR_M1_FWD, speed);
    analogWrite(ACB_MOTOR_M2_BCK, speed);

    Serial.printf("[AceBotArm] ↗ RIGHT (speed: %d)\n", speed);
}

void AceBotArmController::stopMotors() {
    if (!initialized) return;

    digitalWrite(ACB_MOTOR_M1_FWD, LOW);
    digitalWrite(ACB_MOTOR_M1_BCK, LOW);
    digitalWrite(ACB_MOTOR_M2_FWD, LOW);
    digitalWrite(ACB_MOTOR_M2_BCK, LOW);

    Serial.println("[AceBotArm] 🟧 MOTORS STOPPED");
}

// ===== ARM CONTROL =====

void AceBotArmController::setGripper(int angle) {
    if (!initialized) return;

    angle = constrain(angle, 0, 180);
    gripperServo.write(angle);

    Serial.printf("[AceBotArm] Gripper: %d°\n", angle);
    delay(300);
}

void AceBotArmController::setShoulder(int angle) {
    if (!initialized) return;

    angle = constrain(angle, 0, 180);
    shoulderServo.write(angle);

    Serial.printf("[AceBotArm] Shoulder: %d°\n", angle);
    delay(300);
}

void AceBotArmController::gripperOpen() {
    setGripper(GRIPPER_OPEN);
}

void AceBotArmController::gripperClose() {
    setGripper(GRIPPER_CLOSED);
}

void AceBotArmController::shoulderUp() {
    setShoulder(SHOULDER_UP);
}

void AceBotArmController::shoulderDown() {
    setShoulder(SHOULDER_DOWN);
}

void AceBotArmController::shoulderMid() {
    setShoulder(SHOULDER_MID);
}

// ===== PICKUP SEQUENCES =====

void AceBotArmController::pickupRed() {
    Serial.println("\n ┌─────────────────────────────┐");
    Serial.println("  |    PICKING UP RED OBJECT    |");
    Serial.println("  └─────────────────────────────┘\n");

    stopMotors();
    delay(300);

    // Step 1: Open gripper
    Serial.println("[AceBotArm] Step 1: Opening gripper");
    gripperOpen();
    delay(300);

    // Step 2: Lower shoulder to pickup height
    Serial.println("[AceBotArm] Step 2: Lowering shoulder");
    shoulderDown();
    delay(500);

    // Step 3: Close gripper (grab object)
    Serial.println("[AceBotArm] Step 3: Closing gripper to grab");
    gripperClose();
    delay(500);

    // Step 4: Raise shoulder (lift object)
    Serial.println("[AceBotArm] Step 4: Raising shoulder");
    shoulderUp();
    delay(500);

    // Step 5: Move backward with object
    Serial.println("[AceBotArm] Step 5: Retreating with object");
    moveBackward(MOTOR_SLOW);
    delay(1000);
    stopMotors();

    // Step 6: Drop object (optional)
    Serial.println("[AceBotArm] Step 6: Releasing object");
    gripperOpen();
    delay(300);

    Serial.println("[AceBotArm] ✓ Pickup sequence complete\n");
    lastDetectedColor = COLOR_RED;
}

void AceBotArmController::avoidBlack() {
    Serial.println("\n ┌─────────────────────────────┐");
    Serial.println("  |    AVOIDING BLACK OBSTACLE  |");
    Serial.println("  └─────────────────────────────┘\n");

    // Step 1: Stop
    Serial.println("[AceBotArm] Step 1: Stopping");
    stopMotors();
    delay(300);

    // Step 2: Move backward to avoid
    Serial.println("[AceBotArm] Step 2: Moving backward");
    moveBackward(MOTOR_HALF);
    delay(600);

    // Step 3: Turn right (avoid left side)
    Serial.println("[AceBotArm] Step 3: Turning right");
    turnRight(MOTOR_HALF);
    delay(700);

    // Step 4: Move forward around obstacle
    Serial.println("[AceBotArm] Step 4: Moving around obstacle");
    moveForward(MOTOR_HALF);
    delay(800);

    // Step 5: Turn left to align
    Serial.println("[AceBotArm] Step 5: Aligning");
    turnLeft(MOTOR_HALF);
    delay(600);

    stopMotors();

    Serial.println("[AceBotArm] ✓ Avoidance complete\n");
    lastDetectedColor = COLOR_BLACK;
}

void AceBotArmController::returnHome() {
    Serial.println("[AceBotArm] Returning to home position...");

    stopMotors();
    delay(200);

    gripperOpen();
    delay(300);

    shoulderUp();
    delay(500);

    Serial.println("[AceBotArm] ✓ Home position reached\n");
}

bool AceBotArmController::isInitialized() {
    return initialized;
}

void AceBotArmController::printStatus() {
    Serial.println("\n ┌─────────────────────────────────┐");
    Serial.println("  |   ACEBOTT ARM CONTROLLER STATUS |");
    Serial.println("  ├─────────────────────────────────┤");
    Serial.println("  |  Gripper Servo: Pin 4            |");
    Serial.println("  |  Shoulder Servo: Pin 26          |");
    Serial.println("  |  Motor Control: ACB Library      |");
    Serial.println("  |                                  |");
    Serial.println("  |  Status: Ready                   |");
    Serial.println("  └─────────────────────────────────┘\n");
}