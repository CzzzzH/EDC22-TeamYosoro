#ifndef maze_H
#define maze_H

#include <map>
#include <list>
#include <queue>
#include <stack>
#include <algorithm>
#define MAZE_SIZE 6

// image axis
//            Start
//             | |
// -0--1--2--3--4--5-
//  |  |  |  |  |  |
// -6--7--8--9-10-11-
//  |  |  |  |  |  |
// -12-13-14-15-16-17-
//  |  |  |  |  |  |
// -18-19-20-21-22-23-
//  |  |  |  |  |  |
// -24-25-26-27-28-29-
//  |  |  |  |  |  |
// -30-31-32-33-34-35-
//    | |
//    End
class Maze {
private:
    static std::map <int, std::list<int>> adjList;
    static int Stack[MAZE_SIZE * MAZE_SIZE];
    static std::vector<int> history;
    static std::vector<int> Rotate;

public:
    static void initialize();
    static void addEdge(int, int, bool);
    static void genRoute();
    static void bfs(int, int);
};

#endif //maze_H