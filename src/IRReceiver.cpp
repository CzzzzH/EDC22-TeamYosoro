#include "IRReceiver.h"
#include "MotorControl.h"
#include "statemachine.h"
#include "information.h"
#include "Maze.h"

#define ZIGBEE_OFFSET 0.1

void IRReceiver::initialize()
{
    memset(leftValue, 0, sizeof(leftValue));
    memset(midValue, 0, sizeof(midValue));
    memset(rightValue, 0, sizeof(rightValue));
    for (int i = 0; i < SIDE_IR_COUNT; i++)
    {
        pinMode(LEFT_BEGIN + i, INPUT);
        pinMode(RIGHT_BEGIN + i, INPUT);
    }
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
    for (int i = 0; i < SIDE_IR_COUNT; ++i)
    {
        leftValue[i] = digitalRead(LEFT_BEGIN + i);
        rightValue[i] = digitalRead(RIGHT_BEGIN + i);
    }
    for (int i = 0; i < MID_IR_COUNT; ++i)
        midValue[i] = digitalRead(MID_BEGIN + i);
    // if (leftValue[2] == SIDE_DETECT) leftBack = true;
    // if (rightValue[2] == SIDE_DETECT) rightBack = true;    
}

 /*
    判断当前位置是否处于十字路口正中央，如果是（可能立即转弯）就返回true
    
    1. 这个函数的重点在于atCross这个变量，当前置红外碰线时被置位true，标志着进入预备转弯状态
    2. 只有atCross等于true时，中间红外碰黑线线才会返回true
*/

bool IRReceiver::atCrossroad(int angle)
{   
    StateMachine &sm = StateMachine::getInstance();
    // 当前位置处于十字路口正中央，重置atCross并返回true
    if (atCross && (leftValue[1] == SIDE_DETECT || rightValue[1] == SIDE_DETECT))
    {
        atCross = false;
        Motor::targetSpeed = 30;
        return sm.motorDirection == 1; 
        // ↑ 这里可以看到如果小车是倒走就会返回false，这样设计使得倒走状态不会被判定过路口，而是先调整为正走再过
    }
    else if (!atCross)
    {   

        // 计算前置红外的碰黑线数目
        int midCount = 0;
        for (int i = 0; i < MID_IR_COUNT; ++i)
            midCount += ( (i < 2 || i > 3) && midValue[i] == MID_DETECT);

        
        if (sm.motorDirection == 1)
        {   
            // 正走时前置红外碰到足够多的黑线，则进入预备转弯状态(atCross置为true)
            if (midCount >= 3)
            {
                atCross = true;
                if (angle == 90 || angle == -90)
                    Motor::targetSpeed = 30;
                else
                    Motor::targetSpeed = 30;
            }
        }
        else if (sm.motorDirection == -1)
        {   
            /* 
                反走时前置红外碰到足够多的黑线，则分两种情况进入预备转弯状态(atCross置为true)
                1. backFlag = true，说明倒走已经过了第二根黑线了，置为正走并强制更改上一个经过的十字路口坐标，避免转向错误
                2. backFlag = false，说明倒走刚过第一根线，应该什么都不做，所以把backFlag置为true，以便下一次碰到黑线执行1
                注意不管是1、2状态都会进入预备转弯状态，但是状态2下正式转弯的时候由于还是倒走状态所以会强制返回false（见58行注释）
                这样使得倒退具有严格的完备性
            */
            if (midCount >= 3)
            {
                atCross = true;
                if (backFlag)
                {
                    Motor::targetSpeed = 30;
                    sm.motorDirection = 1;
                    sm.lastMazeIndex = 2 * sm.nowMazeIndex - sm.lastMazeIndex;
                    sm.crossroadAction = Maze::getDirection(sm.lastMazeIndex, sm.nowMazeIndex, sm.insideTarget);
                }
                else backFlag = true;
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
            else if (sm.nowPosition.X < sm.midLine - 1) offset = ZIGBEE_OFFSET;
        }
        // else if (sm.outsideTarget.size() == 1)
        // {
        //     if (sm.nowPosition.Y > 240) offset = ZIGBEE_OFFSET;
        //     else if (sm.nowPosition.Y < 236) offset = -ZIGBEE_OFFSET;
        // }
    }
    else if (sm.nowMission == RETURN)
    {   
        if (sm.outsideTarget.size() == 1)
        {
            if (sm.nowPosition.Y > sm.midLine + 1) offset = -ZIGBEE_OFFSET;
            else if (sm.nowPosition.Y < sm.midLine -1) offset = ZIGBEE_OFFSET;
        }
    }
    return offset;
}

int IRReceiver::leftValue[SIDE_IR_COUNT];
int IRReceiver::rightValue[SIDE_IR_COUNT];
int IRReceiver::midValue[MID_IR_COUNT];
double IRReceiver::midWeight[MID_IR_COUNT];
bool IRReceiver::atCross = false;
bool IRReceiver::backFlag = true;