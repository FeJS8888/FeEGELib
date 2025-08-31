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
	        return ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
		}
	    case WM_KILLFOCUS:{
	        m_bInputFocus = 0;
	        m_focus = false;
	        // call textbox's own message process to hide caret
	        return ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
		}
	    case WM_IME_STARTCOMPOSITION:{
            SetIMEPosition(getHWnd(),InputPositionX,InputPositionY);
			return TRUE;
		}
		case WM_IME_COMPOSITION:{
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
			return ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
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
		case EM_SETSEL:{
			LRESULT r = ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
			::PostMessageW(m_hwnd,WM_USER + 100 + 1,0,0);
			return r;
		}
        default:
	        return ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
	}
}

void sys_edit::updatecursor(){
	static_cast<InputBox*>(m_object)->moveCursor(getCursorPos());
}
}