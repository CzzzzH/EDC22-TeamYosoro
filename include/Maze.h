#ifndef maze_H
#define maze_H

#include <map>
#include <list>
#include <queue>
#include <stack>
#include <algorithm>
#include "information.h"

// image axis
//             END
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
//   START

class Maze {
private:
    static std::map <int, std::list<int>> adjList;
    static std::vector<int> block;
    static int getWay(int now, std::deque<int> &target);
public:
    static void initialize(Information &info);
    static void addEdge(int u, int v, bool dir);
    static CrossroadAction getDirection(int last, int now, std::deque<int> &target);
};

#endif //maze_H