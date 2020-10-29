#include "IRReceiver.h"

void IRReceiver::initialize()
{
    pinMode(SENSOR_LEFT_OUTNER, INPUT);
    pinMode(SENSOR_LEFT_INNER, INPUT);
    pinMode(SENSOR_RIGHT_INNER, INPUT);
    pinMode(SENSOR_RIGHT_OUTNER, INPUT);
}

void IRReceiver::updateValue()
{
    leftOuterValue = digitalRead(SENSOR_LEFT_OUTNER);
    leftInnerValue = digitalRead(SENSOR_LEFT_INNER);
    rightInnerValue = digitalRead(SENSOR_RIGHT_INNER);
    rightOuterValue = digitalRead(SENSOR_RIGHT_OUTNER);
}

bool IRReceiver::atCrossroad()
{
    return leftOuterValue & leftInnerValue & rightInnerValue & rightOuterValue;
}

int IRReceiver::leftOuterValue = 0;
int IRReceiver::leftInnerValue = 0;
int IRReceiver::rightInnerValue = 0;
int IRReceiver::rightOuterValue = 0;