#include <JY61.h>
#include <PID_v1.h>
#include <TimerInterrupt.h>
#include <math.h>
#include "AngleControl.h"
#include <Arduino.h>

void AngleControl::initialize()
{
	pid.SetMode(AUTOMATIC);
	pid.SetSampleTime(timePeriod);
	pid.SetOutputLimits(-100, 100);

	JY61::read();
	delay(100);
	JY61::read();
	initAngle = JY61::Angle[2];
	target = initAngle;
}
double AngleControl::getOutput() { return output; }
bool AngleControl::Compute()
{
	Serial.println("JY61::Angle[2]   :   " + String(JY61::Angle[2]));
	JY61::Angle[2] += floor((target + 180 - JY61::Angle[2]) / 360.0) * 360;
	return pid.Compute();
}

double AngleControl::output;
double AngleControl::target;
double AngleControl::initAngle;
PID AngleControl::pid = PID(&JY61::Angle[2], &output, &target, 1, 0.00000000001, 0.000000001, DIRECT);
