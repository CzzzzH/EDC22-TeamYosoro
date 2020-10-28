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

	Serial.print("\tAcc:  ");
	for (int i = 0; i < 3; i++)
		Serial.print(String(Acc[i]) + "    ");
	Serial.println("");

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
	static double last_Angle_2;
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
	if (fabs(Angle[2] - last_Angle_2) > 30)
	{
		Serial.println("angle jump !!!");
		Serial.println("last angle : " + String(last_Angle_2));
		Serial.println("this angle : " + String(Angle[2]));
		Angle[2] = last_Angle_2;
	}
	last_Angle_2 = Angle[2];
	print();
}

bool JY61::isDebug = false;

double JY61::Acc[3];
double JY61::Gyro[3];
double JY61::Angle[3];
