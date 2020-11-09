#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "util.h"
#include "information.h"
#include <queue>
class StateMachine // The statemachine of th2e car (Singleton)
{
private:
    StateMachine() {}
    StateMachine &operator=(const StateMachine &) = delete;

public:
    // Attributes
    Mission nowMission;
    int lastMazeIndex, nowMazeIndex;
    int counter = 0;
    int offset = 0;
    
    std::queue<Position> outsideTarget;
    std::queue<int> insideTarget;

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