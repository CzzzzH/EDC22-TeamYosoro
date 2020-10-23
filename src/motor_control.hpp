#include <Arduino.h>

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

class Motor
{
public:
	const static pin_2 left_pin;
	const static pin_2 right_pin;

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
		Serial.print("setting left pwm : ");
		pwm = std::min(pwm, 255);
		pwm = std::max(pwm, -255);
		Serial.println(pwm);
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
		Serial.print("setting right pwm : ");
		pwm = std::min(pwm, 255);
		pwm = std::max(pwm, -255);
		Serial.println(pwm);
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

const pin_2 Motor::left_pin = {6, 7};
const pin_2 Motor::right_pin = {12, 13};