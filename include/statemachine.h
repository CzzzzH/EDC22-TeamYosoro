#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "util.h"
#include "information.h"
#include <deque>

class StateMachine // The statemachine of th2e car (Singleton)
{
private:
    StateMachine() {}
    StateMachine &operator=(const StateMachine &) = delete;

public:
    // Attributes
    Match nowHalf;
    Mission nowMission;
    Position lastPosition = {0, 0};
    Position nowPosition = {0, 0};
    CrossroadAction crossroadAction;
    unsigned int midLine = 0;
    int lastMazeIndex, nowMazeIndex;
    int targetTransportCount = 0;
    int offset = 0;
    int motorDirection = 1;
    uint16_t backTime = 0;
    
    unsigned long lastCrossTime, nowCrossTime;

    bool havePatient = false;
    bool getItems = false;
    
    std::deque<Position> outsideTarget;
    std::deque<int> insideTarget;

    // Methods
    ~StateMachine() {}
    static StateMachine &getInstance();

    void init();
    void process();
    void exceptionHandle();
    void updateInfo();
    void updateMission(Information &info);
    void updateAction(Information &info);
    void updateMotor(Information &info);
};

#endif