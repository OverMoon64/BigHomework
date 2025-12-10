#include"Display.h"
#include"Level_Manage.h"
#include"Game_Run.h"
#include<iostream>
#include<fstream>

using namespace std;

int pf[5]={1};


int levelnumber=-1;

int main()
{
    cout<<"Welcome to the game, nice to see you again~~~"<<endl;
    while(1)

    {
        showlevel();
        levelnumber=chooselevel();
        displayLevelDetails(levelnumber);
        LevelInfo current_info=levelData[levelnumber-1];
        levelrun(levelnumber,current_info);
    }
    return 0;
}