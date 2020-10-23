#include <Arduino.h>
#include "statemachine.h"
#include <MsTimer2.h>
#include <string.h>
#include <Encoder.h>
#include "TimerInterrupt.hpp"
#include "motor_control.hpp"
// #include "PID.hpp"
#include <PID_v1.h>

StateMachine &sm = StateMachine::getInstance();

double Output;
double Setpoint;
PID myPID(&right_encoder_counter, &Output, &Setpoint, 1.6, 0.0005, 0.0005, DIRECT);

// PID pid = PID(
// 	0.1, 0.01, 0.01, [](int d) { setRightPWM(d); }, &right_encoder_counter);

Encoder rightEnc(right_back_encoder_A, right_back_encoder_B);
Encoder leftEnc(left_back_encoder_A, left_back_encoder_B);

void setup()
{
	initialize_motor_control_pin();
	Serial.begin(9600);

	//Timer Interrupt 10ms
	MsTimer2::set(10, Interrupt_10ms);
	MsTimer2::start();
	add_10ms([] {
		right_encoder_counter = rightEnc.read();
		left_encoder_counter = leftEnc.read();

		Serial.print("right:  ");
		Serial.println(right_encoder_counter);
		// Serial.print("\t\tleft:   ");
		// Serial.println(left_encoder_counter);

		setRightPWM(Output);

		// encoder
		// Serial.println(right_encoder_counter);
		//Serial.println(encoderB_counter);
		// Serial.println();
		rightEnc.write(0);
		leftEnc.write(0);
	});

	myPID.SetMode(AUTOMATIC);
	myPID.SetSampleTime(10);
}

void loop()
{
	// put your main code here, to run repeatedly:
	// setRightPWM(100);
	Setpoint = 60;
	myPID.Compute();

	// delay(200);
}
