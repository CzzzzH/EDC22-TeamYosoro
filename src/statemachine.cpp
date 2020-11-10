#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>
#include "statemachine.h"
#include "AngleControl.h"
#include "MotorControl.h"
#include "TimerInterrupt.h"
#include "JY61.h"
#include "IRReceiver.h"
#include "Maze.h"

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

    // Other Initialization
    outsideTarget.push({16, 240});
    outsideTarget.push({72, 240});

    // A Simple Path
    insideTarget.push(20);
    insideTarget.push(19);
    insideTarget.push(13);
    insideTarget.push(14);
    insideTarget.push(15);
    insideTarget.push(9);
    insideTarget.push(8);
    insideTarget.push(9);
    insideTarget.push(14);
    insideTarget.push(28);
    insideTarget.push(32);

    // Test big turn only
    // insideTarget.push(26);
    // insideTarget.push(27);
    // insideTarget.push(26);
    // insideTarget.push(27);
    // insideTarget.push(26);
    // insideTarget.push(27);
    // insideTarget.push(26);

    nowMission = SEARCH_MAZE;
    nowMazeIndex = 26;
    lastMazeIndex = 32;

    // Initialize components
    Motor::initialize();
    Motor::targetSpeed = 30;
    AngleControl::initialize();
    IRReceiver::initialize();
    Maze::initialize(Information::getInstance());
    TimerInterrupt::initialize(interrupt_period);
}

// Execute every clock interruption
void StateMachine::process()
{
    Information &info = Information::getInstance();

    updateInfo();
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

void StateMachine::updateInfo()
{
    // TODO: update IR info
    IRReceiver::updateValue();
}

void StateMachine::updateAction(Information &info)
{
    // TODO: update the current action (use infrared info and zigbee info)
    if (nowMission == GO_TO_MAZE)
    {
        if (info.getCarpos().getDist(outsideTarget.front()) < 15)
        {
            outsideTarget.pop();
            switch (outsideTarget.size())
            {
            case 1:
                AngleControl::target += 90;
                break;
            case 0:
                AngleControl::target += 90;
                break;
            default:
                break;
            }   
        }
    }
    else if (nowMission == SEARCH_MAZE)
    {   
        CrossroadAction crossroadAction = Maze::getDirection(lastMazeIndex, nowMazeIndex, insideTarget.front());
        nowAction = GO_AHEAD;
        if (IRReceiver::atCrossroad(crossroadAction.rotateAngle) && !insideTarget.empty())
        {
            AngleControl::target += crossroadAction.rotateAngle;
            if (crossroadAction.rotateAngle != 0)
            {
                if (crossroadAction.rotateAngle == 180) nowAction = BIG_TURN;
                else nowAction = SMALL_TURN;
                AngleControl::target -= offset;
                offset = 0;
            }
            lastMazeIndex = nowMazeIndex;
            nowMazeIndex = crossroadAction.nextPosition;
            if (nowMazeIndex == insideTarget.front()) insideTarget.pop();
        }
        else if (AngleControl::getAngleDist() < 15)
        {
            offset += IRReceiver::angleOffset();
            AngleControl::target += IRReceiver::angleOffset();
        }
    }
}

void StateMachine::updateMission(Information &info)
{
    // TODO: update the current mission  (use updated info)
    // Serial.println("GameState" + String(info.getGameState()));
    if (info.getGameState() == GameGoing && nowMission == WAIT_FOR_START)
    {
        nowMission = GO_TO_MAZE;
        Motor::targetSpeed = 30;
    }
    if (nowMission == WAIT_FOR_START && outsideTarget.empty())
        nowMission = SEARCH_MAZE;
}

void StateMachine::updateMotor(Information &info)
{
    // TODO: update the motor paramters (use PID)
    Motor::PID_compute();

    if (nowMission == WAIT_FOR_START)
        Motor::targetSpeed = 0;
}