#include "IRReceiver.h"

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
    for (int i = 0; i < SIDE_IR_COUNT; ++i)
    {
        leftValue[i] = digitalRead(LEFT_BEGIN + i); 
        rightValue[i] = digitalRead(RIGHT_BEGIN + i); 
    }
    for (int i = 0; i <  MID_IR_COUNT; ++i)
        midValue[i] = digitalRead(MID_BEGIN + i); 

    if (leftPointer < SIDE_IR_COUNT && leftValue[leftPointer] == SIDE_DETECT) leftPointer++;
    if (rightPointer < SIDE_IR_COUNT && rightValue[rightPointer] == SIDE_DETECT) rightPointer++;
}

bool IRReceiver::atCrossroad()
{
    if (leftPointer == SIDE_IR_COUNT && rightPointer == SIDE_IR_COUNT)
    {
        leftPointer = 0;
        rightPointer = 0;
        return true;
    }
    return false;
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
    res += max(0, leftCount - rightCount - NON_SENSITIVITY);
    return res;
}

int IRReceiver::leftValue[SIDE_IR_COUNT];
int IRReceiver::rightValue[SIDE_IR_COUNT];
int IRReceiver::midValue[MID_IR_COUNT];
int IRReceiver::rightPointer = 0;
int IRReceiver::leftPointer = 0;