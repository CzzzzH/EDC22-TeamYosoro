#ifndef LED_H
#define LED_H

#include <Arduino.h>
#define portLED 12

class LED
{
private:
    static uint8_t ledReg;
public:
    static void initialize();
    static void ledOn();
    static void ledOff();
    static uint8_t ledStatus();
};

#endif //LED_H