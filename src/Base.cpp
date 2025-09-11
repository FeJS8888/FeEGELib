#include "Base.h"
#include "Element.h"
#include <math.h>

namespace FeEGE{

LPSTR current_cursor = IDC_ARROW;
WNDPROC g_oldWndProc = nullptr;
FontManager fontManager;
bool needReflushCursor = true;

double InputPositionX,InputPositionY;

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

void setCursor(LPSTR cursor){
    current_cursor = cursor;
}

LPSTR getCursor(){
    return current_cursor;
}

int fixed(double x){
    return static_cast<int>(x + 0.5);
}

}

void SetIMEPosition(HWND hwnd, int x, int y)
{
    // 获取当前窗口输入法上下文
    HIMC hIMC = ImmGetContext(hwnd);
    if (hIMC) {
        COMPOSITIONFORM cf = {0};
        cf.dwStyle = CFS_POINT;   // 使用位置样式
        cf.ptCurrentPos.x = x;
        cf.ptCurrentPos.y = y;

        ImmSetCompositionWindow(hIMC, &cf);

        ImmReleaseContext(hwnd, hIMC);  // 释放输入法上下文
    }
}

LRESULT CALLBACK FeEGEProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if(message == WM_SETCURSOR){
        SetCursor(LoadCursor(NULL, current_cursor));
        return TRUE;
    }
    if(message == WM_IME_STARTCOMPOSITION){
        cout<<InputPositionX<<" "<<InputPositionY<<"\n";
        SetIMEPosition(getHWnd(),InputPositionX,InputPositionY);
        return TRUE;
    }
    return CallWindowProcW(g_oldWndProc, hWnd, message, wParam, lParam);
}

/**
 * @brief 从 IME 获取组合或结果字符串。
 *
 * @param hwnd 窗口句柄。
 * @param dwIndex 要获取的字符串类型，可以是 GCS_COMPSTR 或 GCS_RESULTSTR。
 * @return 成功时返回获取到的字符串，失败时返回空字符串。
 */
std::wstring GetImeCompositionString(HWND hwnd, DWORD dwIndex) {
    std::wstring result;
    HIMC hIMC = ImmGetContext(hwnd);

    if (!hIMC)
        return result;

    LONG size = ImmGetCompositionStringW(hIMC, dwIndex, NULL, 0);
    if (size > 0) {
        int wcharCount = size / sizeof(wchar_t);

        // 安全地读取数据（避免 resize 留下脏数据）
        std::vector<wchar_t> buffer(wcharCount + 1, L'\0');  // +1 保险空位
        ImmGetCompositionStringW(hIMC, dwIndex, buffer.data(), size);

        result.assign(buffer.begin(), buffer.begin() + wcharCount);
    }

    ImmReleaseContext(hwnd, hIMC);
    return result;
}
