#include "ConsoleRenderer.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

ConsoleRenderer::ConsoleRenderer(RenderOptions opt) : opt_(opt) {}

void ConsoleRenderer::ClearScreenAnsi() {
    // ANSI: 清屏 + 光标回到左上角。
    // 在 Windows Terminal / VSCode terminal / macOS terminal / Linux terminal 上一般可用。
    std::cout << "\x1b[2J\x1b[H";
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

void ConsoleRenderer::Render(const RenderSnapshot& s) {
    if (opt_.clear_screen) ClearScreenAnsi();

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

    // ===== IN / OUT =====
    // 注意：项目代码里 inputque 是“从末尾 pop”作为队首（下一块积木）。
    // 这里把“下一块”显示在最上方，所以从 back 开始取。
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

    // ===== 中间：当前积木 + 机器人 + 空地 =====
    auto holdBox = Box3(s.has_holding ? std::to_string(s.holding_value) : " ", innerW);
    std::vector<std::string> robot = {
        "   o   ",
        "  /|\\  ",
        "  / \\  "
    };

    // 空地：一排盒子 + 一排编号
    std::vector<std::string> floorLines;
    {
        std::ostringstream rowTop, rowMid, rowBot, rowIdx;
        size_t tileN = std::min(s.tile_value.size(), s.tile_taken.size());
        for (size_t i = 0; i < tileN; ++i) {
            std::string v = (s.tile_taken[i] ? std::to_string(s.tile_value[i]) : " ");
            auto b = Box3(v, innerW);
            rowTop << b[0] << " ";
            rowMid << b[1] << " ";
            rowBot << b[2] << " ";

            // 编号对齐
            std::ostringstream tmp;
            tmp << i;
            rowIdx << Fit(tmp.str(), colBoxW + 1);
        }
        floorLines.push_back(rowTop.str());
        floorLines.push_back(rowMid.str());
        floorLines.push_back(rowBot.str());
        floorLines.push_back(rowIdx.str());
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

    // ===== 拼接布局 =====
    std::vector<std::string> midLines;
    midLines.push_back("Holding:");
    midLines.insert(midLines.end(), holdBox.begin(), holdBox.end());
    midLines.push_back("");
    midLines.insert(midLines.end(), robot.begin(), robot.end());
    midLines.push_back("");
    midLines.push_back("Tiles:");
    midLines.insert(midLines.end(), floorLines.begin(), floorLines.end());

    int H = 0;
    H = std::max(H, (int)inCol.size());
    H = std::max(H, (int)outCol.size());
    H = std::max(H, (int)midLines.size());
    H = std::max(H, (int)codeLines.size());

    auto getLine = [](const std::vector<std::string>& v, int i) -> std::string {
        if (i < 0 || i >= (int)v.size()) return "";
        return v[i];
    };

    // 各列宽度（可按需调整）
    const int leftW = colBoxW + 2;
    const int midW = 56;
    const int outW = colBoxW + 2;
    const int codeW = 40;

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
