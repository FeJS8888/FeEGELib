#include "FeEGELib.h"
using namespace FeEGE;

/**
 * 测试 Panel 的 scale 和 Layout 的配合使用
 * 
 * 此测试演示了当 Panel 设置了缩放比例后，FlexLayout 能否正确布局子控件。
 * 
 * 预期行为：
 * 1. 未缩放的 Panel (scale=1.0) 应该正确布局三个按钮（水平排列）
 * 2. 缩放后的 Panel (scale=1.5) 应该以相同的相对位置布局按钮，但整体放大 1.5 倍
 * 3. 两个 Panel 中的按钮应该保持相同的相对间距和对齐方式
 */
int main() {
    _setmode(_fileno(stdout), _O_WTEXT);
    init(800, 600);
    
    // 创建一个水平布局，左对齐，带间距和内边距
    shared_ptr<Layout> layout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Row)
        .setPadding(20)
        .setSpacing(10)
        .setAlign(LayoutAlign::Start)
        .build();

    // ====== 测试 1: 未缩放的 Panel (scale = 1.0) ======
    Button* btn1 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮1")
        .setRadius(8)
        .build();
    
    Button* btn2 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮2")
        .setRadius(8)
        .build();
    
    Button* btn3 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮3")
        .setRadius(8)
        .build();

    Panel* panel = PanelBuilder()
        .setCenter(400, 200)
        .setSize(300, 100)
        .setRadius(10)
        .setLayout(layout)
        .addChild({btn1, btn2, btn3})
        .setBackground(EGERGB(230, 230, 250))
        .build();

    // ====== 测试 2: 缩放后的 Panel (scale = 1.5) ======
    // 虽然现在 Layout 不会修改 spacing_ 成员，但为了测试清晰，使用独立的布局实例
    shared_ptr<Layout> layout2 = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Row)
        .setPadding(20)
        .setSpacing(10)
        .setAlign(LayoutAlign::Start)
        .build();

    Button* btn4 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮4")
        .setRadius(8)
        .build();
    
    Button* btn5 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮5")
        .setRadius(8)
        .build();
    
    Button* btn6 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮6")
        .setRadius(8)
        .build();

    Panel* panelScaled = PanelBuilder()
        .setCenter(400, 400)
        .setSize(300, 100)
        .setRadius(10)
        .setLayout(layout2)
        .addChild({btn4,btn5,btn6})
        .setBackground(EGERGB(250, 230, 230))
        .build();
    
    // 设置 scale AFTER 添加 children
    // 这会同时缩放 Panel 和所有子控件
    panelScaled->setScale(1.5);

    assignOrder({panel, panelScaled});

    start();
    
    // Cleanup
    delete panel;
    delete panelScaled;
    delete btn1;
    delete btn2;
    delete btn3;
    delete btn4;
    delete btn5;
    delete btn6;
    
    return 0;
}
