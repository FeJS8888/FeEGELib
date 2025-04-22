#pragma once
#include "Base.h"
#include <vector>
using namespace FeEGE;
namespace FeEGE{
/**
 * @namespace FeEGE
 * @brief FeEGE物理引擎的碰撞检测模块
 */

typedef std::vector<Position> Polygon;  ///< 多边形类型定义，由顶点向量组成

/**
 * @brief 检测两个凸多边形是否碰撞
 * @param shapeA 第一个多边形顶点集合
 * @param shapeB 第二个多边形顶点集合
 * @return 是否发生碰撞
 */
bool isTouched(const std::vector<Position>& shapeA, const std::vector<Position>& shapeB);

/**
 * @brief 检测两个经过缩放与旋转变换后的凸多边形是否相交。
 *
 * 此函数会对传入的形状进行缩放、绕指定原点旋转后，再通过 GJK + EPA 算法进行碰撞检测。
 *
 * @param shapeA 原始的凸多边形 A 点集（局部空间坐标）
 * @param scaleA 多边形 A 的缩放因子
 * @param originA 多边形 A 的旋转中心
 * @param angleA 多边形 A 的旋转角度（单位：弧度，逆时针为正）
 * @param shapeB 原始的凸多边形 B 点集（局部空间坐标）
 * @param scaleB 多边形 B 的缩放因子
 * @param originB 多边形 B 的旋转中心
 * @param angleB 多边形 B 的旋转角度（单位：弧度，逆时针为正）
 *
 * @return true 如果两个形状在变换后发生了碰撞（有重叠）
 * @return false 如果两个形状在变换后未发生碰撞
 */
bool isTouched(
    const std::vector<Position>& shapeA, double scaleA, const Position& originA, double angleA,
    const std::vector<Position>& shapeB, double scaleB, const Position& originB, double angleB
);

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
double getSeparateDistance(const std::vector<Position>& shapeA,
                         const std::vector<Position>& shapeB,
                         const Position& direction);
                         
/**
 * @brief 计算两个经过缩放与旋转变换后的凸多边形在给定方向上的穿透深度。
 *
 * 此函数在将两个形状经过缩放、绕指定原点旋转变换之后，
 * 使用 support 映射的方式，计算它们在某一方向上的 Minkowski 差的投影距离。
 * 通常用于碰撞后修正或方向性判定。
 *
 * @param shapeA 原始的凸多边形 A 点集（局部空间坐标）
 * @param scaleA 多边形 A 的缩放因子
 * @param originA 多边形 A 的旋转中心
 * @param angleA 多边形 A 的旋转角度（单位：弧度，逆时针为正）
 * @param shapeB 原始的凸多边形 B 点集（局部空间坐标）
 * @param scaleB 多边形 B 的缩放因子
 * @param originB 多边形 B 的旋转中心
 * @param angleB 多边形 B 的旋转角度（单位：弧度，逆时针为正）
 * @param direction 要计算穿透的方向向量（无需单位化）
 *
 * @return double 返回 shapeA 与 shapeB 在该方向上的穿透深度（若为负值则说明未重叠）
 */
double getSeparateDistance(
    const std::vector<Position>& shapeA, double scaleA, const Position& originA, double angleA,
    const std::vector<Position>& shapeB, double scaleB, const Position& originB, double angleB,
    const Position& direction
);

} // namespace FeEGE
