#pragma once
#include "Base.h"
#include "json.hpp"
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <limits>
#include <algorithm>
#include <fstream>

using namespace FeEGE;

namespace FeEGE{
/**
 * @namespace FeEGE
 * @brief FeEGE物理引擎的碰撞检测模块
 */

/**
 * @brief 检测两个凸多边形是否碰撞
 * @param shapeA 第一个多边形顶点集合
 * @param shapeB 第二个多边形顶点集合
 * @return 是否发生碰撞
 */
bool isTouched(const std::vector<Position>& shapeA, const std::vector<Position>& shapeB);

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
 * @return 分离距离
 */
double getSeparateDistance(const std::vector<Position>& shapeA,const std::vector<Position>& shapeB,const Position& direction);

/**
 * @brief 获取两个凸多边形在指定方向上的支持点间投影距离
 * 
 * @param shapeA 第一个多边形
 * @param shapeB 第二个多边形
 * @param direction 单位方向向量
 * @return double 在给定方向上的估算间距（支持函数差投影）
 * 
 * @note
 * - 当多边形未接触时，返回正值，表示最小间距的估计；
 * - 当多边形接触或穿透时，返回值可能为 0 或负数，但**不等价于实际穿透深度**；
 * - 若需获取真实的穿透深度，请使用 `getLastInfo()` 并投影其 `direction * depth`；
 * - 本函数常用于移动预测中的快速间距估计。
 */
double getSafeDistance(const std::vector<Position>& shapeA,const std::vector<Position>& shapeB,const Position& direction);

/**
 * @brief 变换多边形形状（缩放和旋转）
 * @param shape 原始多边形顶点集合
 * @param scale 缩放比例（1.0为原始大小）
 * @param rotationOrigin 旋转中心点坐标
 * @param rotationAngle 旋转角度（弧度制）
 * @return 变换后的多边形顶点集合
 * @note 先进行缩放，再进行旋转
 */
std::vector<Position> transformShape(const std::vector<Position>& shape,double scale,const Position& rotationOrigin,double rotationAngle);

/**
 * @brief 对多边形顶点集合进行位置变换（平移）
 * @param shape 原始多边形顶点集合
 * @param origin 变换基准点（局部坐标系原点）
 * @param pos 目标位置（世界坐标系）
 * @return 变换后的多边形顶点集合
 * @details 将多边形从局部坐标系（以origin为原点）平移到世界坐标系中的pos位置
 * @note 1. 首先将多边形顶点转换为相对于origin的局部坐标
 *       2. 然后将局部坐标平移到目标pos位置
 *       3. 保持多边形的形状和方向不变
 */
std::vector<Position> transformPosition(const std::vector<Position>& shape,const Position& origin,const Position& pos);

/**
 * @brief 判断点是否在凸多边形内
 * @param polygon 凸多边形顶点集合（按顺时针或逆时针顺序排列）
 * @param point 要检测的点坐标
 * @return 点是否在多边形内
 * @retval true 点在多边形内或边上
 * @retval false 点在多边形外
 * @note 使用射线法实现，要求多边形必须是凸多边形
 */
bool isPointInConvexPolygon(const std::vector<Position>& polygon,const Position& point);

std::vector<Position> readPolygonFromJSON(const std::string& file,const std::string& id);
} // namespace FeEGE
