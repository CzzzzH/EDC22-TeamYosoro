#ifndef UTIL_H
#define UTIL_H

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

enum Mission {WAIT_FOR_START, GO_TO_MAZE, SEARCH_MAZE, RETURN};
enum Action {GO_AHEAD, SMALL_TURN, BIG_TURN};

struct bfsInfo
{
    int nextNode;
    int dist;
};

struct CrossroadAction
{
    int rotateAngle; // 0 90 -90 180
    int nextPosition; //-1~36
};

struct barrierEdge
{
    int A;
    int B;
    friend bool operator < (barrierEdge u,barrierEdge v)
    {
        return u.A > v.A;   //Delibrately sort from big to small
    }
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
    uint8_t GameState; //
    uint16_t Time;     //
    uint8_t stop;      //
};

struct CarInfo
{
    struct Position pos;     //
    uint16_t score;          //
    uint8_t picknum;         //
    uint8_t task;            //
    uint8_t transport;       //
    uint8_t transportnum;    //
    uint8_t area;            //
    uint8_t WhetherRightPos; //
};

struct PassengerInfo
{
    struct Position startpos; //
    struct Position finalpos; //
};

struct PackageInfo
{
    uint8_t No; //
    struct Position pos;
    uint8_t whetherpicked; //
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
    GameNotStart, //
    GameGoing,    //
    GamePause,    //
    GameOver      //
};

#endif