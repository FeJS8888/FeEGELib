#include "FeEGELib.h"
using namespace FeEGE;

/**
 * 测试嵌套 Panel 的交互
 * 
 * 本测试演示：
 * 1. Panel 内嵌套 Panel 的正常显示
 * 2. 嵌套 Panel 中的控件可以正常交互
 */
int main() {
    SetProcessDPIAware();
    _setmode(_fileno(stdout), _O_WTEXT);
    init(1000, 700);
    
    // ===== 测试1：Panel 内的 Panel 包含按钮 =====
    
    // 内层 Panel 中的按钮
    Button* innerBtn1 = ButtonBuilder()
        .setSize(100, 35)
        .setRadius(6)
        .setContent(L"内层1")
        .setColor(EGERGB(231, 76, 60))
        .setOnClick([]() {
            std::wcout << L"内层按钮1被点击！" << std::endl;
        })
        .build();
    
    Button* innerBtn2 = ButtonBuilder()
        .setSize(100, 35)
        .setRadius(6)
        .setContent(L"内层2")
        .setColor(EGERGB(46, 204, 113))
        .setOnClick([]() {
            std::wcout << L"内层按钮2被点击！" << std::endl;
        })
        .build();
    
    // 内层 Panel 的布局
    shared_ptr<Layout> innerLayout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .setSpacing(15)
        .setPadding(15)
        .build();
    
    // 创建内层 Panel
    Panel* innerPanel = PanelBuilder()
        .setCenter(0, 0)  // 相对于外层 Panel 中心
        .setSize(150, 130)
        .setRadius(8)
        .setLayout(innerLayout)
        .addChild({innerBtn1, innerBtn2})
        .setBackground(EGERGB(189, 195, 199))
        .build();
    
    // 外层 Panel 的按钮
    Button* outerBtn = ButtonBuilder()
        .setSize(120, 40)
        .setRadius(8)
        .setContent(L"外层按钮")
        .setColor(EGERGB(52, 152, 219))
        .setOnClick([]() {
            std::wcout << L"外层按钮被点击！" << std::endl;
        })
        .build();
    
    // 外层 Panel 的布局
    shared_ptr<Layout> outerLayout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .setSpacing(20)
        .setPadding(20)
        .build();
    
    // 创建外层 Panel
    Panel* outerPanel = PanelBuilder()
        .setCenter(250, 300)
        .setSize(250, 300)
        .setRadius(12)
        .setLayout(outerLayout)
        .addChild({innerPanel, outerBtn})
        .setBackground(EGERGB(236, 240, 241))
        .build();
    
    Text* label1 = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(250, 100)
        .setContent(L"嵌套Panel测试")
        .setFont(22, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    // ===== 测试2：Panel 内的 Panel 包含输入框 =====
    
    // 内层 Panel 中的输入框
    InputBox* innerInput = InputBoxBuilder()
        .setCenter(0, 0)
        .setSize(150, 35)
        .setRadius(6)
        .setTextHeight(16)
        .build();
    
    // 创建内层 Panel（包含输入框）
    Panel* innerPanel2 = PanelBuilder()
        .setCenter(0, 0)
        .setSize(180, 80)
        .setRadius(8)
        .addChild(innerInput, 0, 0)
        .setBackground(EGERGB(189, 195, 199))
        .build();
    
    Text* innerLabel = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(0, 0)
        .setContent(L"输入测试")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    // 外层 Panel 的布局
    shared_ptr<Layout> outerLayout2 = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .setSpacing(25)
        .setPadding(25)
        .build();
    
    // 创建外层 Panel
    Panel* outerPanel2 = PanelBuilder()
        .setCenter(600, 300)
        .setSize(280, 250)
        .setRadius(12)
        .setLayout(outerLayout2)
        .addChild({innerLabel, innerPanel2})
        .setBackground(EGERGB(236, 240, 241))
        .build();
    
    Text* label2 = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(600, 100)
        .setContent(L"输入框嵌套测试")
        .setFont(22, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    // ===== 测试3：三层嵌套 =====
    
    // 最内层按钮
    Button* deepBtn = ButtonBuilder()
        .setSize(80, 30)
        .setRadius(5)
        .setContent(L"深层")
        .setColor(EGERGB(155, 89, 182))
        .setOnClick([]() {
            std::wcout << L"深层按钮被点击！" << std::endl;
        })
        .build();
    
    // 第二层 Panel
    Panel* midPanel = PanelBuilder()
        .setCenter(0, 0)
        .setSize(120, 80)
        .setRadius(6)
        .addChild(deepBtn, 0, 0)
        .setBackground(EGERGB(189, 195, 199))
        .build();
    
    // 第一层 Panel
    Panel* topPanel = PanelBuilder()
        .setCenter(0, 0)
        .setSize(160, 120)
        .setRadius(8)
        .addChild(midPanel, 0, 0)
        .setBackground(EGERGB(149, 165, 166))
        .build();
    
    // 最外层 Panel
    Panel* rootPanel = PanelBuilder()
        .setCenter(420, 550)
        .setSize(200, 160)
        .setRadius(10)
        .addChild(topPanel, 0, 0)
        .setBackground(EGERGB(236, 240, 241))
        .build();
    
    Text* label3 = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(420, 480)
        .setContent(L"三层嵌套")
        .setFont(18, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    // 说明文字
    Text* info = TextBuilder()
        .setAlign(TextAlign::Left)
        .setPosition(50, 650)
        .setContent(L"测试嵌套Panel中的按钮和输入框是否可以正常交互")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(127, 140, 141))
        .build();
    
    assignOrder({
        label1, outerPanel,
        label2, outerPanel2,
        label3, rootPanel,
        info
    });
    
    start();
    
    return 0;
}
