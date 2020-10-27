#include <Arduino.h>
#include "statemachine.h"

StateMachine &sm = StateMachine::getInstance();

void setup()
{
	sm.init();
}

void loop()
{
	sm.updateInfo(Information::getInstance());
}
