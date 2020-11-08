#include <Arduino.h>
#include "Maze.h"

void Maze::addEdge(int u, int v, bool dir = 1)
{
    adjList[u].push_back(v);
    if(dir)
        adjList[v].push_back(u);
}

void Maze::genRoute()
{
    for(int i = 0;i < history.size() - 1;i++)
    {
        history[i] = history[i + 1] - history[i];
    }
    history.pop_back();
    for(int i = 0;i < history.size() - 1;i++)
    {
        int rotate = 0;
        if(history[i + 1] - history[i] == 0)
            rotate = 0;
        else
        {
            if(history[i + 1] == -1)
                rotate = (history[i] == -MAZE_SIZE) ? 1 : -1;
            else if(history[i + 1] == 1)
                rotate = (history[i] == MAZE_SIZE) ? 1 : -1;
            else if(history[i + 1] == MAZE_SIZE)
                rotate = (history[i] == 1) ? 1 : -1;
            else if(history[i + 1] == -MAZE_SIZE)
                rotate = (history[i] == -1) ? 1 : -1;
        }
        Rotate.push_back(rotate);
    }
}

void Maze::bfs(int src, int dest)
{
    bool Break = false;
    std::queue<int> q;

    std::map<int, bool> visited;

    q.push(src);
    visited[src] = true;

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
                if(neighbours == dest)
                {
                    Break = true;
                    break;
                }
            }
        }
        if(Break)
            break;
    }
    //trace back
    int t = Stack[dest];
    history.push_back(dest);
    while(Stack[t] != -1)
    {
        history.push_back(t);
        int val = Stack[t];
        Stack[t] = -1;
        t = val;
    }
    std::reverse(history.begin(),history.end());
}

void Maze::initialize()
{
    memset(Stack, 0, sizeof(Stack));;
}

std::map <int, std::list<int>>Maze::adjList;
int Maze::Stack[MAZE_SIZE * MAZE_SIZE];
std::vector<int> Maze::history;
std::vector<int> Maze::Rotate;