#include <JY61.h>
#include <PID_v1.h>
#include <TimerInterrupt.h>
#include <math.h>
#include "AngleControl.h"

void AngleControl::initialize()
{
	pid.SetMode(AUTOMATIC);
	pid.SetSampleTime(timePeriod);
	pid.SetOutputLimits(-100, 100);

	JY61::read();
	initAngle = JY61::Angle[2];
	target = initAngle;
}
void AngleControl::setTarget(double targetAngle)
{
	AngleControl::target = targetAngle;
	AngleControl::target = fmod(AngleControl::target, 360) - 180;
}
double AngleControl::getTarget() { return target; }
double AngleControl::getOutput() { return output; }
bool AngleControl::Compute()
{
	int k = floor((target + 180.0 - JY61::Angle[2]) / 360.0);
	JY61::Angle[2] += k * 360;
	return pid.Compute();
}

double AngleControl::output;
double AngleControl::target;
double AngleControl::initAngle;
PID AngleControl::pid = PID(&JY61::Angle[2], &output, &target, 1.5, 0.000000001, 0.0000001, DIRECT);
