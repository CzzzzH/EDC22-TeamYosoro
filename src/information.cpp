#include "information.h"
#include "statemachine.h"
#include <deque>
#include <string>

Information &Information::getInstance()
{
    static Information instance;
    return instance;
}

void Information::updateInfo()
{
    std::string zigbeeMessage;
    zigbeeMessage.resize(75);
    // Serial.print("Start Update!!!");
    while (true)
    {
        // Serial.print("Zigbee status: ");
        // Serial.println(Serial3.available());
        // Serial.print("\n");
        if (Serial3.available())
        {
            uint8_t zigbeeBuffer = Serial3.read();
            // Serial.print(zigbeeBuffer);
            message_index = receiveIndexAdd(message_index, 1); 
            zigbeeMessage[message_index] = zigbeeBuffer;

            if (zigbeeMessage[receiveIndexMinus(message_index, 2)] == 0x0D && zigbeeMessage[receiveIndexMinus(message_index, 1)] == 0x0A) //һ����Ϣ�Ľ�β
            {
                // Serial.println("Message Complete!");
                // Serial.println("*** message_head: " + String(message_head));
                // Serial.println("*** message_index: " + String(message_index));
                if (receiveIndexMinus(message_index, message_head) == 0)
                {

                    int index = message_head;
                    for (int i = 0; i < 70; i++)
                    {
                        zigbeeReceive[i] = zigbeeMessage[index];
                        index = receiveIndexAdd(index, 1);
                    }
                    // Serial.println("OK!!!");
                    Decode();
                    // Serial.println("X: " + String(getCarposX()));
                    // Serial.println("Y: " + String(getCarposY()));
                    break;
                }
                else
                    message_head = message_index;
            }
        }
    }
}

enum GameStateEnum Information::getGameState()
{
    uint8_t state = Game.GameState;
    if (state == 0)
    {
        return GameNotStart;
    }
    else if (state == 1)
    {
        return GameGoing;
    }
    else if (state == 2)
    {
        return GamePause;
    }
    else if (state == 3)
    {
        return GameOver;
    }

    return GameNotStart;
}

uint16_t Information::getGameTime()
{
    return Game.Time;
}

uint16_t Information::getGameFlood()
{
    return (uint16_t)Game.stop;
}

uint16_t Information::getPassengerstartposX()
{
    return Passenger.startpos.X;
}

uint16_t Information::getPassengerstartposY()
{
    return Passenger.startpos.Y;
}

struct Position Information::getPassengerstartpos()
{
    return Passenger.startpos;
}

uint16_t Information::getPassengerfinalposX()
{
    return Passenger.finalpos.X;
}
uint16_t Information::getPassengerfinalposY()
{
    return Passenger.finalpos.Y;
}

struct Position Information::getPassengerfinalpos()
{
    return Passenger.finalpos;
}

uint16_t Information::getFloodposX(int FloodNo)
{
    return Flood[FloodNo].pos.X;
}

uint16_t Information::getFloodposY(int FloodNo)
{
    return Flood[FloodNo].pos.Y;
}

struct Position Information::getFloodpos(int FloodNo)
{
    return Flood[FloodNo].pos;
}

uint16_t Information::getCarposX()
{
    return (uint16_t)Car.pos.X;
}

uint16_t Information::getCarposY()  
{
    return (uint16_t)Car.pos.Y;
}

struct Position Information::getCarpos()
{
    return Car.pos;
}

uint16_t Information::getPackageposX(int PackNo)
{
    if (PackNo != 0 && PackNo != 1 && PackNo != 2 && PackNo != 3 && PackNo != 4 && PackNo != 5)
        return (uint16_t)INVALID_ARG;
    else
        return (uint16_t)Package[PackNo].pos.X;
}

uint16_t Information::getPackageposY(int PackNo)
{
    if (PackNo != 0 && PackNo != 1 && PackNo != 2 && PackNo != 3 && PackNo != 4 && PackNo != 5)
        return (uint16_t)INVALID_ARG;
    else
        return (uint16_t)Package[PackNo].pos.Y;
}

uint16_t Information::getPackagewhetherpicked(int PackNo)
{
    if (PackNo != 0 && PackNo != 1 && PackNo != 2 && PackNo != 3 && PackNo != 4 && PackNo != 5)
        return (uint16_t)INVALID_ARG;
    else
        return (uint16_t)Package[PackNo].whetherpicked;
}

struct Position Information::getPackagepos(int PackNo)
{
    return Package[PackNo].pos;
}

uint16_t Information::getCarpicknum()
{
    return (uint16_t)Car.picknum;
}

uint16_t Information::getCartransportnum()
{
    return (uint16_t)Car.transportnum;
}

uint16_t Information::getCartransport()
{
    return (uint16_t)Car.transport;
}

uint16_t Information::getCarscore()
{
    return (uint16_t)Car.score;
}

uint16_t Information::getCartask()
{
    return (uint16_t)Car.task;
}

uint16_t Information::getCararea()
{
    return (uint16_t)Car.area;
}

uint16_t Information::getObstacleAposX(int ObstacleNo)
{
    return (uint16_t)Obstacle[ObstacleNo].posA.X;
}

uint16_t Information::getObstacleAposY(int ObstacleNo)
{
    return (uint16_t)Obstacle[ObstacleNo].posA.Y;
}

uint16_t Information::getObstacleBposX(int ObstacleNo)
{
    return (uint16_t)Obstacle[ObstacleNo].posB.X;
}

uint16_t Information::getObstacleBposY(int ObstacleNo)
{
    return (uint16_t)Obstacle[ObstacleNo].posB.Y;
}

struct Position Information::getObstacleApos(int ObstacleNo)
{
    return Obstacle[ObstacleNo].posA;
}

struct Position Information::getObstacleBpos(int ObstacleNo)
{
    return Obstacle[ObstacleNo].posB;
}

void Information::DecodeBasicInfo()
{
    Game.Time = (zigbeeReceive[0] << 8) + zigbeeReceive[1];
    Game.GameState = (zigbeeReceive[2] & 0xC0) >> 6;
    Game.stop = (zigbeeReceive[2] & 0x07);
}

void Information::DecodeCarInfo()
{
    Car.pos.X = (zigbeeReceive[3]);
    Car.pos.Y = (zigbeeReceive[4]);
    Car.score = (zigbeeReceive[32] << 8) + zigbeeReceive[33];
    Car.picknum = zigbeeReceive[35];
    Car.task = ((zigbeeReceive[2] & 0x20) >> 5);
    Car.transport = ((zigbeeReceive[2] & 0x08) >> 3);
    Car.transportnum = (zigbeeReceive[34]);
    Car.area = ((zigbeeReceive[19] & 0x02) >> 1);
    Car.WhetherRightPos = (zigbeeReceive[19] & 0x01);
}

void Information::DecodePassengerInfo()
{
    Passenger.startpos.X = (zigbeeReceive[15]);
    Passenger.startpos.Y = (zigbeeReceive[16]);
    Passenger.finalpos.X = (zigbeeReceive[17]);
    Passenger.finalpos.Y = (zigbeeReceive[18]);
}

void Information::DecodePackageAInfo()
{
    Package[0].pos.X = (zigbeeReceive[20]);
    Package[0].pos.Y = (zigbeeReceive[21]);
    Package[0].whetherpicked = ((zigbeeReceive[19] & 0x80) >> 7);
}
void Information::DecodePackageBInfo()
{
    Package[1].pos.X = (zigbeeReceive[22]);
    Package[1].pos.Y = (zigbeeReceive[23]);
    Package[1].whetherpicked = ((zigbeeReceive[19] & 0x40) >> 6);
}

void Information::DecodePackageCInfo()
{
    Package[2].pos.X = (zigbeeReceive[24]);
    Package[2].pos.Y = (zigbeeReceive[25]);
    Package[2].whetherpicked = ((zigbeeReceive[19] & 0x20) >> 5);
}

void Information::DecodePackageDInfo()
{
    Package[3].pos.X = (zigbeeReceive[26]);
    Package[3].pos.Y = (zigbeeReceive[27]);
    Package[3].whetherpicked = ((zigbeeReceive[19] & 0x10) >> 4);
}

void Information::DecodePackageEInfo()
{
    Package[4].pos.X = (zigbeeReceive[28]);
    Package[4].pos.Y = (zigbeeReceive[29]);
    Package[4].whetherpicked = ((zigbeeReceive[19] & 0x08) >> 3);
}

void Information::DecodePackageFInfo()
{
    Package[5].pos.X = (zigbeeReceive[30]);
    Package[5].pos.Y = (zigbeeReceive[31]);
    Package[5].whetherpicked = ((zigbeeReceive[19] & 0x04) >> 2);
}

void Information::DecodeFloodInfo()
{
    Flood[0].pos.X = (zigbeeReceive[5]);
    Flood[0].pos.Y = (zigbeeReceive[6]);
    Flood[1].pos.X = (zigbeeReceive[7]);
    Flood[1].pos.Y = (zigbeeReceive[8]);
    Flood[2].pos.X = (zigbeeReceive[9]);
    Flood[2].pos.Y = (zigbeeReceive[10]);
    Flood[3].pos.X = (zigbeeReceive[11]);
    Flood[3].pos.Y = (zigbeeReceive[12]);
    Flood[4].pos.X = (zigbeeReceive[13]);
    Flood[4].pos.Y = (zigbeeReceive[14]);
}

void Information::DecodeObstacle()
{
    int i;
    for (i = 0; i < 8; i++)
    {
        Obstacle[i].posA.X = (zigbeeReceive[36 + i * 4]);
        Obstacle[i].posA.Y = (zigbeeReceive[37 + i * 4]);
        Obstacle[i].posB.X = (zigbeeReceive[38 + i * 4]);
        Obstacle[i].posB.Y = (zigbeeReceive[39 + i * 4]);
    }
}

void Information::Decode()
{
    DecodeBasicInfo();
    DecodeCarInfo();
    DecodePassengerInfo();
    DecodePackageAInfo();
    DecodePackageBInfo();
    DecodePackageCInfo();
    DecodePackageDInfo();
    DecodePackageEInfo();
    DecodePackageFInfo();
    DecodeFloodInfo();
    DecodeObstacle();
}

int Information::receiveIndexMinus(int index_h, int num)
{
    if (index_h - num >= 0)
    {
        return index_h - num;
    }
    else
    {
        return index_h - num + ZIGBEE_MESSAGE_LENTH;
    }
}

int Information::receiveIndexAdd(int index_h, int num)
{
    if (index_h + num < ZIGBEE_MESSAGE_LENTH)
    {
        return index_h + num;
    }
    else
    {
        return index_h + num - ZIGBEE_MESSAGE_LENTH;
    }
}

int Information::positonTransform(Position &pos)
{
    int xOffset = (pos.X - 37)/30 + 1;
    int yOffset = (pos.Y - 37)/30;
    return (xOffset + yOffset * MAZE_SIZE);
}

bool Information::indexNotExist(int index)
{
    std::deque<int> &insideTarget = StateMachine::getInstance().insideTarget;
    return std::find(insideTarget.begin(), insideTarget.end(), index) == insideTarget.end();
}