#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "util.h"
#include "information.h"
#include <vector>
class StateMachine // The statemachine of th2e car (Singleton)
{
private:
    StateMachine() {}
    StateMachine &operator=(const StateMachine &) = delete;

public:
    // Attributes
    Mission nowMission;
    Direction nowDirection;
    MazePosition nowMazePosition;
    int nowTargetIndex = 0;
    int counter = 0;
    
    std::vector<Position> outsideTarget;
    std::vector<MazePosition> insideTarget;

    // Methods
    ~StateMachine() {}
    static StateMachine &getInstance();

    void init();
    void process();
    void updateInfo(Information &info);
    void updateMission(Information &info);
    void updateAction(Information &info);
    void updateMotor(Information &info);
    void turnInMaze(int dir);
};

#endif