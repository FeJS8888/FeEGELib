#pragma once
#include<vector>

namespace FeEGE {

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

}
