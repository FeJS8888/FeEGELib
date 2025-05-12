#include "Base.h"
#include <math.h>

namespace FeEGE{
	
typedef std::vector<Position> Polygon;  ///< ��������Ͷ��壬�ɶ����������
	
// Position ʵ��
Position::Position() : x(0), y(0) {}
Position::Position(double _x, double _y) : x(_x), y(_y) {}

Position Position::operator+(const Position& rhs) const { return Position(x + rhs.x, y + rhs.y); }
Position Position::operator-(const Position& rhs) const { return Position(x - rhs.x, y - rhs.y); }
Position Position::operator*(double s) const { return Position(x * s, y * s); }

double Position::dot(const Position& rhs) const { return x * rhs.x + y * rhs.y; }
double Position::length() const { return sqrt(x * x + y * y); }

Position Position::normalize() const {
    double len = length();
    return len == 0 ? Position(0, 0) : Position(x / len, y / len);
}

Position Position::perpendicular() const {
    return Position(-y, x);
}

inline double cross(const Position& O, const Position& A, const Position& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

FeEGE::Polygon sortPolygonCCW(const FeEGE::Polygon& polygon_c) {
	FeEGE::Polygon polygon = polygon_c;
    // ���������С��3��ֱ�ӷ��أ�����Ҫ����
    if (polygon.size() <= 3) return polygon;

    // Step 1: ��x��������x��ͬ��y����
    std::sort(polygon.begin(), polygon.end(), 
        [](const Position& a, const Position& b) {
            return a.x < b.x || (a.x == b.x && a.y < b.y);
        });

    // Step 2: ������͹��
    Polygon hull;
    for (size_t i = 0; i < polygon.size(); ++i) {
        while (hull.size() >= 2 && 
               cross(hull[hull.size()-2], hull.back(), polygon[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(polygon[i]);
    }

    // Step 3: ������͹��
    size_t lower_size = hull.size();
    for (size_t i = polygon.size()-1; i > 0; --i) {
        while (hull.size() > lower_size && 
               cross(hull[hull.size()-2], hull.back(), polygon[i-1]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(polygon[i-1]);
    }

    // �Ƴ��ظ�����㣨������������Σ�
    if (!hull.empty()) hull.pop_back();

    return hull;
}

}

using FeEGE::Polygon;
