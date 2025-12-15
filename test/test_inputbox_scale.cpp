#include "FeEGELib.h"
using namespace FeEGE;

/**
 * 测试 InputBox 在 Panel 缩放时的文本位置正确性
 * 
 * 此测试演示了当 Panel 设置了缩放比例后，InputBox 中的文本位置是否正确。
 * 
 * 预期行为：
 * 1. 未缩放的 Panel (scale=1.0) 中的 InputBox 文本位置正确
 * 2. 缩放后的 Panel (scale=1.5, 2.0) 中的 InputBox 文本应保持相同的相对位置
 * 3. 文本的 padding（左边距）应该随着缩放比例正确缩放
 */
int main() {
    _setmode(_fileno(stdout), _O_WTEXT);
    init(900, 700);
    
    // ====== 测试 1: 未缩放的 Panel (scale = 1.0) ======
    InputBox* input1 = InputBoxBuilder()
        .setCenter(0, 0)
        .setSize(200, 40)
        .setRadius(6)
        .setContent(L"未缩放 1.0x")
        .build();
    
    Panel* panel1 = PanelBuilder()
        .setCenter(250, 150)
        .setSize(250, 100)
        .setRadius(10)
        .addChild({input1})
        .setBackground(EGERGB(230, 240, 255))
        .build();

    // ====== 测试 2: 缩放 1.5 倍的 Panel ======
    InputBox* input2 = InputBoxBuilder()
        .setCenter(0, 0)
        .setSize(200, 40)
        .setRadius(6)
        .setContent(L"缩放 1.5x")
        .build();
    
    Panel* panel2 = PanelBuilder()
        .setCenter(250, 350)
        .setSize(250, 100)
        .setRadius(10)
        .addChild({input2})
        .setBackground(EGERGB(240, 255, 230))
        .build();
    
    panel2->setScale(1.5);

    // ====== 测试 3: 缩放 2.0 倍的 Panel ======
    InputBox* input3 = InputBoxBuilder()
        .setCenter(0, 0)
        .setSize(200, 40)
        .setRadius(6)
        .setContent(L"缩放 2.0x")
        .build();
    
    Panel* panel3 = PanelBuilder()
        .setCenter(650, 350)
        .setSize(250, 100)
        .setRadius(10)
        .addChild({input3})
        .setBackground(EGERGB(255, 240, 230))
        .build();
    
    panel3->setScale(2.0);

    assignOrder({panel1, panel2, panel3});

    start();
    
    // Cleanup
    delete panel1;
    delete panel2;
    delete panel3;
    delete input1;
    delete input2;
    delete input3;
    
    return 0;
}
