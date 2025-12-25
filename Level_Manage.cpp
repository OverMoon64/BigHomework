#include"Level_Manage.h"
#include<fstream>
#include<iostream>
using namespace std;


/*const std::array<LevelInfo, 4> levelData = {{
    // 关卡1
    {{"inbox","outbox"},"让机器人取出输入序列上的每个积木放入输出序列中",{1,2},{1,2},0},//请让机器⼈取出输⼊序列上的每个积⽊放⼊输出序列中
    // 关卡2
    {{"inbox","outbox","copyfrom","copyto","add","sub","jump","jumpifzero"},"对于输⼊序列中的每两个东西,先把第1个减去第2个,并把结果放在输出序列中,然后把第2个减去第1个,再把结果放在输出序列中,重复。",{3, 9, 5, 1, -2, -2, 9, -9},{-6, 6, 4, -4, 0, 0, 18, -18},3},
    // 关卡3
    {{"inbox","outbox","copyfrom","copyto","add","sub","jump","jumpifzero"},"从输⼊序列中依次取2个数字,如果相等则将其中⼀个输出,否则扔掉。重复这个过程直到输⼊传送带为空。",{6, 2, 7, 7, -9, 3, -3, -3},{7,-3},3},
    // 关卡4 - 最终试炼
    {{},"极乐净土",{},{},0}
}};
*/
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

void showlevel()
{
    ifstream fin("level.txt");
    if(!fin) {cout<<"Error:can't find your level record!";return;}
    for(int i=1;i<=4;i++)//读取关卡通过情况
    {
        fin>>pf[i];
    }
    fin.close();

    cout<<"Please choose your level:"<<endl;
    cout<<endl;
    for(int i=1;i<=4;i++)
    {
        
        cout<<"Level "<<i<<" : ";
        if(pf[i]) cout<<"finished!"<<endl;
        else cout<<"unfinished!"<<endl;
    }
    cout<<endl;
}
int chooselevel()
{
    int levelnumber=-1;
    string snumber;//读取数字，为了排除非整数
    
    while(1)//输入选择关卡
    {

        cout<<"Input the level number you choose and start to play! (or input q to quit):";
        cin>>snumber;
        cin.clear();

        // 允许输入 q / Q 退出选关界面
        if (snumber == "q" || snumber == "Q") {
            cout << "\nQuit selected. Bye!\n" << endl;
            return -1;
        }
        
        bool Isvalidnumber=1;//输入内容有效
        for(int i=0;i<snumber.length();i++)
        {
            
            if(snumber[i]<'0'||snumber[i]>'9')
            {
                cout<<endl<<"Please don't enter invalid content."<<endl;
                cout<<endl;
                Isvalidnumber=0;
                break;
            }

        }
        if(!Isvalidnumber) continue;
        levelnumber=stoi(snumber);
        if(levelnumber<1||levelnumber>4)//关卡不存在
        {
            cout<<endl<<"Level doesn't exit, please check."<<endl;
            cout<<endl;
            continue;
        }
        if(!pf[levelnumber-1])//判断前置关卡完成情况
        {
            cout<<endl<<"Level is locked, finish the previous level first!"<<endl;
            cout<<endl;
            continue;
        }
        else//合法，加载进选择的关卡
        {
            cout<<endl<<"OK, Let's go to level "<<levelnumber<<" !"<<endl;
            
            break;
        }
    }
    return levelnumber;
}

void displayLevelDetails(int levelNumber)//显示关卡详细信息
{
    cout<<"Here is some information you have to know to pass this level:"<<endl<<endl;
    cout<<"Level target:"<<levelData[levelNumber-1].description<<endl;
    cout<<"List entries:";
    for(int i=0;i<levelData[levelNumber-1].listentry.size();i++)
    {
        cout<<levelData[levelNumber-1].listentry[i];
        if(i<levelData[levelNumber-1].listentry.size()-1) cout<<",";
    }
    cout<<endl;
    cout<<"Target list:";
    for(int i=0;i<levelData[levelNumber-1].target.size();i++)
    {
        cout<<levelData[levelNumber-1].target[i];
        if(i<levelData[levelNumber-1].target.size()-1) cout<<",";
    }
    cout<<endl;
    cout<<"Available spaces:"<<levelData[levelNumber-1].spacenumber<<endl;
    cout<<"Available orders:";
    for(int i=0;i<levelData[levelNumber-1].orderset.size();i++)
    {
        cout<<levelData[levelNumber-1].orderset[i];
        if(i<levelData[levelNumber-1].orderset.size()-1) cout<<",";
    }
    cout<<endl;
}
