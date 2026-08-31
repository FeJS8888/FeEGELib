#include "sys_edit.h"
#include "FeEGELib.h"

namespace FeEGE{

namespace {
constexpr LONG MAX_IME_COMPOSITION_BYTES = 16L * 1024L * 1024L;
}
void PrintStringAsInts(const std::wstring& str) {
    if (str.empty()) {
        std::wcout << "(empty)" << std::endl;
        return;
    }
    for (wchar_t wc : str) {
        std::wcout << static_cast<wchar_t>(wc) << " ";
    }
    std::wcout << std::endl;
}

/**
 * @brief 获取窗口默认 DC 中当前字体的上行高度 (tmAscent)。
 *
 * @param hWnd 目标窗口的句柄。
 * @return 字体在当前 DC 中的上行高度（像素），失败返回 0。
 */
int GetTextAscent(HWND hWnd) {
    if (hWnd == NULL) {
        return 0;
    }

    HDC hDC = GetDC(hWnd);
    if (hDC == NULL) {
        // 无法获取设备上下文
        return 0;
    }

    TEXTMETRIC tm;
    
    // 获取当前 DC 中所选字体的 TEXTMETRIC
    BOOL bResult = GetTextMetrics(hDC, &tm); 

    // 释放 DC
    ReleaseDC(hWnd, hDC);
    
    if (bResult) {
        // tm.tmAscent 就是上行高度
        return tm.tmAscent; 
    } else {
        // GetTextMetrics 失败
        return 0;
    }
}

void sys_edit::killIME(){
	HWND hwnd = m_hwnd;
	if(!hwnd || !::IsWindow(hwnd)) return;
	InputBox* object = static_cast<InputBox*>(m_object);
	// Read the raw phonetic composition string (if any) before cancelling the IME,
	// then commit it manually at imeStartPos via commitIMEString().
	// We use CPS_CANCEL rather than CPS_COMPLETE because we returned TRUE from
	// WM_IME_STARTCOMPOSITION without forwarding to the EDIT, so the EDIT has no
	// composition insertion point; CPS_COMPLETE would insert the raw phonetic
	// string at position 0 instead of at the correct position.
	std::wstring compStr;
	HIMC hIMC = ImmGetContext(hwnd);
	if (hIMC) {
		LONG byteSize = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0);
		if (byteSize > 0 &&
			byteSize <= MAX_IME_COMPOSITION_BYTES &&
			byteSize % (LONG)sizeof(wchar_t) == 0) {
			size_t charCount = (size_t)byteSize / sizeof(wchar_t);
			std::vector<wchar_t> buf(charCount + 1, L'\0');
			LONG copiedBytes = ImmGetCompositionStringW(
				hIMC, GCS_COMPSTR, buf.data(), byteSize);
			if (copiedBytes > 0) {
				size_t copiedChars = std::min(
					charCount, (size_t)copiedBytes / sizeof(wchar_t));
				compStr.assign(buf.data(), copiedChars);
			}
		}
		// Always cancel the native composition.  The self-drawn InputBox owns
		// the insertion position, so CPS_COMPLETE could insert at the wrong one.
		ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
		ImmReleaseContext(hwnd, hIMC);
	}
	// The owner may have detached itself while IME cancellation dispatched
	// nested messages.  In that case cancel the native IME but do not call back
	// into the detached object.
	if (object && m_object == object) {
		object->commitIMEString(compStr);
	}
}

LRESULT sys_edit::onMessage(UINT message, WPARAM wParam, LPARAM lParam){
    auto forwardToNative = [this](UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
        if (!m_hwnd) return 0;
        if (!m_callback) {
            return ::DefWindowProcW(m_hwnd, msg, wp, lp);
        }
        return ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(
            m_hwnd, msg, wp, lp);
    };

    switch (message) {
	    case WM_CTLCOLOREDIT: {
	        HDC dc = (HDC)wParam;
	        if (!dc) return 0;
	        HBRUSH br = ::CreateSolidBrush(ARGBTOZBGR(m_bgcolor));
	        if (!br) return 0;
	
	        ::SetBkColor(dc, ARGBTOZBGR(m_bgcolor));
	        ::SetTextColor(dc, ARGBTOZBGR(m_color));
	        if (m_hBrush) ::DeleteObject(m_hBrush);
	        m_hBrush = br;
	        return (LRESULT)br;
	    }
	case WM_SETFOCUS:{
	        m_bInputFocus = 1;
	        m_focus = true;
			// call textbox's own message process to show caret
			auto lr = forwardToNative(message, wParam, lParam);
			// CreateCaret(m_hwnd, (HBITMAP)NULL, 1, 1);
			if(m_object) static_cast<InputBox*>(m_object)->updateIMEPosition();
	        return lr;
	    }
	    case WM_KILLFOCUS:{
	        m_bInputFocus = 0;
	        m_focus = false;
	        killIME();
	        // call textbox's own message process to hide caret
	        return forwardToNative(message, wParam, lParam);
		}
		case WM_IME_STARTCOMPOSITION:{
            // 在输入法开始组合前，若有文字选区则先删除选区内容
            // （WM_IME_STARTCOMPOSITION 返回 TRUE 后 EDIT 控件不会自行处理选区删除）
            if(!m_object) {
				return forwardToNative(message, wParam, lParam);
			}
            InputBox* p = static_cast<InputBox*>(m_object);
            p->deleteSelectedText();
            // 记录组合起点，供 killIME() 正确插入未完成组合串
            p->markIMEStart();
            SetIMEPosition(getHWnd(),InputPositionX,InputPositionY);
			return TRUE;
		}
		case WM_IME_COMPOSITION:{
			auto lr = forwardToNative(message, wParam, lParam);
			if(!m_object || !m_hwnd || !::IsWindow(m_hwnd)) return lr;
			InputBox* p = static_cast<InputBox*>(m_object);
			if (lParam & GCS_COMPSTR) {
				std::wstring compositionString = GetImeCompositionString(m_hwnd, GCS_COMPSTR);
				p->setIMECompositionString(compositionString);
            }

            if (lParam & GCS_RESULTSTR) {
				// 结果字符串由原生 EDIT 写入真实文本；这里只清除自绘制的组合层。
				p->setIMECompositionString(L"");
            }
			HIMC hIMC = ImmGetContext(m_hwnd);
			LONG cursorPos = 0;
			if(hIMC) {
				LONG rawCursorPos = ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, nullptr, 0);
				if(rawCursorPos >= 0) cursorPos = rawCursorPos;
				ImmReleaseContext(m_hwnd, hIMC);
			}
			p->setIMECursorPos(cursorPos);
			p->reflushCursorTick();
			return lr;
		}
		case WM_KEYDOWN:{
			if(!m_object) return forwardToNative(message, wParam, lParam);
			InputBox* p = static_cast<InputBox*>(m_object);
			if (!p->haveIMEString() && p->handleNativeKeyDown((unsigned int)wParam)) return 0;
			if (p->isMultiline()) {
				// 多行模式的方向键/Home/End/PageUp/PageDown 已由 InputBox
				// 按自绘制的换行布局处理；Enter 仍交给 EDIT 插入换行。
				return forwardToNative(message, wParam, lParam);
			}
			switch (wParam) {
                case VK_LEFT: {
					// 方向键不应继续维持鼠标拖动状态。
					p->cancelDrag();
					int pos = getCursorPos();
					if(pos <= 0) break;
					p->moveCursor(pos - 1);
					p->reflushCursorTick();
                    break;
                }
                case VK_RIGHT: {
					// 方向键不应继续维持鼠标拖动状态。
					p->cancelDrag();
					int pos = getCursorPos();
					if(pos < 0 || pos >= (int)p->getContent().size()) break;
					p->moveCursor(pos + 1);
					p->reflushCursorTick();
                    break;
                }
            }
			return forwardToNative(message, wParam, lParam);
		}
		case WM_CHAR:{
			if (wParam == 1 && (GetKeyState(VK_CONTROL) & 0x8000)) {
				return 0;
			}
		}
        case WM_PASTE:
        case WM_CUT:
		case WM_SETTEXT:{
			// 先让 EDIT 处理字符输入/粘贴/剪切（包括选区替换删除），
			// 再结束拖动状态，避免提前清空选区导致“只取消框选不删除文字”。
			auto lr = forwardToNative(message, wParam, lParam);
			if(m_object) static_cast<InputBox*>(m_object)->cancelDrag();
			if(m_hwnd && ::IsWindow(m_hwnd)) {
				::PostMessageW(m_hwnd, WM_USER + 100 + 1, 0, 0);
			}
			return lr;
		}
		case WM_USER + 100 + 1 :{
			if(m_object) updatecursor();
			return TRUE;
		}
		case WM_USER + 100 + 2 :{
			killIME();
			return TRUE;
		}
		case EM_SETSEL:{
			// 注意：不再异步 PostMessageW(WM_USER+100+1)，
			// 因为 EM_SETSEL 由 inv.movecursor() 触发，调用方已通过
			// moveCursor() 手动维护 cursor_pos，异步 updatecursor()
			// 会把光标重置为 EM_GETSEL 的 start（=选区锚点 dragBegin），
			// 导致拖动时光标不跟随鼠标而回到起点。
			return forwardToNative(message, wParam, lParam);
		}
        default:
	        return forwardToNative(message, wParam, lParam);
	}
}

void sys_edit::updatecursor(){
	if(!m_object || !m_hwnd || !::IsWindow(m_hwnd)) return;
	int pos = getCursorPos();
	if(pos >= 0) static_cast<InputBox*>(m_object)->moveCursor(pos);
}
}
