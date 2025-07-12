#pragma once
#include <vector>
#include <math.h>
#include <string>
#include <graphics.h>

namespace FeEGE {
	
extern bool Lpressed;

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

inline const SimpleOptional<std::string> nullopt{};

}
