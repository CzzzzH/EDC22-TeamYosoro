#ifndef UTIL_H
#define UTIL_H

enum MissionType {GO_TO_MAZE, RETURN};
enum ActionType {MOVE, TURN};
enum State {IN_MAZE, OUT_MAZE};

struct Position
{
    float x, y;
};

struct Action
{
    ActionType type;
    int direction, value;
};

#endif