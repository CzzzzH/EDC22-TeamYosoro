#include <Arduino.h>
#include "statemachine.h"
#include <MsTimer2.h>
#include <string.h>
#include <Encoder.h>
#include "TimerInterrupt.hpp"
#include "motor_control.hpp"
#include <PID_v1.h>
#include <SoftwareSerial.h>
#include <JY61.hpp>

StateMachine &sm = StateMachine::getInstance();

double Output;
double Setpoint;
PID myPID(&encoder::counter.right, &Output, &Setpoint, 2, 0.05, 0.05, DIRECT);

Encoder rightEnc(encoder::right_pin.A, encoder::right_pin.B);
Encoder leftEnc(encoder::left_pin.A, encoder::left_pin.B);

TimerInterrupt timer1(3, [] {
	encoder::counter.right = rightEnc.read();
	encoder::counter.left = leftEnc.read();

	// Serial.print("right:  ");
	// Serial.print(encoder::counter.right);
	// Serial.print("\t\tleft:   ");
	// Serial.println(encoder::counter.left);

	// Motor::setRightPWM(250);
	// Motor::setLeftPWM(250);

	rightEnc.write(0);
	leftEnc.write(0);
});

TimerInterrupt timer2(50, [] {
	JY61_read();
	// JY61_print();
});

void setup()
{
	Motor::initialize();
	Serial.begin(9600);
	JY61_serial.begin(115200);
	// delay(2000);

	//Timer Interrupt 10ms
	TimerInterrupt::initialize(10);

	myPID.SetMode(AUTOMATIC);
	myPID.SetSampleTime(10);
}

void loop()
{
	// put your main code here, to run repeatedly:
	// setRightPWM(100);
	Setpoint = 100;
	// myPID.Compute();

	// delay(200);
}
