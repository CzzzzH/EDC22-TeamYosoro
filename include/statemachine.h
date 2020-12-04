#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "util.h"
#include "information.h"
#include <deque>

// 各种Define，用于debug
// #define USE_ZIGBEE
// #define DEBUG_MOTOR
// #define DEBUG_ANGLECONTROLER
#define DEBUG_IRRECEIVER
// #define DEBUG_ZIGBEE
// #define DEBUG_TIMER
// #define DEBUG_POSITION
// #define DEBUG_MAZE_POS
// #define DEBUG_CROSS_ACTION

class StateMachine // The statemachine of th2e car (Singleton)
{
public:
    static Match nowHalf;
    static Mission nowMission;
    static Position lastPosition;
    static Position nowPosition;
    static CrossroadAction crossroadAction;
    static unsigned int midLine;
    static int nowMazeIndex, nextMazeIndex;
    static int targetTransportCount;
    static int offset;
    static int motorDirection;
    static uint16_t backTime;
    static int lastScore, nowScore;
    static int counter;
    static int turnAngle;

    static unsigned long lastCrossTime, nowCrossTime;

    static bool havePatient;
    static bool getItems;
    static bool restart;
    static bool addNew;
    static bool stop;

    static std::deque<Position> outsideTarget;
    static std::deque<int> insideTarget;

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