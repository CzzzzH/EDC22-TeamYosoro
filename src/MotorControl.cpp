#include "MotorControl.h"
#include "AngleControl.h"
#include <Arduino.h>
#include <Encoder.h>
#include <PID_v1.h>
#include <string.h>
#include <string>
#include "IRReceiver.h"
#include "statemachine.h"

const double right_left_coeff = 1.03;

const int RIGHT_MAX_PWM = 240;
// const int LEFT_MAX_PWM = 255 / right_left_coeff;

void encoder::initialize()
{
    pinMode(left_pin.A, INPUT);
    pinMode(left_pin.B, INPUT);
    pinMode(right_pin.A, INPUT);
    pinMode(right_pin.B, INPUT);
}

void encoder::Read()
{
    counter.right = rightEnc.read();
    counter.left = leftEnc.read();
}
void encoder::Reset()
{
    rightEnc.write(0);
    leftEnc.write(0);
}

const pin_2 encoder::left_pin = {19, 18};
const pin_2 encoder::right_pin = {21, 20};
decltype(encoder::counter) encoder::counter;

Encoder encoder::rightEnc(encoder::right_pin.A, encoder::right_pin.B);
Encoder encoder::leftEnc(encoder::left_pin.A, encoder::left_pin.B);

void Motor::PID_initialize()
{
    leftPID.SetMode(AUTOMATIC);
    leftPID.SetSampleTime(timePeriod);
    leftPID.SetOutputLimits(-100, 100);

    rightPID.SetMode(AUTOMATIC);
    rightPID.SetSampleTime(timePeriod);
    rightPID.SetOutputLimits(-100, 100);
}

void Motor::initialize()
{
    pinMode(left_pin.A, OUTPUT);
    pinMode(left_pin.B, OUTPUT);

    pinMode(right_pin.A, OUTPUT);
    pinMode(right_pin.B, OUTPUT);

    encoder::initialize();
    PID_initialize();
}

void Motor::setPWM(int pwm, bool isRight)
{
    pwm = min(pwm, RIGHT_MAX_PWM);
    pwm = max(pwm, -RIGHT_MAX_PWM);
    if (!isRight)
        pwm /= right_left_coeff;
    if (isDebug)
    {
        Serial.print(String("setting ") + (isRight ? "right" : "left") + String(" pwm : "));
        Serial.println(pwm);
    }
    pin_2 pin = isRight ? right_pin : left_pin;
    if (pwm > 0)
    {
        analogWrite(pin.A, pwm);
        analogWrite(pin.B, 0);
    }
    else
    {
        analogWrite(pin.A, 0);
        analogWrite(pin.B, -pwm);
    }
}

void Motor::PID_compute()
{
    // int i = 0;
    // i = i + 1;
    // Serial.println(i);
    encoder::Read();
    if (isDebug)
    {
        Serial.println("Left encoder : " + String(encoder::counter.left));
        Serial.println("right encoder : " + String(encoder::counter.right));
    }
    // Serial.println("mills : " + String(millis()));
    leftPID.Compute(false);
    rightPID.Compute(false);
    // Serial.print("Left Motor Counter: " + String(encoder::counter.left) + ";\t");
    // Serial.print("Right Motor Counter: " + String(encoder::counter.right) + ";\t");
    // Serial.println("motor left output: " + String(leftOutput));
    // Serial.println("motor right output: " + String(rightOutput));
    encoder::Reset();
}

double diffVelocity(const double angle)
{
    if (AngleControl::target == 0)
        return 0;
    double result_angle = 1.6 * angle + pow(angle / 15, 3);
    return result_angle;
}

void Motor::updatePWM()
{
    // Serial.println("Getoutput: " + String(AngleControl::getOutput()));
    double IRcoff = 20;
    if (targetSpeed < 0)
        IRcoff = -0.99 * IRcoff;
    double IR_in = (fabs(AngleControl::getOutput()) < 6 ? IRcoff : 0) * IRReceiver::IRPidResult;
    double diff_velocity_in = -AngleControl::getOutput();
    // if (StateMachine::getInstance().motorDirection == -1)
    //     diff_velocity_in = -diff_velocity_in;
    // Serial.println("Angle output : " + String(AngleControl::getOutput()));
    // Serial.println("IR_in : " + String(IR_in));
    double angle_slow = abs(30.0 / AngleControl::getOutput());
    angle_slow = min(angle_slow, 1);

    setPWM(angle_slow * estimatePWM(targetSpeed) + rightOutput + diffVelocity(-diff_velocity_in) + IR_in, true);
    setPWM(angle_slow * estimatePWM(targetSpeed) + leftOutput + diffVelocity(diff_velocity_in) - IR_in, false);
    // setPWM(120, true);
    // setPWM(120, false);
}

double Motor::estimatePWM(double targeteSpeed)
{
    return 2.2 * targeteSpeed;
}

const pin_2 Motor::left_pin = {6, 7};
const pin_2 Motor::right_pin = {5, 4};
bool Motor::isDebug = false;

const int Motor::timePeriod = 10;
// double basePWM = 0;
double Motor::rightOutput = 0;
double Motor::leftOutput = 0;
double Motor::targetSpeed = 0;

PID Motor::rightPID = PID(&encoder::counter.right, &rightOutput, &targetSpeed, 2, 0.2, 0.1, DIRECT);
PID Motor::leftPID = PID(&encoder::counter.left, &leftOutput, &targetSpeed, 2, 0.2, 0.1, DIRECT);
