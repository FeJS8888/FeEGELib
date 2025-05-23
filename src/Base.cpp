#include "Base.h"
#include "FeEGELib.h"
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

// �Զ��жϱ��벢���� EGE ���
void outtextxy_auto(int x, int y, const std::string& str) {
    std::string text = str;

    if (detectEncoding(text) == "UTF-8") {
        text = UTF8ToANSI(text);
    }

    outtextxy(x, y, text.c_str());
}

}
