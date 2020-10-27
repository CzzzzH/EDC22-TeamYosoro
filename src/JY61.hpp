#ifndef JY61_HPP
#define JY61_HPP

#include <JY901.h>
#include <Wire.h>
#include <Arduino.h>

class JY61
{
public:
	static double Acc[3];
	static double Gyro[3];
	static double Angle[3];

	static bool isDebug;

	static void print()
	{
		if (!isDebug)
			return;
		Serial.println("JY61 :");
		Serial.print("\tAcc:  ");
		for (int i = 0; i < 3; i++)
		{
			Serial.print(Acc[i]);
			Serial.print(" ");
		}
		Serial.println("");

		Serial.print("\tGyro:");
		for (int i = 0; i < 3; i++)
		{
			Serial.print(Gyro[i]);
			Serial.print(" ");
		}
		Serial.println("");

		Serial.print("\tAngle:");
		for (int i = 0; i < 3; i++)
		{
			Serial.print(Angle[i]);
			Serial.print(" ");
		}
		Serial.println("");

		Serial.println("");
	}

	static void read()
	{
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
		print();
	}
};

bool JY61::isDebug = false;

double JY61::Acc[3];
double JY61::Gyro[3];
double JY61::Angle[3];

#endif