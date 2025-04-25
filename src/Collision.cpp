#include "Collision.h"
#include "base.h"
#include <cmath>
#include<iostream>
#include <limits>
#include <algorithm>

namespace FeEGE{
	
static PenetrationInfo last_info;

Position transformPoint(const Position& p, const Position& origin, double angle, double scale) {
    // 1. ƽ�Ƶ�ԭ��
    Position translated = p - origin;

    // 2. ����
    translated = translated * scale;

    // 3. ��ת
    double cosA = std::cos(angle);
    double sinA = std::sin(angle);
    Position rotated(
        translated.x * cosA - translated.y * sinA,
        translated.x * sinA + translated.y * cosA
    );

    // 4. ƽ�ƻ�ȥ
    return (rotated + origin);
}

std::vector<Position> transformShape(const std::vector<Position>& shape,double scale,const Position& rotationOrigin,double rotationAngle){
    std::vector<Position> result;
    for (const auto& p : shape) {
        result.push_back(transformPoint(p, rotationOrigin, rotationAngle, scale));
    }
    return result;
}

std::vector<Position> transformPosition(const std::vector<Position>& shape,const Position& origin,const Position& pos){
	std::vector<Position> result;
	for(const auto& p : shape){
		result.push_back(Position{p.x + pos.x - origin.x,p.y + pos.y - origin.y});
	}
	return result;
} 

// Support ����
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

// GJK simplex �����߼�
static bool doSimplex(std::vector<Position>& simplex, Position& dir) {
    if (simplex.size() == 2) {
        Position B = simplex[0];
        Position A = simplex[1];
        Position AB = B - A;
        Position AO = A * -1;
        dir = Position(AB.y, -AB.x);
        if (dir.dot(AO) < 0) dir = dir * -1;
    } else if (simplex.size() == 3) {
        Position C = simplex[0];
        Position B = simplex[1];
        Position A = simplex[2];
        Position AB = B - A;
        Position AC = C - A;
        Position AO = A * -1;

        Position ABperp = Position(AB.y, -AB.x);
        if (ABperp.dot(AC) > 0) ABperp = ABperp * -1;

        if (ABperp.dot(AO) > 0) {
            simplex = { A, B };
            dir = ABperp;
            return false;
        }

        Position ACperp = Position(-AC.y, AC.x);
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

// GJK ����
static bool gjk(const std::vector<Position>& shapeA, const std::vector<Position>& shapeB, std::vector<Position>& simplex) {
    Position dir(1, 0);
    simplex.push_back(support(shapeA, shapeB, dir));
    dir = simplex[0] * -1;

    while (true) {
        Position A = support(shapeA, shapeB, dir);
        if (A.dot(dir) <= 0)
            return false;

        simplex.push_back(A);
        if (doSimplex(simplex, dir))
            return true;
    }
}

// EPA �����ṹ
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
            closest = { normal, dist, j };
        }
    }
    return closest;
}

// EPA ����
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

// �ⲿ�ӿڣ��Ƿ��ཻ
bool isTouched(const std::vector<Position>& shapeA, const std::vector<Position>& shapeB) {
    std::vector<Position> simplex;
    if (gjk(shapeA, shapeB, simplex)) {
        last_info = epa(shapeA, shapeB, simplex);
        last_info.direction = Position{0,0} - last_info.direction;
        return true;
    }
    return false;
}

// �ⲿ�ӿڣ��������һ����ײ��Ϣ
const PenetrationInfo& getLastInfo() {
    return last_info;
}

// �ⲿ�ӿڣ������ط���dir�ϵĴ�͸��ȣ�����shapeA��shapeB�Ѿ��ཻ��
double getSeparateDistance(const std::vector<Position>& shapeA,const std::vector<Position>& shapeB,const Position& direction) {
    Position supportPt = support(shapeA, shapeB,direction); // �ڷ��������� support ��
    return supportPt.dot(direction); // ���� = support �㵽ԭ���ͶӰ���ȣ�ȡ����
}

// �ⲿ�ӿڣ����ص��Ƿ��ڶ������ 
bool isPointInConvexPolygon(const std::vector<Position>& polygon, const Position& point) {
    int n = polygon.size();
    bool hasPositive = false;
    bool hasNegative = false;

    for (int i = 0; i < n; ++i) {
        Position A = polygon[i];
        std::cout<<A.x<<" "<<A.y<<"\n";
        Position B = polygon[(i + 1) % n];
        Position AB = B - A;
        Position AP = point - A;
        double cross = AB.x * AP.y - AB.y * AP.x;
        if (cross > 0) hasPositive = true;
        if (cross < 0) hasNegative = true;
        if (hasPositive && hasNegative) return false; // ����ͬ��
    }
    return true;
}

}
