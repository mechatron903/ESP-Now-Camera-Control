#ifndef AI_THINKER_BRAIN_LOGIC_H
#define AI_THINKER_BRAIN_LOGIC_H

#include <Arduino.h>
#include "ultrasonic_sensor.h"
#include "acebott_arm_controller.h"
#include "esp_camera.h"

// ==========================================
// AI Thinker Brain Logic
// Ultrasonic + Edge Impulse + ACEBOTT
// ==========================================

enum RobotState {
    STATE_INIT,
    STATE_ROAMING,
    STATE_OBSTACLE_DETECTED,
    STATE_RED_DETECTED,
    STATE_BLACK_DETECTED,
    STATE_PICKING_UP,
    STATE_AVOIDING,
    STATE_COMPLETE
};

struct BrainDecision {
    RobotState state;
    char action[30];
    ObjectColor objectColor;
    float distance;
    unsigned long timestamp;
};

class AIThinkerBrainLogic {
private:
    UltrasonicSensor ultrasonic;
    AceBotArmController armController;

    RobotState currentState;
    BrainDecision lastDecision;
    unsigned long lastDecisionTime;
    unsigned long decisionInterval;

    // Color detection variables
    bool redDetected;
    bool blackDetected;
    int redConfidence;
    int blackConfidence;

public:
    AIThinkerBrainLogic();

    bool init();
    void update();
    void detectColors();        // 
    void makeDecision();       // 
    void executeDecision();    // 

    void setState(RobotState newState);
    RobotState getCurrentState() const;

    void printState();
    void printDiagnostics();
};

#endif