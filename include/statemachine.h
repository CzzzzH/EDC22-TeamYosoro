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
    unsigned int midLine = 0;
    int lastMazeIndex, nowMazeIndex;
    int targetTransportCount = 0;
    int counter = 0;
    int offset = 0;
    int motorDirection = 1;
    int backTime = 0;
    
    std::deque<Position> outsideTarget;
    std::deque<int> insideTarget;

    // Methods
    ~StateMachine() {}
    static StateMachine &getInstance();

    void init();
    void process();
    void updateInfo();
    void updateMission(Information &info);
    void updateAction(Information &info);
    void updateMotor(Information &info);
};

#endif