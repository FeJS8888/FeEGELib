#include "Collision.h"
#include <cmath>
#include <limits>
#include <algorithm>

namespace FeEGE{
	
static PenetrationInfo last_info;

// Vector2 实现
Vector2::Vector2() : x(0), y(0) {}
Vector2::Vector2(double _x, double _y) : x(_x), y(_y) {}

Vector2 Vector2::operator+(const Vector2& rhs) const { return Vector2(x + rhs.x, y + rhs.y); }
Vector2 Vector2::operator-(const Vector2& rhs) const { return Vector2(x - rhs.x, y - rhs.y); }
Vector2 Vector2::operator*(double s) const { return Vector2(x * s, y * s); }

double Vector2::dot(const Vector2& rhs) const { return x * rhs.x + y * rhs.y; }
double Vector2::length() const { return std::sqrt(x * x + y * y); }

Vector2 Vector2::normalize() const {
    double len = length();
    return len == 0 ? Vector2(0, 0) : Vector2(x / len, y / len);
}

Vector2 Vector2::perpendicular() const {
    return Vector2(-y, x);
}

// Support 函数
static Vector2 support(const std::vector<Vector2>& shapeA, const std::vector<Vector2>& shapeB, const Vector2& dir) {
    auto getFarthestPoint = [](const std::vector<Vector2>& shape, const Vector2& dir) {
        double maxDot = -std::numeric_limits<double>::infinity();
        Vector2 bestPoint;
        for (const auto& p : shape) {
            double d = p.dot(dir);
            if (d > maxDot) {
                maxDot = d;
                bestPoint = p;
            }
        }
        return bestPoint;
    };

    Vector2 p1 = getFarthestPoint(shapeA, dir);
    Vector2 p2 = getFarthestPoint(shapeB, dir * -1);
    return p1 - p2;
}

// GJK simplex 更新逻辑
static bool doSimplex(std::vector<Vector2>& simplex, Vector2& dir) {
    if (simplex.size() == 2) {
        Vector2 B = simplex[0];
        Vector2 A = simplex[1];
        Vector2 AB = B - A;
        Vector2 AO = A * -1;
        dir = Vector2(AB.y, -AB.x);
        if (dir.dot(AO) < 0) dir = dir * -1;
    } else if (simplex.size() == 3) {
        Vector2 C = simplex[0];
        Vector2 B = simplex[1];
        Vector2 A = simplex[2];
        Vector2 AB = B - A;
        Vector2 AC = C - A;
        Vector2 AO = A * -1;

        Vector2 ABperp = Vector2(AB.y, -AB.x);
        if (ABperp.dot(AC) > 0) ABperp = ABperp * -1;

        if (ABperp.dot(AO) > 0) {
            simplex = { A, B };
            dir = ABperp;
            return false;
        }

        Vector2 ACperp = Vector2(-AC.y, AC.x);
        if (ACperp.dot(AB) > 0) ACperp = ACperp * -1;

        if (ACperp.dot(AO) > 0) {
            simplex = { A, C };
            dir = ACperp;
            return false;
        }

        return true;
    }
    return false;
}

// GJK 主体
static bool gjk(const std::vector<Vector2>& shapeA, const std::vector<Vector2>& shapeB, std::vector<Vector2>& simplex) {
    Vector2 dir(1, 0);
    simplex.push_back(support(shapeA, shapeB, dir));
    dir = simplex[0] * -1;

    while (true) {
        Vector2 A = support(shapeA, shapeB, dir);
        if (A.dot(dir) <= 0)
            return false;

        simplex.push_back(A);
        if (doSimplex(simplex, dir))
            return true;
    }
}

// EPA 辅助结构
class Edge {
public:
    Vector2 normal;
    double distance;
    size_t index;
};

static Edge findClosestEdge(const std::vector<Vector2>& polytope) {
    Edge closest;
    closest.distance = std::numeric_limits<double>::infinity();

    for (size_t i = 0; i < polytope.size(); ++i) {
        size_t j = (i + 1) % polytope.size();
        Vector2 A = polytope[i];
        Vector2 B = polytope[j];
        Vector2 E = B - A;
        Vector2 normal = Vector2(E.y, -E.x).normalize();
        double dist = normal.dot(A);

        if (dist < closest.distance) {
            closest = { normal, dist, j };
        }
    }
    return closest;
}

// EPA 主体
static PenetrationInfo epa(const std::vector<Vector2>& shapeA, const std::vector<Vector2>& shapeB, std::vector<Vector2>& simplex) {
    const double tolerance = 0.0001f;
    while (true) {
        Edge edge = findClosestEdge(simplex);
        Vector2 p = support(shapeA, shapeB, edge.normal);
        double d = p.dot(edge.normal);
        double diff = d - edge.distance;

        if (diff < tolerance) {
            return { edge.normal, d };
        } else {
            simplex.insert(simplex.begin() + edge.index, p);
        }
    }
}

// 外部接口：是否相交
bool isTouched(const std::vector<Vector2>& shapeA, const std::vector<Vector2>& shapeB) {
    std::vector<Vector2> simplex;
    if (gjk(shapeA, shapeB, simplex)) {
        last_info = epa(shapeA, shapeB, simplex);
        last_info.direction = Vector2{0,0} - last_info.direction;
        return true;
    }
    return false;
}

// 外部接口：返回最近一次碰撞信息
const PenetrationInfo& getLastInfo() {
    return last_info;
}

// 外部接口：返回沿方向dir上的穿透深度（假设shapeA和shapeB已经相交）
double getSeparateDistance(const std::vector<Vector2>& shapeA,const std::vector<Vector2>& shapeB,const Vector2& direction) {
    Vector2 supportPt = support(shapeA, shapeB,direction); // 在反方向上找 support 点
    return supportPt.dot(direction); // 距离 = support 点到原点的投影长度（取负）
}

}
