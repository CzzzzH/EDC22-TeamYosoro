#include <SoftwareSerial.h>
#include <JY901.h>
#include <Wire.h>
#include <Arduino.h>

SoftwareSerial JY61_serial(10, 11); // 10 is TX , 11 is RX

void JY61_print()
{
	Serial.print("Acc:");
	Serial.print((float)JY901.stcAcc.a[0] / 32768 * 16);
	Serial.print(" ");
	Serial.print((float)JY901.stcAcc.a[1] / 32768 * 16);
	Serial.print(" ");
	Serial.println((float)JY901.stcAcc.a[2] / 32768 * 16);

	Serial.print("Gyro:");
	Serial.print((float)JY901.stcGyro.w[0] / 32768 * 2000);
	Serial.print(" ");
	Serial.print((float)JY901.stcGyro.w[1] / 32768 * 2000);
	Serial.print(" ");
	Serial.println((float)JY901.stcGyro.w[2] / 32768 * 2000);

	Serial.print("Angle:");
	Serial.print((float)JY901.stcAngle.Angle[0] / 32768 * 180);
	Serial.print(" ");
	Serial.print((float)JY901.stcAngle.Angle[1] / 32768 * 180);
	Serial.print(" ");
	Serial.println((float)JY901.stcAngle.Angle[2] / 32768 * 180);

	Serial.println("");
}

/*
    SerialEvent occurs whenever a new data comes in the
    hardware serial RX.  This routine is run between each
    time loop() runs, so using delay inside loop can delay
    response.  Multiple bytes of data may be available.
 */
void JY61_read()
{
	while (JY61_serial.available())
	{
		JY901.CopeSerialData(JY61_serial.read()); //Call JY901 data cope function
	}
	JY61_print();
}
