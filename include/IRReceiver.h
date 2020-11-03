#include <Arduino.h>

// From left to right
#define IR_PIN_BEGIN 22
#define SENSOR_COUNT 4

class IRReceiver
{
public:
    /*  *** A single IR sensor array. ***
    __       __ __       __
    ||-------||-||-------||

    *** A pair of IR sensor arrays. ***
    A0       A1 A2       A3  A4       A5 A6       A7
    __       __ __       __  __       __ __       __
    ||-------||-||-------||--||-------||-||-------||  
*/
    static int leftValue[SENSOR_COUNT / 2];
    static int rightValue[SENSOR_COUNT / 2];
    static double filter[SENSOR_COUNT];
    static double filterSum;
    static void initialize();
    static void updateValue();
    static bool atCrossroad();
    // static bool turnLeft();
    // static bool turnRight();
};