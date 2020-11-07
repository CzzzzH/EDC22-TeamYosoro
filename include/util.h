#ifndef UTIL_H
#define UTIL_H

enum Mission {WAIT_FOR_START, GO_TO_MAZE, SEARCH_MAZE, RETURN};
enum Direction {Y_POSITIVE, X_NEGTIVE, Y_NEGTIVE, X_POSITIVE}; 

struct MazePosition
{
    int x, y;
};

struct Position
{
    unsigned int X;
    unsigned int Y;
    double getDist(const Position &t) 
    { 
        return sqrt((X - t.X) * (X - t.X) + (Y - t.Y) * (Y - t.Y));
    }
};

struct BasicInfo
{
    uint8_t GameState; //��Ϸ״̬��00δ��ʼ��01�����У�10��ͣ��11����
    uint16_t Time;     //����ʱ�䣬��0.1sΪ��λ
    uint8_t stop;      //й��ڿ�����Ϣ
};

struct CarInfo
{
    struct Position pos;     //С��λ��
    uint16_t score;          //�÷�
    uint8_t picknum;         //С���ɹ��ռ����ʸ���
    uint8_t task;            //С������0�ϰ볡��1�°볡
    uint8_t transport;       //С�����Ƿ�����
    uint8_t transportnum;    //С�������˵ĸ���
    uint8_t area;            //С�����ڵ�����
    uint8_t WhetherRightPos; //С�����λ����Ϣ�Ƿ�����ȷ�ģ�1����ȷ�ģ�0�ǲ���ȷ��.
};

struct PassengerInfo
{
    struct Position startpos; //��Ա��ʼλ��
    struct Position finalpos; //��ԱҪ�����λ��
};

struct PackageInfo
{
    uint8_t No; //���ʱ��
    struct Position pos;
    uint8_t whetherpicked; //�����Ƿ��ѱ�ʰȡ
};

struct FloodInfo
{
    uint8_t FloodNo;
    struct Position pos;
};

struct ObstacleInfo
{
    uint8_t ObstacleNo;
    struct Position posA;
    struct Position posB;
};

enum GameStateEnum
{
    GameNotStart, //δ��ʼ
    GameGoing,    //������
    GamePause,    //��ͣ��
    GameOver      //�ѽ���
};

#endif