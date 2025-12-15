#include "Game_Run.h"
#include <iostream>
#include <vector>
using namespace std;
enum Direction {Up,Down,Right,Left};
Direction direction;
vector<string>  robot_up;
vector<string>  robot_down;
vector<string>  robot_left;
vector<string>  robot_right;
vector<string> *current_direction;

void change_direction(Direction c)
{
    switch(c)
    {
        case Up:current_direction=&robot_up;break;
        case Down:current_direction=&robot_down;break;
        case Left:current_direction=&robot_left;break;
        case Right:current_direction=&robot_right;break;
    }
}

const int HEIGHT=45,WIDTH=80;

//需要考虑怎么显示箱子、空地、机器人
