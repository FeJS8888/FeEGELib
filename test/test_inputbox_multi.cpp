#include "FeEGELib.h"
using namespace FeEGE;

/**
 * 测试多行 InputBox
 *
 * 演示了：
 * 1. 多行输入框的创建和显示
 * 2. 滚动条功能
 * 3. 多行文本换行
 */
int main() {
    SetProcessDPIAware();
    _setmode(_fileno(stdout), _O_WTEXT);
    init(800, 600);

    // 多行输入框
    InputBox* multiInput = InputBoxBuilder()
        .setCenter(400, 300)
        .setSize(350, 300)
        .setRadius(8)
        .setMultiline(true)
        .build();
    multiInput->setContent(L"第一行文本\n第二行文本\n第三行文本\n第四行文本\n第五行文本\n第六行文本\n第七行文本\n第八行文本");

    // 标题
    Text* title = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(400, 50)
        .setContent(L"多行输入框测试")
        .setFont(24, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();

    // 说明文字
    Text* info = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(400, 650)
        .setContent(L"测试内容：多行输入、垂直滚动、文本选择")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(127, 140, 141))
        .build();

    // 注册控件到绘图区域
    assignOrder({
        title, multiInput, info
    });

    start();

    return 0;
}
