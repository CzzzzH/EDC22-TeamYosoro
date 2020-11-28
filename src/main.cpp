#include <Arduino.h>
#include "statemachine.h"
#include "TimerInterrupt.h"
#include "AngleControl.h"
#include "JY61.h"
#include "MotorControl.h"
#include "IRReceiver.h"
#include "SoftwareSerial.h"

#define INTERRUPT_INTERVAL 50

std::list<TimerInterrupt *>
	TimerInterrupt::timer_list = std::list<TimerInterrupt *>();

TimerInterrupt motorTimer(INTERRUPT_INTERVAL, [] {
	Motor::PID_compute();
});

TimerInterrupt angleTimer(10, [] {
	IRReceiver::updateValue();
	JY61::read();
	AngleControl::Compute();
	Motor::updatePWM();
    StateMachine::getInstance().process();
});

StateMachine &sm = StateMachine::getInstance();
void setup()
{
	sm.init(); // 一切的初始化
}

void loop()
{
    #ifdef USE_ZIGBEE
	    sm.updateInfo(); // 轮询更新串口信息
    #endif
}
