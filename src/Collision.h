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
 * @brief FeEGE�����������ײ���ģ��
 */

/**
 * @brief �������͹������Ƿ���ײ
 * @param shapeA ��һ������ζ��㼯��
 * @param shapeB �ڶ�������ζ��㼯��
 * @return �Ƿ�����ײ
 */
bool isTouched(const std::vector<Position>& shapeA, const std::vector<Position>& shapeB);

/**
 * @brief ��ȡ���һ����ײ����ϸ��Ϣ
 * @return ������ײ�������ȵ�PenetrationInfo����
 */
const PenetrationInfo& getLastInfo();

/**
 * @brief ����������״��ָ�������ϵķ������
 * @param shapeA ��һ����״
 * @param shapeB �ڶ�����״
 * @param direction ��ⷽ��
 * @return �������
 */
double getSeparateDistance(const std::vector<Position>& shapeA,const std::vector<Position>& shapeB,const Position& direction);

/**
 * @brief ��ȡ����͹�������ָ�������ϵ�֧�ֵ��ͶӰ����
 * 
 * @param shapeA ��һ�������
 * @param shapeB �ڶ��������
 * @param direction ��λ��������
 * @return double �ڸ��������ϵĹ����֧ࣨ�ֺ�����ͶӰ��
 * 
 * @note
 * - �������δ�Ӵ�ʱ��������ֵ����ʾ��С���Ĺ��ƣ�
 * - ������νӴ���͸ʱ������ֵ����Ϊ 0 ��������**���ȼ���ʵ�ʴ�͸���**��
 * - �����ȡ��ʵ�Ĵ�͸��ȣ���ʹ�� `getLastInfo()` ��ͶӰ�� `direction * depth`��
 * - �������������ƶ�Ԥ���еĿ��ټ����ơ�
 */
double getSafeDistance(const std::vector<Position>& shapeA,const std::vector<Position>& shapeB,const Position& direction);

/**
 * @brief �任�������״�����ź���ת��
 * @param shape ԭʼ����ζ��㼯��
 * @param scale ���ű�����1.0Ϊԭʼ��С��
 * @param rotationOrigin ��ת���ĵ�����
 * @param rotationAngle ��ת�Ƕȣ������ƣ�
 * @return �任��Ķ���ζ��㼯��
 * @note �Ƚ������ţ��ٽ�����ת
 */
std::vector<Position> transformShape(const std::vector<Position>& shape,double scale,const Position& rotationOrigin,double rotationAngle);

/**
 * @brief �Զ���ζ��㼯�Ͻ���λ�ñ任��ƽ�ƣ�
 * @param shape ԭʼ����ζ��㼯��
 * @param origin �任��׼�㣨�ֲ�����ϵԭ�㣩
 * @param pos Ŀ��λ�ã���������ϵ��
 * @return �任��Ķ���ζ��㼯��
 * @details ������δӾֲ�����ϵ����originΪԭ�㣩ƽ�Ƶ���������ϵ�е�posλ��
 * @note 1. ���Ƚ�����ζ���ת��Ϊ�����origin�ľֲ�����
 *       2. Ȼ�󽫾ֲ�����ƽ�Ƶ�Ŀ��posλ��
 *       3. ���ֶ���ε���״�ͷ��򲻱�
 */
std::vector<Position> transformPosition(const std::vector<Position>& shape,const Position& origin,const Position& pos);

/**
 * @brief �жϵ��Ƿ���͹�������
 * @param polygon ͹����ζ��㼯�ϣ���˳ʱ�����ʱ��˳�����У�
 * @param point Ҫ���ĵ�����
 * @return ���Ƿ��ڶ������
 * @retval true ���ڶ�����ڻ����
 * @retval false ���ڶ������
 * @note ʹ�����߷�ʵ�֣�Ҫ�����α�����͹�����
 */
bool isPointInConvexPolygon(const std::vector<Position>& polygon,const Position& point);

std::vector<Position> readPolygonFromJSON(const std::string& file,const std::string& id);
} // namespace FeEGE
