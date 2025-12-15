#include"Game_Run.h"
#include<vector>
#include<iostream>
#include"Level_Manage.h"
#include"Order_Set.h"

enum Direction {Up,Down,Right,Left};
Direction direction;
vector<string>  robot_up;
vector<string>  robot_down;
vector<string>  robot_left;
vector<string>  robot_right;
vector<string> *current_direction;

void change_direction(Direction c)
{
    switch(c)
    {
        case Up:current_direction=&robot_up;break;
        case Down:current_direction=&robot_down;break;
        case Left:current_direction=&robot_left;break;
        case Right:current_direction=&robot_right;break;
    }
}

const int HEIGHT=45,WIDTH=80;

//需要考虑怎么显示箱子、空地、机器人

void levelrun(int clevelnum,LevelInfo currentlevel)
{
    int cntentry=sizeof(currentlevel.listentry)/sizeof(int);
    
    //vector<int> inputque=currentlevel.listentry;//输入队列
    vector<int> inputque;
    vector<int> targetque;//输出队列
    vector<int> space;
    int currentbox=-100;//初始化当前积木
    bool current_box=0;//是否存在当前积木，开局标记为无积木
    int currentline=1;//初始化行数
    for(int i=0;i<=currentlevel.spacenumber;i++)//按照关卡要求开辟空地，0号空地作废，编号与X保持一致
    {
        space.push_back(0);//开辟空地
        is_space_taken.push_back(0);//标记为无积木

    }
    for(int i=cntentry-1;i>=0;i--)
        inputque.push_back(currentlevel.listentry[i]);//输入队列,从后往前为原来输入序列顺序
    vector<order> orders;//用于存储各行指令
    orders.push_back({});//占据零号位置
    while(cin.peek())
    {
        orders.push_back(get_order(currentline));//类型，操作数，行数
        currentline++;

    }
    int j=1;//表示执行的行数（可以用currentline,今天懒得打字了——2025.12.15）
    for(;j<=orders.size();)
    {
        if(!is_in_level(orders[j].type,currentlevel.orderset))//判断指令是否被包含
        {
            cout<<"Error on instruction "<<orders[j].line_num<<endl;
            break;
        }
        if(!check_legal(orders[j]))//判断指令是否合法
        {
            cout<<"Error on instruction "<<orders[j].line_num<<endl;
            break;
        }
        
        if(orders[j].type=="inbox")
        {
            inbox(inputque,currentbox);//记得要实现输入队列的更新
            current_box=1;//有积木了
            j++;//执行下一行

        }
        else if(orders[j].type=="outbox")
        {
            outbox(targetque,currentbox);
            current_box=0;
            j++;//执行下一行
        }
        else if(orders[j].type=="add")
        {
            add(space,currentbox,orders[j].operand);
            j++;//执行下一行
        }
        else if(orders[j].type=="sub")
        {
            sub(space,currentbox,orders[j].operand);
            j++;//执行下一行
        }
        else if(orders[j].type=="copyto")
        {
            copyto(space,currentbox,orders[j].operand);
            j++;//执行下一行
        }
        else if(orders[j].type=="copyfrom")
        {
            copyfrom(space,currentbox,orders[j].operand);
            j++;//执行下一行
        }
        else if(orders[j].type=="jump")
        {
            jump(j,orders[j].operand);//跳转行
        }
        else if(orders[j].type=="jumpifzero")
        {
            jumpifzero(j,currentline,orders[j].operand);//跳转行
        }
    }
    /*
    while(cin.peek())
    {
        order input=get_order(currentline);//类型，操作数，行数
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
        
        if(input.type=="inbox")
        {
            inbox(inputque,currentbox);//记得要实现输入队列的更新
            current_box=1;//有积木了
            currentline++;//执行下一行

        }
        else if(input.type=="outbox")
        {
            outbox(targetque,currentbox);
            current_box=0;
            currentline++;//执行下一行
        }
        else if(input.type=="add")
        {
            add(space,currentbox,input.operand);
            currentline++;//执行下一行
        }

        else if(input.type=="sub")
        {
            sub(space,currentbox,input.operand);
            currentline++;//执行下一行
        }

        else if(input.type=="copyto")
        {
            copyto(space,currentbox,input.operand);
            currentline++;//执行下一行
        }

        else if(input.type=="copyfrom")
        {
            copyfrom(space,currentbox,input.operand);
            currentline++;//执行下一行
        }

        else if(input.type=="jump")
        {
            jump(currentline,input.operand);
        }

        else if(input.type=="jumpifzero")
        {
            jumpifzero(currentbox,currentline,input.operand);
        }

    }
    */
}