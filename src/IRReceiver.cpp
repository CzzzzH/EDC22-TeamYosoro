#include "IRReceiver.h"
#include "MotorControl.h"
#include "statemachine.h"
#include "information.h"
#include "Maze.h"
#include "AngleControl.h"

#define ZIGBEE_OFFSET 0.5

void IRReceiver::initialize()
{
    memset(midValue, 0, sizeof(midValue));
    pinMode(LEFT_BEGIN, INPUT);
    pinMode(RIGHT_BEGIN, INPUT);
    for (int i = 0; i < MID_IR_COUNT; i++)
        pinMode(MID_BEGIN + i, INPUT);

    // Right 
    midWeight[0] = 3.5; 
    midWeight[1] = 3;  
    midWeight[2] = 2.5; 
    midWeight[3] = 2;
    midWeight[4] = 1.5; 
    midWeight[5] = 1;
    midWeight[6] = 0.5; 
    midWeight[7] = 0.5;  

    // Left
    midWeight[8] = -0.5; 
    midWeight[9] = -0.5;
    midWeight[10] = -1; 
    midWeight[11] = -1.5;  
    midWeight[12] = -2; 
    midWeight[13] = -2.5;
    midWeight[14] = -3; 
    midWeight[15] = -3.5; 
}

void IRReceiver::updateValue()
{   
    int midCount = 0;
    leftValue = digitalRead(LEFT_BEGIN);
    rightValue = digitalRead(RIGHT_BEGIN);
    for (int i = 0; i < MID_IR_COUNT; ++i)
    {
        midValue[i] = digitalRead(MID_BEGIN + i);
        midCount += midValue[i] == MID_DETECT;
    }
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
        midCount += midValue[i] == MID_DETECT;

    // 转弯结束
    if (turn)
    {
        if (AngleControl::getAngleDist() < 10 && millis() - sm.lastCrossTime > 1000)
            turn = false;
    } 
    // 直走结束
    else if (ahead)
    {   
        // 正走和倒走时判断先后顺序正好相反
        if (sm.motorDirection == 1)
        {
            if (leftValue == SIDE_DETECT || rightValue == SIDE_DETECT)
                ahead = false;
        }
        else
        {
            if (midCount >= 8)
                ahead = false;
        }    
    }
    // 过交叉线
    else if (!turn && !ahead)
    {   
        if (sm.motorDirection == 1)
        {
            if (midCount >= 8)
            {
                if (angle) turn = true;
                else ahead = true;
                return true;
            }
        }
        else
        {
            if (leftValue == SIDE_DETECT || rightValue == SIDE_DETECT)
            {
                if (angle) turn = true;
                else ahead = true;
                return true;
            }
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
        for (int i = 0; i < MID_IR_COUNT; ++i)
            offset += midWeight[i] * (midValue[i] == MID_DETECT);
    }
    else if (sm.nowMission == GO_TO_MAZE)
    {
        if (sm.outsideTarget.size() == 2)
        {
            if (sm.nowPosition.X > sm.midLine + 1) offset = ZIGBEE_OFFSET;
            else if (sm.nowPosition.X < sm.midLine - 1) offset = -ZIGBEE_OFFSET;
        }
    }
    else if (sm.nowMission == RETURN)
    {   
        if (sm.outsideTarget.size() == 1)
        {
            if (sm.nowPosition.Y > sm.midLine + 1) offset = ZIGBEE_OFFSET;
            else if (sm.nowPosition.Y < sm.midLine -1) offset = -ZIGBEE_OFFSET;
        }
    }
    return offset;
}

int IRReceiver::leftValue = 0;
int IRReceiver::rightValue = 0;
int IRReceiver::midValue[MID_IR_COUNT];
double IRReceiver::midWeight[MID_IR_COUNT];
bool IRReceiver::turn = false;
bool IRReceiver::ahead = false;