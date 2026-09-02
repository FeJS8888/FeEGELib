#include "FeEGELib.h"

using namespace FeEGE;

class InputBoxProbe : public InputBox {
public:
    InputBoxProbe() : InputBox(100, 80, 160, 100, 4, true) {}

    HWND nativeHandle() const { return inv.m_hwnd; }

    void setNativeCaret(int position) {
        inv.movecursor(position, position);
        moveCursor(position);
    }

    void flushNativeState() { flushPendingNativeEditState(); }

    std::wstring nativeContent() {
        const int length = inv.gettextlength();
        std::vector<wchar_t> buffer((size_t)length + 1, L'\0');
        inv.gettext((int)buffer.size(), buffer.data());
        return std::wstring(buffer.data(), (size_t)length);
    }
};

int main() {
    init(240, 180);

    int result = 0;
    {
        InputBoxProbe input;
        input.setMaxlen(-1);
        input.setContent(L"first\r\nsecond");
        input.setNativeCaret(5);

        ::SendMessageW(input.nativeHandle(), WM_CHAR, L'X', 0);
        input.flushNativeState();

        if(input.nativeContent() != L"firstX\r\nsecond") result |= 1;
        if(input.getContent() != L"firstX\r\nsecond") result |= 2;

        const std::wstring manyLines = L"1\r\n2\r\n3\r\n4\r\n5\r\n6\r\n7\r\n8\r\n9\r\n10";
        input.setContent(manyLines);
        input.setNativeCaret(1);
        ::SendMessageW(input.nativeHandle(), WM_CHAR, L'A', 0);
        input.flushNativeState();
        if(input.getContent() != L"1A\r\n2\r\n3\r\n4\r\n5\r\n6\r\n7\r\n8\r\n9\r\n10") result |= 4;

        input.setNativeCaret((int)input.getContent().size());
        ::SendMessageW(input.nativeHandle(), WM_CHAR, L'Z', 0);
        input.flushNativeState();
        if(input.getContent() != L"1A\r\n2\r\n3\r\n4\r\n5\r\n6\r\n7\r\n8\r\n9\r\n10Z") result |= 8;
    }

    closegraph();
    return result;
}
