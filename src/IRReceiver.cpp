#include "IRReceiver.h"
#include "MotorControl.h"
#include "statemachine.h"
#include "information.h"
#include "Maze.h"
#include "AngleControl.h"

#define ZIGBEE_OFFSET 0.3

void IRReceiver::initialize()
{
    for (int i = 0; i < MID_IR_COUNT; ++i)
        pinMode(MID_BEGIN + i, INPUT);
    for (int i = 0; i < MID_BACK_IR_COUNT; ++i)
        pinMode(MID_BACK_BEGIN + i, INPUT);

    pinMode(RIGHT_FRONT, INPUT);
    pinMode(LEFT_FRONT, INPUT);
    pinMode(RIGHT_BACK, INPUT);
    pinMode(LEFT_BACK, INPUT);
}

void IRReceiver::updateValue()
{
    for (int i = 0; i < MID_IR_COUNT; ++i)
    {
        midValue[i] = digitalRead(MID_BEGIN + i);
        if (!turn && !ahead)
            totalMidValue[i] = max(totalMidValue[i], midValue[i]);
        else
        {
            if (StateMachine::motorDirection == 1)
                totalMidValue[i] = min(totalMidValue[i], midValue[i]);
            else
                totalMidValue[i] = max(totalMidValue[i], midValue[i]);
        }
    }
    for (int i = 0; i < MID_BACK_IR_COUNT; ++i)
    {
        midBackValue[i] = digitalRead(MID_BACK_BEGIN + i);
        if (!turn && !ahead)
            totalMidBackValue[i] = max(totalMidBackValue[i], midBackValue[i]);
        else
        {
            if (StateMachine::motorDirection == -1)
                totalMidBackValue[i] = min(totalMidBackValue[i], midBackValue[i]);
            else
                totalMidBackValue[i] = max(totalMidBackValue[i], midBackValue[i]);
        }
    }
    rightFrontValue = digitalRead(RIGHT_FRONT);
    leftFrontValue = digitalRead(LEFT_FRONT);
    rightBackValue = digitalRead(RIGHT_BACK);
    leftBackValue = digitalRead(LEFT_BACK);
}

/*
    判断当前位置是否处于十字路口正中央，如果是（可能立即转弯）就返回true
    
    1. 这个函数的重点在于atCross这个变量，当前置红外碰线时被置位true，标志着进入预备转弯状态
    2. 只有atCross等于true时，中间红外碰黑线线才会返回true
*/
bool IRReceiver::atCrossroad(int angle)
{
    // 前置红外熄灭个数
    int midCount = 0;
    int midBackCount = 0;
    for (int i = 0; i < MID_IR_COUNT; ++i)
        midCount += totalMidValue[i];
    for (int i = 0; i < MID_BACK_IR_COUNT; ++i)
        midBackCount += totalMidBackValue[i];

    // 转弯结束
    if (turn)
    {
        if ((leftBackValue || rightBackValue) && AngleControl::getAngleDist() < 10 && millis() - StateMachine::lastCrossTime > 500)
        {
            turn = false;
            slowRight = false;
            slowLeft = false;
            Motor::targetSpeed = AHEAD_SPEED;
            Serial.println("[END TURN at time " + String(millis()) + "]");
            Serial.println();
        }
    }
    // 直走结束
    else if (ahead)
    {
        if (StateMachine::motorDirection == 1)
        {
            if ((leftBackValue || rightBackValue) && millis() - restartTime > 300)
            {
                ahead = false;
                slowRight = false;
                if (leftBackValue || rightBackValue)
                    slowLeft = false;
                Motor::targetSpeed = AHEAD_SPEED;
                Serial.println("[END AHEAD at time " + String(millis()) + "]");
                Serial.println();
            }
        }
        else
        {
            if ((leftFrontValue || rightFrontValue) && millis() - restartTime > 300)
            {
                ahead = false;
                slowRight = false;
                slowLeft = false;
                Motor::targetSpeed = AHEAD_SPEED;
                Serial.println("[END AHEAD at time " + String(millis()) + "]");
                Serial.println();
            }
        }
    }
    // 过交叉线
    else if (!turn && !ahead)
    {

        if (StateMachine::motorDirection == 1)
        {
            if (!StateMachine::insideTarget.empty() && abs(Motor::targetSpeed) != SLOW_SPEED && abs(StateMachine::crossroadAction.rotateAngle) == 90)
            {
                if (slowRight && slowLeft)
                {
                    Motor::targetSpeed = SLOW_SPEED;
                    Serial.println("[SLOW at time " + String(millis()) + "]");
                }
                else
                {
                    if (leftFrontValue)
                        slowLeft = true;
                    if (rightFrontValue)
                        slowRight = true;
                }
            }

            if (midCount >= 12 || StateMachine::restart)
            {
                Serial.println("[CROSS at time " + String(millis()) + "]");
                if (StateMachine::restart)
                {
                    Serial.println("Restart!!! ");
                    StateMachine::restart = false;
                    restartTime = millis();
                }

                if (angle == 90 || angle == -90)
                {
                    Motor::targetSpeed = TURN_SPEED;
                    StateMachine::lastCrossTime = millis();
                    turn = true;
                }
                else
                    ahead = true;

                for (int i = 0; i < MID_IR_COUNT; ++i)
                    totalMidValue[i] = 0;
                for (int i = 0; i < MID_BACK_IR_COUNT; ++i)
                    totalMidBackValue[i] = 0;

                return true;
            }
        }
        else
        {
            if (!StateMachine::insideTarget.empty() && abs(Motor::targetSpeed) != SLOW_SPEED && abs(StateMachine::crossroadAction.rotateAngle) == 90)
            {
                if (slowRight && slowLeft)
                {
                    Motor::targetSpeed = SLOW_SPEED;
                    Serial.println("[SLOW at time " + String(millis()) + "]");
                }
                else
                {
                    if (leftBackValue)
                        slowLeft = true;
                    if (rightBackValue)
                        slowRight = true;
                }
            }

            if (midBackCount >= 6 || StateMachine::restart)
            {
                Serial.println("[CROSS at time " + String(millis()) + "]");
                if (StateMachine::restart)
                {
                    Serial.println("Restart!!! ");
                    StateMachine::restart = false;
                    restartTime = millis();
                }

                if (angle == 90 || angle == -90)
                {
                    Motor::targetSpeed = TURN_SPEED;
                    StateMachine::lastCrossTime = millis();
                    turn = true;
                }
                else
                    ahead = true;

                for (int i = 0; i < MID_IR_COUNT; ++i)
                    totalMidValue[i] = 0;
                for (int i = 0; i < MID_BACK_IR_COUNT; ++i)
                    totalMidBackValue[i] = 0;

                return true;
            }
        }
    }
    return false;
}

double compute_weight(int index, int total_count, double slope)
{
    int mid_index = total_count / 2;
    index += (index >= mid_index) ? 1 : 0;
    return slope * (mid_index - index);
}

/*
    1.在迷宫内的两个状态，根据中轴线输出offset较正位置（StateMachine::outsideTarget用于判断当前正在走第几条路径）
    2.在迷宫外的两个状态，根据红外巡线输出offset较正位置

    offset > 0 强制向右偏转
    offset < 0 强制向左偏转
*/
double IRReceiver::angleOffset()
{
    if (turn)
        return 0;
    double offset = 0;
    if (StateMachine::nowMission == SEARCH_MAZE || StateMachine::nowMission == GO_OUT_MAZE)
    {
        int mid_count = (StateMachine::motorDirection == 1) ? MID_IR_COUNT : MID_BACK_IR_COUNT;
        int *mid_value = (StateMachine::motorDirection == 1) ? midValue : midBackValue;

        int i = mid_count / 2 - 1;
        int j = mid_count / 2;
        int count = 0;
        while (i >= 0)
        {
            offset += mid_value[i] * compute_weight(i, mid_count, 0.25) + mid_value[j] * compute_weight(j, mid_count, 0.25);
            count += mid_value[i];
            count += mid_value[j];
            if (mid_value[i] == 0 && mid_value[i + 1] == 1)
                break;
            if (mid_value[j] == 0 && mid_value[j - 1] == 1)
                break;
            i--;
            j++;
        }
        if (count > 5)
            offset = 0;
        else if (count > 0)
            offset = offset / (double)count;
    }
    else if (StateMachine::nowMission == GO_TO_MAZE)
    {
        if (StateMachine::outsideTarget.size() == 2)
            offset = (StateMachine::nowPosition.X - StateMachine::midLine) * ZIGBEE_OFFSET;
    }
    else if (StateMachine::nowMission == RETURN)
    {
        if (StateMachine::outsideTarget.size() == 1)
            offset = (StateMachine::nowPosition.X - StateMachine::midLine) * ZIGBEE_OFFSET;
    }
#ifdef DEBUG_IRRECEIVER
    Serial.println("[IR Offset: " + String(offset) + " ]");
#endif
    return offset;
}

int IRReceiver::rightFrontValue = 0;
int IRReceiver::leftFrontValue = 0;
int IRReceiver::rightBackValue = 0;
int IRReceiver::leftBackValue = 0;
int IRReceiver::restartTime = 0;
int IRReceiver::midValue[MID_IR_COUNT];
int IRReceiver::totalMidValue[MID_IR_COUNT];
int IRReceiver::midBackValue[MID_BACK_IR_COUNT];
int IRReceiver::totalMidBackValue[MID_BACK_IR_COUNT];
// double IRReceiver::midWeight[MID_IR_COUNT];
// double IRReceiver::midBackWeight[MID_BACK_IR_COUNT];
bool IRReceiver::turn = false;
bool IRReceiver::ahead = false;
bool IRReceiver::slowLeft = false;
bool IRReceiver::slowRight = false;