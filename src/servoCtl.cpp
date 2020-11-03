#include "servoCtl.h"
#include <Arduino.h>

//快速跳转脱机角度可能发生堵转，这个舵机需要缓慢调整才行
void servoCtl::angleReach(int16_t ang)
{
  angMemory = angNow;
  angNow = ang;
  int16_t angleTmp = angMemory;
  int16_t change;
  while (angleTmp != angNow)
  {
    change = (angNow > angMemory) ? 1 : -1;
    angleTmp = angleTmp + change;
    myServo.write(angleTmp);
    // Serial.print("angle temp :    ");
    // Serial.println(angleTmp);
    delay(delayT);
  }
}

void servoCtl::initialize(int Pin)
{
  angMemory = 90;
  angNow = 90;
  myServo.attach(Pin, 500, 2500);
}

int16_t servoCtl::angNow = 90;
int16_t servoCtl::angMemory = 90;
Servo servoCtl::myServo = Servo();  