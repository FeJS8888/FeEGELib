#include "Collision.h"
#include "Base.h"
#include "json.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <algorithm>
#include <string>
#include <fstream>

using nlohmann::json;

namespace FeEGE {

static PenetrationInfo last_info;

// 安全比较浮点数是否接近零
static bool isZero(double val) {
    return std::abs(val) < std::numeric_limits<double>::epsilon();
}

// 安全获取符号函数
static int sgnSafe(double val) {
    if (isZero(val)) {
        return 0;
    }
    return (val > 0) ? 1 : -1;
}

Position transformPoint(const Position& p, const Position& origin, double angle, double scale) {
    // 1. 平移到原点
    Position translated = p - origin;

    // 2. 缩放
    translated = translated * scale;

    // 3. 旋转
    double cosA = std::cos(angle);
    double sinA = std::sin(angle);
    Position rotated(
        translated.x * cosA - translated.y * sinA,
        translated.x * sinA + translated.y * cosA
    );

    // 4. 平移回去
    return (rotated + origin);
}

std::vector<Position> transformShape(const std::vector<Position>& shape, double scale, const Position& rotationOrigin, double rotationAngle) {
    std::vector<Position> result;
    for (const auto& p : shape) {
        result.push_back(transformPoint(p, rotationOrigin, rotationAngle, scale));
    }
    return result;
}

std::vector<Position> transformPosition(const std::vector<Position>& shape, const Position& origin, const Position& pos) {
    std::vector<Position> result;
    for (const auto& p : shape) {
        result.push_back(Position{p.x + pos.x - origin.x, p.y + pos.y - origin.y});
    }
    return result;
}

// Support 函数
static Position support(const std::vector<Position>& shapeA, const std::vector<Position>& shapeB, const Position& dir) {
    auto getFarthestPoint = [](const std::vector<Position>& shape, const Position& dir) {
        double maxDot = -std::numeric_limits<double>::infinity();
        Position bestPoint;
        for (const auto& p : shape) {
            double d = p.dot(dir);
            if (d > maxDot) {
                maxDot = d;
                bestPoint = p;
            }
        }
        return bestPoint;
    };

    Position p1 = getFarthestPoint(shapeA, dir);
    Position p2 = getFarthestPoint(shapeB, dir * -1);
    return p1 - p2;
}

// GJK simplex 更新逻辑
static bool doSimplex(std::vector<Position>& simplex, Position& dir) {
    if (simplex.size() == 2) {
        Position B = simplex[0];
        Position A = simplex[1];
        Position AB = B - A;
        Position AO = A * -1;
        dir = Position(AB.y, -AB.x);
        if (sgnSafe(dir.dot(AO)) < 0) dir = dir * -1;
    } else if (simplex.size() == 3) {
        Position C = simplex[0];
        Position B = simplex[1];
        Position A = simplex[2];
        Position AB = B - A;
        Position AC = C - A;
        Position AO = A * -1;

        Position ABperp = Position(AB.y, -AB.x);
        if (sgnSafe(ABperp.dot(AC)) > 0) ABperp = ABperp * -1;

        if (sgnSafe(ABperp.dot(AO)) > 0) {
            simplex = {A, B};
            dir = ABperp;
            return false;
        }

        Position ACperp = Position(-AC.y, AC.x);
        if (sgnSafe(ACperp.dot(AB)) > 0) ACperp = ACperp * -1;

        if (sgnSafe(ACperp.dot(AO)) > 0) {
            simplex = {A, C};
            dir = ACperp;
            return false;
        }

        return true;
    }
    return false;
}

// GJK 主体
static bool gjk(const std::vector<Position>& shapeA, const std::vector<Position>& shapeB, std::vector<Position>& simplex) {
    Position dir(1, 0);
    simplex.push_back(support(shapeA, shapeB, dir));
    dir = simplex[0] * -1;

    while (true) {
        Position A = support(shapeA, shapeB, dir);
        if (sgnSafe(A.dot(dir)) <= 0)
            return false;

        simplex.push_back(A);
        if (doSimplex(simplex, dir))
            return true;
    }
}

// EPA 辅助结构
class Edge {
public:
    Position normal;
    double distance;
    size_t index;
};

static Edge findClosestEdge(const std::vector<Position>& polytope) {
    Edge closest;
    closest.distance = std::numeric_limits<double>::infinity();

    for (size_t i = 0; i < polytope.size(); ++i) {
        size_t j = (i + 1) % polytope.size();
        Position A = polytope[i];
        Position B = polytope[j];
        Position E = B - A;
        Position normal = Position(E.y, -E.x).normalize();
        double dist = normal.dot(A);

        if (dist < closest.distance) {
            closest = {normal, dist, j};
        }
    }
    return closest;
}

// EPA 主体
static PenetrationInfo epa(const std::vector<Position>& shapeA, const std::vector<Position>& shapeB, std::vector<Position>& simplex) {
    const double tolerance = 0.0001f;
    while (true) {
        Edge edge = findClosestEdge(simplex);
        Position p = support(shapeA, shapeB, edge.normal);
        double d = p.dot(edge.normal);
        double diff = d - edge.distance;

        if (diff < tolerance) {
            PenetrationInfo info;
            info.direction = edge.normal;
            info.depth = d;
            return info;
        } else {
            simplex.insert(simplex.begin() + edge.index, p);
        }
    }
}

// 外部接口：是否相交
bool isTouched(const std::vector<Position>& shapeA, const std::vector<Position>& shapeB) {
    std::vector<Position> simplex;
    if (gjk(shapeA, shapeB, simplex)) {
        last_info = epa(shapeA, shapeB, simplex);
        last_info.direction = Position{0, 0} - last_info.direction;
        return true;
    }
    return false;
}

// 外部接口：返回最近一次碰撞信息
const PenetrationInfo& getLastInfo() {
    return last_info;
}

// 外部接口：返回沿方向dir上的穿透深度（假设shapeA和shapeB已经相交）
double getSeparateDistance(const std::vector<Position>& shapeA, const std::vector<Position>& shapeB, const Position& direction) {
    Position supportPt = support(shapeA, shapeB, direction); // 在反方向上找 support 点
    return supportPt.dot(direction);                             // 距离 = support 点到原点的投影长度（取负）
}

// 外部接口：返回点是否在多边形内
bool isPointInConvexPolygon(const std::vector<Position>& polygon, const Position& point) {
    int n = polygon.size();
    bool hasPositive = false;
    bool hasNegative = false;

    for (int i = 0; i < n; ++i) {
        Position A = polygon[i];
        Position B = polygon[(i + 1) % n];
        Position AB = B - A;
        Position AP = point - A;
        double cross = AB.x * AP.y - AB.y * AP.x;
        if (sgnSafe(cross) > 0) hasPositive = true;
        if (sgnSafe(cross) < 0) hasNegative = true;
        if (hasPositive && hasNegative) return false; // 不在同侧
    }
    return true;
}

// 外部接口：从 JSON 读取多边形
std::vector<Position> readPolygonFromJSON(const std::string& file, const std::string& id) {
    std::ifstream in(file);
    json dat = json::parse(in);
    std::vector<Position> res;
    for (auto vec : dat["FeEGE"]["point"][id]) {
        res.push_back(Position{vec[0], vec[1]});
    }
    in.close();
    return res;
}

} // namespace FeEGE
