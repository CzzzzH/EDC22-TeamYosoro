#ifndef ANGLECONTROL_HPP
#define ANGLECONTROL_HPP

#include <JY61.hpp>
#include <PID_v1.h>
#include <TimerInterrupt.hpp>

double angleOutput = 0;
double targetAngle = 0;
double initAngle = 0;
double initSpeed = 0;
double baseAngle = 85;
const int angle_time_period = 20;
PID anglePID(&JY61::Angle[2], &angleOutput, &targetAngle, 1.5, 0, 0.0000001, DIRECT);

TimerInterrupt angleTimer(angle_time_period / interrupt_period, [] {
	if (anglePID.Compute())
	{
		JY61::read();
		// Serial.print("angleOutput:　");
		// Serial.println(angleOutput);
		servoCtl::myServo.write(baseAngle - angleOutput);
	}
});

void angleInitialize()
{
	anglePID.SetMode(AUTOMATIC);
	anglePID.SetSampleTime(angle_time_period);
	anglePID.SetOutputLimits(-100, 100);
}

#endif