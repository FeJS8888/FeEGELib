/*********************************************************
 * EGE (Easy Graphics Engine)  24.04
 * FileName:    ege.h
 * Website:     https://xege.org
 * Community:   https://club.xege.org
 * GitHub:      https://github.com/wysaid/xege
 * GitHub:      https://github.com/Easy-Graphics-Engine
 * Gitee:       https://gitee.com/xege/xege
 * Blog:        https://blog.csdn.net/qq_39151563/article/details/125688290
 * E-Mail:      this@xege.org
 *
 *********************************************************/

#ifndef EGE_H
#define EGE_H

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

// Easy Graphics Engine Version
// Calendar Versioning, format: YY.0M.PatchNumber (If the PatchNumber equals 0, the YY.0M format is used.)
#define EGE_VERSION        "24.04"
#define EGE_VERSION_MAJOR  24
#define EGE_VERSION_MINOR  4
#define EGE_VERSION_PATCH  0
#define EGE_MAKE_VERSION_NUMBER(major, minor, patch)    ((major) * 10000L + (minor) * 100L + (patch))
#define EGE_VERSION_NUMBER    EGE_MAKE_VERSION_NUMBER(EGE_VERSION_MAJOR, EGE_VERSION_MINOR, EGE_VERSION_PATCH)

#ifndef __cplusplus
#error You must use a C++ compiler and ensure that your source files is named with the '.cpp' suffix.
#endif

#if defined(_INC_CONIO) || defined(_CONIO_H_)
#error You cannot include "conio.h" before "graphics.h".
#endif

#if defined(_MSC_VER)
#   pragma warning(disable: 4355)
#   ifndef _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH
#       define _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH
#   endif
#   ifndef _ALLOW_RUNTIME_LIBRARY_MISMATCH
#       define _ALLOW_RUNTIME_LIBRARY_MISMATCH
#   endif
#endif

#if !defined(EGE_GRAPH_LIB_BUILD) && !defined(EGE_GRAPH_NO_LIB)
#   ifdef _MSC_VER
#       pragma comment(lib,"gdiplus.lib")
#       ifdef _WIN64 // 64 bit libs
#           pragma comment(lib,"graphics.lib")
#       else   // 32 bit libs
#           pragma comment(lib,"graphics.lib")
#       endif
#   endif
#endif


#if !defined(EGE_GRAPH_LIB_BUILD) && !defined(EGE_GRAPH_NO_LIB)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif
#endif

#include "ege/stdint.h"

#if defined(EGE_FOR_AUTO_CODE_COMPLETETION_ONLY)
#include <windef.h>
#include <winuser.h>
#include <wingdi.h>
#else
#include <windows.h>
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1300)
#define EGE_COMPILERINFO_VC6
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1200 && !defined(SetWindowLongPtr)
#   define SetWindowLongPtrW   SetWindowLongW
#   define GetWindowLongPtrW   GetWindowLongW
#   define GWLP_USERDATA       GWL_USERDATA
#   define GWLP_WNDPROC        GWL_WNDPROC
#endif

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif

#ifndef EGE_CDECL
#   if __STDC__
#       define EGE_CDECL  __cdecl
#   else
#       define EGE_CDECL  __cdecl
#   endif
#endif

#ifdef _MSC_VER
#   if defined(_WIN64)
#       define EGEAPI
#   else
#       define EGEAPI EGE_CDECL
#   endif
#else
#   if defined(__WORDSIZE)
#       if __WORDSIZE > 32
#           define EGEAPI
#       else
#           define EGEAPI EGE_CDECL
#       endif
#   else
#       define EGEAPI
#   endif
#endif

#ifndef EGE_DEPRECATE
#   ifdef _MSC_VER
#       ifdef _CRT_DEPRECATE_TEXT
#           define EGE_DEPRECATE(function, msg) _CRT_DEPRECATE_TEXT("This function is deprecated. " msg " For more information, visit https://xege.org .")
#       else
#           define EGE_DEPRECATE(function, msg)
#       endif
#   elif ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)))
#       define EGE_DEPRECATE(function, msg) __attribute__((deprecated(msg " For more information, visit https://xege.org .")))
#   else
#       define EGE_DEPRECATE(function, msg) __attribute__((deprecated))
#   endif
#endif

#define EGE_GDIPLUS

#define EGERGBA(r, g, b, a)  ((::ege::color_t)(((r) << 16) | ((g) << 8) | (b) | ((a) << 24)))
#define EGERGB(r, g, b)      EGERGBA(r, g, b, 0xFF)
#define EGEARGB(a, r, g, b)  EGERGBA(r, g, b, a)
#define EGEACOLOR(a, color)  ((::ege::color_t)(((color) & 0xFFFFFF) | ((a) << 24)))
#define EGECOLORA(color, a)  EGEACOLOR(a, color)
#define EGEGET_R(c)          (((c) >> 16) & 0xFF)
#define EGEGET_G(c)          (((c) >> 8) & 0xFF)
#define EGEGET_B(c)          (((c)) & 0xFF)
#define EGEGET_A(c)          (((c) >> 24) & 0xFF)
#define EGEGRAY(gray)        EGERGB(gray, gray, gray)
#define EGEGRAYA(gray, a)    EGERGBA(gray, gray, gray, a)
#define EGEAGRAY(a, gray)    EGEGRAYA(gray, a)

/* you can also use 932 as shift-jis, 950 as big5 ... */
/* see https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers */
#define EGE_CODEPAGE_GB2312    936
#define EGE_CODEPAGE_UTF8      65001
#define EGE_CODEPAGE_ANSI      0

namespace ege
{

const double PI = 3.1415926535897932384626;

/* define graphics drivers */
enum graphics_drivers
{
    DETECT = 0,         /* requests autodetection */
    CGA, MCGA, EGA, EGA64, EGAMONO, IBM8514,/* 1 - 6 */
    HERCMONO, ATT400, VGA, PC3270,          /* 7 - 10 */
    TRUECOLOR, TRUECOLORSIZE,
    CURRENT_DRIVER = -1
};

/* graphics modes for each driver */
enum graphics_modes
{
    CGAC0      = 0, /* 320x200 palette 0; 1 page  */
    CGAC1      = 1, /* 320x200 palette 1; 1 page  */
    CGAC2      = 2, /* 320x200 palette 2: 1 page  */
    CGAC3      = 3, /* 320x200 palette 3; 1 page  */
    CGAHI      = 4, /* 640x200 1 page             */
    MCGAC0     = 0, /* 320x200 palette 0; 1 page  */
    MCGAC1     = 1, /* 320x200 palette 1; 1 page  */
    MCGAC2     = 2, /* 320x200 palette 2; 1 page  */
    MCGAC3     = 3, /* 320x200 palette 3; 1 page  */
    MCGAMED    = 4, /* 640x200 1 page             */
    MCGAHI     = 5, /* 640x480 1 page             */
    EGALO      = 0, /* 640x200 16 color 4 pages   */
    EGAHI      = 1, /* 640x350 16 color 2 pages   */
    EGA64LO    = 0, /* 640x200 16 color 1 page    */
    EGA64HI    = 1, /* 640x350 4 color  1 page    */
    EGAMONOHI  = 0, /* 640x350 64K on card, 1 page - 256K on card, 4 pages */
    HERCMONOHI = 0, /* 720x348 2 pages            */
    ATT400C0   = 0, /* 320x200 palette 0; 1 page  */
    ATT400C1   = 1, /* 320x200 palette 1; 1 page  */
    ATT400C2   = 2, /* 320x200 palette 2; 1 page  */
    ATT400C3   = 3, /* 320x200 palette 3; 1 page  */
    ATT400MED  = 4, /* 640x200 1 page             */
    ATT400HI   = 5, /* 640x400 1 page             */
    VGALO      = 0, /* 640x200 16 color 4 pages   */
    VGAMED     = 1, /* 640x350 16 color 2 pages   */
    VGAHI      = 2, /* 640x480 16 color 1 page    */
    PC3270HI   = 0, /* 720x350 1 page             */
    IBM8514LO  = 0, /* 640x480 256 colors         */
    IBM8514HI  = 1  /*1024x768 256 colors         */
};

enum initmode_flag
{
    INIT_DEFAULT         = 0x0,   ///< 榛樿妯″紡
    INIT_NOBORDER        = 0x1,   ///< 鏃犺竟妗嗙獥鍙?
    INIT_CHILD           = 0x2,   ///< 瀛愮獥鍙ｆā寮?
    INIT_TOPMOST         = 0x4,   ///< 缃《绐楀彛
    INIT_RENDERMANUAL    = 0x8,   ///< 鎵嬪姩娓叉煋妯″紡
    INIT_NOFORCEEXIT     = 0x10,  ///< 鍏抽棴绐楀彛鏃朵笉寮哄埗閫€鍑虹▼搴忥紝鍙缃唴閮ㄦ爣蹇椾綅锛宨s_run() 鍙互鑾峰彇鏍囧織浣?
    INIT_UNICODE         = 0x20,  ///< Unicode瀛楃娑堟伅 (绛夊悓浜巗etunicodecharmessage(true))
    INIT_HIDE            = 0x40,  ///< 闅愯棌绐楀彛
    INIT_WITHLOGO        = 0x100, ///< 鍚姩鏃舵樉绀篍GE Logo 鍔ㄧ敾 (Debug鐗堟湰涓嬮粯璁や笉鏄剧ず)
    INIT_ANIMATION       = INIT_DEFAULT | INIT_RENDERMANUAL | INIT_NOFORCEEXIT ///< 鍔ㄧ敾妯″紡
};

/**
 * @enum rendermode_e
 * @brief 娓叉煋妯″紡
 */
enum rendermode_e
{
    RENDER_AUTO,   ///< 鑷姩娓叉煋
    RENDER_MANUAL  ///< 鎵嬪姩娓叉煋
};

/**
 * @enum graphics_errors
 * @brief 鍥惧舰鎿嶄綔閿欒鐮?
 * 
 * 瀹氫箟浜嗗浘褰㈡搷浣滃彲鑳借繑鍥炵殑鍚勭閿欒浠ｇ爜
 */
enum graphics_errors
{
    grOk                 = 0,           ///< 鎿嶄綔鎴愬姛
    grNoInitGraph        = -1,          ///< 鍥惧舰绯荤粺鏈垵濮嬪寲
    grNotDetected        = -2,          ///< 鏈娴嬪埌鍥惧舰璁惧
    grFileNotFound       = -3,          ///< 鏂囦欢鏈壘鍒?
    grInvalidDriver      = -4,          ///< 鏃犳晥鐨勯┍鍔?
    grNoLoadMem          = -5,          ///< 鍐呭瓨鍔犺浇澶辫触
    grNoScanMem          = -6,          ///< 鎵弿鍐呭瓨澶辫触
    grNoFloodMem         = -7,          ///< 濉厖鍐呭瓨澶辫触
    grFontNotFound       = -8,          ///< 瀛椾綋鏈壘鍒?
    grNoFontMem          = -9,          ///< 瀛椾綋鍐呭瓨涓嶈冻
    grInvalidMode        = -10,         ///< 鏃犳晥妯″紡
    grError              = -11,         ///< 閫氱敤閿欒
    grIOerror            = -12,         ///< I/O閿欒
    grInvalidFont        = -13,         ///< 鏃犳晥瀛椾綋
    grInvalidFontNum     = -14,         ///< 鏃犳晥瀛椾綋缂栧彿
    grInvalidVersion     = -18,         ///< 鐗堟湰涓嶅吋瀹?

    grException          = 0x10,        ///< EGE寮傚父
    grParamError         = 0x11,        ///< 鍙傛暟閿欒
    grInvalidRegion      = 0x12,        ///< 鏃犳晥鍖哄煙
    grOutOfMemory        = 0x13,        ///< 鍐呭瓨涓嶈冻
    grNullPointer        = 0x14,        ///< 绌烘寚閽?
    grAllocError         = 0x15,        ///< 鍒嗛厤閿欒
    grInvalidMemory      = 0xCDCDCDCD   ///< 鏃犳晥鍐呭瓨
};

/**
 * @enum message_event
 * @brief 娑堟伅浜嬩欢绫诲瀷
 * 
 * 瀹氫箟浜嗛紶鏍囧拰閿洏娑堟伅鐨勪簨浠剁被鍨嬶紝鐢ㄤ簬娑堟伅澶勭悊
 */
enum message_event
{
    MSG_EVENT_UP         = 0x00,    ///< 鎸夐敭/榧犳爣鎸夐挳閲婃斁浜嬩欢
    MSG_EVENT_DOWN       = 0x01,    ///< 鎸夐敭/榧犳爣鎸夐挳鎸変笅浜嬩欢
    MSG_EVENT_CLICK      = 0x01,    ///< 榧犳爣鍗曞嚮浜嬩欢锛堢瓑鍚屼簬DOWN锛?
    MSG_EVENT_DBCLICK    = 0x02,    ///< 榧犳爣鍙屽嚮浜嬩欢
    MSG_EVENT_MOVE       = 0x04,    ///< 榧犳爣绉诲姩浜嬩欢
    MSG_EVENT_WHEEL      = 0x10     ///< 榧犳爣婊氳疆浜嬩欢
};

/**
 * @enum message_mouse
 * @brief 榧犳爣鎸夐挳鏍囪瘑
 * 
 * 瀹氫箟浜嗕笉鍚岀殑榧犳爣鎸夐挳锛屽彲浠ラ€氳繃浣嶆垨鎿嶄綔缁勫悎浣跨敤
 */
enum message_mouse
{
    MSG_MOUSE_LEFT  	 = 0x01,    ///< 榧犳爣宸﹂敭
    MSG_MOUSE_RIGHT 	 = 0x02,    ///< 榧犳爣鍙抽敭
    MSG_MOUSE_MID   	 = 0x04     ///< 榧犳爣涓敭锛堟粴杞寜閿級
};


#ifndef EGE_COLOR_T_TYPEDEF
#define EGE_COLOR_T_TYPEDEF
/// @brief 棰滆壊绫诲瀷瀹氫箟锛屼娇鐢?2浣嶆棤绗﹀彿鏁存暟琛ㄧずARGB棰滆壊
typedef uint32_t color_t;
#endif

/**
 * @enum alpha_type
 * @brief Alpha閫氶亾绫诲瀷
 * 
 * 瀹氫箟浜嗗浘鍍廇lpha閫氶亾鐨勪笉鍚屽鐞嗘柟寮?
 */
enum alpha_type
{
    ALPHATYPE_STRAIGHT      = 0,    ///< 鐩存帴Alpha锛堥潪棰勪箻Alpha锛?
    ALPHATYPE_PREMULTIPLIED = 1     ///< 棰勪箻Alpha
};

/**
 * @struct ege_point
 * @brief 娴偣鍧愭爣鐐圭粨鏋?
 * 
 * 鐢ㄤ簬琛ㄧず浜岀淮绌洪棿涓殑涓€涓偣锛屽潗鏍囦娇鐢ㄦ诞鐐规暟
 */
struct ege_point
{
    float x;    ///< x鍧愭爣
    float y;    ///< y鍧愭爣
};

/**
 * @struct ege_rect
 * @brief 鐭╁舰鍖哄煙缁撴瀯
 * 
 * 鐢ㄤ簬琛ㄧず鐭╁舰鍖哄煙锛屽寘鍚綅缃拰灏哄淇℃伅
 */
struct ege_rect
{
    float x;    ///< 鐭╁舰宸︿笂瑙抶鍧愭爣
    float y;    ///< 鐭╁舰宸︿笂瑙抷鍧愭爣
    float w;    ///< 鐭╁舰瀹藉害
    float h;    ///< 鐭╁舰楂樺害
};

/**
 * @struct ege_colpoint
 * @brief 甯﹂鑹茬殑鍧愭爣鐐圭粨鏋?
 * 
 * 鐢ㄤ簬琛ㄧず甯︽湁棰滆壊淇℃伅鐨勪簩缁村潗鏍囩偣锛屽父鐢ㄤ簬娓愬彉鏁堟灉
 */
struct ege_colpoint
{
    float   x;      ///< x鍧愭爣
    float   y;      ///< y鍧愭爣
    color_t color;  ///< 璇ョ偣鐨勯鑹插€?
};

/**
 * @enum COLORS
 * @brief 棰勫畾涔夐鑹插父閲?
 * 
 * 鎻愪緵浜嗗父鐢ㄧ殑棰滆壊甯搁噺锛屽熀浜嶹eb瀹夊叏鑹插僵鏍囧噯瀹氫箟
 * 棰滆壊鍊间娇鐢≧GB鏍煎紡锛屽彲浠ョ洿鎺ョ敤浜庣粯鍥惧嚱鏁?
 */
enum COLORS
{
    ALICEBLUE            = EGERGB(0xF0, 0xF8, 0xFF),
    ANTIQUEWHITE         = EGERGB(0xFA, 0xEB, 0xD7),
    AQUA                 = EGERGB(0x00, 0xFF, 0xFF),
    AQUAMARINE           = EGERGB(0x7F, 0xFF, 0xD4),
    AZURE                = EGERGB(0xF0, 0xFF, 0xFF),
    BEIGE                = EGERGB(0xF5, 0xF5, 0xDC),
    BISQUE               = EGERGB(0xFF, 0xE4, 0xC4),
    BLACK                = EGERGB(0x00, 0x00, 0x00),
    BLANCHEDALMOND       = EGERGB(0xFF, 0xEB, 0xCD),
    BLUE                 = EGERGB(0x00, 0x00, 0xFF),
    BLUEVIOLET           = EGERGB(0x8A, 0x2B, 0xE2),
    BROWN                = EGERGB(0xA5, 0x2A, 0x2A),
    BURLYWOOD            = EGERGB(0xDE, 0xB8, 0x87),
    CADETBLUE            = EGERGB(0x5F, 0x9E, 0xA0),
    CHARTREUSE           = EGERGB(0x7F, 0xFF, 0x00),
    CHOCOLATE            = EGERGB(0xD2, 0x69, 0x1E),
    CORAL                = EGERGB(0xFF, 0x7F, 0x50),
    CORNFLOWERBLUE       = EGERGB(0x64, 0x95, 0xED),
    CORNSILK             = EGERGB(0xFF, 0xF8, 0xDC),
    CRIMSON              = EGERGB(0xDC, 0x14, 0x3C),
    CYAN                 = EGERGB(0x00, 0xFF, 0xFF),
    DARKBLUE             = EGERGB(0x00, 0x00, 0x8B),
    DARKCYAN             = EGERGB(0x00, 0x8B, 0x8B),
    DARKGOLDENROD        = EGERGB(0xB8, 0x86, 0x0B),
    DARKGRAY             = EGERGB(0xA9, 0xA9, 0xA9),
    DARKGREEN            = EGERGB(0x00, 0x64, 0x00),
    DARKKHAKI            = EGERGB(0xBD, 0xB7, 0x6B),
    DARKMAGENTA          = EGERGB(0x8B, 0x00, 0x8B),
    DARKOLIVEGREEN       = EGERGB(0x55, 0x6B, 0x2F),
    DARKORANGE           = EGERGB(0xFF, 0x8C, 0x00),
    DARKORCHID           = EGERGB(0x99, 0x32, 0xCC),
    DARKRED              = EGERGB(0x8B, 0x00, 0x00),
    DARKSALMON           = EGERGB(0xE9, 0x96, 0x7A),
    DARKSEAGREEN         = EGERGB(0x8F, 0xBC, 0x8F),
    DARKSLATEBLUE        = EGERGB(0x48, 0x3D, 0x8B),
    DARKSLATEGRAY        = EGERGB(0x2F, 0x4F, 0x4F),
    DARKTURQUOISE        = EGERGB(0x00, 0xCE, 0xD1),
    DARKVIOLET           = EGERGB(0x94, 0x00, 0xD3),
    DEEPPINK             = EGERGB(0xFF, 0x14, 0x93),
    DEEPSKYBLUE          = EGERGB(0x00, 0xBF, 0xFF),
    DIMGRAY              = EGERGB(0x69, 0x69, 0x69),
    DODGERBLUE           = EGERGB(0x1E, 0x90, 0xFF),
    FIREBRICK            = EGERGB(0xB2, 0x22, 0x22),
    FLORALWHITE          = EGERGB(0xFF, 0xFA, 0xF0),
    FORESTGREEN          = EGERGB(0x22, 0x8B, 0x22),
    FUCHSIA              = EGERGB(0xFF, 0x00, 0xFF),
    GAINSBORO            = EGERGB(0xDC, 0xDC, 0xDC),
    GHOSTWHITE           = EGERGB(0xF8, 0xF8, 0xFF),
    GOLD                 = EGERGB(0xFF, 0xD7, 0x00),
    GOLDENROD            = EGERGB(0xDA, 0xA5, 0x20),
    GRAY                 = EGERGB(0x80, 0x80, 0x80),
    GREEN                = EGERGB(0x00, 0x80, 0x00),
    GREENYELLOW          = EGERGB(0xAD, 0xFF, 0x2F),
    HONEYDEW             = EGERGB(0xF0, 0xFF, 0xF0),
    HOTPINK              = EGERGB(0xFF, 0x69, 0xB4),
    INDIANRED            = EGERGB(0xCD, 0x5C, 0x5C),
    INDIGO               = EGERGB(0x4B, 0x00, 0x82),
    IVORY                = EGERGB(0xFF, 0xFF, 0xF0),
    KHAKI                = EGERGB(0xF0, 0xE6, 0x8C),
    LAVENDER             = EGERGB(0xE6, 0xE6, 0xFA),
    LAVENDERBLUSH        = EGERGB(0xFF, 0xF0, 0xF5),
    LAWNGREEN            = EGERGB(0x7C, 0xFC, 0x00),
    LEMONCHIFFON         = EGERGB(0xFF, 0xFA, 0xCD),
    LIGHTBLUE            = EGERGB(0xAD, 0xD8, 0xE6),
    LIGHTCORAL           = EGERGB(0xF0, 0x80, 0x80),
    LIGHTCYAN            = EGERGB(0xE0, 0xFF, 0xFF),
    LIGHTGOLDENRODYELLOW = EGERGB(0xFA, 0xFA, 0xD2),
    LIGHTGRAY            = EGERGB(0xD3, 0xD3, 0xD3),
    LIGHTGREEN           = EGERGB(0x90, 0xEE, 0x90),
    LIGHTPINK            = EGERGB(0xFF, 0xB6, 0xC1),
    LIGHTSALMON          = EGERGB(0xFF, 0xA0, 0x7A),
    LIGHTSEAGREEN        = EGERGB(0x20, 0xB2, 0xAA),
    LIGHTSKYBLUE         = EGERGB(0x87, 0xCE, 0xFA),
    LIGHTSLATEGRAY       = EGERGB(0x77, 0x88, 0x99),
    LIGHTSTEELBLUE       = EGERGB(0xB0, 0xC4, 0xDE),
    LIGHTYELLOW          = EGERGB(0xFF, 0xFF, 0xE0),
    LIGHTRED             = EGERGB(0xFC, 0x54, 0x54),
    LIGHTMAGENTA         = EGERGB(0xFC, 0x54, 0xFC),
    LIME                 = EGERGB(0x00, 0xFF, 0x00),
    LIMEGREEN            = EGERGB(0x32, 0xCD, 0x32),
    LINEN                = EGERGB(0xFA, 0xF0, 0xE6),
    MAGENTA              = EGERGB(0xFF, 0x00, 0xFF),
    MAROON               = EGERGB(0x80, 0x00, 0x00),
    MEDIUMAQUAMARINE     = EGERGB(0x66, 0xCD, 0xAA),
    MEDIUMBLUE           = EGERGB(0x00, 0x00, 0xCD),
    MEDIUMORCHID         = EGERGB(0xBA, 0x55, 0xD3),
    MEDIUMPURPLE         = EGERGB(0x93, 0x70, 0xDB),
    MEDIUMSEAGREEN       = EGERGB(0x3C, 0xB3, 0x71),
    MEDIUMSLATEBLUE      = EGERGB(0x7B, 0x68, 0xEE),
    MEDIUMSPRINGGREEN    = EGERGB(0x00, 0xFA, 0x9A),
    MEDIUMTURQUOISE      = EGERGB(0x48, 0xD1, 0xCC),
    MEDIUMVIOLETRED      = EGERGB(0xC7, 0x15, 0x85),
    MIDNIGHTBLUE         = EGERGB(0x19, 0x19, 0x70),
    MINTCREAM            = EGERGB(0xF5, 0xFF, 0xFA),
    MISTYROSE            = EGERGB(0xFF, 0xE4, 0xE1),
    MOCCASIN             = EGERGB(0xFF, 0xE4, 0xB5),
    NAVAJOWHITE          = EGERGB(0xFF, 0xDE, 0xAD),
    NAVY                 = EGERGB(0x00, 0x00, 0x80),
    OLDLACE              = EGERGB(0xFD, 0xF5, 0xE6),
    OLIVE                = EGERGB(0x80, 0x80, 0x00),
    OLIVEDRAB            = EGERGB(0x6B, 0x8E, 0x23),
    ORANGE               = EGERGB(0xFF, 0xA5, 0x00),
    ORANGERED            = EGERGB(0xFF, 0x45, 0x00),
    ORCHID               = EGERGB(0xDA, 0x70, 0xD6),
    PALEGOLDENROD        = EGERGB(0xEE, 0xE8, 0xAA),
    PALEGREEN            = EGERGB(0x98, 0xFB, 0x98),
    PALETURQUOISE        = EGERGB(0xAF, 0xEE, 0xEE),
    PALEVIOLETRED        = EGERGB(0xDB, 0x70, 0x93),
    PAPAYAWHIP           = EGERGB(0xFF, 0xEF, 0xD5),
    PEACHPUFF            = EGERGB(0xFF, 0xDA, 0xB9),
    PERU                 = EGERGB(0xCD, 0x85, 0x3F),
    PINK                 = EGERGB(0xFF, 0xC0, 0xCB),
    PLUM                 = EGERGB(0xDD, 0xA0, 0xDD),
    POWDERBLUE           = EGERGB(0xB0, 0xE0, 0xE6),
    PURPLE               = EGERGB(0x80, 0x00, 0x80),
    RED                  = EGERGB(0xFF, 0x00, 0x00),
    ROSYBROWN            = EGERGB(0xBC, 0x8F, 0x8F),
    ROYALBLUE            = EGERGB(0x41, 0x69, 0xE1),
    SADDLEBROWN          = EGERGB(0x8B, 0x45, 0x13),
    SALMON               = EGERGB(0xFA, 0x80, 0x72),
    SANDYBROWN           = EGERGB(0xF4, 0xA4, 0x60),
    SEAGREEN             = EGERGB(0x2E, 0x8B, 0x57),
    SEASHELL             = EGERGB(0xFF, 0xF5, 0xEE),
    SIENNA               = EGERGB(0xA0, 0x52, 0x2D),
    SILVER               = EGERGB(0xC0, 0xC0, 0xC0),
    SKYBLUE              = EGERGB(0x87, 0xCE, 0xEB),
    SLATEBLUE            = EGERGB(0x6A, 0x5A, 0xCD),
    SLATEGRAY            = EGERGB(0x70, 0x80, 0x90),
    SNOW                 = EGERGB(0xFF, 0xFA, 0xFA),
    SPRINGGREEN          = EGERGB(0x00, 0xFF, 0x7F),
    STEELBLUE            = EGERGB(0x46, 0x82, 0xB4),
    TAN                  = EGERGB(0xD2, 0xB4, 0x8C),
    TEAL                 = EGERGB(0x00, 0x80, 0x80),
    THISTLE              = EGERGB(0xD8, 0xBF, 0xD8),
    TOMATO               = EGERGB(0xFF, 0x63, 0x47),
    TURQUOISE            = EGERGB(0x40, 0xE0, 0xD0),
    VIOLET               = EGERGB(0xEE, 0x82, 0xEE),
    WHEAT                = EGERGB(0xF5, 0xDE, 0xB3),
    WHITE                = EGERGB(0xFF, 0xFF, 0xFF),
    WHITESMOKE           = EGERGB(0xF5, 0xF5, 0xF5),
    YELLOW               = EGERGB(0xFF, 0xFF, 0x00),
    YELLOWGREEN          = EGERGB(0x9A, 0xCD, 0x32)
};

/**
 * @enum line_styles
 * @brief 绾挎潯鏍峰紡
 * 
 * 瀹氫箟浜嗙粯鍒剁嚎鏉℃椂鍙互浣跨敤鐨勪笉鍚屾牱寮?
 */
enum line_styles
{
    SOLID_LINE           = PS_SOLID,        ///< 瀹炵嚎
    CENTER_LINE          = PS_DASH,         ///< 涓績绾匡紙铏氱嚎锛?
    DOTTED_LINE          = PS_DOT,          ///< 鐐圭嚎
    DASHED_LINE          = PS_DASHDOT,      ///< 鐐瑰垝绾?
    NULL_LINE            = PS_NULL,         ///< 绌虹嚎锛堜笉缁樺埗锛?
    USERBIT_LINE         = PS_USERSTYLE     ///< 鐢ㄦ埛鑷畾涔夌嚎鏉℃牱寮?
};

/**
 * @struct line_style_type
 * @brief 绾挎潯鏍峰紡缁撴瀯
 * 
 * 鎻忚堪绾挎潯鐨勮缁嗘牱寮忓睘鎬?
 */
struct line_style_type
{
    int             linestyle;  ///< 绾挎潯鏍峰紡
    unsigned short  upattern;   ///< 鐢ㄦ埛瀹氫箟鐨勭嚎鏉℃ā寮?
    int             thickness;  ///< 绾挎潯绮楃粏
};

/**
 * @enum line_cap_type
 * @brief 绾挎潯绔偣鏍峰紡
 * 
 * 瀹氫箟浜嗙嚎鏉′袱绔殑缁樺埗鏍峰紡
 */
enum line_cap_type
{
    LINECAP_FLAT   = 0,     ///< 骞崇洿绔偣
    LINECAP_SQUARE,         ///< 鏂瑰舰绔偣  
    LINECAP_ROUND           ///< 鍦嗗舰绔偣
};

/**
 * @enum line_join_type
 * @brief 绾挎潯杩炴帴鏍峰紡
 * 
 * 瀹氫箟浜嗗鏉＄嚎杩炴帴澶勭殑缁樺埗鏍峰紡
 */
enum line_join_type
{
    LINEJOIN_MITER = 0,     ///< 灏栬杩炴帴
    LINEJOIN_BEVEL,         ///< 鏂滆杩炴帴
    LINEJOIN_ROUND          ///< 鍦嗚杩炴帴
};

/**
 * @enum fill_patterns
 * @brief 濉厖鍥炬
 * 
 * 瀹氫箟浜嗗嚑浣曞浘褰㈠～鍏呮椂鍙互浣跨敤鐨勪笉鍚屽浘妗堟牱寮?
 */
enum fill_patterns
{
    EMPTY_FILL,         ///< 鏃犲～鍏?
    SOLID_FILL,         ///< 瀹炲績濉厖锛堜娇鐢ㄥ～鍏呰壊濉厖锛?
    LINE_FILL,          ///< 姘村钩绾垮～鍏?---
    LTSLASH_FILL,       ///< 缁嗘枩绾垮～鍏?"///"
    SLASH_FILL,         ///< 绮楁枩绾垮～鍏?"///"
    BKSLASH_FILL,       ///< 绮楀弽鏂滅嚎濉厖 "\\\"
    LTBKSLASH_FILL,     ///< 缁嗗弽鏂滅嚎濉厖 "\\\"
    HATCH_FILL,         ///< 娴呯綉鏍煎～鍏?
    XHATCH_FILL,        ///< 娣变氦鍙夌綉鏍煎～鍏?
    INTERLEAVE_FILL,    ///< 浜ら敊绾垮～鍏?
    WIDE_DOT_FILL,      ///< 绋€鐤忕偣濉厖
    CLOSE_DOT_FILL,     ///< 瀵嗛泦鐐瑰～鍏?
    USER_FILL           ///< 鐢ㄦ埛鑷畾涔夊～鍏?
};

/**
 * @enum fill_mode
 * @brief 濉厖妯″紡
 * 
 * 瀹氫箟浜嗗鏉傚浘褰㈢殑濉厖绠楁硶
 */
enum fill_mode
{
    FILLMODE_DEFAULT   = 0,     ///< 榛樿濉厖妯″紡
    FILLMODE_ALTERNATE = 1,     ///< 浜ゆ浛濉厖妯″紡
    FILLMODE_WINDING   = 2      ///< 鍥炲嵎濉厖妯″紡
};

/**
 * @enum text_just
 * @brief 鏂囨湰瀵归綈鏂瑰紡
 * 
 * 瀹氫箟浜嗘枃鏈殑姘村钩鍜屽瀭鐩村榻愭柟寮?
 */
enum text_just
{
    LEFT_TEXT            = 0,   ///< 宸﹀榻?
    CENTER_TEXT          = 1,   ///< 灞呬腑瀵归綈
    RIGHT_TEXT           = 2,   ///< 鍙冲榻?

    TOP_TEXT             = 0,   ///< 涓婂榻?
/*  CENTER_TEXT          = 1,     宸插湪涓婇潰瀹氫箟 */
    BOTTOM_TEXT          = 2    ///< 涓嬪榻?
};

/**
 * @struct textsettingstype
 * @brief 鏂囨湰璁剧疆缁撴瀯
 * 
 * 鍖呭惈鏂囨湰鐨勫瓧浣撱€佹柟鍚戙€佸ぇ灏忓拰瀵归綈鏂瑰紡绛夎缃?
 */
struct textsettingstype
{
    int font;       ///< 瀛椾綋绫诲瀷
    int direction;  ///< 鏂囧瓧鏂瑰悜
    int charsize;   ///< 瀛楃澶у皬
    int horiz;      ///< 姘村钩瀵归綈鏂瑰紡
    int vert;       ///< 鍨傜洿瀵归綈鏂瑰紡
};

/**
 * @enum font_styles
 * @brief 瀛椾綋鏍峰紡
 * 
 * 瀹氫箟浜嗗瓧浣撶殑鍚勭鏍峰紡锛屽彲浠ラ€氳繃浣嶆垨鎿嶄綔缁勫悎浣跨敤
 */
enum font_styles
{
    FONTSTYLE_BOLD       = 1,   ///< 绮椾綋
    FONTSTYLE_ITALIC     = 2,   ///< 鏂滀綋  
    FONTSTYLE_UNDERLINE  = 4,   ///< 涓嬪垝绾?
    FONTSTYLE_STRIKEOUT  = 8    ///< 鍒犻櫎绾?
};

/**
 * @enum music_state_flag
 * @brief 闊充箰鎾斁鐘舵€佹爣蹇?
 * 
 * 瀹氫箟浜嗛煶涔愭挱鏀惧櫒鐨勫悇绉嶇姸鎬?
 */
enum music_state_flag
{
    MUSIC_MODE_NOT_OPEN  = 0x0,     ///< 鏈墦寮€鐘舵€?
    MUSIC_MODE_NOT_READY = 0x20C,   ///< 鏈氨缁姸鎬?
    MUSIC_MODE_PAUSE     = 0x211,   ///< 鏆傚仠鐘舵€?
    MUSIC_MODE_PLAY      = 0x20E,   ///< 鎾斁鐘舵€?
    MUSIC_MODE_STOP      = 0x20D,   ///< 鍋滄鐘舵€?
    MUSIC_MODE_OPEN      = 0x212,   ///< 宸叉墦寮€鐘舵€?
    MUSIC_MODE_SEEK      = 0x210    ///< 瀹氫綅鐘舵€?
};

/// @brief 闊充箰鎿嶄綔閿欒浠ｇ爜
#define MUSIC_ERROR  0xFFFFFFFF

/**
 * @enum key_msg_flag
 * @brief 鎸夐敭娑堟伅鏍囧織
 * 
 * 瀹氫箟浜嗘寜閿秷鎭殑绫诲瀷鍜岀姸鎬佹爣蹇?
 */
enum key_msg_flag
{
    KEYMSG_CHAR_FLAG     = 2,       ///< 瀛楃娑堟伅鏍囧織
    KEYMSG_DOWN_FLAG     = 1,       ///< 鎸変笅娑堟伅鏍囧織
    KEYMSG_UP_FLAG       = 1,       ///< 閲婃斁娑堟伅鏍囧織

    KEYMSG_CHAR          = 0x40000, ///< 瀛楃娑堟伅
    KEYMSG_DOWN          = 0x10000, ///< 鎸夐敭鎸変笅娑堟伅
    KEYMSG_UP            = 0x20000, ///< 鎸夐敭閲婃斁娑堟伅
    KEYMSG_FIRSTDOWN     = 0x80000  ///< 棣栨鎸変笅娑堟伅
};

/**
 * @enum key_code_e
 * @brief 閿洏鍜岄紶鏍囬敭鐮?
 * 
 * 瀹氫箟浜嗘墍鏈夊彲浠ユ娴嬬殑閿洏鎸夐敭鍜岄紶鏍囨寜閽殑閿爜鍊?
 * 閿爜鍊煎熀浜嶹indows铏氭嫙閿爜(Virtual Key Codes)
 */
enum key_code_e
{
    // 榧犳爣鎸夐挳
    key_mouse_l         = 0x01,     ///< 榧犳爣宸﹂敭
    key_mouse_r         = 0x02,     ///< 榧犳爣鍙抽敭
    key_mouse_m         = 0x04,     ///< 榧犳爣涓敭
    key_mouse_x1        = 0x05,     ///< 榧犳爣X1閿?
    key_mouse_x2        = 0x06,     ///< 榧犳爣X2閿?
    
    // 鐗规畩鍔熻兘閿?
    key_back            = 0x08,     ///< 閫€鏍奸敭 (Backspace)
    key_tab             = 0x09,     ///< 鍒惰〃閿?(Tab)
    key_enter           = 0x0d,     ///< 鍥炶溅閿?(Enter)
    key_shift           = 0x10,     ///< Shift閿?
    key_control         = 0x11,     ///< Ctrl閿?
    key_menu            = 0x12,     ///< Alt閿?
    key_pause           = 0x13,     ///< 鏆傚仠閿?(Pause)
    key_capslock        = 0x14,     ///< 澶у啓閿佸畾閿?(Caps Lock)
    key_esc             = 0x1b,     ///< 閫冮€搁敭 (Escape)
    key_space           = 0x20,     ///< 绌烘牸閿?(Space)

    // 瀵艰埅閿?
    key_pageup          = 0x21,     ///< 鍚戜笂缈婚〉閿?(Page Up)
    key_pagedown        = 0x22,     ///< 鍚戜笅缈婚〉閿?(Page Down)
    key_end             = 0x23,     ///< 缁撴潫閿?(End)
    key_home            = 0x24,     ///< 涓婚〉閿?(Home)

    // 鏂瑰悜閿?
    key_left            = 0x25,     ///< 宸︽柟鍚戦敭
    key_up              = 0x26,     ///< 涓婃柟鍚戦敭
    key_right           = 0x27,     ///< 鍙虫柟鍚戦敭
    key_down            = 0x28,     ///< 涓嬫柟鍚戦敭

    // 缂栬緫閿?
    key_print           = 0x2a,     ///< 鎵撳嵃閿?(Print)
    key_snapshot        = 0x2c,     ///< 鎴浘閿?(Print Screen)
    key_insert          = 0x2d,     ///< 鎻掑叆閿?(Insert)
    key_delete          = 0x2e,     ///< 鍒犻櫎閿?(Delete)

    // 鏁板瓧閿?(涓婚敭鐩樺尯)
    key_0               = 0x30,     ///< 鏁板瓧閿?
    key_1               = 0x31,     ///< 鏁板瓧閿?
    key_2               = 0x32,     ///< 鏁板瓧閿?
    key_3               = 0x33,     ///< 鏁板瓧閿?
    key_4               = 0x34,     ///< 鏁板瓧閿?
    key_5               = 0x35,     ///< 鏁板瓧閿?
    key_6               = 0x36,     ///< 鏁板瓧閿?
    key_7               = 0x37,     ///< 鏁板瓧閿?
    key_8               = 0x38,     ///< 鏁板瓧閿?
    key_9               = 0x39,     ///< 鏁板瓧閿?

    // 瀛楁瘝閿?
    key_A               = 0x41,     ///< 瀛楁瘝閿瓵
    key_B               = 0x42,     ///< 瀛楁瘝閿瓸
    key_C               = 0x43,     ///< 瀛楁瘝閿瓹
    key_D               = 0x44,     ///< 瀛楁瘝閿瓺
    key_E               = 0x45,     ///< 瀛楁瘝閿瓻
    key_F               = 0x46,     ///< 瀛楁瘝閿瓼
    key_G               = 0x47,     ///< 瀛楁瘝閿瓽
    key_H               = 0x48,     ///< 瀛楁瘝閿瓾
    key_I               = 0x49,     ///< 瀛楁瘝閿甀
    key_J               = 0x4a,     ///< 瀛楁瘝閿甁
    key_K               = 0x4b,     ///< 瀛楁瘝閿甂
    key_L               = 0x4c,     ///< 瀛楁瘝閿甃
    key_M               = 0x4d,     ///< 瀛楁瘝閿甅
    key_N               = 0x4e,     ///< 瀛楁瘝閿甆
    key_O               = 0x4f,     ///< 瀛楁瘝閿甇
    key_P               = 0x50,     ///< 瀛楁瘝閿甈
    key_Q               = 0x51,     ///< 瀛楁瘝閿甉
    key_R               = 0x52,     ///< 瀛楁瘝閿甊
    key_S               = 0x53,     ///< 瀛楁瘝閿甋
    key_T               = 0x54,     ///< 瀛楁瘝閿甌
    key_U               = 0x55,     ///< 瀛楁瘝閿甎
    key_V               = 0x56,     ///< 瀛楁瘝閿甐
    key_W               = 0x57,     ///< 瀛楁瘝閿甒
    key_X               = 0x58,     ///< 瀛楁瘝閿甔
    key_Y               = 0x59,     ///< 瀛楁瘝閿甕
    key_Z               = 0x5a,     ///< 瀛楁瘝閿甖
    
    // Windows閿?
    key_win_l           = 0x5b,     ///< 宸indows閿?
    key_win_r           = 0x5c,     ///< 鍙砏indows閿?

    key_sleep           = 0x5f,     ///< 浼戠湢閿?

    // 鏁板瓧閿洏
    key_num0            = 0x60,     ///< 鏁板瓧閿洏0
    key_num1            = 0x61,     ///< 鏁板瓧閿洏1
    key_num2            = 0x62,     ///< 鏁板瓧閿洏2
    key_num3            = 0x63,     ///< 鏁板瓧閿洏3
    key_num4            = 0x64,     ///< 鏁板瓧閿洏4
    key_num5            = 0x65,     ///< 鏁板瓧閿洏5
    key_num6            = 0x66,     ///< 鏁板瓧閿洏6
    key_num7            = 0x67,     ///< 鏁板瓧閿洏7
    key_num8            = 0x68,     ///< 鏁板瓧閿洏8
    key_num9            = 0x69,     ///< 鏁板瓧閿洏9

    // 鏁板瓧閿洏杩愮畻绗?
    key_multiply        = 0x6a,     ///< 鏁板瓧閿洏涔樺彿 (*)
    key_add             = 0x6b,     ///< 鏁板瓧閿洏鍔犲彿 (+)
    key_separator       = 0x6c,     ///< 鏁板瓧閿洏鍒嗛殧绗?
    key_subtract        = 0x6d,     ///< 鏁板瓧閿洏鍑忓彿 (-)
    key_decimal         = 0x6e,     ///< 鏁板瓧閿洏灏忔暟鐐?(.)
    key_divide          = 0x6f,     ///< 鏁板瓧閿洏闄ゅ彿 (/)

    // 鍔熻兘閿?
    key_f1              = 0x70,     ///< F1鍔熻兘閿?
    key_f2              = 0x71,     ///< F2鍔熻兘閿?
    key_f3              = 0x72,     ///< F3鍔熻兘閿?
    key_f4              = 0x73,     ///< F4鍔熻兘閿?
    key_f5              = 0x74,     ///< F5鍔熻兘閿?
    key_f6              = 0x75,     ///< F6鍔熻兘閿?
    key_f7              = 0x76,     ///< F7鍔熻兘閿?
    key_f8              = 0x77,     ///< F8鍔熻兘閿?
    key_f9              = 0x78,     ///< F9鍔熻兘閿?
    key_f10             = 0x79,     ///< F10鍔熻兘閿?
    key_f11             = 0x7a,     ///< F11鍔熻兘閿?
    key_f12             = 0x7b,     ///< F12鍔熻兘閿?

    // 閿佸畾閿?
    key_numlock         = 0x90,     ///< 鏁板瓧閿佸畾閿?(Num Lock)
    key_scrolllock      = 0x91,     ///< 婊氬姩閿佸畾閿?(Scroll Lock)

    // 宸﹀彸鍖哄垎鐨勪慨楗伴敭
    key_shift_l         = 0xa0,     ///< 宸hift閿?
    key_shift_r         = 0xa1,     ///< 鍙砈hift閿?
    key_control_l       = 0xa2,     ///< 宸trl閿?
    key_control_r       = 0xa3,     ///< 鍙矯trl閿?
    key_menu_l          = 0xa4,     ///< 宸lt閿?
    key_menu_r          = 0xa5,     ///< 鍙矨lt閿?

    // 鏍囩偣绗﹀彿閿?
    key_semicolon       = 0xba,     ///< 鍒嗗彿閿?(;)
    key_plus            = 0xbb,     ///< 绛夊彿/鍔犲彿閿?(=)
    key_comma           = 0xbc,     ///< 閫楀彿閿?(,)
    key_minus           = 0xbd,     ///< 鍑忓彿/涓嬪垝绾块敭 (-)
    key_period          = 0xbe,     ///< 鍙ュ彿閿?(.)
    key_slash           = 0xbf,     ///< 鏂滄潬閿?(/)
    key_tilde           = 0xc0,     ///< 娉㈡氮鍙烽敭 (~)
    key_lbrace          = 0xdb,     ///< 宸︽柟鎷彿閿?([)
    key_backslash       = 0xdc,     ///< 鍙嶆枩鏉犻敭 (\)
    key_rbrace          = 0xdd,     ///< 鍙虫柟鎷彿閿?(])
    key_quote           = 0xde,     ///< 寮曞彿閿?(')

    key_ime_process     = 0xe5      ///< 杈撳叆娉曞鐞嗛敭
};

/**
 * @enum key_msg_e
 * @brief 鎸夐敭娑堟伅绫诲瀷
 * 
 * 瀹氫箟浜嗘寜閿簨浠剁殑鍏蜂綋绫诲瀷
 */
enum key_msg_e
{
    key_msg_down        = 1,    ///< 鎸夐敭鎸変笅娑堟伅
    key_msg_up          = 2,    ///< 鎸夐敭閲婃斁娑堟伅
    key_msg_char        = 4     ///< 瀛楃杈撳叆娑堟伅
};

/**
 * @enum key_flag_e
 * @brief 鎸夐敭鐘舵€佹爣蹇?
 * 
 * 瀹氫箟浜嗘寜閿簨浠剁殑淇グ閿姸鎬佸拰鐗规畩鏍囧織
 */
enum key_flag_e
{
    key_flag_shift      = 0x100,    ///< Shift閿鎸変笅
    key_flag_ctrl       = 0x200,    ///< Ctrl閿鎸変笅
    key_flag_first_down = 0x80000   ///< 棣栨鎸変笅鏍囧織
};

/**
 * @struct key_msg
 * @brief 鎸夐敭娑堟伅缁撴瀯
 * 
 * 鍖呭惈瀹屾暣鐨勬寜閿簨浠朵俊鎭?
 */
struct key_msg
{
    int             key;    ///< 鎸夐敭閿爜
    key_msg_e       msg;    ///< 娑堟伅绫诲瀷
    unsigned int    flags;  ///< 鐘舵€佹爣蹇?
};

/**
 * @enum mouse_msg_e
 * @brief 榧犳爣娑堟伅绫诲瀷
 * 
 * 瀹氫箟浜嗛紶鏍囦簨浠剁殑鍏蜂綋绫诲瀷
 */
enum mouse_msg_e
{
    mouse_msg_down      = 0x10,     ///< 榧犳爣鎸夐挳鎸変笅娑堟伅
    mouse_msg_up        = 0x20,     ///< 榧犳爣鎸夐挳閲婃斁娑堟伅
    mouse_msg_move      = 0x40,     ///< 榧犳爣绉诲姩娑堟伅
    mouse_msg_wheel     = 0x80      ///< 榧犳爣婊氳疆娑堟伅
};

/**
 * @enum mouse_flag_e
 * @brief 榧犳爣鐘舵€佹爣蹇?
 * 
 * 瀹氫箟浜嗛紶鏍囦簨浠朵腑鍚勬寜閽拰淇グ閿殑鐘舵€?
 */
enum mouse_flag_e
{
    mouse_flag_left     = 0x001,    ///< 榧犳爣宸﹂敭琚寜涓?
    mouse_flag_right    = 0x002,    ///< 榧犳爣鍙抽敭琚寜涓?
    mouse_flag_mid      = 0x004,    ///< 榧犳爣涓敭琚寜涓?
    mouse_flag_x1       = 0x008,    ///< 榧犳爣X1閿鎸変笅
    mouse_flag_x2       = 0x010,    ///< 榧犳爣X2閿鎸変笅
    mouse_flag_shift    = 0x100,    ///< Shift閿鎸変笅
    mouse_flag_ctrl     = 0x200     ///< Ctrl閿鎸変笅
};

/**
 * @struct mouse_msg
 * @brief 榧犳爣娑堟伅缁撴瀯
 * 
 * 鍖呭惈瀹屾暣鐨勯紶鏍囦簨浠朵俊鎭紝鎻愪緵浜嗕究鎹风殑鐘舵€佹煡璇㈡柟娉?
 */
struct mouse_msg
{
    int             x;      ///< 榧犳爣x鍧愭爣
    int             y;      ///< 榧犳爣y鍧愭爣
    mouse_msg_e     msg;    ///< 娑堟伅绫诲瀷
    unsigned int    flags;  ///< 鐘舵€佹爣蹇?
    int             wheel;  ///< 婊氳疆婊氬姩澧為噺

    /// @brief 妫€鏌ユ槸鍚︿负榧犳爣宸﹂敭浜嬩欢
    bool is_left()  const {return (flags & mouse_flag_left)  != 0;}
    /// @brief 妫€鏌ユ槸鍚︿负榧犳爣鍙抽敭浜嬩欢
    bool is_right() const {return (flags & mouse_flag_right) != 0;}
    /// @brief 妫€鏌ユ槸鍚︿负榧犳爣涓敭浜嬩欢
    bool is_mid()   const {return (flags & mouse_flag_mid)   != 0;}
    /// @brief 妫€鏌ユ槸鍚︿负榧犳爣X1閿簨浠?
    bool is_x1()    const {return (flags & mouse_flag_x1)    != 0;}
    /// @brief 妫€鏌ユ槸鍚︿负榧犳爣X2閿簨浠?
    bool is_x2()    const {return (flags & mouse_flag_x2)    != 0;}

    /// @brief 妫€鏌ユ槸鍚︿负鎸夐挳鎸変笅浜嬩欢
    bool is_down()  const {return msg == mouse_msg_down; }
    /// @brief 妫€鏌ユ槸鍚︿负鎸夐挳閲婃斁浜嬩欢
    bool is_up()    const {return msg == mouse_msg_up;   }
    /// @brief 妫€鏌ユ槸鍚︿负榧犳爣绉诲姩浜嬩欢
    bool is_move()  const {return msg == mouse_msg_move; }
    /// @brief 妫€鏌ユ槸鍚︿负婊氳疆浜嬩欢
    bool is_wheel() const {return msg == mouse_msg_wheel;}
};

/**
 * @struct MOUSEMSG
 * @brief 浼犵粺榧犳爣娑堟伅缁撴瀯锛堝吋瀹规€э級
 * 
 * 鎻愪緵涓庢棫鐗堟湰鍏煎鐨勯紶鏍囨秷鎭牸寮?
 */
struct MOUSEMSG
{
    UINT  uMsg;         ///< Windows娑堟伅ID
    bool  mkCtrl;       ///< Ctrl閿姸鎬?
    bool  mkShift;      ///< Shift閿姸鎬?
    bool  mkLButton;    ///< 宸﹂敭鐘舵€?
    bool  mkMButton;    ///< 涓敭鐘舵€?
    bool  mkRButton;    ///< 鍙抽敭鐘舵€?
    bool  mkXButton1;   ///< X1閿姸鎬?
    bool  mkXButton2;   ///< X2閿姸鎬?
    short x;            ///< x鍧愭爣
    short y;            ///< y鍧愭爣
    short wheel;        ///< 婊氳疆澧為噺
};

/**
 * @struct viewporttype
 * @brief 瑙嗗彛绫诲瀷缁撴瀯
 * 
 * 瀹氫箟浜嗙粯鍥捐鍙ｇ殑杈圭晫鐭╁舰
 */
/**
 * @struct viewporttype
 * @brief 瑙嗗彛绫诲瀷缁撴瀯
 * 
 * 瀹氫箟浜嗙粯鍥捐鍙ｇ殑杈圭晫鐭╁舰
 */
struct viewporttype
{
    int left;       ///< 宸﹁竟鐣?
    int top;        ///< 涓婅竟鐣?
    int right;      ///< 鍙宠竟鐣?
    int bottom;     ///< 涓嬭竟鐣?
    int clipflag;   ///< 瑁佸壀鏍囧織
};

/**
 * @struct ege_transform_matrix
 * @brief 2D鍙樻崲鐭╅樀
 * 
 * 鐢ㄤ簬2D鍥惧舰鍙樻崲鐨?x2鐭╅樀锛屾敮鎸佸钩绉汇€佹棆杞€佺缉鏀剧瓑鍙樻崲
 */
struct ege_transform_matrix
{
    float m11, m12;     ///< 绗竴琛岋細[m11, m12]
    float m21, m22;     ///< 绗簩琛岋細[m21, m22]
    float m31, m32;     ///< 绗笁琛岋細[m31, m32] 骞崇Щ鍒嗛噺
};

/**
 * @struct ege_path
 * @brief 鍥惧舰璺緞
 * 
 * 鐢ㄤ簬瀹氫箟澶嶆潅鐨勫浘褰㈣矾寰勶紝鏀寔鐩寸嚎銆佹洸绾跨瓑鍥惧舰鍏冪礌鐨勭粍鍚?
 */
struct ege_path
{
private:
    void* m_data;       ///< 鍐呴儴鏁版嵁鎸囬拡

public:
    /// @brief 榛樿鏋勯€犲嚱鏁?
    ege_path();
    
    /// @brief 浠庣偣鏁扮粍鍜岀被鍨嬫暟缁勬瀯閫犺矾寰?
    /// @param points 鐐规暟缁?
    /// @param types 璺緞绫诲瀷鏁扮粍
    /// @param count 鐐圭殑鏁伴噺
    ege_path(const ege_point* points, const unsigned char* types, int count);
    
    /// @brief 鎷疯礉鏋勯€犲嚱鏁?
    /// @param path 瑕佹嫹璐濈殑璺緞
    ege_path(const ege_path& path);
    
    /// @brief 鏋愭瀯鍑芥暟
    virtual ~ege_path();

    /// @brief 鑾峰彇鍙鏁版嵁鎸囬拡
    /// @return 甯搁噺鏁版嵁鎸囬拡
    const void* data() const;
    
    /// @brief 鑾峰彇鍙啓鏁版嵁鎸囬拡
    /// @return 鏁版嵁鎸囬拡
    void* data();
    
    /// @brief 璧嬪€兼搷浣滅
    /// @param path 瑕佽祴鍊肩殑璺緞
    /// @return 璺緞寮曠敤
    ege_path& operator=(const ege_path& path);
};

/**
 * @struct msg_createwindow
 * @brief 鍒涘缓绐楀彛娑堟伅缁撴瀯
 * 
 * 鐢ㄤ簬绐楀彛鍒涘缓鏃朵紶閫掑弬鏁扮殑娑堟伅缁撴瀯
 */
struct msg_createwindow
{
    HANDLE  hEvent;         ///< 浜嬩欢鍙ユ焺
    HWND    hwnd;           ///< 绐楀彛鍙ユ焺
    const wchar_t* classname; ///< 绐楀彛绫诲悕
    DWORD   style;          ///< 绐楀彛鏍峰紡
    DWORD   exstyle;        ///< 鎵╁睍绐楀彛鏍峰紡
    size_t  id;             ///< 绐楀彛ID
    LPVOID  param;          ///< 鍙傛暟鎸囬拡
};

/// @brief 閫氱敤鍥炶皟鍑芥暟绫诲瀷
typedef void (CALLBACK_PROC)();

/// @brief 閿洏娑堟伅澶勭悊鍥炶皟鍑芥暟绫诲瀷
/// @param userdata 鐢ㄦ埛鏁版嵁鎸囬拡
/// @param message 娑堟伅绫诲瀷
/// @param key 閿爜
/// @return 澶勭悊缁撴灉
typedef int (__stdcall MSG_KEY_PROC  )(void*, unsigned, int);

/// @brief 榧犳爣娑堟伅澶勭悊鍥炶皟鍑芥暟绫诲瀷
/// @param userdata 鐢ㄦ埛鏁版嵁鎸囬拡
/// @param message 娑堟伅绫诲瀷
/// @param x x鍧愭爣
/// @param y y鍧愭爣
/// @param flags 鏍囧織浣?
/// @return 澶勭悊缁撴灉
typedef int (__stdcall MSG_MOUSE_PROC)(void*, unsigned, int, int, int);

/// @brief 鍥炶皟鍑芥暟鎸囬拡绫诲瀷
typedef CALLBACK_PROC       * LPCALLBACK_PROC;
/// @brief 閿洏娑堟伅澶勭悊鍑芥暟鎸囬拡绫诲瀷
typedef MSG_KEY_PROC        * LPMSG_KEY_PROC;
/// @brief 榧犳爣娑堟伅澶勭悊鍑芥暟鎸囬拡绫诲瀷
typedef MSG_MOUSE_PROC      * LPMSG_MOUSE_PROC;

struct VECTOR3D;

/// @brief 缁昘杞存棆杞?D鐐?
/// @param point 瑕佹棆杞殑3D鐐规寚閽?
/// @param rad 鏃嬭浆瑙掑害锛堝姬搴︼級
void EGEAPI rotate_point3d_x(VECTOR3D* point, float rad);

/// @brief 缁昚杞存棆杞?D鐐?
/// @param point 瑕佹棆杞殑3D鐐规寚閽?
/// @param rad 鏃嬭浆瑙掑害锛堝姬搴︼級
void EGEAPI rotate_point3d_y(VECTOR3D* point, float rad);

/// @brief 缁昛杞存棆杞?D鐐?
/// @param point 瑕佹棆杞殑3D鐐规寚閽?
/// @param rad 鏃嬭浆瑙掑害锛堝姬搴︼級
void EGEAPI rotate_point3d_z(VECTOR3D* point, float rad);

/**
 * @struct VECTOR3D
 * @brief 3D鍚戦噺缁撴瀯
 * 
 * 琛ㄧず涓夌淮绌洪棿涓殑鍚戦噺鎴栫偣锛屾彁渚涗簡鍩烘湰鐨?D鍥惧舰杩愮畻鍔熻兘
 */
struct VECTOR3D
{
    float x, y, z;      ///< 涓夌淮鍧愭爣鍒嗛噺

    /// @brief 榛樿鏋勯€犲嚱鏁帮紝鍒濆鍖栦负鍘熺偣
    VECTOR3D() : x(0.0f), y(0.0f), z(0.0f) {}
    
    /// @brief 鏋勯€犲嚱鏁?
    /// @param x x鍧愭爣
    /// @param y y鍧愭爣
    /// @param z z鍧愭爣锛堥粯璁や负0锛?
    VECTOR3D(float x, float y, float z = 0.0f) : x(x), y(y), z(z) {}    /// @brief 璧嬪€兼搷浣滅
    /// @param vector 瑕佽祴鍊肩殑鍚戦噺
    /// @return 鍚戦噺寮曠敤
    VECTOR3D& operator=(const VECTOR3D& vector)
    {
        x = vector.x;
        y = vector.y;
        z = vector.z;
        return *this;
    }

    /// @brief 鍚戦噺鍔犳硶璧嬪€兼搷浣滅
    VECTOR3D& operator+=(const VECTOR3D& vector);
    /// @brief 鍚戦噺鍑忔硶璧嬪€兼搷浣滅
    VECTOR3D& operator-=(const VECTOR3D& vector);
    /// @brief 鍚戦噺鍔犳硶鎿嶄綔绗?
    VECTOR3D  operator+ (const VECTOR3D& vector) const;
    /// @brief 鍚戦噺鍑忔硶鎿嶄綔绗?
    VECTOR3D  operator- (const VECTOR3D& vector) const;
    /// @brief 鏍囬噺涔樻硶璧嬪€兼搷浣滅
    VECTOR3D& operator*=(float scale);
    /// @brief 鏍囬噺涔樻硶鎿嶄綔绗?
    VECTOR3D  operator* (float scale) const;
    /// @brief 鍚戦噺鐐圭Н鎿嶄綔绗?
    float     operator* (const VECTOR3D& vector) const;
    /// @brief 鍚戦噺鍙夌Н鎿嶄綔绗?
    VECTOR3D  operator& (const VECTOR3D& vector) const;
    /// @brief 鍚戦噺鍙夌Н璧嬪€兼搷浣滅
    VECTOR3D& operator&=(const VECTOR3D& vector);
    
    /// @brief 鑾峰彇鍚戦噺妯￠暱
    /// @return 鍚戦噺鐨勬ā闀?
    float     GetModule() const;

    /// @brief 鑾峰彇鍚戦噺妯￠暱鐨勫钩鏂?
    /// @return 鍚戦噺妯￠暱鐨勫钩鏂?
    float GetSqrModule() const { return float(x * x + y * y + z * z); }

    /// @brief 璁剧疆鍚戦噺妯￠暱
    /// @param m 鏂扮殑妯￠暱
    /// @return 鍚戦噺寮曠敤
    VECTOR3D& SetModule(float m)
    {
        float t  = m / GetModule();
        *this   *= t;
        return *this;
    }

    /// @brief 缁曟寚瀹氳酱鏃嬭浆鍚戦噺
    /// @param rad 鏃嬭浆瑙掑害锛堝姬搴︼級
    /// @param vector 鏃嬭浆杞村悜閲?
    /// @return 鍚戦噺寮曠敤
    VECTOR3D& Rotate(float rad, const VECTOR3D& vector);

    /// @brief 缁曟寚瀹氳酱鏃嬭浆鍚戦噺
    /// @param rad 鏃嬭浆瑙掑害锛堝姬搴︼級
    /// @param x 鏃嬭浆杞磝鍒嗛噺
    /// @param y 鏃嬭浆杞磞鍒嗛噺
    /// @param z 鏃嬭浆杞磟鍒嗛噺
    /// @return 鍚戦噺寮曠敤
    VECTOR3D& Rotate(float rad, float x, float y, float z)
    {
        VECTOR3D v(x, y, z);
        return Rotate(rad, v);
    }

    /// @brief 浠庤捣濮嬪悜閲忔棆杞埌鐩爣鍚戦噺
    /// @param e 鐩爣鍚戦噺
    /// @param s 璧峰鍚戦噺锛堥粯璁や负z杞存鏂瑰悜锛?
    /// @return 鍚戦噺寮曠敤
    VECTOR3D&    Rotate  (const VECTOR3D& e, const VECTOR3D& s = VECTOR3D(0.0f, 0.0f, 1.0f));
    
    /// @brief 璁＄畻涓や釜鍚戦噺涔嬮棿鐨勫す瑙?
    /// @param e 绗竴涓悜閲?
    /// @param s 绗簩涓悜閲忥紙榛樿涓簔杞存鏂瑰悜锛?
    /// @return 澶硅锛堝姬搴︼級
    static float GetAngle(const VECTOR3D& e, const VECTOR3D& s = VECTOR3D(0.0f, 0.0f, 1.0f));
};

/// @brief 鍥惧儚瀵硅薄鍓嶇疆澹版槑
class IMAGE;
/// @brief 鍥惧儚瀵硅薄鎸囬拡绫诲瀷
typedef IMAGE *PIMAGE;
/// @brief 甯搁噺鍥惧儚瀵硅薄鎸囬拡绫诲瀷
typedef const IMAGE *PCIMAGE;

/**
 * @brief 璁剧疆浠ｇ爜椤?
 * 
 * 璁剧疆瀛楃缂栫爜锛屽奖鍝嶆枃鏈鐞嗗拰鏄剧ず
 * @param codepage 浠ｇ爜椤碉紝搴斾娇鐢?EGE_CODEPAGE_XXX 甯搁噺锛岄粯璁や负 EGE_CODEPAGE_ANSI
 */
void EGEAPI setcodepage(unsigned int codepage);

/**
 * @brief 鑾峰彇褰撳墠浠ｇ爜椤?
 * 
 * @return 褰撳墠璁剧疆鐨勪唬鐮侀〉
 */
unsigned int EGEAPI getcodepage();

/**
 * @brief 璁剧疆鏄惁鍚敤Unicode瀛楃娑堟伅
 * 
 * 鎺у埗 getkey() 鍑芥暟鏄惁浣跨敤UTF-16缂栫爜鐨勫瓧绗︽秷鎭?
 * @param enable true鍚敤UTF-16锛宖alse浣跨敤ANSI
 */
void EGEAPI setunicodecharmessage(bool enable);

/**
 * @brief 鑾峰彇Unicode瀛楃娑堟伅璁剧疆鐘舵€?
 * 
 * @return true琛ㄧず鍚敤UTF-16锛宖alse琛ㄧず浣跨敤ANSI
 */
bool EGEAPI getunicodecharmessage();

/**
 * @brief 璁剧疆鍒濆鍖栨ā寮?
 * 
 * 璁剧疆绐楀彛鍒涘缓鏃剁殑榛樿鍙傛暟
 * @param mode 鍒濆鍖栨ā寮忔爣蹇?
 * @param x 绐楀彛鍒濆x鍧愭爣锛堥粯璁や负CW_USEDEFAULT锛?
 * @param y 绐楀彛鍒濆y鍧愭爣锛堥粯璁や负CW_USEDEFAULT锛?
 */
void EGEAPI setinitmode(initmode_flag mode, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
inline void setinitmode(int mode, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT)
{
    setinitmode(static_cast<initmode_flag>(mode), x, y);
}

/**
 * @brief 鑾峰彇褰撳墠鍒濆鍖栨ā寮?
 * 
 * @return 褰撳墠璁剧疆鐨勫垵濮嬪寲妯″紡鏍囧織
 */
initmode_flag  EGEAPI getinitmode();

/**
 * @brief 鍒涘缓 EGE 鍥惧舰鍖栫獥鍙ｏ紝骞惰繘琛岀幆澧冨垵濮嬪寲
 * 
 * 杩欐槸EGE鍥惧舰搴撶殑涓昏鍒濆鍖栧嚱鏁帮紝鎵ц鍚庝細鍒涘缓骞舵樉绀哄浘褰㈢獥鍙?
 * 
 * @param width 绐楀彛瀹藉害锛堝儚绱狅級
 * @param height 绐楀彛楂樺害锛堝儚绱狅級
 * @param mode 鍒濆鍖栨ā寮忔爣蹇楋紝鎺у埗绐楀彛鐨勫悇绉嶅睘鎬?
 * 
 * @code
 * // 鍒涘缓涓€涓?00x600鐨勯粯璁ょ獥鍙?
 * initgraph(800, 600, INIT_DEFAULT);
 * 
 * // 鍒涘缓涓€涓棤杈规鐨勭疆椤剁獥鍙?
 * initgraph(640, 480, INIT_NOBORDER | INIT_TOPMOST);
 * @endcode
 */
void EGEAPI initgraph(int width, int height, initmode_flag mode);
inline void initgraph(int width, int height, int mode)
{
    initgraph(width, height, static_cast<initmode_flag>(mode));
}

/**
 * @brief 鍒涘缓 EGE 鍥惧舰鍖栫獥鍙ｏ紙绠€鍖栫増鏈級
 * 
 * 浣跨敤褰撳墠璁剧疆鐨勫垵濮嬪寲妯″紡鍒涘缓鍥惧舰绐楀彛
 * 鍦ㄨ皟璇曠増鏈腑浣跨敤榛樿妯″紡锛屽湪鍙戝竷鐗堟湰涓樉绀篍GE鏍囧織
 * 
 * @param width 绐楀彛瀹藉害锛堝儚绱狅級
 * @param height 绐楀彛楂樺害锛堝儚绱狅級
 */
inline void EGEAPI initgraph(int width, int height)
{
#if !defined(NDEBUG) || defined(DEBUG) || defined(_DEBUG)
    initgraph(width, height, getinitmode());
#else
    initgraph(width, height, getinitmode() | INIT_WITHLOGO);
#endif
}

/**
 * @brief 鍒濆鍖栧浘褰㈢郴缁燂紙BGI鍏煎鐗堟湰锛?
 * 
 * 鎻愪緵涓庝紶缁烞GI鍥惧舰搴撶殑鍏煎鎺ュ彛
 * 
 * @param graphdriver 鍥惧舰椹卞姩绫诲瀷鎸囬拡锛岄€氬父浼犲叆DETECT浠ヨ嚜鍔ㄦ娴?
 * @param graphmode 鍥惧舰妯″紡鎸囬拡锛岄€氬父浼犲叆0浠ヨ嚜鍔ㄩ€夋嫨
 * @param pathtodriver BGI椹卞姩鏂囦欢璺緞锛岃嫢椹卞姩鏂囦欢鍦ㄥ綋鍓嶇洰褰曞彲浼犵┖瀛楃涓?"
 */
void initgraph(int *graphdriver, int *graphmode, const char *pathtodriver);

/**
 * @brief 鍏抽棴鍥惧舰绯荤粺
 * 
 * 閫昏緫涓婂叧闂浘褰㈢郴缁?
 * 鎵ц鍚嶦GE绐楀彛浼氶殣钘忥紝浣嗚祫婧愪笉浼氳鍏ㄩ儴閲婃斁锛宨s_run()鍑芥暟浠嶇劧杩斿洖true (闇€娉ㄦ剰)
 * 濡傛灉闇€瑕侀噴鏀?IMAGE 瀵硅薄璧勬簮锛屼粛闇€璋冪敤 delimage 鍑芥暟
 */
void EGEAPI closegraph();

/**
 * @brief 妫€鏌ュ浘褰㈢幆澧冩槸鍚︽鍦ㄨ繍琛?
 * 
 * @return true 褰揈GE鍥惧舰鐜瀛樺湪涓旂獥鍙ｆ湭琚叧闂椂
 * @return false 褰揈GE鍥惧舰鐜涓嶅瓨鍦ㄦ垨鐢ㄦ埛鐐瑰嚮鍏抽棴鎸夐挳鍚?
 */
bool EGEAPI is_run();

/**
 * @brief 璁剧疆绐楀彛鏍囬
 * @param caption 绐楀彛鏍囬瀛楃涓?
 */
void EGEAPI setcaption(const char* caption);

/**
 * @brief 璁剧疆绐楀彛鏍囬(Unicode鐗堟湰)
 * @param caption 绐楀彛鏍囬瀹藉瓧绗︿覆
 */
void EGEAPI setcaption(const wchar_t* caption);

/**
 * @brief 璁剧疆绐楀彛鍥炬爣
 * @param icon_id 鍥炬爣璧勬簮ID
 */
void EGEAPI seticon(int icon_id);

/**
 * @brief 闄勫姞鍒板凡鏈夌殑绐楀彛鍙ユ焺
 * @param hWnd 瑕侀檮鍔犵殑绐楀彛鍙ユ焺
 * @return 鎿嶄綔缁撴灉浠ｇ爜
 */
int  EGEAPI attachHWND(HWND hWnd);

/**
 * @brief 鏄剧ず绐楀彛
 */
void EGEAPI showwindow();

/**
 * @brief 闅愯棌绐楀彛
 */
void EGEAPI hidewindow();

/**
 * @brief 绉诲姩绐楀彛鍒版寚瀹氫綅缃?
 * @param x 鏂扮殑x鍧愭爣
 * @param y 鏂扮殑y鍧愭爣
 * @param redraw 鏄惁閲嶇粯绐楀彛锛堥粯璁や负true锛?
 */
void EGEAPI movewindow(int x, int y, bool redraw = true);

/**
 * @brief 璋冩暣绐楀彛澶у皬
 * @param width 鏂扮殑绐楀彛瀹藉害
 * @param height 鏂扮殑绐楀彛楂樺害
 */
void EGEAPI resizewindow(int width, int height);

/**
 * @brief 鍒锋柊绐楀彛鏄剧ず
 */
void EGEAPI flushwindow();

/**
 * @brief 璁剧疆娓叉煋妯″紡
 * @param mode 娓叉煋妯″紡锛堣嚜鍔ㄦ垨鎵嬪姩锛?
 */
void EGEAPI setrendermode(rendermode_e mode);

/**
 * @brief 鑾峰彇褰撳墠缁樺浘鐩爣
 * @return 褰撳墠缁樺浘鐩爣鍥惧儚鎸囬拡锛孨ULL琛ㄧず灞忓箷
 */
PIMAGE      gettarget();

/**
 * @brief 璁剧疆缁樺浘鐩爣
 * @param pbuf 鐩爣鍥惧儚鎸囬拡锛孨ULL琛ㄧず灞忓箷
 * @return 璁剧疆缁撴灉浠ｇ爜
 */
int         settarget(PIMAGE pbuf);

/**
 * @brief 娓呯┖璁惧锛堟竻灞忥級
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI cleardevice(PIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇瑙嗗彛璁剧疆
 * @param left 宸﹁竟鐣岃緭鍑烘寚閽?
 * @param top 涓婅竟鐣岃緭鍑烘寚閽?
 * @param right 鍙宠竟鐣岃緭鍑烘寚閽?
 * @param bottom 涓嬭竟鐣岃緭鍑烘寚閽?
 * @param clip 瑁佸壀鏍囧織杈撳嚭鎸囬拡锛堝彲閫夛級
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI getviewport(int* left, int* top, int* right, int* bottom, int* clip = 0, PCIMAGE pimg = NULL);

/**
 * @brief 璁剧疆瑙嗗彛
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param clip 鏄惁鍚敤瑁佸壀锛堥粯璁や负1锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setviewport(int  left, int  top, int  right, int  bottom, int  clip = 1, PIMAGE  pimg = NULL);

/**
 * @brief 娓呯┖瑙嗗彛鍖哄煙
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI clearviewport(PIMAGE pimg = NULL);

/**
 * @deprecated 宸插純鐢紝璇蜂娇鐢ㄥ浘鍍忓嚱鏁版浛浠?
 * @brief 璁剧疆娲诲姩椤甸潰
 * @param page 椤甸潰缂栧彿
 */
EGE_DEPRECATE(setactivepage, "Please use the image function instead.")
void EGEAPI setactivepage(int page);

/**
 * @deprecated 宸插純鐢紝璇蜂娇鐢ㄥ浘鍍忓嚱鏁版浛浠?
 * @brief 璁剧疆鍙椤甸潰
 * @param page 椤甸潰缂栧彿
 */
EGE_DEPRECATE(setvisualpage, "Please use the image function instead.")
void EGEAPI setvisualpage(int page);

/**
 * @deprecated 宸插純鐢紝璇蜂娇鐢ㄥ浘鍍忓嚱鏁版浛浠?
 * @brief 浜ゆ崲椤甸潰
 */
EGE_DEPRECATE(swappage, "Please use the image function instead.")
void EGEAPI swappage();

/**
 * @brief 鑾峰彇绐楀彛瑙嗗彛璁剧疆
 * @param viewport 瑙嗗彛缁撴瀯鎸囬拡
 */
void EGEAPI window_getviewport(viewporttype * viewport);

/**
 * @brief 鑾峰彇绐楀彛瑙嗗彛璁剧疆
 * @param left 宸﹁竟鐣岃緭鍑烘寚閽?
 * @param top 涓婅竟鐣岃緭鍑烘寚閽?
 * @param right 鍙宠竟鐣岃緭鍑烘寚閽?
 * @param bottom 涓嬭竟鐣岃緭鍑烘寚閽?
 */
void EGEAPI window_getviewport(int* left, int* top, int* right, int* bottom);

/**
 * @brief 璁剧疆绐楀彛瑙嗗彛
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 */
void EGEAPI window_setviewport(int  left, int  top, int  right, int  bottom);

/**
 * @brief 璁剧疆绾挎潯瀹藉害
 * @param width 绾挎潯瀹藉害锛堝儚绱狅級
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setlinewidth(float width, PIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇绾挎潯鏍峰紡
 * @param linestyle 绾挎潯鏍峰紡杈撳嚭鎸囬拡
 * @param pattern 绾挎潯鍥炬杈撳嚭鎸囬拡锛堝彲閫夛級
 * @param thickness 绾挎潯绮楃粏杈撳嚭鎸囬拡锛堝彲閫夛級
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI getlinestyle(int* linestyle, unsigned short* pattern = NULL, int* thickness = NULL, PCIMAGE pimg = NULL);

/**
 * @brief 璁剧疆绾挎潯鏍峰紡
 * @param linestyle 绾挎潯鏍峰紡
 * @param pattern 绾挎潯鍥炬锛堥粯璁や负0锛?
 * @param thickness 绾挎潯绮楃粏锛堥粯璁や负1锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setlinestyle(int  linestyle, unsigned short  pattern = 0,    int  thickness = 1,    PIMAGE  pimg = NULL);

/**
 * @brief 璁剧疆绾挎潯绔偣鏍峰紡
 * @param linecap 绔偣鏍峰紡
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setlinecap(line_cap_type linecap, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆绾挎潯绔偣鏍峰紡锛堝垎鍒缃捣濮嬪拰缁撴潫绔偣锛?
 * @param startCap 璧峰绔偣鏍峰紡
 * @param endCap 缁撴潫绔偣鏍峰紡
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setlinecap(line_cap_type  startCap, line_cap_type  endCap, PIMAGE  pimg = NULL);

/**
 * @brief 鑾峰彇绾挎潯绔偣鏍峰紡
 * @param startCap 璧峰绔偣鏍峰紡杈撳嚭鎸囬拡
 * @param endCap 缁撴潫绔偣鏍峰紡杈撳嚭鎸囬拡
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI getlinecap(line_cap_type* startCap, line_cap_type* endCap, PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇绾挎潯绔偣鏍峰紡
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 绔偣鏍峰紡
 */
line_cap_type EGEAPI getlinecap(PCIMAGE pimg = NULL);

/**
 * @brief 璁剧疆绾挎潯杩炴帴鏍峰紡
 * @param linejoin 杩炴帴鏍峰紡
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setlinejoin(line_join_type  linejoin, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆绾挎潯杩炴帴鏍峰紡锛堝甫鏂滄帴闄愬埗锛?
 * @param linejoin 杩炴帴鏍峰紡
 * @param miterLimit 鏂滄帴闄愬埗鍊?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setlinejoin(line_join_type  linejoin, float  miterLimit, PIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇绾挎潯杩炴帴鏍峰紡
 * @param linejoin 杩炴帴鏍峰紡杈撳嚭鎸囬拡
 * @param miterLimit 鏂滄帴闄愬埗鍊艰緭鍑烘寚閽?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI getlinejoin(line_join_type* linejoin, float* miterLimit, PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇绾挎潯杩炴帴鏍峰紡
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 杩炴帴鏍峰紡
 */
line_join_type EGEAPI getlinejoin(PCIMAGE pimg = NULL);

/**
 * @brief 璁剧疆濉厖鏍峰紡
 * @param pattern 濉厖鍥炬绫诲瀷
 * @param color 濉厖棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setfillstyle(int pattern, color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍐欏叆妯″紡
 * @param mode 鍐欏叆妯″紡
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setwritemode(int mode, PIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇缁樺浘棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 褰撳墠缁樺浘棰滆壊
 */
color_t EGEAPI getcolor      (PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇绾挎潯棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 褰撳墠绾挎潯棰滆壊
 */
color_t EGEAPI getlinecolor  (PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇濉厖棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 褰撳墠濉厖棰滆壊
 */
color_t EGEAPI getfillcolor  (PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇鑳屾櫙棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 褰撳墠鑳屾櫙棰滆壊
 */
color_t EGEAPI getbkcolor    (PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇鏂囨湰棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 褰撳墠鏂囨湰棰滆壊
 */
color_t EGEAPI gettextcolor  (PCIMAGE pimg = NULL);

/**
 * @brief 璁剧疆缁樺浘棰滆壊
 * @param color 缁樺浘棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI setcolor      (color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆绾挎潯棰滆壊
 * @param color 绾挎潯棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI setlinecolor  (color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆濉厖棰滆壊
 * @param color 濉厖棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI setfillcolor  (color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鑳屾櫙棰滆壊, 浼氫富鍔ㄥ皢鏃ц儗鏅壊鍍忕礌鏇挎崲涓烘柊鑳屾櫙鑹插儚绱?
 * @param color 鑳屾櫙棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI setbkcolor    (color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鑳屾櫙棰滆壊, 浣嗕笉浼氬皢鏃ц儗鏅壊鍍忕礌鏇挎崲涓烘柊鑳屾櫙鑹插儚绱?
 * @param color 鑳屾櫙棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI setbkcolor_f  (color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鏂囨湰棰滆壊
 * @param color 鏂囨湰棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI settextcolor  (color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆瀛椾綋鑳屾櫙棰滆壊
 * @param color 瀛椾綋鑳屾櫙棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI setfontbkcolor(color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鑳屾櫙娣峰悎妯″紡
 * @param bkMode 鑳屾櫙妯″紡锛圱RANSPARENT鎴朞PAQUE锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI setbkmode(int bkMode, PIMAGE pimg = NULL);

/// @defgroup ColorConversion 棰滆壊杞崲瀹忓畾涔?
/// @{
#define RGBtoGRAY   rgb2gray    ///< RGB杞伆搴?
#define RGBtoHSL    rgb2hsl     ///< RGB杞琀SL
#define RGBtoHSV    rgb2hsv     ///< RGB杞琀SV
#define HSLtoRGB    hsl2rgb     ///< HSL杞琑GB
#define HSVtoRGB    hsv2rgb     ///< HSV杞琑GB
/// @}

/**
 * @brief RGB棰滆壊杞崲涓虹伆搴?
 * @param rgb RGB棰滆壊鍊?
 * @return 鐏板害棰滆壊鍊?
 */
color_t EGEAPI rgb2gray(color_t rgb);

/**
 * @brief RGB棰滆壊杞崲涓篐SL
 * @param rgb RGB棰滆壊鍊?
 * @param H 鑹茶皟杈撳嚭鎸囬拡锛?-360搴︼級
 * @param S 楗卞拰搴﹁緭鍑烘寚閽堬紙0-1锛?
 * @param L 浜害杈撳嚭鎸囬拡锛?-1锛?
 */
void    EGEAPI rgb2hsl(color_t rgb, float* H, float* S, float* L);

/**
 * @brief RGB棰滆壊杞崲涓篐SV
 * @param rgb RGB棰滆壊鍊?
 * @param H 鑹茶皟杈撳嚭鎸囬拡锛?-360搴︼級
 * @param S 楗卞拰搴﹁緭鍑烘寚閽堬紙0-1锛?
 * @param V 鏄庡害杈撳嚭鎸囬拡锛?-1锛?
 */
void    EGEAPI rgb2hsv(color_t rgb, float* H, float* S, float* V);

/**
 * @brief HSL棰滆壊杞崲涓篟GB
 * @param H 鑹茶皟锛?-360搴︼級
 * @param S 楗卞拰搴︼紙0-1锛?
 * @param L 浜害锛?-1锛?
 * @return RGB棰滆壊鍊?
 */
color_t EGEAPI hsl2rgb(float H, float S, float L);

/**
 * @brief HSV棰滆壊杞崲涓篟GB
 * @param H 鑹茶皟锛?-360搴︼級
 * @param S 楗卞拰搴︼紙0-1锛?
 * @param V 鏄庡害锛?-1锛?
 * @return RGB棰滆壊鍊?
 */
color_t EGEAPI hsv2rgb(float H, float S, float V);

/**
 * @brief 棰滆壊娣峰悎
 * @param dst 鐩爣棰滆壊
 * @param src 婧愰鑹?
 * @param alpha Alpha閫忔槑搴︼紙0-255锛?
 * @return 娣峰悎鍚庣殑棰滆壊
 */
color_t EGEAPI colorblend  (color_t dst, color_t src, unsigned char alpha);

/**
 * @brief 棰滆壊娣峰悎锛坒ast鐗堟湰, 绮惧害杈冧綆锛?
 * @param dst 鐩爣棰滆壊
 * @param src 婧愰鑹?
 * @param alpha Alpha閫忔槑搴︼紙0-255锛?
 * @return 娣峰悎鍚庣殑棰滆壊
 */
color_t EGEAPI colorblend_f(color_t dst, color_t src, unsigned char alpha);

/**
 * @brief Alpha娣峰悎
 * @param dst 鐩爣棰滆壊
 * @param src 婧愰鑹诧紙鍖呭惈Alpha閫氶亾锛?
 * @return 娣峰悎鍚庣殑棰滆壊
 */
color_t EGEAPI alphablend  (color_t dst, color_t src);

/**
 * @brief Alpha娣峰悎锛堝甫閫忔槑搴﹀洜瀛愶級
 * @param dst 鐩爣棰滆壊
 * @param src 婧愰鑹?
 * @param srcAlphaFactor 婧愰鑹查€忔槑搴﹀洜瀛愶紙0-255锛?
 * @return 娣峰悎鍚庣殑棰滆壊
 */
color_t EGEAPI alphablend  (color_t dst, color_t src, unsigned char srcAlphaFactor);

/**
 * @brief Alpha娣峰悎锛堥涔楢lpha锛?
 * @param dst 鐩爣棰滆壊
 * @param src 婧愰鑹诧紙棰勪箻Alpha锛?
 * @return 娣峰悎鍚庣殑棰滆壊
 */
color_t EGEAPI alphablend_premultiplied(color_t dst, color_t src);

/**
 * @brief Alpha娣峰悎锛堥涔楢lpha锛屽甫閫忔槑搴﹀洜瀛愶級
 * @param dst 鐩爣棰滆壊
 * @param src 婧愰鑹诧紙棰勪箻Alpha锛?
 * @param srcAlphaFactor 婧愰鑹查€忔槑搴﹀洜瀛愶紙0-255锛?
 * @return 娣峰悎鍚庣殑棰滆壊
 */
color_t EGEAPI alphablend_premultiplied(color_t dst, color_t src, unsigned char srcAlphaFactor);

/**
 * @brief 鑾峰彇鍍忕礌棰滆壊
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 鍍忕礌棰滆壊鍊?
 */
color_t EGEAPI getpixel   (int x, int y, PCIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍍忕礌棰滆壊
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param color 棰滆壊鍊?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixel   (int x, int y, color_t color, PIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇鍍忕礌棰滆壊锛堝揩閫熺増鏈? 鏃犺竟鐣屾鏌ワ紝鏃犺viewport锛?
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 鍍忕礌棰滆壊鍊?
 */
color_t EGEAPI getpixel_f (int x, int y, PCIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍍忕礌棰滆壊锛堝揩閫熺増鏈? 鏃犺竟鐣屾鏌ワ紝鏃犺viewport锛?
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param color 棰滆壊鍊?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixel_f (int x, int y, color_t color, PIMAGE pimg = NULL);

/**
 * @brief 鎵归噺璁剧疆鍍忕礌
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐瑰潗鏍囨暟缁勶紝鏍煎紡涓篬x1,y1,x2,y2,...]
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixels  (int numOfPoints, const int* points, PIMAGE pimg = NULL);

/**
 * @brief 鎵归噺璁剧疆鍍忕礌锛堝揩閫熺増鏈? 鏃犺竟鐣屾鏌ワ級
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐瑰潗鏍囨暟缁勶紝鏍煎紡涓篬x1,y1,x2,y2,...]
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixels_f(int numOfPoints, const int* points, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍍忕礌(RGB 閫氶亾璁剧疆涓轰笌 ARGB 棰滆壊娣峰悎鐨勭粨鏋滐紝alpha 閫氶亾淇濇寔涓嶅彉)
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param color 棰滆壊鍊硷紙鍖呭惈Alpha閫氶亾锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixel_withalpha   (int x, int y, color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍍忕礌锛堜繚鐣橝lpha閫氶亾锛屽揩閫熺増鏈? 鏃犺竟鐣屾鏌ワ級
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param color 棰滆壊鍊硷紙鍖呭惈Alpha閫氶亾锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixel_withalpha_f (int x, int y, color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍍忕礌 (RGB 閫氶亾鏇挎崲涓烘寚瀹氱殑棰滆壊鍊硷紝alpha 閫氶亾淇濇寔涓嶅彉)
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param color 棰滆壊鍊?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixel_savealpha   (int x, int y, color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍍忕礌锛堜繚瀛楢lpha閫氶亾锛屽揩閫熺増鏈? 鏃犺竟鐣屾鏌ワ級
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param color 棰滆壊鍊?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixel_savealpha_f (int x, int y, color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍍忕礌锛圓lpha娣峰悎锛?
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param color 棰滆壊鍊?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixel_alphablend  (int x, int y, color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍍忕礌锛圓lpha娣峰悎锛屽揩閫熺増鏈? 鏃犺竟鐣屾鏌ワ級
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param color 棰滆壊鍊?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixel_alphablend_f(int x, int y, color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍍忕礌锛圓lpha娣峰悎锛屽甫閫忔槑搴﹀洜瀛愶級
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param color 棰滆壊鍊?
 * @param alphaFactor 閫忔槑搴﹀洜瀛愶紙0-255锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixel_alphablend  (int x, int y, color_t color, unsigned char alphaFactor, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍍忕礌锛圓lpha娣峰悎锛屽甫閫忔槑搴﹀洜瀛愶紝蹇€熺増鏈? 鏃犺竟鐣屾鏌ワ級
 * @param x x鍧愭爣
 * @param y y鍧愭爣
 * @param color 棰滆壊鍊?
 * @param alphaFactor 閫忔槑搴﹀洜瀛愶紙0-255锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI putpixel_alphablend_f(int x, int y, color_t color, unsigned char alphaFactor, PIMAGE pimg = NULL);

/**
 * @brief 绉诲姩褰撳墠缁樺浘浣嶇疆
 * @param x 鏂扮殑x鍧愭爣
 * @param y 鏂扮殑y鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI moveto (int x,  int y,  PIMAGE pimg = NULL);

/**
 * @brief 鐩稿绉诲姩褰撳墠缁樺浘浣嶇疆
 * @param dx x鏂瑰悜鍋忕Щ閲?
 * @param dy y鏂瑰悜鍋忕Щ閲?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI moverel(int dx, int dy, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鐩寸嚎
 * @param x1 璧风偣x鍧愭爣
 * @param y1 璧风偣y鍧愭爣
 * @param x2 缁堢偣x鍧愭爣
 * @param y2 缁堢偣y鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI line     (int   x1, int   y1, int   x2, int   y2, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鐩寸嚎锛堟诞鐐圭増鏈級
 * @param x1 璧风偣x鍧愭爣
 * @param y1 璧风偣y鍧愭爣
 * @param x2 缁堢偣x鍧愭爣
 * @param y2 缁堢偣y鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI line_f   (float x1, float y1, float x2, float y2, PIMAGE pimg = NULL);

/**
 * @brief 浠庡綋鍓嶄綅缃粯鍒剁洿绾垮埌鎸囧畾鐐?
 * @param x 缁堢偣x鍧愭爣
 * @param y 缁堢偣y鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI lineto   (int   x,  int   y,  PIMAGE pimg = NULL);

/**
 * @brief 浠庡綋鍓嶄綅缃粯鍒剁洿绾垮埌鎸囧畾鐐癸紙娴偣鐗堟湰锛?
 * @param x 缁堢偣x鍧愭爣
 * @param y 缁堢偣y鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI lineto_f (float x,  float y,  PIMAGE pimg = NULL);

/**
 * @brief 浠庡綋鍓嶄綅缃粯鍒剁浉瀵圭洿绾?
 * @param dx x鏂瑰悜鍋忕Щ閲?
 * @param dy y鏂瑰悜鍋忕Щ閲?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI linerel  (int   dx, int   dy, PIMAGE pimg = NULL);

/**
 * @brief 浠庡綋鍓嶄綅缃粯鍒剁浉瀵圭洿绾匡紙娴偣鐗堟湰锛?
 * @param dx x鏂瑰悜鍋忕Щ閲?
 * @param dy y鏂瑰悜鍋忕Щ閲?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void    EGEAPI linerel_f(float dx, float dy, PIMAGE pimg = NULL);

//void EGEAPI getarccoords(int *px, int *py, int *pxstart, int *pystart, int *pxend, int *pyend, PIMAGE pimg = NULL);    // ###

/**
 * @brief 缁樺埗妞渾寮?
 * @param x 妞渾涓績x鍧愭爣
 * @param y 妞渾涓績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ellipse      (int   x, int   y, int   startAngle, int   endAngle, int   xRadius, int   yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗妞渾寮э紙娴偣鐗堟湰锛?
 * @param x 妞渾涓績x鍧愭爣
 * @param y 妞渾涓績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ellipsef     (float x, float y, float startAngle, float endAngle, float xRadius, float yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鎵囧舰锛堜粎杈规锛?
 * @param x 鎵囧舰涓績x鍧愭爣
 * @param y 鎵囧舰涓績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI sector       (int   x, int   y, int   startAngle, int   endAngle, int   xRadius, int   yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鎵囧舰锛堜粎杈规锛屾诞鐐圭増鏈級
 * @param x 鎵囧舰涓績x鍧愭爣
 * @param y 鎵囧舰涓績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI sectorf      (float x, float y, float startAngle, float endAngle, float xRadius, float yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗楗煎浘锛堜粎杈规锛?
 * @param x 楗煎浘涓績x鍧愭爣
 * @param y 楗煎浘涓績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI pie          (int   x, int   y, int   startAngle, int   endAngle, int   xRadius, int   yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗楗煎浘锛堜粎杈规锛屾诞鐐圭増鏈級
 * @param x 楗煎浘涓績x鍧愭爣
 * @param y 楗煎浘涓績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI pief         (float x, float y, float startAngle, float endAngle, float xRadius, float yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖楗煎浘
 * @param x 楗煎浘涓績x鍧愭爣
 * @param y 楗煎浘涓績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI fillpie      (int   x, int   y, int   startAngle, int   endAngle, int   xRadius, int   yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖楗煎浘锛堟诞鐐圭増鏈級
 * @param x 楗煎浘涓績x鍧愭爣
 * @param y 楗煎浘涓績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI fillpief     (float x, float y, float startAngle, float endAngle, float xRadius, float yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗瀹炲績楗煎浘
 * @param x 楗煎浘涓績x鍧愭爣
 * @param y 楗煎浘涓績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI solidpie     (int   x, int   y, int   startAngle, int   endAngle, int   xRadius, int   yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗瀹炲績楗煎浘锛堟诞鐐圭増鏈級
 * @param x 楗煎浘涓績x鍧愭爣
 * @param y 楗煎浘涓績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI solidpief    (float x, float y, float startAngle, float endAngle, float xRadius, float yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍦嗗姬
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI arc          (int   x, int   y, int   startAngle, int   endAngle, int   radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍦嗗姬锛堟诞鐐圭増鏈級
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI arcf         (float x, float y, float startAngle, float endAngle, float radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍦嗘墖褰?
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI pieslice     (int   x, int   y, int   startAngle, int   endAngle, int   radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍦嗘墖褰紙娴偣鐗堟湰锛?
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param endAngle 缁撴潫瑙掑害锛堝害锛?
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI pieslicef    (float x, float y, float startAngle, float endAngle, float radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖妞渾
 * @param x 妞渾涓績x鍧愭爣
 * @param y 妞渾涓績y鍧愭爣
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI fillellipse  (int   x, int   y, int   xRadius,    int   yRadius,  PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖妞渾锛堟诞鐐圭増鏈級
 * @param x 妞渾涓績x鍧愭爣
 * @param y 妞渾涓績y鍧愭爣
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI fillellipsef (float x, float y, float xRadius,    float yRadius,  PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗瀹炲績妞渾
 * @param x 妞渾涓績x鍧愭爣
 * @param y 妞渾涓績y鍧愭爣
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI solidellipse (int   x, int   y, int   xRadius,    int   yRadius,  PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗瀹炲績妞渾锛堟诞鐐圭増鏈級
 * @param x 妞渾涓績x鍧愭爣
 * @param y 妞渾涓績y鍧愭爣
 * @param xRadius x杞村崐寰?
 * @param yRadius y杞村崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI solidellipsef(float x, float y, float xRadius,    float yRadius,  PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍦嗗舰杈规
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI circle       (int   x, int   y, int   radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍦嗗舰杈规锛堟诞鐐圭増鏈級
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI circlef      (float x, float y, float radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖鍦嗗舰
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI fillcircle   (int   x, int   y, int   radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖鍦嗗舰锛堟诞鐐圭増鏈級
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI fillcirclef  (float x, float y, float radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗瀹炲績鍦嗗舰
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI solidcircle  (int   x, int   y, int   radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗瀹炲績鍦嗗舰锛堟诞鐐圭増鏈級
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI solidcirclef (float x, float y, float radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗3D鏌辩姸鍥?
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param depth 娣卞害
 * @param topFlag 鏄惁缁樺埗椤堕潰
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI bar3d        (int left, int top, int right, int bottom, int depth,   int topFlag, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鏌辩姸鍥撅紙濉厖鐭╁舰锛?
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI bar          (int left, int top, int right, int bottom, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鐭╁舰杈规
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI rectangle    (int left, int top, int right, int bottom, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖鐭╁舰
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI fillrect     (int left, int top, int right, int bottom, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗瀹炲績鐭╁舰
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI solidrect    (int left, int top, int right, int bottom, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍦嗚鐭╁舰杈规
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param radius 鍦嗚鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI roundrect     (int left, int top, int right, int bottom, int radius,  PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖鍦嗚鐭╁舰
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param radius 鍦嗚鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI fillroundrect (int left, int top, int right, int bottom, int radius,  PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗瀹炲績鍦嗚鐭╁舰
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param radius 鍦嗚鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI solidroundrect(int left, int top, int right, int bottom, int radius,  PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍦嗚鐭╁舰杈规锛堜笉鍚屽渾瑙掑崐寰勶級
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param xRadius x杞村渾瑙掑崐寰?
 * @param yRadius y杞村渾瑙掑崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI roundrect     (int left, int top, int right, int bottom, int xRadius, int yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖鍦嗚鐭╁舰锛堜笉鍚屽渾瑙掑崐寰勶級
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param xRadius x杞村渾瑙掑崐寰?
 * @param yRadius y杞村渾瑙掑崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI fillroundrect (int left, int top, int right, int bottom, int xRadius, int yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗瀹炲績鍦嗚鐭╁舰锛堜笉鍚屽渾瑙掑崐寰勶級
 * @param left 宸﹁竟鐣?
 * @param top 涓婅竟鐣?
 * @param right 鍙宠竟鐣?
 * @param bottom 涓嬭竟鐣?
 * @param xRadius x杞村渾瑙掑崐寰?
 * @param yRadius y杞村渾瑙掑崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI solidroundrect(int left, int top, int right, int bottom, int xRadius, int yRadius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗澶氳竟褰?
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐瑰潗鏍囨暟缁勶紝鏍煎紡涓篬x1,y1,x2,y2,...]
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI drawpoly      (int numOfPoints, const int *points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗澶氭潯绾挎
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐瑰潗鏍囨暟缁勶紝鏍煎紡涓篬x1,y1,x2,y2,...]
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI polyline      (int numOfPoints, const int *points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗澶氳竟褰㈣竟妗?
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐瑰潗鏍囨暟缁勶紝鏍煎紡涓篬x1,y1,x2,y2,...]
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI polygon       (int numOfPoints, const int *points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖澶氳竟褰?
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐瑰潗鏍囨暟缁勶紝鏍煎紡涓篬x1,y1,x2,y2,...]
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI fillpoly      (int numOfPoints, const int *points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗瀹炲績澶氳竟褰?
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐瑰潗鏍囨暟缁勶紝鏍煎紡涓篬x1,y1,x2,y2,...]
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI solidpoly     (int numOfPoints, const int *points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗娓愬彉濉厖澶氳竟褰?
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 甯﹂鑹茬殑鐐瑰潗鏍囨暟缁?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI fillpoly_gradient(int numOfPoints, const ege_colpoint* points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗澶氭潯绾挎
 * @param numOfLines 绾挎鏁伴噺
 * @param points 绾挎绔偣鍧愭爣鏁扮粍锛屾瘡涓や釜鐐规瀯鎴愪竴鏉＄嚎娈?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI drawlines     (int numOfLines,  const int *points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗璐濆灏旀洸绾?
 * @param numOfPoints 鎺у埗鐐规暟閲?
 * @param points 鎺у埗鐐瑰潗鏍囨暟缁勶紝鏍煎紡涓篬x1,y1,x2,y2,...]
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI drawbezier    (int numOfPoints, const int *points, PIMAGE pimg = NULL);

/**
 * @brief 婕按濉厖锛堟牴鎹竟鐣岄鑹诧級
 * @param x 濉厖璧峰鐐箈鍧愭爣
 * @param y 濉厖璧峰鐐箉鍧愭爣
 * @param borderColor 杈圭晫棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI floodfill     (int x, int y, int borderColor, PIMAGE pimg = NULL);

/**
 * @brief 鍖哄煙濉厖锛堟牴鎹尯鍩熼鑹诧級
 * @param x 濉厖璧峰鐐箈鍧愭爣
 * @param y 濉厖璧峰鐐箉鍧愭爣
 * @param areaColor 瑕佹浛鎹㈢殑鍖哄煙棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI floodfillsurface (int x, int y, color_t areaColor, PIMAGE pimg = NULL);

#ifdef EGE_GDIPLUS
/// @defgroup EGEGDIPlus EGE GDI+澧炲己鍑芥暟
/// 闇€瑕佸畾涔塃GE_GDIPLUS瀹忔墠鑳戒娇鐢ㄧ殑澧炲己缁樺浘鍔熻兘
/// @{

/**
 * @brief 鍚敤鎴栫鐢ㄦ姉閿娇
 * @param enable true鍚敤鎶楅敮榻匡紝false绂佺敤
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_enable_aa(bool enable, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍏ㄥ眬Alpha閫忔槑搴?
 * @param alpha Alpha鍊硷紙0-255锛?瀹屽叏閫忔槑锛?55瀹屽叏涓嶉€忔槑锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_setalpha(int alpha, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鐩寸嚎锛圙DI+澧炲己鐗堟湰锛?
 * @param x1 璧风偣x鍧愭爣
 * @param y1 璧风偣y鍧愭爣
 * @param x2 缁堢偣x鍧愭爣
 * @param y2 缁堢偣y鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_line(float x1, float y1, float x2, float y2, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗澶氳竟褰紙GDI+澧炲己鐗堟湰锛?
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐瑰潗鏍囨暟缁?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawpoly       (int numOfPoints, const ege_point* points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗澶氭潯绾挎锛圙DI+澧炲己鐗堟湰锛?
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐瑰潗鏍囨暟缁?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_polyline       (int numOfPoints, const ege_point* points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗澶氳竟褰㈣竟妗嗭紙GDI+澧炲己鐗堟湰锛?
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐瑰潗鏍囨暟缁?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_polygon        (int numOfPoints, const ege_point* points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖澶氳竟褰紙GDI+澧炲己鐗堟湰锛?
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐瑰潗鏍囨暟缁?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_fillpoly       (int numOfPoints, const ege_point* points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗璐濆灏旀洸绾匡紙GDI+澧炲己鐗堟湰锛?
 * @param numOfPoints 鎺у埗鐐规暟閲?
 * @param points 鎺у埗鐐瑰潗鏍囨暟缁?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_bezier         (int numOfPoints, const ege_point* points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗璐濆灏旀洸绾匡紙鍚宔ge_bezier锛?
 * @param numOfPoints 鎺у埗鐐规暟閲?
 * @param points 鎺у埗鐐瑰潗鏍囨暟缁?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawbezier     (int numOfPoints, const ege_point* points, PIMAGE pimg = NULL);  // Same as ege_bezier

/**
 * @brief 缁樺埗鍩烘暟鏍锋潯鏇茬嚎
 * @param numOfPoints 鎺у埗鐐规暟閲?
 * @param points 鎺у埗鐐瑰潗鏍囨暟缁?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawcurve      (int numOfPoints, const ege_point* points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗闂悎鍩烘暟鏍锋潯鏇茬嚎
 * @param numOfPoints 鎺у埗鐐规暟閲?
 * @param points 鎺у埗鐐瑰潗鏍囨暟缁?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawclosedcurve(int numOfPoints, const ege_point* points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖闂悎鍩烘暟鏍锋潯鏇茬嚎
 * @param numOfPoints 鎺у埗鐐规暟閲?
 * @param points 鎺у埗鐐瑰潗鏍囨暟缁?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_fillclosedcurve(int numOfPoints, const ege_point* points, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍩烘暟鏍锋潯鏇茬嚎锛堝甫寮犲姏鍙傛暟锛?
 * @param numOfPoints 鎺у埗鐐规暟閲?
 * @param points 鎺у埗鐐瑰潗鏍囨暟缁?
 * @param tension 寮犲姏鍙傛暟锛?.0涓虹洿绾匡紝鍊艰秺澶ф洸绾胯秺寮洸锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawcurve      (int numOfPoints, const ege_point* points, float tension, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗闂悎鍩烘暟鏍锋潯鏇茬嚎锛堝甫寮犲姏鍙傛暟锛?
 * @param numOfPoints 鎺у埗鐐规暟閲?
 * @param points 鎺у埗鐐瑰潗鏍囨暟缁?
 * @param tension 寮犲姏鍙傛暟锛?.0涓虹洿绾匡紝鍊艰秺澶ф洸绾胯秺寮洸锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawclosedcurve(int numOfPoints, const ege_point* points, float tension, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖闂悎鍩烘暟鏍锋潯鏇茬嚎锛堝甫寮犲姏鍙傛暟锛?
 * @param numOfPoints 鎺у埗鐐规暟閲?
 * @param points 鎺у埗鐐瑰潗鏍囨暟缁?
 * @param tension 寮犲姏鍙傛暟锛?.0涓虹洿绾匡紝鍊艰秺澶ф洸绾胯秺寮洸锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_fillclosedcurve(int numOfPoints, const ege_point* points, float tension, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鐭╁舰杈规锛圙DI+澧炲己鐗堟湰锛?
 * @param x 鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param w 鐭╁舰瀹藉害
 * @param h 鐭╁舰楂樺害
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_rectangle    (float x, float y, float w, float h, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖鐭╁舰锛圙DI+澧炲己鐗堟湰锛?
 * @param x 鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param w 鐭╁舰瀹藉害
 * @param h 鐭╁舰楂樺害
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_fillrect     (float x, float y, float w, float h, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍦嗗舰杈规锛圙DI+澧炲己鐗堟湰锛?
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_circle       (float x, float y, float radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖鍦嗗舰锛圙DI+澧炲己鐗堟湰锛?
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param radius 鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_fillcircle   (float x, float y, float radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗妞渾杈规锛圙DI+澧炲己鐗堟湰锛?
 * @param x 妞渾杈圭晫鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 妞渾杈圭晫鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param w 妞渾杈圭晫鐭╁舰瀹藉害
 * @param h 妞渾杈圭晫鐭╁舰楂樺害
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_ellipse      (float x, float y, float w, float h, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖妞渾锛圙DI+澧炲己鐗堟湰锛?
 * @param x 妞渾杈圭晫鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 妞渾杈圭晫鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param w 妞渾杈圭晫鐭╁舰瀹藉害
 * @param h 妞渾杈圭晫鐭╁舰楂樺害
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_fillellipse  (float x, float y, float w, float h, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗妞渾寮э紙GDI+澧炲己鐗堟湰锛?
 * @param x 妞渾杈圭晫鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 妞渾杈圭晫鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param w 妞渾杈圭晫鐭╁舰瀹藉害
 * @param h 妞渾杈圭晫鐭╁舰楂樺害
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param sweepAngle 鎵弿瑙掑害锛堝害锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_arc          (float x, float y, float w, float h, float startAngle, float sweepAngle, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗楗煎浘杈规锛圙DI+澧炲己鐗堟湰锛?
 * @param x 妞渾杈圭晫鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 妞渾杈圭晫鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param w 妞渾杈圭晫鐭╁舰瀹藉害
 * @param h 妞渾杈圭晫鐭╁舰楂樺害
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param sweepAngle 鎵弿瑙掑害锛堝害锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_pie          (float x, float y, float w, float h, float startAngle, float sweepAngle, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖楗煎浘锛圙DI+澧炲己鐗堟湰锛?
 * @param x 妞渾杈圭晫鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 妞渾杈圭晫鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param w 妞渾杈圭晫鐭╁舰瀹藉害
 * @param h 妞渾杈圭晫鐭╁舰楂樺害
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param sweepAngle 鎵弿瑙掑害锛堝害锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_fillpie      (float x, float y, float w, float h, float startAngle, float sweepAngle, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍦嗚鐭╁舰杈规锛圙DI+澧炲己鐗堟湰锛?
 * @param x 鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param w 鐭╁舰瀹藉害
 * @param h 鐭╁舰楂樺害
 * @param radius 鍦嗚鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_roundrect    (float x, float y, float w, float h,  float radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖鍦嗚鐭╁舰锛圙DI+澧炲己鐗堟湰锛?
 * @param x 鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param w 鐭╁舰瀹藉害
 * @param h 鐭╁舰楂樺害
 * @param radius 鍦嗚鍗婂緞
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_fillroundrect(float x, float y, float w, float h,  float radius, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍦嗚鐭╁舰杈规锛堝悇瑙掍笉鍚屽崐寰勶級
 * @param x 鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param w 鐭╁舰瀹藉害
 * @param h 鐭╁舰楂樺害
 * @param radius1 宸︿笂瑙掑渾瑙掑崐寰?
 * @param radius2 鍙充笂瑙掑渾瑙掑崐寰?
 * @param radius3 鍙充笅瑙掑渾瑙掑崐寰?
 * @param radius4 宸︿笅瑙掑渾瑙掑崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_roundrect    (float x, float y, float w, float h,  float radius1, float radius2, float radius3, float radius4, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖鍦嗚鐭╁舰锛堝悇瑙掍笉鍚屽崐寰勶級
 * @param x 鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param w 鐭╁舰瀹藉害
 * @param h 鐭╁舰楂樺害
 * @param radius1 宸︿笂瑙掑渾瑙掑崐寰?
 * @param radius2 鍙充笂瑙掑渾瑙掑崐寰?
 * @param radius3 鍙充笅瑙掑渾瑙掑崐寰?
 * @param radius4 宸︿笅瑙掑渾瑙掑崐寰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_fillroundrect(float x, float y, float w, float h,  float radius1, float radius2, float radius3, float radius4, PIMAGE pimg = NULL);

/**
 * @brief 娓呴櫎濉厖鍥炬锛堣涓烘棤鍥炬锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_setpattern_none(PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆绾挎€ф笎鍙樺～鍏呭浘妗?
 * @param x1 璧峰鐐箈鍧愭爣
 * @param y1 璧峰鐐箉鍧愭爣
 * @param c1 璧峰鐐归鑹?
 * @param x2 缁撴潫鐐箈鍧愭爣
 * @param y2 缁撴潫鐐箉鍧愭爣
 * @param c2 缁撴潫鐐归鑹?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_setpattern_lineargradient(float x1, float y1, color_t c1, float x2, float y2, color_t c2, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆璺緞娓愬彉濉厖鍥炬
 * @param center 涓績鐐?
 * @param centerColor 涓績棰滆壊
 * @param count 杈圭晫鐐规暟閲?
 * @param points 杈圭晫鐐规暟缁?
 * @param colorCount 杈圭晫棰滆壊鏁伴噺
 * @param pointColors 杈圭晫棰滆壊鏁扮粍
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_setpattern_pathgradient(ege_point center, color_t centerColor, int count, const ege_point* points, int colorCount, const color_t* pointColors, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆妞渾娓愬彉濉厖鍥炬
 * @param center 涓績鐐?
 * @param centerColor 涓績棰滆壊
 * @param x 妞渾宸︿笂瑙抶鍧愭爣
 * @param y 妞渾宸︿笂瑙抷鍧愭爣
 * @param w 妞渾瀹藉害
 * @param h 妞渾楂樺害
 * @param color 杈圭晫棰滆壊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_setpattern_ellipsegradient(ege_point center, color_t centerColor, float x, float y, float w, float h, color_t color, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆绾圭悊濉厖鍥炬
 * @param imgSrc 婧愮汗鐞嗗浘鍍?
 * @param x 绾圭悊鍖哄煙宸︿笂瑙抶鍧愭爣
 * @param y 绾圭悊鍖哄煙宸︿笂瑙抷鍧愭爣
 * @param w 绾圭悊鍖哄煙瀹藉害
 * @param h 绾圭悊鍖哄煙楂樺害
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_setpattern_texture(PIMAGE imgSrc, float x, float y, float w, float h, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鏂囨湰锛圙DI+澧炲己鐗堟湰锛?
 * @param text 瑕佺粯鍒剁殑鏂囨湰
 * @param x 鏂囨湰宸︿笂瑙抶鍧愭爣
 * @param y 鏂囨湰宸︿笂瑙抷鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawtext(const char*    text, float x, float y, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鏂囨湰锛圙DI+澧炲己鐗堟湰锛孶nicode锛?
 * @param text 瑕佺粯鍒剁殑鏂囨湰
 * @param x 鏂囨湰宸︿笂瑙抶鍧愭爣
 * @param y 鏂囨湰宸︿笂瑙抷鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawtext(const wchar_t* text, float x, float y, PIMAGE pimg = NULL);

/**
 * @brief 鐢熸垚绾圭悊
 * @param generate 鏄惁鐢熸垚绾圭悊
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_gentexture(bool generate, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗绾圭悊鍥惧儚
 * @param imgSrc 婧愮汗鐞嗗浘鍍?
 * @param x 鐩爣鍖哄煙宸︿笂瑙抶鍧愭爣
 * @param y 鐩爣鍖哄煙宸︿笂瑙抷鍧愭爣
 * @param w 鐩爣鍖哄煙瀹藉害
 * @param h 鐩爣鍖哄煙楂樺害
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_puttexture(PCIMAGE imgSrc, float x, float y, float w, float h, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗绾圭悊鍥惧儚锛堟寚瀹氱洰鏍囩煩褰級
 * @param imgSrc 婧愮汗鐞嗗浘鍍?
 * @param dest 鐩爣鐭╁舰
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_puttexture(PCIMAGE imgSrc, ege_rect dest, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗绾圭悊鍥惧儚锛堟寚瀹氭簮鍜岀洰鏍囩煩褰級
 * @param imgSrc 婧愮汗鐞嗗浘鍍?
 * @param dest 鐩爣鐭╁舰
 * @param src 婧愮煩褰?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_puttexture(PCIMAGE imgSrc, ege_rect dest, ege_rect src, PIMAGE pimg = NULL);

//draw image
/**
 * @brief 缁樺埗鍥惧儚锛圙DI+澧炲己鐗堟湰锛?
 * @param imgSrc 婧愬浘鍍?
 * @param xDest 鐩爣x鍧愭爣
 * @param yDest 鐩爣y鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawimage(PCIMAGE imgSrc,int xDest, int yDest, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗鍥惧儚锛圙DI+澧炲己鐗堟湰锛屾寚瀹氭簮鍜岀洰鏍囧尯鍩燂級
 * @param imgSrc 婧愬浘鍍?
 * @param xDest 鐩爣鍖哄煙宸︿笂瑙抶鍧愭爣
 * @param yDest 鐩爣鍖哄煙宸︿笂瑙抷鍧愭爣
 * @param widthDest 鐩爣鍖哄煙瀹藉害
 * @param heightDest 鐩爣鍖哄煙楂樺害
 * @param xSrc 婧愬尯鍩熷乏涓婅x鍧愭爣
 * @param ySrc 婧愬尯鍩熷乏涓婅y鍧愭爣
 * @param widthSrc 婧愬尯鍩熷搴?
 * @param heightSrc 婧愬尯鍩熼珮搴?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawimage(PCIMAGE imgSrc,int xDest, int yDest, int widthDest, int heightDest, int xSrc, int ySrc, int widthSrc, int heightSrc,PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗璺緞杈规
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawpath(const ege_path* path, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖璺緞
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_fillpath(const ege_path* path, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗璺緞杈规锛堟寚瀹氬亸绉伙級
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x x鏂瑰悜鍋忕Щ
 * @param y y鏂瑰悜鍋忕Щ
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_drawpath(const ege_path* path, float x, float y, PIMAGE pimg = NULL);

/**
 * @brief 缁樺埗濉厖璺緞锛堟寚瀹氬亸绉伙級
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x x鏂瑰悜鍋忕Щ
 * @param y y鏂瑰悜鍋忕Щ
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_fillpath(const ege_path* path, float x, float y, PIMAGE pimg = NULL);

/// @defgroup PathManagement 璺緞绠＄悊鍑芥暟
/// @{

/**
 * @brief 鍒涘缓鏂扮殑璺緞瀵硅薄
 * @return 璺緞瀵硅薄鎸囬拡
 */
ege_path* EGEAPI ege_path_create     ();

/**
 * @brief 浠庣偣鏁扮粍鍜岀被鍨嬫暟缁勫垱寤鸿矾寰?
 * @param points 鐐瑰潗鏍囨暟缁?
 * @param types 鐐圭被鍨嬫暟缁?
 * @param count 鐐圭殑鏁伴噺
 * @return 璺緞瀵硅薄鎸囬拡
 */
ege_path* EGEAPI ege_path_createfrom (const ege_point* points, const unsigned char* types, int count);

/**
 * @brief 鍏嬮殕璺緞
 * @param path 婧愯矾寰勫璞℃寚閽?
 * @return 鏂扮殑璺緞瀵硅薄鎸囬拡
 */
ege_path* EGEAPI ege_path_clone      (const ege_path* path);

/**
 * @brief 閿€姣佽矾寰勫璞?
 * @param path 璺緞瀵硅薄鎸囬拡
 */
void      EGEAPI ege_path_destroy    (const ege_path* path);

/**
 * @brief 寮€濮嬫柊鐨勫瓙璺緞
 * @param path 璺緞瀵硅薄鎸囬拡
 */
void      EGEAPI ege_path_start      (ege_path* path);

/**
 * @brief 鍏抽棴褰撳墠瀛愯矾寰?
 * @param path 璺緞瀵硅薄鎸囬拡
 */
void      EGEAPI ege_path_close      (ege_path* path);

/**
 * @brief 鍏抽棴鎵€鏈夋墦寮€鐨勫瓙璺緞
 * @param path 璺緞瀵硅薄鎸囬拡
 */
void      EGEAPI ege_path_closeall   (ege_path* path);

/**
 * @brief 璁剧疆璺緞濉厖妯″紡
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param mode 濉厖妯″紡
 */
void      EGEAPI ege_path_setfillmode(ege_path* path, fill_mode mode);

/**
 * @brief 閲嶇疆璺緞锛堟竻绌烘墍鏈夊瓙璺緞锛?
 * @param path 璺緞瀵硅薄鎸囬拡
 */
void      EGEAPI ege_path_reset      (ege_path* path);

/**
 * @brief 鍙嶈浆璺緞鏂瑰悜
 * @param path 璺緞瀵硅薄鎸囬拡
 */
void      EGEAPI ege_path_reverse    (ege_path* path);

/**
 * @brief 鎵╁睍璺緞锛堢敓鎴愯疆寤擄級
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param lineWidth 绾挎潯瀹藉害
 * @param matrix 鍙樻崲鐭╅樀锛堝彲閫夛級
 */
void      EGEAPI ege_path_widen      (ege_path* path, float lineWidth, const ege_transform_matrix* matrix = NULL);

/**
 * @brief 鎵╁睍璺緞锛堢敓鎴愯疆寤擄紝鎸囧畾骞冲潶搴︼級
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param lineWidth 绾挎潯瀹藉害
 * @param matrix 鍙樻崲鐭╅樀
 * @param flatness 骞冲潶搴?
 */
void      EGEAPI ege_path_widen      (ege_path* path, float lineWidth, const ege_transform_matrix* matrix,  float flatness);

/**
 * @brief 骞冲潶鍖栬矾寰勶紙灏嗘洸绾胯浆鎹负鐩寸嚎娈碉級
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param matrix 鍙樻崲鐭╅樀锛堝彲閫夛級
 */
void      EGEAPI ege_path_flatten    (ege_path* path, const ege_transform_matrix* matrix = NULL);

/**
 * @brief 骞冲潶鍖栬矾寰勶紙鎸囧畾骞冲潶搴︼級
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param matrix 鍙樻崲鐭╅樀
 * @param flatness 骞冲潶搴?
 */
void      EGEAPI ege_path_flatten    (ege_path* path, const ege_transform_matrix* matrix, float flatness);

/**
 * @brief 鎵洸璺緞
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param points 鎵洸鎺у埗鐐规暟缁?
 * @param count 鎺у埗鐐规暟閲?
 * @param rect 鎵洸鐭╁舰鍖哄煙
 * @param matrix 鍙樻崲鐭╅樀锛堝彲閫夛級
 */
void      EGEAPI ege_path_warp       (ege_path* path, const ege_point* points, int count, const ege_rect* rect, const ege_transform_matrix* matrix = NULL);

/**
 * @brief 鎵洸璺緞锛堟寚瀹氬钩鍧﹀害锛?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param points 鎵洸鎺у埗鐐规暟缁?
 * @param count 鎺у埗鐐规暟閲?
 * @param rect 鎵洸鐭╁舰鍖哄煙
 * @param matrix 鍙樻崲鐭╅樀
 * @param flatness 骞冲潶搴?
 */
void      EGEAPI ege_path_warp       (ege_path* path, const ege_point* points, int count, const ege_rect* rect, const ege_transform_matrix* matrix, float flatness);

/**
 * @brief 鐢熸垚璺緞杞粨
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param matrix 鍙樻崲鐭╅樀锛堝彲閫夛級
 */
void      EGEAPI ege_path_outline    (ege_path* path, const ege_transform_matrix* matrix = NULL);

/**
 * @brief 鐢熸垚璺緞杞粨锛堟寚瀹氬钩鍧﹀害锛?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param matrix 鍙樻崲鐭╅樀
 * @param flatness 骞冲潶搴?
 */
void      EGEAPI ege_path_outline    (ege_path* path, const ege_transform_matrix* matrix, float flatness);

/**
 * @brief 妫€娴嬬偣鏄惁鍦ㄨ矾寰勫唴閮?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x 妫€娴嬬偣x鍧愭爣
 * @param y 妫€娴嬬偣y鍧愭爣
 * @return 濡傛灉鐐瑰湪璺緞鍐呴儴杩斿洖true锛屽惁鍒欒繑鍥瀎alse
 */
bool      EGEAPI ege_path_inpath     (const ege_path* path, float x, float y);

/**
 * @brief 妫€娴嬬偣鏄惁鍦ㄨ矾寰勫唴閮紙鎸囧畾鍥惧儚涓婁笅鏂囷級
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x 妫€娴嬬偣x鍧愭爣
 * @param y 妫€娴嬬偣y鍧愭爣
 * @param pimg 鍥惧儚涓婁笅鏂囨寚閽?
 * @return 濡傛灉鐐瑰湪璺緞鍐呴儴杩斿洖true锛屽惁鍒欒繑鍥瀎alse
 */
bool      EGEAPI ege_path_inpath     (const ege_path* path, float x, float y, PCIMAGE pimg);

/**
 * @brief 妫€娴嬬偣鏄惁鍦ㄨ矾寰勮竟妗嗕笂
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x 妫€娴嬬偣x鍧愭爣
 * @param y 妫€娴嬬偣y鍧愭爣
 * @return 濡傛灉鐐瑰湪璺緞杈规涓婅繑鍥瀟rue锛屽惁鍒欒繑鍥瀎alse
 */
bool      EGEAPI ege_path_instroke   (const ege_path* path, float x, float y);

/**
 * @brief 妫€娴嬬偣鏄惁鍦ㄨ矾寰勮竟妗嗕笂锛堟寚瀹氬浘鍍忎笂涓嬫枃锛?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x 妫€娴嬬偣x鍧愭爣
 * @param y 妫€娴嬬偣y鍧愭爣
 * @param pimg 鍥惧儚涓婁笅鏂囨寚閽?
 * @return 濡傛灉鐐瑰湪璺緞杈规涓婅繑鍥瀟rue锛屽惁鍒欒繑鍥瀎alse
 */
bool      EGEAPI ege_path_instroke   (const ege_path* path, float x, float y, PCIMAGE pimg);

/**
 * @brief 鑾峰彇璺緞鏈€鍚庝竴涓偣鐨勫潗鏍?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @return 鏈€鍚庝竴涓偣鐨勫潗鏍?
 */
ege_point      EGEAPI ege_path_lastpoint    (const ege_path* path);

/**
 * @brief 鑾峰彇璺緞涓偣鐨勬暟閲?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @return 璺緞涓偣鐨勬暟閲?
 */
int            EGEAPI ege_path_pointcount   (const ege_path* path);

/**
 * @brief 鑾峰彇璺緞杈圭晫鐭╁舰
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param matrix 鍙樻崲鐭╅樀锛孨ULL琛ㄧず涓嶈繘琛屽彉鎹?
 * @return 璺緞鐨勮竟鐣岀煩褰?
 */
ege_rect       EGEAPI ege_path_getbounds    (const ege_path* path, const ege_transform_matrix* matrix = NULL);

/**
 * @brief 鑾峰彇璺緞杈圭晫鐭╁舰锛堟寚瀹氬浘鍍忎笂涓嬫枃锛?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param matrix 鍙樻崲鐭╅樀
 * @param pimg 鍥惧儚涓婁笅鏂囨寚閽?
 * @return 璺緞鐨勮竟鐣岀煩褰?
 */
ege_rect       EGEAPI ege_path_getbounds    (const ege_path* path, const ege_transform_matrix* matrix, PCIMAGE pimg);

/**
 * @brief 鑾峰彇璺緞涓殑鎵€鏈夌偣
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param points 鐐规暟缁勭紦鍐插尯锛孨ULL琛ㄧず鑷姩鍒嗛厤
 * @return 鎸囧悜鐐规暟缁勭殑鎸囬拡
 * @note 濡傛灉points涓篘ULL锛屽嚱鏁颁細鍒嗛厤鍐呭瓨锛岃皟鐢ㄨ€呴渶瑕佽礋璐ｉ噴鏀?
 */
ege_point*     EGEAPI ege_path_getpathpoints(const ege_path* path, ege_point* points = NULL);

/**
 * @brief 鑾峰彇璺緞涓殑鎵€鏈夌偣绫诲瀷
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param types 绫诲瀷鏁扮粍缂撳啿鍖猴紝NULL琛ㄧず鑷姩鍒嗛厤
 * @return 鎸囧悜绫诲瀷鏁扮粍鐨勬寚閽?
 * @note 濡傛灉types涓篘ULL锛屽嚱鏁颁細鍒嗛厤鍐呭瓨锛岃皟鐢ㄨ€呴渶瑕佽礋璐ｉ噴鏀?
 */
unsigned char* EGEAPI ege_path_getpathtypes (const ege_path* path, unsigned char* types = NULL);

/**
 * @brief 鍙樻崲璺緞
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param matrix 鍙樻崲鐭╅樀
 */
void EGEAPI ege_path_transform     (ege_path* path, const ege_transform_matrix* matrix);

/// @defgroup PathAdd 璺緞娣诲姞鍑芥暟
/// @{

/**
 * @brief 鍚戣矾寰勬坊鍔犲彟涓€涓矾寰?
 * @param dstPath 鐩爣璺緞瀵硅薄鎸囬拡
 * @param srcPath 婧愯矾寰勫璞℃寚閽?
 * @param connect 鏄惁杩炴帴鍒板綋鍓嶈矾寰勭殑鏈€鍚庝竴鐐?
 */
void EGEAPI ege_path_addpath       (ege_path* dstPath, const ege_path* srcPath, bool connect);

/**
 * @brief 鍚戣矾寰勬坊鍔犵洿绾挎
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x1 璧风偣x鍧愭爣
 * @param y1 璧风偣y鍧愭爣
 * @param x2 缁堢偣x鍧愭爣
 * @param y2 缁堢偣y鍧愭爣
 */
void EGEAPI ege_path_addline       (ege_path* path, float x1, float y1, float x2, float y2);

/**
 * @brief 鍚戣矾寰勬坊鍔犲姬褰?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x 妞渾宸︿笂瑙抶鍧愭爣
 * @param y 妞渾宸︿笂瑙抷鍧愭爣
 * @param width 妞渾瀹藉害
 * @param height 妞渾楂樺害
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param sweepAngle 鎵弿瑙掑害锛堝害锛?
 */
void EGEAPI ege_path_addarc        (ege_path* path, float x, float y, float width, float height, float startAngle, float sweepAngle);

/**
 * @brief 鍚戣矾寰勬坊鍔犳姌绾?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param numOfPoints 鐐圭殑鏁伴噺
 * @param points 鐐规暟缁?
 */
void EGEAPI ege_path_addpolyline   (ege_path* path, int numOfPoints, const ege_point* points);

/**
 * @brief 鍚戣矾寰勬坊鍔犺礉濉炲皵鏇茬嚎锛堢偣鏁扮粍鐗堟湰锛?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param numOfPoints 鎺у埗鐐圭殑鏁伴噺
 * @param points 鎺у埗鐐规暟缁?
 */
void EGEAPI ege_path_addbezier     (ege_path* path, int numOfPoints, const ege_point* points);

/**
 * @brief 鍚戣矾寰勬坊鍔犺礉濉炲皵鏇茬嚎锛堝潗鏍囩増鏈級
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x1 璧风偣x鍧愭爣
 * @param y1 璧风偣y鍧愭爣
 * @param x2 绗竴涓帶鍒剁偣x鍧愭爣
 * @param y2 绗竴涓帶鍒剁偣y鍧愭爣
 * @param x3 绗簩涓帶鍒剁偣x鍧愭爣
 * @param y3 绗簩涓帶鍒剁偣y鍧愭爣
 * @param x4 缁堢偣x鍧愭爣
 * @param y4 缁堢偣y鍧愭爣
 */
void EGEAPI ege_path_addbezier     (ege_path* path, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);

/**
 * @brief 鍚戣矾寰勬坊鍔犲熀鏁版牱鏉℃洸绾?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param numOfPoints 鎺у埗鐐圭殑鏁伴噺
 * @param points 鎺у埗鐐规暟缁?
 */
void EGEAPI ege_path_addcurve      (ege_path* path, int numOfPoints, const ege_point* points);

/**
 * @brief 鍚戣矾寰勬坊鍔犲熀鏁版牱鏉℃洸绾匡紙鎸囧畾寮犲姏锛?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param numOfPoints 鎺у埗鐐圭殑鏁伴噺
 * @param points 鎺у埗鐐规暟缁?
 * @param tension 寮犲姏鍊硷紙0.0-1.0锛?
 */
void EGEAPI ege_path_addcurve      (ege_path* path, int numOfPoints, const ege_point* points, float tension);

/**
 * @brief 鍚戣矾寰勬坊鍔犲渾褰?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x 鍦嗗績x鍧愭爣
 * @param y 鍦嗗績y鍧愭爣
 * @param radius 鍗婂緞
 */
void EGEAPI ege_path_addcircle     (ege_path* path, float x, float y, float radius);

/**
 * @brief 鍚戣矾寰勬坊鍔犵煩褰?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x 鐭╁舰宸︿笂瑙抶鍧愭爣
 * @param y 鐭╁舰宸︿笂瑙抷鍧愭爣
 * @param width 鐭╁舰瀹藉害
 * @param height 鐭╁舰楂樺害
 */
void EGEAPI ege_path_addrect       (ege_path* path, float x, float y, float width, float height);

/**
 * @brief 鍚戣矾寰勬坊鍔犳き鍦?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x 妞渾杈圭晫宸︿笂瑙抶鍧愭爣
 * @param y 妞渾杈圭晫宸︿笂瑙抷鍧愭爣
 * @param width 妞渾瀹藉害
 * @param height 妞渾楂樺害
 */
void EGEAPI ege_path_addellipse    (ege_path* path, float x, float y, float width, float height);

/**
 * @brief 鍚戣矾寰勬坊鍔犻ゼ鍥?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x 妞渾杈圭晫宸︿笂瑙抶鍧愭爣
 * @param y 妞渾杈圭晫宸︿笂瑙抷鍧愭爣
 * @param width 妞渾瀹藉害
 * @param height 妞渾楂樺害
 * @param startAngle 璧峰瑙掑害锛堝害锛?
 * @param sweepAngle 鎵弿瑙掑害锛堝害锛?
 */
void EGEAPI ege_path_addpie        (ege_path* path, float x, float y, float width, float height, float startAngle, float sweepAngle);

/**
 * @brief 鍚戣矾寰勬坊鍔犳枃鏈紙ANSI鐗堟湰锛?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x 鏂囨湰璧峰x鍧愭爣
 * @param y 鏂囨湰璧峰y鍧愭爣
 * @param text 鏂囨湰鍐呭
 * @param height 鏂囨湰楂樺害
 * @param length 鏂囨湰闀垮害锛?1琛ㄧず鑷姩璁＄畻
 * @param typeface 瀛椾綋鍚嶇О锛孨ULL琛ㄧず浣跨敤榛樿瀛椾綋
 * @param fontStyle 瀛椾綋鏍峰紡
 */
void EGEAPI ege_path_addtext       (ege_path* path, float x, float y, const char*    text, float height, int length = -1, const char*    typeface = NULL, int fontStyle = 0);

/**
 * @brief 鍚戣矾寰勬坊鍔犳枃鏈紙Unicode鐗堟湰锛?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param x 鏂囨湰璧峰x鍧愭爣
 * @param y 鏂囨湰璧峰y鍧愭爣
 * @param text 鏂囨湰鍐呭
 * @param height 鏂囨湰楂樺害
 * @param length 鏂囨湰闀垮害锛?1琛ㄧず鑷姩璁＄畻
 * @param typeface 瀛椾綋鍚嶇О锛孨ULL琛ㄧず浣跨敤榛樿瀛椾綋
 * @param fontStyle 瀛椾綋鏍峰紡
 */
void EGEAPI ege_path_addtext       (ege_path* path, float x, float y, const wchar_t* text, float height, int length = -1, const wchar_t* typeface = NULL, int fontStyle = 0);

/**
 * @brief 鍚戣矾寰勬坊鍔犲杈瑰舰
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param numOfPoints 椤剁偣鏁伴噺
 * @param points 椤剁偣鏁扮粍
 */
void EGEAPI ege_path_addpolygon    (ege_path* path, int numOfPoints, const ege_point* points);

/**
 * @brief 鍚戣矾寰勬坊鍔犻棴鍚堝熀鏁版牱鏉℃洸绾?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param numOfPoints 鎺у埗鐐圭殑鏁伴噺
 * @param points 鎺у埗鐐规暟缁?
 */
void EGEAPI ege_path_addclosedcurve(ege_path* path, int numOfPoints, const ege_point* points);

/**
 * @brief 鍚戣矾寰勬坊鍔犻棴鍚堝熀鏁版牱鏉℃洸绾匡紙鎸囧畾寮犲姏锛?
 * @param path 璺緞瀵硅薄鎸囬拡
 * @param numOfPoints 鎺у埗鐐圭殑鏁伴噺
 * @param points 鎺у埗鐐规暟缁?
 * @param tension 寮犲姏鍊硷紙0.0-1.0锛?
 */
void EGEAPI ege_path_addclosedcurve(ege_path* path, int numOfPoints, const ege_point* points, float tension);

/// @}


/// @defgroup Transform 鍙樻崲鍑芥暟
/// @{

/**
 * @brief 鏃嬭浆鍙樻崲
 * @param angle 鏃嬭浆瑙掑害锛堝害锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_transform_rotate(float angle, PIMAGE pimg = NULL);

/**
 * @brief 骞崇Щ鍙樻崲
 * @param x x鏂瑰悜骞崇Щ閲?
 * @param y y鏂瑰悜骞崇Щ閲?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_transform_translate(float x, float y, PIMAGE pimg = NULL);

/**
 * @brief 缂╂斁鍙樻崲
 * @param xScale x鏂瑰悜缂╂斁姣斾緥
 * @param yScale y鏂瑰悜缂╂斁姣斾緥
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_transform_scale(float xScale, float yScale, PIMAGE pimg = NULL);

/**
 * @brief 閲嶇疆鍙樻崲鐭╅樀涓哄崟浣嶇煩闃?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_transform_reset(PIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇褰撳墠鍙樻崲鐭╅樀
 * @param matrix 杈撳嚭鍙樻崲鐭╅樀鐨勬寚閽?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_get_transform(ege_transform_matrix* matrix, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆鍙樻崲鐭╅樀
 * @param matrix 瑕佽缃殑鍙樻崲鐭╅樀
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI ege_set_transform(const ege_transform_matrix* matrix, PIMAGE pimg = NULL);

/**
 * @brief 璁＄畻鐐圭粡杩囧彉鎹㈠悗鐨勫潗鏍?
 * @param p 鍘熷鐐瑰潗鏍?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 鍙樻崲鍚庣殑鐐瑰潗鏍?
 */
ege_point EGEAPI ege_transform_calc(ege_point p, PIMAGE pimg = NULL);

/// @}

/**
 * @brief 璁＄畻鐐圭粡杩囧彉鎹㈠悗鐨勫潗鏍囷紙鍧愭爣鐗堟湰锛?
 * @param x 鍘熷鐐箈鍧愭爣
 * @param y 鍘熷鐐箉鍧愭爣
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 鍙樻崲鍚庣殑鐐瑰潗鏍?
 */
ege_point EGEAPI ege_transform_calc(float x, float y, PIMAGE pimg = NULL);


#endif

// It is not supported in VC 6.0.
#ifndef EGE_COMPILERINFO_VC6

/// @defgroup Console 鎺у埗鍙板嚱鏁?
/// @{

/**
 * @brief 鍒濆鍖栨帶鍒跺彴
 * @return 鎴愬姛杩斿洖true锛屽け璐ヨ繑鍥瀎alse
 * @note 鍒涘缓涓€涓帶鍒跺彴绐楀彛锛岀敤浜庢爣鍑嗚緭鍏ヨ緭鍑?
 */
bool EGEAPI init_console();

/**
 * @brief 娓呯┖鎺у埗鍙?
 * @return 鎴愬姛杩斿洖true锛屽け璐ヨ繑鍥瀎alse
 */
bool EGEAPI clear_console();

/**
 * @brief 鏄剧ず鎺у埗鍙扮獥鍙?
 * @return 鎴愬姛杩斿洖true锛屽け璐ヨ繑鍥瀎alse
 */
bool EGEAPI show_console();

/**
 * @brief 闅愯棌鎺у埗鍙扮獥鍙?
 * @return 鎴愬姛杩斿洖true锛屽け璐ヨ繑鍥瀎alse
 */
bool EGEAPI hide_console();

/**
 * @brief 鍏抽棴鎺у埗鍙板苟鎭㈠鏍囧噯杈撳叆杈撳嚭
 * @return 鎴愬姛杩斿洖true锛屽け璐ヨ繑鍥瀎alse
 */
bool EGEAPI close_console();
#endif

/**
 * @brief 浠庢帶鍒跺彴鑾峰彇瀛楃锛堟浛浠?conio.h>涓殑getch鍑芥暟锛?
 * @return 鑾峰彇鍒扮殑瀛楃鐮?
 */
int  EGEAPI getch_console();

/**
 * @brief 妫€娴嬫帶鍒跺彴鏄惁鏈夋寜閿紙鏇夸唬<conio.h>涓殑kbhit鍑芥暟锛?
 * @return 鏈夋寜閿繑鍥為潪闆跺€硷紝鏃犳寜閿繑鍥?
 */
int  EGEAPI kbhit_console();

/// @}

/// @defgroup Time 鏃堕棿鐩稿叧鍑芥暟
/// @{

/**
 * @brief EGE搴撳欢鏃跺嚱鏁?
 * @param ms 寤舵椂鏃堕棿锛堟绉掞級
 * @note 绮剧‘鐨勫欢鏃跺嚱鏁帮紝涓嶄細琚郴缁熻皟搴﹀奖鍝?
 */
void EGEAPI ege_sleep (long ms);

/**
 * @brief 寤舵椂鍑芥暟
 * @param ms 寤舵椂鏃堕棿锛堟绉掞級
 * @note 鏍囧噯寤舵椂鍑芥暟
 */
void EGEAPI delay     (long ms);

/**
 * @brief 姣寤舵椂鍑芥暟
 * @param ms 寤舵椂鏃堕棿锛堟绉掞級
 */
void EGEAPI delay_ms  (long ms);

/**
 * @brief API寤舵椂鍑芥暟
 * @param ms 寤舵椂鏃堕棿锛堟绉掞級
 * @note 浣跨敤绯荤粺API瀹炵幇鐨勫欢鏃?
 */
void EGEAPI api_sleep (long ms);

/**
 * @brief 鎸夊抚鐜囧欢鏃讹紙鏁存暟鐗堟湰锛?
 * @param fps 鐩爣甯х巼
 * @note 鏍规嵁甯х巼鑷姩璁＄畻寤舵椂鏃堕棿
 */
void EGEAPI delay_fps (int    fps);

/**
 * @brief 鎸夊抚鐜囧欢鏃讹紙闀挎暣鏁扮増鏈級
 * @param fps 鐩爣甯х巼
 */
void EGEAPI delay_fps (long   fps);

/**
 * @brief 鎸夊抚鐜囧欢鏃讹紙鍙岀簿搴︽诞鐐圭増鏈級
 * @param fps 鐩爣甯х巼
 */
void EGEAPI delay_fps (double fps);

/**
 * @brief 甯﹁烦甯х殑鎸夊抚鐜囧欢鏃?
 * @param fps 鐩爣甯х巼
 * @note 鏇寸簿纭殑甯х巼鎺у埗
 */
void EGEAPI delay_jfps(int    fps);

/**
 * @brief 绮剧‘鎸夊抚鐜囧欢鏃讹紙闀挎暣鏁扮増鏈級
 * @param fps 鐩爣甯х巼
 */
void EGEAPI delay_jfps(long   fps);

/**
 * @brief 绮剧‘鎸夊抚鐜囧欢鏃讹紙鍙岀簿搴︽诞鐐圭増鏈級
 * @param fps 鐩爣甯х巼
 */
void EGEAPI delay_jfps(double fps);

/**
 * @brief 鑾峰彇楂樼簿搴︽椂閽?
 * @return 褰撳墠鏃堕棿锛堢锛屽弻绮惧害娴偣锛?
 * @note 鐢ㄤ簬楂樼簿搴﹁鏃跺拰鎬ц兘娴嬮噺
 */
double EGEAPI fclock();

/// @}

/**
 * @defgroup TextOutput 鏂囨湰杈撳嚭
 * @brief 鏂囨湰杈撳嚭鍜屾樉绀虹浉鍏冲嚱鏁?
 * @{
 */

/**
 * @brief 鍦ㄥ綋鍓嶄綅缃緭鍑烘枃鏈瓧绗︿覆
 * @param text 瑕佽緭鍑虹殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 鏂囨湰杈撳嚭浣嶇疆鐢卞綋鍓嶇粯鍥句綅缃喅瀹氾紝浣跨敤 moveto() 璁剧疆
 */
void EGEAPI outtext(const char*    text, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄥ綋鍓嶄綅缃緭鍑烘枃鏈瓧绗︿覆锛圲nicode鐗堟湰锛?
 * @param text 瑕佽緭鍑虹殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 鏂囨湰杈撳嚭浣嶇疆鐢卞綋鍓嶇粯鍥句綅缃喅瀹氾紝浣跨敤 moveto() 璁剧疆
 */
void EGEAPI outtext(const wchar_t* text, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄥ綋鍓嶄綅缃緭鍑哄崟涓瓧绗?
 * @param c 瑕佽緭鍑虹殑瀛楃
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 鏂囨湰杈撳嚭浣嶇疆鐢卞綋鍓嶇粯鍥句綅缃喅瀹氾紝浣跨敤 moveto() 璁剧疆
 */
void EGEAPI outtext(char    c, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄥ綋鍓嶄綅缃緭鍑哄崟涓瓧绗︼紙Unicode鐗堟湰锛?
 * @param c 瑕佽緭鍑虹殑瀛楃
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 鏂囨湰杈撳嚭浣嶇疆鐢卞綋鍓嶇粯鍥句綅缃喅瀹氾紝浣跨敤 moveto() 璁剧疆
 */
void EGEAPI outtext(wchar_t c, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃緭鍑烘枃鏈瓧绗︿覆
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣
 * @param text 瑕佽緭鍑虹殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 瀹為檯杈撳嚭浣嶇疆鍙楁枃鏈榻愭柟寮忓拰瀛椾綋鍊炬枩瑙掑害褰卞搷
 */
void EGEAPI outtextxy(int x, int y, const char*    text, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃緭鍑烘枃鏈瓧绗︿覆锛圲nicode鐗堟湰锛?
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣
 * @param text 瑕佽緭鍑虹殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 瀹為檯杈撳嚭浣嶇疆鍙楁枃鏈榻愭柟寮忓拰瀛椾綋鍊炬枩瑙掑害褰卞搷
 */
void EGEAPI outtextxy(int x, int y, const wchar_t* text, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃緭鍑哄崟涓瓧绗?
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣
 * @param c 瑕佽緭鍑虹殑瀛楃
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 瀹為檯杈撳嚭浣嶇疆鍙楁枃鏈榻愭柟寮忓拰瀛椾綋鍊炬枩瑙掑害褰卞搷
 */
void EGEAPI outtextxy(int x, int y, char    c, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃緭鍑哄崟涓瓧绗︼紙Unicode鐗堟湰锛?
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣
 * @param c 瑕佽緭鍑虹殑瀛楃
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 瀹為檯杈撳嚭浣嶇疆鍙楁枃鏈榻愭柟寮忓拰瀛椾綋鍊炬枩瑙掑害褰卞搷
 */
void EGEAPI outtextxy(int x, int y, wchar_t c, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃牸寮忓寲杈撳嚭鏂囨湰
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣
 * @param format 鏍煎紡鍖栧瓧绗︿覆锛堢被浼紁rintf锛?
 * @param ... 鍙彉鍙傛暟鍒楄〃
 * @note 瀹為檯杈撳嚭浣嶇疆鍙楁枃鏈榻愭柟寮忓拰瀛椾綋鍊炬枩瑙掑害褰卞搷
 */
void EGEAPI xyprintf (int x, int y, const char*    format, ...);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃牸寮忓寲杈撳嚭鏂囨湰锛圲nicode鐗堟湰锛?
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣
 * @param format 鏍煎紡鍖栧瓧绗︿覆锛堢被浼紁rintf锛?
 * @param ... 鍙彉鍙傛暟鍒楄〃
 * @note 瀹為檯杈撳嚭浣嶇疆鍙楁枃鏈榻愭柟寮忓拰瀛椾綋鍊炬枩瑙掑害褰卞搷
 */
void EGEAPI xyprintf (int x, int y, const wchar_t* format, ...);

/**
 * @brief 鍦ㄦ寚瀹氱煩褰㈠尯鍩熷唴杈撳嚭鏂囨湰
 * @param x 鐭╁舰鍖哄煙宸︿笂瑙抶鍧愭爣
 * @param y 鐭╁舰鍖哄煙宸︿笂瑙抷鍧愭爣
 * @param w 鐭╁舰鍖哄煙瀹藉害
 * @param h 鐭╁舰鍖哄煙楂樺害
 * @param text 瑕佽緭鍑虹殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 鏂囨湰浼氬湪鎸囧畾鐭╁舰鑼冨洿鍐呰嚜鍔ㄦ崲琛岋紝鏀寔鏂囨湰瀵归綈璁剧疆
 */
void EGEAPI outtextrect(int x, int y, int w, int h, const char*    text, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氱煩褰㈠尯鍩熷唴杈撳嚭鏂囨湰锛圲nicode鐗堟湰锛?
 * @param x 鐭╁舰鍖哄煙宸︿笂瑙抶鍧愭爣
 * @param y 鐭╁舰鍖哄煙宸︿笂瑙抷鍧愭爣
 * @param w 鐭╁舰鍖哄煙瀹藉害
 * @param h 鐭╁舰鍖哄煙楂樺害
 * @param text 瑕佽緭鍑虹殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 鏂囨湰浼氬湪鎸囧畾鐭╁舰鑼冨洿鍐呰嚜鍔ㄦ崲琛岋紝鏀寔鏂囨湰瀵归綈璁剧疆
 */
void EGEAPI outtextrect(int x, int y, int w, int h, const wchar_t* text, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氱煩褰㈠尯鍩熷唴鏍煎紡鍖栬緭鍑烘枃鏈?
 * @param x 鐭╁舰鍖哄煙宸︿笂瑙抶鍧愭爣
 * @param y 鐭╁舰鍖哄煙宸︿笂瑙抷鍧愭爣
 * @param w 鐭╁舰鍖哄煙瀹藉害
 * @param h 鐭╁舰鍖哄煙楂樺害
 * @param format 鏍煎紡鍖栧瓧绗︿覆锛堢被浼紁rintf锛?
 * @param ... 鍙彉鍙傛暟鍒楄〃
 * @note 鏂囨湰浼氬湪鎸囧畾鐭╁舰鑼冨洿鍐呰嚜鍔ㄦ崲琛岋紝鏀寔鏂囨湰瀵归綈璁剧疆
 */
void EGEAPI rectprintf (int x, int y, int w, int h, const char*    format, ...);

/**
 * @brief 鍦ㄦ寚瀹氱煩褰㈠尯鍩熷唴鏍煎紡鍖栬緭鍑烘枃鏈紙Unicode鐗堟湰锛?
 * @param x 鐭╁舰鍖哄煙宸︿笂瑙抶鍧愭爣
 * @param y 鐭╁舰鍖哄煙宸︿笂瑙抷鍧愭爣
 * @param w 鐭╁舰鍖哄煙瀹藉害
 * @param h 鐭╁舰鍖哄煙楂樺害
 * @param format 鏍煎紡鍖栧瓧绗︿覆锛堢被浼紁rintf锛?
 * @param ... 鍙彉鍙傛暟鍒楄〃
 * @note 鏂囨湰浼氬湪鎸囧畾鐭╁舰鑼冨洿鍐呰嚜鍔ㄦ崲琛岋紝鏀寔鏂囨湰瀵归綈璁剧疆
 */
void EGEAPI rectprintf (int x, int y, int w, int h, const wchar_t* format, ...);

/**
 * @brief 鑾峰彇鏂囨湰瀛楃涓茬殑鏄剧ず瀹藉害
 * @param text 瑕佹祴閲忕殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 鏂囨湰鏄剧ず瀹藉害锛堝儚绱狅級
 * @note 杩斿洖鍊煎彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
int  EGEAPI textwidth(const char*    text, PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇鏂囨湰瀛楃涓茬殑鏄剧ず瀹藉害锛圲nicode鐗堟湰锛?
 * @param text 瑕佹祴閲忕殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 鏂囨湰鏄剧ず瀹藉害锛堝儚绱狅級
 * @note 杩斿洖鍊煎彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
int  EGEAPI textwidth(const wchar_t* text, PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇鍗曚釜瀛楃鐨勬樉绀哄搴?
 * @param c 瑕佹祴閲忕殑瀛楃
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 瀛楃鏄剧ず瀹藉害锛堝儚绱狅級
 * @note 杩斿洖鍊煎彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
int  EGEAPI textwidth(char    c, PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇鍗曚釜瀛楃鐨勬樉绀哄搴︼紙Unicode鐗堟湰锛?
 * @param c 瑕佹祴閲忕殑瀛楃
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 瀛楃鏄剧ず瀹藉害锛堝儚绱狅級
 * @note 杩斿洖鍊煎彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
int  EGEAPI textwidth(wchar_t c, PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇鏂囨湰瀛楃涓茬殑鏄剧ず楂樺害
 * @param text 瑕佹祴閲忕殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 鏂囨湰鏄剧ず楂樺害锛堝儚绱狅級
 * @note 杩斿洖鍊煎彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
int  EGEAPI textheight(const char*    text, PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇鏂囨湰瀛楃涓茬殑鏄剧ず楂樺害锛圲nicode鐗堟湰锛?
 * @param text 瑕佹祴閲忕殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 鏂囨湰鏄剧ず楂樺害锛堝儚绱狅級
 * @note 杩斿洖鍊煎彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
int  EGEAPI textheight(const wchar_t* text, PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇鍗曚釜瀛楃鐨勬樉绀洪珮搴?
 * @param c 瑕佹祴閲忕殑瀛楃
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 瀛楃鏄剧ず楂樺害锛堝儚绱狅級
 * @note 杩斿洖鍊煎彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
int  EGEAPI textheight(char    c, PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇鍗曚釜瀛楃鐨勬樉绀洪珮搴︼紙Unicode鐗堟湰锛?
 * @param c 瑕佹祴閲忕殑瀛楃
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @return 瀛楃鏄剧ず楂樺害锛堝儚绱狅級
 * @note 杩斿洖鍊煎彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
int  EGEAPI textheight(wchar_t c, PCIMAGE pimg = NULL);

/**
 * @brief 浣跨敤 GDI+ 绮剧‘娴嬮噺鏂囨湰瀛楃涓茬殑鏄剧ず瀹介珮
 * @param text 瑕佹祴閲忕殑鏂囨湰瀛楃涓?
 * @param width 杩斿洖鏂囨湰鏄剧ず瀹藉害锛堝儚绱狅級
 * @param height 杩斿洖鏂囨湰鏄剧ず楂樺害锛堝儚绱狅級
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠 EGE 绐楀彛
 * @note 鏈嚱鏁颁娇鐢?GDI+ 绮剧‘娴嬮噺锛岄€傜敤浜?ege_ 绯诲垪鏂囨湰缁樺埗鍑芥暟锛岀粨鏋滃彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
void EGEAPI measuretext(const char* text, float* width, float* height, PCIMAGE pimg = NULL);

/**
 * @brief 浣跨敤 GDI+ 绮剧‘娴嬮噺鏂囨湰瀛楃涓茬殑鏄剧ず瀹介珮锛圲nicode 鐗堟湰锛?
 * @param text 瑕佹祴閲忕殑鏂囨湰瀛楃涓?
 * @param width 杩斿洖鏂囨湰鏄剧ず瀹藉害锛堝儚绱狅級
 * @param height 杩斿洖鏂囨湰鏄剧ず楂樺害锛堝儚绱狅級
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠 EGE 绐楀彛
 * @note 鏈嚱鏁颁娇鐢?GDI+ 绮剧‘娴嬮噺锛岄€傜敤浜?ege_ 绯诲垪鏂囨湰缁樺埗鍑芥暟锛岀粨鏋滃彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
void EGEAPI measuretext(const wchar_t* text, float* width, float* height, PCIMAGE pimg = NULL);

/**
 * @brief 浣跨敤 GDI+ 绮剧‘娴嬮噺鍗曚釜瀛楃鐨勬樉绀哄楂?
 * @param c 瑕佹祴閲忕殑瀛楃
 * @param width 杩斿洖瀛楃鏄剧ず瀹藉害锛堝儚绱狅級
 * @param height 杩斿洖瀛楃鏄剧ず楂樺害锛堝儚绱狅級
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠 EGE 绐楀彛
 * @note 鏈嚱鏁颁娇鐢?GDI+ 绮剧‘娴嬮噺锛岄€傜敤浜?ege_ 绯诲垪鏂囨湰缁樺埗鍑芥暟锛岀粨鏋滃彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
void EGEAPI measuretext(char c, float* width, float* height, PCIMAGE pimg = NULL);

/**
 * @brief 浣跨敤 GDI+ 绮剧‘娴嬮噺鍗曚釜瀛楃鐨勬樉绀哄楂橈紙Unicode 鐗堟湰锛?
 * @param c 瑕佹祴閲忕殑瀛楃
 * @param width 杩斿洖瀛楃鏄剧ず瀹藉害锛堝儚绱狅級
 * @param height 杩斿洖瀛楃鏄剧ず楂樺害锛堝儚绱狅級
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠 EGE 绐楀彛
 * @note 鏈嚱鏁颁娇鐢?GDI+ 绮剧‘娴嬮噺锛岄€傜敤浜?ege_ 绯诲垪鏂囨湰缁樺埗鍑芥暟锛岀粨鏋滃彈褰撳墠瀛椾綋璁剧疆褰卞搷
 */
void EGEAPI measuretext(wchar_t c, float* width, float* height, PCIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃緭鍑烘枃鏈紙鏀寔娴偣鍧愭爣鍜孉RGB棰滆壊锛?
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣锛堟诞鐐规暟锛?
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣锛堟诞鐐规暟锛?
 * @param text 瑕佽緭鍑虹殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 浣跨敤GDI+娓叉煋锛屾敮鎸丄RGB棰滆壊鍜屾姉閿娇锛屽疄闄呰緭鍑轰綅缃彈鏂囨湰瀵归綈鏂瑰紡鍜屽瓧浣撳€炬枩瑙掑害褰卞搷
 */
void EGEAPI ege_outtextxy(float x, float y, const char*    text, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃緭鍑烘枃鏈紙鏀寔娴偣鍧愭爣鍜孉RGB棰滆壊锛孶nicode鐗堟湰锛?
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣锛堟诞鐐规暟锛?
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣锛堟诞鐐规暟锛?
 * @param text 瑕佽緭鍑虹殑鏂囨湰瀛楃涓?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 浣跨敤GDI+娓叉煋锛屾敮鎸丄RGB棰滆壊鍜屾姉閿娇锛屽疄闄呰緭鍑轰綅缃彈鏂囨湰瀵归綈鏂瑰紡鍜屽瓧浣撳€炬枩瑙掑害褰卞搷
 */
void EGEAPI ege_outtextxy(float x, float y, const wchar_t* text, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃緭鍑哄崟涓瓧绗︼紙鏀寔娴偣鍧愭爣鍜孉RGB棰滆壊锛?
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣锛堟诞鐐规暟锛?
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣锛堟诞鐐规暟锛?
 * @param c 瑕佽緭鍑虹殑瀛楃
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 浣跨敤GDI+娓叉煋锛屾敮鎸丄RGB棰滆壊鍜屾姉閿娇锛屽疄闄呰緭鍑轰綅缃彈鏂囨湰瀵归綈鏂瑰紡鍜屽瓧浣撳€炬枩瑙掑害褰卞搷
 */
void EGEAPI ege_outtextxy(float x, float y, char    c, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃緭鍑哄崟涓瓧绗︼紙鏀寔娴偣鍧愭爣鍜孉RGB棰滆壊锛孶nicode鐗堟湰锛?
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣锛堟诞鐐规暟锛?
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣锛堟诞鐐规暟锛?
 * @param c 瑕佽緭鍑虹殑瀛楃
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 浣跨敤GDI+娓叉煋锛屾敮鎸丄RGB棰滆壊鍜屾姉閿娇锛屽疄闄呰緭鍑轰綅缃彈鏂囨湰瀵归綈鏂瑰紡鍜屽瓧浣撳€炬枩瑙掑害褰卞搷
 */
void EGEAPI ege_outtextxy(float x, float y, wchar_t c, PIMAGE pimg = NULL);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃牸寮忓寲杈撳嚭鏂囨湰锛堟敮鎸佹诞鐐瑰潗鏍囧拰ARGB棰滆壊锛?
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣锛堟诞鐐规暟锛?
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣锛堟诞鐐规暟锛?
 * @param format 鏍煎紡鍖栧瓧绗︿覆锛堢被浼紁rintf锛?
 * @param ... 鍙彉鍙傛暟鍒楄〃
 * @note 浣跨敤GDI+娓叉煋锛屾敮鎸丄RGB棰滆壊鍜屾姉閿娇锛屽疄闄呰緭鍑轰綅缃彈鏂囨湰瀵归綈鏂瑰紡鍜屽瓧浣撳€炬枩瑙掑害褰卞搷
 */
void EGEAPI ege_xyprintf (float x, float y, const char*    format, ...);

/**
 * @brief 鍦ㄦ寚瀹氫綅缃牸寮忓寲杈撳嚭鏂囨湰锛堟敮鎸佹诞鐐瑰潗鏍囧拰ARGB棰滆壊锛孶nicode鐗堟湰锛?
 * @param x 杈撳嚭浣嶇疆鐨剎鍧愭爣锛堟诞鐐规暟锛?
 * @param y 杈撳嚭浣嶇疆鐨剏鍧愭爣锛堟诞鐐规暟锛?
 * @param format 鏍煎紡鍖栧瓧绗︿覆锛堢被浼紁rintf锛?
 * @param ... 鍙彉鍙傛暟鍒楄〃
 * @note 浣跨敤GDI+娓叉煋锛屾敮鎸丄RGB棰滆壊鍜屾姉閿娇锛屽疄闄呰緭鍑轰綅缃彈鏂囨湰瀵归綈鏂瑰紡鍜屽瓧浣撳€炬枩瑙掑害褰卞搷
 */
void EGEAPI ege_xyprintf (float x, float y, const wchar_t* format, ...);

/// @}

/**
 * @defgroup FontSettings 瀛椾綋璁剧疆
 * @brief 瀛椾綋鍜屾枃鏈榻愮浉鍏宠缃嚱鏁?
 * @{
 */

/**
 * @brief 璁剧疆鏂囨湰瀵归綈鏂瑰紡
 * @param horiz 姘村钩瀵归綈鏂瑰紡锛圠EFT_TEXT銆丆ENTER_TEXT銆丷IGHT_TEXT锛?
 * @param vert 鍨傜洿瀵归綈鏂瑰紡锛圱OP_TEXT銆丆ENTER_TEXT銆丅OTTOM_TEXT锛?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @note 褰卞搷鍚庣画鎵€鏈夋枃鏈緭鍑哄嚱鏁扮殑瀵归綈鏁堟灉
 */
void EGEAPI settextjustify(int horiz, int vert, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆瀛椾綋锛堢畝鍖栫増鏈級
 * @param height 瀛椾綋楂樺害锛堝儚绱狅級
 * @param width 瀛椾綋瀹藉害锛堝儚绱狅級锛?琛ㄧず鑷姩
 * @param typeface 瀛椾綋鍚嶇О
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setfont(int height, int width, const char* typeface,  PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆瀛椾綋锛堢畝鍖栫増鏈紝Unicode锛?
 * @param height 瀛椾綋楂樺害锛堝儚绱狅級
 * @param width 瀛椾綋瀹藉害锛堝儚绱狅級锛?琛ㄧず鑷姩
 * @param typeface 瀛椾綋鍚嶇О
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setfont(int height, int width, const wchar_t* typeface, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆瀛椾綋锛堝畬鏁寸増鏈級
 * @param height 瀛椾綋楂樺害锛堝儚绱狅級
 * @param width 瀛椾綋瀹藉害锛堝儚绱狅級锛?琛ㄧず鑷姩
 * @param typeface 瀛椾綋鍚嶇О
 * @param escapement 瀛椾綋鍊炬枩瑙掑害锛堝崄鍒嗕箣涓€搴︿负鍗曚綅锛?
 * @param orientation 瀛楃鍊炬枩瑙掑害锛堝崄鍒嗕箣涓€搴︿负鍗曚綅锛?
 * @param weight 瀛椾綋绮楃粏锛?00-900锛?00涓烘甯革紝700涓虹矖浣擄級
 * @param italic 鏄惁鏂滀綋
 * @param underline 鏄惁涓嬪垝绾?
 * @param strikeOut 鏄惁鍒犻櫎绾?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setfont(int height, int width, const char* typeface,  int escapement, int orientation,
                    int weight, bool italic, bool underline, bool strikeOut, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆瀛椾綋锛堝畬鏁寸増鏈紝Unicode锛?
 * @param height 瀛椾綋楂樺害锛堝儚绱狅級
 * @param width 瀛椾綋瀹藉害锛堝儚绱狅級锛?琛ㄧず鑷姩
 * @param typeface 瀛椾綋鍚嶇О
 * @param escapement 瀛椾綋鍊炬枩瑙掑害锛堝崄鍒嗕箣涓€搴︿负鍗曚綅锛?
 * @param orientation 瀛楃鍊炬枩瑙掑害锛堝崄鍒嗕箣涓€搴︿负鍗曚綅锛?
 * @param weight 瀛椾綋绮楃粏锛?00-900锛?00涓烘甯革紝700涓虹矖浣擄級
 * @param italic 鏄惁鏂滀綋
 * @param underline 鏄惁涓嬪垝绾?
 * @param strikeOut 鏄惁鍒犻櫎绾?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setfont(int height, int width, const wchar_t* typeface, int escapement, int orientation,
                    int weight, bool italic, bool underline, bool strikeOut, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆瀛椾綋锛堥珮绾х増鏈級
 * @param height 瀛椾綋楂樺害锛堝儚绱狅級
 * @param width 瀛椾綋瀹藉害锛堝儚绱狅級锛?琛ㄧず鑷姩
 * @param typeface 瀛椾綋鍚嶇О
 * @param escapement 瀛椾綋鍊炬枩瑙掑害锛堝崄鍒嗕箣涓€搴︿负鍗曚綅锛?
 * @param orientation 瀛楃鍊炬枩瑙掑害锛堝崄鍒嗕箣涓€搴︿负鍗曚綅锛?
 * @param weight 瀛椾綋绮楃粏锛?00-900锛?00涓烘甯革紝700涓虹矖浣擄級
 * @param italic 鏄惁鏂滀綋
 * @param underline 鏄惁涓嬪垝绾?
 * @param strikeOut 鏄惁鍒犻櫎绾?
 * @param charSet 瀛楃闆?
 * @param outPrecision 杈撳嚭绮惧害
 * @param clipPrecision 瑁佸壀绮惧害
 * @param quality 杈撳嚭璐ㄩ噺
 * @param pitchAndFamily 瀛椾綋闂磋窛鍜屾棌淇℃伅
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setfont(int height, int width, const char* typeface,  int escapement, int orientation,
                    int weight, bool italic, bool underline, bool strikeOut, BYTE charSet,
                    BYTE outPrecision, BYTE clipPrecision, BYTE quality, BYTE pitchAndFamily, PIMAGE pimg = NULL);

/**
 * @brief 璁剧疆瀛椾綋锛堥珮绾х増鏈紝Unicode锛?
 * @param height 瀛椾綋楂樺害锛堝儚绱狅級
 * @param width 瀛椾綋瀹藉害锛堝儚绱狅級锛?琛ㄧず鑷姩
 * @param typeface 瀛椾綋鍚嶇О
 * @param escapement 瀛椾綋鍊炬枩瑙掑害锛堝崄鍒嗕箣涓€搴︿负鍗曚綅锛?
 * @param orientation 瀛楃鍊炬枩瑙掑害锛堝崄鍒嗕箣涓€搴︿负鍗曚綅锛?
 * @param weight 瀛椾綋绮楃粏锛?00-900锛?00涓烘甯革紝700涓虹矖浣擄級
 * @param italic 鏄惁鏂滀綋
 * @param underline 鏄惁涓嬪垝绾?
 * @param strikeOut 鏄惁鍒犻櫎绾?
 * @param charSet 瀛楃闆?
 * @param outPrecision 杈撳嚭绮惧害
 * @param clipPrecision 瑁佸壀绮惧害
 * @param quality 杈撳嚭璐ㄩ噺
 * @param pitchAndFamily 瀛椾綋闂磋窛鍜屾棌淇℃伅
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setfont(int height, int width, const wchar_t* typeface, int escapement, int orientation,
                    int weight, bool italic, bool underline, bool strikeOut, BYTE charSet,
                    BYTE outPrecision, BYTE clipPrecision, BYTE quality, BYTE pitchAndFamily, PIMAGE pimg = NULL);

/**
 * @brief 浣跨敤LOGFONTW缁撴瀯璁剧疆瀛椾綋
 * @param font 鎸囧悜LOGFONTW缁撴瀯鐨勬寚閽?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI setfont(const LOGFONTW *font, PIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇褰撳墠瀛椾綋璁剧疆
 * @param font 鐢ㄤ簬鎺ユ敹瀛椾綋淇℃伅鐨凩OGFONTW缁撴瀯鎸囬拡
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 */
void EGEAPI getfont(LOGFONTW *font, PCIMAGE pimg = NULL);

/**
 * @brief 浣跨敤LOGFONTA缁撴瀯璁剧疆瀛椾綋
 * @param font 鎸囧悜LOGFONTA缁撴瀯鐨勬寚閽?
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @deprecated 寤鸿浣跨敤甯OGFONTW鍙傛暟鐨剆etfont鍑芥暟
 */
EGE_DEPRECATE(setfont, "Please use the 'getfont' function with the LOGFONTW* parameter instead.")
void EGEAPI setfont(const LOGFONTA *font, PIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇褰撳墠瀛椾綋璁剧疆锛圓NSI鐗堟湰锛?
 * @param font 鐢ㄤ簬鎺ユ敹瀛椾綋淇℃伅鐨凩OGFONTA缁撴瀯鎸囬拡
 * @param pimg 鐩爣鍥惧儚鎸囬拡锛孨ULL 琛ㄧず褰撳墠ege绐楀彛
 * @deprecated 寤鸿浣跨敤甯OGFONTW鍙傛暟鐨刧etfont鍑芥暟
 */
EGE_DEPRECATE(getfont, "Please use the 'getfont' function with the LOGFONTW* parameter instead.")
void EGEAPI getfont(LOGFONTA *font, PCIMAGE pimg = NULL);

/// @}

#define getmaxx getwidth
#define getmaxy getheight

/**
 * @brief 鑾峰彇绐楀彛鎴栧浘鍍忕殑瀹藉害
 * @param pimg 鍥惧儚瀵硅薄鎸囬拡锛孨ULL 琛ㄧず鑾峰彇褰撳墠缁樺浘绐楀彛鐨勫搴?
 * @return 杩斿洖绐楀彛鎴栧浘鍍忕殑瀹藉害锛堝儚绱狅級锛屽鏋滃浘鍍忓璞℃棤鏁堝垯杩斿洖0
 * 
 * 杩欎釜鍑芥暟鐢ㄤ簬鑾峰彇鎸囧畾鍥惧儚鎴栫獥鍙ｇ殑瀹藉害銆傚綋pimg涓篘ULL鏃讹紝
 * 杩斿洖褰撳墠EGE缁樺浘绐楀彛鐨勫搴︼紱褰損img鎸囧悜鏈夋晥鍥惧儚鏃讹紝杩斿洖璇ュ浘鍍忕殑瀹藉害銆?
 * 
 * @see getheight() 鑾峰彇楂樺害
 * @see getx() 鑾峰彇褰撳墠x鍧愭爣
 * @see gety() 鑾峰彇褰撳墠y鍧愭爣
 */
int EGEAPI getwidth(PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇绐楀彛鎴栧浘鍍忕殑楂樺害
 * @param pimg 鍥惧儚瀵硅薄鎸囬拡锛孨ULL 琛ㄧず鑾峰彇褰撳墠缁樺浘绐楀彛鐨勯珮搴?
 * @return 杩斿洖绐楀彛鎴栧浘鍍忕殑楂樺害锛堝儚绱狅級锛屽鏋滃浘鍍忓璞℃棤鏁堝垯杩斿洖0
 * 
 * 杩欎釜鍑芥暟鐢ㄤ簬鑾峰彇鎸囧畾鍥惧儚鎴栫獥鍙ｇ殑楂樺害銆傚綋pimg涓篘ULL鏃讹紝
 * 杩斿洖褰撳墠EGE缁樺浘绐楀彛鐨勯珮搴︼紱褰損img鎸囧悜鏈夋晥鍥惧儚鏃讹紝杩斿洖璇ュ浘鍍忕殑楂樺害銆?
 * 
 * @see getwidth() 鑾峰彇瀹藉害
 * @see getx() 鑾峰彇褰撳墠x鍧愭爣
 * @see gety() 鑾峰彇褰撳墠y鍧愭爣
 */
int EGEAPI getheight(PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇褰撳墠鐢荤瑪浣嶇疆鐨剎鍧愭爣
 * @param pimg 鍥惧儚瀵硅薄鎸囬拡锛孨ULL 琛ㄧず鑾峰彇褰撳墠缁樺浘绐楀彛鐨勭敾绗斾綅缃?
 * @return 杩斿洖褰撳墠鐢荤瑪浣嶇疆鐨剎鍧愭爣锛屽鏋滃浘鍍忓璞℃棤鏁堝垯杩斿洖-1
 * 
 * 鑾峰彇褰撳墠缁樺浘浣嶇疆鐨剎鍧愭爣銆傜敾绗斾綅缃€氬父鐢眒oveto()銆乴ineto()绛夊嚱鏁拌缃紝
 * 鎴栬€呯敱缁樺浘鍑芥暟锛堝line()锛夌殑鎵ц鑰屾敼鍙樸€?
 * 
 * @see gety() 鑾峰彇褰撳墠y鍧愭爣
 * @see moveto() 绉诲姩鐢荤瑪鍒版寚瀹氫綅缃?
 * @see getwidth() 鑾峰彇瀹藉害
 * @see getheight() 鑾峰彇楂樺害
 */
int EGEAPI getx(PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇褰撳墠鐢荤瑪浣嶇疆鐨剏鍧愭爣
 * @param pimg 鍥惧儚瀵硅薄鎸囬拡锛孨ULL 琛ㄧず鑾峰彇褰撳墠缁樺浘绐楀彛鐨勭敾绗斾綅缃?
 * @return 杩斿洖褰撳墠鐢荤瑪浣嶇疆鐨剏鍧愭爣锛屽鏋滃浘鍍忓璞℃棤鏁堝垯杩斿洖-1
 * 
 * 鑾峰彇褰撳墠缁樺浘浣嶇疆鐨剏鍧愭爣銆傜敾绗斾綅缃€氬父鐢眒oveto()銆乴ineto()绛夊嚱鏁拌缃紝
 * 鎴栬€呯敱缁樺浘鍑芥暟锛堝line()锛夌殑鎵ц鑰屾敼鍙樸€?
 * 
 * @see getx() 鑾峰彇褰撳墠x鍧愭爣
 * @see moveto() 绉诲姩鐢荤瑪鍒版寚瀹氫綅缃?
 * @see getwidth() 鑾峰彇瀹藉害
 * @see getheight() 鑾峰彇楂樺害
 */
int EGEAPI gety(PCIMAGE pimg = NULL);

/**
 * @brief 鍒涘缓涓€涓柊鐨勫浘鍍忓璞★紙1x1鍍忕礌锛?
 * @return 杩斿洖鏂板垱寤虹殑鍥惧儚瀵硅薄鎸囬拡锛屽け璐ユ椂杩斿洖NULL
 * 
 * 鍒涘缓涓€涓ぇ灏忎负1x1鍍忕礌鐨勫浘鍍忓璞★紝鑳屾櫙鑹蹭负榛戣壊銆?
 * 鍒涘缓鍚庣殑鍥惧儚闇€瑕佷娇鐢╠elimage()鍑芥暟閿€姣佷互闃叉鍐呭瓨娉勬紡銆?
 * 
 * @note 杩欎釜鍑芥暟鍒涘缓鐨勬槸鏈€灏忓昂瀵哥殑鍥惧儚锛岄€氬父鐢ㄤ簬鍚庣画璋冩暣澶у皬鎴栦綔涓哄崰浣嶇
 * @warning 蹇呴』浣跨敤delimage()閿€姣佸垱寤虹殑鍥惧儚锛屽惁鍒欎細閫犳垚鍐呭瓨娉勬紡
 * 
 * @see newimage(int, int) 鍒涘缓鎸囧畾澶у皬鐨勫浘鍍?
 * @see delimage() 閿€姣佸浘鍍忓璞?
 * @see resize() 璋冩暣鍥惧儚澶у皬
 */
PIMAGE         EGEAPI newimage();

/**
 * @brief 鍒涘缓鎸囧畾澶у皬鐨勬柊鍥惧儚瀵硅薄
 * @param width 鍥惧儚瀹藉害锛堝儚绱狅級锛屽皬浜?鏃惰嚜鍔ㄨ皟鏁翠负1
 * @param height 鍥惧儚楂樺害锛堝儚绱狅級锛屽皬浜?鏃惰嚜鍔ㄨ皟鏁翠负1
 * @return 杩斿洖鏂板垱寤虹殑鍥惧儚瀵硅薄鎸囬拡锛屽け璐ユ椂杩斿洖NULL
 * 
 * 鍒涘缓涓€涓寚瀹氬ぇ灏忕殑鍥惧儚瀵硅薄锛岃儗鏅壊涓洪粦鑹层€?
 * 濡傛灉鎸囧畾鐨勫搴︽垨楂樺害灏忎簬1锛屼細鑷姩璋冩暣涓?銆?
 * 鍒涘缓鍚庣殑鍥惧儚闇€瑕佷娇鐢╠elimage()鍑芥暟閿€姣佷互闃叉鍐呭瓨娉勬紡銆?
 * 
 * @warning 蹇呴』浣跨敤delimage()閿€姣佸垱寤虹殑鍥惧儚锛屽惁鍒欎細閫犳垚鍐呭瓨娉勬紡
 * 
 * @see newimage() 鍒涘缓1x1鍍忕礌鐨勫浘鍍?
 * @see delimage() 閿€姣佸浘鍍忓璞?
 * @see resize() 璋冩暣鍥惧儚澶у皬
 */
PIMAGE         EGEAPI newimage(int width, int height);

/**
 * @brief 閿€姣佸浘鍍忓璞″苟閲婃斁鍐呭瓨
 * @param pimg 瑕侀攢姣佺殑鍥惧儚瀵硅薄鎸囬拡锛屽鏋滀负NULL鍒欏拷鐣?
 * 
 * 閿€姣佺敱newimage()鍑芥暟鍒涘缓鐨勫浘鍍忓璞★紝閲婃斁鐩稿叧鐨勫唴瀛樺拰绯荤粺璧勬簮銆?
 * 閿€姣佸悗鐨勫浘鍍忔寚閽堜笉搴斿啀琚娇鐢ㄣ€?
 * 
 * @note 浼犲叆NULL鎸囬拡鏄畨鍏ㄧ殑锛屽嚱鏁颁細蹇界暐NULL鍙傛暟
 * @warning 閿€姣佸浘鍍忓悗锛屼笉瑕佸啀浣跨敤璇ュ浘鍍忔寚閽堬紝鍚﹀垯鍙兘瀵艰嚧绋嬪簭宕╂簝
 * 
 * @see newimage() 鍒涘缓鍥惧儚瀵硅薄
 * @see newimage(int, int) 鍒涘缓鎸囧畾澶у皬鐨勫浘鍍忓璞?
 */
void           EGEAPI delimage(PCIMAGE pimg);
//==================================================================================
// 鍥惧儚绠＄悊鍜屽鐞嗗嚱鏁?- EGE鍥惧舰搴撶殑鍥惧儚鎿嶄綔鍔熻兘
//==================================================================================
/**
 * @defgroup image_management 鍥惧儚绠＄悊鍜屽鐞嗗嚱鏁?
 * @brief EGE鍥惧舰搴撶殑鍥惧儚鍒涘缓銆佸姞杞姐€佷繚瀛樺拰澶勭悊鍔熻兘
 * 
 * 鍥惧儚绠＄悊妯″潡鎻愪緵浜嗗畬鏁寸殑鍥惧儚鎿嶄綔鑳藉姏锛屽寘鎷細
 * - 鍥惧儚缂撳啿鍖烘搷浣滐細getbuffer() 鑾峰彇鍍忕礌缂撳啿鍖?
 * - 鍥惧儚灏哄璋冩暣锛歳esize(), resize_f() 璋冩暣鍥惧儚澶у皬
 * - 鍥惧儚鑾峰彇锛歡etimage() 绯诲垪鍑芥暟浠庝笉鍚屾簮鑾峰彇鍥惧儚鏁版嵁
 * - 鍥惧儚淇濆瓨锛歴aveimage(), savepng(), savebmp() 淇濆瓨鍥惧儚鍒版枃浠?
 * 
 * 鏀寔鐨勫浘鍍忔牸寮忥細PNG, BMP, JPG, GIF, EMF, WMF, ICO
 * 鏀寔浠庣獥鍙ｃ€佹枃浠躲€佽祫婧愩€佸叾浠朓MAGE瀵硅薄鑾峰彇鍥惧儚
 * @{
 */

/**
 * @brief 鑾峰彇鍥惧儚鍍忕礌缂撳啿鍖烘寚閽?
 * @param pimg 瑕佽幏鍙栫紦鍐插尯鐨勫浘鍍忓璞℃寚閽堬紝榛樿涓?NULL锛堣〃绀虹獥鍙ｏ級
 * @return 鍥惧儚缂撳啿鍖洪鍦板潃锛岀紦鍐插尯涓轰竴缁存暟缁勶紝澶у皬涓?鍥惧儚瀹藉害脳鍥惧儚楂樺害
 * @note 鍧愭爣涓?x, y)鐨勫儚绱犲搴旂紦鍐插尯绱㈠紩锛歜uffer[y * width + x]
 * @note 杩斿洖鐨勬寚閽堝彲浠ョ洿鎺ユ搷浣滃儚绱犳暟鎹紝淇敼鍚庝細绔嬪嵆鐢熸晥
 */
color_t*       EGEAPI getbuffer(PIMAGE pimg);

/**
 * @brief 鑾峰彇鍥惧儚鍍忕礌缂撳啿鍖烘寚閽堬紙鍙鐗堟湰锛?
 * @param pimg 瑕佽幏鍙栫紦鍐插尯鐨勫浘鍍忓璞℃寚閽堬紝榛樿涓?NULL锛堣〃绀虹獥鍙ｏ級
 * @return 鍥惧儚缂撳啿鍖洪鍦板潃锛堝彧璇伙級锛岀紦鍐插尯涓轰竴缁存暟缁勶紝澶у皬涓?鍥惧儚瀹藉害脳鍥惧儚楂樺害
 * @note 鍧愭爣涓?x, y)鐨勫儚绱犲搴旂紦鍐插尯绱㈠紩锛歜uffer[y * width + x]
 * @note 杩斿洖鐨勬寚閽堝彧鑳借鍙栧儚绱犳暟鎹紝涓嶈兘淇敼
 */
const color_t* EGEAPI getbuffer(PCIMAGE pimg);

/**
 * @brief 璋冩暣鍥惧儚灏哄锛堝揩閫熺増鏈級
 * @param pimg 瑕佽皟鏁村ぇ灏忕殑鍥惧儚瀵硅薄鎸囬拡锛屼笉鑳戒负 NULL
 * @param width 鍥惧儚鏂板搴?
 * @param height 鍥惧儚鏂伴珮搴?
 * @return 鎴愬姛杩斿洖 0锛屽け璐ヨ繑鍥為潪 0 鍊?
 * @note 璋冩暣鍚庡浘鍍忓唴瀹规湭瀹氫箟锛岃鍙ｉ噸缃负鍒濆鐘舵€?
 * @note 姝ゅ嚱鏁伴€熷害杈冨揩锛屼絾涓嶄繚鐣欏師鍥惧儚鍐呭
 * @warning 濡傛灉 pimg 涓烘棤鏁堟寚閽堬紝浼氬紩鍙戣繍琛屾椂寮傚父
 */
int  EGEAPI resize_f(PIMAGE pimg, int width, int height);

/**
 * @brief 璋冩暣鍥惧儚灏哄锛堟爣鍑嗙増鏈級
 * @param pimg 瑕佽皟鏁村ぇ灏忕殑鍥惧儚瀵硅薄鎸囬拡锛屼笉鑳戒负 NULL
 * @param width 鍥惧儚鏂板搴?
 * @param height 鍥惧儚鏂伴珮搴?
 * @return 鎴愬姛杩斿洖 0锛屽け璐ヨ繑鍥為潪 0 鍊?
 * @note 璋冩暣鍚庡浘鍍忕敤鑳屾櫙鑹插～鍏咃紝瑙嗗彛閲嶇疆涓哄垵濮嬬姸鎬?
 * @note 姝ゅ嚱鏁颁細娓呯┖鍥惧儚鍐呭骞剁敤鑳屾櫙鑹插～鍏?
 * @warning 濡傛灉 pimg 涓烘棤鏁堟寚閽堬紝浼氬紩鍙戣繍琛屾椂寮傚父
 */
int  EGEAPI resize  (PIMAGE pimg, int width, int height);

/**
 * @brief 浠庣獥鍙ｈ幏鍙栧浘鍍?
 * @param imgDest 淇濆瓨鍥惧儚鐨?IMAGE 瀵硅薄鎸囬拡
 * @param xSrc 瑕佽幏鍙栧浘鍍忕殑鍖哄煙宸︿笂瑙?x 鍧愭爣
 * @param ySrc 瑕佽幏鍙栧浘鍍忕殑鍖哄煙宸︿笂瑙?y 鍧愭爣
 * @param widthSrc 瑕佽幏鍙栧浘鍍忕殑鍖哄煙瀹藉害
 * @param heightSrc 瑕佽幏鍙栧浘鍍忕殑鍖哄煙楂樺害
 * @return 鎴愬姛杩斿洖 grOk(0)锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 * @note 浠庡綋鍓嶇獥鍙ｇ殑鎸囧畾鍖哄煙鑾峰彇鍥惧儚鏁版嵁
 * @see getimage(PIMAGE, PCIMAGE, int, int, int, int)
 */
int  EGEAPI getimage(PIMAGE imgDest, int xSrc, int ySrc, int widthSrc, int heightSrc);

/**
 * @brief 浠庡彟涓€涓?IMAGE 瀵硅薄鑾峰彇鍥惧儚
 * @param imgDest 淇濆瓨鍥惧儚鐨?IMAGE 瀵硅薄鎸囬拡
 * @param imgSrc 婧愬浘鍍?IMAGE 瀵硅薄鎸囬拡
 * @param xSrc 瑕佽幏鍙栧浘鍍忕殑鍖哄煙宸︿笂瑙?x 鍧愭爣
 * @param ySrc 瑕佽幏鍙栧浘鍍忕殑鍖哄煙宸︿笂瑙?y 鍧愭爣
 * @param widthSrc 瑕佽幏鍙栧浘鍍忕殑鍖哄煙瀹藉害
 * @param heightSrc 瑕佽幏鍙栧浘鍍忕殑鍖哄煙楂樺害
 * @return 鎴愬姛杩斿洖 grOk(0)锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 * @note 浠庢簮 IMAGE 瀵硅薄鐨勬寚瀹氬尯鍩熷鍒跺浘鍍忔暟鎹埌鐩爣 IMAGE 瀵硅薄
 * @see getimage(PIMAGE, int, int, int, int)
 */
int  EGEAPI getimage(PIMAGE imgDest, PCIMAGE imgSrc, int xSrc, int ySrc, int widthSrc, int heightSrc);

/**
 * @brief 浠庡浘鐗囨枃浠惰幏鍙栧浘鍍忥紙char* 鐗堟湰锛?
 * @param imgDest 淇濆瓨鍥惧儚鐨?IMAGE 瀵硅薄鎸囬拡
 * @param imageFile 鍥剧墖鏂囦欢鍚?
 * @param zoomWidth 璁惧畾鍥惧儚缂╂斁鑷崇殑瀹藉害锛? 琛ㄧず浣跨敤鍘熷瀹藉害锛屼笉缂╂斁
 * @param zoomHeight 璁惧畾鍥惧儚缂╂斁鑷崇殑楂樺害锛? 琛ㄧず浣跨敤鍘熷楂樺害锛屼笉缂╂斁
 * @return 鎴愬姛杩斿洖 grOk(0)锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜锛坓rAllocError/grFileNotFound/grNullPointer/grIOerror锛?
 * @note 鏀寔鏍煎紡锛歅NG, BMP, JPG, GIF, EMF, WMF, ICO
 * @note 濡傛灉鍥惧儚鍖呭惈澶氬抚锛屼粎鑾峰彇绗竴甯?
 * @see getimage(PIMAGE, const wchar_t*, int, int)
 */
int  EGEAPI getimage(PIMAGE imgDest, const char*  imageFile, int zoomWidth = 0, int zoomHeight = 0);

/**
 * @brief 浠庡浘鐗囨枃浠惰幏鍙栧浘鍍忥紙wchar_t* 鐗堟湰锛?
 * @param imgDest 淇濆瓨鍥惧儚鐨?IMAGE 瀵硅薄鎸囬拡
 * @param imageFile 鍥剧墖鏂囦欢鍚嶏紙瀹藉瓧绗︾増鏈級
 * @param zoomWidth 璁惧畾鍥惧儚缂╂斁鑷崇殑瀹藉害锛? 琛ㄧず浣跨敤鍘熷瀹藉害锛屼笉缂╂斁
 * @param zoomHeight 璁惧畾鍥惧儚缂╂斁鑷崇殑楂樺害锛? 琛ㄧず浣跨敤鍘熷楂樺害锛屼笉缂╂斁
 * @return 鎴愬姛杩斿洖 grOk(0)锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜锛坓rAllocError/grFileNotFound/grNullPointer/grIOerror锛?
 * @note 鏀寔鏍煎紡锛歅NG, BMP, JPG, GIF, EMF, WMF, ICO
 * @note 濡傛灉鍥惧儚鍖呭惈澶氬抚锛屼粎鑾峰彇绗竴甯?
 * @see getimage(PIMAGE, const char*, int, int)
 */
int  EGEAPI getimage(PIMAGE imgDest, const wchar_t* imageFile, int zoomWidth = 0, int zoomHeight = 0);

/**
 * @brief 浠庤祫婧愭枃浠惰幏鍙栧浘鍍忥紙char* 鐗堟湰锛?
 * @param imgDest 淇濆瓨鍥惧儚鐨?IMAGE 瀵硅薄鎸囬拡
 * @param resType 璧勬簮绫诲瀷
 * @param resName 璧勬簮鍚嶇О
 * @param zoomWidth 璁惧畾鍥惧儚缂╂斁鑷崇殑瀹藉害锛? 琛ㄧず浣跨敤鍘熷瀹藉害锛屼笉缂╂斁
 * @param zoomHeight 璁惧畾鍥惧儚缂╂斁鑷崇殑楂樺害锛? 琛ㄧず浣跨敤鍘熷楂樺害锛屼笉缂╂斁
 * @return 鎴愬姛杩斿洖 grOk(0)锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜锛坓rAllocError/grFileNotFound/grNullPointer/grIOerror锛?
 * @note 鏀寔鏍煎紡锛歅NG, BMP, JPG, GIF, EMF, WMF, ICO
 * @note 濡傛灉鍥惧儚鍖呭惈澶氬抚锛屼粎鑾峰彇绗竴甯?
 * @see getimage(PIMAGE, const wchar_t*, const wchar_t*, int, int)
 */
int  EGEAPI getimage(PIMAGE imgDest, const char*  resType, const char*  resName, int zoomWidth = 0, int zoomHeight = 0);

/**
 * @brief 浠庤祫婧愭枃浠惰幏鍙栧浘鍍忥紙wchar_t* 鐗堟湰锛?
 * @param imgDest 淇濆瓨鍥惧儚鐨?IMAGE 瀵硅薄鎸囬拡
 * @param resType 璧勬簮绫诲瀷锛堝瀛楃鐗堟湰锛?
 * @param resName 璧勬簮鍚嶇О锛堝瀛楃鐗堟湰锛?
 * @param zoomWidth 璁惧畾鍥惧儚缂╂斁鑷崇殑瀹藉害锛? 琛ㄧず浣跨敤鍘熷瀹藉害锛屼笉缂╂斁
 * @param zoomHeight 璁惧畾鍥惧儚缂╂斁鑷崇殑楂樺害锛? 琛ㄧず浣跨敤鍘熷楂樺害锛屼笉缂╂斁
 * @return 鎴愬姛杩斿洖 grOk(0)锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜锛坓rAllocError/grFileNotFound/grNullPointer/grIOerror锛?
 * @note 鏀寔鏍煎紡锛歅NG, BMP, JPG, GIF, EMF, WMF, ICO
 * @note 濡傛灉鍥惧儚鍖呭惈澶氬抚锛屼粎鑾峰彇绗竴甯?
 * @see getimage(PIMAGE, const char*, const char*, int, int)
 */
int  EGEAPI getimage(PIMAGE imgDest, const wchar_t* resType, const wchar_t* resName, int zoomWidth = 0, int zoomHeight = 0);

/**
 * @brief 浠?PNG 鍥剧墖鏂囦欢鑾峰彇鍥惧儚锛坈har* 鐗堟湰锛?
 * @param pimg 鍥惧儚瀵硅薄鎸囬拡锛岄渶瑕佸厛浣跨敤 newimage() 鍒涘缓
 * @param filename 鍥惧儚鏂囦欢鍚?
 * @return 鎴愬姛杩斿洖 0锛屽け璐ヨ繑鍥為潪 0 鍊?
 * @note 鑾峰彇鍚庡浘鍍忓ぇ灏忎笌鍘熷浘澶у皬涓€鑷达紝鑰屼笉鏄浘鍍忓師鍏堢殑澶у皬
 * @note 涓撻棬鐢ㄤ簬澶勭悊 PNG 鏍煎紡鍥惧儚
 * @warning 濡傛灉 pimg 涓烘棤鏁堟寚閽堬紝浼氬紩鍙戣繍琛屾椂寮傚父
 * @see getimage_pngfile(PIMAGE, const wchar_t*)
 */
int  EGEAPI getimage_pngfile(PIMAGE pimg, const char*  filename);

/**
 * @brief 浠?PNG 鍥剧墖鏂囦欢鑾峰彇鍥惧儚锛坵char_t* 鐗堟湰锛?
 * @param pimg 鍥惧儚瀵硅薄鎸囬拡锛岄渶瑕佸厛浣跨敤 newimage() 鍒涘缓
 * @param filename 鍥惧儚鏂囦欢鍚嶏紙瀹藉瓧绗︾増鏈級
 * @return 鎴愬姛杩斿洖 0锛屽け璐ヨ繑鍥為潪 0 鍊?
 * @note 鑾峰彇鍚庡浘鍍忓ぇ灏忎笌鍘熷浘澶у皬涓€鑷达紝鑰屼笉鏄浘鍍忓師鍏堢殑澶у皬
 * @note 涓撻棬鐢ㄤ簬澶勭悊 PNG 鏍煎紡鍥惧儚
 * @warning 濡傛灉 pimg 涓烘棤鏁堟寚閽堬紝浼氬紩鍙戣繍琛屾椂寮傚父
 * @see getimage_pngfile(PIMAGE, const char*)
 */
int  EGEAPI getimage_pngfile(PIMAGE pimg, const wchar_t* filename);

//==================================================================================
// putimage 绯诲垪鍑芥暟 - EGE鍥惧舰搴撶殑鏍稿績鍥惧儚缁樺埗鍔熻兘
//==================================================================================
/**
 * @defgroup putimage_functions putimage绯诲垪鍑芥暟
 * @brief EGE鍥惧舰搴撶殑鏍稿績鍥惧儚缁樺埗鍔熻兘
 * 
 * putimage绯诲垪鍑芥暟鎻愪緵浜嗕赴瀵岀殑鍥惧儚缁樺埗鍜屽鐞嗚兘鍔涳紝鍖呮嫭锛?
 * - 鍩虹鍥惧儚缁樺埗锛歱utimage() 绯诲垪閲嶈浇鍑芥暟
 * - 閫忔槑鏁堟灉锛歱utimage_transparent(), putimage_alphablend(), putimage_withalpha()
 * - 娣峰悎鏁堟灉锛歱utimage_alphatransparent(), putimage_alphafilter()
 * - 鍙樻崲鏁堟灉锛歱utimage_rotate(), putimage_rotatezoom(), putimage_rotatetransparent()
 * 
 * 杩欎簺鍑芥暟鏀寔澶氱缁樺埗妯″紡锛?
 * - 缁樺埗鍒板睆骞曪紙imgDest = NULL锛夋垨鍙︿竴涓浘鍍?
 * - 鏀寔鍖哄煙瑁佸壀銆佹媺浼哥缉鏀俱€佹棆杞彉鎹?
 * - 鏀寔澶氱閫忔槑鍜屾贩鍚堟ā寮?
 * - 鏀寔鍏夋爡鎿嶄綔鐮侊紙SRCCOPY, SRCAND, SRCPAINT绛夛級
 * - 鏀寔骞虫粦澶勭悊锛堟姉閿娇锛?
 * @{
 */

/**
 * @brief 鍩虹鍥惧儚缁樺埗鍑芥暟 - 鍦ㄦ寚瀹氫綅缃粯鍒舵暣涓浘鍍?
 * @param x 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param y 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param pimg 瑕佺粯鍒剁殑 IMAGE 瀵硅薄鎸囬拡
 * @param dwRop 涓夊厓鍏夋爡鎿嶄綔鐮侊紝榛樿涓?SRCCOPY锛堢洿鎺ュ鍒讹級
 */
void EGEAPI putimage(int x, int y, PCIMAGE pimg, DWORD dwRop = SRCCOPY);

/**
 * @brief 鍖哄煙鍥惧儚缁樺埗鍑芥暟 - 缁樺埗鍥惧儚鐨勬寚瀹氬尯鍩?
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param widthDest 缁樺埗鐨勫搴?
 * @param heightDest 缁樺埗鐨勯珮搴?
 * @param imgSrc 瑕佺粯鍒剁殑 IMAGE 瀵硅薄鎸囬拡
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣
 * @param dwRop 涓夊厓鍏夋爡鎿嶄綔鐮侊紝榛樿涓?SRCCOPY锛堢洿鎺ュ鍒讹級
 */
void EGEAPI putimage(int xDest, int yDest, int widthDest, int heightDest, PCIMAGE imgSrc, int xSrc, int ySrc, DWORD dwRop = SRCCOPY);

/**
 * @brief 鎷変几鍥惧儚缁樺埗鍑芥暟 - 灏嗘簮鍥惧儚鍖哄煙鎷変几鍒扮洰鏍囧尯鍩?
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param widthDest 缁樺埗鐨勫搴?
 * @param heightDest 缁樺埗鐨勯珮搴?
 * @param imgSrc 瑕佺粯鍒剁殑 IMAGE 瀵硅薄鎸囬拡
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣
 * @param widthSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑瀹藉害
 * @param heightSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑楂樺害
 * @param dwRop 涓夊厓鍏夋爡鎿嶄綔鐮侊紝榛樿涓?SRCCOPY锛堢洿鎺ュ鍒讹級
 */
void EGEAPI putimage(int xDest, int yDest, int widthDest, int heightDest, PCIMAGE imgSrc, int xSrc, int ySrc, int widthSrc, int heightSrc, DWORD dwRop = SRCCOPY);

/**
 * @brief 鍥惧儚鍒板浘鍍忕粯鍒跺嚱鏁?- 鍦ㄥ彟涓€涓浘鍍忎笂缁樺埗鍥惧儚
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param dwRop 涓夊厓鍏夋爡鎿嶄綔鐮侊紝榛樿涓?SRCCOPY锛堢洿鎺ュ鍒讹級
 */
void EGEAPI putimage(PIMAGE imgDest, int xDest, int yDest, PCIMAGE imgSrc, DWORD dwRop = SRCCOPY);

/**
 * @brief 鍥惧儚鍒板浘鍍忓尯鍩熺粯鍒跺嚱鏁?- 鍦ㄥ彟涓€涓浘鍍忎笂缁樺埗鍥惧儚鐨勬寚瀹氬尯鍩?
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param widthDest 缁樺埗鐨勫搴?
 * @param heightDest 缁樺埗鐨勯珮搴?
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣
 * @param dwRop 涓夊厓鍏夋爡鎿嶄綔鐮侊紝榛樿涓?SRCCOPY锛堢洿鎺ュ鍒讹級
 */
void EGEAPI putimage(PIMAGE imgDest, int xDest, int yDest, int widthDest, int heightDest, PCIMAGE imgSrc, int xSrc, int ySrc, DWORD dwRop = SRCCOPY);

/**
 * @brief 鍥惧儚鍒板浘鍍忔媺浼哥粯鍒跺嚱鏁?- 鍦ㄥ彟涓€涓浘鍍忎笂鎷変几缁樺埗鍥惧儚
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param widthDest 缁樺埗鐨勫搴?
 * @param heightDest 缁樺埗鐨勯珮搴?
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣
 * @param widthSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑瀹藉害
 * @param heightSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑楂樺害
 * @param dwRop 涓夊厓鍏夋爡鎿嶄綔鐮侊紝榛樿涓?SRCCOPY锛堢洿鎺ュ鍒讹級
 */
void EGEAPI putimage(PIMAGE imgDest, int xDest, int yDest, int widthDest, int heightDest, PCIMAGE imgSrc, int xSrc, int ySrc, int widthSrc, int heightSrc, DWORD dwRop = SRCCOPY);

/**
 * @brief 灏嗗浘鍍忎繚瀛樺埌鏂囦欢锛坈har* 鐗堟湰锛?
 * @param pimg 瑕佷繚瀛樼殑鍥惧儚瀵硅薄鎸囬拡
 * @param filename 淇濆瓨鐨勫浘鍍忔枃浠跺悕
 * @param withAlphaChannel 鏄惁淇濆瓨鍥惧儚鐨勯€忔槑閫氶亾锛宼rue: 淇濆瓨, false: 涓嶄繚瀛橈紝榛樿涓?false
 * @return 鎴愬姛杩斿洖 grOk(0)锛屽け璐ヨ繑鍥為潪 0 鍊?
 * @note 鐩墠鏀寔 BMP 鍜?PNG 鏍煎紡
 * @note 鏂囦欢鍚嶄互 .bmp 缁撳熬淇濆瓨涓?BMP 鏍煎紡锛屼互 .png 缁撳熬鎴栨棤鍚庣紑鍚嶄繚瀛樹负 PNG 鏍煎紡
 * @note 濡傛灉鏂囦欢宸插瓨鍦紝浼氳鐩栧師鏂囦欢
 * @warning 濡傛灉 pimg 涓烘棤鏁堟寚閽堬紝浼氬紩鍙戣繍琛屾椂寮傚父
 * @see saveimage(PCIMAGE, const wchar_t*, bool)
 */
int  EGEAPI saveimage(PCIMAGE pimg, const char*  filename, bool withAlphaChannel = false);

/**
 * @brief 灏嗗浘鍍忎繚瀛樺埌鏂囦欢锛坵char_t* 鐗堟湰锛?
 * @param pimg 瑕佷繚瀛樼殑鍥惧儚瀵硅薄鎸囬拡
 * @param filename 淇濆瓨鐨勫浘鍍忔枃浠跺悕锛堝瀛楃鐗堟湰锛?
 * @param withAlphaChannel 鏄惁淇濆瓨鍥惧儚鐨勯€忔槑閫氶亾锛宼rue: 淇濆瓨, false: 涓嶄繚瀛橈紝榛樿涓?false
 * @return 鎴愬姛杩斿洖 grOk(0)锛屽け璐ヨ繑鍥為潪 0 鍊?
 * @note 鐩墠鏀寔 BMP 鍜?PNG 鏍煎紡
 * @note 鏂囦欢鍚嶄互 .bmp 缁撳熬淇濆瓨涓?BMP 鏍煎紡锛屼互 .png 缁撳熬鎴栨棤鍚庣紑鍚嶄繚瀛樹负 PNG 鏍煎紡
 * @note 濡傛灉鏂囦欢宸插瓨鍦紝浼氳鐩栧師鏂囦欢
 * @warning 濡傛灉 pimg 涓烘棤鏁堟寚閽堬紝浼氬紩鍙戣繍琛屾椂寮傚父
 * @see saveimage(PCIMAGE, const char*, bool)
 */
int  EGEAPI saveimage(PCIMAGE pimg, const wchar_t* filename, bool withAlphaChannel = false);

/**
 * @brief 灏嗗浘鍍忎互 PNG 鏍煎紡淇濆瓨鍒版枃浠讹紙char* 鐗堟湰锛?
 * @param pimg 瑕佷繚瀛樼殑鍥惧儚瀵硅薄鎸囬拡
 * @param filename 淇濆瓨鐨勫浘鍍忔枃浠跺悕
 * @param withAlphaChannel 鏄惁淇濆瓨鍥惧儚鐨勯€忔槑閫氶亾锛宼rue: 淇濆瓨, false: 涓嶄繚瀛橈紝榛樿涓?false
 * @return 鎴愬姛杩斿洖 0锛屽け璐ヨ繑鍥為潪 0 鍊?
 * @note 寮哄埗浠?PNG 鏍煎紡淇濆瓨锛屼笉鍙楁枃浠跺悕鍚庣紑褰卞搷
 * @note 濡傛灉鏂囦欢宸插瓨鍦紝浼氳鐩栧師鏂囦欢
 * @warning 濡傛灉 pimg 涓烘棤鏁堟寚閽堬紝浼氬紩鍙戣繍琛屾椂寮傚父
 * @see savepng(PCIMAGE, const wchar_t*, bool)
 */
int  EGEAPI savepng  (PCIMAGE pimg, const char*  filename, bool withAlphaChannel = false);

/**
 * @brief 灏嗗浘鍍忎互 PNG 鏍煎紡淇濆瓨鍒版枃浠讹紙wchar_t* 鐗堟湰锛?
 * @param pimg 瑕佷繚瀛樼殑鍥惧儚瀵硅薄鎸囬拡
 * @param filename 淇濆瓨鐨勫浘鍍忔枃浠跺悕锛堝瀛楃鐗堟湰锛?
 * @param withAlphaChannel 鏄惁淇濆瓨鍥惧儚鐨勯€忔槑閫氶亾锛宼rue: 淇濆瓨, false: 涓嶄繚瀛橈紝榛樿涓?false
 * @return 鎴愬姛杩斿洖 0锛屽け璐ヨ繑鍥為潪 0 鍊?
 * @note 寮哄埗浠?PNG 鏍煎紡淇濆瓨锛屼笉鍙楁枃浠跺悕鍚庣紑褰卞搷
 * @note 濡傛灉鏂囦欢宸插瓨鍦紝浼氳鐩栧師鏂囦欢
 * @warning 濡傛灉 pimg 涓烘棤鏁堟寚閽堬紝浼氬紩鍙戣繍琛屾椂寮傚父
 * @see savepng(PCIMAGE, const char*, bool)
 */
int  EGEAPI savepng  (PCIMAGE pimg, const wchar_t* filename, bool withAlphaChannel = false);

/**
 * @brief 灏嗗浘鍍忎互 BMP 鏍煎紡淇濆瓨鍒版枃浠讹紙char* 鐗堟湰锛?
 * @param pimg 瑕佷繚瀛樼殑鍥惧儚瀵硅薄鎸囬拡
 * @param filename 淇濆瓨鐨勫浘鍍忔枃浠跺悕
 * @param withAlphaChannel 鏄惁淇濆瓨鍥惧儚鐨勯€忔槑閫氶亾锛宼rue: 淇濆瓨, false: 涓嶄繚瀛橈紝榛樿涓?false
 * @return 鎴愬姛杩斿洖 0锛屽け璐ヨ繑鍥為潪 0 鍊?
 * @note 寮哄埗浠?BMP 鏍煎紡淇濆瓨锛屼笉鍙楁枃浠跺悕鍚庣紑褰卞搷
 * @note 濡傛灉鏂囦欢宸插瓨鍦紝浼氳鐩栧師鏂囦欢
 * @warning 濡傛灉 pimg 涓烘棤鏁堟寚閽堬紝浼氬紩鍙戣繍琛屾椂寮傚父
 * @see savebmp(PCIMAGE, const wchar_t*, bool)
 */
int  EGEAPI savebmp  (PCIMAGE pimg, const char*  filename, bool withAlphaChannel = false);

/**
 * @brief 灏嗗浘鍍忎互 BMP 鏍煎紡淇濆瓨鍒版枃浠讹紙wchar_t* 鐗堟湰锛?
 * @param pimg 瑕佷繚瀛樼殑鍥惧儚瀵硅薄鎸囬拡
 * @param filename 淇濆瓨鐨勫浘鍍忔枃浠跺悕锛堝瀛楃鐗堟湰锛?
 * @param withAlphaChannel 鏄惁淇濆瓨鍥惧儚鐨勯€忔槑閫氶亾锛宼rue: 淇濆瓨, false: 涓嶄繚瀛橈紝榛樿涓?false
 * @return 鎴愬姛杩斿洖 0锛屽け璐ヨ繑鍥為潪 0 鍊?
 * @note 寮哄埗浠?BMP 鏍煎紡淇濆瓨锛屼笉鍙楁枃浠跺悕鍚庣紑褰卞搷
 * @note 濡傛灉鏂囦欢宸插瓨鍦紝浼氳鐩栧師鏂囦欢
 * @warning 濡傛灉 pimg 涓烘棤鏁堟寚閽堬紝浼氬紩鍙戣繍琛屾椂寮傚父
 * @see savebmp(PCIMAGE, const char*, bool)
 */
int  EGEAPI savebmp  (PCIMAGE pimg, const wchar_t* filename, bool withAlphaChannel = false);

/**
 * @brief 閫忔槑鑹茬粯鍒跺嚱鏁?- 鎸囧畾棰滆壊鍙樹负閫忔槑鐨勫浘鍍忕粯鍒?
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param transparentColor 瑕佸彉涓洪€忔槑鐨勫儚绱犻鑹?
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣锛岄粯璁や负 0
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣锛岄粯璁や负 0
 * @param widthSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑瀹藉害锛岄粯璁や负 0锛堜娇鐢ㄦ暣涓浘鍍忓搴︼級
 * @param heightSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑楂樺害锛岄粯璁や负 0锛堜娇鐢ㄦ暣涓浘鍍忛珮搴︼級
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_transparent(
    PIMAGE  imgDest,            // handle to dest
    PCIMAGE imgSrc,             // handle to source
    int xDest,                  // x-coord of destination upper-left corner
    int yDest,                  // y-coord of destination upper-left corner
    color_t transparentColor,   // color to make transparent
    int xSrc = 0,               // x-coord of source upper-left corner
    int ySrc = 0,               // y-coord of source upper-left corner
    int widthSrc = 0,           // width of source rectangle    
    int heightSrc = 0           // height of source rectangle
);

/**
 * @brief Alpha娣峰悎缁樺埗鍑芥暟 - 鍩虹鐗堟湰锛屾寚瀹氭暣浣撻€忔槑搴?
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param alpha 鍥惧儚鏁翠綋閫忔槑搴?(0-255)锛?涓哄畬鍏ㄩ€忔槑锛?55涓哄畬鍏ㄤ笉閫忔槑
 * @param alphaType 婧愬浘鍍忓儚绱犵殑 alpha 绫诲瀷锛岄粯璁や负 ALPHATYPE_STRAIGHT
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_alphablend(
    PIMAGE  imgDest,
    PCIMAGE imgSrc,
    int xDest,
    int yDest,
    unsigned char alpha,
    alpha_type alphaType = ALPHATYPE_STRAIGHT
);

/**
 * @brief Alpha娣峰悎缁樺埗鍑芥暟 - 鎸囧畾婧愬浘鍍忚捣濮嬩綅缃?
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param alpha 鍥惧儚鏁翠綋閫忔槑搴?(0-255)锛?涓哄畬鍏ㄩ€忔槑锛?55涓哄畬鍏ㄤ笉閫忔槑
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣
 * @param alphaType 婧愬浘鍍忓儚绱犵殑 alpha 绫诲瀷锛岄粯璁や负 ALPHATYPE_STRAIGHT
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_alphablend(
    PIMAGE  imgDest,
    PCIMAGE imgSrc,
    int xDest,
    int yDest,
    unsigned char alpha,
    int xSrc,
    int ySrc,
    alpha_type alphaType = ALPHATYPE_STRAIGHT
);

/**
 * @brief Alpha娣峰悎缁樺埗鍑芥暟 - 鎸囧畾婧愬浘鍍忓尯鍩?
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param alpha 鍥惧儚鏁翠綋閫忔槑搴?(0-255)锛?涓哄畬鍏ㄩ€忔槑锛?55涓哄畬鍏ㄤ笉閫忔槑
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣
 * @param widthSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑瀹藉害
 * @param heightSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑楂樺害
 * @param alphaType 婧愬浘鍍忓儚绱犵殑 alpha 绫诲瀷锛岄粯璁や负 ALPHATYPE_STRAIGHT
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_alphablend(
    PIMAGE  imgDest,
    PCIMAGE imgSrc,
    int xDest,
    int yDest,
    unsigned char alpha,
    int xSrc,
    int ySrc,
    int widthSrc,
    int heightSrc,
    alpha_type alphaType = ALPHATYPE_STRAIGHT
);

/**
 * @brief Alpha娣峰悎缁樺埗鍑芥暟 - 瀹屾暣鐗堟湰锛屾敮鎸佹媺浼稿拰骞虫粦澶勭悊
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param widthDest 缁樺埗鐨勫搴?
 * @param heightDest 缁樺埗鐨勯珮搴?
 * @param alpha 鍥惧儚鏁翠綋閫忔槑搴?(0-255)锛?涓哄畬鍏ㄩ€忔槑锛?55涓哄畬鍏ㄤ笉閫忔槑
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣
 * @param widthSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑瀹藉害
 * @param heightSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑楂樺害
 * @param smooth 鏄惁浣跨敤骞虫粦澶勭悊锛堟姉閿娇锛夛紝榛樿涓?false
 * @param alphaType 婧愬浘鍍忓儚绱犵殑 alpha 绫诲瀷锛岄粯璁や负 ALPHATYPE_STRAIGHT
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_alphablend(
    PIMAGE  imgDest,
    PCIMAGE imgSrc,
    int xDest,
    int yDest,
    int widthDest,
    int heightDest,
    unsigned char alpha,
    int xSrc,
    int ySrc,
    int widthSrc,
    int heightSrc,
    bool smooth = false,
    alpha_type alphaType = ALPHATYPE_STRAIGHT
);

/**
 * @brief Alpha閫忔槑鑹叉贩鍚堢粯鍒跺嚱鏁?- 缁撳悎閫忔槑鑹插拰Alpha娣峰悎
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param transparentColor 瑕佸彉涓洪€忔槑鐨勫儚绱犻鑹?
 * @param alpha 鍥惧儚鏁翠綋閫忔槑搴?(0-255)锛?涓哄畬鍏ㄩ€忔槑锛?55涓哄畬鍏ㄤ笉閫忔槑
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣锛岄粯璁や负 0
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣锛岄粯璁や负 0
 * @param widthSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑瀹藉害锛岄粯璁や负 0锛堜娇鐢ㄦ暣涓浘鍍忓搴︼級
 * @param heightSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑楂樺害锛岄粯璁や负 0锛堜娇鐢ㄦ暣涓浘鍍忛珮搴︼級
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_alphatransparent(
    PIMAGE  imgDest,            // handle to dest
    PCIMAGE imgSrc,             // handle to source
    int xDest,                  // x-coord of destination upper-left corner
    int yDest,                  // y-coord of destination upper-left corner
    color_t transparentColor,   // color to make transparent
    unsigned char alpha,        // alpha
    int xSrc = 0,               // x-coord of source upper-left corner
    int ySrc = 0,               // y-coord of source upper-left corner
    int widthSrc = 0,           // width of source rectangle
    int heightSrc = 0           // height of source rectangle
);

/**
 * @brief Alpha閫氶亾缁樺埗鍑芥暟 - 浣跨敤鍥惧儚鑷韩鐨凙lpha閫氶亾锛屽熀纭€鐗堟湰
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡锛堝繀椤诲寘鍚獳lpha閫氶亾鏁版嵁锛?
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣锛岄粯璁や负 0
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣锛岄粯璁や负 0
 * @param widthSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑瀹藉害锛岄粯璁や负 0锛堜娇鐢ㄦ暣涓浘鍍忓搴︼級
 * @param heightSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑楂樺害锛岄粯璁や负 0锛堜娇鐢ㄦ暣涓浘鍍忛珮搴︼級
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_withalpha(
    PIMAGE  imgDest,            // handle to dest
    PCIMAGE imgSrc,             // handle to source
    int xDest,                  // x-coord of destination upper-left corner
    int yDest,                  // y-coord of destination upper-left corner
    int xSrc = 0,               // x-coord of source upper-left corner
    int ySrc = 0,               // y-coord of source upper-left corner
    int widthSrc = 0,           // width of source rectangle
    int heightSrc = 0           // height of source rectangle
);

/**
 * @brief Alpha閫氶亾缁樺埗鍑芥暟 - 浣跨敤鍥惧儚鑷韩鐨凙lpha閫氶亾锛屾敮鎸佹媺浼稿拰骞虫粦澶勭悊
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡锛堝繀椤诲寘鍚獳lpha閫氶亾鏁版嵁锛?
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param widthDest 缁樺埗鐨勫搴?
 * @param heightDest 缁樺埗鐨勯珮搴?
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣
 * @param widthSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑瀹藉害
 * @param heightSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑楂樺害
 * @param smooth 鏄惁浣跨敤骞虫粦澶勭悊锛堟姉閿娇锛夛紝榛樿涓?false
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_withalpha(
    PIMAGE  imgDest,            // handle to dest
    PCIMAGE imgSrc,             // handle to source
    int xDest,                  // x-coord of destination upper-left corner
    int yDest,                  // y-coord of destination upper-left corner
    int widthDest,              // width of destination rectangle
    int heightDest,             // height of destination rectangle
    int xSrc,                   // x-coord of source upper-left corner
    int ySrc,                   // y-coord of source upper-left corner
    int widthSrc,               // width of source rectangle
    int heightSrc,              // height of source rectangle
    bool smooth = false
);

/**
 * @brief Alpha婊ら暅缁樺埗鍑芥暟 - 浣跨敤鍙︿竴鍥惧儚浣滀负Alpha閬僵
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param imgAlpha 鐢ㄤ綔Alpha閬僵鐨?IMAGE 瀵硅薄鎸囬拡
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣
 * @param widthSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑瀹藉害
 * @param heightSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑楂樺害
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_alphafilter(
    PIMAGE  imgDest,            // handle to dest
    PCIMAGE imgSrc,             // handle to source
    int xDest,                  // x-coord of destination upper-left corner
    int yDest,                  // y-coord of destination upper-left corner
    PCIMAGE imgAlpha,           // handle to alpha
    int xSrc,                   // x-coord of source upper-left corner
    int ySrc,                   // y-coord of source upper-left corner
    int widthSrc,               // width of source rectangle
    int heightSrc               // height of source rectangle
);

/**
 * @brief 鍥惧儚妯＄硦婊ら暅鍑芥暟 - 瀵瑰浘鍍忚繘琛屾ā绯婂鐞?
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛岃杩涜妯＄硦澶勭悊鐨勫浘鍍?
 * @param intensity 妯＄硦寮哄害锛屽€艰秺澶фā绯婃晥鏋滆秺寮?
 * @param alpha 鍥惧儚鏁翠綋閫忔槑搴?(0-255)锛?55涓哄畬鍏ㄤ笉閫忔槑
 * @param xDest 澶勭悊鍖哄煙鐨勫乏涓婅 x 鍧愭爣锛岄粯璁や负 0
 * @param yDest 澶勭悊鍖哄煙鐨勫乏涓婅 y 鍧愭爣锛岄粯璁や负 0
 * @param widthDest 澶勭悊鍖哄煙鐨勫搴︼紝榛樿涓?0锛堜娇鐢ㄦ暣涓浘鍍忓搴︼級
 * @param heightDest 澶勭悊鍖哄煙鐨勯珮搴︼紝榛樿涓?0锛堜娇鐢ㄦ暣涓浘鍍忛珮搴︼級
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI imagefilter_blurring (
    PIMAGE imgDest,
    int intensity,
    int alpha,
    int xDest = 0,
    int yDest = 0,
    int widthDest = 0,
    int heightDest = 0
);

/**
 * @brief 鏃嬭浆缁樺埗鍑芥暟 - 鍥寸粫涓績鐐规棆杞浘鍍?
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgTexture 婧愮汗鐞?IMAGE 瀵硅薄鎸囬拡
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param xCenter 鏃嬭浆涓績鐐圭殑 x 鍧愭爣锛堢浉瀵逛簬婧愬浘鍍忥級
 * @param yCenter 鏃嬭浆涓績鐐圭殑 y 鍧愭爣锛堢浉瀵逛簬婧愬浘鍍忥級
 * @param radian 鏃嬭浆瑙掑害锛堝姬搴﹀埗锛岄『鏃堕拡鏂瑰悜锛?
 * @param transparent 鏄惁浣跨敤鍥惧儚鐨勯€忔槑閫氶亾锛岄粯璁や负 false
 * @param alpha 鍥惧儚鏁翠綋閫忔槑搴?(0-256)锛?1琛ㄧず涓嶄娇鐢╝lpha锛岄粯璁や负 -1
 * @param smooth 鏄惁浣跨敤骞虫粦澶勭悊锛堟姉閿娇锛夛紝榛樿涓?false
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_rotate(
    PIMAGE  imgDest,
    PCIMAGE imgTexture,
    int   xDest,
    int   yDest,
    float xCenter,
    float yCenter,
    float radian,
    bool  transparent = false,   // use the transparent channel of the image
    int   alpha = -1,            // in range[0, 256], alpha== -1 means no alpha
    bool  smooth = false
);

/**
 * @brief 鏃嬭浆缂╂斁缁樺埗鍑芥暟 - 鍚屾椂杩涜鏃嬭浆鍜岀缉鏀?
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgTexture 婧愮汗鐞?IMAGE 瀵硅薄鎸囬拡
 * @param xDest 缁樺埗浣嶇疆鐨?x 鍧愭爣
 * @param yDest 缁樺埗浣嶇疆鐨?y 鍧愭爣
 * @param xCenter 鏃嬭浆涓績鐐圭殑 x 鍧愭爣锛堢浉瀵逛簬婧愬浘鍍忥級
 * @param yCenter 鏃嬭浆涓績鐐圭殑 y 鍧愭爣锛堢浉瀵逛簬婧愬浘鍍忥級
 * @param radian 鏃嬭浆瑙掑害锛堝姬搴﹀埗锛岄『鏃堕拡鏂瑰悜锛?
 * @param zoom 缂╂斁姣斾緥锛?.0涓哄師濮嬪ぇ灏?
 * @param transparent 鏄惁浣跨敤鍥惧儚鐨勯€忔槑閫氶亾锛岄粯璁や负 false
 * @param alpha 鍥惧儚鏁翠綋閫忔槑搴?(0-256)锛?1琛ㄧず涓嶄娇鐢╝lpha锛岄粯璁や负 -1
 * @param smooth 鏄惁浣跨敤骞虫粦澶勭悊锛堟姉閿娇锛夛紝榛樿涓?false
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_rotatezoom(
    PIMAGE imgDest,
    PCIMAGE imgTexture,
    int xDest,
    int yDest,
    float xCenter,
    float yCenter,
    float radian,
    float zoom,
    bool transparent = false,   // use the transparent channel of the image
    int alpha = -1,             // in range[0, 256], alpha== -1 means no alpha
    bool smooth = false
);

/**
 * @brief 鏃嬭浆閫忔槑缁樺埗鍑芥暟 - 鏃嬭浆鏃舵寚瀹氶€忔槑鑹诧紝鍩虹鐗堟湰
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param xCenterDest 鏃嬭浆涓績鐐瑰湪鐩爣鍥惧儚涓殑 x 鍧愭爣
 * @param yCenterDest 鏃嬭浆涓績鐐瑰湪鐩爣鍥惧儚涓殑 y 鍧愭爣
 * @param xCenterSrc 鏃嬭浆涓績鐐瑰湪婧愬浘鍍忎腑鐨?x 鍧愭爣
 * @param yCenterSrc 鏃嬭浆涓績鐐瑰湪婧愬浘鍍忎腑鐨?y 鍧愭爣
 * @param transparentColor 瑕佸彉涓洪€忔槑鐨勫儚绱犻鑹?
 * @param radian 鏃嬭浆瑙掑害锛堝姬搴﹀埗锛岄『鏃堕拡鏂瑰悜锛?
 * @param zoom 缂╂斁姣斾緥锛?.0涓哄師濮嬪ぇ灏忥紝榛樿涓?1.0f
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_rotatetransparent(
    PIMAGE imgDest,             /* handle to dest, NULL means the SCREEN  */
    PCIMAGE imgSrc,             /* handle to source */
    int xCenterDest,            /* x-coord of rotation center in dest */
    int yCenterDest,            /* y-coord of rotation center in dest */
    int xCenterSrc,             /* x-coord of rotation center in source */
    int yCenterSrc,             /* y-coord of rotation center in source */
    color_t transparentColor,   /* color to make transparent */
    float radian,               /* rotation angle (clockwise, in radian) */
    float zoom = 1.0f           /* zoom factor */
);

/**
 * @brief 鏃嬭浆閫忔槑缁樺埗鍑芥暟 - 鏃嬭浆鏃舵寚瀹氶€忔槑鑹诧紝瀹屾暣鐗堟湰
 * @param imgDest 鐩爣 IMAGE 瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欑粯鍒跺埌灞忓箷
 * @param imgSrc 婧?IMAGE 瀵硅薄鎸囬拡
 * @param xCenterDest 鏃嬭浆涓績鐐瑰湪鐩爣鍥惧儚涓殑 x 鍧愭爣
 * @param yCenterDest 鏃嬭浆涓績鐐瑰湪鐩爣鍥惧儚涓殑 y 鍧愭爣
 * @param xSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?x 鍧愭爣
 * @param ySrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑宸︿笂瑙?y 鍧愭爣
 * @param widthSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑瀹藉害
 * @param heightSrc 缁樺埗鍐呭鍦ㄦ簮 IMAGE 瀵硅薄涓殑楂樺害
 * @param xCenterSrc 鏃嬭浆涓績鐐瑰湪婧愬浘鍍忎腑鐨?x 鍧愭爣
 * @param yCenterSrc 鏃嬭浆涓績鐐瑰湪婧愬浘鍍忎腑鐨?y 鍧愭爣
 * @param transparentColor 瑕佸彉涓洪€忔槑鐨勫儚绱犻鑹?
 * @param radian 鏃嬭浆瑙掑害锛堝姬搴﹀埗锛岄『鏃堕拡鏂瑰悜锛?
 * @param zoom 缂╂斁姣斾緥锛?.0涓哄師濮嬪ぇ灏忥紝榛樿涓?1.0f
 * @return 鎴愬姛杩斿洖 grOk锛屽け璐ヨ繑鍥炵浉搴旈敊璇爜
 */
int EGEAPI putimage_rotatetransparent(
    PIMAGE imgDest,             /* handle to dest, NULL means the SCREEN */
    PCIMAGE imgSrc,             /* handle to source */
    int xCenterDest,            /* x-coord of rotation center in dest */
    int yCenterDest,            /* y-coord of rotation center in dest */
    int xSrc,                   /* x-coord of source upper-left corner */
    int ySrc,                   /* y-coord of source upper-left corner */
    int widthSrc,               /* width of source rectangle */
    int heightSrc,              /* height of source rectangle */
    int xCenterSrc,             /* x-coord of rotation center in source */
    int yCenterSrc,             /* y-coord of rotation center in source */
    color_t transparentColor,   /* color to make transparent */
    float radian,               /* rotation angle (clockwise, in radian) */
    float zoom = 1.0f           /* zoom factor */
);

/** @} */ // 缁撴潫 putimage_functions 缁?

/**
 * @brief 鑾峰彇缁樺浘绐楀彛鐨勭獥鍙ｅ彞鏌?
 * @return 缁樺浘绐楀彛鐨勭獥鍙ｅ彞鏌?HWND)
 * @note 杩斿洖鐨勬槸 Windows 绯荤粺鐨勭獥鍙ｅ彞鏌勶紝鍙敤浜?Windows API 璋冪敤
 * @see getHInstance(), getHDC()
 */
HWND        EGEAPI getHWnd();

/**
 * @brief 鑾峰彇缁樺浘绐楀彛鐨勫疄渚嬪彞鏌?
 * @return 搴旂敤绋嬪簭瀹炰緥鍙ユ焺(HINSTANCE)
 * @note 杩斿洖鐨勬槸 Windows 绯荤粺鐨勫簲鐢ㄧ▼搴忓疄渚嬪彞鏌勶紝鍙敤浜?Windows API 璋冪敤
 * @see getHWnd(), getHDC()
 */
HINSTANCE   EGEAPI getHInstance();

/**
 * @brief 鑾峰彇缁樺浘璁惧涓婁笅鏂?
 * @param pimg 鍥惧儚瀵硅薄鎸囬拡锛屽鏋滀负 NULL 鍒欒幏鍙栫粯鍥剧獥鍙ｇ殑璁惧涓婁笅鏂?
 * @return 璁惧涓婁笅鏂囧彞鏌?HDC)
 * @note 杩斿洖鐨勬槸 Windows 绯荤粺鐨勮澶囦笂涓嬫枃鍙ユ焺锛屽彲鐢ㄤ簬 GDI 缁樺浘鎿嶄綔
 * @warning 涓嶈鎵嬪姩閲婃斁杩斿洖鐨?HDC锛岀敱 EGE 搴撹嚜鍔ㄧ鐞?
 * @see getHWnd(), getHInstance()
 */
HDC         EGEAPI getHDC(PCIMAGE pimg = NULL);

/**
 * @brief 鑾峰彇杩囩▼鍑芥暟鎸囬拡
 * @return 杩囩▼鍑芥暟鎸囬拡
 * @note 鍐呴儴浣跨敤鍑芥暟锛岀敤浜庤幏鍙栫獥鍙ｈ繃绋嬪嚱鏁版寚閽?
 */
PVOID       EGEAPI getProcfunc();

/**
 * @brief 鑾峰彇 EGE 鍥惧舰搴撶増鏈彿
 * @return EGE 鍥惧舰搴撶殑鐗堟湰鍙?
 * @note 鐗堟湰鍙锋牸寮忎负鏁板瓧褰㈠紡锛屽彲鐢ㄤ簬鍏煎鎬ф鏌?
 */
long        EGEAPI getGraphicsVer();

/**
 * @brief 鑾峰彇褰撳墠甯х巼
 * @return 褰撳墠鐨勫抚鐜?fps)
 * @note 杩斿洖姣忕甯ф暟锛岀敤浜庢€ц兘鐩戞帶鍜岃皟璇?
 * @see delay_fps()
 */
float       EGEAPI getfps();

/**
 * @brief 鍒濆鍖栭殢鏈烘暟鐢熸垚鍣紙浣跨敤褰撳墠鏃堕棿浣滀负绉嶅瓙锛?
 * @return 浣跨敤鐨勯殢鏈烘暟绉嶅瓙鍊?
 * @note 浣跨敤褰撳墠鏃堕棿鎴充綔涓虹瀛愬垵濮嬪寲 Mersenne Twister 闅忔満鏁扮敓鎴愬櫒
 * @see randomize(unsigned int seed), random(), randomf()
 */
unsigned int    EGEAPI randomize();

/**
 * @brief 鍒濆鍖栭殢鏈烘暟鐢熸垚鍣紙浣跨敤鎸囧畾绉嶅瓙锛?
 * @param seed 闅忔満鏁扮瀛?
 * @return 浣跨敤鐨勯殢鏈烘暟绉嶅瓙鍊?
 * @note 浣跨敤鎸囧畾绉嶅瓙鍒濆鍖?Mersenne Twister 闅忔満鏁扮敓鎴愬櫒锛岀浉鍚岀瀛愪骇鐢熺浉鍚屽簭鍒?
 * @see randomize(), random(), randomf()
 */
unsigned int    EGEAPI randomize(unsigned int seed);

/**
 * @brief 鐢熸垚闅忔満鏁存暟
 * @param n 闅忔満鏁拌寖鍥寸殑涓婇檺锛屽鏋滀负 0 鍒欑敓鎴愬畬鏁磋寖鍥寸殑闅忔満鏁?
 * @return 鐢熸垚鐨勯殢鏈烘暣鏁帮紝鑼冨洿涓?[0, n) 鎴?[0, UINT_MAX]
 * @note 浣跨敤 Mersenne Twister 绠楁硶鐢熸垚楂樿川閲忛殢鏈烘暟
 * @see randomize(), randomf()
 */
unsigned int    EGEAPI random(unsigned int n = 0);

/**
 * @brief 鐢熸垚闅忔満娴偣鏁?
 * @return 鐢熸垚鐨勯殢鏈烘诞鐐规暟锛岃寖鍥翠负 [0.0, 1.0)
 * @note 浣跨敤 Mersenne Twister 绠楁硶鐢熸垚楂樿川閲忛殢鏈烘诞鐐规暟
 * @see randomize(), random()
 */
double          EGEAPI randomf();

/**
 * @brief 鏄剧ず杈撳叆瀵硅瘽妗嗚幏鍙栧崟琛屾枃鏈紙ASCII 鐗堟湰锛?
 * @param title 瀵硅瘽妗嗘爣棰?
 * @param text 鎻愮ず鏂囨湰
 * @param buf 鐢ㄤ簬瀛樺偍杈撳叆鏂囨湰鐨勭紦鍐插尯
 * @param len 缂撳啿鍖洪暱搴?
 * @return 鎴愬姛杩斿洖闈為浂鍊硷紝澶辫触鎴栧彇娑堣繑鍥?0
 * @note 鏄剧ず涓€涓ā鎬佸璇濇璁╃敤鎴疯緭鍏ュ崟琛屾枃鏈?
 * @warning 纭繚缂撳啿鍖鸿冻澶熷ぇ浠ラ伩鍏嶆孩鍑?
 * @see inputbox_getline(const wchar_t*, const wchar_t*, LPWSTR, int)
 */
int EGEAPI inputbox_getline(const char*  title, const char*  text, LPSTR  buf, int len);

/**
 * @brief 鏄剧ず杈撳叆瀵硅瘽妗嗚幏鍙栧崟琛屾枃鏈紙Unicode 鐗堟湰锛?
 * @param title 瀵硅瘽妗嗘爣棰?
 * @param text 鎻愮ず鏂囨湰
 * @param buf 鐢ㄤ簬瀛樺偍杈撳叆鏂囨湰鐨勭紦鍐插尯
 * @param len 缂撳啿鍖洪暱搴?
 * @return 鎴愬姛杩斿洖闈為浂鍊硷紝澶辫触鎴栧彇娑堣繑鍥?0
 * @note 鏄剧ず涓€涓ā鎬佸璇濇璁╃敤鎴疯緭鍏ュ崟琛屾枃鏈紝鏀寔 Unicode 瀛楃
 * @warning 纭繚缂撳啿鍖鸿冻澶熷ぇ浠ラ伩鍏嶆孩鍑?
 * @see inputbox_getline(const char*, const char*, LPSTR, int)
 */
int EGEAPI inputbox_getline(const wchar_t* title, const wchar_t* text, LPWSTR buf, int len);



/// @defgroup InputHandling 杈撳叆澶勭悊
/// 閿洏鍜岄紶鏍囪緭鍏ュ鐞嗙浉鍏冲嚱鏁?
/// @{

/// @defgroup KeyboardInput 閿洏杈撳叆
/// 閿洏杈撳叆妫€娴嬪拰娑堟伅澶勭悊鍑芥暟
/// @{

/**
 * @brief 妫€娴嬪綋鍓嶆槸鍚︽湁閿洏娑堟伅
 * @return 闈為浂鍊艰〃绀烘湁閿洏娑堟伅锛?琛ㄧず娌℃湁锛屼竴鑸笌getkey鎼厤浣跨敤
 * @note 鐢ㄤ簬闈為樆濉炴娴嬮敭鐩樿緭鍏ワ紝閫氬父鍦ㄤ富寰幆涓笌getkey()涓€璧蜂娇鐢?
 * @see getkey()
 */
int     EGEAPI kbmsg();

/**
 * @brief 鑾峰彇閿洏娑堟伅
 * @return 閿洏娑堟伅缁撴瀯浣擄紝鍖呭惈鎸夐敭浠ｇ爜銆佹秷鎭被鍨嬪拰鏍囧織浣?
 * @note 濡傛灉褰撳墠娌℃湁閿洏娑堟伅鍒欑瓑寰咃紝鏀寔鎸夐敭鎸変笅銆侀噴鏀惧拰瀛楃娑堟伅
 * @see kbmsg(), key_msg, key_msg_e, key_flag_e
 */
key_msg EGEAPI getkey();

/**
 * @brief 鑾峰彇閿洏瀛楃杈撳叆锛堟墿灞曠増鏈級
 * @param flag 娑堟伅鏍囧織浣嶏紝鎺у埗鎺ユ敹鐨勬秷鎭被鍨?
 * @return 鎸夐敭浠ｇ爜鍜屾秷鎭被鍨嬬粍鍚?
 * @deprecated 璇蜂娇鐢╣etch()鍑芥暟浠ｆ浛
 * @note 鏀寔鎸夐敭鎸変笅鍜岄噴鏀句簨浠剁殑鑾峰彇
 * @see getch()
 */
EGE_DEPRECATE(getchEx, "Please use the 'getch' function instead.")
int     EGEAPI getchEx(int flag);

/**
 * @brief 妫€娴嬪綋鍓嶆槸鍚︽湁閿洏瀛楃杈撳叆锛堟墿灞曠増鏈級
 * @param flag 娑堟伅鏍囧織浣嶏紝鎺у埗妫€娴嬬殑娑堟伅绫诲瀷
 * @return 闈為浂鍊艰〃绀烘湁杈撳叆锛?琛ㄧず娌℃湁
 * @deprecated 璇蜂娇鐢╧bhit()鍑芥暟浠ｆ浛
 * @note 鏀寔鎸夐敭鎸変笅鍜岄噴鏀句簨浠剁殑妫€娴?
 * @see kbhit()
 */
EGE_DEPRECATE(kbhitEx, "Please use the 'kbhit' function instead.")
int     EGEAPI kbhitEx(int flag);

/**
 * @brief 鍒ゆ柇閿洏鎴栭紶鏍囦笂鐨勬煇鎸夐敭鏄惁澶勪簬鎸変笅鐘舵€?
 * @param key 瑕佹娴嬬殑鎸夐敭浠ｇ爜锛屽弬瑙乲ey_code_e鏋氫妇
 * @return 闈為浂鍊艰〃绀烘寜閿寜涓嬶紝0琛ㄧず鏈寜涓嬶紝-1琛ㄧず鍙傛暟閿欒
 * @note 瀹炴椂妫€娴嬫寜閿姸鎬侊紝涓嶆秷鑰楁秷鎭槦鍒楋紱鏀寔閿洏鍜岄紶鏍囨寜閿?
 * @see key_code_e
 */
int     EGEAPI keystate(int key);

/**
 * @brief 娓呯┖鎸夐敭娑堟伅缂撳瓨鍖?
 * @note 娓呴櫎鎵€鏈夋湭澶勭悊鐨勯敭鐩樻秷鎭紝甯哥敤浜庡拷鐣ョ紦瀛樼殑鎸夐敭杈撳叆
 * @see flushmouse()
 */
void    EGEAPI flushkey();

/// @} // KeyboardInput

/// @defgroup CharacterInput 瀛楃杈撳叆
/// 瀛楃绾ч敭鐩樿緭鍏ュ鐞嗗嚱鏁?
/// @{

/**
 * @brief 鑾峰彇閿洏瀛楃杈撳叆锛圗GE鍐呴儴鐗堟湰锛?
 * @return 瀛楃鐨凙SCII鐮侊紝鐗规畩閿繑鍥炴墿灞曠爜
 * @note EGE鍐呴儴瀹炵幇锛岀敤浜庨伩鍏嶄笌conio.h搴撳啿绐?
 * @see getch()
 */
int     EGEAPI ege_getch();

/**
 * @brief 妫€娴嬪綋鍓嶆槸鍚︽湁閿洏瀛楃杈撳叆锛圗GE鍐呴儴鐗堟湰锛?
 * @return 闈為浂鍊艰〃绀烘湁瀛楃杈撳叆锛?琛ㄧず娌℃湁
 * @note EGE鍐呴儴瀹炵幇锛岀敤浜庨伩鍏嶄笌conio.h搴撳啿绐?
 * @see kbhit()
 */
int     EGEAPI ege_kbhit();

#if !defined(_INC_CONIO) && !defined(_CONIO_H_)
#define _INC_CONIO
#define _CONIO_H_

/**
 * @brief 鑾峰彇閿洏瀛楃杈撳叆
 * @return 瀛楃鐨凙SCII鐮侊紝鐗规畩閿繑鍥炴墿灞曠爜
 * @note 濡傛灉褰撳墠娌℃湁瀛楃杈撳叆鍒欑瓑寰咃紱绛夊悓浜巈ge_getch()
 * @see ege_getch(), kbhit()
 */
int EGEAPI getch();  // Same as ege_getch()

/**
 * @brief 妫€娴嬪綋鍓嶆槸鍚︽湁閿洏瀛楃杈撳叆
 * @return 闈為浂鍊艰〃绀烘湁瀛楃杈撳叆锛?琛ㄧず娌℃湁
 * @note 鐢ㄤ簬闈為樆濉炴娴嬪瓧绗﹁緭鍏ワ紝涓€鑸笌getch鎼厤浣跨敤锛涚瓑鍚屼簬ege_kbhit()
 * @see ege_kbhit(), getch()
 */
int EGEAPI kbhit();  // Same as ege_kbhit()
#else
#define getch ege_getch
#define kbhit ege_kbhit
#endif

/// @} // CharacterInput

/// @defgroup MouseInput 榧犳爣杈撳叆
/// 榧犳爣杈撳叆妫€娴嬪拰娑堟伅澶勭悊鍑芥暟
/// @{

/**
 * @brief 妫€娴嬪綋鍓嶆槸鍚︽湁榧犳爣娑堟伅
 * @return 闈為浂鍊艰〃绀烘湁榧犳爣娑堟伅锛?琛ㄧず娌℃湁
 * @note 鐢ㄤ簬闈為樆濉炴娴嬮紶鏍囪緭鍏ワ紝涓€鑸笌getmouse鎼厤浣跨敤
 * @see getmouse()
 */
int         EGEAPI mousemsg();

/**
 * @brief 鑾峰彇涓€涓紶鏍囨秷鎭?
 * @return 榧犳爣娑堟伅缁撴瀯浣擄紝鍖呭惈鍧愭爣銆佹寜閿姸鎬佸拰娑堟伅绫诲瀷
 * @note 濡傛灉褰撳墠榧犳爣娑堟伅闃熷垪涓虹┖鍒欑瓑寰咃紝鐩村埌鏈夋柊娑堟伅浜х敓
 * @see mousemsg(), mouse_msg, mouse_msg_e, mouse_flag_e
 */
mouse_msg   EGEAPI getmouse();

/**
 * @brief 鑾峰彇涓€涓紶鏍囨秷鎭紙杩囨椂鍑芥暟锛?
 * @return MOUSEMSG缁撴瀯浣擄紝鍖呭惈榧犳爣鐘舵€佷俊鎭?
 * @deprecated 璇蜂娇鐢╣etmouse()鍑芥暟浠ｆ浛
 * @note 鍏煎鏃х増鏈珹PI锛屽缓璁娇鐢ㄦ洿鐜颁唬鐨刧etmouse()鍑芥暟
 * @see getmouse()
 */
EGE_DEPRECATE(GetMouseMsg, "Please use the 'getmouse' function instead.")
MOUSEMSG    EGEAPI GetMouseMsg();

/**
 * @brief 娓呯┖榧犳爣娑堟伅缂撳啿鍖?
 * @note 娓呴櫎鎵€鏈夋湭澶勭悊鐨勯紶鏍囨秷鎭紝甯哥敤浜庡拷鐣ョ紦瀛樼殑榧犳爣杈撳叆
 * @see flushkey()
 */
void        EGEAPI flushmouse();

/**
 * @brief 璁剧疆榧犳爣鎸囬拡鏄惁鏄剧ず
 * @param bShow 闈為浂鍊兼樉绀洪紶鏍囨寚閽堬紝0闅愯棌榧犳爣鎸囬拡
 * @return 涔嬪墠鐨勬樉绀虹姸鎬?
 * @note 鐢ㄤ簬鎺у埗榧犳爣鍏夋爣鍦ㄥ浘褰㈢獥鍙ｄ腑鐨勫彲瑙佹€?
 */
int         EGEAPI showmouse(int bShow);

/**
 * @brief 鑾峰彇褰撳墠榧犳爣浣嶇疆鍧愭爣
 * @param x 鎺ユ敹榧犳爣x鍧愭爣鐨勬寚閽?
 * @param y 鎺ユ敹榧犳爣y鍧愭爣鐨勬寚閽?
 * @return 鍑芥暟鎵ц鐘舵€?
 * @note 鑾峰彇榧犳爣鍦ㄥ浘褰㈢獥鍙ｄ腑鐨勫疄鏃跺潗鏍囦綅缃?
 * @see keystate()
 */
int         EGEAPI mousepos(int *x, int *y);

/// @} // MouseInput
/// @} // InputHandling

/*
callback function define as:
int __stdcall on_msg_key(void* param, unsigned msg, int key);
msg: see 'enum message_event'
key: keycode
return zero means process this message, otherwise means pass it and then process with 'getkey' function
*/
//int message_addkeyhandler(void* param, LPMSG_KEY_PROC func);
/*
callback function define as:
int __stdcall on_msg_mouse(void* param, unsigned msg, int key, int x, int y);
msg: see 'enum message_event'
key: see 'enum message_mouse', if msg==MSG_EVENT_WHELL, key is a int number that indicates the distance the wheel is rotated, expressed in multiples or divisions of WHEEL_DELTA, which is 120.
x,y: current mouse (x, y)
return zero means process this message, otherwise means pass it and then process with 'GetMouseMsg' function
*/
//int message_addmousehandler(void* param, LPMSG_MOUSE_PROC func);
int EGEAPI SetCloseHandler(LPCALLBACK_PROC func);

/**
 * @brief 闊充箰鎾斁绫?
 * 
 * MUSIC 绫绘彁渚涗簡鍩轰簬 Windows Media Control Interface (MCI) 鐨勯煶涔愭挱鏀惧姛鑳斤紝
 * 鏀寔鎾斁 WAV銆丮P3銆丮IDI 绛夊绉嶉煶棰戞牸寮忋€?
 * 
 * @note 璇ョ被鍩轰簬 Windows MCI 瀹炵幇锛屼粎鏀寔 Windows 骞冲彴
 * @note 鏀寔鐨勯煶棰戞牸寮忓寘鎷細WAV, MP3, MIDI 绛?
 * @see music_state_flag, MUSIC_ERROR
 */
class MUSIC
{
public:
    /**
     * @brief 鏋勯€犲嚱鏁?
     * @note 鍒濆鍖栭煶涔愭挱鏀惧櫒锛岃缃垵濮嬬姸鎬佷负鏈墦寮€
     */
    MUSIC();
    
    /**
     * @brief 鏋愭瀯鍑芥暟
     * @note 鑷姩鍏抽棴宸叉墦寮€鐨勯煶涔愭枃浠跺苟娓呯悊璧勬簮
     */
    virtual ~MUSIC();

    /**
     * @brief 绫诲瀷杞崲鎿嶄綔绗?
     * @return 杩斿洖绐楀彛鍙ユ焺(HWND)
     * @note 鐢ㄤ簬涓?Windows API 浜や簰
     */
    operator HWND() const { return (HWND)m_dwCallBack; }

public:
    /**
     * @brief 妫€鏌ユ槸鍚﹀凡鎵撳紑闊充箰鏂囦欢
     * @return 1 琛ㄧず宸叉墦寮€锛? 琛ㄧず鏈墦寮€
     * @note 鐢ㄤ簬鍒ゆ柇鏄惁鎴愬姛鎵撳紑浜嗛煶涔愭枃浠?
     */
    int IsOpen() { return (m_DID != MUSIC_ERROR) ? 1 : 0; }

    /**
     * @brief 鎵撳紑闊充箰鏂囦欢锛圓SCII 鐗堟湰锛?
     * @param filepath 闊充箰鏂囦欢璺緞锛堝寘鍚枃浠跺悕锛?
     * @return 鎿嶄綔鎴愬姛杩斿洖 0锛屾搷浣滃け璐ヨ繑鍥為潪 0
     * @note 鏀寔 WAV銆丮P3銆丮IDI 绛夋牸寮忥紝鎴愬姛鎵撳紑鍚庢挱鏀剧姸鎬佷负 MUSIC_MODE_STOP
     * @note 濡傛灉宸茬粡鎵撳紑浜嗗叾浠栨枃浠讹紝浼氳嚜鍔ㄥ叧闂師鏂囦欢
     * @see OpenFile(const wchar_t*), Close()
     */
    DWORD OpenFile(const char* filepath);
    
    /**
     * @brief 鎵撳紑闊充箰鏂囦欢锛圲nicode 鐗堟湰锛?
     * @param filepath 闊充箰鏂囦欢璺緞锛堝寘鍚枃浠跺悕锛?
     * @return 鎿嶄綔鎴愬姛杩斿洖 0锛屾搷浣滃け璐ヨ繑鍥為潪 0
     * @note 鏀寔 WAV銆丮P3銆丮IDI 绛夋牸寮忥紝鎴愬姛鎵撳紑鍚庢挱鏀剧姸鎬佷负 MUSIC_MODE_STOP
     * @note 濡傛灉宸茬粡鎵撳紑浜嗗叾浠栨枃浠讹紝浼氳嚜鍔ㄥ叧闂師鏂囦欢
     * @see OpenFile(const char*), Close()
     */
    DWORD OpenFile(const wchar_t* filepath);
    
    /**
     * @brief 鎾斁闊充箰
     * @param dwFrom 鎾斁寮€濮嬩綅缃紙姣锛夛紝榛樿涓?MUSIC_ERROR锛堢户缁挱鏀撅級
     * @param dwTo 鎾斁缁撴潫浣嶇疆锛堟绉掞級锛岄粯璁や负 MUSIC_ERROR锛堟挱鏀惧埌鏈熬锛?
     * @return 鎿嶄綔鎴愬姛杩斿洖 0锛屾搷浣滃け璐ヨ繑鍥為潪 0
     * @note Play() 涓虹户缁挱鏀撅紝Play(0) 涓轰粠澶村紑濮嬫挱鏀?
     * @note 鎾斁鐘舵€佸彉涓?MUSIC_MODE_PLAY
     * @see Pause(), Stop(), RepeatPlay()
     */
    DWORD Play(DWORD dwFrom = MUSIC_ERROR, DWORD dwTo = MUSIC_ERROR);
    
    /**
     * @brief 寰幆鎾斁闊充箰
     * @param dwFrom 鎾斁寮€濮嬩綅缃紙姣锛夛紝榛樿涓?MUSIC_ERROR
     * @param dwTo 鎾斁缁撴潫浣嶇疆锛堟绉掞級锛岄粯璁や负 MUSIC_ERROR
     * @return 鎿嶄綔鎴愬姛杩斿洖 0锛屾搷浣滃け璐ヨ繑鍥為潪 0
     * @note 鍦ㄦ寚瀹氱殑鏃堕棿娈靛唴寰幆鎾斁闊充箰
     * @see Play(), Pause(), Stop()
     */
    DWORD RepeatPlay(DWORD dwFrom = MUSIC_ERROR, DWORD dwTo = MUSIC_ERROR);
    
    /**
     * @brief 鏆傚仠鎾斁
     * @return 鎿嶄綔鎴愬姛杩斿洖 0锛屾搷浣滃け璐ヨ繑鍥為潪 0
     * @note 鎾斁鐘舵€佸彉涓?MUSIC_MODE_PAUSE锛屾挱鏀捐繘搴︿繚鎸佷笉鍙?
     * @see Play(), Stop()
     */
    DWORD Pause();
    
    /**
     * @brief 瀹氫綅鎾斁浣嶇疆
     * @param dwTo 鐩爣鎾斁浣嶇疆锛堟绉掞級
     * @return 鎿嶄綔鎴愬姛杩斿洖 0锛屾搷浣滃け璐ヨ繑鍥為潪 0
     * @note 鐩墠姝ゅ嚱鏁版棤鏁堬紝寤鸿浣跨敤 Play(dwTo) 浠ｆ浛
     * @deprecated 鎺ㄨ崘浣跨敤 Play(dwTo) 瀹炵幇瀹氫綅鎾斁
     * @see Play()
     */
    DWORD Seek(DWORD dwTo);
    
    /**
     * @brief 璁剧疆鎾斁闊抽噺
     * @param value 闊抽噺澶у皬锛岃寖鍥?0.0~1.0
     * @return 鎿嶄綔鎴愬姛杩斿洖 0锛屾搷浣滃け璐ヨ繑鍥為潪 0
     * @note 0.0 涓洪潤闊筹紝1.0 涓烘渶澶ч煶閲?
     */
    DWORD SetVolume(float value);
    
    /**
     * @brief 鍏抽棴闊充箰鏂囦欢
     * @return 鎿嶄綔鎴愬姛杩斿洖 0锛屾搷浣滃け璐ヨ繑鍥為潪 0
     * @note 鍏抽棴褰撳墠鎵撳紑鐨勯煶涔愭枃浠跺苟閲婃斁鐩稿叧璧勬簮
     * @see OpenFile()
     */
    DWORD Close();
    
    /**
     * @brief 鍋滄鎾斁
     * @return 鎿嶄綔鎴愬姛杩斿洖 0锛屾搷浣滃け璐ヨ繑鍥為潪 0
     * @note 鎾斁鐘舵€佸彉涓?MUSIC_MODE_STOP锛屾挱鏀捐繘搴︿繚鎸佷笉鍙?
     * @see Play(), Pause()
     */
    DWORD Stop();
    
    /**
     * @brief 鑾峰彇褰撳墠鎾斁浣嶇疆
     * @return 褰撳墠鎾斁浣嶇疆锛堟绉掞級
     * @note 杩斿洖褰撳墠鎾斁杩涘害锛屽崟浣嶄负姣
     * @see GetLength(), GetPlayStatus()
     */
    DWORD GetPosition();
    
    /**
     * @brief 鑾峰彇闊充箰鎬绘椂闀?
     * @return 闊充箰鎬绘椂闀匡紙姣锛?
     * @note 杩斿洖闊充箰鏂囦欢鐨勬€婚暱搴︼紝鍗曚綅涓烘绉?
     * @see GetPosition(), GetPlayStatus()
     */
    DWORD GetLength();

    /**
     * @brief 鑾峰彇鎾斁鐘舵€?
     * @return 褰撳墠鎾斁鐘舵€侊紝鍙傝 music_state_flag 鏋氫妇
     * @note 杩斿洖鍊煎彲鑳戒负锛?
     *       - MUSIC_MODE_NOT_OPEN: 鏈墦寮€
     *       - MUSIC_MODE_NOT_READY: 璁惧鏈氨缁?
     *       - MUSIC_MODE_PAUSE: 鏆傚仠涓?
     *       - MUSIC_MODE_PLAY: 姝ｅ湪鎾斁
     *       - MUSIC_MODE_STOP: 鍋滄鐘舵€?
     *       - MUSIC_MODE_OPEN: 鎵撳紑涓?
     *       - MUSIC_MODE_SEEK: 瀹氫綅涓?
     * @see music_state_flag, GetPosition(), GetLength()
     */
    DWORD GetPlayStatus();

private:
    DWORD m_DID;        ///< MCI 璁惧 ID
    PVOID m_dwCallBack; ///< 鍥炶皟鍙ユ焺
};

int           EGEAPI ege_compress  (void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen);
int           EGEAPI ege_compress2 (void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen, int level);
int           EGEAPI ege_uncompress(void *dest, unsigned long *destLen, const void *source, unsigned long sourceLen);
unsigned long EGEAPI ege_uncompress_size(const void *source, unsigned long sourceLen);

}

#ifndef EGE_GRAPH_LIB_BUILD
    #if defined(_MSC_VER) && (defined(HIDE_CONSOLE) || !defined(SHOW_CONSOLE))
        #pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
    #endif

    #define Sleep(ms) delay_ms(ms)
#endif

#if !defined(_MSC_VER)
    #define WinMain(...) WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#elif defined(_CONSOLE)
    #if (_MSC_VER > 1200)
        #define WinMain(...) main(int argc, char* argv[])
    #else
        #define WinMain main
    #endif
#endif

#endif
