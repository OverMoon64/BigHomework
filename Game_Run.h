#include <iostream>
#include<vector>
using namespace std;

char scene[100][100];

char **box(int);

char **robot(bool l_or_r,bool f_or_back);


bool current_box();//是否存在当前积木
vector<bool> is_space_taken;//记录空地是否被占据

void levelrun(int clevelnum,LevelInfo currentlevel);//关卡入口


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
