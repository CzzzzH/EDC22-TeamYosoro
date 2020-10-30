#include "IRReceiver.h"

void IRReceiver::initialize()
{
    memset(leftValue, 0, sizeof(leftValue));
    memset(rightValue, 0, sizeof(rightValue));
    filterSum = 0;
    for (int count = 0;count < SENSOR_COUNT;count++)
        pinMode(A0 + count, INPUT);
    for (int count = 0;count < SENSOR_COUNT;count++)
    {
        filter[count] = abs(3.5 - count);
        filterSum += filter[count];
    }
}

void IRReceiver::updateValue()
{
    for (int count = 0;count < SENSOR_COUNT / 2;count++)
    {
        leftValue[count] = digitalRead(A0 + count);
        rightValue[count] = digitalRead(A0 + count + 4);
    }
}

bool IRReceiver::atCrossroad()
{
    double sum = 0;
    for (int count = 0;count < SENSOR_COUNT / 2;count ++)
        sum += leftValue[count] * filter[count] + rightValue[count] * filter[count + 4];
    sum /= filterSum;
    return (sum * sum) > 0.5;
}