#pragma once
#include "Base.h"
#include <vector>
using namespace FeEGE;
namespace FeEGE{
/**
 * @namespace FeEGE
 * @brief FeEGE�����������ײ���ģ��
 */

typedef std::vector<Position> Polygon;  ///< ��������Ͷ��壬�ɶ����������

/**
 * @brief �������͹������Ƿ���ײ
 * @param shapeA ��һ������ζ��㼯��
 * @param shapeB �ڶ�������ζ��㼯��
 * @return �Ƿ�����ײ
 */
bool isTouched(const std::vector<Position>& shapeA, const std::vector<Position>& shapeB);

/**
 * @brief �������������������ת�任���͹������Ƿ��ཻ��
 *
 * �˺�����Դ������״�������š���ָ��ԭ����ת����ͨ�� GJK + EPA �㷨������ײ��⡣
 *
 * @param shapeA ԭʼ��͹����� A �㼯���ֲ��ռ����꣩
 * @param scaleA ����� A ����������
 * @param originA ����� A ����ת����
 * @param angleA ����� A ����ת�Ƕȣ���λ�����ȣ���ʱ��Ϊ����
 * @param shapeB ԭʼ��͹����� B �㼯���ֲ��ռ����꣩
 * @param scaleB ����� B ����������
 * @param originB ����� B ����ת����
 * @param angleB ����� B ����ת�Ƕȣ���λ�����ȣ���ʱ��Ϊ����
 *
 * @return true ���������״�ڱ任��������ײ�����ص���
 * @return false ���������״�ڱ任��δ������ײ
 */
bool isTouched(
    const std::vector<Position>& shapeA, double scaleA, const Position& originA, double angleA,
    const std::vector<Position>& shapeB, double scaleB, const Position& originB, double angleB
);

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
 * @return �������(��ֵ��ʾ�ص�)
 */
double getSeparateDistance(const std::vector<Position>& shapeA,
                         const std::vector<Position>& shapeB,
                         const Position& direction);
                         
/**
 * @brief ��������������������ת�任���͹������ڸ��������ϵĴ�͸��ȡ�
 *
 * �˺����ڽ�������״�������š���ָ��ԭ����ת�任֮��
 * ʹ�� support ӳ��ķ�ʽ������������ĳһ�����ϵ� Minkowski ���ͶӰ���롣
 * ͨ��������ײ�������������ж���
 *
 * @param shapeA ԭʼ��͹����� A �㼯���ֲ��ռ����꣩
 * @param scaleA ����� A ����������
 * @param originA ����� A ����ת����
 * @param angleA ����� A ����ת�Ƕȣ���λ�����ȣ���ʱ��Ϊ����
 * @param shapeB ԭʼ��͹����� B �㼯���ֲ��ռ����꣩
 * @param scaleB ����� B ����������
 * @param originB ����� B ����ת����
 * @param angleB ����� B ����ת�Ƕȣ���λ�����ȣ���ʱ��Ϊ����
 * @param direction Ҫ���㴩͸�ķ������������赥λ����
 *
 * @return double ���� shapeA �� shapeB �ڸ÷����ϵĴ�͸��ȣ���Ϊ��ֵ��˵��δ�ص���
 */
double getSeparateDistance(
    const std::vector<Position>& shapeA, double scaleA, const Position& originA, double angleA,
    const std::vector<Position>& shapeB, double scaleB, const Position& originB, double angleB,
    const Position& direction
);

} // namespace FeEGE
