/**
 * @file Control.h
 * @brief 包含各种UI控件的声明（按钮、输入框、滑动条、文本等）
 * @author FeJS8888
 */

#pragma once

#include <graphics.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <functional>
#include "sys_edit.h"  // mouse_msg 等结构体
#include "FeEGELib.h"

using namespace FeEGE;

/**
 * @brief 波纹动画效果结构体
 */
struct Ripple {
    int x, y;           ///< 波纹中心坐标
    int maxRadius;      ///< 最大半径
    int life;           ///< 生命周期
    int age = 0;        ///< 当前年龄

    /**
     * @brief 构造函数
     * @param _x 中心x坐标
     * @param _y 中心y坐标
     * @param _r 最大半径
     * @param _life 生命周期
     */
    Ripple(int _x, int _y, int _r, int _life);

    /**
     * @brief 检查波纹是否存活
     * @return 是否存活
     */
    bool alive() const;

    /**
     * @brief 更新波纹状态
     */
    void update();

    /**
     * @brief 绘制波纹
     * @param dst 目标图像
     */
    void draw(PIMAGE dst) const;
};

/**
 * @brief 按钮控件类
 */
class Button {
private:
    int centerX, centerY;   ///< 中心坐标
    int width, height;      ///< 宽高
    int radius;             ///< 圆角半径
    int left, top;          ///< 左上角坐标
    std::vector<Ripple> ripples; ///< 波纹效果集合
    PIMAGE btnLayer = nullptr;  ///< 按钮图层
    PIMAGE maskLayer = nullptr; ///< 遮罩图层

public:
    /**
     * @brief 构造函数
     * @param cx 中心x坐标
     * @param cy 中心y坐标
     * @param w 宽度
     * @param h 高度
     * @param r 圆角半径
     */
    Button(int cx, int cy, int w, int h, int r);

    /**
     * @brief 析构函数
     */
    virtual ~Button();

    /**
     * @brief 绘制按钮
     * @param content 按钮文本
     */
    virtual void draw(const std::string& content);

    /**
     * @brief 处理鼠标事件
     * @param msg 鼠标消息
     */
    virtual void handleEvent(const mouse_msg& msg);

    /**
     * @brief 检查点是否在按钮内
     * @param x x坐标
     * @param y y坐标
     * @return 是否在按钮内
     */
    bool isInside(int x, int y);
};

/**
 * @brief 输入框控件类
 */
class InputBox {
private:
    int centerX, centerY;   ///< 中心坐标
    int width, height;     ///< 宽高
    int radius;            ///< 圆角半径
    int left, top;         ///< 左上角坐标
    std::vector<Ripple> ripples; ///< 波纹效果集合
    PIMAGE btnLayer = nullptr;  ///< 按钮图层
    PIMAGE maskLayer = nullptr; ///< 遮罩图层
    std::string content;   ///< 输入内容
    FeEGE::sys_edit inv;   ///< 输入控件
    bool on_focus;         ///< 是否获得焦点

public:
    /**
     * @brief 构造函数
     * @param cx 中心x坐标
     * @param cy 中心y坐标
     * @param w 宽度
     * @param h 高度
     * @param r 圆角半径
     */
    InputBox(int cx, int cy, int w, int h, int r);

    /**
     * @brief 析构函数
     */
    virtual ~InputBox();

    /**
     * @brief 绘制输入框
     */
    virtual void draw();

    /**
     * @brief 处理鼠标事件
     * @param msg 鼠标消息
     */
    virtual void handleEvent(const mouse_msg& msg);

    /**
     * @brief 检查点是否在输入框内
     * @param x x坐标
     * @param y y坐标
     * @return 是否在输入框内
     */
    bool isInside(int x, int y);

    /**
     * @brief 设置输入内容
     * @param s 内容字符串
     */
    void setContent(const std::string& s);

    /**
     * @brief 设置最大输入长度
     * @param maxlen 最大长度
     */
    void setMaxlen(int maxlen);
};

/**
 * @brief 滑动条控件类
 */
class Slider {
private:
    int m_x, m_y, m_w, m_h;    ///< 位置和尺寸
    int m_radius;              ///< 滑块半径
    int m_dragOffset;          ///< 拖动偏移量
    double m_value;            ///< 当前值(0.0-1.0)
    bool m_dragging;           ///< 是否正在拖动
    color_t m_bgColor, m_fgColor; ///< 背景和前景色
    std::function<void(double)> m_onChange; ///< 值改变回调

public:
    /**
     * @brief 默认构造函数
     */
    Slider();

    /**
     * @brief 创建滑动条
     * @param x x坐标
     * @param y y坐标
     * @param w 宽度
     * @param h 高度
     */
    void create(int x, int y, int w, int h);

    /**
     * @brief 绘制滑动条
     */
    void draw();

    /**
     * @brief 处理鼠标事件
     * @param msg 鼠标消息
     */
    virtual void handleEvent(const mouse_msg& msg);

    /**
     * @brief 设置进度值
     * @param v 进度值(0.0-1.0)
     */
    void setProgress(double v);

    /**
     * @brief 获取当前进度值
     * @return 当前进度值
     */
    double getProgress() const;

    /**
     * @brief 设置颜色
     * @param bg 背景色
     * @param fg 前景色
     */
    void setColor(color_t bg, color_t fg);

    /**
     * @brief 设置值改变回调
     * @param callback 回调函数
     */
    void setOnChange(std::function<void(double)> callback);
};

/**
 * @brief 文本显示控件类
 */
class Text {
private:
    std::string content;   ///< 文本内容
    float scale = 1.0f;    ///< 缩放比例
    std::string font;      ///< 字体
    int wrapWidth = 300;   ///< 自动换行宽度

    /**
     * @brief UTF8字符串分割
     * @param str 输入字符串
     * @return 分割后的字符数组
     */
    static std::vector<std::string> utf8_split(const std::string& str);

public:
    /**
     * @brief 设置字体
     * @param f 字体名称
     */
    void setFont(const std::string& f);

    /**
     * @brief 设置缩放比例
     * @param s 缩放比例
     */
    void setScale(float s);

    /**
     * @brief 设置内容
     * @param str 文本内容
     */
    void setContent(const std::string& str);

    /**
     * @brief 设置换行宽度
     * @param w 宽度值
     */
    void setWrapWidth(int w);

    /**
     * @brief 绘制文本
     * @param x x坐标
     * @param y y坐标
     */
    void draw(int x, int y) const;
};
