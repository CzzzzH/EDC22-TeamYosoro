#include <Arduino.h>
#include "statemachine.h"

StateMachine& StateMachine::getInstance()
{
    static StateMachine instance;
    return instance;
}

// Execute every clock interruption
void StateMachine::process()
{
    // Begin
    updateInfo();
    updateMission();
    updateAction();
    updateMotor();
    // End
}

void StateMachine::updateInfo()
{
    // TODO: update constant info (from gyroscope, encoders, zigbee)
    
}

void StateMachine::updateMission()
{
    // TODO: update the current mission  (use updated info)
}

void StateMachine::updateAction()
{
    // TODO: update the current action (use infrared info and zigbee info)
}

void StateMachine::updateMotor()
{
    // TODO: update the motor paramters (use PID)
}