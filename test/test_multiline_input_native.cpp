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

    void flushNativeState() {
        ::SendMessageW(inv.m_hwnd, WM_USER + 100 + 1, 0, 0);
    }

    void prepareLayout() { ensureTextLayout(); }

    int visualLineCount() const { return (int)textLines.size(); }

    int visualLineEnd(int line) const { return textLines.at((size_t)line).end; }

    int caretPosition() const { return cursor_pos; }

    int caretVisualLine() const {
        return lineIndexForCaretDisplayPosition(displayPositionForContentPosition(cursor_pos));
    }

    int selectionAnchor() const { return dragBegin; }

    int selectionCaret() const { return dragEnd; }

    void setVisualCaretAtLineEnd(int line) {
        const int position = visualLineEnd(line);
        dragBegin = dragEnd = cursor_pos = position;
        inv.movecursor(position, position);
        setCaretVisualLineHint(line, displayPositionForContentPosition(position));
        verticalNavigationXValid = false;
    }

    bool moveVertically(int direction, bool extendSelection = false) {
        if(!enqueueVerticalNavigationFromNativeEdit(direction, extendSelection)) return false;
        processPendingVerticalNavigation();
        return true;
    }

    bool hasPendingVerticalMove() const { return hasPendingVerticalNavigation(); }

    void advanceLayoutAndNavigation() {
        ensureTextLayout();
        processPendingVerticalNavigation();
    }

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

        input.setContent(
            L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
            L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
        input.prepareLayout();
        if(input.visualLineCount() < 4) {
            result |= 16;
        }
        else {
            input.setVisualCaretAtLineEnd(0);
            input.moveVertically(1);
            if(input.caretPosition() != input.visualLineEnd(1) ||
               input.caretVisualLine() != 1) {
                result |= 32;
            }

            input.moveVertically(1);
            if(input.caretPosition() != input.visualLineEnd(2) ||
               input.caretVisualLine() != 2) {
                result |= 64;
            }

            input.setVisualCaretAtLineEnd(2);
            const int anchor = input.caretPosition();
            input.moveVertically(-1, true);
            input.moveVertically(-1, true);
            if(input.selectionAnchor() != anchor ||
               input.selectionCaret() != input.visualLineEnd(0) ||
               input.caretVisualLine() != 0) {
                result |= 128;
            }

            input.moveVertically(1, true);
            if(input.selectionAnchor() != anchor ||
               input.selectionCaret() != input.visualLineEnd(1) ||
               input.caretVisualLine() != 1) {
                result |= 256;
            }
        }

        std::wstring longParagraph(12000, L'x');
        input.setContent(longParagraph);
        input.setNativeCaret((int)longParagraph.size());
        input.moveVertically(-1);
        if(!input.hasPendingVerticalMove()) result |= 512;
        for(int frame = 0; frame < 256 && input.hasPendingVerticalMove(); ++frame) {
            input.advanceLayoutAndNavigation();
        }
        if(input.hasPendingVerticalMove() ||
           input.caretPosition() >= (int)longParagraph.size()) {
            result |= 1024;
        }

        input.setContent(
            L"First wrapped line is intentionally long enough to create several visual rows. "
            L"Second wrapped line keeps the renderer measuring while native key messages arrive. "
            L"Third wrapped line repeats the same workload for the cross-thread regression.");
        input.prepareLayout();
        if(input.visualLineCount() >= 3) {
            input.setVisualCaretAtLineEnd(1);
            PIMAGE renderTarget = newimage(240, 180);
            std::atomic<bool> senderDone{false};
            std::thread sender([&]() {
                for(int index = 0; index < 2000; ++index) {
                    ::SendMessageW(input.nativeHandle(), WM_KEYDOWN,
                        index % 2 == 0 ? VK_DOWN : VK_UP, 0);
                }
                senderDone.store(true, std::memory_order_release);
            });
            while(!senderDone.load(std::memory_order_acquire) ||
                  input.hasPendingVerticalMove()) {
                input.draw(renderTarget, 100, 80);
            }
            sender.join();
            input.draw(renderTarget, 100, 80);
            delimage(renderTarget);
        }
        else {
            result |= 2048;
        }
    }

    closegraph();
    return result;
}
