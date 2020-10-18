#include <Arduino.h>
#include <MsTimer2.h>
#include <string.h>
#include "TimerInterrupt.hpp"

const int PWM_1_A = 11;
const int PWM_1_B = 10;

const int encoderA = 21;
const int encoderB = 20;

int encoderA_counter = 0;
int encoderB_counter = 0;

void setup()
{
	// put your setup code here, to run once:
	pinMode(PWM_1_A, OUTPUT);
	pinMode(PWM_1_B, OUTPUT);
	Serial.begin(9600);

	//Timer Interrupt 10ms
	MsTimer2::set(10, Interrupt_10ms);
	MsTimer2::start();
	add_100ms([] {
		// encoder
		Serial.println(encoderA_counter);
		Serial.println(encoderB_counter);
		Serial.println();
		encoderA_counter = 0;
		encoderB_counter = 0;
	});

	// Encoder input
	pinMode(encoderA, INPUT);
	pinMode(encoderB, INPUT);

	// External Interrupt
	attachInterrupt(
		2, [] { encoderA_counter++; }, RISING);
	attachInterrupt(
		3, [] { encoderB_counter++; }, RISING);
}

void loop()
{
	// put your main code here, to run repeatedly:
	analogWrite(PWM_1_A, 100);
	analogWrite(PWM_1_B, 0);
	delay(200);
}
