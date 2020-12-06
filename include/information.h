#ifndef INFORMATION_H
#define INFORMATION_H

#include <Arduino.h>
#include "util.h"
#define MAZE_SIZE 6
#define OBSTACLE 8
#define INVALID_ARG -1
#define ZIGBEE_MESSAGE_LENTH 70

class Information
{
public:
    static BasicInfo Game;
    static CarInfo Car;
    static PassengerInfo Passenger;
    static PackageInfo Package[6];
    static FloodInfo Flood[5];
    static ObstacleInfo Obstacle[8];

    static uint8_t zigbeeReceive[ZIGBEE_MESSAGE_LENTH];
    static uint8_t zigbeeMessage[ZIGBEE_MESSAGE_LENTH];
    static int message_index;
    static int message_head;

    static void Decode();
    static void DecodeBasicInfo();
    static void DecodeCarInfo();
    static void DecodePassengerInfo();
    static void DecodePackageAInfo();
    static void DecodePackageBInfo();
    static void DecodePackageCInfo();
    static void DecodePackageDInfo();
    static void DecodePackageEInfo();
    static void DecodePackageFInfo();
    static void DecodeFloodInfo();
    static void DecodeObstacle();

    static int receiveIndexMinus(int index_h, int num);
    static int receiveIndexAdd(int index_h, int num);

    static enum GameStateEnum getGameState(void);
    static void updateInfo();
    static uint16_t getGameTime(void);
    static uint16_t getPassengerstartposX(void);
    static uint16_t getPassengerstartposY(void);
    static Position getPassengerstartpos(void);
    static uint16_t getPassengerfinalposX(void);
    static uint16_t getPassengerfinalposY(void);
    static Position getPassengerfinalpos(void);
    static uint16_t getGameFlood(void);
    static uint16_t getFloodposX(int FloodNo);
    static uint16_t getFloodposY(int FloodNo);
    static Position getFloodpos(int FloodNo);
    static uint16_t getCarposX();
    static uint16_t getCarposY();
    static Position getCarpos();
    static uint16_t getPackageposX(int PackNo);
    static uint16_t getPackageposY(int PackNo);
    static uint16_t getPackagewhetherpicked(int PackNo);
    static Position getPackagepos(int PackNo);
    static uint16_t getCarpicknum();
    static uint16_t getCartransportnum();
    static uint16_t getCartransport();
    static uint16_t getCarscore();
    static uint16_t getCartask();
    static uint16_t getCararea();
    static uint16_t getObstacleAposX(int ObstacleNo);
    static uint16_t getObstacleAposY(int ObstacleNo);
    static uint16_t getObstacleBposX(int ObstacleNo);
    static uint16_t getObstacleBposY(int ObstacleNo);
    static Position getObstacleApos(int ObstacleNo);
    static Position getObstacleBpos(int ObstacleNo);
    static int positonTransform(Position &pos);
    static bool indexNotExist(int index);
};

#endif
