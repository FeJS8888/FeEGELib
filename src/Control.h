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
#include <string>
#include <iostream>
#include <functional>
#include <set>
#include "sys_edit.h"  // mouse_msg 等结构体
#include "Element.h"
#include "Base.h"

using namespace FeEGE;

/**
 * @brief 所有可绘制控件的基类
 */
class Widget {
public:
    /**
     * @brief 虚析构，保证子类析构正常调用
     */
    virtual ~Widget() {}
    
    virtual void handleEvent(const mouse_msg& msg);
    bool is_global = true;

    /**
     * @brief 绘制控件到指定图像
     * @param dst 绘制目标图像
     * @param offsetX 控件中心 x 坐标（相对于 dst）
     * @param offsetY 控件中心 y 坐标（相对于 dst）
     */
    virtual void draw(PIMAGE dst, int offsetX, int offsetY) = 0;
    virtual void draw() = 0;
    
    virtual void setPosition(int x,int y) = 0;
    
    virtual void setScale(double s) = 0;
};

/**
 * @brief 圆角面板控件类，支持嵌套子控件
 */
class Panel : public Widget {
public:
    /**
     * @brief 构造函数
     * @param centerX 面板中心 x 坐标
     * @param centerY 面板中心 y 坐标
     * @param w 宽度
     * @param h 高度
     * @param r 圆角半径
     * @param bg 背景颜色
     */
    Panel(int centerX, int centerY, double w, double h, double r, color_t bg);
    
    ~Panel();

    /**
     * @brief 添加子控件
     * @param child 控件指针
     * @param offsetX 相对于面板中心的 x 偏移
     * @param offsetY 相对于面板中心的 y 偏移
     */
    void addChild(Widget* child, double offsetX, double offsetY);

    /**
     * @brief 绘制 Panel 到指定图层
     * @param dst 目标图像
     * @param x 粘贴到 dst 中的中心 x 坐标
     * @param y 粘贴到 dst 中的中心 y 坐标
     */
    void draw(PIMAGE dst, int x, int y) override;
    void draw() override;
    
    void setPosition(int x,int y) override;
    
    Position getPosition();
    
    void setScale(double s) override;
    
    double getScale();
    
    void handleEvent(const mouse_msg& msg) override;

private:
    int cx, cy;  ///< 中心坐标
    double width, height;
    double radius;
    double origin_width, origin_height;
    double origin_radius;
    color_t bgColor;
    double scale = 1;
    PIMAGE layer = newimage(width, height);
    PIMAGE maskLayer = newimage(width,height);

    std::vector<Widget*> children;
    std::vector<Position> childOffsets;  ///< 每个子控件的相对偏移（以面板中心为参考）
};

/**
 * @brief 波纹动画效果结构体
 */
struct Ripple{
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
    void draw(PIMAGE dst,double scale) const;
};

/**
 * @brief 按钮控件类
 */
class Button : public Widget {
private:
    int centerX, centerY;   ///< 中心坐标
    double width, height;      ///< 宽高
    double radius;             ///< 圆角半径
    double origin_width, origin_height;
    double origin_radius;
    double scale = 1;
    double left, top;          ///< 左上角坐标
    std::vector<Ripple> ripples; ///< 波纹效果集合
    PIMAGE btnLayer = nullptr;  ///< 按钮图层
    PIMAGE maskLayer = nullptr; ///< 遮罩图层
    std::string content;
    std::function<void(void)> on_click_event = nullptr;
    color_t color;
    bool needRedraw = true;

public:
    /**
     * @brief 构造函数
     * @param cx 中心x坐标
     * @param cy 中心y坐标
     * @param w 宽度
     * @param h 高度
     * @param r 圆角半径
     */
    Button(int cx, int cy, double w, double h, double r);

    /**
     * @brief 析构函数
     */
    virtual ~Button();

    /**
     * @brief 绘制按钮
     * @param content 按钮文本
     */
    virtual void draw(PIMAGE dst,int x,int y) override;
    virtual void draw() override;

    /**
     * @brief 处理鼠标事件
     * @param msg 鼠标消息
     */
    virtual void handleEvent(const mouse_msg& msg);
    
    void setContent(const std::string& str);
    std::string getContent();

    /**
     * @brief 检查点是否在按钮内
     * @param x x坐标
     * @param y y坐标
     * @return 是否在按钮内
     */
    bool isInside(int x, int y) const;
    
    void setPosition(int x,int y) override;
    
    void setScale(double s) override;
    
    void setOnClickEvent(std::function<void(void)> func);

    void setColor(color_t col);
};

/**
 * @brief 输入框控件类
 */
class InputBox : public Widget {
private:
    int centerX, centerY;   ///< 中心坐标
    double width, height;     ///< 宽高
    double radius;            ///< 圆角半径
    double origin_width, origin_height;
    double origin_radius;
    double scale = 1;
    double left, top;         ///< 左上角坐标
    std::vector<Ripple> ripples; ///< 波纹效果集合
    PIMAGE btnLayer = nullptr;  ///< 按钮图层
    PIMAGE maskLayer = nullptr; ///< 遮罩图层
    std::string content;   ///< 输入内容
    FeEGE::sys_edit inv;   ///< 输入控件
    bool on_focus;         ///< 是否获得焦点
    color_t color = EGERGB(245, 245, 235);

public:
    /**
     * @brief 构造函数
     * @param cx 中心x坐标
     * @param cy 中心y坐标
     * @param w 宽度
     * @param h 高度
     * @param r 圆角半径
     */
    InputBox(int cx, int cy, double w, double h, double r);

    /**
     * @brief 析构函数
     */
    virtual ~InputBox();

    /**
     * @brief 绘制输入框
     */
    virtual void draw(PIMAGE dst,int x,int y) override;
    virtual void draw() override;

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
    bool isInside(int x, int y) const;

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
    
    void setPosition(int x,int y) override;
    
    void setScale(double s) override;
};

/**
 * @brief 滑动条控件类
 */
class Slider : public Widget {
private:
    double left, top;
	double width, height;    ///< 位置和尺寸
    double radius;              ///< 滑块半径
    double origin_width, origin_height;
    double origin_radius;
    int m_dragOffset;          ///< 拖动偏移量
    double m_value;            ///< 当前值(0.0-1.0)
    bool m_dragging;           ///< 是否正在拖动
    color_t m_bgColor, m_fgColor; ///< 背景和前景色
    std::function<void(double)> m_onChange; ///< 值改变回调
	bool m_hover = false;            ///< 鼠标是否悬停
	bool m_pressed = false;          ///< 是否正在按住
	float m_scale = 1.0f;            ///< 当前缩放比例
	double thickness = 4;
	double origin_thickness = 4;
	double scale = 1;


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
    void create(int x, int y, double w, double h);

    /**
     * @brief 绘制滑动条
     */
    virtual void draw(PIMAGE dst,int x,int y) override ;
    virtual void draw();
    
    /**
     * @brief 检查点是否在滑块内
     * @param x x坐标
     * @param y y坐标
     * @return 是否在输入框内
     */
    bool isInside(int x, int y) const;

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
     * @brief 设置粗细
     * @param t 粗细
     */
    void setThickness(double t);

    /**
     * @brief 设置值改变回调
     * @param callback 回调函数
     */
    void setOnChange(std::function<void(double)> callback);
    
    void setPosition(int x,int y) override;
    
    void setScale(double s) override;
};

 /**
  * @brief 用于构建 Panel 对象的构建器类
  */
class PanelBuilder {
public:
    PanelBuilder& setCenter(int x, int y);
    PanelBuilder& setSize(double w, double h);
    PanelBuilder& setRadius(double r);
    PanelBuilder& setBackground(color_t color);
    PanelBuilder& setScale(double s);
    PanelBuilder& addChild(Widget* child, double offsetX, double offsetY);
    Panel* build();

private:
    int cx = 0, cy = 0;
    double width = 100, height = 50;
    double radius = 5;
    color_t bg = EGERGB(240, 240, 240);
    double scale = 1.0;
    std::vector<Widget*> children;
    std::vector<Position> childOffsets;  ///< 每个子控件的相对偏移（以面板中心为参考）
};

/**
 * @brief 用于构建 Button 对象的构建器类
 */
class ButtonBuilder {
public:
    ButtonBuilder& setCenter(int x, int y);
    ButtonBuilder& setSize(double w, double h);
    ButtonBuilder& setRadius(double r);
    ButtonBuilder& setContent(const std::string& text);
    ButtonBuilder& setScale(double s);
    ButtonBuilder& setOnClick(std::function<void()> func);
    ButtonBuilder& setColor(color_t col);
    Button* build();

private:
    int cx = 0, cy = 0;
    double width = 100, height = 50;
    double radius = 8;
    std::string content = "Button";
    double scale = 1.0;
    color_t color = EGERGB(245, 245, 235);
    std::function<void()> onClick = nullptr;
};

/**
 * @brief 用于构建 InputBox 对象的构建器类
 */
class InputBoxBuilder {
public:
    InputBoxBuilder& setCenter(int x, int y);
    InputBoxBuilder& setSize(double w, double h);
    InputBoxBuilder& setRadius(double r);
    InputBoxBuilder& setContent(const std::string& text);
    InputBoxBuilder& setMaxLength(int maxLen);
    InputBoxBuilder& setScale(double s);
    InputBox* build();

private:
    int cx = 0, cy = 0;
    double width = 160, height = 40;
    double radius = 6;
    std::string content = "";
    int maxLength = 100;
    double scale = 1.0;
};

/**
 * @brief 用于构建 Slider 对象的构建器类
 */
class SliderBuilder {
public:
    /**
     * @brief 设置位置
     * @param cx 中心 x 坐标
     * @param cy 中心 y 坐标
     */
    SliderBuilder& setCenter(int cx, int cy);

    /**
     * @brief 设置尺寸
     * @param w 宽度
     * @param h 高度
     */
    SliderBuilder& setSize(double w, double h);

    /**
     * @brief 设置颜色
     * @param bg 背景色
     * @param fg 前景色
     */
    SliderBuilder& setColor(color_t bg, color_t fg);

    /**
     * @brief 设置粗细
     * @param t 粗细值
     */
    SliderBuilder& setThickness(double t);

    /**
     * @brief 设置初始进度
     * @param v 初始值 (0.0~1.0)
     */
    SliderBuilder& setProgress(double v);

    /**
     * @brief 设置缩放比例
     * @param s 缩放比例
     */
    SliderBuilder& setScale(double s);

    /**
     * @brief 设置值改变时的回调函数
     * @param callback 回调函数
     */
    SliderBuilder& setOnChange(std::function<void(double)> callback);

    /**
     * @brief 构建并返回 Slider 对象
     */
    Slider* build();

private:
    int x = 0, y = 0;
    double width = 200, height = 20;
    color_t bgColor = EGERGB(200, 200, 200);
    color_t fgColor = EGERGB(100, 100, 255);
    double thickness = 4;
    double progress = 0.0;
    double scale = 1.0;
    std::function<void(double)> onChange = nullptr;
};

extern std::set<Widget*> widgets;
