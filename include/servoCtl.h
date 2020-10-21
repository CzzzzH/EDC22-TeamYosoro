#include <Servo.h>

class servoCtl //舵机角度控制类
{
  public:
    servoCtl(void);
    void setServo(int Pin);
    void changeAngle(int16_t ang);
	  void angleReach();
    int16_t angNow;

  private:
    int16_t angMemory;
    Servo myServo;
    static const int delayT = 15;
};
