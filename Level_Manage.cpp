#include"Level_Manage.h"
#include<fstream>
#include<iostream>
#include<iomanip>
using namespace std;

const std::array<LevelInfo, 4> levelData = {{
    // Level 1
    {{"inbox","outbox"}, "Take each block from input and put into output", {1,2}, {1,2}, 0},
    // Level 2
    {{"inbox","outbox","copyfrom","copyto","add","sub","jump","jumpifzero"},
     "For each pair in input: first minus second, then second minus first", 
     {3, 9, 5, 1, -2, -2, 9, -9}, {-6, 6, 4, -4, 0, 0, 18, -18}, 3},
    // Level 3
    {{"inbox","outbox","copyfrom","copyto","add","sub","jump","jumpifzero"},
     "Take two numbers from input, output if equal, discard if not", 
     {6, 2, 7, 7, -9, 3, -3, -3}, {7,-3}, 3},
    // Level 4
    {{"inbox","outbox","copyfrom","copyto","add","sub","jump","jumpifzero"}, 
    "Segmented Sums: 0 is an delimiter. Output the sum of each segment; empty segment outputs 0.", 
    {5,-2,4,0,3,1,0,0,-3,8,-5,0}, {7,4,0,0}, 2}
}};

void showlevel() {
    ifstream fin("level.txt");
    if(fin) {
        for(int i = 1; i <= 4; i++) {
            fin >> pf[i];
        }
        fin.close();
    }
    
    // 清屏并显示标题
    cout << "\n\n";
    cout << "      ===============================================================" << endl;
    cout << "      |                 LITTLE MING'S SUPER ROBOT                   |" << endl;
    cout << "      ===============================================================" << endl;
    cout << endl;
    
    // 机器人ASCII艺术 - 简化版本，确保兼容性
    vector<string> robotArt = {
        "    ====================================",
        "    +---------------------------+",
        "    | Little Ming's Super Robot |",
        "    +---------------------------+",
        "           /           \\",
        "          /             \\",
        "           +-----------+",
        "           |  O     O  |",
        "           +-----------+",
        "                 |",
        "                 |",
        "           +-----------+",
        "           |KFC        |",
        "           |     vivo50|",
        "           +-----------+",
        "         +---+       +---+",
        "         |   |       |   |",
        "         |   |       |   |",
        "             |       |",
        "             |       |",
        "           +---+   +---+",
        "           |   |   |   |"
    };
    
    // 关卡状态表格 - 使用ASCII字符
    vector<string> statusTable = {
        "+---------------------------------------+",
        "|         LEVEL STATUS BOARD            |",
        "+------------+----------------------- --+",
        "|   Level    |        Status            |",
        "+------------+---------------------- ---+",
        "|    1       | " + string(pf[1] ? "     FINISHED!           " : "     UNFINISHED          ") + "|",
        "+------------+--------------------------+",
        "|    2       | " + string(pf[2] ? "     FINISHED!           " : "     UNFINISHED          ") + "|",
        "+------------+--------------------------+",
        "|    3       | " + string(pf[3] ? "     FINISHED!           " : "     UNFINISHED          ") + "|",
        "+------------+--------------------------+",
        "|    4       | " + string(pf[4] ? "     FINISHED!           " : "     UNFINISHED          ") + "|",
        "+------------+--------------------------+",
        "|  " + string(pf[1] && pf[2] && pf[3] && pf[4] ? "       ALL LEVELS COMPLETED!        " : 
                      "Progress: " + to_string(pf[1]+pf[2]+pf[3]+pf[4]) + "/4") + 
                      string(pf[1] && pf[2] && pf[3] && pf[4] ? "" : "                       ") + " |",
        "+---------------------------------------+"
    };
    
    // 创建主框架
    cout << "  +--------------------------------------------------------------+" << endl;
    cout << "  |                                                              |" << endl;
    
    // 并排显示机器人和关卡状态
    size_t maxLines = max(robotArt.size(), statusTable.size());
    
    for(size_t i = 0; i < maxLines; i++) {
        cout << "  |  ";
        
        // 显示机器人部分
        if(i < robotArt.size()) {
            cout << robotArt[i];
            // 填充空格对齐
            if(robotArt[i].length() < 40) {
                cout << string(40 - robotArt[i].length(), ' ');
            }
        } else {
            cout << string(40, ' ');
        }
        
        // 显示状态表格部分
        if(i < statusTable.size()) {
            cout << "  " << statusTable[i];
        } else {
            cout << "                                        ";
        }
        
        cout << "  |" << endl;
    }
    
    cout << "  |                                                                                    |" << endl;
    cout << "  +------------------------------------------------------------------------------------+" << endl;
    cout << endl;
    
    cout << "================================================================" << endl;
    cout << "                PLEASE CHOOSE YOUR LEVEL" << endl;
    cout << "================================================================\n" << endl;
}

int chooselevel() {
    int levelnumber = -1;
    string snumber;
    
    while(1) {
        cout << "================================================================" << endl;
        cout << " Input the level number you choose (1-4) or 'q' to quit:" << endl;
        cout << "================================================================" << endl;
        cout << "Your choice: ";
        cin >> snumber;
        cin.clear();

        if (snumber == "q" || snumber == "Q") {
            cout << "\n================================================================" << endl;
            cout << "                    Quit selected. Bye!" << endl;
            cout << "================================================================" << endl;
            return -1;
        }
        
        bool Isvalidnumber = true;
        for(size_t i = 0; i < snumber.length(); i++) {
            if(snumber[i] < '0' || snumber[i] > '9') {
                cout << "\n================================================================" << endl;
                cout << "          Invalid input! Please enter a number." << endl;
                cout << "================================================================" << endl;
                Isvalidnumber = false;
                break;
            }
        }
        if(!Isvalidnumber) continue;
        
        levelnumber = stoi(snumber);
        if(levelnumber < 1 || levelnumber > 4) {
            cout << "\n================================================================" << endl;
            cout << "       Level doesn't exist! Please choose 1-4." << endl;
            cout << "================================================================" << endl;
            continue;
        }
        if(!pf[levelnumber-1]) {
            cout << "\n================================================================" << endl;
            cout << "   Level is locked! Finish the previous level first!" << endl;
            cout << "================================================================" << endl;
            continue;
        } else {
            cout << "\n================================================================" << endl;
            cout << "                 OK, Let's go to LEVEL " << levelnumber << " !" << endl;
            cout << "================================================================" << endl;
            break;
        }
    }
    return levelnumber;
}

void displayLevelDetails(int levelNumber) {
    cout << "================================================================" << endl;
    cout << "                 LEVEL " << levelNumber << " INFORMATION" << endl;
    cout << "================================================================" << endl;
    
    cout << "+-----------------------------------------------------------------------------------------------------------------------------------+" << endl;
    cout << "| Level Target: " << setw(45) << left << levelData[levelNumber-1].description << "" << endl;
    
    cout << "| List Entries: ";
    for(size_t i = 0; i < levelData[levelNumber-1].listentry.size(); i++) {
        cout << levelData[levelNumber-1].listentry[i];
        if(i < levelData[levelNumber-1].listentry.size()-1) cout << ",";
    }
    
    cout <<endl;
    
    cout << "| Target List:  ";
    for(size_t i = 0; i < levelData[levelNumber-1].target.size(); i++) {
        cout << levelData[levelNumber-1].target[i];
        if(i < levelData[levelNumber-1].target.size()-1) cout << ",";
    }
    cout <<endl;
    
    cout << "| Available Spaces: " << setw(39) << left << levelData[levelNumber-1].spacenumber << "" << endl;
    
    cout << "| Available Orders: ";
    for(size_t i = 0; i < levelData[levelNumber-1].orderset.size(); i++) {
        cout << levelData[levelNumber-1].orderset[i];
        if(i < levelData[levelNumber-1].orderset.size()-1) cout << ",";
    }
    cout <<endl;
    cout << "+-----------------------------------------------------------------------------------------------------------------------------------+" << endl;
    cout << endl;
}