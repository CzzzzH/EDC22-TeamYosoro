#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <servoCtl.h>
#include "statemachine.h"
#include "AngleControl.h"
#include "MotorControl.h"
#include "TimerInterrupt.h"
#include "JY61.h"
#include "IRReceiver.h"

StateMachine &StateMachine::getInstance()
{
    static StateMachine instance;
    return instance;
}

void StateMachine::init()
{
    // Debug Mode
    JY61::isDebug = false;
    Motor::isDebug = false;

    // Initialize Serial
    Serial.begin(9600);
    Serial2.begin(115200);
    Serial3.begin(115200);

    // Initialize components
    Motor::initialize();
    TimerInterrupt::initialize(interrupt_period);
    AngleControl::initialize();
    servoCtl::initialize(4);
    IRReceiver::initialize();
    
    // Other Initialization 
    outsideTarget.push_back({16, 240});
    outsideTarget.push_back({72, 240});
    nowMission = WAIT_FOR_START;
}

// Execute every clock interruption
void StateMachine::process()
{
    // Serial.println("TargetSpeed: " + String(Motor::targetSpeed));
    // Serial.println("TargetAngle: " + String(AngleControl::target));

    Information &info = Information::getInstance();
    // updateInfo(info);
    // Serial.println("Position:  " + String(info.getCarposX()) + "  " + String(info.getCarposY()));
    updateAction(info);
    updateMission(info);
    updateMotor(info);
    counter++;
    // Serial.println("counter : " + String(counter));
    // Serial.println("milli seconds : " + String(millis()));
}

void StateMachine::updateInfo(Information &info)
{
    // TODO: update constant info (from gyroscope, encoders, zigbee)
    info.updateInfo();
    IRReceiver::updateValue();
}

void StateMachine::updateAction(Information &info)
{
    // TODO: update the current action (use infrared info and zigbee info)
    if (nowMission == GO_TO_MAZE)
    {
        if (info.getCarpos().getDist(outsideTarget[nowTargetIndex]) < 15)
        {
            switch (nowTargetIndex)
            {
            case 0:
                AngleControl::target += 90;
                break;
            case 1:
                AngleControl::target += 90;
                break;
            default:
                break;
            }
            nowTargetIndex++;
        }
    }
    else if (nowMission == SEARCH_MAZE)
    {
        if (IRReceiver::atCrossroad() && counter >= 0)
        {
            if (nowTargetIndex == 3) AngleControl::target += 90;
            if (nowTargetIndex == 6) AngleControl::target -= 90;
            counter = -10;
            nowTargetIndex++;
        }
    }
}

void StateMachine::updateMission(Information &info)
{
    // TODO: update the current mission  (use updated info)
    // Serial.println("GameState" + String(info.getGameState()));
    if (info.getGameState() == GameGoing && nowMission == WAIT_FOR_START)
        nowMission = GO_TO_MAZE;
    if (nowTargetIndex == 2 && nowMission == GO_TO_MAZE)
    {
        nowTargetIndex = 0;
        nowMission = SEARCH_MAZE;
    }
}

void StateMachine::updateMotor(Information &info)
{
    // TODO: update the motor paramters (use PID)
    Motor::PID_compute();

    JY61::read();
	if (AngleControl::Compute())
	{
		// Serial.println("read angle : " + String(JY61::Angle[2]));
		// Serial.println("target angle : " + String(AngleControl::target));
		// Serial.println("angle output :　" + String(AngleControl::getOutput()));
		servoCtl::myServo.write(AngleControl::middle - AngleControl::getOutput());
		// Motor::updatePWM();
	}
    Motor::updatePWM();
    if (nowMission == WAIT_FOR_START) Motor::targetSpeed = 0;
    else Motor::targetSpeed = 30;
} 