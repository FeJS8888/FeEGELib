#include "Base.h"
#include "Element.h"
#include <math.h>

namespace FeEGE{

LPSTR current_cursor = IDC_ARROW;
WNDPROC g_oldWndProc = nullptr;
bool needReflushCursor = true;

double InputPositionX,InputPositionY;

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
std::wstring autoToWString(const std::string& str) {
    // ���ȳ��� UTF-8
    int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);
    if (len > 0) {
        std::wstring result(len, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], len);
        result.pop_back(); // ȥ��ĩβ���ַ�
        return result;
    }

    // ��� UTF-8 ����ʧ�ܣ�����ϵͳ ANSI��ͨ��Ϊ GBK��
    len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    if (len > 0) {
        std::wstring result(len, 0);
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &result[0], len);
        result.pop_back();
        return result;
    }

    // �����ʧ�ܣ����ؿմ������׳��쳣��
    return L"(Decode Failed)";
}

void setCursor(LPSTR cursor){
    current_cursor = cursor;
}

LPSTR getCursor(){
    return current_cursor;
}

}

void SetIMEPosition(HWND hwnd, int x, int y)
{
    // ��ȡ��ǰ�������뷨������
    HIMC hIMC = ImmGetContext(hwnd);
    if (hIMC) {
        COMPOSITIONFORM cf = {0};
        cf.dwStyle = CFS_POINT;   // ʹ��λ����ʽ
        cf.ptCurrentPos.x = x;
        cf.ptCurrentPos.y = y;

        ImmSetCompositionWindow(hIMC, &cf);

        ImmReleaseContext(hwnd, hIMC);  // �ͷ����뷨������
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
 * @brief �� IME ��ȡ��ϻ����ַ�����
 *
 * @param hwnd ���ھ����
 * @param dwIndex Ҫ��ȡ���ַ������ͣ������� GCS_COMPSTR �� GCS_RESULTSTR��
 * @return �ɹ�ʱ���ػ�ȡ�����ַ�����ʧ��ʱ���ؿ��ַ�����
 */
std::wstring GetImeCompositionString(HWND hwnd, DWORD dwIndex) {
    std::wstring result;
    HIMC hIMC = ImmGetContext(hwnd);

    if (!hIMC)
        return result;

    LONG size = ImmGetCompositionStringW(hIMC, dwIndex, NULL, 0);
    if (size > 0) {
        int wcharCount = size / sizeof(wchar_t);

        // ��ȫ�ض�ȡ���ݣ����� resize ���������ݣ�
        std::vector<wchar_t> buffer(wcharCount + 1, L'\0');  // +1 ���տ�λ
        ImmGetCompositionStringW(hIMC, dwIndex, buffer.data(), size);

        result.assign(buffer.begin(), buffer.begin() + wcharCount);
    }

    ImmReleaseContext(hwnd, hIMC);
    return result;
}
