#include "LED.h"

void LED::ledOff()
{
    ledReg = LOW;
    digitalWrite(portLED, ledReg);
}

void LED::ledOn()
{
    ledReg = HIGH;
    analogWrite(portLED, 255);
    //Serial.println(portLED);
}

uint8_t LED::ledStatus()
{
    return ledReg;
}

void LED::initialize()
{
    pinMode(portLED, OUTPUT);
}

uint8_t LED::ledReg;