#include "Base.h"
#include "Element.h"
#include <math.h>

namespace FeEGE{

bool Lpressed;
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

void reflushMouseStatu(mouse_msg msg){
	if(msg.is_left()){
		if(msg.is_down()){
			Lpressed = true;
		}
		if(msg.is_up()){
			Lpressed = false;
		} 
	}
}

}
