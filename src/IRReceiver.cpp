#include "IRReceiver.h"
#include "MotorControl.h"
#include "statemachine.h"
#include "information.h"
#include "Maze.h"
#include "AngleControl.h"

#define ZIGBEE_OFFSET 0.3

void IRReceiver::initialize()
{
    memset(midValue, 0, sizeof(midValue));
    memset(totalMidValue, 0, sizeof(totalMidValue));
    memset(midBackValue, 0, sizeof(midValue));
    memset(totalMidBackValue, 0, sizeof(totalMidBackValue));
    pinMode(LEFT_BEGIN, INPUT);
    pinMode(RIGHT_BEGIN, INPUT);
    for (int i = 0; i < MID_IR_COUNT; i++)
        pinMode(MID_BEGIN + i, INPUT);
    for (int i = 0; i < MID_BACK_IR_COUNT; i++)
        pinMode(MID_BACK_BEGIN + i, INPUT);
        
    // Right 
    midWeight[0] = 0; 
    midWeight[1] = 0;  
    midWeight[2] = 1.5; 
    midWeight[3] = 1.25;
    midWeight[4] = 1; 
    midWeight[5] = 0.75;
    midWeight[6] = 0.5; 
    midWeight[7] = 0.25;
    midBackWeight[0] = 1;
    midBackWeight[1] = 0.5;
    midBackWeight[2] = 0.5;

    // Left
    midBackWeight[3] = -0.5;
    midBackWeight[4] = -0.5;
    midBackWeight[5] = -1;
    midWeight[8] = -0.25; 
    midWeight[9] = -0.5;
    midWeight[10] = -0.75; 
    midWeight[11] = -1;  
    midWeight[12] = -1.25; 
    midWeight[13] = -1.5;
    midWeight[14] = 0; 
    midWeight[15] = 0; 
}

void IRReceiver::updateValue()
{   
    leftValue = digitalRead(LEFT_BEGIN);
    rightValue = digitalRead(RIGHT_BEGIN);
    for (int i = 0; i < MID_IR_COUNT; ++i)
    {
        midValue[i] = digitalRead(MID_BEGIN + i);
        if (!turn && !ahead)
        {
            totalMidValue[i] = max(totalMidValue[i], midValue[i]);
            totalMidBackValue[i] = min(totalMidBackValue[i], midBackValue[i]);
        }
        else
        {
            totalMidValue[i] = min(totalMidValue[i], midValue[i]);
            totalMidBackValue[i] = min(totalMidBackValue[i], midBackValue[i]);
        }
    }
    for (int i = 0; i < MID_BACK_IR_COUNT; ++i)
        midBackValue[i] = digitalRead(MID_BACK_BEGIN + i);
}

/*
    判断当前位置是否处于十字路口正中央，如果是（可能立即转弯）就返回true
    
    1. 这个函数的重点在于atCross这个变量，当前置红外碰线时被置位true，标志着进入预备转弯状态
    2. 只有atCross等于true时，中间红外碰黑线线才会返回true
*/
bool IRReceiver::atCrossroad(int angle)
{   
    StateMachine &sm = StateMachine::getInstance();

    // 前置红外熄灭个数
    int midCount = 0;
    for (int i = 0; i < MID_IR_COUNT; ++i)
        midCount += totalMidValue[i];

    // 转弯结束
    if (turn)
    {
        if (AngleControl::getAngleDist() < 10 && millis() - sm.lastCrossTime > 800)
        {
            turn = false;
            Serial.println("END TURN at time " + String(millis()));
        }
    } 
    // 直走结束
    else if (ahead)
    {   
        if ((leftValue == IR_DETECT || rightValue == IR_DETECT) && millis() - restartTime > 300)
        {
            ahead = false;
            Serial.println("END AHEAD at time " + String(millis()));
        }
    }
    // 过交叉线
    else if (!turn && !ahead)
    {   
        if (midCount >= 14 || sm.restart)
        {
            Serial.println("CROSS at time " + String(millis()));
            if (sm.restart)
            {
                Serial.println("Restart! ");
                sm.restart = false;
                restartTime = millis();
            }
                    
            if (angle == 90 || angle == -90)
            {
                sm.lastCrossTime = millis();
                turn = true;
            }
            else ahead = true;

            for (int i = 0; i < MID_IR_COUNT; ++i)
                totalMidValue[i] = 0;

            return true;
        }
    }
    return false;
}

/*
    1.在迷宫内的两个状态，根据中轴线输出offset较正位置（sm.outsideTarget用于判断当前正在走第几条路径）
    2.在迷宫外的两个状态，根据红外巡线输出offset较正位置

    offset > 0 强制向右偏转
    offset < 0 强制向左偏转
*/
double IRReceiver::angleOffset()
{   
    if (turn) return 0;
    StateMachine &sm = StateMachine::getInstance();
    double offset = 0;
    if (sm.nowMission == SEARCH_MAZE || sm.nowMission == GO_OUT_MAZE)
    {
        if (Motor::targetSpeed > 0)
        {
            for (int i = 0; i < MID_IR_COUNT; ++i)
                offset += midWeight[i] * (midValue[i] == IR_DETECT);
        }
        else
        {   
            for (int i = 0; i < MID_BACK_IR_COUNT; ++i)
                offset += midBackWeight[i] * (midBackValue[i] == IR_DETECT);
        }
    }
    else if (sm.nowMission == GO_TO_MAZE)
    {
        if (sm.outsideTarget.size() == 2)
            offset = (sm.nowPosition.X - sm.midLine) * ZIGBEE_OFFSET;

    }
    else if (sm.nowMission == RETURN)
    {   
        if (sm.outsideTarget.size() == 1)
            offset = (sm.nowPosition.X - sm.midLine) * ZIGBEE_OFFSET;
    }
    return offset;
}

int IRReceiver::leftValue = 0;
int IRReceiver::rightValue = 0;
int IRReceiver::restartTime = 0;
int IRReceiver::midValue[MID_IR_COUNT];
int IRReceiver::totalMidValue[MID_IR_COUNT];
int IRReceiver::midBackValue[MID_BACK_IR_COUNT];
int IRReceiver::totalMidBackValue[MID_BACK_IR_COUNT];
double IRReceiver::midWeight[MID_IR_COUNT];
double IRReceiver::midBackWeight[MID_BACK_IR_COUNT];
bool IRReceiver::turn = false;
bool IRReceiver::ahead = false;