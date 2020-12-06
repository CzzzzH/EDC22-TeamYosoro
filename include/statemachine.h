#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "util.h"
#include "information.h"
#include <deque>

// 各种Define，用于debug
#define USE_ZIGBEE
// #define DEBUG_MOTOR
// #define DEBUG_ANGLECONTROLER
// #define DEBUG_IRRECEIVER
// #define DEBUG_ZIGBEE
// #define DEBUG_TIMER
#define DEBUG_POSITION
#define DEBUG_MAZE_POS
#define DEBUG_CROSS_ACTION

class StateMachine // The statemachine of th2e car (Singleton)
{
public:
    static Match nowHalf;
    static Mission nowMission;
    static Position lastPosition;
    static Position nowPosition;
    static CrossroadAction crossroadAction;
    static int8_t midLine;
    static int8_t nowMazeIndex, nextMazeIndex;
    static int8_t motorDirection;
    static uint16_t backTime;
    static int16_t lastScore, nowScore;
    static uint16_t counter;

    static uint32_t lastCrossTime, nowCrossTime;

    static bool havePatient;
    static bool getItems;
    static bool restart;
    static bool addNew;
    static bool stop;

    static std::deque<Position> outsideTarget;
    static std::deque<int8_t> insideTarget;

    // Methods
    static void init();
    static void process();
    static void exceptionHandle();
    static void updateInfo();
    static void updateMission();
    static void updateAction();
    static void updateMotor();
    static void printDebugInfo();
    static void interruptionFunction();
};

#endif