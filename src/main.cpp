#include <Arduino.h>
#include "statemachine.h"
#include "TimerInterrupt.h"
#include "AngleControl.h"
#include "servoCtl.h"
#include "JY61.h"
#include "MotorControl.h"
#include "MPU9250.h"
#include <string.h>

std::list<TimerInterrupt *>
	TimerInterrupt::timer_list = std::list<TimerInterrupt *>();

TimerInterrupt angleTimer(AngleControl::timePeriod, [] {
	JY61::read();
	if (AngleControl::Compute())
	{
		servoCtl::myServo.write(AngleControl::middle - AngleControl::getOutput());
		Motor::updatePWM();
	}
	Serial.println("mills : " + String(millis()));
});

TimerInterrupt motorTimer(Motor::timePeriod, [] {
	StateMachine::getInstance().process();
});

// TimerInterrupt timer(1000, [] { Serial.println("aaa"); });

StateMachine &sm = StateMachine::getInstance();

// MPU9250 mpu;

void setup()
{
	sm.init();
	// Serial.begin(9600);
	// Wire.begin();

	// delay(1000);
	// mpu.setup();

	// delay(1000);

	// // calibrate anytime you want to
	// mpu.calibrateAccelGyro();
	// mpu.calibrateMag();

	// mpu.printCalibration();
}

void loop()
{
    sm.updateInfo(Information::getInstance());
}
