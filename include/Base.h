/**
 * @file Base.h
 * @brief FeEGE图形库基础类型和工具函数定义
 * @author FeJS8888
 * @version 2.10.0.0
 * @date 2025-10-26
 * 
 * 本文件包含了FeEGE库的核心数据结构、工具函数和全局变量声明
 * 包括向量运算、碰撞信息、可选值类型、字符串处理等基础功能
 */

#pragma once

// C++ 标准库
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <locale>
#include <map>
#include <malloc.h>
#include <mutex>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

// Windows 系统 API
#include <windows.h>
#include <tlhelp32.h>
#include <fcntl.h>
#include <io.h>

// 图形库
#include <graphics.h>

// 第三方库
#include "json.hpp"

// 自定义头文件
#include "sys_edit.h"
#include "font_manager.h"

namespace FeEGE {
	
extern bool Lpressed;                       ///< 鼠标左键按下状态
extern double InputPositionX,InputPositionY; ///< 输入位置坐标
extern bool needReflushCursor;              ///< 是否需要刷新鼠标光标
extern LPSTR current_cursor;                ///< 当前鼠标光标句柄
extern WNDPROC g_oldWndProc;                ///< 原窗口过程函数指针
extern FontManager fontManager;             ///< 全局字体管理器

/**
 * @class Position
 * @brief 二维向量类，用于表示物理引擎中的位置、方向等
 */
class Position {
public:
    double x, y;  ///< 向量的x和y分量

    /**
     * @brief 默认构造函数
     * @details 初始化向量为(0,0)
     */
    Position();

    /**
     * @brief 带参数的构造函数
     * @param _x x分量值
     * @param _y y分量值
     */
    Position(double _x, double _y);

    /**
     * @brief 向量加法运算符重载
     * @param rhs 右操作数向量
     * @return 相加后的新向量
     */
    Position operator+(const Position& rhs) const;

    /**
     * @brief 向量减法运算符重载
     * @param rhs 右操作数向量
     * @return 相减后的新向量
     */
    Position operator-(const Position& rhs) const;

    /**
     * @brief 向量数乘运算符重载
     * @param s 标量值
     * @return 缩放后的新向量
     */
    Position operator*(double s) const;

    /**
     * @brief 计算向量点积
     * @param rhs 另一个向量
     * @return 两个向量的点积值
     */
    double dot(const Position& rhs) const;

    /**
     * @brief 计算向量长度
     * @return 向量的模长
     */
    double length() const;

    /**
     * @brief 向量归一化
     * @return 归一化后的单位向量
     */
    Position normalize() const;

    /**
     * @brief 获取垂直向量
     * @return 垂直于当前向量的向量
     */
    Position perpendicular() const;
};

/**
 * @class PenetrationInfo
 * @brief 碰撞穿透信息类
 */
class PenetrationInfo {
public:
    Position direction;  ///< 碰撞方向(法向量)
    double depth;       ///< 穿透深度
};

typedef std::vector<Position> Polygon;  ///< 多边形类型定义，由顶点向量组成

/**
 * @brief 限制数值在指定区间内
 * @tparam T1 返回值类型
 * @tparam T2 最小值类型
 * @tparam T3 最大值类型
 * @param val 要限制的值
 * @param minVal 最小值
 * @param maxVal 最大值
 * @return 限制后的值
 */
template<typename T1,typename T2,typename T3>
T1 clamp(T1 val, T2 minVal, T3 maxVal) {
    return (val < minVal) ? minVal : (val > maxVal) ? maxVal : val;
}

/**
 * @brief 自动检测编码并输出文本
 * @param x x坐标
 * @param y y坐标
 * @param str 字符串(UTF-8或ANSI编码)
 * @details 传入UTF-8或ANSI编码字符串，自动转换后调用EGE输出文本
 */
void outtextxy_auto(int x, int y, const std::string& str);

/**
 * @brief 刷新鼠标状态
 * @param msg 鼠标消息
 */
void reflushMouseStatu(mouse_msg msg);

/**
 * @brief 自动将字符串转换为宽字符串
 * @param str 输入字符串
 * @return 转换后的宽字符串
 */
std::wstring autoToWString(const std::string& str);

/**
 * @class SimpleOptional
 * @brief 简单的可选值容器类
 * @tparam T 值类型
 * @details 类似于std::optional，用于表示可能存在或不存在的值
 */
template<typename T>
class SimpleOptional {
    bool has_val;  ///< 是否有值
    T val;         ///< 存储的值

public:
    /**
     * @brief 默认构造函数，创建空的可选值
     */
    SimpleOptional() : has_val(false) {}
    
    /**
     * @brief 带值构造函数
     * @param value 初始值
     */
    SimpleOptional(const T& value) : has_val(true), val(value) {}

    /**
     * @brief 检查是否有值
     * @return 是否有值
     */
    bool hasValue() const { return has_val; }
    
    /**
     * @brief bool转换运算符
     * @return 是否有值
     */
    operator bool() const { return has_val; }
    
    /**
     * @brief 获取值的常量引用
     * @return 值的常量引用
     */
    const T& value() const { return val; }
    
    /**
     * @brief 获取值的引用
     * @return 值的引用
     */
    T& value() { return val; }

    /**
     * @brief 获取值或默认值
     * @param default_val 默认值
     * @return 存在则返回存储的值，否则返回默认值
     */
    const T& value_or(const T& default_val) const {
        return has_val ? val : default_val;
    }
};

#define nullopt SimpleOptional<std::string>()  ///< 空可选值常量

/**
 * @brief 设置鼠标光标
 * @param cursor 光标句柄
 */
void setCursor(LPSTR cursor);

/**
 * @brief 获取当前鼠标光标
 * @return 光标句柄
 */
LPSTR getCursor();

/**
 * @brief 将浮点数转换为整数
 * @param x 浮点数
 * @return 转换后的整数
 */
int fixed(double x);

}

/**
 * @brief 设置输入法位置
 * @param hwnd 窗口句柄
 * @param x x坐标
 * @param y y坐标
 */
void SetIMEPosition(HWND hwnd, int x, int y);

/**
 * @brief 获取输入法组合字符串
 * @param hwnd 窗口句柄
 * @param dwIndex 索引标识
 * @return 组合字符串
 */
std::wstring GetImeCompositionString(HWND hwnd, DWORD dwIndex);

/**
 * @brief FeEGE窗口过程回调函数
 * @param hWnd 窗口句柄
 * @param message 消息类型
 * @param wParam 消息参数
 * @param lParam 消息参数
 * @return 消息处理结果
 */
LRESULT CALLBACK FeEGEProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);