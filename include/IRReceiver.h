#include <Arduino.h>

// From left to right
#define AHEAD_SPEED 23
#define TURN_SPEED 17
#define SLOW_SPEED 3

#define RIGHT_FRONT 49
#define LEFT_FRONT 48
#define RIGHT_BACK 46
#define LEFT_BACK 47

#define MID_BEGIN 22
#define MID_BACK_BEGIN 38
#define MID_IR_COUNT 16
#define MID_BACK_IR_COUNT 8

#define IR_DETECT 1

class IRReceiver
{
public:
    static int rightFrontValue, leftFrontValue, rightBackValue, leftBackValue;
    static int midCount, restartTime;
    static int totalMidValue[MID_IR_COUNT], midValue[MID_IR_COUNT];
    static int totalMidBackValue[MID_BACK_IR_COUNT], midBackValue[MID_BACK_IR_COUNT];
    static double midWeight[MID_IR_COUNT];
    static double midBackWeight[MID_BACK_IR_COUNT];
    static bool slowLeft, slowRight;
    static bool turn, ahead;
    static void initialize();
    static void updateValue();
    static bool atCrossroad(int angle);
    static double angleOffset();
};