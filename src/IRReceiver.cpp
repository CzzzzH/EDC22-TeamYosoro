#include "IRReceiver.h"

void IRReceiver::initialize()
{
    memset(leftValue, 0, sizeof(leftValue));
    memset(rightValue, 0, sizeof(rightValue));
    filterSum = 0;
    for (int count = 0; count < SENSOR_COUNT; count++)
        pinMode(IR_PIN_BEGIN + count, INPUT);
    for (int count = 0; count < SENSOR_COUNT; count++)
    {
        filter[count] = abs(SENSOR_COUNT / 2 - 0.5 - count);
        filterSum += filter[count];
    }
}

void IRReceiver::updateValue()
{
    for (int count = 0; count < SENSOR_COUNT / 2; count++)
    {
        rightValue[count] = 1 - digitalRead(IR_PIN_BEGIN + count);
        leftValue[count] = 1 - digitalRead(IR_PIN_BEGIN + count + SENSOR_COUNT / 2);
    }
    // Serial.println(String(leftValue[0]) + " " + String(leftValue[1]) + " " + String(rightValue[0]) + " " + String(rightValue[1]));
}

bool IRReceiver::atCrossroad()
{
    double sum = 0;
    for (int count = 0; count < SENSOR_COUNT / 2; count++)
        sum += leftValue[count] * filter[count] + rightValue[count] * filter[count + SENSOR_COUNT / 2];
    sum /= filterSum;
    return (sum * sum) > 0.5;
}

int IRReceiver::leftValue[SENSOR_COUNT / 2];
int IRReceiver::rightValue[SENSOR_COUNT / 2];
double IRReceiver::filter[SENSOR_COUNT];
double IRReceiver::filterSum;