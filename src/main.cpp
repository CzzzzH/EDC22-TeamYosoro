#include <Arduino.h>
#include "statemachine.h"
#include <MsTimer2.h>
#include <string.h>
#include "TimerInterrupt.hpp"
#include "motor_control.hpp"

StateMachine &sm = StateMachine::getInstance();

void setup()
{
	initialize_motor_control_pin();
	Serial.begin(9600);

	//Timer Interrupt 10ms
	MsTimer2::set(10, Interrupt_10ms);
	MsTimer2::start();
	add_100ms([] {
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
	setRightPWM(100);
	delay(200);
}
