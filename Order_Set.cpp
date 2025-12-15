#include "Order_Set.h"
#include <iostream>
#include <sstream>
using namespace std;

 
extern bool current_box;
extern vector<bool> is_space_taken;//网上说可能有坑！！！！！！！

order get_order(int line_num)   //注意此函数新增参数line_num
{
    order input;
    input.line_num=line_num;
    string input_line;
    getline(cin,input_line);//输入一整行
    stringstream ss(input_line);
    if(!(ss>>input.type))   //没有输入
    {
        input.type="Undefined";
        input.operand=-1;
        return input;
    }
    string token;
    if(ss>>token)//尝试读取操作数
    {
        string extra;
        if(ss>>extra)   //看有没有多余内容
        {
        input.operand=-1;
        return input;
        }
        bool is_integer=true;//判断操作数是否为整数
        for(int i=0; i<token.length(); i++)
        {
            if(!isdigit(token[i]))    {is_integer=false;break;}
        }
        if(is_integer)
        {
            input.operand=stoi(token);
            if(input.operand<=0)    input.operand=-1;
        }   
    }
    else
    {
        input.operand=0;//0代表没有操作数
    }
    return input;
}

bool check_legal(order x)   
{
    if(x.operand==-1)   return false;//-1代表错误
    if(x.type=="inbox")//指令为inbox，需要无操作数
    {
        if(x.operand==0)   return true;
        else return false;
    }
    if(x.type=="outbox")//指令为outbox，需要无操作数、有当前积木
    {
        if(x.operand==0 && current_box)   return true;
        else return false;
    }
    if(x.type=="add" || x.type=="sub")//指令为add或sub，需要有操作数、有当前积木、x号空地存在且有积木
    {
        if(is_space_taken.size()-1<x.operand)   return false;//空地数不够
        if(x.operand==0)   return false; //没有操作数
        if(!current_box)    return false;//没有当前积木
        if(!is_space_taken[x.operand])  return false;//空地上没有积木
        return true;
    }
    if(x.type=="copyto")//指令为copyto，需要有操作数、有当前积木，x号空地存在
    {
        if(x.operand!=0 && current_box && is_space_taken.size()>x.operand)  return true;
        else return false;
    }
    if(x.type=="copyfrom")//指令为copyyfrom，需要有操作数、x号空地存在且有积木
    {
        if(x.operand!=0 && is_space_taken.size()>x.operand && is_space_taken[x.operand])    return true;
        else return false;
    }
    if(x.type=="jump")//指令为jump，需要有操作数、x号指令存在
    {

    }
    if(x.type=="jumpifzero")//指令为jumpifzero，需要有操作数、有当前积木、x号指令存在
    {

    }
}

bool is_in_level(string c,vector<string> set)
{
    for(int i=0; i<set.size(); i++)
    if(c==set[i])   return true;
    return false;
}
