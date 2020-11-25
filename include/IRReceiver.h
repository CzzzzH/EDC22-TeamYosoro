#include <Arduino.h>

// From left to right
#define LEFT_BEGIN 38
#define MID_BEGIN 22
#define RIGHT_BEGIN 39
#define MID_IR_COUNT 16
#define MID_DETECT 1
#define SIDE_DETECT 1
#define NON_SENSITIVITY 0

class IRReceiver
{
public:

    static bool leftBack, rightBack;
    static int leftValue, rightValue;
    static int midValue[MID_IR_COUNT];
    static double midWeight[MID_IR_COUNT];
    static bool turn, ahead;
    static void initialize();
    static void updateValue();
    static bool atCrossroad(int angle);
    static double angleOffset();
};