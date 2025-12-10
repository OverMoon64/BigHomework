#include <iostream>

using namespace std;

char scene[100][100];

char **box(int);

char **robot(bool l_or_r,bool f_or_back);

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
