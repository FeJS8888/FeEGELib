/**
 * @file sys_edit.h
 * @brief 系统编辑框控件类定义
 * @author FeJS8888
 * @version 2.10.0.0
 * @date 2025-10-26
 * 
 * 本文件定义了sys_edit类，封装了Windows原生编辑框控件
 * 提供了文本输入、光标控制、焦点管理等功能
 */

#ifndef SYS_EDIT_H
#define SYS_EDIT_H

#include <atomic>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include <ege/egecontrolbase.h>
#include "Base.h"

/// 将ARGB颜色转换为ZBGR格式
#define ARGBTOZBGR(c) ((color_t)((((c) & 0xFF) << 16) | (((c) & 0xFF0000) >> 16) | ((c) & 0xFF00)))

/// 将多字节字符串转换为宽字符串的宏
#define EGE_CONVERT_TO_WSTR_WITH(mbStr, block)                                               \
    {                                                                                        \
        int    bufsize = ::MultiByteToWideChar(::ege::getcodepage(), 0, mbStr, -1, NULL, 0); \
        WCHAR* wStr    = new WCHAR[bufsize];                                                 \
        ::MultiByteToWideChar(::ege::getcodepage(), 0, mbStr, -1, &wStr[0], bufsize);        \
        block delete wStr;                                                                   \
    }

namespace FeEGE
{

/**
 * @class sys_edit
 * @brief 系统编辑框控件类
 * @details 封装了Windows原生EDIT控件，提供文本输入功能
 *          继承自egeControlBase，支持与EGE图形系统集成
 */
class sys_edit : public egeControlBase
{
public:
    /**
     * @brief 原生 EDIT 在 UI 线程采集到的输入事件。
     *
     * sys_edit 的窗口过程运行在 xEGE 的 UI 消息线程，而 InputBox 的
     * 状态和绘制运行在应用线程。窗口过程只能向此队列写入数据，不能直接
     * 调用 InputBox，避免跨线程同时读写 std::wstring/布局缓存。
     */
    struct PendingInputEvent {
        enum class Type {
            KeyDown,
            TextInput,
            Copy,
            Cut,
            DeleteSelection,
            ImeStart,
            ImeUpdate,
            ImeResult,
            ImeCommit,
            ImeEnd,
            FocusLost,
        };

        Type type = Type::TextInput;
        unsigned int key = 0;
        bool shift = false;
        bool ctrl = false;
        std::wstring text;
        int imeCursorPos = 0;
    };

    CTL_PREINIT(sys_edit, egeControlBase)
    {
        // do sth. before sub objects' construct function call
    }

    CTL_PREINITEND;

    /**
     * @brief 构造函数
     * @param CTL_DEFPARAM 控件默认参数
     */
    sys_edit(CTL_DEFPARAM) : CTL_INITBASE(egeControlBase)
    {
        CTL_INIT; // must be the first linef
        directdraw(true);
        m_hwnd = NULL;
        m_hFont = NULL;
        m_hBrush = NULL;
        m_color = 0;
        m_bgcolor = 0xFFFFFF;
        m_callback = 0;
        m_focus = false;
        m_multiline = false;
        m_imeComposing = false;
        m_imeCancelling = false;
        m_suppressedBackspaceChars = 0;
        m_suppressedReturnChars = 0;
        m_destroying.store(false, std::memory_order_relaxed);
    }

    /**
     * @brief 析构函数
     */
    ~sys_edit() { destroy(); }

    /**
     * @brief 创建编辑框控件
     * @param multiline 是否多行(默认false)
     * @param scrollbar 滚动条设置(默认2)
     * @return 创建结果(0表示成功)
     */
    int create(bool multiline = false, int scrollbar = 2)
    {
        m_destroying.store(false, std::memory_order_release);
        if (m_hwnd) {
            if (!destroy()) return -1;
            m_destroying.store(false, std::memory_order_release);
        }

        msg_createwindow msg = {NULL};
        HWND parentWindow = getHWnd();
        if (!parentWindow) return -1;

        msg.hEvent           = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        if (!msg.hEvent) return -1;
        msg.classname        = L"EDIT";
        msg.id               = egeControlBase::allocId();
        msg.style            = WS_CHILD | WS_BORDER | ES_LEFT | ES_WANTRETURN;

        if (multiline) {
            // 原生 EDIT 只作为输入/IME 后端使用，界面由 InputBox 自绘制；
            // 保留多行与自动换行能力，但不显示原生滚动条。
            msg.style |= ES_MULTILINE | ES_AUTOVSCROLL;
        } else {
            msg.style |= ES_AUTOHSCROLL;
        }

        msg.exstyle = WS_EX_CLIENTEDGE; // | WS_EX_STATICEDGE;
        msg.param   = this;

        if (!::PostMessageW(parentWindow, WM_USER + 1, 1, (LPARAM)&msg)) {
            ::CloseHandle(msg.hEvent);
            return -1;
        }
        DWORD waitResult = ::WaitForSingleObject(msg.hEvent, INFINITE);
        ::CloseHandle(msg.hEvent);
        if (waitResult != WAIT_OBJECT_0 || !msg.hwnd || !::IsWindow(msg.hwnd)) {
            m_hwnd = NULL;
            return -1;
        }

        m_hwnd    = msg.hwnd;
        m_hFont   = NULL;
        m_hBrush  = NULL;
        m_color   = 0x0;
        m_bgcolor = 0xFFFFFF;
        m_focus = false;
        m_multiline = multiline;
        m_imeComposing = false;
        m_imeCancelling = false;
        m_suppressedBackspaceChars = 0;
        m_suppressedReturnChars = 0;

        ::SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
        m_callback = ::GetWindowLongPtrW(m_hwnd, GWLP_WNDPROC);
        if (!m_callback) {
            destroy();
            return -1;
        }
        ::SetWindowLongPtrW(m_hwnd, GWLP_WNDPROC, (LONG_PTR)getProcfunc());
        {
            WCHAR fontname[] = L"SimSun";
            setfont(30, 0, fontname);
        }
        visible(false);

        return 0;
    }

    /**
     * @brief 销毁编辑框控件
     * @return 销毁结果(1表示成功，0表示失败)
     */
    int destroy()
    {
        if (!m_hwnd) return 0;

        m_destroying.store(true, std::memory_order_release);
        HWND hwnd = m_hwnd;
        visible(false);
        if (::IsWindow(hwnd)) {
            ::SendMessage(hwnd, WM_SETFONT, 0, 0);
        }
        if (m_hFont) {
            ::DeleteObject(m_hFont);
            m_hFont = NULL;
        }

        bool destroyed = !::IsWindow(hwnd);
        if (!destroyed) {
            if (isWindowOwnedByCurrentThread(hwnd)) {
                destroyed = ::DestroyWindow(hwnd) != FALSE;
            }
            else {
                HWND parentWindow = getHWnd();
                HANDLE event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
                if (parentWindow && event) {
                    msg_createwindow msg = {NULL};
                    msg.hwnd = hwnd;
                    msg.hEvent = event;
                    if (::PostMessageW(parentWindow, WM_USER + 1, 0, (LPARAM)&msg) &&
                        ::WaitForSingleObject(event, INFINITE) == WAIT_OBJECT_0) {
                        destroyed = !::IsWindow(hwnd);
                    }
                }
                if (event) ::CloseHandle(event);
            }
        }

        if (!destroyed) {
            m_destroying.store(false, std::memory_order_release);
            return 0;
        }
        if (m_hBrush) {
            ::DeleteObject(m_hBrush);
            m_hBrush = NULL;
        }
        m_hwnd = NULL;
        m_callback = 0;
        m_focus = false;
        m_imeComposing = false;
        m_imeCancelling = false;
        m_suppressedBackspaceChars = 0;
        m_suppressedReturnChars = 0;
        {
            std::lock_guard<std::mutex> lock(m_pendingInputMutex);
            m_pendingInputEvents.clear();
        }
        {
            std::lock_guard<std::mutex> lock(m_pendingNativeStateMutex);
            m_hasPendingNativeState = false;
            m_pendingNativeState = PendingNativeState{};
        }
        return 1;
    }
    
    /**
     * @brief 检查是否有焦点
     * @return 是否有焦点
     */
    bool hasFocus() const {
	    return m_focus.load(std::memory_order_acquire);
	}

    /**
     * @brief 取走 UI 线程采集的待处理输入事件。
     *
     * 必须由 InputBox 的应用线程调用。交换队列时只持有队列锁，不会在
     * 持锁状态下发送任何窗口消息。
     */
    void takePendingInputEvents(std::vector<PendingInputEvent>& events) {
        std::lock_guard<std::mutex> lock(m_pendingInputMutex);
        events.swap(m_pendingInputEvents);
    }

    /**
     * @brief 请求将应用线程的输入模型镜像到隐藏的原生 EDIT。
     *
     * 请求只保存最新快照，并投递给拥有 EDIT 的 UI 线程。UI 线程会在
     * 没有 IME 组合时一次性执行 WM_SETTEXT 和 EM_SETSEL，因此应用线程
     * 不会在原生输入法事务中跨线程发送同步消息。
     */
    void queueNativeState(const std::wstring& text, int selectionBegin, int selectionEnd);

    /**
     * @brief 移动光标到指定位置
     * @param begin 起始位置
     * @param end 结束位置
     */
    void movecursor(int begin,int end){
        if (m_hwnd && ::IsWindow(m_hwnd)) {
            SendMessageW(m_hwnd, EM_SETSEL, begin, end);
        }
    }

    /**
     * @brief 更新光标位置
     */
    void updatecursor();

    /**
     * @brief 获取光标位置
     * @return 光标位置(-1表示获取失败)
     */
    int getCursorPos()
    {
        if (m_hwnd && ::IsWindow(m_hwnd))
        {
            DWORD start = 0, end = 0;
            SendMessageW(m_hwnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
            return (int)start;
        }
        return -1;
    }

    /**
     * @brief 处理消息
     * @param message 消息类型
     * @param wParam 消息参数
     * @param lParam 消息参数
     * @return 处理结果
     */
    LRESULT onMessage(UINT message, WPARAM wParam, LPARAM lParam);

    /**
     * @brief 设置可见性
     * @param bvisible 是否可见
     */
    void visible(bool bvisible)
    {
        egeControlBase::visible(bvisible);
        if (m_hwnd && ::IsWindow(m_hwnd)) {
            ::ShowWindow(m_hwnd, (int)bvisible);
        }
    }

    /**
     * @brief 设置字体(ANSI版本)
     * @param h 字体高度
     * @param w 字体宽度
     * @param fontface 字体名称
     */
    void setfont(int h, int w, LPCSTR fontface)
    {
        if (!fontface) return;
        EGE_CONVERT_TO_WSTR_WITH(fontface, { setfont(h, w, wStr); });
    }

    /**
     * @brief 设置字体(Unicode版本)
     * @param h 字体高度
     * @param w 字体宽度
     * @param fontface 字体名称
     */
    void setfont(int h, int w, LPCWSTR fontface)
    {
        if (!m_hwnd || !::IsWindow(m_hwnd) || !fontface) return;
        LOGFONTW lf         = {0};
        lf.lfHeight         = h;
        lf.lfWidth          = w;
        lf.lfEscapement     = 0;
        lf.lfOrientation    = 0;
        lf.lfWeight         = FW_DONTCARE;
        lf.lfItalic         = 0;
        lf.lfUnderline      = 0;
        lf.lfStrikeOut      = 0;
        lf.lfCharSet        = DEFAULT_CHARSET;
        lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
        lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
        lf.lfQuality        = DEFAULT_QUALITY;
        lf.lfPitchAndFamily = DEFAULT_PITCH;
        lstrcpyW(lf.lfFaceName, fontface);
        HFONT hFont = CreateFontIndirectW(&lf);
        if (hFont) {
            ::SendMessageW(m_hwnd, WM_SETFONT, (WPARAM)hFont, 0);
            ::DeleteObject(m_hFont);
            m_hFont = hFont;
        }
    }

    /**
     * @brief 移动控件位置
     * @param x x坐标
     * @param y y坐标
     */
    void move(int x, int y)
    {
        egeControlBase::move(x, y);
        if (m_hwnd && ::IsWindow(m_hwnd)) {
            ::MoveWindow(m_hwnd, m_x, m_y, m_w, m_h, TRUE);
        }
    }

    /**
     * @brief 设置控件大小
     * @param w 宽度
     * @param h 高度
     */
    void size(int w, int h)
    {
        egeControlBase::size(w, h);
        if (m_hwnd && ::IsWindow(m_hwnd)) {
            ::MoveWindow(m_hwnd, m_x, m_y, m_w, m_h, TRUE);
        }
    }

    /**
     * @brief 设置文本(ANSI版本)
     * @param text 文本内容
     */
    void settext(LPCSTR text)
    {
        if (!text) return;
        EGE_CONVERT_TO_WSTR_WITH(text, { settext(wStr); });
    }
    
    /**
     * @brief 取消焦点
     */
    void killfocus()
	{
	    if (!m_hwnd || !::IsWindow(m_hwnd)) return;
	    HWND parentWindow = getHWnd();
	    if (isWindowOwnedByCurrentThread(m_hwnd)) {
	        ::SetFocus(parentWindow);
	        return;
	    }
	    // 将焦点设置回主窗口或 NULL（无焦点）
	    msg_createwindow msg = {NULL};
		msg.hwnd = parentWindow;
		msg.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (!msg.hEvent) return;
		if (parentWindow &&
		    PostMessageW(parentWindow, WM_USER + 2, 0, (LPARAM)&msg)) {
			WaitForSingleObject(msg.hEvent, INFINITE);
		}
		CloseHandle(msg.hEvent);
	}

    /**
     * @brief 设置文本(Unicode版本)
     * @param text 文本内容
     */
    void settext(LPCWSTR text)
    {
        if (m_hwnd && ::IsWindow(m_hwnd) && text) {
            ::SendMessageW(m_hwnd, WM_SETTEXT, 0, (LPARAM)text);
        }
    }

    /**
     * @brief 获取文本(ANSI版本)
     * @param maxlen 最大长度
     * @param text 输出文本缓冲区
     */
    void gettext(int maxlen, LPSTR text)
    {
        if (m_hwnd && ::IsWindow(m_hwnd) && maxlen > 0 && text) {
            ::SendMessageA(m_hwnd, WM_GETTEXT, (WPARAM)maxlen, (LPARAM)text);
        }
    }

    /**
     * @brief 获取文本(Unicode版本)
     * @param maxlen 最大长度
     * @param text 输出文本缓冲区
     */
    void gettext(int maxlen, LPWSTR text)
    {
        if (m_hwnd && ::IsWindow(m_hwnd) && maxlen > 0 && text) {
            ::SendMessageW(m_hwnd, WM_GETTEXT, (WPARAM)maxlen, (LPARAM)text);
        }
    }

    int gettextlength() const
    {
        if (!m_hwnd || !::IsWindow(m_hwnd)) return -1;
        LRESULT length = ::SendMessageW(m_hwnd, WM_GETTEXTLENGTH, 0, 0);
        if (length < 0 || length > INT_MAX) return -1;
        return (int)length;
    }

    /**
     * @brief 设置最大文本长度
     * @param maxlen 最大长度
     */
    void setmaxlen(int maxlen)
    {
        if (m_hwnd && ::IsWindow(m_hwnd) && maxlen >= 0) {
            ::SendMessageW(m_hwnd, EM_LIMITTEXT, (WPARAM)maxlen, 0);
        }
    }

    /**
     * @brief 设置文本颜色
     * @param color 颜色值
     */
    void setcolor(color_t color)
    {
        m_color = color;
        if (m_hwnd && ::IsWindow(m_hwnd)) {
            ::InvalidateRect(m_hwnd, NULL, TRUE);
        }
    }

    /**
     * @brief 设置背景颜色
     * @param bgcolor 背景颜色值
     */
    void setbgcolor(color_t bgcolor)
    {
        m_bgcolor = bgcolor;
        //::RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
        if (m_hwnd && ::IsWindow(m_hwnd)) {
            ::InvalidateRect(m_hwnd, NULL, TRUE);
        }
    }

    /**
     * @brief 设置只读状态
     * @param readonly 是否只读
     */
    void setreadonly(bool readonly)
    {
        if (m_hwnd && ::IsWindow(m_hwnd)) {
            ::SendMessageW(m_hwnd, EM_SETREADONLY, (WPARAM)readonly, 0);
            ::InvalidateRect(m_hwnd, NULL, TRUE);
        }
    }

    /**
     * @brief 设置焦点到编辑框
     */
    void setfocus()
    {
        if (!m_hwnd || !::IsWindow(m_hwnd)) return;
        if (isWindowOwnedByCurrentThread(m_hwnd)) {
            ::SetFocus(m_hwnd);
            return;
        }
        msg_createwindow msg = {NULL};
        msg.hwnd             = m_hwnd;
        msg.hEvent           = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        if (!msg.hEvent) return;
        HWND parentWindow = getHWnd();
        if (parentWindow &&
            ::PostMessageW(parentWindow, WM_USER + 2, 0, (LPARAM)&msg)) {
            ::WaitForSingleObject(msg.hEvent, INFINITE);
        }
        ::CloseHandle(msg.hEvent);
    }

    void killIME();

private:
    struct PendingNativeState {
        std::wstring text;
        int selectionBegin = 0;
        int selectionEnd = 0;
    };

    void enqueueInputEvent(PendingInputEvent event) {
        if (m_destroying.load(std::memory_order_acquire)) return;
        std::lock_guard<std::mutex> lock(m_pendingInputMutex);
        m_pendingInputEvents.emplace_back(std::move(event));
    }

    // Must run on the UI/window thread.  It never holds either mutex while
    // calling the original EDIT window procedure, which can dispatch nested
    // Windows messages.
    void applyPendingNativeState();

    static bool isWindowOwnedByCurrentThread(HWND window) {
        return window != NULL &&
            ::GetWindowThreadProcessId(window, NULL) == ::GetCurrentThreadId();
    }

public:
    HWND     m_hwnd;        ///< 窗口句柄
    HFONT    m_hFont;       ///< 字体句柄
    HBRUSH   m_hBrush;      ///< 画刷句柄
    color_t  m_color;       ///< 文本颜色
    color_t  m_bgcolor;     ///< 背景颜色
    LONG_PTR m_callback;    ///< 回调函数指针
    std::atomic_bool m_focus; ///< 焦点状态（由 UI 线程写、可能被应用线程读取）

private:
    std::mutex m_pendingInputMutex;
    std::vector<PendingInputEvent> m_pendingInputEvents;
    std::mutex m_pendingNativeStateMutex;
    PendingNativeState m_pendingNativeState;
    bool m_hasPendingNativeState = false;
    std::atomic_bool m_destroying{false};
    bool m_multiline = false;
    // UI 线程在 WM_IME_START/ENDCOMPOSITION 间保持该标记。原生 EDIT
    // 镜像也只由 UI 线程据此决定何时应用，避免跨线程检查后的竞态。
    std::atomic_bool m_imeComposing{false};
    bool m_imeCancelling = false;
    unsigned int m_suppressedBackspaceChars = 0;
    unsigned int m_suppressedReturnChars = 0;
};

#undef EGE_CONVERT_TO_WSTR_WITH

} // namespace FeEGE
#endif /*EGE_SYS_EDIT_H*/
