#include "sys_edit.h"
#include "FeEGELib.h"

namespace FeEGE{
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
	// Read the raw phonetic composition string (if any) before cancelling the IME,
	// then commit it manually at imeStartPos via commitIMEString().
	// We use CPS_CANCEL rather than CPS_COMPLETE because we returned TRUE from
	// WM_IME_STARTCOMPOSITION without forwarding to the EDIT, so the EDIT has no
	// composition insertion point; CPS_COMPLETE would insert the raw phonetic
	// string at position 0 instead of at the correct position.
	std::wstring compStr;
	HIMC hIMC = ImmGetContext(m_hwnd);
	if (hIMC) {
		LONG size = ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0);
		if (size > 0) {
			LONG charCount = size / sizeof(wchar_t);
			std::vector<wchar_t> buf(charCount + 1, 0);
			ImmGetCompositionStringW(hIMC, GCS_COMPSTR, buf.data(), size);
			compStr.assign(buf.data(), charCount);
			ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
		}
		ImmReleaseContext(m_hwnd, hIMC);
	}
	static_cast<InputBox*>(m_object)->commitIMEString(compStr);
}

LRESULT sys_edit::onMessage(UINT message, WPARAM wParam, LPARAM lParam){
    switch (message) {
	    case WM_CTLCOLOREDIT: {
	        HDC dc = (HDC)wParam;
	        HBRUSH br = ::CreateSolidBrush(ARGBTOZBGR(m_bgcolor));
	
	        ::SetBkColor(dc, ARGBTOZBGR(m_bgcolor));
	        ::SetTextColor(dc, ARGBTOZBGR(m_color));
	        ::DeleteObject(m_hBrush);
	        m_hBrush = br;
	        return (LRESULT)br;
	    }
	    case WM_SETFOCUS:{
	        m_bInputFocus = 1;
	        m_focus = true;
			// call textbox's own message process to show caret
			auto lr = ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
			CreateCaret(m_hwnd, (HBITMAP)NULL, 1, 1);
			InputBox* p = static_cast<InputBox*>(m_object);
			p->updateIMEPosition();
	        return lr;
		}
	    case WM_KILLFOCUS:{
	        m_bInputFocus = 0;
	        m_focus = false;
			killIME();
	        // call textbox's own message process to hide caret
	        return ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
		}
	    case WM_IME_STARTCOMPOSITION:{
            // 在输入法开始组合前，若有文字选区则先删除选区内容
            // （WM_IME_STARTCOMPOSITION 返回 TRUE 后 EDIT 控件不会自行处理选区删除）
            InputBox* p = static_cast<InputBox*>(m_object);
            p->deleteSelectedText();
            // 记录组合起点，供 killIME() 正确插入未完成组合串
            p->markIMEStart();
            SetIMEPosition(getHWnd(),InputPositionX,InputPositionY);
			return TRUE;
		}
		case WM_IME_COMPOSITION:{
			auto lr = ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
			InputBox* p = static_cast<InputBox*>(m_object);
			if (lParam & GCS_COMPSTR) {
                std::wstring compositionString = GetImeCompositionString(getHWnd(), GCS_COMPSTR);
				p->setIMECompositionString(compositionString);
            }

            if (lParam & GCS_RESULTSTR) {
                std::wstring resultString = GetImeCompositionString(getHWnd(), GCS_RESULTSTR);
				p->setIMECompositionString(L"");
            }
			HIMC hIMC = ImmGetContext(getHWnd());
			LONG cursorPos = ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, nullptr, 0);
			ImmReleaseContext(getHWnd(), hIMC);
			p->setIMECursorPos(cursorPos);
			p->reflushCursorTick();
			return lr;
		}
		case WM_KEYDOWN:{
			switch (wParam) {
                case VK_LEFT: {
					InputBox* p = static_cast<InputBox*>(m_object);
					int pos = getCursorPos();
					if(pos <= 0) break;
					p->moveCursor(pos - 1);
					p->reflushCursorTick();
                    break;
                }
                case VK_RIGHT: {
					InputBox* p = static_cast<InputBox*>(m_object);
					int pos = getCursorPos();
					if(pos >= p->getContent().size()) break;
					p->moveCursor(pos + 1);
					p->reflushCursorTick();
                    break;
                }
            }
			return ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
		}
		case WM_CHAR:
        case WM_PASTE:
        case WM_CUT:
        case WM_SETTEXT:{
			::PostMessageW(m_hwnd,WM_USER + 100 + 1,0,0);
			return ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
		}
		case WM_USER + 100 + 1 :{
			updatecursor();
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
			return ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
		}
        default:
	        return ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
	}
}

void sys_edit::updatecursor(){
	static_cast<InputBox*>(m_object)->moveCursor(getCursorPos());
}
}