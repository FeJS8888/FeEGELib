#pragma once
#include <vector>

/**
 * @namespace FeEGE
 * @brief FeEGE物理引擎的碰撞检测模块
 */
namespace FeEGE {
    
    /**
     * @class Vector2
     * @brief 二维向量类，用于表示物理引擎中的位置、方向等
     */
    class Vector2 {
    public:
        double x, y;  ///< 向量的x和y分量

        /**
         * @brief 默认构造函数
         * @details 初始化向量为(0,0)
         */
        Vector2();

        /**
         * @brief 带参数的构造函数
         * @param _x x分量值
         * @param _y y分量值
         */
        Vector2(double _x, double _y);

        /**
         * @brief 向量加法运算符重载
         * @param rhs 右操作数向量
         * @return 相加后的新向量
         */
        Vector2 operator+(const Vector2& rhs) const;

        /**
         * @brief 向量减法运算符重载
         * @param rhs 右操作数向量
         * @return 相减后的新向量
         */
        Vector2 operator-(const Vector2& rhs) const;

        /**
         * @brief 向量数乘运算符重载
         * @param s 标量值
         * @return 缩放后的新向量
         */
        Vector2 operator*(double s) const;

        /**
         * @brief 计算向量点积
         * @param rhs 另一个向量
         * @return 两个向量的点积值
         */
        double dot(const Vector2& rhs) const;

        /**
         * @brief 计算向量长度
         * @return 向量的模长
         */
        double length() const;

        /**
         * @brief 向量归一化
         * @return 归一化后的单位向量
         */
        Vector2 normalize() const;

        /**
         * @brief 获取垂直向量
         * @return 垂直于当前向量的向量
         */
        Vector2 perpendicular() const;
    };

    /**
     * @class PenetrationInfo
     * @brief 碰撞穿透信息类
     */
    class PenetrationInfo {
    public:
        Vector2 direction;  ///< 碰撞方向(法向量)
        double depth;       ///< 穿透深度
    };

    typedef std::vector<Vector2> Polygon;  ///< 多边形类型定义，由顶点向量组成

    /**
     * @brief 检测两个凸多边形是否碰撞
     * @param shapeA 第一个多边形顶点集合
     * @param shapeB 第二个多边形顶点集合
     * @return 是否发生碰撞
     */
    bool isTouched(const std::vector<Vector2>& shapeA, const std::vector<Vector2>& shapeB);

    /**
     * @brief 获取最后一次碰撞的详细信息
     * @return 包含碰撞方向和深度的PenetrationInfo对象
     */
    const PenetrationInfo& getLastInfo();

    /**
     * @brief 计算两个形状在指定方向上的分离距离
     * @param shapeA 第一个形状
     * @param shapeB 第二个形状
     * @param direction 检测方向
     * @return 分离距离(负值表示重叠)
     */
    double getSeparateDistance(const std::vector<Vector2>& shapeA,
                             const std::vector<Vector2>& shapeB,
                             const Vector2& direction);

} // namespace FeEGE
