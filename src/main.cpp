#include <Arduino.h>
#include "statemachine.h"
#include "TimerInterrupt.h"
#include "AngleControl.h"
#include "JY61.h"
#include "MotorControl.h"
#include "IRReceiver.h"

#define INTERRUPT_INTERVAL 50

std::list<TimerInterrupt *>
	TimerInterrupt::timer_list = std::list<TimerInterrupt *>();

TimerInterrupt motorTimer(INTERRUPT_INTERVAL, [] {
	StateMachine::getInstance().process();
});

TimerInterrupt angleTimer(10, [] {
	// Serial.println("mills : " + String(millis()));
	IRReceiver::updateValue();
	JY61::read();
	AngleControl::Compute();
	Motor::updatePWM();
});

StateMachine &sm = StateMachine::getInstance();

void setup()
{
	sm.init();
}

void loop()
{
}
