#include "Base.h"
#include <math.h>

namespace FeEGE{
	
typedef std::vector<Position> Polygon;  ///< 多边形类型定义，由顶点向量组成
	
// Position 实现
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

}

using FeEGE::Polygon;
