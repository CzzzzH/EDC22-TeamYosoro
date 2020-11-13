#include "IRReceiver.h"
#include "MotorControl.h"
#include "statemachine.h"
#include "information.h"

#define ZIGBEE_OFFSET 0.1

void IRReceiver::initialize()
{
    memset(leftValue, 0, sizeof(leftValue));
    memset(midValue, 0, sizeof(midValue));
    memset(rightValue, 0, sizeof(rightValue));
    for (int i = 0; i < SIDE_IR_COUNT; i++)
    {
        pinMode(LEFT_BEGIN + i, INPUT);
        pinMode(RIGHT_BEGIN + i, INPUT);
    }
    for (int i = 0; i < MID_IR_COUNT; i++)
        pinMode(MID_BEGIN + i, INPUT);

    midWeight[0] = 1.5;  // * 0
    midWeight[1] = 1;  // * 0 
    midWeight[2] = 0.5; // * 1
    midWeight[3] = -0.5; // * 1
    midWeight[4] = -1; // * 0
    midWeight[5] = -1.5; // * 0
}

void IRReceiver::updateValue()
{
    for (int i = 0; i < SIDE_IR_COUNT; ++i)
    {
        leftValue[i] = digitalRead(LEFT_BEGIN + i);
        rightValue[i] = digitalRead(RIGHT_BEGIN + i);
    }
    for (int i = 0; i < MID_IR_COUNT; ++i)
        midValue[i] = digitalRead(MID_BEGIN + i);
    // if (leftValue[2] == SIDE_DETECT) leftBack = true;
    // if (rightValue[2] == SIDE_DETECT) rightBack = true;    
}

bool IRReceiver::atCrossroad(int angle)
{
    if (atCross && (leftValue[1] == SIDE_DETECT || rightValue[1] == SIDE_DETECT))
    {
        atCross = false;
        Motor::targetSpeed = 30;
        return StateMachine::getInstance().motorDirection == 1;
    }
    else if (!atCross)
    {
        int midCount = 0;
        for (int i = 0; i < MID_IR_COUNT; ++i)
            midCount += ( (i < 2 || i > 3) && midValue[i] == MID_DETECT);

        if (StateMachine::getInstance().motorDirection == 1)
        {
            if (midCount >= 3)
            {
                atCross = true;
                if (angle == 90 || angle == -90)
                    Motor::targetSpeed = 30;
                else
                    Motor::targetSpeed = 30;
            }
        }
        else if (StateMachine::getInstance().motorDirection == -1)
        {   
            if (midCount >= 3)
            {
                atCross = true;
                if (backFlag)
                {
                    Motor::targetSpeed = 30;
                    StateMachine::getInstance().motorDirection = 1;
                    StateMachine::getInstance().lastMazeIndex = 2 * StateMachine::getInstance().nowMazeIndex - StateMachine::getInstance().lastMazeIndex;
                }
                else backFlag = true;
            }
        }
    }
    return false;
}

double IRReceiver::angleOffset()
{   
    StateMachine &sm = StateMachine::getInstance();
    double offset = 0;
    if (sm.nowMission == SEARCH_MAZE || sm.nowMission == GO_OUT_MAZE)
    {
        for (int i = 0; i < MID_IR_COUNT; ++i)
            offset += midWeight[i] * (midValue[i] == MID_DETECT);
    }
    else if (sm.nowMission == GO_TO_MAZE)
    {
        if (sm.outsideTarget.size() == 2)
        {
            if (sm.nowPosition.X > sm.midLine + 1) offset = ZIGBEE_OFFSET;
            else if (sm.nowPosition.X < sm.midLine - 1) offset = ZIGBEE_OFFSET;
        }
        // else if (sm.outsideTarget.size() == 1)
        // {
        //     if (sm.nowPosition.Y > 240) offset = ZIGBEE_OFFSET;
        //     else if (sm.nowPosition.Y < 236) offset = -ZIGBEE_OFFSET;
        // }
    }
    else if (sm.nowMission == RETURN)
    {   
        if (sm.outsideTarget.size() == 1)
        {
            if (sm.nowPosition.Y > sm.midLine + 1) offset = -ZIGBEE_OFFSET;
            else if (sm.nowPosition.Y < sm.midLine -1) offset = ZIGBEE_OFFSET;
        }
    }
    return offset;
}

int IRReceiver::leftValue[SIDE_IR_COUNT];
int IRReceiver::rightValue[SIDE_IR_COUNT];
int IRReceiver::midValue[MID_IR_COUNT];
double IRReceiver::midWeight[MID_IR_COUNT];
bool IRReceiver::atCross = false;
bool IRReceiver::backFlag = true;