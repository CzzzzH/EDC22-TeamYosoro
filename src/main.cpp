#include <Arduino.h>
#include <MsTimer2.h>

const int PWM_pin1 = 11;

void Interrupt_10ms()
{
	static int count = 0;
	if (count == 50)
	{
		Serial.write("Interrupt_500ms");
		count = 0;
	}
	count++;
}

void setup()
{
	// put your setup code here, to run once:
	pinMode(PWM_pin1, OUTPUT);
	Serial.begin(9600);

	//Timer Interrupt 10ms
	MsTimer2::set(10, Interrupt_10ms);
	MsTimer2::start();
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
