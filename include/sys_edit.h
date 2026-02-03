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
        if (m_hwnd) {
            destroy();
        }

        msg_createwindow msg = {NULL};
        msg.hEvent           = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        msg.classname        = L"EDIT";
        msg.id               = egeControlBase::allocId();
        msg.style            = WS_CHILD | WS_BORDER | ES_LEFT;

        if (multiline) {
            msg.style |= ES_MULTILINE | WS_VSCROLL | ES_WANTRETURN;
        } else {
            msg.style |= ES_AUTOHSCROLL;
        }

        msg.exstyle = WS_EX_CLIENTEDGE; // | WS_EX_STATICEDGE;
        msg.param   = this;

        ::PostMessageW(getHWnd(), WM_USER + 1, 1, (LPARAM)&msg);
        ::WaitForSingleObject(msg.hEvent, INFINITE);

        m_hwnd    = msg.hwnd;
        m_hFont   = NULL;
        m_hBrush  = NULL;
        m_color   = 0x0;
        m_bgcolor = 0xFFFFFF;
        m_focus = false;

        ::SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
        m_callback = ::GetWindowLongPtrW(m_hwnd, GWLP_WNDPROC);
        ::SetWindowLongPtrW(m_hwnd, GWLP_WNDPROC, (LONG_PTR)getProcfunc());
        {
            WCHAR fontname[] = L"SimSun";
            setfont(30, 0, fontname);
        }
        visible(false);

        ::CloseHandle(msg.hEvent);

        return 0;
    }

    /**
     * @brief 销毁编辑框控件
     * @return 销毁结果(1表示成功，0表示失败)
     */
    int destroy()
    {
        if (m_hwnd) {
            visible(false);
            msg_createwindow msg = {NULL};
            msg.hwnd             = m_hwnd;
            msg.hEvent           = ::CreateEvent(NULL, TRUE, FALSE, NULL);
            ::SendMessage(m_hwnd, WM_SETFONT, 0, 0);
            ::DeleteObject(m_hFont);
            ::PostMessageW(getHWnd(), WM_USER + 1, 0, (LPARAM)&msg);
            ::WaitForSingleObject(msg.hEvent, INFINITE);
            ::CloseHandle(msg.hEvent);
            if (m_hBrush) {
                ::DeleteObject(m_hBrush);
            }
            m_hwnd = NULL;
            return 1;
        }
        return 0;
    }
    
    /**
     * @brief 检查是否有焦点
     * @return 是否有焦点
     */
    bool hasFocus() const {
	    return m_focus;
	}

    /**
     * @brief 设置父对象
     * @param p 父对象指针
     */
    void setparent(void* p){
        m_object = p;
    }

    /**
     * @brief 移动光标到指定位置
     * @param begin 起始位置
     * @param end 结束位置
     */
    void movecursor(int begin,int end){
        SendMessageW(m_hwnd, EM_SETSEL, begin, end);
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
        if (m_hwnd)
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
        ::ShowWindow(m_hwnd, (int)bvisible);
    }

    /**
     * @brief 设置字体(ANSI版本)
     * @param h 字体高度
     * @param w 字体宽度
     * @param fontface 字体名称
     */
    void setfont(int h, int w, LPCSTR fontface)
    {
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
        ::MoveWindow(m_hwnd, m_x, m_y, m_w, m_h, TRUE);
    }

    /**
     * @brief 设置控件大小
     * @param w 宽度
     * @param h 高度
     */
    void size(int w, int h)
    {
        egeControlBase::size(w, h);
        ::MoveWindow(m_hwnd, m_x, m_y, m_w, m_h, TRUE);
    }

    /**
     * @brief 设置文本(ANSI版本)
     * @param text 文本内容
     */
    void settext(LPCSTR text)
    {
        EGE_CONVERT_TO_WSTR_WITH(text, { settext(wStr); });
    }
    
    /**
     * @brief 取消焦点
     */
    void killfocus()
	{
	    // 将焦点设置回主窗口或 NULL（无焦点）
	    msg_createwindow msg = {NULL};
		msg.hwnd = GetForegroundWindow(); // 注意这里，这样可以不改变窗口活动状态
		msg.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		PostMessageW(getHWnd(), WM_USER + 2, 0, (LPARAM)&msg);
		WaitForSingleObject(msg.hEvent, INFINITE);
	}

    /**
     * @brief 设置文本(Unicode版本)
     * @param text 文本内容
     */
    void settext(LPCWSTR text) { ::SendMessageW(m_hwnd, WM_SETTEXT, 0, (LPARAM)text); }

    /**
     * @brief 获取文本(ANSI版本)
     * @param maxlen 最大长度
     * @param text 输出文本缓冲区
     */
    void gettext(int maxlen, LPSTR text) { ::SendMessageA(m_hwnd, WM_GETTEXT, (WPARAM)maxlen, (LPARAM)text); }

    /**
     * @brief 获取文本(Unicode版本)
     * @param maxlen 最大长度
     * @param text 输出文本缓冲区
     */
    void gettext(int maxlen, LPWSTR text) { ::SendMessageW(m_hwnd, WM_GETTEXT, (WPARAM)maxlen, (LPARAM)text); }

    /**
     * @brief 设置最大文本长度
     * @param maxlen 最大长度
     */
    void setmaxlen(int maxlen) { ::SendMessageW(m_hwnd, EM_LIMITTEXT, (WPARAM)maxlen, 0); }

    /**
     * @brief 设置文本颜色
     * @param color 颜色值
     */
    void setcolor(color_t color)
    {
        m_color = color;
        ::InvalidateRect(m_hwnd, NULL, TRUE);
    }

    /**
     * @brief 设置背景颜色
     * @param bgcolor 背景颜色值
     */
    void setbgcolor(color_t bgcolor)
    {
        m_bgcolor = bgcolor;
        //::RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE);
        ::InvalidateRect(m_hwnd, NULL, TRUE);
    }

    /**
     * @brief 设置只读状态
     * @param readonly 是否只读
     */
    void setreadonly(bool readonly)
    {
        ::SendMessageW(m_hwnd, EM_SETREADONLY, (WPARAM)readonly, 0);
        ::InvalidateRect(m_hwnd, NULL, TRUE);
    }

    /**
     * @brief 设置焦点到编辑框
     */
    void setfocus()
    {
        msg_createwindow msg = {NULL};
        msg.hwnd             = m_hwnd;
        msg.hEvent           = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        ::PostMessageW(getHWnd(), WM_USER + 2, 0, (LPARAM)&msg);
        ::WaitForSingleObject(msg.hEvent, INFINITE);
    }

public:
    HWND     m_hwnd;        ///< 窗口句柄
    HFONT    m_hFont;       ///< 字体句柄
    HBRUSH   m_hBrush;      ///< 画刷句柄
    color_t  m_color;       ///< 文本颜色
    color_t  m_bgcolor;     ///< 背景颜色
    LONG_PTR m_callback;    ///< 回调函数指针
    bool m_focus;           ///< 焦点状态
    void* m_object;         ///< 关联对象指针
};

#undef EGE_CONVERT_TO_WSTR_WITH

} // namespace ege
#endif /*EGE_SYS_EDIT_H*/
