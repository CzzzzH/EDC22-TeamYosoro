#ifndef maze_H
#define maze_H

#include <map>
#include <list>
#include <queue>
#include <stack>
#include <algorithm>
#include "information.h"

#define INF 100

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
public:
    static std::map <int8_t, std::list<int8_t>> adjList;
    static std::map <int8_t, std::list<int8_t>> blockAdj;
    static std::vector<int8_t> block;
    static std::vector<int8_t> ourTrick;
    static std::vector<barrierEdge> barrierMaze;

    static void initialize();
    static void printAdjList();
    static void addEdge(int8_t u, int8_t v, bool dir);
    static void addEdgeBlock(std::map <int8_t, std::list<int8_t>> &graph, int8_t u, int8_t v, bool dir);
    static void deleteEdge(std::map <int8_t, std::list<int8_t>> &graph, int8_t u, int8_t v, bool dir);
    static void deleteNode(std::map <int8_t, std::list<int8_t>> &graph, int8_t node);
    static bool existEdge(std::map <int8_t, std::list<int8_t>> &graph, int8_t u, int8_t v);
    static void putBlock();
    static int8_t getDist(std::map <int8_t, std::list<int8_t>> &graph, int8_t u, int8_t v);
    static int8_t getWay(int8_t now, std::deque<int8_t> &target);
    //find connectivity between now and target
    static CrossroadAction getDirection(int8_t last, int8_t now, std::deque<int8_t> &target);
};

#endif //maze_H