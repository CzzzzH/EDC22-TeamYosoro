#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <servoCtl.h>
#include "statemachine.h"
#include "AngleControl.h"
#include "MotorControl.h"
#include "TimerInterrupt.h"
#include "JY61.h"

StateMachine &StateMachine::getInstance()
{
    static StateMachine instance;
    return instance;
}

void StateMachine::init()
{
    Serial.begin(9600);
    Serial2.begin(115200);
    Serial3.begin(115200);
    JY61::isDebug = false;
    Motor::isDebug = true;

    Motor::initialize();

    //Timer Interrupt 10ms
    TimerInterrupt::initialize(interrupt_period);

    AngleControl::initialize();

    servoCtl::initialize(4);

    Motor::targetSpeed = 0;

    outsideTarget.push_back({23, 236});
    outsideTarget.push_back({83, 232});
    nowState = OUT_MAZE;
    nowMission = GO_TO_MAZE;
}

// Execute every clock interruption
void StateMachine::process()
{
    // Begin

    // Serial.println("TargetSpeed: " + String(Motor::targetSpeed));
    // Serial.println("TargetAngle: " + String(AngleControl::target));

    Information &info = Information::getInstance();
    // updateInfo(info);
    // Serial.println("Position:  " + String(info.getCarposX()) + "  " + String(info.getCarposY()));
    updateAction(info);
    updateMission();
    Motor::PID_compute();
    Motor::updatePWM();
    Motor::targetSpeed = 30;
    counter += 1;
    // Serial.println("counter : " + String(counter));
    Serial.println("milli seconds : " + String(millis()));
    // End
}

void StateMachine::updateInfo(Information &info)
{
    // TODO: update constant info (from gyroscope, encoders, zigbee)
    info.updateInfo();
}

void StateMachine::updateMission()
{
    // TODO: update the current mission  (use updated info)
    if (nowTargetIndex == 2 && nowMission == GO_TO_MAZE)
    {
        nowTargetIndex = 0;
        nowMission = RETURN;
    }
}

void StateMachine::updateAction(Information &info)
{
    // TODO: update the current action (use infrared info and zigbee info)
    if (nowMission == GO_TO_MAZE)
        ;
    {
        if (info.getCarpos().getDist(outsideTarget[nowTargetIndex]) < 5)
        {
            switch (nowTargetIndex)
            {
            case 0:
                AngleControl::target -= 90;
                break;
            case 1:
                AngleControl::target -= 90;
                break;
            default:
                break;
            }
            nowTargetIndex++;
        }
        if (counter == 30)
        {
            counter = 0;
            AngleControl::target += 90;
        }
    }
}

// void StateMachine::updateMotor()
// {
//     // TODO: update the motor paramters (use PID)
// }