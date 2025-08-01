#include "Base.h"
#include "Element.h"
#include <math.h>

namespace FeEGE{

bool Lpressed;
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

// 自动判断编码并调用 EGE 输出
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
std::wstring autoToWString(const std::string& str) {
    // 优先尝试 UTF-8
    int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);
    if (len > 0) {
        std::wstring result(len, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], len);
        result.pop_back(); // 去掉末尾空字符
        return result;
    }

    // 如果 UTF-8 解码失败，尝试系统 ANSI（通常为 GBK）
    len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    if (len > 0) {
        std::wstring result(len, 0);
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &result[0], len);
        result.pop_back();
        return result;
    }

    // 如果都失败，返回空串（或抛出异常）
    return L"(Decode Failed)";
}

}
