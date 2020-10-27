/********************************
information.h
������λ��������
����˵������USART2Ϊ����
	�ڳ���ʼ��ʱ��ʹ��zigbee_Init(&huart2)���г�ʼ��;
	�ڻص�������ʹ��zigbeeMessageRecord(void)��¼���ݣ������¿����ж�

����˵��
    struct BasicInfo Game;�������״̬��ʱ�䡢й�����Ϣ
    struct CarInfo CarInfo;//���泵����Ϣ
    struct PassengerInfo Passenger;//������Ա����Ϣ��λ�ú��ʹ�λ��
    struct PackageInfo Package[6];//�������ʵ���Ϣ
    struct StopInfo Stop[2];//��������λ����Ϣ
    struct ObstacleInfo Obstacle[8];//���������ϰ�����Ϣ
    ͨ���ӿڻ�ȡ����
**********************************/
#ifndef INFORMATION_H
#define INFORMATION_H

#include <Arduino.h>
#include "util.h"

#define INVALID_ARG -1
#define ZIGBEE_MESSAGE_LENTH 70

class Information
{
private:
    Information() {}
    Information &operator=(const Information &) = delete;

public:
    BasicInfo Game;           //�������״̬��ʱ�䡢й�����Ϣ
    CarInfo Car;              //���泵����Ϣ
    PassengerInfo Passenger;  //������Ա����Ϣ��λ�ú��ʹ�λ��
    PackageInfo Package[6];   //�����Ѵ���ʵ���Ϣ
    FloodInfo Flood[5];       //����й���λ����Ϣ
    ObstacleInfo Obstacle[8]; //���������ϰ���Ϣ

    uint8_t zigbeeReceive[ZIGBEE_MESSAGE_LENTH]; //ʵʱ��¼�յ�����Ϣ
    uint8_t zigbeeMessage[ZIGBEE_MESSAGE_LENTH]; //��������˳���õ�����Ϣ
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

    enum GameStateEnum getGameState(void);			//����״̬
    void updateInfo();                    //ʵʱ��¼��Ϣ����ÿ�ν�����ɺ�������ݣ����¿����ж�
    uint16_t getGameTime(void);           //����ʱ�䣬��λΪ0.1s
    uint16_t getPassengerstartposX(void); //��Ա��ʼλ��
    uint16_t getPassengerstartposY(void);
    struct Position getPassengerstartpos(void);
    uint16_t getPassengerfinalposX(void); //��Ա�赽��λ��
    uint16_t getPassengerfinalposY(void);
    struct Position getPassengerfinalpos(void);
    uint16_t getGameFlood(void);              //����㿪����Ϣ
    uint16_t getFloodposX(int FloodNo);       //�����λ��X
    uint16_t getFloodposY(int FloodNo);       //�����λ��Y
    struct Position getFloodpos(int FloodNo); //�����λ��
    uint16_t getCarposX();                    //С��x����
    uint16_t getCarposY();                    //С��y����
    struct Position getCarpos();              //С��λ��
    uint16_t getPackageposX(int PackNo);             //����x����
    uint16_t getPackageposY(int PackNo);             //����y����
    uint16_t getPackagewhetherpicked(int PackNo);    //�����Ƿ��ѱ��ռ�
    struct Position getPackagepos(int PackNo);       //����λ��
    uint16_t getCarpicknum();                        //С���ռ���
    uint16_t getCartransportnum();                   //С��������Ա��
    uint16_t getCartransport();                      //С���Ƿ�����������Ա
    uint16_t getCarscore();                          //С���÷�
    uint16_t getCartask();                           //С������
    uint16_t getCararea();                           //С������
    uint16_t getObstacleAposX(int ObstacleNo);       //�����ϰ�Ax����
    uint16_t getObstacleAposY(int ObstacleNo);       //�����ϰ�Ay����
    uint16_t getObstacleBposX(int ObstacleNo);       //�����ϰ�Bx����
    uint16_t getObstacleBposY(int ObstacleNo);       //�����ϰ�By����
    struct Position getObstacleApos(int ObstacleNo); //�����ϰ�Aλ��
    struct Position getObstacleBpos(int ObstacleNo); //�����ϰ�Bλ��

    ~Information() {}
    static Information &getInstance();
};

#endif
