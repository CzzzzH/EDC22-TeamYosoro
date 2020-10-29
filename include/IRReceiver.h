#include <Arduino.h>

// From left to right
#define SENSOR_LEFT_OUTNER 22
#define SENSOR_LEFT_INNER 23
#define SENSOR_RIGHT_INNER 24
#define SENSOR_RIGHT_OUTNER 25


class IRReceiver
{
public:
    static int leftOuterValue;
    static int leftInnerValue;
    static int rightInnerValue;
    static int rightOuterValue;
    static void initialize();
    static void updateValue();
    static bool atCrossroad();
    
};