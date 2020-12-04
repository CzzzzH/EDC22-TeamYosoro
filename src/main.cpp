#include <Arduino.h>
#include "statemachine.h"
#include "TimerInterrupt.h"
#include "AngleControl.h"
#include "JY61.h"
#include "MotorControl.h"
#include "IRReceiver.h"
#include "SoftwareSerial.h"
#include "Maze.h"

std::list<TimerInterrupt *> TimerInterrupt::timer_list = std::list<TimerInterrupt *>();

void setup()
{
    StateMachine::init(); // 一切的初始化
}

void loop()
{
#ifdef USE_ZIGBEE
    StateMachine::updateInfo(); // 轮询更新串口信息
#endif
}
