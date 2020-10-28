#ifndef MOTOR_CONTROL_HPP
#define MOTOR_CONTROL_HPP

#include <Arduino.h>
#include <PID_v1.h>
#include <Encoder.h>
#include <string.h>
#include <string>

struct pin_2
{
	int A;
	int B;
};

class encoder
{
private:
	static Encoder rightEnc;
	static Encoder leftEnc;

public:
	const static pin_2 left_pin;
	const static pin_2 right_pin;

	static struct
	{
		double left;
		double right;
	} counter;

	static void initialize();

	static void Read();
	static void Reset();
};

class Motor
{
private:
	static PID rightPID;
	static PID leftPID;
	static double rightOutput;
	static double leftOutput;

public:
	const static pin_2 left_pin;
	const static pin_2 right_pin;
	const static int timePeriod;
	static bool isDebug;
	static double targetSpeed;

	static void PID_initialize();
	static void initialize();

	static void setPWM(int pwm, bool isRight);

	static void PID_compute();

	static void updateSpeed();

	static double estimatePWM(double targeteSpeed);
};

#endif