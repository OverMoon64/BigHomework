#pragma once
#include <string>
#include <vector>
#include <array>
using namespace std;
extern int pf[5];    //记录是否通过

// 定义关卡信息结构体
struct LevelInfo 
{
    vector<string>orderset;//关卡中合法的指令集
    string description;
    vector<int>listentry;//输入序列
    vector<int>target;//目标输出序列
    int spacenumber;
};

// 声明结构体数组和相关函数
extern const std::array<LevelInfo, 4> levelData;// 存储4个关卡的信息
void showlevel();
int chooselevel();
void displayLevelDetails(int levelNumber); // 显示指定关卡的详细信息