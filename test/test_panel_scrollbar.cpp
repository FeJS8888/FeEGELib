#include "FeEGELib.h"
using namespace FeEGE;

/**
 * 测试 Panel 滚动条功能
 * 
 * 演示了：
 * 1. Panel 启用滚动条后，内容超出面板高度时自动显示滚动条
 * 2. 可通过顶部/底部按钮点击滚动
 * 3. 可拖动滑块滚动
 * 4. 滑块大小与内容比例相关
 */
int main() {
    SetProcessDPIAware();
    _setmode(_fileno(stdout), _O_WTEXT);
    init(800, 600);
    
    // 创建多个按钮，内容超出Panel高度
    std::vector<Widget*> buttons;
    for (int i = 0; i < 12; ++i) {
        wchar_t buf[64];
        swprintf(buf, 64, L"按钮 %d", i + 1);
        Button* btn = ButtonBuilder()
            .setSize(200, 40)
            .setRadius(8)
            .setContent(buf)
            .setColor(EGERGB(100 + i * 10, 150, 200 - i * 10))
            .build();
        buttons.push_back(btn);
    }
    
    // 使用 Panel + FlexLayout + ScrollBar
    shared_ptr<Layout> layout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Start)
        .setSpacing(10)
        .setPadding(10)
        .build();
    
    Panel* panel = PanelBuilder()
        .setIdentifier(L"scrollPanel")
        .setCenter(400, 300)
        .setSize(300, 400)
        .setRadius(10)
        .setLayout(layout)
        .addChild(buttons)
        .setBackground(EGERGB(250, 250, 250))
        .setScrollBar(true, 16)
        .build();
    
    // 标题
    Text* title = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(400, 50)
        .setContent(L"Panel 滚动条测试")
        .setFont(24, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    Text* info = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(400, 550)
        .setContent(L"12个按钮超出Panel高度，右侧出现滚动条")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(127, 140, 141))
        .build();
    
    assignOrder({title, panel, info});
    
    start();
    
    return 0;
}
