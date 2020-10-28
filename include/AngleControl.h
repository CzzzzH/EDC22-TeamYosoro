#ifndef ANGLECONTROL_HPP
#define ANGLECONTROL_HPP

#include <PID_v1.h>

class AngleControl
{
private:
	static double output;
	static double target;
	static double initAngle;
	static PID pid;

public:
	const static int timePeriod = 20;
	const static int middle = 85;
	static void initialize();
	static void setTarget(double targetAngle);
	static double getTarget();
	static double getOutput();
	static bool Compute();
};

#endif