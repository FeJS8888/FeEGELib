#include "FeEGELib.h"
using namespace FeEGE;

/**
 * 测试 Box 布局容器
 * 
 * Box 是一个透明的布局容器，自动使用 FlexLayout 排列子控件
 * 本测试演示了：
 * 1. 使用 BoxBuilder 创建垂直和水平布局的 Box
 * 2. Box 不显示背景，只显示子控件
 * 3. Box 与 Panel 的对比
 */
int main() {
    SetProcessDPIAware();
    _setmode(_fileno(stdout), _O_WTEXT);
    init(1000, 700);
    
    // ===== 示例1：使用 Box 创建垂直布局 =====
    
    // 创建三个按钮
    Button* btn1 = ButtonBuilder()
        .setSize(150, 40)
        .setRadius(8)
        .setContent(L"按钮 1")
        .setColor(EGERGB(102, 126, 234))
        .build();
    
    Button* btn2 = ButtonBuilder()
        .setSize(150, 40)
        .setRadius(8)
        .setContent(L"按钮 2")
        .setColor(EGERGB(46, 204, 113))
        .build();
    
    Button* btn3 = ButtonBuilder()
        .setSize(150, 40)
        .setRadius(8)
        .setContent(L"按钮 3")
        .setColor(EGERGB(231, 76, 60))
        .build();
    
    // 使用 Box 垂直排列按钮（透明容器）
    Box* verticalBox = BoxBuilder()
        .setCenter(200, 300)
        .setSize(200, 300)
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .setSpacing(20)
        .setPadding(10)
        .addChild({btn1, btn2, btn3})
        .build();
    
    // 标题文字
    Text* titleBox = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(200, 100)
        .setContent(L"Box 容器（透明）")
        .setFont(24, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    // ===== 示例2：使用 Panel + FlexLayout 对比 =====
    
    Button* btn4 = ButtonBuilder()
        .setSize(150, 40)
        .setRadius(8)
        .setContent(L"按钮 4")
        .setColor(EGERGB(102, 126, 234))
        .build();
    
    Button* btn5 = ButtonBuilder()
        .setSize(150, 40)
        .setRadius(8)
        .setContent(L"按钮 5")
        .setColor(EGERGB(46, 204, 113))
        .build();
    
    Button* btn6 = ButtonBuilder()
        .setSize(150, 40)
        .setRadius(8)
        .setContent(L"按钮 6")
        .setColor(EGERGB(231, 76, 60))
        .build();
    
    // 使用 Panel + FlexLayout（有背景的容器）
    shared_ptr<Layout> layout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .setSpacing(20)
        .setPadding(10)
        .build();
    
    Panel* panel = PanelBuilder()
        .setCenter(500, 300)
        .setSize(200, 300)
        .setRadius(15)
        .setLayout(layout)
        .addChild({btn4, btn5, btn6})
        .setBackground(EGERGB(250, 250, 250))
        .build();
    
    Text* titlePanel = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(500, 100)
        .setContent(L"Panel 容器（有背景）")
        .setFont(24, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    // ===== 示例3：水平布局的 Box =====
    
    Button* btn7 = ButtonBuilder()
        .setSize(100, 40)
        .setRadius(8)
        .setContent(L"左")
        .setColor(EGERGB(155, 89, 182))
        .build();
    
    Button* btn8 = ButtonBuilder()
        .setSize(100, 40)
        .setRadius(8)
        .setContent(L"中")
        .setColor(EGERGB(52, 152, 219))
        .build();
    
    Button* btn9 = ButtonBuilder()
        .setSize(100, 40)
        .setRadius(8)
        .setContent(L"右")
        .setColor(EGERGB(241, 196, 15))
        .build();
    
    Box* horizontalBox = BoxBuilder()
        .setCenter(800, 300)
        .setSize(400, 100)
        .setDirection(LayoutDirection::Row)
        .setAlign(LayoutAlign::SpaceBetween)
        .setSpacing(10)
        .setPadding(20)
        .addChild({btn7, btn8, btn9})
        .build();
    
    Text* titleHBox = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(800, 100)
        .setContent(L"水平 Box")
        .setFont(24, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    // 说明文字
    Text* info = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(500, 550)
        .setContent(L"Box 是透明容器，Panel 有背景。两者都使用 FlexLayout 排列子控件。")
        .setFont(18, L"Microsoft YaHei")
        .setColor(EGERGB(127, 140, 141))
        .build();
    
    assignOrder({titleBox, verticalBox, titlePanel, panel, titleHBox, horizontalBox, info});
    
    start();
    
    return 0;
}
