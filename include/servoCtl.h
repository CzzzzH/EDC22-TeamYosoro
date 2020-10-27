#include <Servo.h>

class servoCtl //舵机角度控制类
{
public:
  static void initialize(int);
  // static void setServo(int Pin);
  static void angleReach(int16_t);
  static int16_t angNow;

public:
  static Servo myServo;

private:
  static int16_t angMemory;

  static const int delayT = 15;
};
