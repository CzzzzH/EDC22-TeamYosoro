#include "IRReceiver.h"
#include "MotorControl.h"

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
}

void IRReceiver::updateValue()
{   
    int midCount = 0;
    int leftCount = 0;
    int rightCount = 0;
    for (int i = 0; i < SIDE_IR_COUNT; ++i)
    {
        leftValue[i] = digitalRead(LEFT_BEGIN + i); 
        rightValue[i] = digitalRead(RIGHT_BEGIN + i);
        leftCount += (leftValue[i] == SIDE_DETECT);
        rightCount += (rightValue[i] == SIDE_DETECT);
    }
    for (int i = 0; i < MID_IR_COUNT; ++i)
    {
        midValue[i] = digitalRead(MID_BEGIN + i);
        midCount += (midValue[i] == MID_DETECT);
    }
    if (midCount >= 3 && (leftCount + rightCount) < 6 && atCross == false)
    {
        atCross = true;
        Motor::targetSpeed -= 10;
    }
}

bool IRReceiver::atCrossroad()
{
    if (atCross && (leftValue[1] == SIDE_DETECT || rightValue[1] == SIDE_DETECT))
    {
        atCross = false;
        Motor::targetSpeed += 10;
        return true;
    }
    else return false;
}

double IRReceiver::angleOffset()
{
    int res = 0;
    int leftCount = 0;
    int rightCount = 0;
    for (int i = 0; i < SIDE_IR_COUNT; ++i)
    {
        leftCount += leftValue[i];
        rightCount += rightValue[i];
    }
    for (int i = 0; i < MID_IR_COUNT; ++i)
    {
        if (i < MID_IR_COUNT / 2) rightCount += midValue[i];
        else leftCount += midValue[i];
    }
    res += std::max(0, leftCount - rightCount - NON_SENSITIVITY);
    return res;
}

int IRReceiver::leftValue[SIDE_IR_COUNT];
int IRReceiver::rightValue[SIDE_IR_COUNT];
int IRReceiver::midValue[MID_IR_COUNT];
bool IRReceiver::atCross = false;