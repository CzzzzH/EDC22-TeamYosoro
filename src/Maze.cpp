#include "Maze.h"
#include "statemachine.h"
#include <Arduino.h>

// #define MAZE_DEBUG

void Maze::printAdjList()
{
    for (auto key : adjList)
    {
        Serial.print(String(key.first) + "<-->");
        for (auto neighbours : key.second)
            Serial.print(String(neighbours) + ",");
        Serial.println();
    }
    Serial.println("Print adjlist end;");
}

void Maze::initialize()
{
    std::vector<barrierEdge> barrier;

    for(int i = 0;i < OBSTACLE;i++)
    {
        int xoffsetA = (Information::getObstacleApos(i).X + 12)/30;
        int xoffsetB = (Information::getObstacleBpos(i).X + 12)/30;
        int yoffsetA = (Information::getObstacleApos(i).Y - 22)/30 - 1;
        int yoffsetB = (Information::getObstacleBpos(i).Y - 22)/30 - 1;
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
    #ifdef MAZE_DEBUG
    for (auto it : barrier)
    {
        Serial.println(String(it.A) + "<-->" + String(it.B));
    }
    #endif

    std::sort(barrier.begin(), barrier.end());
    barrierMaze = barrier;
    // adding the edges in the Maze
    for (int i = 0;i < MAZE_SIZE;i++)
    {
        for(int j = 1;j <= MAZE_SIZE;j++)
        {
            if(j + 1 <= MAZE_SIZE)
            {
                if(barrier.back().A == i * MAZE_SIZE + j && barrier.back().B - barrier.back().A == 1)
                    barrier.pop_back();
                else
                    addEdge(i * MAZE_SIZE + j, i * MAZE_SIZE + j + 1, 1);
            }
            if(i < MAZE_SIZE - 1)
            {
                if(barrier.back().A == i * MAZE_SIZE + j && barrier.back().B - barrier.back().A == MAZE_SIZE)
                    barrier.pop_back();
                else
                    addEdge(i * MAZE_SIZE + j, i * MAZE_SIZE + j + MAZE_SIZE, 1);
            }
        }
    }

    addEdge(0, 5, 1);
    addEdge(32, 38, 1);
    /*
    if(StateMachine::getInstance().nowHalf == SECOND_HALF)
    {
        uint8_t blockCount = Information::Game.stop;
        Serial.println("Stop Count: " + String(Information::Game.stop));
        for (int i = 0;i < blockCount;i++)
        {
            int blockNum = Information::positonTransform(Information::Flood[blockCount].pos);
            block.push_back(blockNum);
        }
    }
    */
}

int Maze::getWay(int now, std::deque<int> &target)
{
    std::vector<int> Stack(MAZE_SIZE * MAZE_SIZE + 10);
    bool Break = false;
    std::deque<sortNode> q;
    std::map<int, bool> visited;
    std::vector<int> history;

    q.push_front({now, 0});
    visited[now] = true;
    int layer = 0;

    std::deque<int>::iterator it = std::find(target.begin(), target.end(), now);
    if(it != target.end())
    {
        std::swap(target[std::distance(target.begin(), it)], target.back());
        target.pop_back();
    }

    if (target.empty())
        return -1;

    while (!q.empty())
    {
        int node = q.front().node;
        layer = q.front().layer;
        q.pop_front();
        layer++;
        for (auto neighbours : adjList[node])
        {
            if (!visited[neighbours])
            {
                std::vector<int>::iterator blockFind = std::find(block.begin(), block.end(), neighbours);
                if(blockFind != block.end())
                    q.push_back({neighbours, layer});
                else
                    q.push_back({neighbours, 0});
                visited[neighbours] = true;
                Stack[neighbours] = node;
                std::deque<int>::iterator it = std::find(target.begin(), target.end(), neighbours);
                if(it != target.end())
                {
                    std::swap(target[std::distance(target.begin(), it)], target.front());
                    Break = true;
                }
            }
        }

        std::sort(q.begin(), q.end());
        if(Break)
            break;
    
    }

    //回溯
    int t = Stack[target.front()];
    history.push_back(target.front());
    while(!(t == 0 || t == 38))
    {
        history.push_back(t);
        t = Stack[t];
    }

    history.pop_back();
    int index1 = history.back();
    return index1;
}

CrossroadAction Maze::getDirection(int last, int now, std::deque<int> &target)
{  
    if(target.front() == 0 && now == 5)
    {
        if(last == 4)
            return {90, 0};
        else if(last == 6)
            return {-90, 0};
        else
            return {0, 0};
    }
    int index1 = getWay(now, target);
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


void Maze::addEdge(int u, int v, bool dir = 1)
{
    adjList[u].push_back(v);
    if(dir)
        adjList[v].push_back(u);
}

void Maze::addEdgeBlock(std::map <int, std::list<int>> &graph, int u, int v, bool dir = 1)
{
    graph[u].push_back(v);
    if(dir)
        graph[v].push_back(u);
}

void Maze::deleteEdge(std::map <int, std::list<int>> &graph, int u, int v, bool dir = 1)
{
    std::list<int>::iterator it = std::find(graph[u].begin(), graph[u].end(), v);
    if(it != graph[u].end())
        graph[u].erase(it);
    if(dir)
    {
        std::list<int>::iterator it_ = std::find(graph[v].begin(), graph[v].end(), u);
        if(it_ != graph[v].end())
            graph[v].erase(it_);
    }
}

void Maze::deleteNode(std::map <int, std::list<int>> &graph, int node)
{
    if(node + MAZE_SIZE < MAZE_SIZE * MAZE_SIZE + 1)
        deleteEdge(graph, node + MAZE_SIZE, node);
    if(node + 1 <= ((1 + ((node - 1) / MAZE_SIZE)) * MAZE_SIZE))
        deleteEdge(graph, node + 1, node);
    if(node - 1 > ((node - 1) / MAZE_SIZE) * MAZE_SIZE)
        deleteEdge(graph, node - 1, node);
    if(node - MAZE_SIZE > 0)
        deleteEdge(graph, node - MAZE_SIZE, node);
}

bool Maze::existEdge(std::map <int, std::list<int>> &graph, int u, int v)
{
    if(std::find(graph[u].begin(), graph[u].end(), v) == graph[u].end())
        return false;
    else
        return true;
}

int Maze::getDist(std::map <int, std::list<int>> &graph, int now, int target)
{
    if(graph[now].empty() || graph[target].empty())
    {
        return 0;
    }
    bool Break = false;
    std::deque<sortNode> q;
    std::map<int, bool> visited;
    std::vector<int> history;
    int layer = 0;
    q.push_front({now, layer});
    visited[now] = true;
    
    while (!q.empty())
    {
        int node = q.front().node;
        layer = q.front().layer;
        q.pop_front();
        layer++;
        for (auto neighbours : graph[node])
        {
            if (!visited[neighbours])
            {
                q.push_back({neighbours, layer});
                visited[neighbours] = true;
                if(neighbours == target)
                    Break = true;
            }
        }
        if(Break)
            break;
    }
    if(Break == false)
        layer = INF;
    return layer;
}

void Maze::putBlock()
{
    std::deque<sortNode> dq;
    // copy map
    // choose barrier
    for(auto bar : barrierMaze)
    {
        std::vector<int> nodeList;
        if(abs(bar.A - bar.B) == 1) // vertical
        {
            nodeList.push_back(bar.A);
            nodeList.push_back(bar.B);
            if(bar.A - MAZE_SIZE > 0)
            {
                nodeList.push_back(bar.A - MAZE_SIZE);
                nodeList.push_back(bar.B - MAZE_SIZE);
            }
            if(bar.A + MAZE_SIZE < MAZE_SIZE * MAZE_SIZE + 1)
            {
                nodeList.push_back(bar.A + MAZE_SIZE);
                nodeList.push_back(bar.B + MAZE_SIZE);
            }
        }
        else // horizontal
        {
            nodeList.push_back(bar.A);
            nodeList.push_back(bar.B);
            if(bar.A - 1 > ((bar.A - 1) / MAZE_SIZE) * MAZE_SIZE)
            {
                nodeList.push_back(bar.A - 1);
                nodeList.push_back(bar.B - 1);
            }
            if(bar.B + 1 <= ((1 + ((bar.B - 1) / MAZE_SIZE)) * MAZE_SIZE))
            {
                nodeList.push_back(bar.A + 1);
                nodeList.push_back(bar.B + 1);
            }
        }
        std::sort(nodeList.begin(), nodeList.end());
        std::vector<int>::iterator pos = std::unique(nodeList.begin(), nodeList.end());
        nodeList.erase(pos, nodeList.end());

        // bfs
        for(auto it : nodeList)
        {
            bool continu = false;
            for(auto find : dq)
            {
                if(it == find.node)
                {
                    continu = true;
                    break;
                }
            }
            if(continu)
                continue;
            bool Add1 = false, Minus1 = false, Add6 = false, Minus6 = false;
            if(existEdge(adjList, it, it + 1))
                Add1 = true;
            if(existEdge(adjList, it, it - 1))
                Minus1 = true;
            if(existEdge(adjList, it, it + MAZE_SIZE))
                Add6 = true;
            if(existEdge(adjList, it, it - MAZE_SIZE))
                Minus6 = true;
            deleteNode(adjList, it);
            int distTmp = 0;
            if(Add1 && Minus1)
            {
                int dist1 = getDist(adjList, it - 1, it + 1);
                if(dist1 > 4)
                    distTmp += dist1;
                else
                    distTmp += 1;
            }
            if(Add6 && Minus6)
            {
                int dist2 = getDist(adjList, it - MAZE_SIZE, it + MAZE_SIZE);
                if(dist2 > 4)
                    distTmp += dist2;
                else
                    distTmp += 1;
            }
            distTmp += 1;   //bias, exclude 0
            dq.push_back({it, distTmp});
            if(Add1)
                addEdgeBlock(adjList, it, it + 1);
            if(Minus1)
                addEdgeBlock(adjList, it, it - 1);
            if(Add6)
                addEdgeBlock(adjList, it, it + MAZE_SIZE);
            if(Minus6)
                addEdgeBlock(adjList, it, it - MAZE_SIZE);
        }
    }
    int number = 0;
    std::sort(dq.begin(), dq.end());
    while(number < 5)
    {
        int nodeNow = dq.back().node;
        std::vector<int>::iterator it1 = std::find(ourTrick.begin(), ourTrick.end(), nodeNow + 6);
        std::vector<int>::iterator it2 = std::find(ourTrick.begin(), ourTrick.end(), nodeNow - 6);
        std::vector<int>::iterator it3 = std::find(ourTrick.begin(), ourTrick.end(), nodeNow - 1);
        std::vector<int>::iterator it4 = std::find(ourTrick.begin(), ourTrick.end(), nodeNow + 1);
        bool save = it1 == ourTrick.end() && it2 == ourTrick.end() && it3 == ourTrick.end() && it4 == ourTrick.end();
        if(save)
        {
            ourTrick.push_back(nodeNow);
            number++;
        }
        dq.pop_back();
    }

    for(auto it : ourTrick)
    {
        Serial.println(it);
    }
}

std::map<int, std::list<int>> Maze::adjList;
std::map <int, std::list<int>> Maze::blockAdj;

std::vector<barrierEdge> Maze::barrierMaze;
std::vector<int> Maze::block;
std::vector<int> Maze::ourTrick;

std::vector<int> Maze::Stack;
std::deque<sortNode> Maze::q;
std::map<int, bool> Maze::visited;
std::vector<int> Maze::history;