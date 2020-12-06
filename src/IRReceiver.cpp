#include "IRReceiver.h"
#include "MotorControl.h"
#include "statemachine.h"
#include "information.h"
#include "Maze.h"
#include "AngleControl.h"

#define ZIGBEE_OFFSET 0.3

void IRReceiver::initialize()
{
    for (uint8_t i = 0; i < MID_IR_COUNT; ++i)
        pinMode(MID_BEGIN + i, INPUT);
    for (uint8_t i = 0; i < MID_BACK_IR_COUNT; ++i)
        pinMode(MID_BACK_BEGIN + i, INPUT);

    pinMode(RIGHT_FRONT, INPUT);
    pinMode(LEFT_FRONT, INPUT);
    pinMode(RIGHT_BACK, INPUT);
    pinMode(LEFT_BACK, INPUT);


    IRReceiver::offsetPid.SetMode(AUTOMATIC);
    IRReceiver::offsetPid.SetSampleTime(10);
    IRReceiver::offsetPid.SetOutputLimits(-100, 100);
}

void IRReceiver::updateValue()
{
    IRMidHistory = IRMidAccum;
    IRMidAccum = 0;
    for (uint8_t i = 0; i < MID_IR_COUNT; ++i)
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
        if(midValue[i])
            IRMidAccum++;
    }
    for (uint8_t i = 0; i < MID_BACK_IR_COUNT; ++i)
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
    updateOffset();
    // Serial.println("IR offset : " + String(IROffset));
    offsetPid.Compute(false);
    // Serial.println("IR pid result : " + String(IRPidResult));
}

/*
    判断当前位置是否处于十字路口正中央，如果是（可能立即转弯）就返回true
    
    1. 这个函数的重点在于atCross这个变量，当前置红外碰线时被置位true，标志着进入预备转弯状态
    2. 只有atCross等于true时，中间红外碰黑线线才会返回true
*/
bool IRReceiver::atCrossroad(int16_t angle)
{
    // 前置红外熄灭个数
    uint8_t midCount = 0;
    uint8_t midBackCount = 0;
    for (uint8_t i = 0; i < MID_IR_COUNT; ++i)
        midCount += totalMidValue[i];
    for (uint8_t i = 0; i < MID_BACK_IR_COUNT; ++i)
        midBackCount += totalMidBackValue[i];

    // 转弯结束
    if (turn)
    {   
        if (AngleControl::getAngleDist() < 10)
            turnCount++;

        if (turnCount >= 3)
        {
            turn = false;
            slow = false;
            slowRight = false;
            slowLeft = false;
            Motor::targetSpeed = AHEAD_SPEED;
            #ifdef DEBUG_CROSS_ACTION
                Serial.println("[END TURN at time " + String(millis()) + "]");
                Serial.println();
            #endif
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
                slowLeft = false;
                Motor::targetSpeed = AHEAD_SPEED;
                #ifdef DEBUG_CROSS_ACTION
                    Serial.println("[END AHEAD at time " + String(millis()) + "]");
                    Serial.println();
                #endif
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
                #ifdef DEBUG_CROSS_ACTION
                    Serial.println("[END AHEAD at time " + String(millis()) + "]");
                    Serial.println();
                #endif
            }
        }
    }
    // 过交叉线
    else if (!turn && !ahead)
    {
        uint8_t IRCount = (StateMachine::motorDirection == 1) ? midCount : midBackCount;
        uint8_t threshold = (StateMachine::motorDirection == 1) ? 9 : 6;
        uint8_t leftValue = (StateMachine::motorDirection == 1) ? leftFrontValue : leftBackValue;
        uint8_t rightValue = (StateMachine::motorDirection == 1) ? rightFrontValue : rightBackValue;

        if (!StateMachine::insideTarget.empty() && !slow && abs(StateMachine::crossroadAction.rotateAngle) == 90)
        {
            if (slowRight && slowLeft)
            {
                Motor::targetSpeed = SLOW_SPEED;
                slow = true;
                #ifdef DEBUG_CROSS_ACTION
                    Serial.println("[SLOW at time " + String(millis()) + "]");
                #endif
            }
            else
            {
                if (leftValue)
                    slowLeft = true;
                if (rightValue)
                    slowRight = true;
            }
        }

        if ((IRCount >= threshold && IRMidAccum >= 1 && IRMidAccum >= IRMidHistory) || StateMachine::restart)
        {
            Serial.println("[CROSS at time " + String(millis()) + "]");
            if (StateMachine::restart)
            {
                #ifdef DEBUG_CROSS_ACTION
                    Serial.println("Restart!!! ");
                #endif
                StateMachine::restart = false;
                restartTime = millis();
            }

            if (angle == 90 || angle == -90)
            {
                Motor::targetSpeed = TURN_SPEED;
                StateMachine::lastCrossTime = millis();
                turnCount = 0;
                slow = false;
                turn = true;
            }
            else
                ahead = true;

            for (uint8_t i = 0; i < MID_IR_COUNT; ++i)
                totalMidValue[i] = 0;
            for (uint8_t i = 0; i < MID_BACK_IR_COUNT; ++i)
                totalMidBackValue[i] = 0;

            return true;
        }
    }
    return false;
}

float IRReceiver::compute_weight(uint8_t index, uint8_t total_count, float slope)
{
    uint8_t mid_index = total_count / 2;
    index += (index >= mid_index) ? 1 : 0;
    return slope * (mid_index - index);
}

/*
    1.在迷宫内的两个状态，根据中轴线输出offset较正位置（StateMachine::outsideTarget用于判断当前正在走第几条路径）
    2.在迷宫外的两个状态，根据红外巡线输出offset较正位置

    offset > 0 强制向右偏转
    offset < 0 强制向左偏转
*/
void IRReceiver::updateOffset()
{
    IROffset = 0;
    if (turn) return;
    if (StateMachine::nowMission == SEARCH_MAZE || StateMachine::nowMission == GO_OUT_MAZE)
    {
        uint8_t mid_count = (StateMachine::motorDirection == 1) ? MID_IR_COUNT : MID_BACK_IR_COUNT;
        uint8_t *mid_value = (StateMachine::motorDirection == 1) ? midValue : midBackValue;

        uint8_t i = mid_count / 2 - 1;
        uint8_t j = mid_count / 2;
        uint8_t count = 0;
        while (i >= 0)
        {
            IROffset += mid_value[i] * compute_weight(i, mid_count, 0.25) + mid_value[j] * compute_weight(j, mid_count, 0.25);
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
            IROffset = 0;
        else if (count > 0)
            IROffset = IROffset / (double)count;
    }
    else if (StateMachine::nowMission == GO_TO_MAZE)
    {
        if (StateMachine::outsideTarget.size() == 2)
            IROffset = (StateMachine::nowPosition.X - StateMachine::midLine) * ZIGBEE_OFFSET;
    }
    else if (StateMachine::nowMission == RETURN)
    {
        if (StateMachine::outsideTarget.size() == 1)
            IROffset = (StateMachine::nowPosition.X - StateMachine::midLine) * ZIGBEE_OFFSET;
    }
#ifdef DEBUG_IRRECEIVER
    Serial.println("[IR Offset: " + String(offset) + " ]");
#endif
}

uint8_t IRReceiver::turnCount = 0;
uint8_t IRReceiver::rightFrontValue = 0;
uint8_t IRReceiver::leftFrontValue = 0;
uint8_t IRReceiver::rightBackValue = 0;
uint8_t IRReceiver::leftBackValue = 0;
uint8_t IRReceiver::restartTime = 0;
uint8_t IRReceiver::midValue[MID_IR_COUNT];
uint8_t IRReceiver::totalMidValue[MID_IR_COUNT];
uint8_t IRReceiver::midBackValue[MID_BACK_IR_COUNT];
uint8_t IRReceiver::totalMidBackValue[MID_BACK_IR_COUNT];
bool IRReceiver::turn = false;
bool IRReceiver::ahead = false;
bool IRReceiver::slowLeft = false;
bool IRReceiver::slowRight = false;
bool IRReceiver::slow = false;
double IRReceiver::IROffset = 0;
double IRReceiver::IRPidResult = 0;
double IRReceiver::zero = 0;
uint8_t IRReceiver::IRMidAccum;
uint8_t IRReceiver::IRMidHistory;
PID IRReceiver::offsetPid = PID(&IRReceiver::IROffset, &IRReceiver::IRPidResult, &IRReceiver::zero, 1, 0.2, 0.1, DIRECT);