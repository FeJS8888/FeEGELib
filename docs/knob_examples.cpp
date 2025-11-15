/**
 * @file knob_examples.cpp
 * @brief Knob 组件示例代码
 * @details 展示如何使用新的 Knob 组件及其 Builder 模式
 * 
 * 将此代码复制到您的 main.cpp 中即可运行示例
 */

#include "include/FeEGELib.h"

using namespace FeEGE;

int main() {
    // 初始化窗口
    init(1200, 800);
    setcaption("Knob Component Demo");
    
    // 设置背景色
    setbkcolor(EGERGB(245, 245, 250));
    
    // ========== 示例 1: 基本 Knob（使用 Builder） ==========
    Knob* knob1 = KnobBuilder()
        .setIdentifier(L"knob1")
        .setCenter(200, 200)
        .setRadius(80)
        .setRange(0, 100)
        .setStep(1)
        .setValue(50)
        .setColor(EGERGB(33, 150, 243), EGERGB(230, 230, 230))
        .setShowValue(true)
        .setOnChange([](double val) {
            // 值改变时的回调
            // printf("Knob 1 value: %.0f\n", val);
        })
        .build();
    
    // 添加标签
    Text* label1 = TextBuilder()
        .setIdentifier(L"label1")
        .setPosition(200, 320)
        .setContent(L"基本 Knob (0-100)")
        .setFont(16, L"微软雅黑")
        .setColor(BLACK)
        .setAlign(TextAlign::Center)
        .build();
    
    // ========== 示例 2: 自定义颜色 Knob ==========
    Knob* knob2 = KnobBuilder()
        .setIdentifier(L"knob2")
        .setCenter(400, 200)
        .setRadius(80)
        .setRange(0, 360)
        .setStep(5)
        .setValue(180)
        .setColor(EGERGB(255, 87, 34), EGERGB(255, 224, 178))
        .setShowValue(true)
        .setOnChange([](double val) {
            // printf("Knob 2 angle: %.0f\n", val);
        })
        .build();
    
    Text* label2 = TextBuilder()
        .setIdentifier(L"label2")
        .setPosition(400, 320)
        .setContent(L"角度选择器 (0-360°)")
        .setFont(16, L"微软雅黑")
        .setColor(BLACK)
        .setAlign(TextAlign::Center)
        .build();
    
    // ========== 示例 3: 小步进 Knob（浮点数）==========
    Knob* knob3 = KnobBuilder()
        .setIdentifier(L"knob3")
        .setCenter(600, 200)
        .setRadius(80)
        .setRange(0.0, 1.0)
        .setStep(0.05)
        .setValue(0.5)
        .setColor(EGERGB(76, 175, 80), EGERGB(200, 230, 201))
        .setShowValue(true)
        .build();
    
    Text* label3 = TextBuilder()
        .setIdentifier(L"label3")
        .setPosition(600, 320)
        .setContent(L"音量控制 (0.0-1.0)")
        .setFont(16, L"微软雅黑")
        .setColor(BLACK)
        .setAlign(TextAlign::Center)
        .build();
    
    // ========== 示例 4: 禁用状态 Knob ==========
    Knob* knob4 = KnobBuilder()
        .setIdentifier(L"knob4")
        .setCenter(800, 200)
        .setRadius(80)
        .setRange(0, 100)
        .setValue(75)
        .setDisabled(true)
        .setShowValue(true)
        .build();
    
    Text* label4 = TextBuilder()
        .setIdentifier(L"label4")
        .setPosition(800, 320)
        .setContent(L"禁用状态")
        .setFont(16, L"微软雅黑")
        .setColor(EGERGB(150, 150, 150))
        .setAlign(TextAlign::Center)
        .build();
    
    // ========== 示例 5: 只读状态 Knob ==========
    Knob* knob5 = KnobBuilder()
        .setIdentifier(L"knob5")
        .setCenter(1000, 200)
        .setRadius(80)
        .setRange(0, 100)
        .setValue(30)
        .setReadonly(true)
        .setShowValue(true)
        .setColor(EGERGB(156, 39, 176), EGERGB(225, 190, 231))
        .build();
    
    Text* label5 = TextBuilder()
        .setIdentifier(L"label5")
        .setPosition(1000, 320)
        .setContent(L"只读状态")
        .setFont(16, L"微软雅黑")
        .setColor(BLACK)
        .setAlign(TextAlign::Center)
        .build();
    
    // ========== 示例 6: 偏移角度 Knob ==========
    Knob* knob6 = KnobBuilder()
        .setIdentifier(L"knob6")
        .setCenter(200, 500)
        .setRadius(80)
        .setRange(-50, 50)
        .setStep(1)
        .setValue(0)
        .setOffsetAngle(90)  // 旋转 90 度
        .setShowValue(true)
        .setColor(EGERGB(255, 193, 7), EGERGB(255, 243, 224))
        .build();
    
    Text* label6 = TextBuilder()
        .setIdentifier(L"label6")
        .setPosition(200, 620)
        .setContent(L"偏移角度 (-50 to 50)")
        .setFont(16, L"微软雅黑")
        .setColor(BLACK)
        .setAlign(TextAlign::Center)
        .build();
    
    // ========== 示例 7: 边界测试 (min = max) ==========
    Knob* knob7 = KnobBuilder()
        .setIdentifier(L"knob7")
        .setCenter(400, 500)
        .setRadius(80)
        .setRange(42, 42)  // 边界情况：min = max
        .setValue(42)
        .setShowValue(true)
        .setColor(EGERGB(103, 58, 183), EGERGB(209, 196, 233))
        .build();
    
    Text* label7 = TextBuilder()
        .setIdentifier(L"label7")
        .setPosition(400, 620)
        .setContent(L"边界测试 (min=max)")
        .setFont(16, L"微软雅黑")
        .setColor(BLACK)
        .setAlign(TextAlign::Center)
        .build();
    
    // ========== 示例 8: 温度控制 (-20 到 40) ==========
    Knob* knob8 = KnobBuilder()
        .setIdentifier(L"knob8")
        .setCenter(600, 500)
        .setRadius(80)
        .setRange(-20, 40)
        .setStep(0.5)
        .setValue(22)
        .setShowValue(true)
        .setColor(EGERGB(0, 188, 212), EGERGB(178, 235, 242))
        .setFontSize(24)
        .build();
    
    Text* label8 = TextBuilder()
        .setIdentifier(L"label8")
        .setPosition(600, 620)
        .setContent(L"温度控制 (°C)")
        .setFont(16, L"微软雅黑")
        .setColor(BLACK)
        .setAlign(TextAlign::Center)
        .build();
    
    // ========== 示例 9: 不显示值的 Knob ==========
    Knob* knob9 = KnobBuilder()
        .setIdentifier(L"knob9")
        .setCenter(800, 500)
        .setRadius(80)
        .setRange(0, 10)
        .setStep(1)
        .setValue(5)
        .setShowValue(false)  // 不显示值
        .setColor(EGERGB(233, 30, 99), EGERGB(248, 187, 208))
        .build();
    
    Text* label9 = TextBuilder()
        .setIdentifier(L"label9")
        .setPosition(800, 620)
        .setContent(L"不显示值")
        .setFont(16, L"微软雅黑")
        .setColor(BLACK)
        .setAlign(TextAlign::Center)
        .build();
    
    // ========== 示例 10: 缩放 Knob ==========
    Knob* knob10 = KnobBuilder()
        .setIdentifier(L"knob10")
        .setCenter(1000, 500)
        .setRadius(60)  // 较小的基础半径
        .setRange(0, 100)
        .setValue(60)
        .setScale(1.3)  // 放大 1.3 倍
        .setShowValue(true)
        .setColor(EGERGB(63, 81, 181), EGERGB(197, 202, 233))
        .build();
    
    Text* label10 = TextBuilder()
        .setIdentifier(L"label10")
        .setPosition(1000, 620)
        .setContent(L"缩放 (scale=1.3)")
        .setFont(16, L"微软雅黑")
        .setColor(BLACK)
        .setAlign(TextAlign::Center)
        .build();
    
    // 添加标题
    Text* title = TextBuilder()
        .setIdentifier(L"title")
        .setPosition(600, 50)
        .setContent(L"Knob 组件演示 - 拖拽旋钮改变数值")
        .setFont(28, L"微软雅黑")
        .setColor(EGERGB(33, 33, 33))
        .setAlign(TextAlign::Center)
        .build();
    
    // 启动事件循环
    start();
    
    return 0;
}
