#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <math.h>

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

enum Match {FIRST_HALF, SECOND_HALF};
enum Mission {WAIT_FOR_START, GO_TO_MAZE, SEARCH_MAZE, GO_OUT_MAZE, RETURN, END_GAME};
struct sortNode
{
    int8_t node;
    int8_t layer;
    friend bool operator < (sortNode u,sortNode v)
    {
        if(u.layer < v.layer)
            return true;
        else
            return false;
    }
};
struct CrossroadAction
{
    int8_t rotateAngle; // 0 90 -90 120
    int8_t nextPosition; //-1~36
};

struct barrierEdge
{
    int16_t A;
    int16_t B;
    friend bool operator < (barrierEdge u,barrierEdge v)
    {
        if(u.A > v.A)
            return true;
        else if(u.A == v.A)
            return u.B > v.B;
        else
            return false;
    }
};

struct Position
{
    uint8_t X;
    uint8_t Y;
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