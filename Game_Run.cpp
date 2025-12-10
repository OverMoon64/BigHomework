#include"Game_Run.h"
#include<vector>
#include<iostream>
#include"Level_Manage.h"
#include"Order_Set.h"

void levelrun(int clevelnum,LevelInfo currentlevel)
{
    while(cin.peek())
    {
        order input=get_order();//类型，操作数，行数
        if(!is_in_level(input.type,currentlevel.orderset))//判断指令是否被包含
        {
            cout<<"Error on instruction "<<input.line_num<<endl;
            break;
        }
        if(!check_legal(input))//判断指令是否合法
        {
            cout<<"Error on instruction "<<input.line_num<<endl;
            break;
        }
        vector<int> inputque=currentlevel.listentry;//输入队列
        vector<int> targetque;//输出队列
        vector<int> space;
        int currentbox=0;//初始化当前积木
        bool current_box=0;//是否存在当前积木，开局标记为无积木
        
        for(int i=0;i<=currentlevel.spacenumber;i++)//按照关卡要求开辟空地，0号空地作废，编号与X保持一致
        {
            space.push_back(0);//开辟空地
            is_space_taken.push_back(0);//标记为无积木

        }
        if(input.type=="inbox")
        {
            currentbox=inbox();//记得要实现输入队列的更新
            current_box=1//有积木了

        }
        else if(input.type=="outbox")
        {
            outbox();
            currentbox=0;
            current_box=0;
        }



    }
}