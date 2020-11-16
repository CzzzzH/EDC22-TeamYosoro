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

    midWeight[0] = 1.5;  // * 0
    midWeight[1] = 1;  // * 0 
    midWeight[2] = 0.5; // * 1
    midWeight[3] = -0.5; // * 1
    midWeight[4] = -1; // * 0
    midWeight[5] = -1.5; // * 0
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
    // if (midCount == 0)
    //     StateMachine::getInstance().nowMission = END_GAME;
}

 /*
    判断当前位置是否处于十字路口正中央，如果是（可能立即转弯）就返回true
    
    1. 这个函数的重点在于atCross这个变量，当前置红外碰线时被置位true，标志着进入预备转弯状态
    2. 只有atCross等于true时，中间红外碰黑线线才会返回true
*/

bool IRReceiver::atCrossroad(int angle)
{   
    StateMachine &sm = StateMachine::getInstance();
    // 转弯结束
    if (turn && AngleControl::getAngleDist() < 10 && millis() - sm.lastCrossTime > 1000) 
        turn = false;
    else if (ahead && (leftValue == SIDE_DETECT || rightValue == SIDE_DETECT))
        ahead = false;
    else if (!turn && !ahead)
    {   
        // 计算前置红外的碰黑线数目
        int midCount = 0;
        for (int i = 0; i < MID_IR_COUNT; ++i)
            midCount += ( (i < 2 || i > 3) && midValue[i] == MID_DETECT);
        if (midCount >= 3 )
        {
            if (angle) turn = true;
            else ahead = true;
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
    if (AngleControl::getAngleDist() > 30) return 0;
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