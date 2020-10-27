#include <Arduino.h>
#include "statemachine.h"
#include "information.h"
#include <MsTimer2.h>
#include <string.h>
#include "TimerInterrupt.hpp"
#include "motor_control.hpp"
#include <SoftwareSerial.h>
// #include <JY61.hpp>
#include <servoCtl.h>
#include <AngleControl.hpp>

StateMachine &sm = StateMachine::getInstance();

TimerInterrupt timer1(motor_time_interval / interrupt_period, [] {
	EncoderRead();

	// Serial.print("right:  ");
	// Serial.println(encoder::counter.right);
	// Serial.print("\t\tleft:   ");
	// Serial.println(encoder::counter.left);

	PID_compute();

	Motor::setRightPWM(basePWM + right_Output);
	Motor::setLeftPWM(basePWM + left_Output);

	EncoderReset();
});

void setup()
{
	Serial.begin(9600);
	Serial1.begin(115200);
	Serial2.begin(115200);
	JY61::isDebug = true;

	Motor::initialize();
	Motor::isDebug = false;

	//Timer Interrupt 10ms
	TimerInterrupt::initialize(interrupt_period);

	PID_initialize();

	angleInitialize();

	servoCtl::initialize(4);

	JY61::read();
	initAngle = JY61::Angle[2];
}

void loop()
{
	targetSpeed = 29;
	targetAngle = initAngle;
	// for (int i = 20; i <= 160; i += 2)
	// {
	// 	servoCtl::angleReach(i);
	// 	// delay(200);
	// }
	// for (int i = 160; i >= 20; i -= 2)
	// {
	// 	servoCtl::angleReach(i);
	// 	// delay(200);
	// }

	delay(1000);

	// targetAngle = 90;
	// while (true)
	// {
	// }
}
