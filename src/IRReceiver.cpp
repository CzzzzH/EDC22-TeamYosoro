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

    // // Right
    // midWeight[0] = 0;
    // midWeight[1] = 1.6;
    // midWeight[2] = 1.5;
    // midWeight[3] = 1.25;
    // midWeight[4] = 1;
    // midWeight[5] = 0.75;
    // midWeight[6] = 0.5;
    // midWeight[7] = 0.25;
    // midBackWeight[0] = 2;
    // midBackWeight[1] = 1;
    // midBackWeight[2] = 0.5;
    // midBackWeight[3] = 0.5;

    // // Left
    // midBackWeight[4] = -0.5;
    // midBackWeight[5] = -0.5;
    // midBackWeight[6] = -1;
    // midBackWeight[7] = -2;
    // midWeight[8] = -0.25;
    // midWeight[9] = -0.5;
    // midWeight[10] = -1;
    // midWeight[11] = -0.75;
    // midWeight[12] = -1.25;
    // midWeight[13] = -1.5;
    // midWeight[14] = -1.6;
    // midWeight[15] = 0;
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

            if (midCount >= 14 || StateMachine::restart)
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

            if (midBackCount >= 7 || StateMachine::restart)
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
        if (StateMachine::motorDirection == 1)
        {
            int i = MID_IR_COUNT / 2 - 1;
            int j = MID_IR_COUNT / 2;
            int count = 0;
            while (i >= 0)
            {
                offset += midValue[i] + midValue[j];
                count += midValue[i];
                count += midValue[j];
                if (midValue[i] == 0 && midValue[i + 1] == 1)
                    break;
                if (midValue[j] == 0 && midValue[j - 1] == 1)
                    break;
                i--;
                j++;
            }
            if (count > 5)
                offset = 0;
        }
        else
        {
            int i = MID_BACK_IR_COUNT / 2 - 1;
            int j = MID_BACK_IR_COUNT / 2;
            while (i >= 0)
            {
                // offset += midBackValue[i] * midBackWeight[i] + midBackValue[j] * midBackWeight[j];
                offset += - (midBackValue[i] + midBackValue[i]);
                if (midBackValue[i] == 0 && midBackValue[i + 1] == 1)
                    break;
                if (midBackValue[j] == 0 && midBackValue[j - 1] == 1)
                    break;
                i--;
                j++;
            }
        }
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