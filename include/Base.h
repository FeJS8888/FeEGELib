#pragma once

// C++ ��׼��
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

// Windows ϵͳ API
#include <windows.h>
#include <tlhelp32.h>
#include <fcntl.h>
#include <io.h>

// ͼ�ο�
#include <graphics.h>

// ��������
#include "json.hpp"

// �Զ���ͷ�ļ�
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
 * @brief ��ά�����࣬���ڱ�ʾ���������е�λ�á������
 */
class Position {
public:
    double x, y;  ///< ������x��y����

    /**
     * @brief Ĭ�Ϲ��캯��
     * @details ��ʼ������Ϊ(0,0)
     */
    Position();

    /**
     * @brief �������Ĺ��캯��
     * @param _x x����ֵ
     * @param _y y����ֵ
     */
    Position(double _x, double _y);

    /**
     * @brief �����ӷ����������
     * @param rhs �Ҳ���������
     * @return ��Ӻ��������
     */
    Position operator+(const Position& rhs) const;

    /**
     * @brief �����������������
     * @param rhs �Ҳ���������
     * @return ������������
     */
    Position operator-(const Position& rhs) const;

    /**
     * @brief �����������������
     * @param s ����ֵ
     * @return ���ź��������
     */
    Position operator*(double s) const;

    /**
     * @brief �����������
     * @param rhs ��һ������
     * @return ���������ĵ��ֵ
     */
    double dot(const Position& rhs) const;

    /**
     * @brief ������������
     * @return ������ģ��
     */
    double length() const;

    /**
     * @brief ������һ��
     * @return ��һ����ĵ�λ����
     */
    Position normalize() const;

    /**
     * @brief ��ȡ��ֱ����
     * @return ��ֱ�ڵ�ǰ����������
     */
    Position perpendicular() const;
};

/**
 * @class PenetrationInfo
 * @brief ��ײ��͸��Ϣ��
 */
class PenetrationInfo {
public:
    Position direction;  ///< ��ײ����(������)
    double depth;       ///< ��͸���
};

typedef std::vector<Position> Polygon;  ///< ��������Ͷ��壬�ɶ����������

// ģ�庯����������ֵ��������
template<typename T1,typename T2,typename T3>
T1 clamp(T1 val, T2 minVal, T3 maxVal) {
    return (val < minVal) ? minVal : (val > maxVal) ? maxVal : val;
}

// ���� UTF-8 �� ANSI �����ַ������Զ�ת������� EGE ����ı�
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