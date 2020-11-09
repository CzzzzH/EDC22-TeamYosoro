#ifndef maze_H
#define maze_H

#include <map>
#include <list>
#include <queue>
#include <stack>
#include <algorithm>
#include "information.h"
#define MAZE_SIZE 6
#define OBSATCLE 8

// image axis
//            Start
//             | |
// -1--2--3--4--5--6-
//  |  |  |  |  |  |  
// -7--8--9-10-11--12-
//  |  |  |  |  |  |  
// -13-14-15-16-17-18-
//  |  |  |  |  |  |  
// -19-20-21-22-23-24
//  |  |  |  |  |  |  
// -25-26-27-28-29-30
//  |  |  |  |  |  |  
// -31-32-33-34-35-36
//    | |
//    End

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

class Maze {
private:
    static std::map <int, std::list<int>> adjList;
    static bfsInfo getWay(int now, int target);
public:
    static void initialize(Information &info);
    static void addEdge(int u, int v, bool dir);
    static int getDistance(int now, int target);
    static CrossroadAction getDirection(int last, int now, int target);
};

#endif //maze_H