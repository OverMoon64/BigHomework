#include "Level_Manage.h"
#include "Game_Run.h"
#include <iostream>

using namespace std;

// Level_Manage.h declares: extern int pf[5];
// pf[0] acts like the "unlock" for level 1 (chooselevel checks pf[level-1]),
// so we keep pf[0] = 1 by default.
int pf[5] = {1, 0, 0, 0, 0};

int main() {
    cout << "Welcome to the game, nice to see you again~~~" << endl;

    while (true) {
        showlevel();
        int levelnumber = chooselevel();
        if (levelnumber == -1) {
            //cout << "Goodbye!" << endl;
            break;
        }
        displayLevelDetails(levelnumber);
        LevelInfo current_info = levelData[levelnumber - 1];
        levelrun(levelnumber, current_info);
    }

    return 0;
}
