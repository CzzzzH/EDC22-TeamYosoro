#ifndef MOTOR_CONTROL_HPP
#define MOTOR_CONTROL_HPP

#include <Arduino.h>
#include <PID_v1.h>
#include <Encoder.h>

struct pin_2
{
	int A;
	int B;
};

class encoder
{
public:
	const static pin_2 left_pin;
	const static pin_2 right_pin;

	static struct
	{
		double left;
		double right;
	} counter;
};

const pin_2 encoder::left_pin = {21, 20};
const pin_2 encoder::right_pin = {19, 18};
decltype(encoder::counter) encoder::counter;

Encoder rightEnc(encoder::right_pin.A, encoder::right_pin.B);
Encoder leftEnc(encoder::left_pin.A, encoder::left_pin.B);

void EncoderRead()
{
	encoder::counter.right = rightEnc.read();
	encoder::counter.left = leftEnc.read();
}

void EncoderReset()
{
	rightEnc.write(0);
	leftEnc.write(0);
}

class Motor
{
public:
	const static pin_2 left_pin;
	const static pin_2 right_pin;
	static bool isDebug;

	static void initialize()
	{
		pinMode(left_pin.A, OUTPUT);
		pinMode(left_pin.B, OUTPUT);

		pinMode(right_pin.A, OUTPUT);
		pinMode(right_pin.B, OUTPUT);

		// Encoder input
		pinMode(encoder::left_pin.A, INPUT);
		pinMode(encoder::left_pin.B, INPUT);
		pinMode(encoder::right_pin.A, INPUT);
		pinMode(encoder::right_pin.B, INPUT);
	}

	static void setLeftPWM(int pwm)
	{
		pwm = std::min(pwm, 255);
		pwm = std::max(pwm, -255);
		if (isDebug)
		{
			Serial.print("setting left pwm : ");
			Serial.println(pwm);
		}
		if (pwm > 0)
		{
			analogWrite(left_pin.A, pwm);
			analogWrite(left_pin.B, 0);
		}
		else
		{
			analogWrite(left_pin.A, 0);
			analogWrite(left_pin.B, -pwm);
		}
	}

	static void setRightPWM(int pwm)
	{
		pwm = std::min(pwm, 255);
		pwm = std::max(pwm, -255);
		if (isDebug)
		{
			Serial.print("setting right pwm : ");
			Serial.println(pwm);
		}
		if (pwm > 0)
		{
			analogWrite(right_pin.A, pwm);
			analogWrite(right_pin.B, 0);
		}
		else
		{
			analogWrite(right_pin.A, 0);
			analogWrite(right_pin.B, -pwm);
		}
	}
};

const pin_2 Motor::left_pin = {12, 13};
const pin_2 Motor::right_pin = {6, 7};
bool Motor::isDebug = false;

const int motor_time_interval = 50;
const double basePWM = 140;
double right_Output = 0;
double left_Output = 0;
double targetSpeed = 0;

PID rightPID(&encoder::counter.right, &right_Output, &targetSpeed, 1, 0.00001, 0.00001, DIRECT);
PID leftPID(&encoder::counter.left, &left_Output, &targetSpeed, 1, 0.00001, 0.00001, DIRECT);

void PID_initialize()
{
	leftPID.SetMode(AUTOMATIC);
	leftPID.SetSampleTime(motor_time_interval);
	leftPID.SetOutputLimits(-100, 100);
	rightPID.SetMode(AUTOMATIC);
	rightPID.SetSampleTime(motor_time_interval);
	rightPID.SetOutputLimits(-100, 100);
}

void PID_compute()
{
	leftPID.Compute();
	rightPID.Compute();
}

#endif