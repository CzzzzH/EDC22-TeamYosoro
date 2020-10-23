#include "servoCtl.h"
#include <Arduino.h>

//快速跳转脱机角度可能发生堵转，这个舵机需要缓慢调整才行
void servoCtl::angleReach()
{
  int16_t angleTmp = angMemory;
  int16_t change;
  while (angleTmp != angNow)
  {
    change = (angNow > angMemory) ? 1 : -1;
    angleTmp = angleTmp + change;
    myServo.write(angleTmp);
    delay(delayT);
  }
}
servoCtl::servoCtl(void)
{
  angMemory = 90;
  angNow = 90;
}
void servoCtl::changeAngle(int16_t ang)
{
  angMemory = angNow;
  angNow = ang;
}
void servoCtl::setServo(int Pin)
{
  myServo.attach(Pin, 500, 2500);
}
