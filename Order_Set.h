#pragma once
#include <string>
#include <vector>
#include"Level_Manage.h"

using namespace std;

//指令集结构体
struct order
{
    string type;
    int operand;
    int line_num;
};
//函数声明
order get_order(int line_num);//获取指令
bool is_in_level(string,vector<string>);//检查指令是否在关卡里
bool check_legal(order,int);//检查是否合法


void inbox(vector<int> &inputque,int &currentbox);
void outbox(vector<int> &targetque,int &currentbox);
void add(vector<int> &space,int &currentbox,int num);
void sub(vector<int> &space,int &currentbox,int num);
void copyto(vector<int> &space,int &currentbox,int num);
void copyfrom(vector<int> &space,int &currentbox,int num);
void jump(int &currentline,int num);
void jumpifzero(int &currentbox,int &currentline,int num);



