#include "FeEGELib.h"

using namespace FeEGE;

/**
 * 多行 InputBox 的交互回归测试。
 *
 * CI 会通过 CMake 自动发现并编译该测试程序；在 Windows 桌面环境运行后，
 * 可手动验证：显式/自动换行、滚轮纵向滚动、跨行拖选、键盘导航、Enter、
 * 剪贴板操作和 IME。底部输入框显式设置为单行，用于兼容性对照。
 */
int main() {
    SetProcessDPIAware();
    init(1120, 760);

    Text* title = TextBuilder()
        .setPosition(560, 42)
        .setAlign(TextAlign::Center)
        .setContent(L"InputBox multiline regression test")
        .setFont(28, L"Microsoft YaHei")
        .setColor(EGERGB(45, 55, 72))
        .build();

    Text* instructions = TextBuilder()
        .setPosition(560, 82)
        .setAlign(TextAlign::Center)
        .setContent(L"Try the 100K text with wheel, drag selection, Ctrl+A, paste, and Up/Down/Home/End.")
        .setFont(15, L"Microsoft YaHei")
        .setColor(EGERGB(90, 100, 115))
        .build();

    std::wstring sample =
        L"First explicit line.\n"
        L"Second line is intentionally long enough to exercise automatic wrapping inside the editor.\n"
        L"第三行包含中文文本，用来覆盖无空格文本的自动换行。\n"
        L"Fourth line: [] {} () / \\ + - = * & % 0123456789.\n"
        L"Fifth line.\nSixth line.\nSeventh line.\nEighth line.\nNinth line.\n"
        L"Tenth line makes the content higher than the visible text area, so the internal vertical scroll range is exercised.";

    const std::wstring noSpaceChunk =
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz"
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz";
    std::wstring longUnbrokenParagraph = L"Long unbroken paragraph: ";
    while(longUnbrokenParagraph.size() < 70000) {
        longUnbrokenParagraph += noSpaceChunk;
    }
    // Keep the pathological paragraph at the document start so Ctrl+Home,
    // a middle click, and Ctrl+End exercise the three costly insertion sites.
    sample = longUnbrokenParagraph + L"\n" + sample;
    while(sample.size() < 105000) {
        sample += L"\nOrdinary paragraph text keeps the long-document layout and scrolling path exercised.";
        sample += L"\n\u4e2d\u6587\u6bb5\u843d\u7528\u4e8e\u9a8c\u8bc1\u6ca1\u6709\u7a7a\u683c\u548c\u663e\u5f0f\u6362\u884c\u7684\u7ec4\u5408\u60c5\u51b5\u3002";
        sample += L"\n" + noSpaceChunk + noSpaceChunk + noSpaceChunk + noSpaceChunk;
    }

    InputBox* multiline = InputBoxBuilder()
        .setIdentifier(L"multilineInput")
        .setCenter(0, 0)
        .setSize(620, 420)
        .setRadius(10)
        .setTextHeight(19)
        .setMaxLength(150000)
        .setMultiline()
        .setContent(sample)
        .build();

    Panel* multilinePanel = PanelBuilder()
        .setIdentifier(L"multilinePanel")
        .setCenter(560, 340)
        .setSize(680, 470)
        .setRadius(14)
        .setBackground(EGERGB(238, 242, 247))
        .addChild(multiline)
        .build();

    Text* multilineLabel = TextBuilder()
        .setPosition(560, 108)
        .setAlign(TextAlign::Center)
        .setContent(L"Multi-line mode: setMultiline()")
        .setFont(19, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();

    Text* singleLineLabel = TextBuilder()
        .setPosition(205, 650)
        .setAlign(TextAlign::Center)
        .setContent(L"Single-line mode:")
        .setFont(17, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();

    InputBox* singleLine = InputBoxBuilder()
        .setIdentifier(L"singleLineInput")
        .setCenter(625, 650)
        .setSize(620, 44)
        .setRadius(8)
        .setTextHeight(18)
        .setMultiline(false)
        .setContent(L"Explicit setMultiline(false): existing single-line horizontal scrolling remains available.")
        .build();

    assignOrder({
        title,
        instructions,
        multilineLabel,
        multilinePanel,
        singleLineLabel,
        singleLine
    });

    start();
    return 0;
}
