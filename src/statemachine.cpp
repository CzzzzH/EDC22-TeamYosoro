#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <servoCtl.h>
#include "statemachine.h"
#include "AngleControl.h"
#include "MotorControl.h"
#include "TimerInterrupt.h"
#include "JY61.h"
#include "IRReceiver.h"

StateMachine &StateMachine::getInstance()
{
    static StateMachine instance;
    return instance;
}

void StateMachine::init()
{
    // Debug Mode
    JY61::isDebug = false;
    Motor::isDebug = false;

    // Initialize Serial
    Serial.begin(9600);
    Serial2.begin(115200);
    Serial3.begin(115200);

    // Initialize components
    Motor::initialize();
    TimerInterrupt::initialize(interrupt_period);
    AngleControl::initialize();
    servoCtl::initialize(4);
    IRReceiver::initialize();
    
    // Other Initialization 
    Motor::targetSpeed = 0;
    outsideTarget.push_back({23, 236});
    outsideTarget.push_back({83, 232});
    nowMission = WAIT_FOR_START;
}

// Execute every clock interruption
void StateMachine::process()
{
    // Serial.println("TargetSpeed: " + String(Motor::targetSpeed));
    // Serial.println("TargetAngle: " + String(AngleControl::target));
    Serial.println("NowIRStatus: " + String(IRReceiver::leftOuterValue));
    Information &info = Information::getInstance();
    updateInfo(info);
    // Serial.println("Position:  " + String(info.getCarposX()) + "  " + String(info.getCarposY()));
    updateAction(info);
    updateMission(info);
    updateMotor(info);
    // counter++;
}

void StateMachine::updateInfo(Information &info)
{
    // TODO: update constant info (from gyroscope, encoders, zigbee)
    IRReceiver::updateValue();
}

void StateMachine::updateAction(Information &info)
{
    // TODO: update the current action (use infrared info and zigbee info)
    if (nowMission == GO_TO_MAZE)
    {
        if (info.getCarpos().getDist(outsideTarget[nowTargetIndex]) < 5)
        {
            switch (nowTargetIndex)
            {
            case 0:
                AngleControl::target -= 90;
                break;
            case 1:
                AngleControl::target -= 90;
                break;
            default:
                break;
            }
            nowTargetIndex++;
        }
    }
    else if (nowMission == SEARCH_MAZE)
    {
        if (IRReceiver::atCrossroad() && AngleControl::getAngleDist() < 5)
            AngleControl::target -= 90;
    }
}

void StateMachine::updateMission(Information &info)
{
    // TODO: update the current mission  (use updated info)
    if (info.getGameState() == GameGoing && nowMission == WAIT_FOR_START)
        nowMission = SEARCH_MAZE;
    if (nowTargetIndex == 2 && nowMission == GO_TO_MAZE)
    {
        nowTargetIndex = 0;
        nowMission = RETURN;
    }
}

void StateMachine::updateMotor(Information &info)
{
    // TODO: update the motor paramters (use PID)
    Motor::PID_compute();
    Motor::updatePWM();
    if (nowMission == WAIT_FOR_START) Motor::targetSpeed = 0;
    else Motor::targetSpeed = 30;
} 