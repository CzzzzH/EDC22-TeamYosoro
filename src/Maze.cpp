#include <Arduino.h>
#include "Maze.h"
#include "util.h"

void Maze::addEdge(int u, int v, bool dir = 1)
{
    adjList[u].push_back(v);
    if(dir)
        adjList[v].push_back(u);
}

// void Maze::initialize(Information &info)
// {
//     //adding the edges in the Maze
//     for (int i = 0;i <= MAZE_SIZE;i++)
//     {
//         for(int j = 1;j <= MAZE_SIZE;j++)
//         {
//             if(j + 1 <= MAZE_SIZE)
//                 Maze::addEdge(i * MAZE_SIZE + j, i * MAZE_SIZE + j + 1);
//             if(i < MAZE_SIZE - 1)
//                 Maze::addEdge(i * MAZE_SIZE + j, i * MAZE_SIZE + j + MAZE_SIZE);
//         }
//     }
// }

void Maze::initialize(Information &info)
{
    std::vector<barrierEdge> barrier;

    for(int i = 0;i < OBSTACLE;i++)
    {
        int xoffsetA = (info.getObstacleApos(i).X + 12)/30;
        int xoffsetB = (info.getObstacleBpos(i).X + 12)/30;
        int yoffsetA = (info.getObstacleApos(i).Y - 22)/30 - 1;
        int yoffsetB = (info.getObstacleBpos(i).Y - 22)/30 - 1;
        if(xoffsetA == xoffsetB)
        {
            xoffsetA -= 1;
            yoffsetA += 1;
            yoffsetB += 1;
            int start = min(yoffsetA, yoffsetB);
            int end = max(yoffsetA, yoffsetB);
            for(int i = start;i < end;i++)
                barrier.push_back({xoffsetA + i * MAZE_SIZE, xoffsetA + i * MAZE_SIZE + 1});
        }
        else if(yoffsetA == yoffsetB)
        {
            int start = min(xoffsetA, xoffsetB);
            int end = max(xoffsetA, xoffsetB);
            for(int i = start;i < end;i++)
                barrier.push_back({yoffsetA * MAZE_SIZE + i, (yoffsetA + 1) * MAZE_SIZE + i});
        }
    }
    
    std::sort(barrier.begin(), barrier.end());
    //adding the edges in the Maze
    for (int i = 0;i < MAZE_SIZE;i++)
    {
        for(int j = 1;j <= MAZE_SIZE;j++)
        {
            if(j + 1 <= MAZE_SIZE)
            {
                if(barrier.back().A == i * MAZE_SIZE + j && barrier.back().B - barrier.back().A == 1)
                    barrier.pop_back();
                else
                    Maze::addEdge(i * MAZE_SIZE + j, i * MAZE_SIZE + j + 1);
            }
            if(i < MAZE_SIZE - 1)
            {
                if(barrier.back().A == i * MAZE_SIZE + j && barrier.back().B - barrier.back().A == MAZE_SIZE)
                    barrier.pop_back();
                else
                    Maze::addEdge(i * MAZE_SIZE + j, i * MAZE_SIZE + j + MAZE_SIZE);
            }
        }
    }
}

bfsInfo Maze::getWay(int now, int target)
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

void Maze::getDistance(int now, std::deque<int> &target)
{
    bool Break = false;
    int layer = 0;
    std::queue<int> q;
    std::map<int, bool> visited;
    
    std::deque<int>::iterator it = std::find(target.begin(), target.end(), now);
    if(it != target.end())
    {
        std::swap(target[std::distance(target.begin(), it)], target.back());
        target.pop_back();
    }

    q.push(now);
    visited[now] = true;

    while (!q.empty())
    {
        int node = q.front();
        q.pop();
        layer++;
        for (auto neighbours : adjList[node])
        {
            if (!visited[neighbours])
            {
                q.push(neighbours);
                visited[neighbours] = true;
                std::deque<int>::iterator it = std::find(target.begin(), target.end(), neighbours);
                if(it != target.end())
                {
                    std::swap(target[std::distance(target.begin(), it)], target.front());
                    Break = true;
                }
            }
        }
        if(Break)
            break;
    }
}

CrossroadAction Maze::getDirection(int last, int now, int target)
{
    int index1 = getWay(now, target).nextNode;
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
    return {rotate * 90, index1};
}

std::map <int, std::list<int>>Maze::adjList;