#pragma once

// C++ 标准库
#include <algorithm>
#include <assert.h>
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
	
extern bool Lpressed;
extern double InputPositionX,InputPositionY;
extern bool needReflushCursor;
extern LPSTR current_cursor;
extern WNDPROC g_oldWndProc;
extern FontManager fontManager;

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

// 模板函数：限制数值在区间内
template<typename T1,typename T2,typename T3>
T1 clamp(T1 val, T2 minVal, T3 maxVal) {
    return (val < minVal) ? minVal : (val > maxVal) ? maxVal : val;
}

// 传入 UTF-8 或 ANSI 编码字符串，自动转换后调用 EGE 输出文本
void outtextxy_auto(int x, int y, const std::string& str);

void reflushMouseStatu(mouse_msg msg);

std::wstring autoToWString(const std::string& str);

template<typename T>
class SimpleOptional {
    bool has_val;
    T val;

public:
    SimpleOptional() : has_val(false) {}
    SimpleOptional(const T& value) : has_val(true), val(value) {}

    bool hasValue() const { return has_val; }
    operator bool() const { return has_val; }
    const T& value() const { return val; }
    T& value() { return val; }

    const T& value_or(const T& default_val) const {
        return has_val ? val : default_val;
    }
};

#define nullopt SimpleOptional<std::string>()

void setCursor(LPSTR cursor);
LPSTR getCursor();

int fixed(double x);

}

void SetIMEPosition(HWND hwnd, int x, int y);
std::wstring GetImeCompositionString(HWND hwnd, DWORD dwIndex);

LRESULT CALLBACK FeEGEProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);