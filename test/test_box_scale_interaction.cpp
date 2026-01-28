#include "FeEGELib.h"
using namespace FeEGE;

/**
 * 测试 Box 的缩放行为和内部 Panel 交互
 * 
 * 本测试演示：
 * 1. Box::setScale 使子控件相对于自己的中心缩放（位置不变）
 * 2. Box 内部的 Panel 可以正常交互
 */
int main() {
    SetProcessDPIAware();
    _setmode(_fileno(stdout), _O_WTEXT);
    init(1200, 800);
    
    // ===== 测试1：Box 缩放行为 - 子控件相对于自己中心缩放 =====
    
    // 创建三个按钮用于演示缩放
    Button* scaleBtn1 = ButtonBuilder()
        .setSize(100, 40)
        .setRadius(8)
        .setContent(L"按钮1")
        .setColor(EGERGB(102, 126, 234))
        .build();
    
    Button* scaleBtn2 = ButtonBuilder()
        .setSize(100, 40)
        .setRadius(8)
        .setContent(L"按钮2")
        .setColor(EGERGB(46, 204, 113))
        .build();
    
    Button* scaleBtn3 = ButtonBuilder()
        .setSize(100, 40)
        .setRadius(8)
        .setContent(L"按钮3")
        .setColor(EGERGB(231, 76, 60))
        .build();
    
    // 使用 Box 水平排列
    Box* scaleBox = BoxBuilder()
        .setCenter(300, 200)
        .setSize(400, 100)
        .setDirection(LayoutDirection::Row)
        .setAlign(LayoutAlign::SpaceBetween)
        .setSpacing(20)
        .setPadding(20)
        .addChild({scaleBtn1, scaleBtn2, scaleBtn3})
        .build();
    
    // 创建滑块控制缩放
    Slider* scaleSlider = SliderBuilder()
        .setSize(400, 30)
        .setCenter(300, 320)
        .setOnChange([=](double f){
            // 缩放范围：0.5 到 2.0
            scaleBox->setScale(0.5 + f * 1.5);
        })
        .setProgress(0.33f)  // 默认 scale = 1.0
        .build();
    
    Text* scaleLabel = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(300, 80)
        .setContent(L"缩放测试：按钮应在原位置放大/缩小")
        .setFont(20, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    // ===== 测试2：Box 内的 Panel 交互 =====
    
    // 创建一个输入框在 Panel 中
    InputBox* inputInPanel = InputBoxBuilder()
        .setCenter(0, 0)  // 相对于 Panel 中心
        .setSize(200, 40)
        .setRadius(8)
        .setTextHeight(18)
        .build();
    
    // 创建一个按钮在 Panel 中
    Button* btnInPanel = ButtonBuilder()
        .setSize(100, 40)
        .setRadius(8)
        .setContent(L"点击我")
        .setColor(EGERGB(155, 89, 182))
        .setOnClick([]() {
            std::wcout << L"Panel中的按钮被点击了！" << std::endl;
        })
        .build();
    
    // 创建 Panel 包含控件
    shared_ptr<Layout> panelLayout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .setSpacing(20)
        .setPadding(20)
        .build();
    
    Panel* innerPanel = PanelBuilder()
        .setCenter(0, 0)  // 相对于 Box 中心
        .setSize(250, 150)
        .setRadius(10)
        .setLayout(panelLayout)
        .addChild({inputInPanel, btnInPanel})
        .setBackground(EGERGB(236, 240, 241))
        .build();
    
    // 将 Panel 放入 Box
    Box* interactionBox = BoxBuilder()
        .setCenter(800, 200)
        .setSize(300, 200)
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .addChild({innerPanel})
        .build();
    
    Text* interactionLabel = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(800, 80)
        .setContent(L"交互测试：点击输入框和按钮")
        .setFont(20, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    // ===== 测试3：多个控件在 Box 中 =====
    
    Button* multiBtn1 = ButtonBuilder()
        .setSize(120, 40)
        .setRadius(8)
        .setContent(L"上")
        .setColor(EGERGB(52, 152, 219))
        .setOnClick([]() {
            std::wcout << L"点击了上按钮" << std::endl;
        })
        .build();
    
    Button* multiBtn2 = ButtonBuilder()
        .setSize(120, 40)
        .setRadius(8)
        .setContent(L"下")
        .setColor(EGERGB(46, 204, 113))
        .setOnClick([]() {
            std::wcout << L"点击了下按钮" << std::endl;
        })
        .build();
    
    Box* multiBox = BoxBuilder()
        .setCenter(550, 550)
        .setSize(200, 200)
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .setSpacing(30)
        .addChild({multiBtn1, multiBtn2})
        .build();
    
    // 缩放控制
    Slider* multiSlider = SliderBuilder()
        .setSize(200, 30)
        .setCenter(550, 670)
        .setOnChange([=](double f){
            multiBox->setScale(0.5 + f * 1.5);
        })
        .setProgress(0.33f)
        .build();
    
    Text* multiLabel = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(550, 450)
        .setContent(L"多控件缩放和交互")
        .setFont(20, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    // 说明文字
    Text* info = TextBuilder()
        .setAlign(TextAlign::Left)
        .setPosition(50, 730)
        .setContent(L"左：子控件相对自己中心缩放，位置不变  |  右：Panel内控件可正常交互")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(127, 140, 141))
        .build();
    
    assignOrder({
        scaleLabel, scaleBox, scaleSlider,
        interactionLabel, interactionBox,
        multiLabel, multiBox, multiSlider,
        info
    });
    
    start();
    
    return 0;
}
