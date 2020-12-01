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
    #ifdef DEBUG_MOTOR
        Serial.println("Target Speed: " + String(Motor::targetSpeed));
        Serial.println("Left Motor Counter: " + String(encoder::counter.left));
        Serial.println("Right Motor Counter: " + String(encoder::counter.right));
    #endif
	Motor::PID_compute();
    Motor::targetSpeed = 350;
});

TimerInterrupt angleTimer(10, [] {
	IRReceiver::updateValue();
	JY61::read();
	AngleControl::Compute();
    // StateMachine::getInstance().process();
	Motor::updatePWM();
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
