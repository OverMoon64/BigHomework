#include<bits/stdc++.h>

using namespace std;
bool current_box=0;//是否存在当前积木，开局标记为无积木
vector<int>is_space_taken;
struct LevelInfo 
{

    vector<string>orderset;//关卡中合法的指令集
    string description;
    vector<int>listentry;//输入序列
    vector<int>target;//目标输出序列
    int spacenumber;

};

//指令集结构体
struct order
{
    string type;
    int operand;
    int line_num;
};
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
    {{}, "Final Challenge", {}, {}, 0}
}};
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
            if(input.operand<0)    input.operand=-1;
        }   
    }
    else
    {
        input.operand=-2;//-2代表没有操作数
    }
    return input;
}
bool check_legal(order x,int line_num)   
{
    if(x.operand==-1)   return false;//-1代表错误
    if(x.type=="inbox")//指令为inbox，需要无操作数
    {
        if(x.operand==-2)   return true;
        else return false;
    }
    if(x.type=="outbox")//指令为outbox，需要无操作数、有当前积木
    {
        if(x.operand==-2 && current_box)   return true;
        else return false;
    }
    if(x.type=="add" || x.type=="sub")//指令为add或sub，需要有操作数、有当前积木、x号空地存在且有积木
    {
        if(x.operand<0 || is_space_taken.size()<=x.operand)  return false;//空地数不够
        if(x.operand==-2)   return false; //没有操作数
        if(!current_box)    return false;//没有当前积木
        if(!is_space_taken[x.operand])  return false;//空地上没有积木
        return true;
    }
    if(x.type=="copyto")//指令为copyto，需要有操作数、有当前积木，x号空地存在
    {
         
        if(x.operand!=-2 && current_box && is_space_taken.size()>x.operand && x.operand>=0)  return true;
        else return false;
    }
    if(x.type=="copyfrom")//指令为copyyfrom，需要有操作数、x号空地存在且有积木
    {
        if(x.operand!=-2 && is_space_taken.size()>x.operand && is_space_taken[x.operand])    return true;
        else return false;
    }
    if(x.type=="jump")//指令为jump，需要有操作数、x号指令存在
    {
        if(x.operand!=-2 && x.operand>=1 && x.operand<=line_num) return true;
        else return false;
    }
    if(x.type=="jumpifzero")//指令为jumpifzero，需要有操作数、有当前积木、x号指令存在
    {
        if(x.operand!=-2 && current_box && x.operand>=1 && x.operand<=line_num)return true;
        else return false;
    }
    return true;
}

bool is_in_level(string c,vector<string> set)
{
    for(int i=0; i<set.size(); i++)
    if(c==set[i])   return true;
    
    return false;
}

void inbox(vector<int> &inputque,int &currentbox)
{
    currentbox=inputque[inputque.size()-1];//取走最后一位
    inputque.pop_back();//删除拿走的积木
}

void outbox(vector<int> &targetque,int &currentbox)
{
    targetque.push_back(currentbox);
    currentbox=-100;
    current_box=0;
}

void add(vector<int> &space,int &currentbox,int num)
{
    currentbox+=space[num];
}

void sub(vector<int> &space,int &currentbox,int num)
{
    currentbox-=space[num];
}

void copyto(vector<int> &space,int &currentbox,int num)
{
    space[num]=currentbox;
    is_space_taken[num]=1;
}

void copyfrom(vector<int> &space,int &currentbox,int num)
{
    currentbox=space[num];
    current_box=1;
}

void jump(int &currentline,int num)
{
    currentline=num;
}

void jumpifzero(int &currentbox,int &currentline,int num)
{
    if(currentbox==0) currentline=num;
    else currentline++;
}

void levelrun(int clevelnum,LevelInfo currentlevel)
{
    is_space_taken.clear();
    current_box=0;
    int cntentry = currentlevel.listentry.size();
    
    //vector<int> inputque=currentlevel.listentry;//输入队列
    vector<int> inputque;
    vector<int> targetque;//输出队列
    vector<int> space;
    int currentbox=-100;//初始化当前积木
    
    int currentline=0;//初始化行数
    for(int i=0;i<currentlevel.spacenumber;i++)//按照关卡要求开辟空地，0号空地作废，编号与X保持一致
    {
        space.push_back(0);//开辟空地
        is_space_taken.push_back(0);//标记为无积木

    }
    for(int i=cntentry-1;i>=0;i--)
        inputque.push_back(currentlevel.listentry[i]);//输入队列,从后往前为原来输入序列顺序
    vector<order> orders;//用于存储各行指令
    orders.push_back({});//占据零号位置
    /*
    while(cin.peek())
    {
        currentline++;
        orders.push_back(get_order(currentline));//类型，操作数，行数
        

    }
    */
    int M;
    cin >> M;
    cin.ignore();  // 清除换行符
    
    
    for (int i = 1; i <= M; i++) 
    {
        currentline++;
        orders.push_back(get_order(currentline));
    }
    
    int j=1;//表示执行的行数（可以用currentline,今天懒得打字了——2025.12.15）
    for(;j<orders.size();)
    {
        if(!is_in_level(orders[j].type,currentlevel.orderset))//判断指令是否被包含
        {
            cout<<"Error on instruction "<<orders[j].line_num<<endl;
            return;
        }
        if(!check_legal(orders[j],currentline))//判断指令是否合法
        {
            cout<<"Error on instruction "<<orders[j].line_num<<endl;
            return;
        }
        
        if(orders[j].type=="inbox")
        {
            if(inputque.size()==0)break;
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
            jumpifzero(currentbox,j,orders[j].operand);//跳转行
        }
    }
    bool ispass=1;
    if(targetque.size()!=currentlevel.target.size()) ispass=0;
    else
    {
        for(int i=0;i<targetque.size();i++)
        {
            if(targetque[i]!=currentlevel.target[i])
            {
                ispass=0;
                break;
            }
        }
    }
    if(ispass)
    {
        cout<<"Success"<<endl;
        
    }
    else
    {
        cout<<"Fail"<<endl;
    }

}
int chooselevel()//选关卡函数
{
    int levelnumber=-1;
    cin>>levelnumber;
    return levelnumber;
}
int main()
{
    int levelnumber = chooselevel();
    LevelInfo current_info = levelData[levelnumber - 1];
    levelrun(levelnumber, current_info);
    return 0;
}