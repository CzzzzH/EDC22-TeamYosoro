#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "util.h"
#include "information.h"

class StateMachine // The statemachine of the car (Singleton)
{
private:
    StateMachine() {}
    StateMachine& operator=(const StateMachine&)=delete;
public:

    // Attributes
    Action nowAction;

    // Methods
    ~StateMachine() {}
    static StateMachine& getInstance();
    
    void process();
    void updateInfo();
    void updateMission();
    void updateAction();
    void updateMotor();

};

#endif