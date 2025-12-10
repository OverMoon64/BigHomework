#include "Order_Set.h"
#include <iostream>
using namespace std;

/// @brief 
extern bool is_space_taken[];
extern bool current_box();

order get_order()
{
    order input;
    input.operand=-1;
    cin >>input.type;
    if(cin.peek()<'0' && cin.peek()>'9');
    else 
    {cin>>input.operand;
    if(cin.peek()!='\n')    input.operand=-1;}
    if(input.operand!=(int)input.operand)   input.operand=-1;
    //line_num???
    return input;
}

bool check_legal(order x)   //还需要大改，检查空地、当前积木、指令的行数
{
    if(x.type=="inbox" || x.type=="outbox")
    {
        if(x.operand==-1)    return true;
        else return false;
    }
    else
    {
        if(x.operand==-1)   return false;
        else return true;
    }
}

bool is_in_level(string c,vector<string> set)
{
    for(int i=0; i<set.size(); i++)
    if(c==set[i])   return true;
    return false;
}
