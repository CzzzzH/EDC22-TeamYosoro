#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>
#include "statemachine.h"
#include "AngleControl.h"
#include "MotorControl.h"
#include "TimerInterrupt.h"
#include "JY61.h"
#include "IRReceiver.h"

// Define to debug components
// #define DEBUG_MOTOR
// #define DEBUG_ANGLECONTROLER
// #define DEBUG_IRRECEIVER
// #define DEBUG_ZIGBEE
// #define DEBUG_TIMER

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
    IRReceiver::initialize();
    
    // Other Initialization 
    outsideTarget.push_back({16, 240});
    outsideTarget.push_back({72, 240});
    // insideTarget.push_back({0, 1});
    // insideTarget.push_back({-1, 1});
    // insideTarget.push_back({-1, 0});
    // insideTarget.push_back({0, 0});
    // insideTarget.push_back({0, 1});
    // insideTarget.push_back({-1, 1});
    // insideTarget.push_back({-1, 0});
    // insideTarget.push_back({0, 0});
    nowMission = SEARCH_MAZE;
    nowDirection = Y_POSITIVE;
    nowMazePosition = {0, 0};
}

// Execute every clock interruption
void StateMachine::process()
{
    Information &info = Information::getInstance();

    updateAction(info);
    updateMission(info);
    updateMotor(info);
    counter++;

    // Fill debug codes below
    #ifdef DEBUG_MOTOR
        Serial.println("Target Speed: " + String(Motor::targetSpeed));
        Serial.println("Left Motor Counter: " + String(encoder::counter.left));
        Serial.println("Right Motor Counter: " + String(encoder::counter.right));
    #endif

    #ifdef DEBUG_ANGLECONTROLER
        Serial.println("Now Angle: " + String(JY61::Angle[2]));
        Serial.println("Target Angle: " + String(AngleControl::target));
    #endif

    #ifdef DEBUG_IRRECEIVER
        Serial.println("LeftIR: " + String(IRReceiver::leftValue[0]) + " " + String(IRReceiver::leftValue[1]) + " " + String(IRReceiver::leftValue[2]));
        Serial.println("RightIR: " + String(IRReceiver::rightValue[0]) + " " + String(IRReceiver::rightValue[1]) + " " + String(IRReceiver::rightValue[2]));
        Serial.println("MidIR: " + String(IRReceiver::midValue[0]) + " " + String(IRReceiver::midValue[1]) + " " + String(IRReceiver::midValue[2]) + " " + String(IRReceiver::midValue[3]));
    #endif

    #ifdef DEBUG_ZIGBEE
        Serial.println("Car Position:  " + String(info.getCarposX()) + "  " + String(info.getCarposY()));
    #endif

    #ifdef DEBUG_TIMER
        Serial.println("Counter: " + String(counter));
        Serial.println("Milli Seconds: " + String(millis()));
    #endif
    
}

void StateMachine::updateInfo(Information &info)
{
    // TODO: update constant info (from gyroscope, encoders, zigbee)
    // info.updateInfo();
    IRReceiver::updateValue();
}

void StateMachine::updateAction(Information &info)
{
    // TODO: update the current action (use infrared info and zigbee info)
    if (nowMission == GO_TO_MAZE)
    {
        if (info.getCarpos().getDist(outsideTarget[nowTargetIndex]) < 15)
        {
            switch (nowTargetIndex)
            {
            case 0:
                AngleControl::target += 90;
                break;
            case 1:
                AngleControl::target += 90;
                break;
            default:
                break;
            }
            nowTargetIndex++;
        }
    }
    else if (nowMission == SEARCH_MAZE)
    {
        if (IRReceiver::atCrossroad())
        {
            AngleControl::target += 90;
            switch (nowDirection)
            {
            case Y_POSITIVE:
                nowMazePosition.y++;
                break;
            case Y_NEGTIVE:
                nowMazePosition.y--;
                break;
            case X_POSITIVE:
                nowMazePosition.x++;
                break;
            case X_NEGTIVE:
                nowMazePosition.x--;
                break;
            default:
                break;
            }
            nowTargetIndex++;
            // turnInMaze(1);
        }
        // else AngleControl::target += IRReceiver::angleOffset();
    }
}

void StateMachine::updateMission(Information &info)
{
    // TODO: update the current mission  (use updated info)
    // Serial.println("GameState" + String(info.getGameState()));
    if (info.getGameState() == GameGoing && nowMission == WAIT_FOR_START)
        nowMission = GO_TO_MAZE;
    if (nowTargetIndex == 2 && nowMission == GO_TO_MAZE)
    {
        nowTargetIndex = 0;
        nowMission = SEARCH_MAZE;
    }
}

void StateMachine::updateMotor(Information &info)
{
    // TODO: update the motor paramters (use PID)
    Motor::PID_compute();

    JY61::read();
	AngleControl::Compute();
    Motor::updatePWM();
    // Motor::setPWM(100,false);
    if (nowMission == WAIT_FOR_START) Motor::targetSpeed = 0;
    else Motor::targetSpeed = 30;
} 

void StateMachine::turnInMaze(bool dir)
{
    nowDirection = Direction(int(nowDirection) + int(dir > 0));
    if (nowDirection > 3) nowDirection = Y_POSITIVE;
    if (nowDirection < 0) nowDirection = X_POSITIVE;
}