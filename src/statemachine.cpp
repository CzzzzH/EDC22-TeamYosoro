#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <servoCtl.h>
#include "statemachine.h"
#include "AngleControl.hpp"
#include "MotorControl.hpp"

TimerInterrupt angleTimer(angle_time_period / interrupt_period, [] {
	if (anglePID.Compute())
	{
		JY61::read();
        if (fabs(targetAngle - JY61::Angle[2]) > 360) 
            JY61::Angle[2] += double(int(targetAngle - JY61::Angle[2]) / 360) * 360;
		servoCtl::myServo.write(baseAngle - angleOutput);
	}
});

TimerInterrupt motorTimer(motor_time_interval / interrupt_period, [] 
{
    StateMachine::getInstance().process();
});

StateMachine& StateMachine::getInstance()
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
    Motor::isDebug = false;

	Motor::initialize();
	
    //Timer Interrupt 10ms
	TimerInterrupt::initialize(interrupt_period);

	PID_initialize();

	angleInitialize();

	servoCtl::initialize(4);

	JY61::read();
	initAngle = JY61::Angle[2];
    initSpeed = 0;

    targetSpeed = initSpeed;
	targetAngle = initAngle;

    outsideTarget.push_back({23, 236});
    outsideTarget.push_back({83, 232});
    nowState = OUT_MAZE;
    nowMission = GO_TO_MAZE;
}

// Execute every clock interruption
void StateMachine::process()
{
    // Begin

    Serial.println("TargetSpeed: " + String(targetSpeed));
    Serial.println("TargetAngle: " + String(targetAngle));

    Information &info = Information::getInstance();
    // updateInfo(info);
    Serial.println("Position:  " + String(info.getCarposX()) + "  " + String(info.getCarposY()));
    updateAction(info);
    updateMission();
    updateMotor();
    counter++;
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
    if (nowMission == GO_TO_MAZE);
    {
        if (info.getCarpos().getDist(outsideTarget[nowTargetIndex]) < 5)
        {
            switch (nowTargetIndex)
            {
                case 0:
                    targetAngle -= 90;
                    break;
                case 1:
                    targetAngle -= 90;
                    break;
                default:
                    break;
            }
            nowTargetIndex ++;
        }
        if (counter == 30)
        {
            counter = 0;
            targetAngle += 90;
        }
    }
}

void StateMachine::updateMotor()
{
    // TODO: update the motor paramters (use PID)
    EncoderRead();
    PID_compute();
	Motor::setRightPWM(targetSpeed * 4.2 + right_Output);
	Motor::setLeftPWM(targetSpeed * 4.2 + left_Output);
    EncoderReset();
}