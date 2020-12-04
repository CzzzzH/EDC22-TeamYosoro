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
private:
    static std::map <int, std::list<int>> adjList;
    static std::map <int, std::list<int>> blockAdj;
    static std::vector<int> block;
    static std::vector<barrierEdge> barrierMaze;
       
    // Get Way & Get Dist
    static std::vector<int> Stack;
    static std::deque<sortNode> q;
    static std::map<int, bool> visited;
    static std::vector<int> history;
    
public:
    static std::vector<int> ourTrick;
    static void initialize();
    static int getWay(int now, std::deque<int> &target);
    static void addEdge(int u, int v, bool dir);
    static void addEdgeBlock(std::map <int, std::list<int>> &graph, int u, int v, bool dir);
    static void deleteEdge(std::map <int, std::list<int>> &graph, int u, int v, bool dir);
    static void deleteNode(std::map <int, std::list<int>> &graph, int node);
    static bool existEdge(std::map <int, std::list<int>> &graph, int u, int v);
    static int getDist(std::map <int, std::list<int>> &graph, int u, int v);
    static void printAdjList();
    static void putBlock();
    static CrossroadAction getDirection(int last, int now, std::deque<int> &target);
};

#endif //maze_H