#include "IRReceiver.h"
#include "MotorControl.h"
#include "statemachine.h"

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
    midWeight[0] = 1;  // * 0
    midWeight[1] = 1;  // * 1
    midWeight[2] = -1; // * 1
    midWeight[3] = -1; // * 0
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
            midCount += (midValue[i] == MID_DETECT);

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

int IRReceiver::angleOffset()
{
    int offset = 0;
    for (int i = 0; i < MID_IR_COUNT; ++i)
        offset += midWeight[i] * (midValue[i] == MID_DETECT);
    // if (StateMachine::getInstance().motorDirection == -1)
    //     offset = 0;
    return offset;
}

int IRReceiver::leftValue[SIDE_IR_COUNT];
int IRReceiver::rightValue[SIDE_IR_COUNT];
int IRReceiver::midValue[MID_IR_COUNT];
int IRReceiver::midWeight[MID_IR_COUNT];
bool IRReceiver::atCross = false;
bool IRReceiver::backFlag = true;