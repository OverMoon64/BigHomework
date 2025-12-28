#include"Game_Run.h"
#include<vector>
#include<iostream>
#include"Level_Manage.h"
#include"Order_Set.h"
#include<fstream>

#include <chrono>
#include <thread>

#include "ConsoleRenderer.h"

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
bool current_box=0;//是否存在当前积木，开局标记为无积木
char scene[100][100];
vector<int>is_space_taken;
//需要考虑怎么显示箱子、空地、机器人

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

    // 额外保存每行的“原始显示文本”，用于渲染 CODE 列表
    vector<string> program_lines;
    /*
    while(cin.peek())
    {
        currentline++;
        orders.push_back(get_order(currentline));//类型，操作数，行数
        

    }
    */
    string M;
    int m;
    while(1)
    {
    cout << "Enter the order num: ";
    
    cin >> M;
    bool iii=1;
    for(int i=0;i<M.length();i++)
    {
        if(M[i]<'0'||M[i]>'9')
        {
            cout<<"Please enter right number."<<endl<<endl;
            iii=0;
            break;
        }
    }
    if(!iii)continue;
    m=stoi(M);
    if(m>0)
    {
        cin.ignore();  // 清除换行符
        break;
    }
    else
    {
        cout<<"Please enter right number."<<endl<<endl;
    }

    }
    
    for (int i = 1; i <= m; i++) 
    {
        currentline++;
        cout << "Please enter order " << i << endl;
        cout << "> ";
        order od = get_order(currentline);
        orders.push_back(od);
        // 重建一条可显示的指令文本（inbox / add X / ...）
        if (od.operand == -2) program_lines.push_back(od.type);
        else program_lines.push_back(od.type + " " + to_string(od.operand));
    }

    // ===== UI 渲染器 =====
    RenderOptions ropt;
    ropt.max_in_preview = 6;
    ropt.max_out_preview = 6;
    ropt.clear_screen = true;
    ropt.pause_each_step = false; // 如需“按回车单步”，改成 true
    ropt.walk_delay_ms = 1000;      // 走路动画每一小步的延迟（毫秒）
    ropt.frame_delay_ms = 1000;    // 关键帧停留（毫秒），让清屏刷新也能看清
    ropt.box_inner_width = 3;
    ConsoleRenderer renderer(ropt);

    // ===== 机器人“画面移动”状态 =====
    RobotPosKind robot_pos = RobotPosKind::Idle;
    int robot_tile = -1;

    auto pos_to_track_index = [&](RobotPosKind k, int tile_idx) -> int {
        // 把位置映射到一条“轨道”上的离散点：
        // IN=-1, Tile=i, OUT=tileN（tileN=空地数量）
        int tileN = (int)space.size();
        if (k == RobotPosKind::In) return -1;
        if (k == RobotPosKind::Out) return tileN;
        if (k == RobotPosKind::Tile) return std::max(0, tile_idx);
        // Idle：不参与走路插帧（返回一个特殊值）
        return 1000000;
    };

    auto track_index_to_pos = [&](int idx) {
        int tileN = (int)space.size();
        if (idx <= -1) return std::make_pair(RobotPosKind::In, -1);
        if (idx >= tileN) return std::make_pair(RobotPosKind::Out, -1);
        return std::make_pair(RobotPosKind::Tile, idx);
    };

    auto build_snapshot = [&](int ip, int executed, const string& event) {
        RenderSnapshot snap;
        snap.level_desc = currentlevel.description;
        snap.executed = executed;
        snap.last_event = event;
        snap.inbox_queue = inputque;
        snap.outbox_queue = targetque;
        snap.has_holding = current_box;
        snap.holding_value = currentbox;

        // 空地：直接传 value + taken 标记
        snap.tile_value = space;
        snap.tile_taken = is_space_taken;
        snap.program_lines = program_lines;
        snap.ip = ip;
        snap.robot_pos = robot_pos;
        snap.robot_tile_index = robot_tile;
        return snap;
    };

    auto render_walk_to = [&](RobotPosKind target_pos, int target_tile, int ip, int executed, const std::string& instr_txt) {
        // 走路动画：逐格移动（IN -> Tile0 -> Tile1 -> ... -> OUT）
        // 说明：只有 robot_pos/target_pos 都不是 Idle 时才插帧；否则直接渲染一帧。

        int s = pos_to_track_index(robot_pos, robot_tile);
        int t = pos_to_track_index(target_pos, target_tile);

        // 任一端是 Idle：不做插帧
        if (s == 1000000 || t == 1000000) {
            robot_pos = target_pos;
            robot_tile = target_tile;
            renderer.Render(build_snapshot(ip, executed, "Move: " + instr_txt));
            if (!ropt.pause_each_step && ropt.frame_delay_ms > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ropt.frame_delay_ms));
            }
            return;
        }

        if (s == t) {
            robot_pos = target_pos;
            robot_tile = target_tile;
            renderer.Render(build_snapshot(ip, executed, "Move: " + instr_txt));
            if (!ropt.pause_each_step && ropt.frame_delay_ms > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ropt.frame_delay_ms));
            }
            return;
        }

        int step = (t > s) ? 1 : -1;
        // 从下一格开始走到目标（包含目标）
        for (int idx = s + step; ; idx += step) {
            auto [k, tile] = track_index_to_pos(idx);
            robot_pos = k;
            robot_tile = tile;

            // 走路中间帧/到达帧的提示信息
            std::string msg = (idx == t) ? ("Move: " + instr_txt)
                                         : ("Walking: " + instr_txt);
            renderer.Render(build_snapshot(ip, executed, msg));

            if (idx == t) {
                if (!ropt.pause_each_step && ropt.frame_delay_ms > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(ropt.frame_delay_ms));
                }
                break;
            }

            // 如果不是“按回车单步”，加一点延迟让动画可见
            if (!ropt.pause_each_step && ropt.walk_delay_ms > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ropt.walk_delay_ms));
            }
        }
    };
    
    int j=1;//表示执行的行数（可以用currentline,今天懒得打字了——2025.12.15）
    int executed = 0;

    // 初始画面（尚未执行任何指令）
    renderer.Render(build_snapshot(j, executed, ""));

    for(;j<orders.size();)
    {
        int ip_before = j;
        string instr_txt;
        if (ip_before >= 1 && ip_before <= (int)program_lines.size())
            instr_txt = program_lines[ip_before - 1];
        else
            instr_txt = orders[j].type;

        if(!is_in_level(orders[j].type,currentlevel.orderset))//判断指令是否被包含
        {
            string msg = "Error on instruction " + to_string(orders[j].line_num);

            renderer.Render(build_snapshot(j, executed, msg));
            cout<<endl;
            cout<<"==================!!!!!!WARNING!!!!!!=================="<<endl;
            cout <<"==========!!!!!!"<< msg <<"!!!!!!=========="<< endl;
            cout<<endl;
            break;
        }
        if(!check_legal(orders[j],currentline))//判断指令是否合法
        {
            string msg = "Error on instruction " + to_string(orders[j].line_num);
            
            renderer.Render(build_snapshot(j, executed, msg));
            cout<<endl;
            cout<<"==================!!!!!!WARNING!!!!!!=================="<<endl;
            cout <<"==========!!!!!!"<< msg <<"!!!!!!=========="<< endl;
            cout<<endl;
            break;
        }

	    // =========================================================
	    // 先渲染“机器人移动到目标位置”，再执行指令，再渲染结果
	    // =========================================================
	    // 1) 根据指令类型确定目标位置
	    RobotPosKind target_pos = RobotPosKind::Idle;
	    int target_tile = -1;
	    if (orders[j].type == "inbox") {
	        target_pos = RobotPosKind::In;  target_tile = -1;
	    } else if (orders[j].type == "outbox") {
	        target_pos = RobotPosKind::Out; target_tile = -1;
	    } else if (orders[j].type == "add" || orders[j].type == "sub" ||
	               orders[j].type == "copyto" || orders[j].type == "copyfrom") {
	        target_pos = RobotPosKind::Tile; target_tile = orders[j].operand;
	    } else {
	        target_pos = RobotPosKind::Idle; target_tile = -1;
	    }

	    // 2) 走路动画：逐格渲染直到到达目标位置（此时游戏状态未改变，ip 仍指向当前行）
	    render_walk_to(target_pos, target_tile, j, executed, instr_txt);

	    // 3) 执行指令
	    if(orders[j].type=="inbox")
	    {
	        if(inputque.size()==0) {
	            string msg = "Game Over: inbox on empty IN";
	            renderer.Render(build_snapshot(j, executed, msg));
	            break;
	        }
	        inbox(inputque,currentbox);//记得要实现输入队列的更新
	        current_box=1;//有积木了
	        j++;//执行下一行

	        executed++;
	        renderer.Render(build_snapshot(j, executed, "Executed: " + instr_txt));

	    }
	    else if(orders[j].type=="outbox")
	    {
	        outbox(targetque,currentbox);
	        current_box=0;
	        j++;//执行下一行

	        executed++;
	        renderer.Render(build_snapshot(j, executed, "Executed: " + instr_txt));
	    }
	    else if(orders[j].type=="add")
	    {
	        add(space,currentbox,orders[j].operand);
	        j++;//执行下一行

	        executed++;
	        renderer.Render(build_snapshot(j, executed, "Executed: " + instr_txt));
	    }
	    else if(orders[j].type=="sub")
	    {
	        sub(space,currentbox,orders[j].operand);
	        j++;//执行下一行

	        executed++;
	        renderer.Render(build_snapshot(j, executed, "Executed: " + instr_txt));
	    }
	    else if(orders[j].type=="copyto")
	    {
	        copyto(space,currentbox,orders[j].operand);
	        j++;//执行下一行

	        executed++;
	        renderer.Render(build_snapshot(j, executed, "Executed: " + instr_txt));
	    }
	    else if(orders[j].type=="copyfrom")
	    {
	        copyfrom(space,currentbox,orders[j].operand);
	        j++;//执行下一行

	        executed++;
	        renderer.Render(build_snapshot(j, executed, "Executed: " + instr_txt));
	    }
	    else if(orders[j].type=="jump")
	    {
	        jump(j,orders[j].operand);//跳转行

	        executed++;
	        renderer.Render(build_snapshot(j, executed, "Executed: " + instr_txt));
	    }
	    else if(orders[j].type=="jumpifzero")
	    {
	        jumpifzero(currentbox,j,orders[j].operand);//跳转行

	        executed++;
	        renderer.Render(build_snapshot(j, executed, "Executed: " + instr_txt));
	    }
    }

    // 若正常执行完所有指令（未触发 break），渲染一帧“程序结束”
    if (j >= (int)orders.size()) {
        renderer.Render(build_snapshot(j, executed, "Program finished."));
        if (!ropt.pause_each_step && ropt.frame_delay_ms > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(ropt.frame_delay_ms));
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
        printf(
            "+=============================================+\n"
            "|    Mission accomplish. Congratulation!!!    |\n"
            "+=============================================+\n"
        );
        
        ifstream fin("level.txt");
        if(!fin)return;
        int p[5];
        for(int i=1;i<=4;i++)
        {
            fin>>p[i];
        }
        fin.close();
        ofstream fout("level.txt");
        for(int i=1;i<=4;i++)
        {
            if(i!=clevelnum)fout<<p[i]<<endl;
            else fout<<1<<endl;
        }
        fout.close();
    }
    else
    {
        printf(
            "+=============================================+\n"
            "|        Mission failed, what a pity!         |\n"
            "+=============================================+\n"
        );
        bool isis=1;
        while(isis)
        {
        cout<<">>>>>>Do you wanna try again?<<<<<<"<<endl;
        string str;
        cout<<"Input Y/N to make your choice>>>";
        cin>>str;
        if(str!="Y"&&str!="N")
        {
            cout<<endl;
            cout<<"Stop joking, bro. Don't lose heart."<<endl<<endl;
        }
        else if(str=="Y")
        {
            cout<<endl;
            cout<<"Nice, bro! You can make it!"<<endl<<endl;
            isis=0;
            levelrun(clevelnum,currentlevel);
        }
        else if(str=="N")
        {
            cout<<endl;
            cout<<"Are you sure? Hurry up, bro.(Y/N)>>>";
            string secondchoice;
            cin>>secondchoice;
            if(secondchoice=="Y")
            {
                cout<<endl;
                cout<<"OK. Have a break and you will make it!"<<endl;
                isis=0;
            }
            else if(secondchoice=="N")
            {
                cout<<endl;
                cout<<"Nice, bro! Take it easy and you can make it!"<<endl<<endl;
                isis=0;
                levelrun(clevelnum,currentlevel);
            }
            else
            {
                cout<<endl;
                cout<<"lol,I have guessed that you will take this step! I know you're tired, let's take a break!"<<endl;
                isis=0;
            }
        }
        }
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
