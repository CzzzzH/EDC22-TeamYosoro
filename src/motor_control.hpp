#include <Arduino.h>

const int left_back_pin_A = 6;
const int left_back_pin_B = 7;

const int left_front_pin_A = 8;
const int left_front_pin_B = 9;

const int right_back_pin_A = 10;
const int right_back_pin_B = 11;

const int right_front_pin_A = 12;
const int right_front_pin_B = 13;

const int left_back_encoder_A = 21;
const int left_back_encoder_B = 20;

const int right_back_encoder_A = 19;
const int right_back_encoder_B = 18;

double left_encoder_counter = 0;
double right_encoder_counter = 0;

void initialize_motor_control_pin()
{
	pinMode(left_back_pin_A, OUTPUT);
	pinMode(left_back_pin_B, OUTPUT);

	pinMode(left_front_pin_A, OUTPUT);
	pinMode(left_front_pin_B, OUTPUT);

	pinMode(right_back_pin_A, OUTPUT);
	pinMode(right_back_pin_B, OUTPUT);

	pinMode(right_front_pin_A, OUTPUT);
	pinMode(right_front_pin_B, OUTPUT);

	// Encoder input
	pinMode(left_back_encoder_A, INPUT);
	pinMode(left_back_encoder_B, INPUT);
	pinMode(right_back_encoder_A, INPUT);
	pinMode(right_back_encoder_B, INPUT);

	// External Interrupt
	attachInterrupt(
		2, [] { left_encoder_counter++; }, RISING);
	attachInterrupt(
		4, [] { right_encoder_counter++; }, RISING);
}

void setLeftPWM(const int &pwm)
{
	if (pwm > 0)
	{
		analogWrite(left_back_pin_A, pwm);
		analogWrite(left_back_pin_B, 0);

		analogWrite(left_front_pin_A, pwm);
		analogWrite(left_front_pin_B, 0);
	}
	else
	{
		analogWrite(left_back_pin_A, 0);
		analogWrite(left_back_pin_B, -pwm);

		analogWrite(left_front_pin_A, 0);
		analogWrite(left_front_pin_B, -pwm);
	}
}

void setRightPWM(const int &pwm)
{
	if (pwm > 0)
	{
		analogWrite(right_back_pin_A, pwm);
		analogWrite(right_back_pin_B, 0);

		analogWrite(right_front_pin_A, pwm);
		analogWrite(right_front_pin_B, 0);
	}
	else
	{
		analogWrite(right_back_pin_A, 0);
		analogWrite(right_back_pin_B, -pwm);

		analogWrite(right_front_pin_A, 0);
		analogWrite(right_front_pin_B, -pwm);
	}
}
