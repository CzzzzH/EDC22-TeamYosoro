#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "util.h"
#include "information.h"
#include <queue>
#include <vector>
class StateMachine // The statemachine of th2e car (Singleton)
{
private:
    StateMachine() {}
    StateMachine &operator=(const StateMachine &) = delete;

public:
    // Attributes
    Match nowHalf;
    Mission nowMission;
    Action nowAction;
    int lastMazeIndex, nowMazeIndex;
    int counter = 0;
    int offset = 0;
    int motorDirection = 1;
    
    std::queue<Position> outsideTarget;
    std::vector<int> insideTarget;

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