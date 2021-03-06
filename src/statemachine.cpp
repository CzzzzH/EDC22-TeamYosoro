#include "statemachine.h"
#include "AngleControl.h"
#include "IRReceiver.h"
#include "JY61.h"
#include "LED.h"
#include "Maze.h"
#include "MotorControl.h"
#include "information.h"
#include <Arduino.h>
#include <string.h>
#include <MsTimer2.h>

// 中断异常
void StateMachine::interruptionFunction()
{
    // Serial.println("Start int: " + String(millis()));
    IRReceiver::updateValue();
    // Serial.print("OK2");
    JY61::read();
    // Serial.print("OK3");
    AngleControl::Compute();
    // Serial.print("OK4");
    Motor::PID_compute();
    // Serial.print("OK5");
    process();
    // Serial.print("OK6");
    // Motor::targetSpeed = 45;
    Motor::updatePWM();
    // Serial.println("End int: " + String(millis()));
}

// 状态机初始化（也是整个程序的初始化）
void StateMachine::init()
{
    // 设置Debug模式
    JY61::isDebug = false;
    Motor::isDebug = false;

    // 初始化串口
    Serial.begin(115200);
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

    // 先初始化一些固定信息
    nowMazeIndex = 0;
    nextMazeIndex = 5;
    lastCrossTime = nowCrossTime = 0;
    lastScore = nowScore = 0;
    nowHalf = FIRST_HALF;
    nowMission = SEARCH_MAZE;

// 阻塞接收上位机的游戏开始信号，以得到必要的比赛信息进行后续初始化
#ifdef USE_ZIGBEE
    Information::updateInfo();
    while (Information::getGameState() != GameGoing)
        Information::updateInfo();
    updateInfo();
#endif

    Serial.println("Get Start Signal!");

    // 设置第一步直线走的中轴线，就是小车初始的X坐标
    midLine = Information::getCarposX();

    // 初始化迷宫（现在有障碍物信息了）
    Maze::initialize();

    // backTime指当前已经过的时间（单位为0.1s），过了这个时间小车就会强制返回起点
    if (nowHalf == FIRST_HALF)
    {
        /*
            如果是上半场，那只点灯就行了
            添加我们算法生成的障碍物位置
            按我的算法它到那就会自动停下了（因为目标集合变空）
        */
        // Maze::putBlock();
        Maze::ourTrick.push_back(3);
        Maze::ourTrick.push_back(15);
        Maze::ourTrick.push_back(17);
        Maze::ourTrick.push_back(5);
        Maze::ourTrick.push_back(3);
        Maze::ourTrick.push_back(15);
        Maze::ourTrick.push_back(17);
        Maze::ourTrick.push_back(5);
        insideTarget.push_back(Maze::ourTrick.front());
        Maze::ourTrick.erase(Maze::ourTrick.begin());
        // insideTarget.push_back(30);
        // insideTarget.push_back(27);
        // insideTarget.push_back(6);
        // insideTarget.push_back(12);
        // insideTarget.push_back(15);
        // insideTarget.push_back(20);
    }
    else
    {
        // 下半场不需要在这加目标点，updateInfo里会自己加的
    }

    // 初始化进迷宫前的坐标
    outsideTarget.push_back({172, 17});
    outsideTarget.push_back({16, 18});

    /*
        小车初始任务：
        标准模式下，应该写 GO_TO_MAZE（因为执行到这的时候已经接收到上位机的游戏开始信号了，所以需要出发）
        其他写法用于调试：
        1. WAIT_FOR_START: 该任务状态下小车不会动
        2. SEARCH_MAZE: 该任务状态下小车会直接开始在迷宫进行搜寻工作
        3. RETURN: 该任务状态可以测试小车走出迷宫到返回起点的一段路
    */

    // 求第一次转弯的路径
    crossroadAction = Maze::getDirection(int8_t(nowMazeIndex), int8_t(nextMazeIndex), insideTarget);

    // 小车的初始方向和速度
    motorDirection = 1;
    Motor::targetSpeed = AHEAD_SPEED;

    // 最后再初始化中断
    MsTimer2::set(10, interruptionFunction);
    MsTimer2::start();
    // interruptionFunction();
    //
    Serial.println("Init Complete!");
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
    // exceptionHandle();
    updateAction();
    updateMission();
    updateMotor();
    printDebugInfo();
    counter++;
}

void StateMachine::exceptionHandle()
{
    if (nowMission != SEARCH_MAZE && nowMission != GO_OUT_MAZE)
        return;
    if (millis() - lastCrossTime > 20000)
        nowMission = END_GAME;
}

// 上位机信息更新（注意必须在Loop中执行而不是中断，避免阻塞超过中断时间)
void StateMachine::updateInfo()
{
    // 直接调用zigbee的接收函数，更新一系列信息
    // Serial.print("[US]");
    Information::updateInfo();

    /*
        如果当前是在下半场且现在还在进入迷宫或者搜寻迷宫的过程
        那我们就需要更新物资、病人的信息了（其实就是要去的目标节点）
        这里的具体实现不用太关心，大概思路就是遍历info的所有信息，如果还没有加入到target里，就加进去
        注意运输病人先要经过起点再经过目标点，至于判断当前应该是去起点还是目标
        靠Information::getCartransport()这个函数就可以知道车上是否载有病人了
    */
    if (nowHalf == SECOND_HALF && (nowMission == GO_TO_MAZE || nowMission == SEARCH_MAZE))
    {
        // 添加物资包到target中
        for (int i = 0; i < 6; ++i)
        {
            PackageInfo package = Information::Package[i];
            if (package.whetherpicked)
                continue;
            int packageIndex = Information::positonTransform(package.pos);

            if (Information::indexNotExist(packageIndex) && packageIndex != nextMazeIndex && packageIndex != nowMazeIndex)
            {
                insideTarget.push_back(packageIndex);
                Serial.println("Add Package at " + String(packageIndex));
                addNew = true;
            }
        }
        // 添加患者和目的地到target中
        if (!Information::getCartransport())
        {
            int targetIndex = Information::positonTransform(Information::Passenger.startpos);
            if (Information::indexNotExist(targetIndex) && targetIndex != nextMazeIndex)
            {
                insideTarget.push_back(targetIndex);
                Serial.println("Add Patient at " + String(targetIndex));
                addNew = true;
            }
        }
        else if (Information::getCartransport())
        {
            int targetIndex = Information::positonTransform(Information::Passenger.finalpos);
            if (Information::indexNotExist(targetIndex) && targetIndex != nextMazeIndex)
            {
                insideTarget.push_back(targetIndex);
                Serial.println("Add Hospital at " + String(targetIndex));
                addNew = true;
            }
        }
    }
    // Serial.print("[UE]");
}

// 动作更新
void StateMachine::updateAction()
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
        // 如果是上半场，那就隔一段时间加一个目标点
        if (nowHalf == FIRST_HALF && counter % 500 == 0 && counter > 499 && !Maze::ourTrick.empty())
        {
            Serial.println(counter);
            insideTarget.push_back(Maze::ourTrick.front());
            crossroadAction = Maze::getDirection(nowMazeIndex, nextMazeIndex, insideTarget);
            Maze::ourTrick.erase(Maze::ourTrick.begin());
        }

        // 如果目标集合为空，点亮灯并且停下（显然只有新的目标出现才会继续启动）
        if (stop && insideTarget.empty())
        {
            Motor::targetSpeed = 0;
            LED::ledOn();
            if (stop)
                restart = true;
        }
        // 目标集合不为空，那就做的事多了
        else
        {
            // 设置LED和车速
            LED::ledOff();
            stop = false;

            // 如果当前物资更新了，重新寻路
            if (addNew)
            {
                addNew = false;
                crossroadAction = Maze::getDirection(int8_t(nowMazeIndex), int8_t(nextMazeIndex), insideTarget);
                Serial.println("Add New!");
            }

            /*
                这里请仔细阅读 IRReceiver::atCrossroad() 函数!!!
                否则可能很难理解是怎么实现转弯的
            */
            if (IRReceiver::atCrossroad(crossroadAction.rotateAngle))
            {

#ifdef DEBUG_MAZE_POS
                Serial.println("==============================Begin Cross==============================");
                Serial.println("===============BEFORE CROSS===============");
                Serial.println("NowMazeIndex: " + String(nowMazeIndex));
                Serial.println("NextMazeIndex: " + String(nextMazeIndex));
                if (insideTarget.empty())
                    Serial.println("Target Empty !");
                else
                {
                    Serial.println("Inside Target: ");
                    for (auto x : insideTarget)
                        Serial.print(String(x) + " ");
                    Serial.println();
                }

#endif

#ifdef DEBUG_CROSS_ACTION
                Serial.println("CrossActionAngle: " + String(crossroadAction.rotateAngle));
                Serial.println("NextPosition: " + String(crossroadAction.nextPosition));
                Serial.println("===============BEFORE CROSS===============");
                Serial.println();
#endif

                if (insideTarget.empty())
                {
                    stop = true;
                    Serial.println("Stop!");
                    IRReceiver::ahead = false;
                    IRReceiver::turn = true;
                }
                else
                {
                    if (crossroadAction.rotateAngle == 180)
                        motorDirection = -motorDirection;
                    else
                        AngleControl::target += crossroadAction.rotateAngle;

                    if (nextMazeIndex == insideTarget.front())
                        insideTarget.pop_front();

                    // 更新上一个交叉点的序号和当前交叉点的序号（依赖Maze的返回结果），再进行一次寻路
                    nowMazeIndex = nextMazeIndex;
                    nextMazeIndex = crossroadAction.nextPosition;
                    crossroadAction = Maze::getDirection(int8_t(nowMazeIndex), int8_t(nextMazeIndex), insideTarget);
                    // crossroadAction = {90, 25};
                }

#ifdef DEBUG_MAZE_POS
                Serial.println("===============AFTER CROSS===============");
                Serial.println("NowMazeIndex: " + String(nowMazeIndex));
                Serial.println("NextMazeIndex: " + String(nextMazeIndex));
                if (insideTarget.empty())
                    Serial.println("Target Empty !");
                else
                {
                    Serial.println("Inside Target: ");
                    for (auto x : insideTarget)
                        Serial.print(String(x) + " ");
                    Serial.println();
                }

#endif

#ifdef DEBUG_CROSS_ACTION
                Serial.println("CrossActionAngle: " + String(crossroadAction.rotateAngle));
                Serial.println("NextPosition: " + String(crossroadAction.nextPosition));
                Serial.println("===============AFTER CROSS===============");
                Serial.println();
                Serial.println("==============================End Cross==============================");
#endif
            }
        }
    }
}

// 更新任务放在最后做
void StateMachine::updateMission()
{
    // 如果当前任务是进入迷宫而且已经走完所有迷宫外目标点了，就把任务切换为搜寻迷宫
    if (nowMission == GO_TO_MAZE && outsideTarget.empty())
        nowMission = SEARCH_MAZE;
    // 如果当前任务是搜寻迷宫且预定的返回时间到了，就把任务切换为退出迷宫，并清空迷宫内目标，只留下一个出口
    else if (nowMission == SEARCH_MAZE && millis() > 130000)
    {
        Serial.println("[End Game at " + String(millis()) + "]");
        nowMission = END_GAME;
        // insideTarget.clear();
        // insideTarget.push_back(0);
        // if (motorDirection == -1) lastMazeIndex = 2 * nowMazeIndex - lastMazeIndex;
        // crossroadAction = Maze::getDirection(lastMazeIndex, nowMazeIndex, insideTarget);
        // nowMission = GO_OUT_MAZE;
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
    }
    // 如果当前任务是回程且迷宫外的目标点已经为空，那说明已经到起点了，结束游戏（停车）
    else if (nowMission == RETURN && outsideTarget.empty())
        nowMission = END_GAME;

    lastScore = nowScore;
    nowScore = Information::getCarscore();

    // if (lastScore - nowScore >= 50)
    // {
    //     nowMission = END_GAME;
    //     return;
    // }
}

// 更新电机
void StateMachine::updateMotor()
{
    // 如果当前游戏还没开始（实际不可能出现该情况）或者游戏已经结束，就直接停车
    if (nowMission == WAIT_FOR_START || nowMission == END_GAME)
        Motor::targetSpeed = 0;
    else
        Motor::targetSpeed = fabs(Motor::targetSpeed) * motorDirection;
}

// Debug信息输出
void StateMachine::printDebugInfo()
{
    if (counter % 20 == 0)
    {
#ifdef DEBUG_ANGLECONTROLER
        Serial.println("Now Angle: " + String(JY61::Angle[2]));
        Serial.println("Target Angle: " + String(AngleControl::target));
        Serial.println("Angle Dist: " + String(AngleControl::getAngleDist()));
        Serial.println();
#endif

#ifdef DEBUG_ZIGBEE
        Serial.println("Car Position:  " + String(Information::getCarposX()) + "  " + String(Information::getCarposY()));
#endif

#ifdef DEBUG_TIMER
        Serial.println("Counter: " + String(counter));
        Serial.println("Milli Seconds: " + String(millis()));
#endif

#ifdef DEBUG_POSITION
        if (nowMission == GO_TO_MAZE || nowMission == RETURN)
        {
            Serial.println("NowPos: " + String(Information::getCarposX()) + String(Information::getCarposY()));
            Serial.println("NowMidline: " + String(midLine));
            Serial.println("Target Distance: " + String(nowPosition.getDist(outsideTarget.front())));
        }
#endif
    }
}

Match StateMachine::nowHalf;
Mission StateMachine::nowMission;
Position StateMachine::lastPosition = {0, 0};
Position StateMachine::nowPosition = {0, 0};
CrossroadAction StateMachine::crossroadAction;
int8_t StateMachine::midLine = 0;
int8_t StateMachine::nowMazeIndex;
int8_t StateMachine::nextMazeIndex;
int8_t StateMachine::motorDirection = 1;
uint16_t StateMachine::backTime = 0;
int16_t StateMachine::lastScore;
int16_t StateMachine::nowScore;
uint16_t StateMachine::counter = 0;

uint32_t StateMachine::lastCrossTime;
uint32_t StateMachine::nowCrossTime;

bool StateMachine::restart = false;
bool StateMachine::addNew = false;
bool StateMachine::stop = false;

std::deque<Position> StateMachine::outsideTarget;
std::deque<int8_t> StateMachine::insideTarget;