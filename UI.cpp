#include "UI.h"

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>

ConsoleRenderer::ConsoleRenderer(RenderOptions opt) : opt_(opt) {}

void ConsoleRenderer::ClearScreen() {
#ifdef _WIN32
    // Windows 终端对 ANSI 支持不一；system("cls") 更稳（作业环境一般可用）
    std::system("cls");
#else
    // ANSI: 清屏 + 光标回到左上角
    std::cout << "\x1b[2J\x1b[H";
#endif
}

std::string ConsoleRenderer::Fit(const std::string& s, int w) {
    if ((int)s.size() == w) return s;
    if ((int)s.size() > w) return s.substr(0, w);
    return s + std::string(w - (int)s.size(), ' ');
}

std::vector<std::string> ConsoleRenderer::Box3(const std::string& text, int inner_w) {
    std::string top = "+" + std::string(inner_w, '-') + "+";
    std::string mid = "|" + Fit(text, inner_w) + "|";
    std::string bot = "+" + std::string(inner_w, '-') + "+";
    return {top, mid, bot};
}

std::vector<std::string> ConsoleRenderer::ColumnConveyor(
    const std::string& label,
    const std::vector<std::string>& items_top_to_bottom,
    int inner_w
) {
    std::vector<std::string> lines;
    lines.push_back(label);
    for (const auto& t0 : items_top_to_bottom) {
        std::string t = t0.empty() ? " " : t0;
        auto b = Box3(t, inner_w);
        lines.insert(lines.end(), b.begin(), b.end());
    }
    return lines;
}

static void place_sprite(std::string& line, int offset, const std::string& sprite) {
    if (offset < 0) return;
    if ((int)line.size() < offset) line += std::string(offset - (int)line.size(), ' ');
    // ensure line long enough
    if ((int)line.size() < offset + (int)sprite.size()) {
        line += std::string(offset + (int)sprite.size() - (int)line.size(), ' ');
    }
    for (int i = 0; i < (int)sprite.size(); ++i) {
        char c = sprite[i];
        if (c != ' ') line[offset + i] = c;
    }
}

void ConsoleRenderer::Render(const RenderSnapshot& s) {
    if (opt_.clear_screen) ClearScreen();

    const int innerW = opt_.box_inner_width;
    const int colBoxW = innerW + 2; // "|" + inner + "|"

    // ===== 顶部信息 =====
    if (!s.level_desc.empty()) {
        std::cout << "Level information: " << s.level_desc << "\n";
    }
    if (!s.last_event.empty()) {
        std::cout << s.last_event << "\n";
    }
    std::cout << "Executed instructions: " << s.executed << "\n\n";

    // ===== IN / OUT 内容 =====
    int inShow = std::max(opt_.max_in_preview, 1);
    std::vector<std::string> inItems(inShow);
    for (int i = 0; i < inShow; ++i) {
        int idx_from_back = (int)s.inbox_queue.size() - 1 - i;
        if (idx_from_back >= 0) inItems[i] = std::to_string(s.inbox_queue[idx_from_back]);
    }

    int outShow = std::max(opt_.max_out_preview, 1);
    std::vector<std::string> outItems(outShow);
    for (int i = 0; i < outShow; ++i) {
        int idx_from_back = (int)s.outbox_queue.size() - 1 - i; // 最新输出在 back
        if (idx_from_back >= 0) outItems[i] = std::to_string(s.outbox_queue[idx_from_back]);
    }

    auto inCol = ColumnConveyor("IN ", inItems, innerW);
    auto outCol = ColumnConveyor("OUT", outItems, innerW);

    // ===== 空地（Tiles）：一排盒子 + 一排编号 =====
    std::string floorTop, floorMid, floorBot, floorIdx;
    size_t tileN = std::min(s.tile_value.size(), s.tile_taken.size());
    for (size_t i = 0; i < tileN; ++i) {
        std::string v = (s.tile_taken[i] ? std::to_string(s.tile_value[i]) : " ");
        auto b = Box3(v, innerW);
        floorTop += b[0] + " ";
        floorMid += b[1] + " ";
        floorBot += b[2] + " ";

        std::ostringstream tmp;
        tmp << i;
        // 用盒子宽度对齐下标（box宽 + 一个空格）
        floorIdx += Fit(tmp.str(), colBoxW + 1);
    }

    // ===== 机器人移动表现：在 Tiles 上方加一条“机器人轨道” =====
    // sprite 宽度 7
    const int spriteW = 7;
    std::vector<std::string> robotSprite = {
        "   o   ",
        "  /|\\  ",
        "  / \\  "
    };

    // 轨道宽度：至少覆盖 tiles 的宽度；如果 tiles 很少，给个最小宽
    int railW = std::max<int>((int)floorTop.size(), 30);
    std::string rail1(railW, ' '), rail2(railW, ' '), rail3(railW, ' ');

    // 计算机器人应该出现的 offset
    int offset = 0;
    if (s.robot_pos == RobotPosKind::In) {
        offset = 0;
    } else if (s.robot_pos == RobotPosKind::Out) {
        offset = std::max(0, railW - spriteW);
    } else if (s.robot_pos == RobotPosKind::Tile) {
        int idx = s.robot_tile_index;
        if (idx < 0) idx = 0;
        // 每个 tile 盒子长度为 (colBoxW + 1)（含空格）
        offset = idx * (colBoxW + 1);
        // 让机器人尽量对齐到盒子左侧；并保证不越界
        offset = std::min(offset, std::max(0, railW - spriteW));
    } else {
        // Idle：居中
        offset = std::max(0, railW / 2 - spriteW / 2);
    }

    place_sprite(rail1, offset, robotSprite[0]);
    place_sprite(rail2, offset, robotSprite[1]);
    place_sprite(rail3, offset, robotSprite[2]);

    // ===== 当前积木随机器人移动（Carrying block follows robot position） =====
    // 只在 has_holding=true 时绘制“携带的积木”；没有积木时不绘制，自然也就“不移动”。
    const int boxW = colBoxW; // 盒子宽度（含边框）
    int carryOffset = offset + std::max(0, (spriteW - boxW) / 2);
    carryOffset = std::min(carryOffset, std::max(0, railW - boxW));

    std::string carry1(railW, ' '), carry2(railW, ' '), carry3(railW, ' ');
    if (s.has_holding) {
        auto cbox = Box3(std::to_string(s.holding_value), innerW);
        place_sprite(carry1, carryOffset, cbox[0]);
        place_sprite(carry2, carryOffset, cbox[1]);
        place_sprite(carry3, carryOffset, cbox[2]);
    }

    // ===== 右侧：程序列表 =====
    std::vector<std::string> codeLines;
    codeLines.push_back("==== CODE ====");
    for (int i = 0; i < (int)s.program_lines.size(); ++i) {
        int idx = i + 1;
        std::ostringstream oss;
        oss << (idx == s.ip ? ">" : " ")
            << std::setw(3) << idx << " "
            << s.program_lines[i];
        codeLines.push_back(oss.str());
    }

    // ===== 中间列：Carrying + RobotRail + Tiles =====
    std::vector<std::string> midLines;
    midLines.push_back("Carrying (moves with robot):");
    midLines.push_back(carry1);
    midLines.push_back(carry2);
    midLines.push_back(carry3);
    midLines.push_back("");
    midLines.push_back("Robot:");
    midLines.push_back(rail1);
    midLines.push_back(rail2);
    midLines.push_back(rail3);
    midLines.push_back("");
    midLines.push_back("Tiles:");
    midLines.push_back(floorTop);
    midLines.push_back(floorMid);
    midLines.push_back(floorBot);
    midLines.push_back(floorIdx);

    // ===== 拼接布局：IN | MID | OUT | CODE =====
    int H = 0;
    H = std::max(H, (int)inCol.size());
    H = std::max(H, (int)outCol.size());
    H = std::max(H, (int)midLines.size());
    H = std::max(H, (int)codeLines.size());

    auto getLine = [](const std::vector<std::string>& v, int i) -> std::string {
        if (i < 0 || i >= (int)v.size()) return "";
        return v[i];
    };

    const int leftW = colBoxW + 2;
    const int outW  = colBoxW + 2;

    // midW: 至少容纳 robot rail + tiles（不截断移动效果）
    int midW = 56;
    for (const auto& ln : midLines) midW = std::max(midW, (int)ln.size());

    const int codeW = 44;

    for (int i = 0; i < H; ++i) {
        std::cout
            << Fit(getLine(inCol, i), leftW) << "  "
            << Fit(getLine(midLines, i), midW) << "  "
            << Fit(getLine(outCol, i), outW) << "  "
            << Fit(getLine(codeLines, i), codeW)
            << "\n";
    }
    std::cout << std::flush;

    if (opt_.pause_each_step) {
        std::cout << "\n(Press ENTER to continue...)";
        std::cout.flush();
        std::cin.get();
    }
}
