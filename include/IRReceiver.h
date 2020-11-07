#include <Arduino.h>

// From left to right
#define LEFT_BEGIN 25
#define MID_BEGIN 28
#define RIGHT_BEGIN 22
#define SIDE_IR_COUNT 3
#define MID_IR_COUNT 4
#define MID_DETECT 0
#define SIDE_DETECT 1
#define NON_SENSITIVITY 0

class IRReceiver
{
public:

    static int leftValue[SIDE_IR_COUNT];
    static int midValue[MID_IR_COUNT];
    static int rightValue[SIDE_IR_COUNT];
    static int leftPointer, rightPointer;
    static bool atCross;
    static void initialize();
    static void updateValue();
    static bool atCrossroad();
    static double angleOffset();
};