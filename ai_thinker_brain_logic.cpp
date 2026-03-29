#include "ai_thinker_brain_logic.h"
#include "esp_camera.h"  // for camera functions
#include "ultrasonic.h"  // assume ultrasonic class exists
#include "arm_controller.h" // assume armController exists

// External objects (should be defined elsewhere)
extern Ultrasonic ultrasonic;
extern ArmController armController;

AIThinkerBrainLogic::AIThinkerBrainLogic() 
    : currentState(STATE_INIT), 
      lastDecisionTime(0), 
      decisionInterval(200), 
      redDetected(false), 
      blackDetected(false), 
      redConfidence(0), 
      blackConfidence(0) 
{
}

bool AIThinkerBrainLogic::init() {
    Serial.println("\n[Brain] Initializing AI Thinker Brain Logic...");

    // Initialize ultrasonic sensor
    ultrasonic.init();
    Serial.println("[Brain] ✓ Ultrasonic ready");
    return true;
}

void AIThinkerBrainLogic::update() {
    if (millis() - lastDecisionTime < decisionInterval) {
        return;
    }
    lastDecisionTime = millis();

    // Check ultrasonic sensor
    DistanceZone zone = ultrasonic.getDistanceZone();
    float distance = ultrasonic.getDistance();

    // Detect colors from camera frame
    detectColors();

    // Make decision
    makeDecision();

    // Execute decision
    executeDecision();
}

void AIThinkerBrainLogic::detectColors() {
    // Capture frame
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        return;
    }

    // TODO: Run Edge Impulse model
    // Extract color detection results
    // Update redDetected, blackDetected, redConfidence, blackConfidence

    // For now, placeholder:
    // This should be replaced with actual Edge Impulse inference
    // Example:
    // if (edge_impulse_result.red_detected) {
    //    redDetected = true;
    //    redConfidence = edge_impulse_result.red_confidence;
    // }

    esp_camera_fb_return(fb);
}

void AIThinkerBrainLogic::makeDecision() {
    float distance = ultrasonic.getDistance();

    // Priority 1: Boundary detection (ultrasonic at 10cm)
    if (ultrasonic.isBoundaryDetected()) {
        lastDecision.distance = distance;
        lastDecision.timestamp = millis();

        // Check what's at boundary
        if (redDetected && redConfidence > 60) {
            strcpy(lastDecision.action, "PICKUP_RED");
            lastDecision.objectColor = COLOR_RED;
            setState(STATE_RED_DETECTED);
        }
        else if (blackDetected && blackConfidence > 60) {
            strcpy(lastDecision.action, "AVOID_BLACK");
            lastDecision.objectColor = COLOR_BLACK;
            setState(STATE_BLACK_DETECTED);
        }
        else {
            strcpy(lastDecision.action, "OBSTACLE_FOUND");
            setState(STATE_OBSTACLE_DETECTED);
        }
    }
    // Priority 2: Normal roaming
    else {
        strcpy(lastDecision.action, "ROAMING");
        lastDecision.distance = distance;
        lastDecision.timestamp = millis();
        setState(STATE_ROAMING);
    }
}

void AIThinkerBrainLogic::executeDecision() {
    Serial.println("[Brain] Action: %s | Distance: %.1f cm\n",
        lastDecision.action, lastDecision.distance);

    switch(currentState) {
        case STATE_ROAMING:
            // Move forward at slow speed
            armController.moveForward(MOTOR_SLOW);
            break;

        case STATE_RED_DETECTED:
            // Pick up red object
            armController.pickupRed();
            setState(STATE_PICKING_UP);
            delay(2000);
            setState(STATE_ROAMING);
            break;

        case STATE_BLACK_DETECTED:
            // Avoid black obstacle
            armController.avoidBlack();
            setState(STATE_AVOIDING);
            delay(2000);
            setState(STATE_ROAMING);
            break;

        case STATE_OBSTACLE_DETECTED:
            // Generic obstacle avoidance
            armController.stopMotors();
            delay(300);
            armController.moveBackward(MOTOR_HALF);
            delay(600);
            armController.turnRight(MOTOR_HALF);
            delay(700);
            armController.stopMotors();
            setState(STATE_ROAMING);
            break;

        case STATE_INIT:
        default:
            armController.stopMotors();
            break;
    }
}

void AIThinkerBrainLogic::setState(RobotState newState) {
    if (newState != currentState) {
        currentState = newState;
        lastDecision.state = newState;
        printState();
    }
}

void AIThinkerBrainLogic::printState() {
    const char* stateNames[] = {
        "INIT", "ROAMING", "OBSTACLE_DETECTED",
        "RED_DETECTED", "BLACK_DETECTED", "PICKING_UP", "AVOIDING", "COMPLETE"
    };
    if (currentState < 8) {
        Serial.println("[Brain] 🧠 State: %s\n", stateNames[currentState]);
    }
}

void AIThinkerBrainLogic::printDiagnostics() {
    Serial.println("\n");
    Serial.println("AI THINKER BRAIN 🧠 NOSTICS \n");
    Serial.println("|| Current State: %s\n", stateNames[currentState]);
    Serial.println("|| Last Action: %s\n", lastDecision.action);
    ultrasonic.printDistance();
    Serial.println("|| Red Detected: %s (Correlation? \"VEC\" - \"N\")", %d%%)\n",
        redDetected ? "VEC" : "N");
    Serial.println(" | Red Detected: %s (Conf: %d%) \n",
        redDetected ? "YES" : "NO", redConfidence);
    Serial.println(" | Black Detected: %s (Conf: %d%) \n",
        blackDetected ? "YES" : "NO", blackConfidence);
    Serial.println(" |");
}