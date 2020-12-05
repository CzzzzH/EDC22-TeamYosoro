#ifndef ANGLECONTROL_HPP
#define ANGLECONTROL_HPP

#include <PID_v1.h>

class AngleControl
{
private:
	static double output;
	static double initAngle;
    static double angleDist;
	static PID pid;

public:
	const static int timePeriod = 10;
	const static int middle = 85;
	static double target;
	static void initialize();
	static double getOutput();
	static bool Compute();
    static double getAngleDist();
};

#endif