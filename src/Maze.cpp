#include <Arduino.h>
#include "Maze.h"

void Maze::addEdge(int u, int v, bool dir = 1)
{
    adjList[u].push_back(v);
    if(dir)
        adjList[v].push_back(u);
}

void Maze::initialize(Information &info)
{
    //adding the edges in the Maze
    for (int i = 0;i <= MAZE_SIZE;i++)
    {
        for(int j = 1;j <= MAZE_SIZE;j++)
        {
            if(j + 1 <= MAZE_SIZE)
                Maze::addEdge(i * MAZE_SIZE + j, i * MAZE_SIZE + j + 1);
            if(i < MAZE_SIZE - 1)
                Maze::addEdge(i * MAZE_SIZE + j, i * MAZE_SIZE + j + MAZE_SIZE);
        }
    }
}

bfsInfo Maze::getDistance(int now, int target)
{
    int Stack[MAZE_SIZE * MAZE_SIZE];
    memset(Stack, 0, sizeof(Stack));
    bool Break = false;
    std::queue<int> q;
    std::vector<int> history;
    std::map<int, bool> visited;

    q.push(now);
    visited[now] = true;

    while (!q.empty())
    {
        int node = q.front();
        q.pop();
        for (auto neighbours : adjList[node])
        {
            if (!visited[neighbours])
            {
                q.push(neighbours);
                visited[neighbours] = true;
                Stack[neighbours] = node;
                if(neighbours == target)
                {
                    Break = true;
                    break;
                }
            }
        }
        if(Break)
            break;
    }
    //回溯
    int t = Stack[target];
    history.push_back(target);
    while(t != 0)
    {
        history.push_back(t);
        t = Stack[t];
    }
    history.pop_back();
    int index1 = history.back();
    return {index1, int(history.size())};
}

int Maze::getDirection(int last, int now, int target)
{
    int index1 = getDistance(now, target).nextNode;
    int rotate = 0;
    int diff1 = now - last;
    int diff2 = index1 - now;
    if(diff1 == diff2)
        rotate = 0;
    else if(diff1 + diff2 == 0)
        rotate = 2;
    else
    {
        if(diff2 == -1)
            rotate = (diff1 == MAZE_SIZE) ? -1 : 1;
        else if(diff2 == 1)
            rotate = (diff1 == -MAZE_SIZE) ? -1 : 1;
        else if(diff2 == MAZE_SIZE)
            rotate = (diff1 == 1) ? -1 : 1;
        else if(diff2 == -MAZE_SIZE)
            rotate = (diff1 == -1) ? -1 : 1;
    }
    return rotate * 90;
}

std::map <int, std::list<int>>Maze::adjList;