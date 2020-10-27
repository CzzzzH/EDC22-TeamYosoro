#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "util.h"
#include "information.h"
#include <vector>
class StateMachine // The statemachine of the car (Singleton)
{
private:
    StateMachine() {}
    StateMachine& operator=(const StateMachine&)=delete;
public:

    // Attributes
    State nowState;
    Mission nowMission;
    int nowTargetIndex = 0;
    int counter = -100;
    std::vector<Position> outsideTarget;
    
    // Methods
    ~StateMachine() {}
    static StateMachine& getInstance();
    
    void init();
    void process();
    void updateInfo(Information &info);
    void updateMission();
    void updateAction(Information &info);
    void updateMotor();

};

#endif