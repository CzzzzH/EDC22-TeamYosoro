#include "AngleControl.h"
#include <Arduino.h>
#include <JY61.h>
#include <PID_v1.h>
#include <TimerInterrupt.h>
#include <math.h>

void AngleControl::initialize()
{
    pid.SetMode(AUTOMATIC);
    pid.SetSampleTime(timePeriod);
    pid.SetOutputLimits(-100, 100);

    JY61::read();
    delay(500);
    JY61::read();

    initAngle = JY61::Angle[2];
    target = initAngle;
}

double AngleControl::getOutput()
{
    return output;
}

double AngleControl::getAngleDist()
{
    return angleDist;
}

bool AngleControl::Compute()
{
    // Serial.println("JY61::Angle[2]   :   " + String(JY61::Angle[2]));
    JY61::Angle[2] += floor((target + 180 - JY61::Angle[2]) / 360.0) * 360;
    angleDist = fabs(JY61::Angle[2] - target);
    return pid.Compute(false);
}

double AngleControl::output;
double AngleControl::target;
double AngleControl::initAngle;
double AngleControl::angleDist;
PID AngleControl::pid = PID(&JY61::Angle[2], &output, &target, 1, 0.0000000, 0.00001, DIRECT);
