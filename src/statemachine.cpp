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

    // Set Half
    nowHalf = SECOND_HALF;

    // Initialize Serial
    Serial.begin(9600);
    Serial2.begin(115200);
    Serial3.begin(115200);

    // Other Initialization
    outsideTarget.push({16, 240});
    outsideTarget.push({72, 240});

    // A Simple Path
    insideTarget.push_back(20);
    insideTarget.push_back(19);
    insideTarget.push_back(13);
    insideTarget.push_back(14);
    insideTarget.push_back(15);
    insideTarget.push_back(9);
    insideTarget.push_back(8);
    insideTarget.push_back(9);
    insideTarget.push_back(14);
    insideTarget.push_back(28);
    insideTarget.push_back(32);

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
    Motor::targetSpeed = 45;
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
    // IRReceiver::updateValue();
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
        CrossroadAction crossroadAction;
        if (motorDirection == 1)
            crossroadAction = Maze::getDirection(lastMazeIndex, nowMazeIndex, insideTarget.front());
        else
            crossroadAction = Maze::getDirection(2 * nowMazeIndex - lastMazeIndex, nowMazeIndex, insideTarget.front());
        nowAction = GO_AHEAD;
        if (IRReceiver::atCrossroad(crossroadAction.rotateAngle) && !insideTarget.empty())
        {
            if (crossroadAction.rotateAngle == 180)
            {
                if (motorDirection == 1) motorDirection = -1;
            }
            else
            {
                motorDirection = 1;
                AngleControl::target += crossroadAction.rotateAngle;
            }
            lastMazeIndex = nowMazeIndex;
            nowMazeIndex = crossroadAction.nextPosition;
            if (nowMazeIndex == insideTarget.front())
                insideTarget.erase(0);
        }
    }
}

void StateMachine::updateMission(Information &info)
{
    // TODO: update the current mission  (use updated info)

    if (info.getGameState() == GameGoing && nowMission == WAIT_FOR_START)
    {
        nowMission = GO_TO_MAZE;
        Motor::targetSpeed = 30;
    }
    if (nowMission == WAIT_FOR_START && outsideTarget.empty())
        nowMission = SEARCH_MAZE;
    if (nowMission == SEARCH_MAZE && insideTarget.empty())
        nowMission = END_GAME;
}

void StateMachine::updateMotor(Information &info)
{
    // TODO: update the motor paramters (use PID)
    if (nowMission == WAIT_FOR_START || nowMission == END_GAME)
        Motor::targetSpeed = 0;
    else Motor::targetSpeed = fabs(Motor::targetSpeed) * motorDirection;

    Motor::PID_compute();
}