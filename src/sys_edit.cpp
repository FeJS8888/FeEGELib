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
			if (lParam & GCS_COMPSTR) {
                // 获取当前正在编辑的组合字符串
				int originalMode =  _setmode(_fileno(stdout), _O_U16TEXT);
                std::wstring compositionString = GetImeCompositionString(getHWnd(), GCS_COMPSTR);
                std::wcout<<compositionString<<std::endl;
				static_cast<InputBox*>(m_object)->setIMECompositionString(compositionString);
				_setmode(_fileno(stdout), originalMode);
            }

            if (lParam & GCS_RESULTSTR) {
                // 获取最终确定的结果字符串
				int originalMode =  _setmode(_fileno(stdout), _O_U16TEXT);
                std::wstring resultString = GetImeCompositionString(getHWnd(), GCS_RESULTSTR);
                std::wcout<<resultString<<std::endl;
				static_cast<InputBox*>(m_object)->setIMECompositionString(L"");
				_setmode(_fileno(stdout), originalMode);
            }
			return ((LRESULT(CALLBACK*)(HWND, UINT, WPARAM, LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
		}
		case WM_KEYDOWN:{
			switch (wParam) {
                case VK_LEFT: {
					static_cast<InputBox*>(m_object)->moveCursor(getCursorPos() - 1);
                    break;
                }
                case VK_RIGHT: {
					static_cast<InputBox*>(m_object)->moveCursor(getCursorPos() + 1);
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
			::PostMessageW(m_hwnd,WM_USER + 100 + 1,0,0);
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