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
#include "information.h"

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
    // Get info
    Information &info = Information::getInstance();
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
    outsideTarget.push_back({16, 240});
    outsideTarget.push_back({72, 240});

    // A Simple Path
    insideTarget.push_back(20);
    insideTarget.push_back(13);
    insideTarget.push_back(9);
    insideTarget.push_back(15);
    insideTarget.push_back(7);
    insideTarget.push_back(22);
    insideTarget.push_back(32);

    // Test big turn only
    // insideTarget.push(26);
    // insideTarget.push(27);
    // insideTarget.push(26);
    // insideTarget.push(27);
    // insideTarget.push(26);
    // insideTarget.push(27);
    // insideTarget.push(26);

    info.Obstacle[0].posA.X = 32;
    info.Obstacle[0].posA.Y = 56;
    info.Obstacle[0].posB.X = 83;
    info.Obstacle[0].posB.Y = 70;
    
    info.Obstacle[1].posA.X = 127;
    info.Obstacle[1].posA.Y = 67;
    info.Obstacle[1].posB.X = 127;
    info.Obstacle[1].posB.Y = 97;
    
    info.Obstacle[2].posA.X = 127;
    info.Obstacle[2].posA.Y = 187;
    info.Obstacle[2].posB.X = 157;
    info.Obstacle[2].posB.Y = 187;
    
    info.Obstacle[3].posA.X = 180;
    info.Obstacle[3].posA.Y = 126;
    info.Obstacle[3].posB.X = 180;
    info.Obstacle[3].posB.Y = 220;
    
    info.Obstacle[4].posA.X = 157;
    info.Obstacle[4].posA.Y = 65;
    info.Obstacle[4].posB.X = 187;
    info.Obstacle[4].posB.Y = 67;
    
    info.Obstacle[5].posA.X = 52;
    info.Obstacle[5].posA.Y = 97;
    info.Obstacle[5].posB.X = 82;
    info.Obstacle[5].posB.Y = 97;
    
    info.Obstacle[6].posA.X = 67;
    info.Obstacle[6].posA.Y = 127;
    info.Obstacle[6].posB.X = 67;
    info.Obstacle[6].posB.Y = 157;
    
    info.Obstacle[7].posA.X = 156;
    info.Obstacle[7].posA.Y = 97;
    info.Obstacle[7].posB.X = 180;
    info.Obstacle[7].posB.Y = 97;

    nowMission = SEARCH_MAZE;
    nowMazeIndex = 32;
    lastMazeIndex = 38;
    motorDirection = 1;

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

}

void StateMachine::updateAction(Information &info)
{
    // TODO: update the current action (use infrared info and zigbee info)
    if (nowMission == GO_TO_MAZE)
    {
        if (info.getCarpos().getDist(outsideTarget.front()) < 15)
        {
            outsideTarget.pop_front();
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
        // Serial.println("Rotate Angle: " + String(crossroadAction.rotateAngle));
        // Serial.println("Now Index: " + String(nowMazeIndex));
        // Serial.println("Next Target: " + String(crossroadAction.nextPosition));
        if (IRReceiver::atCrossroad(crossroadAction.rotateAngle) && !insideTarget.empty())
        {
            if (crossroadAction.rotateAngle == 180)
                motorDirection *= -1;
            else
            {
                AngleControl::target += motorDirection * crossroadAction.rotateAngle;
                if (crossroadAction.rotateAngle != 0)
                    motorDirection = 1;
            }
            lastMazeIndex = nowMazeIndex;
            nowMazeIndex = crossroadAction.nextPosition;
            if (nowMazeIndex == insideTarget.front()) insideTarget.pop_front();
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