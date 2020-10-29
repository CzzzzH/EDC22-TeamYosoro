#include <JY901.h>
#include <Wire.h>
#include <Arduino.h>
#include <string.h>
#include "JY61.h"
#include "math.h"

void JY61::print()
{
	if (!isDebug)
		return;
	Serial.println("JY61 :");

	// Serial.print("\tAcc:  ");
	// for (int i = 0; i < 3; i++)
	// 	Serial.print(String(Acc[i]) + "    ");
	// Serial.println("");

	Serial.print("\tGyro:");
	for (int i = 0; i < 3; i++)
		Serial.print(String(Gyro[i]) + "    ");
	Serial.println("");

	Serial.print("\tAngle:");
	for (int i = 0; i < 3; i++)
		Serial.print(String(Angle[i]) + "    ");
	Serial.println("");

	Serial.println("");
}

void JY61::read()
{
	static int count = 0;
	static double last_Angle = 0;
	while (Serial2.available())
	{
		JY901.CopeSerialData(Serial2.read()); //Call JY901 data cope function
	}
	for (int i = 0; i < 3; i++)
	{
		Acc[i] = (double)JY901.stcAcc.a[i] / 32768 * 16;
		Gyro[i] = (double)JY901.stcGyro.w[i] / 32768 * 2000;
		Angle[i] = (double)JY901.stcAngle.Angle[i] / 32768 * 180;
	}

	if (count < 5)
	{
		count++;
	}
	else
	{
		double delta = Angle[2] - last_Angle;
		// Serial.println("delta1 : " + String(delta));
		delta = fmod(delta, 360);
		// Serial.println("delta2 : " + String(delta));
		delta = delta - ((delta > 180) ? 360 : 0);
		// Serial.println("delta3 : " + String(delta));
		delta = fabs(delta);
		double lambda = (delta < 20) ? 1 : exp(0.2 * (20 - delta));
		// Serial.println("delta : " + String(delta));
		// Serial.println("lambda : " + String(lambda));
		// Serial.println("Angle[2] before : " + String(Angle[2]));
		// Serial.println("lastAngle : " + String(last_Angle));

		Angle[2] = lambda * Angle[2] + (1 - lambda) * last_Angle;
	}

	last_Angle = Angle[2];
	print();
}

bool JY61::isDebug = false;

double JY61::Acc[3];
double JY61::Gyro[3];
double JY61::Angle[3];
