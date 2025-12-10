#pragma once
#include <string>
#include <vector>
using namespace std;

//指令集结构体
struct order
{
    string type;
    float operand;
    int line_num;
};
//函数声明
order get_order();//获取指令
bool is_in_level(string,vector<string>);//检查指令是否在关卡里
bool check_legal(order);//检查是否合法

void inbox();
void outbox();
void add(int);
void sub(int);
void copyto(int);
void copyfrom(int);
void jump(int);
void jumpifzero(int);



