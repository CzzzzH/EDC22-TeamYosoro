#include <Arduino.h>

// From left to right
#define LEFT_BEGIN 38
#define MID_BEGIN 22
#define MID_BACK_BEGIN 43
#define RIGHT_BEGIN 39
#define MID_IR_COUNT 16
#define MID_BACK_IR_COUNT 6
#define IR_DETECT 1

class IRReceiver
{
public:

    static bool leftBack, rightBack;
    static int leftValue, rightValue, midCount, restartTime;
    static int totalMidValue[MID_IR_COUNT], midValue[MID_IR_COUNT];
    static int totalMidBackValue[MID_BACK_IR_COUNT], midBackValue[MID_BACK_IR_COUNT];
    static double midWeight[MID_IR_COUNT];
    static double midBackWeight[MID_BACK_IR_COUNT];
    static bool turn, ahead;
    static void initialize();
    static void updateValue();
    static bool atCrossroad(int angle);
    static double angleOffset();
};