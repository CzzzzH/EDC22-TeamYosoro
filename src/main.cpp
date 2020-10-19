#include <Arduino.h>
#include "statemachine.h"
#include <MsTimer2.h>
#include <string.h>
#include "TimerInterrupt.hpp"
#include "motor_control.hpp"
#include "PID.hpp"

StateMachine &sm = StateMachine::getInstance();

PID pid = PID(
	1.1, 0.1, 0.1, [](double d) { setRightPWM(d); }, &right_encoder_counter);

void setup()
{

	initialize_motor_control_pin();
	Serial.begin(9600);

	//Timer Interrupt 10ms
	MsTimer2::set(10, Interrupt_10ms);
	MsTimer2::start();
	add_100ms([] {
		PID::run();

		// encoder
		Serial.println(right_encoder_counter);
		//Serial.println(encoderB_counter);
		Serial.println();
		right_encoder_counter = 0;
		left_encoder_counter = 0;
	});
}

void loop()
{
	// put your main code here, to run repeatedly:
	// setRightPWM(150);
	pid.target = 110;
	delay(200);
}
