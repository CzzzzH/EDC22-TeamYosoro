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
#include "LED.h"

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

    // Initialize Serial
    Serial.begin(9600);
    Serial2.begin(115200);
    Serial3.begin(115200);

    // Initialization before game start
    Motor::initialize();
    Motor::targetSpeed = 0;
    AngleControl::initialize();
    IRReceiver::initialize();
    LED::initialize();

    info.updateInfo();
    while (info.getGameState() != GameGoing)
        info.updateInfo();
    
    midLine = info.getCarposX();
    // Set Half
    if (info.getCartask() == 0) nowHalf = FIRST_HALF;
    else nowHalf = SECOND_HALF;

    Maze::initialize(Information::getInstance());

    if (nowHalf == FIRST_HALF)
    {
        insideTarget.push_back(15);
        backTime = 200;
    }
    else
    {
        backTime = 500;
    }

    // Other Initialization
    outsideTarget.push_back({16, 244});
    outsideTarget.push_back({87, 236});

    // A Simple Path
    // insideTarget.push_back(26);
    // insideTarget.push_back(27);
    // insideTarget.push_back(21);
    // insideTarget.push_back(15);
    // insideTarget.push_back(7);
    // insideTarget.push_back(13);

    // info.Obstacle[0].posA.X = 32;
    // info.Obstacle[0].posA.Y = 56;
    // info.Obstacle[0].posB.X = 83;
    // info.Obstacle[0].posB.Y = 70;

    // info.Obstacle[1].posA.X = 127;
    // info.Obstacle[1].posA.Y = 67;
    // info.Obstacle[1].posB.X = 127;
    // info.Obstacle[1].posB.Y = 97;

    // info.Obstacle[2].posA.X = 127;
    // info.Obstacle[2].posA.Y = 187;
    // info.Obstacle[2].posB.X = 157;
    // info.Obstacle[2].posB.Y = 187;

    // info.Obstacle[3].posA.X = 180;
    // info.Obstacle[3].posA.Y = 126;
    // info.Obstacle[3].posB.X = 180;
    // info.Obstacle[3].posB.Y = 220;

    // info.Obstacle[4].posA.X = 157;
    // info.Obstacle[4].posA.Y = 65;
    // info.Obstacle[4].posB.X = 187;
    // info.Obstacle[4].posB.Y = 67;

    // info.Obstacle[5].posA.X = 52;
    // info.Obstacle[5].posA.Y = 97;
    // info.Obstacle[5].posB.X = 82;
    // info.Obstacle[5].posB.Y = 97;

    // info.Obstacle[6].posA.X = 67;
    // info.Obstacle[6].posA.Y = 127;
    // info.Obstacle[6].posB.X = 67;
    // info.Obstacle[6].posB.Y = 157;

    // info.Obstacle[7].posA.X = 156;
    // info.Obstacle[7].posA.Y = 97;
    // info.Obstacle[7].posB.X = 180;
    // info.Obstacle[7].posB.Y = 97;

    nowMission = SEARCH_MAZE;
    nowMazeIndex = 32;
    lastMazeIndex = 38;
    motorDirection = 1;
    Motor::targetSpeed = 30;

    TimerInterrupt::initialize(interrupt_period);
}

// Execute every clock interruption
void StateMachine::process()
{
    Information &info = Information::getInstance();
    // Serial.println("In Process!");
    updateAction(info);
    updateMission(info);
    updateMotor(info);
    // counter++;

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
    Information &info = Information::getInstance();
    info.updateInfo();
    lastPosition = nowPosition;
    nowPosition = info.getCarpos();
    // Serial.println("Now Half: " + String(nowHalf));
    if (nowHalf == SECOND_HALF && (nowMission == GO_TO_MAZE || nowMission == SEARCH_MAZE))
    {
        // Add pacakges
        for (int i = 0; i < 6; ++i)
        {
            PackageInfo package = info.Package[i];
            if (package.whetherpicked) continue;
            int packageIndex = info.positonTransform(package.pos);
            if (info.indexNotExist(packageIndex))
                insideTarget.push_back(packageIndex);
        }
        if (!info.getCartransport())
        {
            int targetIndex = info.positonTransform(info.Passenger.startpos);
            if (info.indexNotExist(targetIndex))
                insideTarget.push_back(targetIndex);
        }
        else if (info.getCartransport())
        {
            int targetIndex = info.positonTransform(info.Passenger.finalpos);
            if (info.indexNotExist(targetIndex))
                insideTarget.push_back(targetIndex);
        }
    }
}

void StateMachine::updateAction(Information &info)
{
    // TODO: update the current action (use infrared info and zigbee info)
    if (nowMission == GO_TO_MAZE || nowMission == RETURN)
    {
        double distance = nowPosition.getDist(outsideTarget.front());
        // Serial.println("Now Target: " + String(outsideTarget.front().X) + " " + String(outsideTarget.front().Y));
        // Serial.println("Now Pos: " + String(nowPosition.X) + " " + String(nowPosition.Y));
        // Serial.println("Mid Line: " + String(midLine));
        // Serial.println("Now dist: " + String(distance));
        if (distance < 15)
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
    else if (nowMission == SEARCH_MAZE || nowMission == GO_OUT_MAZE)
    {
        Serial.println("InsideTarget: " + String(insideTarget.front()));
        
        if (insideTarget.empty())
        {
            Motor::targetSpeed = 0;
            LED::ledOn();
        }
        else
        {
            Motor::targetSpeed = 30;
            LED::ledOff();
            CrossroadAction crossroadAction = Maze::getDirection(lastMazeIndex, nowMazeIndex, insideTarget);
            if (IRReceiver::atCrossroad(crossroadAction.rotateAngle))
            {
                if (crossroadAction.rotateAngle == 180)
                {
                    motorDirection = -motorDirection;
                    if (motorDirection == -1) IRReceiver::backFlag = false;
                }
                else AngleControl::target += crossroadAction.rotateAngle;
                lastMazeIndex = nowMazeIndex;
                nowMazeIndex = crossroadAction.nextPosition;
                if (nowMazeIndex == insideTarget.front())
                    insideTarget.pop_front();
            }
        }
        
    }
}

void StateMachine::updateMission(Information &info)
{
    // TODO: update the current mission  (use updated info)

    if (nowMission == GO_TO_MAZE && outsideTarget.empty())
        nowMission = SEARCH_MAZE;
    if (nowMission == SEARCH_MAZE && info.getGameTime() > backTime)
    {
        insideTarget.clear();
        insideTarget.push_back(-1);
        nowMission = GO_OUT_MAZE;
    }
    if (nowMission == GO_OUT_MAZE && insideTarget.empty())
    {
        nowMission = END_GAME;
        midLine = nowPosition.Y;
        outsideTarget.push_back({172, 8});
        outsideTarget.push_back({5, 12});
    }
    if (nowMission == RETURN && outsideTarget.empty())
        nowMission = END_GAME;
}

void StateMachine::updateMotor(Information &info)
{
    // TODO: update the motor paramters (use PID)
    if (nowMission == WAIT_FOR_START || nowMission == END_GAME) Motor::targetSpeed = 0;
    else Motor::targetSpeed = fabs(Motor::targetSpeed) * motorDirection;
    Motor::PID_compute();
}