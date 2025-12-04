#include"Display.h"
#include"Level_Manage.h"
#include<iostream>
#include<fstream>

using namespace std;

int pf[5]={1};


int levelnumber=-1;

int main()
{
    cout<<"Welcome to the game, nice to see you again~~~"<<endl;
    showlevel();
    levelnumber=chooselevel();
    displayLevelDetails(levelnumber);

    cin.get();
    return 0;
}