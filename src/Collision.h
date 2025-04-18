#pragma once
#include <vector>

namespace FeEGE{
	
class Vector2 {
public:
    double x, y;
    Vector2();
    Vector2(double _x, double _y);

    Vector2 operator+(const Vector2& rhs) const;
    Vector2 operator-(const Vector2& rhs) const;
    Vector2 operator*(double s) const;

    double dot(const Vector2& rhs) const;
    double length() const;
    Vector2 normalize() const;
    Vector2 perpendicular() const;
};

class PenetrationInfo {
public:
    Vector2 direction;
    double depth;
};

typedef std::vector<Vector2> Polygon;
// 主函数调用接口
bool isTouched(const std::vector<Vector2>& shapeA, const std::vector<Vector2>& shapeB);
const PenetrationInfo& getLastInfo();
double getSeparateDistance(const std::vector<Vector2>& shapeA,const std::vector<Vector2>& shapeB,const Vector2& direction); 

}
