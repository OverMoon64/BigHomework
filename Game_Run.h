#include<iostream>
#include<vector>
#include"Level_Manage.h"
using namespace std;

extern char scene[100][100];
//这是定义，不是声明，编译报错，暂时注释
char **box(int);

char **robot(bool l_or_r,bool f_or_back);



extern vector<int> is_space_taken;//记录空地是否被占据
extern bool current_box;
void levelrun(int clevelnum,LevelInfo currentlevel);//关卡入口

// =============================
// Console UI
// =============================
// 在每条指令执行后渲染一次当前游戏状态（IN/OUT、当前积木、空地、指令列表与指令指针等）。
// 相关实现位于 ConsoleRenderer.*，由 Game_Run.cpp 调用。


/*
+---+
| 3 |
+---+
*/

/*
//an example for robot's whole body
//without box

    +===+
    |@ @|
    | . |
    +===+
      |
    +===+
 ---|   |---
 |  |   |  |
 ^  |   |  ^
    +===+
     | |
     | |
     | |
    -- -- 


//with box
  
    +---+
 --<| 3 |>--
 |  +---+  |
 |         |
 |  +===+  |
 |  |@ @|  |
 |  | . |  |
 |  +===+  |
 |    |    | 
 |  +===+  |
 ---|   |---
    |   |  
    |   |  
    +===+
     | |
     | |
     | |
    -- -- 

*/

char **robot_head();

/*
//front
   +===+
   |@ @|
   | . |
   +===+
    | |
//back 
   +===+
   |   |
   |   |
   +===+
//right & left
   +===+
   | > |
   |   |
   +===+

   +===+
   | < |
   |   |
   +===+

*/

char **robot_body();

/*
//front & back



*/
