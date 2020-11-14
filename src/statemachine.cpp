#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>
#include "statemachine.h"
#include "AngleControl.h"
#include "MotorControl.h"
#include "TimerInterrupt.h"
#include "JY61.h"
#include "IRReceiver.h"
#include "Maze.h"
#include "information.h"
#include "LED.h"

// 各种Define，用于debug

// #define DEBUG_MOTOR
// #define DEBUG_ANGLECONTROLER
// #define DEBUG_IRRECEIVER
// #define DEBUG_ZIGBEE
// #define DEBUG_TIMER

// 获取状态机的实例（在其他源文件要用到状态机时调用这个函数就能获得状态机实例引用） 
StateMachine &StateMachine::getInstance()
{
    static StateMachine instance;
    return instance;
}

// 状态机初始化（也是整个程序的初始化）
void StateMachine::init()
{
    // 获取状态机的实例
    Information &info = Information::getInstance();
    
    // 设置Debug模式
    JY61::isDebug = false;
    Motor::isDebug = false;

    // 初始化串口
    Serial.begin(9600);
    Serial2.begin(115200);
    Serial3.begin(115200);

    /*
        在接收上位机信号之前初始化各种组件
        几点注意：
        1. 这里不能初始化时钟中断，避免提前进入时钟中断
        2. 这里不能初始化迷宫地图，因为还没有障碍物信息
    */
    Motor::initialize();
    Motor::targetSpeed = 0;
    AngleControl::initialize();
    IRReceiver::initialize();
    LED::initialize();

    // 阻塞接收上位机的游戏开始信号，以得到必要的比赛信息进行后续初始化
    info.updateInfo();
    while (info.getGameState() != GameGoing)
        info.updateInfo();
    
    // 设置第一步直线走的中轴线，就是小车初始的X坐标
    midLine = info.getCarposX();

    // 设置上下半场
    nowHalf = FIRST_HALF;

    // 初始化迷宫（现在有障碍物信息了）
    Maze::initialize(Information::getInstance());

    // backTime指当前已经过的时间（单位为0.1s），过了这个时间小车就会强制返回起点
    if (nowHalf == FIRST_HALF)
    {
        /*
            如果是上半场，那只点灯就行了
            我就随便加了个迷宫中心的目标点作为唯一目标
            按我的算法它到那就会自动停下了（因为目标集合变空）
        */
        insideTarget.push_back(35);
        insideTarget.push_back(31);
        backTime = 10;
        // Serial.println("nowHalf : " + String(nowHalf));
        // Serial.println("insideTarget size : " + String(insideTarget.size()));
    }
    else
    {
        // 下半场不需要在这加目标点，updateInfo里会自己加的
        backTime = 50;
    }

    // 初始化进迷宫前的坐标（小车会先走到{16, 244}，然后转弯然后再走到{87, 236}
    outsideTarget.push_back({16, 244});
    outsideTarget.push_back({87, 236});

    /*
        小车初始任务：
        标准模式下，应该写 GO_TO_MAZE（因为执行到这的时候已经接收到上位机的游戏开始信号了，所以需要出发）
        其他写法用于调试：
        1. WAIT_FOR_START: 该任务状态下小车不会动
        2. SEARCH_MAZE: 该任务状态下小车会直接开始在迷宫进行搜寻工作
        3. RETURN: 该任务状态可以测试小车走出迷宫到返回起点的一段路
    */
    nowMission = SEARCH_MAZE;
    
    // 小车在迷宫中的初始序号
    nowMazeIndex = 32;
    lastMazeIndex = 38;

    // 求第一次转弯的路径
    crossroadAction = Maze::getDirection(lastMazeIndex, nowMazeIndex, insideTarget);
    
    // 小车的初始方向和速度
    motorDirection = 1;
    Motor::targetSpeed = 30;

    // 最后再初始化中断
    TimerInterrupt::initialize(interrupt_period);
}

/*
    每50ms时钟中断都会进入的过程，也是整个状态机的运行流程
    主要分三个阶段：
    1. updateAction: 更新小车的动作，包括寻路、转弯、点灯等具体操作
    2. updateMission: 更新小车的任务，一共分为
       [进入迷宫 -> 迷宫搜寻 -> 退出迷宫 -> 返回起点]
       四大阶段，中间两个阶段是在迷宫中进行的，前后两个阶段是在迷宫外进行的
    3. updateMotor: 主要是最后确认小车的速度方向和大小，然后执行PID调速
    另外还有个updateInfo的函数不在这里执行，而是在Loop执行
*/
void StateMachine::process()
{
    Information &info = Information::getInstance();
    updateAction(info);
    updateMission(info);
    updateMotor(info);
    // Serial.println(info.getGameTime());
    // counter++;

// 一些debug代码
#ifdef DEBUG_MOTOR
    Serial.println("Target Speed: " + String(Motor::targetSpeed));
    Serial.println("Left Motor Counter: " + String(encoder::counter.left));
    Serial.println("Right Motor Counter: " + String(encoder::counter.right));
#endif

#ifdef DEBUG_ANGLECONTROLER
    Serial.println("Now Angle: " + String(JY61::Angle[2]));
    Serial.println("Target Angle: " + String(AngleControl::target));
#endif

#ifdef DEBUG_IRRECEIVER
    Serial.println("LeftIR: " + String(IRReceiver::leftValue[0]) + " " + String(IRReceiver::leftValue[1]) + " " + String(IRReceiver::leftValue[2]));
    Serial.println("RightIR: " + String(IRReceiver::rightValue[0]) + " " + String(IRReceiver::rightValue[1]) + " " + String(IRReceiver::rightValue[2]));
    Serial.println("MidIR: " + String(IRReceiver::midValue[0]) + " " + String(IRReceiver::midValue[1]) + " " + String(IRReceiver::midValue[2]) + " " + String(IRReceiver::midValue[3]));
#endif

#ifdef DEBUG_ZIGBEE
    Serial.println("Car Position:  " + String(info.getCarposX()) + "  " + String(info.getCarposY()));
#endif

#ifdef DEBUG_TIMER
    Serial.println("Counter: " + String(counter));
    Serial.println("Milli Seconds: " + String(millis()));
#endif
}

// 上位机信息更新（注意必须在Loop中执行而不是中断，避免阻塞超过中断时间)
void StateMachine::updateInfo()
{
    // 获取info的实例
    Information &info = Information::getInstance();

    // 直接调用zigbee的接收函数，更新一系列信息
    info.updateInfo();
    // Serial.println("Update Success!");
    // 记录当前坐标
    nowPosition = info.getCarpos();
    /*
        如果当前是在下半场且现在还在进入迷宫或者搜寻迷宫的过程
        那我们就需要更新物资、病人的信息了（其实就是要去的目标节点）
        这里的具体实现不用太关心，大概思路就是遍历info的所有信息，如果还没有加入到target里，就加进去
        注意运输病人先要经过起点再经过目标点，至于判断当前应该是去起点还是目标
        靠info.getCartransport()这个函数就可以知道车上是否载有病人了
    */
    if (insideTarget.empty() && nowHalf == SECOND_HALF && (nowMission == GO_TO_MAZE || nowMission == SEARCH_MAZE))
    {
        // 添加物资包到target中
        bool addNew = false;
        for (int i = 0; i < 6; ++i)
        {
            PackageInfo package = info.Package[i];
            if (package.whetherpicked) continue;
            int packageIndex = info.positonTransform(package.pos);
            if (info.indexNotExist(packageIndex))
            {
                insideTarget.push_back(packageIndex);
                addNew = true;
            }
        }
        // 初步debug的时候，建议注释掉下面的代码，只加入物资
        // if (!havePatient)
        // {
        //     int targetIndex = info.positonTransform(info.Passenger.startpos);
        //     if (info.indexNotExist(targetIndex))
        //     {
        //         insideTarget.push_back(targetIndex);
        //         addNew = true;
        //     }
        // }
        // else if (havePatient)
        // {
        //     int targetIndex = info.positonTransform(info.Passenger.finalpos);
        //     if (info.indexNotExist(targetIndex))
        //     {
        //         insideTarget.push_back(targetIndex);
        //         addNew = true;
        //     }
        // }
        if (addNew)
            crossroadAction = Maze::getDirection(lastMazeIndex, nowMazeIndex, insideTarget);
    }
}

// 动作更新
void StateMachine::updateAction(Information &info)
{
    /*
        这里是迷宫外的两个任务状态的动作执行
        它的思路就是判断当前坐标和队首目标的坐标（即将前往的坐标）距离是不是低于一个值
        如果是就认为到达了，做相应动作并把队首目标pop掉
    */
    if (nowMission == GO_TO_MAZE || nowMission == RETURN)
    {
        // 计算距离
        double distance = nowPosition.getDist(outsideTarget.front());
        // Serial.println("Now Target: " + String(outsideTarget.front().X) + " " + String(outsideTarget.front().Y));
        // Serial.println("Now Pos: " + String(nowPosition.X) + " " + String(nowPosition.Y));
        // Serial.println("Mid Line: " + String(midLine));
        // Serial.println("Now dist: " + String(distance));

        // 如果距离小于某个值，就执行动作（转弯，这个是硬编码进去的），然后pop掉队首
        if (distance < 15)
        {
            outsideTarget.pop_front();
            switch (outsideTarget.size())
            {
            case 1:
                AngleControl::target += 90;
                break;
            case 0:
                AngleControl::target += 90;
                break;
            default:
                break;
            }
        }
    }
    /*
        这里是迷宫内的两个任务状态的动作执行
        它的思路就是不断把target集合丢给Maze组件让它寻路，返回一个动作
    */
    else if (nowMission == SEARCH_MAZE || nowMission == GO_OUT_MAZE)
    {   
        // 如果目标集合为空，点亮灯并且停下（显然只有新的目标出现才会继续启动）
        if (insideTarget.empty())
        {
            // Serial.println("insideTarget: fuck your shit");
            Motor::targetSpeed = 0;
            LED::ledOn();
        }
        // 目标集合不为空，那就做的事多了
        else
        {   
            // 设置LED和车速
            Motor::targetSpeed = 30;
            LED::ledOff();

            // 通过Maze寻路得到一个结构体，包含转角和下一个交叉点的序号
            // Serial.println("Begin crossroad: " + String(lastMazeIndex) + " " + String(nowMazeIndex) + " " + String(insideTarget.front()));
            
            /*
                这里请仔细阅读 IRReceiver::atCrossroad() 函数!!!
                否则可能很难理解是怎么实现转弯的
            */
            if (IRReceiver::atCrossroad(crossroadAction.rotateAngle))
            {
                // 如果是要转180度，那就不转，把motorDirection取反（表示方向取反）
                if (crossroadAction.rotateAngle == 180)
                {
                    motorDirection = -motorDirection;
                    if (motorDirection == -1) IRReceiver::backFlag = false;
                }
                // 否则改变目标角度让车转弯
                else AngleControl::target += crossroadAction.rotateAngle;

                // 更新上一个交叉点的序号和当前交叉点的序号（依赖Maze的返回结果），再进行一次寻路
                lastMazeIndex = nowMazeIndex;
                nowMazeIndex = crossroadAction.nextPosition;
                
                /*
                    如果当前交叉点（也就是下一个到达的交叉点）序号正好是最近的目标
                    那 
                */
                if (nowMazeIndex == insideTarget.front())
                {
                    if (nowMazeIndex == info.positonTransform(info.Passenger.startpos))
                        havePatient = true;
                    else if (nowMazeIndex == info.positonTransform(info.Passenger.finalpos))
                        havePatient = false;
                    insideTarget.pop_front();
                }
                if (!insideTarget.empty())
                    crossroadAction = Maze::getDirection(lastMazeIndex, nowMazeIndex, insideTarget);
            }
        }
        
    }
}

// 更新任务放在最后做
void StateMachine::updateMission(Information &info)
{   
    // 如果当前任务是进入迷宫而且已经走完所有迷宫外目标点了，就把任务切换为搜寻迷宫
    if (nowMission == GO_TO_MAZE && outsideTarget.empty())
        nowMission = SEARCH_MAZE;
    // 如果当前任务是搜寻迷宫且预定的返回时间到了，就把任务切换为退出迷宫，并清空迷宫内目标，只留下一个出口
    else if (nowMission == SEARCH_MAZE && info.getGameTime() > backTime)
    {
        insideTarget.clear();
        insideTarget.push_back(0);
        crossroadAction = Maze::getDirection(lastMazeIndex, nowMazeIndex, insideTarget);
        nowMission = GO_OUT_MAZE;
    }
    /*
        如果当前任务是退出迷宫且迷宫内节点（任务开始时就只有一个出口）
        就把任务切换为回程，然后加入两个迷宫外目标点指导回程
        之所以现在才加而不是初始化就加，是因为我们的任务状态切换依赖于目标队列（集合）是否为空
    */
    else if (nowMission == GO_OUT_MAZE && insideTarget.empty())
    {

        // 如果想在单独调试迷宫内行为的化，可以把nowMission换成下面的END_GAME直接结束游戏
        nowMission = RETURN; 
        // nowMission = END_GAME;

        // 更新中轴线（因为这段路也不短，所以必须要用上位机较正方向）
        midLine = nowPosition.Y;
        outsideTarget.push_back({172, 8});
        outsideTarget.push_back({5, 12});
    }
    // 如果当前任务是回程且迷宫外的目标点已经为空，那说明已经到起点了，结束游戏（停车）
    else if (nowMission == RETURN && outsideTarget.empty())
        nowMission = END_GAME;
}

// 更新电机
void StateMachine::updateMotor(Information &info)
{
    // 如果当前游戏还没开始（实际不可能出现该情况）或者游戏已经结束，就直接停车
    if (nowMission == WAIT_FOR_START || nowMission == END_GAME) Motor::targetSpeed = 0;
    else Motor::targetSpeed = fabs(Motor::targetSpeed) * motorDirection;
}