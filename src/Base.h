#pragma once
#include<vector>

namespace FeEGE {

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

}
