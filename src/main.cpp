#include <Arduino.h>
#include "statemachine.h"
#include "TimerInterrupt.h"
#include "AngleControl.h"
#include "servoCtl.h"
#include "JY61.h"
#include "MotorControl.h"

std::list<TimerInterrupt *>
	TimerInterrupt::timer_list = std::list<TimerInterrupt *>();

TimerInterrupt angleTimer(AngleControl::timePeriod / interrupt_period, [] {
	JY61::read();
	if (AngleControl::Compute())
	{
		// Serial.println("read angle : " + String(JY61::Angle[2]));
		// Serial.println("target angle : " + String(AngleControl::getTarget()));
		// Serial.println("angle output :　" + String(AngleControl::getOutput()));
		servoCtl::myServo.write(AngleControl::middle - AngleControl::getOutput());
	}
});

TimerInterrupt motorTimer(Motor::timePeriod / interrupt_period, [] {
	StateMachine::getInstance().process();
});

StateMachine &sm = StateMachine::getInstance();

void setup()
{
	sm.init();
	// TimerInterrupt::initialize(interrupt_period);
}

void loop()
{
	// sm.updateInfo(Information::getInstance());
}
