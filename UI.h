#pragma once

#include <string>
#include <vector>

// 用于把当前游戏状态渲染到命令行（ASCII UI）。

struct RenderOptions {
    int max_in_preview = 6;      // IN 传送带最多显示多少个（从“下一块”开始）
    int max_out_preview = 6;     // OUT 传送带最多显示多少个（从“最新输出”开始）
    bool clear_screen = true;    // 是否每帧清屏（ANSI）
    bool pause_each_step = false;// 是否每步等待回车
    int box_inner_width = 3;     // 盒子内部宽度
};

struct RenderSnapshot {
    std::string level_desc;
    int executed = 0;            // 已执行指令数
    std::string last_event;      // 上一步提示/错误/成功信息

    // IN：下一块积木应显示在最上方。
    // 你可以把 inputque 直接传入（若你的 inputque 用“末尾 pop”表示队首，Render 会自动处理）。
    std::vector<int> inbox_queue;

    // OUT：按输出顺序 push_back；Render 会把“最新输出”显示在最上方。
    std::vector<int> outbox_queue;

    // 当前积木
    bool has_holding = false;
    int holding_value = 0;

    // 空地：tile_value[i] 存储数值；tile_taken[i] 为 1 表示占用，否则为空
    std::vector<int> tile_value;
    std::vector<int> tile_taken;

    // 程序：每行指令文本
    std::vector<std::string> program_lines;
    int ip = 1;                  // 下一条将执行的指令编号（从 1 开始）
};

class ConsoleRenderer {
public:
    explicit ConsoleRenderer(RenderOptions opt = {});
    void Render(const RenderSnapshot& s);

private:
    RenderOptions opt_;

    static void ClearScreenAnsi();
    static std::string Fit(const std::string& s, int w);
    static std::vector<std::string> Box3(const std::string& text, int inner_w);
    static std::vector<std::string> ColumnConveyor(
        const std::string& label,
        const std::vector<std::string>& items_top_to_bottom,
        int inner_w
    );
};
