#include <Arduino.h>

const int PWM_pin1 = 11;

void setup()
{
	// put your setup code here, to run once:
	pinMode(PWM_pin1, OUTPUT);
	Serial.begin(9600);
}

void loop()
{
	// put your main code here, to run repeatedly:
	for (int i = 0; i < 256; i += 10)
	{
		analogWrite(PWM_pin1, i);
		delay(100);
	}
	Serial.write("aaa\n");
}