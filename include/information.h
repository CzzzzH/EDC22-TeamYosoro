#ifndef INFORMATION_H
#define INFORMATION_H

#include <Arduino.h>
#include "util.h"
#define MAZE_SIZE 6
#define OBSTACLE 1
#define INVALID_ARG -1
#define ZIGBEE_MESSAGE_LENTH 70

class Information
{
private:
    Information() {}
    Information &operator=(const Information &) = delete;

public:
    BasicInfo Game;           
    CarInfo Car;              
    PassengerInfo Passenger;  
    PackageInfo Package[6];   
    FloodInfo Flood[5];       
    ObstacleInfo Obstacle[8]; 

    uint8_t zigbeeReceive[ZIGBEE_MESSAGE_LENTH]; 
    uint8_t zigbeeMessage[ZIGBEE_MESSAGE_LENTH]; 
    int message_index = 0;
    int message_head = -1;
    
    void Decode();
    void DecodeBasicInfo();
    void DecodeCarInfo();
    void DecodePassengerInfo();
    void DecodePackageAInfo();
    void DecodePackageBInfo();
    void DecodePackageCInfo();
    void DecodePackageDInfo();
    void DecodePackageEInfo();
    void DecodePackageFInfo();
    void DecodeFloodInfo();
    void DecodeObstacle();

    int receiveIndexMinus(int index_h, int num);
    int receiveIndexAdd(int index_h, int num);

    enum GameStateEnum getGameState(void);			
    void updateInfo();                    
    uint16_t getGameTime(void);           
    uint16_t getPassengerstartposX(void); 
    uint16_t getPassengerstartposY(void);
    struct Position getPassengerstartpos(void);
    uint16_t getPassengerfinalposX(void); 
    uint16_t getPassengerfinalposY(void);
    struct Position getPassengerfinalpos(void);
    uint16_t getGameFlood(void);              
    uint16_t getFloodposX(int FloodNo);       
    uint16_t getFloodposY(int FloodNo);       
    struct Position getFloodpos(int FloodNo); 
    uint16_t getCarposX();            
    uint16_t getCarposY();                
    struct Position getCarpos();          
    uint16_t getPackageposX(int PackNo);          
    uint16_t getPackageposY(int PackNo);             
    uint16_t getPackagewhetherpicked(int PackNo);    
    struct Position getPackagepos(int PackNo);       
    uint16_t getCarpicknum();                     
    uint16_t getCartransportnum();            
    uint16_t getCartransport();                 
    uint16_t getCarscore();                      
    uint16_t getCartask();          
    uint16_t getCararea();                        
    uint16_t getObstacleAposX(int ObstacleNo);      
    uint16_t getObstacleAposY(int ObstacleNo);       
    uint16_t getObstacleBposX(int ObstacleNo);       
    uint16_t getObstacleBposY(int ObstacleNo);       
    struct Position getObstacleApos(int ObstacleNo); 
    struct Position getObstacleBpos(int ObstacleNo); 
    int positonTransform(Position &pos);
    bool indexNotExist(int index);
    ~Information() {}
    static Information &getInstance();
};

#endif
