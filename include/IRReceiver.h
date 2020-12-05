#include <Arduino.h>
#include <PID_v1.h>

// From left to right
#define AHEAD_SPEED 45
#define TURN_SPEED 25
#define SLOW_SPEED 2

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
private:
    static void updateOffset();
    static PID offsetPid;

public:
    static uint8_t rightFrontValue, leftFrontValue, rightBackValue, leftBackValue;
    static uint8_t midCount, turnCount, restartTime;
    static uint8_t totalMidValue[MID_IR_COUNT], midValue[MID_IR_COUNT];
    static uint8_t totalMidBackValue[MID_BACK_IR_COUNT], midBackValue[MID_BACK_IR_COUNT];
    static double IROffset, zero;
    static bool slowLeft, slowRight, slow;
    static bool turn, ahead;
    static void initialize();
    static void updateValue();
    static float compute_weight(uint8_t index, uint8_t total_count, float slope);
    static bool atCrossroad(int16_t angle);
    static double IRPidResult;
};