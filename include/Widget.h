/**
 * @file Widget.h
 * @brief 包含各种UI控件的声明（按钮、输入框、滑动条、文本等）
 * @author FeJS8888
 */

#pragma once

#include "Element.h"
#include "Layout.h"
#include "Base.h"

using namespace FeEGE;

class Layout;
class ScrollBar;
extern Widget* mouseOwningFlag;
extern Widget* focusingWidget;
extern std::vector<Widget*> widgetOrder;
extern bool PanelScaleChanged;

/**
 * @brief 所有可绘制控件的基类
 */
class Widget {
public:
    /**
     * @brief 虚析构，保证子类析构正常调用
     */
    virtual ~Widget();
    
    /**
     * @brief 处理鼠标事件
     * @param msg 鼠标消息
     */
    virtual bool handleEvent(const mouse_msg& msg) = 0;
    bool is_global = true;  ///< 是否全局控件

    /**
     * @brief 绘制控件到指定图像
     * @param dst 绘制目标图像
     * @param offsetX 控件中心 x 坐标（相对于 dst）
     * @param offsetY 控件中心 y 坐标（相对于 dst）
     */
    virtual void draw(PIMAGE dst, double offsetX, double offsetY) = 0;
    
    /**
     * @brief 绘制控件到默认图像
     */
    virtual void draw() = 0;
    
    /**
     * @brief 设置控件位置
     * @param x x坐标
     * @param y y坐标
     */
    virtual void setPosition(double x,double y) = 0;
    
    /**
     * @brief 设置控件缩放比例
     * @param s 缩放比例
     */
    virtual void setScale(double s) = 0;

    /**
     * @brief 获取控件宽度
     * @return 宽度值
     */
    double getWidth();
    
    /**
     * @brief 获取控件高度
     * @return 高度值
     */
    double getHeight();

    virtual void deleteFocus(const mouse_msg& msg);
    virtual void releaseMouseOwningFlag(const mouse_msg& msg);
    virtual void catchMouseOwningFlag(const mouse_msg& msg) ;

    void setParent(Widget* p);

    Widget* getParent();
protected:
    double cx = 0, cy = 0;       ///< 中心坐标
    double width = 0, height = 0; ///< 控件尺寸
    double scale = 1;         ///< 缩放比例
    Widget* parent = nullptr;
};

/**
 * @brief 圆角面板控件类，支持嵌套子控件
 */

class Panel : public Widget {
public:
    /**
     * @brief 构造函数
     * @param cx 面板中心 x 坐标
     * @param cy 面板中心 y 坐标
     * @param w 宽度
     * @param h 高度
     * @param r 圆角半径
     * @param bg 背景颜色
     */
    Panel(double cx, double cy, double w, double h, double r, color_t bg);
    
    /**
     * @brief 析构函数
     */
    ~Panel();

    /**
     * @brief 添加子控件
     * @param child 控件指针
     * @param offsetX 相对于面板中心的 x 偏移
     * @param offsetY 相对于面板中心的 y 偏移
     */
    void addChild(Widget* child, double offsetX, double offsetY);

    /**
     * @brief 绘制 Panel 到指定图层
     * @param dst 目标图像
     * @param x 粘贴到 dst 中的中心 x 坐标
     * @param y 粘贴到 dst 中的中心 y 坐标
     */
    void draw(PIMAGE dst, double x, double y) override;
    
    /**
     * @brief 绘制 Panel 到默认图像
     */
    void draw() override;
    
    /**
     * @brief 设置面板位置
     * @param x x坐标
     * @param y y坐标
     */
    void setPosition(double x,double y) override;
    
    /**
     * @brief 获取面板位置
     * @return 位置对象
     */
    Position getPosition();
    
    /**
     * @brief 设置面板缩放比例
     * @param s 缩放比例
     */
    void setScale(double s) override;
    
    /**
     * @brief 获取面板缩放比例
     * @return 缩放比例
     */
    double getScale();

    /**
     * @brief 设置面板尺寸
     * @param w 宽度
     * @param h 高度
     */
    void setSize(double w,double h);

    /**
     * @brief 清除所有子控件
     */
    void clearChildren();
    
    /**
     * @brief 处理鼠标事件
     * @param msg 鼠标消息
     */
    bool handleEvent(const mouse_msg& msg) override;

    /**
     * @brief 设置透明度
     * @param a 透明度值(0-255)
     */
    void setAlpha(double a);
    
    /**
     * @brief 获取子控件列表
     * @return 子控件列表引用
     */
    std::vector<Widget*>& getChildren();
    
    /**
     * @brief 设置子控件偏移
     * @param index 子控件索引
     * @param pos 新的偏移位置
     */
    void setChildrenOffset(int index,Position pos);

    /**
     * @brief 设置布局管理器
     * @param l 布局对象
     */
    void setLayout(std::shared_ptr<Layout> l) { layout = std::move(l); };
    
    /**
     * @brief 获取布局管理器
     * @return 布局对象
     */
    std::shared_ptr<Layout> getLayout() const { return layout; };

    void setDirty();

    void setAlwaysDirty(bool d);

    int getAlwaysDirtyState();

    /**
     * @brief 启用或禁用滚动条
     * @param enable 是否启用
     * @param scrollBarWidth 滚动条宽度（默认16）
     */
    void enableScrollBar(bool enable, double scrollBarWidth = 16);

    /**
     * @brief 获取滚动条指针
     * @return 滚动条指针（未启用时为nullptr）
     */
    ScrollBar* getScrollBar();

    /**
     * @brief 获取滚动偏移量（像素）
     * @return 当前滚动偏移量
     */
    double getScrollOffset() const;

protected:
    double radius;
    double origin_width, origin_height;
    double origin_radius;
    color_t bgColor;
    double alpha = 255;
    PIMAGE layer = nullptr;
    PIMAGE maskLayer = nullptr;
    PIMAGE drawLayer = nullptr;
    bool scaleChanged = true;
    bool needRedraw = true;
    int needRedrawAlways = 0;
    ege_path clippath;

    std::vector<Widget*> children;
    std::vector<Position> childOffsets;  ///< 每个子控件的相对偏移（以面板中心为参考）
    std::shared_ptr<Layout> layout = nullptr;  ///< 当前布局对象

    ScrollBar* scrollBar_ = nullptr;   ///< 滚动条（启用时非空）
    bool scrollBarEnabled_ = false;     ///< 是否启用了滚动条
    double scrollOffset_ = 0.0;         ///< 当前滚动偏移（像素）
};


class PanelBuilder {
public:
    PanelBuilder& setIdentifier(const std::wstring& identifier);
    PanelBuilder& setCenter(double x, double y);
    PanelBuilder& setSize(double w, double h);
    PanelBuilder& setRadius(double r);
    PanelBuilder& setBackground(color_t color);
    PanelBuilder& setScale(double s);
    PanelBuilder& addChild(Widget* child, double offsetX = 0, double offsetY = 0);
    PanelBuilder& addChild(const std::vector<Widget*>& child, const std::vector<double>& offsetX = {}, const std::vector<double>& offsetY = {});
    PanelBuilder& setLayout(std::shared_ptr<Layout> layout);
    PanelBuilder& setScrollBar(bool enable, double scrollBarWidth = 16);
    Panel* build();

protected:
    std::wstring identifier;
    double cx = 0, cy = 0;
    double width = 100, height = 50;
    double radius = 5;
    color_t bg = EGERGB(240, 240, 240);
    double scale = 1.0;
    std::vector<Widget*> children;
    std::vector<Position> childOffsets;  ///< 每个子控件的相对偏移（以面板中心为参考）
    std::shared_ptr<Layout> layout = nullptr;
    bool scrollBarEnabled = false;
    double scrollBarWidth = 16;
};


/**
 * @brief 波纹动画效果结构体
 */
struct Ripple{
    double x, y;           ///< 波纹中心坐标
    int maxRadius;      ///< 最大半径
    int life;           ///< 生命周期
    int age = 0;        ///< 当前年龄
    int counter = -1;
    mutable bool m_setDirtyState = false;
    Widget* parent;

    /**
     * @brief 构造函数
     * @param _x 中心x坐标
     * @param _y 中心y坐标
     * @param _r 最大半径
     * @param _life 生命周期
     */
    Ripple(int _x, int _y, int _r, int _life,Widget* p,int _c);

    /**
     * @brief 检查波纹是否存活
     * @return 是否存活
     */
    bool alive() const;

    /**
     * @brief 更新波纹状态
     */
    void update();

    /**
     * @brief 绘制波纹
     * @param dst 目标图像
     */
    void draw(PIMAGE dst) const;

    /**
     * @brief 绘制波纹抗锯齿补偿
     * @param dst 目标图像
     */
    void draw_aa(PIMAGE dst) const;
};

/**
 * @brief 按钮控件类
 */

class Button : public Widget {
protected:
    double radius;                              ///< 圆角半径
    double origin_width, origin_height;         ///< 原始宽高
    double origin_radius;                       ///< 原始圆角半径
    double left, top;                           ///< 左上角坐标
    std::vector<Ripple> ripples;                ///< 波纹效果集合
    PIMAGE btnLayer = nullptr;                  ///< 按钮图层
    PIMAGE maskLayer = nullptr;                 ///< 遮罩图层
    PIMAGE bgLayer = nullptr;                   ///< 背景图层
    std::wstring content;                       ///< 按钮文本
    std::function<void(void)> on_click_event = nullptr;  ///< 点击事件回调
    color_t color;                              ///< 按钮颜色
    bool needRedraw = true;                     ///< 是否需要重绘
    bool m_clicking = false;                    ///< 是否正在点击
    int m_counter = 0;                          ///< 计数器
    PIMAGE icon = nullptr;                      ///< 图标图像
    int iconSize = 100;                         ///< 图标尺寸
    ege_path clippath;

public:
    /**
     * @brief 构造函数
     * @param cx 中心x坐标
     * @param cy 中心y坐标
     * @param w 宽度
     * @param h 高度
     * @param r 圆角半径
     */
    Button(double cx, double cy, double w, double h, double r);

    /**
     * @brief 析构函数
     */
    virtual ~Button();

    /**
     * @brief 绘制按钮到指定图像
     * @param dst 目标图像
     * @param x x坐标
     * @param y y坐标
     */
    virtual void draw(PIMAGE dst,double x,double y) override;
    
    /**
     * @brief 绘制按钮到默认图像
     */
    virtual void draw() override;

    /**
     * @brief 处理鼠标事件
     * @param msg 鼠标消息
     */
    virtual bool handleEvent(const mouse_msg& msg);
    
    /**
     * @brief 设置按钮文本
     * @param str 文本内容
     */
    void setContent(const std::wstring& str);
    
    /**
     * @brief 获取按钮文本
     * @return 文本内容
     */
    std::wstring getContent();

    /**
     * @brief 获取点击状态
     * @return 是否正在点击
     */
    bool getClickState();

    /**
     * @brief 检查点是否在按钮内
     * @param x x坐标
     * @param y y坐标
     * @return 是否在按钮内
     */
    bool isInside(double x, double y) const;
    
    /**
     * @brief 设置按钮位置
     * @param x x坐标
     * @param y y坐标
     */
    void setPosition(double x,double y) override;
    
    /**
     * @brief 设置按钮缩放比例
     * @param s 缩放比例
     */
    void setScale(double s) override;
    
    /**
     * @brief 设置点击事件回调
     * @param func 回调函数
     */
    void setOnClickEvent(std::function<void(void)> func);

    /**
     * @brief 设置按钮颜色
     * @param col 颜色值
     */
    void setColor(color_t col);
    
    /**
     * @brief 设置按钮图标
     * @param img 图标图像
     */
    void setIcon(PIMAGE img);

    /**
     * @brief 设置图标尺寸
     * @param is 图标尺寸
     */
    void setIconSize(int is);

    /**
     * @brief 获取计数器值
     * @return 计数器值
     */
    int getMCounter();

    virtual void releaseMouseOwningFlag(const mouse_msg& msg) override;
    virtual void catchMouseOwningFlag(const mouse_msg& msg) override;
};

/**
 * @brief 用于构建 Button 对象的构建器类
 */
class ButtonBuilder {
public:
    ButtonBuilder& setIdentifier(const std::wstring& identifier);
    ButtonBuilder& setCenter(double x, double y);
    ButtonBuilder& setSize(double w, double h);
    ButtonBuilder& setRadius(double r);
    ButtonBuilder& setContent(const std::wstring& text);
    ButtonBuilder& setScale(double s);
    ButtonBuilder& setOnClick(std::function<void()> func);
    ButtonBuilder& setColor(color_t col);
    ButtonBuilder& setIcon(PIMAGE img);
    ButtonBuilder& setIconSize(int size);
    Button* build();

private:
    std::wstring identifier;
    double cx = 0, cy = 0;
    double width = 100, height = 50;
    double radius = 8;
    std::wstring content = L"Button";
    double scale = 1.0;
    color_t color = EGERGB(245, 245, 235);
    std::function<void()> onClick = nullptr;
    PIMAGE icon = nullptr;
    int iconSize = 100;
};

/**
 * @brief 输入框控件类
 */
class InputBox : public Widget {
protected:
    double radius;            ///< 圆角半径
    double origin_width, origin_height;
    double origin_radius;
    double left, top;         ///< 左上角坐标
    std::vector<Ripple> ripples; ///< 波纹效果集合
    PIMAGE btnLayer = nullptr;  ///< 按钮图层
    PIMAGE maskLayer = nullptr; ///< 遮罩图层
    PIMAGE bgLayer = nullptr; ///< 遮罩图层
    std::wstring content;   ///< 输入内容
    FeEGE::sys_edit inv;   ///< 输入控件
    bool on_focus = false;         ///< 是否获得焦点
    bool m_clicking = false;
    int m_counter = 0;
    color_t color = EGERGB(245, 245, 235);
    bool needRedraw = true;
    bool scaleChanged = true;
    ege_path clippath;

    int cursor_pos = 0;
    double text_height = 23;
    std::chrono::_V2::system_clock::time_point start_time;
    bool lastInside = false;
    std::wstring IMECompositionString = L"";
    int IMECursorPos = 0;
    float scroll_offset = 0;

    bool dragging = false;
    int dragBegin = 0, dragEnd = 0;
    int dragSide = 0; // -1=左出界，1=右出界，0=无
    double lastDragTick = 0.0;
    const double DRAG_ADVANCE_INTERVAL = 0.05; // 自动推进间隔，单位秒

    LOGFONT m_font; // 输入框字体
    double lastFontScale = -1;  ///< 缓存上次字体缩放比例
    int lastCursorPos = -1;
    std::wstring lastMeasuredContent;  ///< 缓存上次测量的内容
    float cachedCursorPosWidth = 0;    ///< 缓存的光标位置宽度
    float cachedCursorWithImeWidth = 0;  ///< 缓存的光标+IME位置宽度
    float cachedCursorWithFullImeWidth = 0;  ///< 缓存的光标+完整IME位置宽度

public:
    /**
     * @brief 构造函数
     * @param cx 中心x坐标
     * @param cy 中心y坐标
     * @param w 宽度
     * @param h 高度
     * @param r 圆角半径
     */
    InputBox(double cx, double cy, double w, double h, double r);

    /**
     * @brief 析构函数
     */
    virtual ~InputBox();

    /**
     * @brief 绘制输入框
     */
    virtual void draw(PIMAGE dst,double x,double y) override;
    virtual void draw() override;

    /**
     * @brief 处理鼠标事件
     * @param msg 鼠标消息
     */
    virtual bool handleEvent(const mouse_msg& msg);

    virtual void releaseMouseOwningFlag(const mouse_msg& msg) override;
    virtual void catchMouseOwningFlag(const mouse_msg& msg) override;

    /**
     * @brief 检查点是否在输入框内
     * @param x x坐标
     * @param y y坐标
     * @return 是否在输入框内
     */
    bool isInside(double x, double y) const;

    /**
     * @brief 设置输入内容
     * @param s 内容字符串
     */
    void setContent(const std::wstring& s);

    /**
     * @brief 设置最大输入长度
     * @param maxlen 最大长度
     */
    void setMaxlen(int maxlen);
    
    void setPosition(double x,double y) override;
    
    void setScale(double s) override;

    void setTextHeight(double height);
    double getTextHeight();

    void moveCursor(int pos);

    void setIMECompositionString(const std::wstring& str);
    void setIMECursorPos(int pos);

    void adjustScrollForCursor();
    void reflushCursorTick();

    const std::wstring& getContent();
    bool getClickState();

    int getMCounter();

    virtual void deleteFocus(const mouse_msg& msg) override ;
};


class InputBoxBuilder {
public:
    InputBoxBuilder& setIdentifier(const std::wstring& identifier);
    InputBoxBuilder& setCenter(double x, double y);
    InputBoxBuilder& setSize(double w, double h);
    InputBoxBuilder& setRadius(double r);
    InputBoxBuilder& setContent(const std::wstring& text);
    InputBoxBuilder& setMaxLength(int maxLen);
    InputBoxBuilder& setTextHeight(double height);
    InputBoxBuilder& setScale(double s);
    InputBox* build();

private:
    std::wstring identifier;
    double cx = 0, cy = 0;
    double width = 160, height = 40;
    double radius = 6;
    std::wstring content = L"";
    int maxLength = 100;
    double scale = 1.0;
    double text_height = 23;
};


enum class Orientation {
    Row,    // 行方向（水平）
    Column  // 列方向（垂直）
};

/**
 * @brief 滑动条控件类
 */

class Slider : public Widget {
protected:
    double left, top;
    double radius;              ///< 滑块半径
    double origin_width, origin_height;
    double origin_radius;
    int m_dragOffset;          ///< 拖动偏移量
    double m_value;            ///< 当前值(0.0-1.0)
    double m_progress = 0.0;   ///< 当前进度(0.0-1.0)
    double m_finalprogress = 0.0; ///< 最终进度(0.0-1.0)
    bool m_dragging = false;           ///< 是否正在拖动
    color_t m_bgColor, m_fgColor; ///< 背景和前景色
    std::function<void(double)> m_onChange; ///< 值改变回调
	bool m_hover = false;            ///< 鼠标是否悬停
    bool m_skip = false;             ///< 是否点击轨道改变进度
	bool m_pressed = false;          ///< 是否正在按住
	float m_scale = 1.0f;            ///< 当前缩放比例
	double thickness = 4;
	double origin_thickness = 4;
    double step = 0.0;              ///< 步进值，0表示无步进
    bool needRedraw = true;
    Orientation m_orientation = Orientation::Row; // 方向
    float text_offset_x = 0; // 文本水平偏移量
    float last_cursor_pos_width = 0; // 上次光标位置，用于平滑滚动

public:
    /**
     * @brief 默认构造函数
     */
    Slider();

    /**
     * @brief 创建滑动条
     * @param x x坐标
     * @param y y坐标
     * @param w 宽度
     * @param h 高度
     */
    void create(double x, double y, double w, double h);

    /**
     * @brief 绘制滑动条
     */
    virtual void draw(PIMAGE dst,double x,double y) override ;
    virtual void draw();
    
    /**
     * @brief 检查点是否在滑块内
     * @param x x坐标
     * @param y y坐标
     * @return 是否在输入框内
     */
    bool isInside(double x, double y);

    /**
     * @brief 检查点是否在滑动条轨道内
     * @param x x坐标
     * @param y y坐标
     * @return 是否在轨道内
     */
    bool isInsideBar(double x, double y);

    /**
     * @brief 处理鼠标事件
     * @param msg 鼠标消息
     */
    virtual bool handleEvent(const mouse_msg& msg);

    virtual void releaseMouseOwningFlag(const mouse_msg& msg) override;
    virtual void catchMouseOwningFlag(const mouse_msg& msg) override;

    /**
     * @brief 设置进度值
     * @param v 进度值(0.0-1.0)
     */
    void setProgress(double v);

    /**
     * @brief 获取当前进度值
     * @return 当前进度值
     */
    double getProgress() const;

    /**
     * @brief 设置颜色
     * @param bg 背景色
     * @param fg 前景色
     */
    void setColor(color_t bg, color_t fg);
    
    /**
     * @brief 设置粗细
     * @param t 粗细
     */
    void setThickness(double t);

    /**
     * @brief 设置值改变回调
     * @param callback 回调函数
     */
    void setOnChange(std::function<void(double)> callback);
    
    /**
     * @brief 设置位置
     * @param x x坐标
     * @param y y坐标
     */
    void setPosition(double x,double y) override;
    
    /**
     * @brief 设置缩放比例
     * @param s 缩放比例
     */
    void setScale(double s) override;

    /**
     * @brief 设置滑动条方向
     * @param ori 方向枚举
     */
    void setOrientation(Orientation ori);

    /**
     * @brief 设置步进值
     * @param s 步进值
     */
    void setStep(double s);

    /**
     * @brief 修正进度值到最近的步进点
     */
    double fixProgress();
};


class SliderBuilder {
public:
    SliderBuilder& setIdentifier(const std::wstring& identifier);
    /**
     * @brief 设置位置
     * @param cx 中心 x 坐标
     * @param cy 中心 y 坐标
     */
    SliderBuilder& setCenter(double cx, double cy);

    /**
     * @brief 设置尺寸
     * @param w 宽度
     * @param h 高度
     */
    SliderBuilder& setSize(double w, double h);

    /**
     * @brief 设置颜色
     * @param bg 背景色
     * @param fg 前景色
     */
    SliderBuilder& setColor(color_t bg, color_t fg);

    /**
     * @brief 设置粗细
     * @param t 粗细值
     */
    SliderBuilder& setThickness(double t);

    /**
     * @brief 设置初始进度
     * @param v 初始值 (0.0~1.0)
     */
    SliderBuilder& setProgress(double v);

    /**
     * @brief 设置缩放比例
     * @param s 缩放比例
     */
    SliderBuilder& setScale(double s);

    /**
     * @brief 设置值改变时的回调函数
     * @param callback 回调函数
     */
    SliderBuilder& setOnChange(std::function<void(double)> callback);

    /**
     * @brief 设置滑动条方向
     * @param ori 方向枚举
     */
    SliderBuilder& setOrientation(Orientation ori);

    /**
     * @brief 设置步进值
     * @param s 步进值
     */
    SliderBuilder& setStep(double s);

    /**
     * @brief 构建并返回 Slider 对象
     */
    Slider* build();

private:
    std::wstring identifier;
    double x = 0, y = 0;
    double width = 200, height = 20;
    color_t bgColor = EGERGB(200, 200, 200);
    color_t fgColor = EGERGB(100, 100, 255);
    double thickness = 4;
    double progress = 0.0;
    double scale = 1.0;
    double step = 0.00001;
    std::function<void(double)> onChange = nullptr;
    Orientation orientation = Orientation::Row;
};


class ProgressBar : public Widget {
protected:
    double origin_width, origin_height;
    double radius = 6;
    double left, top;

    color_t bgColor = EGERGB(230, 230, 230);
    color_t fgColor = EGERGB(100, 180, 255);

    double currentProgress = 0.0;  // 当前绘制进度
    double targetProgress = 0.0;   // 实际设置目标
    bool needRedraw = true;

    PIMAGE barLayer = nullptr;

public:
    ProgressBar(double cx, double cy, double w, double h);
    ~ProgressBar();

    void setProgress(double p);      // 目标进度（0~1）
    double getProgress() const;      // 返回 target 值
    void setColor(color_t fg);       // 设置前景颜色
    void setBackground(color_t bg);  // 设置背景颜色

    void draw(PIMAGE dst, double x, double y) override;
    void draw() override;
    bool handleEvent(const mouse_msg& msg) override;
    void setPosition(double x, double y) override;
    void setScale(double s) override;
};


class ProgressBarBuilder {
public:
    ProgressBarBuilder& setIdentifier(const std::wstring& identifier);
    ProgressBarBuilder& setCenter(double x, double y);
    ProgressBarBuilder& setSize(double w, double h);
    ProgressBarBuilder& setScale(double s);
    ProgressBarBuilder& setProgress(double p);
    ProgressBarBuilder& setColor(color_t fg);
    ProgressBarBuilder& setBackground(color_t bg);
    ProgressBar* build();

private:
    std::wstring identifier;
    double cx = 0, cy = 0;
    double width = 200, height = 20;
    double scale = 1.0;
    double progress = 0.0;
    color_t fgColor = EGERGB(100, 180, 255);
    color_t bgColor = EGERGB(230, 230, 230);
};


class Dropdown : public Widget {
public:
    Dropdown(double cx, double cy, double w, double h, double r);
    ~Dropdown();

    void addOption(const std::wstring& text, std::function<void()> onClick);
    void setContent(const std::wstring& text);
    void setColor(color_t col);
    void setScale(double s) override;
    void setPosition(double x, double y) override;

    void draw(PIMAGE dst, double x, double y) override;
    void draw() override;
    bool handleEvent(const mouse_msg& msg) override;
    bool isInside(double x, double y) const;


protected:
    Button* mainButton;
    Panel* dropdownPanel;
    std::vector<Button*> options;
    bool expanded = false;
    double cx, cy;
    double width, height, radius;
    double scale = 1.0;
    double fadeAlpha = 0.0;
    int lastAppliedAlpha = -1;  ///< 缓存上次应用的透明度值
    bool fadingIn = false;
    bool fadingOut = false;

    color_t color = EGERGB(245, 245, 235);

    void toggleDropdown();
    void updateDropdownLayout();
};


class DropdownBuilder {
public:
    DropdownBuilder& setIdentifier(const std::wstring& identifier);
    DropdownBuilder& setCenter(double x, double y);
    DropdownBuilder& setSize(double w, double h);
    DropdownBuilder& setRadius(double r);
    DropdownBuilder& setContent(const std::wstring& text);
    DropdownBuilder& setColor(color_t col);
    DropdownBuilder& setScale(double s);
    DropdownBuilder& addOption(const std::wstring& text, std::function<void()> onClick);
    Dropdown* build();

private:
    std::wstring identifier;
    double cx = 0, cy = 0;
    double width = 100, height = 40;
    double radius = 8;
    double scale = 1.0;
    std::wstring content = L"菜单";
    color_t color = EGERGB(245, 245, 235);

    std::vector<std::pair<std::wstring, std::function<void()>>> optionList;
};


enum class RadioStyle {
    Filled,  // 实心背景
    Outline  // 空心圆
};


class Radio : public Widget {
public:
    Radio(double cx, double cy, double r, const std::wstring& val);

    void setOnSelect(std::function<void()> callback);
    std::wstring getValue() const;
    bool isChecked() const;
    void setPosition(double x, double y) override;
    void setGroupValueRef(std::wstring* ref);
    void setScale(double s) override;
    void setStyle(RadioStyle s);
    void draw(PIMAGE dst, double x, double y) override;
    void draw() override;
    bool handleEvent(const mouse_msg& msg) override;

protected:
    double cx, cy;
    double radius, scale = 1.0;
    double origin_radius;
    std::wstring value;
    std::wstring groupValue;  // 当前组选中的值
    std::wstring* groupValuePtr = nullptr;
    bool animIn = false;
    bool animOut = false;
    double animProgress = 0.0;
    const double animSpeed = 1.0 / 16;  // 控制动画帧率（16帧）
    double lastScale = 1.0;  // 保存最后一帧 scale，用于动画结束后稳定显示
    bool wasChecked = false;
    bool keepColor = false;

    bool hovered = false;
    bool animPlaying = false;
    int animFrame = 0; // 0~10
    std::function<void()> onSelect;

    RadioStyle style = RadioStyle::Filled;
};


class RadioBuilder {
public:
    RadioBuilder& setIdentifier(const std::wstring& identifier);
    RadioBuilder& setCenter(double x, double y);
    RadioBuilder& setRadius(double r);
    RadioBuilder& setValue(const std::wstring& val);
    RadioBuilder& setScale(double s);
    RadioBuilder& setGroupValueRef(std::wstring* ref);
    RadioBuilder& setOnSelect(std::function<void()> cb);
    RadioBuilder& setStyle(RadioStyle s);
    Radio* build();

private:
    std::wstring identifier;
    double cx = 0, cy = 0;
    double radius = 12;
    double scale = 1.0;
    std::wstring value = L"";
    std::wstring* groupPtr = nullptr;
    std::function<void()> onSelect = nullptr;

    RadioStyle style = RadioStyle::Filled;
};


class RadioController {
public:
    RadioController(double cx, double cy, double r, double gap, double scale, RadioStyle style);

    void addValue(const std::wstring& val);
    void setDefault(const std::wstring& val);
    void setOnChange(std::function<void(const std::wstring&)> cb);
    std::wstring getValue();
    void build();

protected:
    std::wstring identifier;
    double cx, cy;
    double radius, scale, gap;
    RadioStyle style;
    std::vector<std::wstring> values;
    std::wstring currentValue;
    std::function<void(const std::wstring&)> onChange;
};


class RadioControllerBuilder {
public:
    RadioControllerBuilder& setIdentifier(const std::wstring& identifier);
    RadioControllerBuilder& setCenter(double x, double y);
    RadioControllerBuilder& setRadius(double r);
    RadioControllerBuilder& setGap(double g);
    RadioControllerBuilder& setScale(double s);
    RadioControllerBuilder& setStyle(RadioStyle s);
    RadioControllerBuilder& add(const std::wstring& val);
    RadioControllerBuilder& setDefault(const std::wstring& val);
    RadioControllerBuilder& setOnChange(std::function<void(const std::wstring&)> cb);
    RadioController* build();

private:
    std::wstring identifier;
    double cx = 0, cy = 0;
    double radius = 12;
    double gap = 40;
    double scale = 1.0;
    RadioStyle style = RadioStyle::Filled;
    std::vector<std::wstring> values;
    std::wstring defaultValue = L"";
    std::function<void(const std::wstring&)> onChange = nullptr;
};


class Toggle : public Widget {
public:
    Toggle(double cx, double cy, double w, double h);

    void setChecked(bool c);
    void toggle();
    bool isChecked() const;
    bool isDisabled() const;
    void setOnToggle(std::function<void(bool)> cb);
    void setScale(double s);
    void setPosition(double x, double y) override;
    void setKeepColor(bool keep);
    void setBaseColor(color_t col);
    void setDisabled(bool d);

    void draw(PIMAGE dst, double x, double y) override;
    void draw() override;
    bool handleEvent(const mouse_msg& msg) override;

    virtual void releaseMouseOwningFlag(const mouse_msg& msg) override;
    virtual void catchMouseOwningFlag(const mouse_msg& msg) override;

protected:
    double cx, cy;
    double width, height, scale = 1.0;

    bool checked = false;
    double knobOffset = 0.0;     // 动画当前值
    bool hovered = false;
    double knobTarget = 0.0;     // 动画目标值
    bool pressedIn = false;
    bool disabled = false;

    const double animationSpeed = 0.1;
    bool keepColor = false;
    color_t baseColor = EGERGB(33, 150, 243);


    std::function<void(bool)> onToggle;
};


class ToggleBuilder {
public:
    ToggleBuilder& setIdentifier(const std::wstring& identifier);
    ToggleBuilder& setCenter(double x, double y);
    ToggleBuilder& setSize(double w, double h);
    ToggleBuilder& setScale(double s);
    ToggleBuilder& setChecked(bool c);
    ToggleBuilder& setOnToggle(std::function<void(bool)> cb);
    ToggleBuilder& setKeepColor(bool keep);
    ToggleBuilder& setBaseColor(color_t col);
    Toggle* build();

private:
    std::wstring identifier;
    double cx = 0, cy = 0;
    double w = 60, h = 30, scale = 1.0;
    bool checked = false;
    std::function<void(bool)> onToggle = nullptr;
    bool keepColor = false;
    color_t baseColor = EGERGB(33, 150, 243);
};


enum class TextAlign {
    Left,
    Center,
    Right
};


class Text : public Widget {
public:
    Text(double x, double y, int maxWidth = 0);

    void setContent(const std::wstring& text);      // UTF-8 / GBK 自动转换
    void setMaxWidth(int width);
    void setFont(int size, const std::wstring& name = L"Consolas");
    void setColor(color_t col);
    void setScale(double s);
    void setAlign(TextAlign a);
    void setLineSpacing(int spacing);

    int getTextWidth() const;
    int getTextHeight() const;
    int getMaxWidth() const;
    TextAlign getAlign() const;
    int getLineSpacing() const;

    void draw(PIMAGE dst, double x, double y) override;
    void draw() override;
    void setPosition(double x, double y) override;
    bool handleEvent(const mouse_msg& msg) override;

protected:
    void updateLayout();

    int posX = 0, posY = 0;
    int maxWidth = 0;
    double scale = 1.0;
    int fontSize = 16;
    std::wstring fontName = L"Consolas";
    color_t color = BLACK;

    std::wstring contentW;
    std::vector<std::wstring> lines;
    double textWidth = 0;
    double textHeight = 0;
    TextAlign align = TextAlign::Left;
    int lineSpacing = 0; // 额外行距
    double lastFontScale = -1;  ///< 缓存上次字体缩放比例
    std::vector<float> cachedLineWidths;  ///< 缓存每行的宽度
};


class TextBuilder {
public:
    TextBuilder& setIdentifier(const std::wstring& identifier);
    TextBuilder& setPosition(double x, double y);
    TextBuilder& setMaxWidth(int w);
    TextBuilder& setFont(int size, const std::wstring& name);
    TextBuilder& setScale(double s);
    TextBuilder& setColor(color_t c);
    TextBuilder& setContent(const std::wstring& text);
    TextBuilder& setAlign(TextAlign a);
    TextBuilder& setLineSpacing(int px);
    Text* build();

private:
    std::wstring identifier;
    double x = 0, y = 0;
    int maxWidth = 0;
    int fontSize = 16;
    std::wstring fontName = L"Consolas";
    double scale = 1.0;
    color_t color = BLACK;
    std::wstring content;
    TextAlign align = TextAlign::Left;
    int lineSpacing = 0;
};


/**
 * @brief Knob 旋钮控件类（基于 Quasar QKnob 风格）
 * @details 通过圆形进度弧显示当前值，支持鼠标拖拽交互改变数值
 */
class Knob : public Widget {
public:
    /**
     * @brief 构造函数
     * @param cx 中心x坐标
     * @param cy 中心y坐标
     * @param r 旋钮半径
     */
    Knob(double cx, double cy, double r);

    /**
     * @brief 设置值范围
     * @param minVal 最小值
     * @param maxVal 最大值
     */
    void setRange(double minVal, double maxVal);

    /**
     * @brief 设置步进值
     * @param s 步进值（0表示无步进）
     */
    void setStep(double s);

    /**
     * @brief 设置当前值
     * @param val 要设置的值
     */
    void setValue(double val);

    /**
     * @brief 设置颜色
     * @param fg 前景色（进度弧颜色）
     * @param bg 背景色（轨道颜色）
     */
    void setColor(color_t fg, color_t bg);

    /**
     * @brief 设置值改变回调
     * @param cb 回调函数
     */
    void setOnChange(std::function<void(double)> cb);

    /**
     * @brief 设置偏移角度（圆弧起始角度偏移）
     * @param angle 偏移角度（度）
     */
    void setOffsetAngle(double angle);

    /**
     * @brief 设置内部值范围限制
     * @param innerMin 内部最小值（用于限制显示范围）
     * @param innerMax 内部最大值（用于限制显示范围）
     */
    void setInnerRange(double innerMin, double innerMax);

    /**
     * @brief 设置是否显示值
     * @param show 是否在中心显示当前值
     */
    void setShowValue(bool show);

    /**
     * @brief 设置字体大小
     * @param size 字体大小
     */
    void setFontSize(int size);

    /**
     * @brief 设置禁用状态
     * @param disabled 是否禁用
     */
    void setDisabled(bool disabled);

    /**
     * @brief 设置只读状态
     * @param readonly 是否只读
     */
    void setReadonly(bool readonly);

    /**
     * @brief 获取当前值
     * @return 当前值
     */
    double getValue() const;

    void setScale(double s) override;
    void setPosition(double x, double y) override;
    void draw(PIMAGE dst, double x, double y) override;
    void draw() override;
    bool handleEvent(const mouse_msg& msg) override;
    virtual void releaseMouseOwningFlag(const mouse_msg& msg) override;
    virtual void catchMouseOwningFlag(const mouse_msg& msg) override;

protected:
    double cx, cy;                      ///< 中心坐标
    double radius;                   ///< 半径
    double origin_radius;            ///< 原始半径（用于缩放）
    double scale = 1.0;              ///< 缩放比例

    double minValue = 0.0;           ///< 最小值
    double maxValue = 100.0;         ///< 最大值
    double step = 1.0;               ///< 步进值
    double value = 50.0;             ///< 当前值（目标值）
    double displayValue = 50.0;      ///< 显示值（用于缓动）

    double offsetAngle = 0.0;        ///< 偏移角度（度）
    double innerMin = 0.0;           ///< 内部最小值
    double innerMax = 100.0;         ///< 内部最大值

    bool showValue = true;           ///< 是否显示值
    int fontSize = 0;                ///< 字体大小（0表示自动）
    bool disabled = false;           ///< 是否禁用
    bool readonly = false;           ///< 是否只读

    color_t fgColor = EGERGB(33, 150, 243);    ///< 前景色
    color_t bgColor = EGERGB(230, 230, 230);   ///< 背景色
    color_t trackColor = EGERGB(200, 200, 200); ///< 轨道颜色

    bool dragging = false;           ///< 是否正在拖动
    int lastMouseX = 0;              ///< 上次鼠标X坐标
    int lastMouseY = 0;              ///< 上次鼠标Y坐标
    bool hovered = false;            ///< 是否鼠标悬停

    std::function<void(double)> onChange;  ///< 值改变回调

    /**
     * @brief 将值限制在范围内
     * @param v 输入值
     * @return 限制后的值
     */
    double clamp(double v) const;

    /**
     * @brief 将值转换为角度
     * @param v 值
     * @return 对应的角度（度）
     */
    double valueToAngle(double v) const;

    /**
     * @brief 将角度转换为值
     * @param angle 角度（度）
     * @return 对应的值
     */
    double angleToValue(double angle) const;

    /**
     * @brief 根据步进修正值
     * @param v 输入值
     * @return 修正后的值
     */
    double applyStep(double v) const;

    /**
     * @brief 检查点是否在旋钮内
     * @param x x坐标
     * @param y y坐标
     * @return 是否在旋钮内
     */
    bool isInside(double x, double y) const;

    /**
     * @brief 计算鼠标位置对应的角度
     * @param x x坐标
     * @param y y坐标
     * @return 角度（度）
     */
    double calculateAngle(double x, double y) const;
};


/**
 * @brief KnobBuilder 构建器类
 * @details 用于链式配置和构建 Knob 对象
 */
class KnobBuilder {
public:
    /**
     * @brief 设置标识符
     * @param identifier 控件标识符
     * @return 构建器引用
     */
    KnobBuilder& setIdentifier(const std::wstring& identifier);

    /**
     * @brief 设置中心位置
     * @param x x坐标
     * @param y y坐标
     * @return 构建器引用
     */
    KnobBuilder& setCenter(double x, double y);

    /**
     * @brief 设置半径
     * @param r 半径
     * @return 构建器引用
     */
    KnobBuilder& setRadius(double r);

    /**
     * @brief 设置值范围
     * @param minVal 最小值
     * @param maxVal 最大值
     * @return 构建器引用
     */
    KnobBuilder& setRange(double minVal, double maxVal);

    /**
     * @brief 设置步进值
     * @param s 步进值
     * @return 构建器引用
     */
    KnobBuilder& setStep(double s);

    /**
     * @brief 设置初始值
     * @param val 初始值
     * @return 构建器引用
     */
    KnobBuilder& setValue(double val);

    /**
     * @brief 设置颜色
     * @param fg 前景色
     * @param bg 背景色
     * @return 构建器引用
     */
    KnobBuilder& setColor(color_t fg, color_t bg);

    /**
     * @brief 设置偏移角度
     * @param angle 偏移角度（度）
     * @return 构建器引用
     */
    KnobBuilder& setOffsetAngle(double angle);

    /**
     * @brief 设置内部范围
     * @param innerMin 内部最小值
     * @param innerMax 内部最大值
     * @return 构建器引用
     */
    KnobBuilder& setInnerRange(double innerMin, double innerMax);

    /**
     * @brief 设置是否显示值
     * @param show 是否显示
     * @return 构建器引用
     */
    KnobBuilder& setShowValue(bool show);

    /**
     * @brief 设置字体大小
     * @param size 字体大小
     * @return 构建器引用
     */
    KnobBuilder& setFontSize(int size);

    /**
     * @brief 设置禁用状态
     * @param disabled 是否禁用
     * @return 构建器引用
     */
    KnobBuilder& setDisabled(bool disabled);

    /**
     * @brief 设置只读状态
     * @param readonly 是否只读
     * @return 构建器引用
     */
    KnobBuilder& setReadonly(bool readonly);

    /**
     * @brief 设置缩放比例
     * @param s 缩放比例
     * @return 构建器引用
     */
    KnobBuilder& setScale(double s);

    /**
     * @brief 设置值改变回调
     * @param callback 回调函数
     * @return 构建器引用
     */
    KnobBuilder& setOnChange(std::function<void(double)> callback);

    /**
     * @brief 构建 Knob 对象
     * @return Knob 指针
     */
    Knob* build();

private:
    std::wstring identifier;
    double cx = 0, cy = 0;
    double radius = 60;
    double minValue = 0.0;
    double maxValue = 100.0;
    double step = 1.0;
    double value = 50.0;
    color_t fgColor = EGERGB(33, 150, 243);
    color_t bgColor = EGERGB(230, 230, 230);
    double offsetAngle = 0.0;
    double innerMin = 0.0;
    double innerMax = 100.0;
    bool showValue = true;
    int fontSize = 0;
    bool disabled = false;
    bool readonly = false;
    double scale = 1.0;
    std::function<void(double)> onChange = nullptr;
};

/**
 * @brief 垂直滚动条控件类，用于Panel内容滚动
 * 
 * 显示在Panel右侧，包含顶部和底部箭头按钮以及中间可拖动的滑块。
 * 滑块的相对长度与Panel内部子控件占据的总高度相关。
 */
class ScrollBar {
public:
    /**
     * @brief 构造函数
     * @param barWidth 滚动条宽度
     * @param barHeight 滚动条高度（等于Panel高度）
     */
    ScrollBar(double barWidth, double barHeight);

    /**
     * @brief 绘制滚动条
     * @param dst 目标图像
     * @param x 滚动条左上角x坐标（相对于Panel layer）
     * @param y 滚动条左上角y坐标（相对于Panel layer）
     * @param scale 缩放比例
     */
    void draw(PIMAGE dst, double x, double y, double scale);

    /**
     * @brief 处理鼠标事件
     * @param msg 鼠标消息
     * @param scrollBarLeft 滚动条在屏幕坐标中的左边界
     * @param scrollBarTop 滚动条在屏幕坐标中的上边界
     * @param scale 缩放比例
     * @return 是否消费了事件
     */
    bool handleEvent(const mouse_msg& msg, double scrollBarLeft, double scrollBarTop, double scale);

    /**
     * @brief 设置内容范围（子控件的最小Y到最大Y的跨度）
     * @param contentHeight 内容总高度
     * @param viewHeight 可见区域高度
     */
    void setContentRange(double contentHeight, double viewHeight);

    /**
     * @brief 设置滚动位置（0.0 ~ 1.0）
     * @param pos 滚动位置
     */
    void setScrollPosition(double pos);

    /**
     * @brief 获取滚动位置（0.0 ~ 1.0）
     * @return 当前滚动位置
     */
    double getScrollPosition() const;

    /**
     * @brief 获取滚动条宽度
     * @return 宽度
     */
    double getWidth() const;

    /**
     * @brief 设置尺寸
     * @param w 宽度
     * @param h 高度
     */
    void setSize(double w, double h);

    /**
     * @brief 是否需要显示滚动条（内容高度 > 可见高度）
     * @return 是否需要显示
     */
    bool isNeeded() const;

    /**
     * @brief 设置关联的Panel（用于通知重绘）
     * @param p Panel指针
     */
    void setParentPanel(Panel* p);

private:
    double barWidth_;             ///< 滚动条宽度
    double barHeight_;            ///< 滚动条总高度
    double contentHeight_ = 0;    ///< 内容总高度
    double viewHeight_ = 0;       ///< 可见区域高度
    double scrollPos_ = 0.0;      ///< 滚动位置(0.0~1.0)
    double targetScrollPos_ = 0.0;///< 目标滚动位置
    
    // 按钮状态
    bool topBtnHovered_ = false;
    bool topBtnPressed_ = false;
    bool bottomBtnHovered_ = false;
    bool bottomBtnPressed_ = false;

    // 轨道按下状态（持续滚动）
    bool trackPressed_ = false;
    int trackScrollDir_ = 0;         ///< 轨道滚动方向: -1=上, 1=下

    // 持续滚动时的setAlwaysDirty管理
    bool btnScrollActive_ = false;
    double pressStartTime_ = 0;      ///< 按钮/轨道按下的时间戳(ms)

    // 平滑滚动动画
    bool smoothScrollActive_ = false; ///< 平滑滚动动画是否激活

    // 滑块状态
    bool thumbHovered_ = false;
    bool thumbPressed_ = false;
    bool thumbDragging_ = false;
    double dragOffset_ = 0;       ///< 拖动偏移

    Panel* parentPanel_ = nullptr;

    /**
     * @brief 获取按钮区域高度（正方形按钮）
     */
    double getButtonSize(double scale) const;

    /**
     * @brief 获取滑块的Y位置和高度
     * @param scale 缩放比例
     * @param outY 输出滑块Y位置（相对于轨道顶部）
     * @param outH 输出滑块高度
     */
    void getThumbRect(double scale, double& outY, double& outH) const;

    /**
     * @brief 绘制三角形箭头
     * @param dst 目标图像
     * @param centerX 中心X
     * @param centerY 中心Y
     * @param size 三角形大小
     * @param up 是否朝上
     * @param color 颜色
     */
    void drawArrow(PIMAGE dst, double centerX, double centerY, double size, bool up, color_t color);
};


/**
 * @brief Box布局容器
 * 
 * Box是一个自带FlexLayout的透明容器，继承自Panel但不显示任何背景和边框，
 * 仅用于对子控件进行排版和显示
 */
class Box : public Panel {
public:
    /**
     * @brief 构造函数
     * @param cx 中心 x 坐标
     * @param cy 中心 y 坐标
     * @param w 宽度
     * @param h 高度
     */
    Box(double cx, double cy, double w, double h);
    
    /**
     * @brief 析构函数
     */
    ~Box();

    /**
     * @brief 绘制 Box 到指定图层（仅绘制子控件，不绘制背景）
     * @param dst 目标图像
     * @param x 粘贴到 dst 中的中心 x 坐标
     * @param y 粘贴到 dst 中的中心 y 坐标
     */
    void draw(PIMAGE dst, double x, double y) override;
    
    /**
     * @brief 绘制 Box 到默认图像
     */
    void draw() override;
    
    /**
     * @brief 设置缩放比例（Box特殊缩放：子控件相对于自己的中心缩放，位置保持不变）
     * @param s 缩放比例
     */
    void setScale(double s) override;
};

/**
 * @brief Box构建器
 */
class BoxBuilder {
public:
    BoxBuilder& setIdentifier(const std::wstring& identifier);
    BoxBuilder& setCenter(double x, double y);
    BoxBuilder& setSize(double w, double h);
    BoxBuilder& setScale(double s);
    BoxBuilder& setDirection(LayoutDirection dir);
    BoxBuilder& setAlign(LayoutAlign align);
    BoxBuilder& setSpacing(double s);
    BoxBuilder& setPadding(double p);
    BoxBuilder& addChild(Widget* child);
    BoxBuilder& addChild(const std::vector<Widget*>& children);
    Box* build();

protected:
    std::wstring identifier;
    double cx = 0, cy = 0;
    double width = 100, height = 50;
    double scale = 1.0;
    LayoutDirection direction = LayoutDirection::Row;
    LayoutAlign align = LayoutAlign::Start;
    double spacing = 0;
    double padding = 0;
    std::vector<Widget*> children;
};

extern std::vector<Widget*> widgets;                ///< 全局控件集合（Widget析构时自动移除）
extern std::map<std::wstring,Widget*> IdToWidget; ///< ID到控件的映射（Widget析构时自动移除）

/**
 * @brief 通过ID获取控件
 * @param identifier 控件ID
 * @return 控件指针(未找到返回nullptr)
 * @warning 返回的指针可能在任何时候失效，使用前应检查有效性
 */
Widget* getWidgetById(const std::wstring& identifier);

/**
 * @brief 将指定的 Widget 序列赋值给内部容器。
 * * @details 该函数会清除原有的 Widget 排序或记录，并使用传入的 vector 
 * 进行覆盖。通常用于重新排列或完全替换当前的 Widget 集合。
 * * @param widgetWithOrder 包含 Widget 指针的向量，代表了新的排列顺序。
 * * @note 调用者需确保 vector 中的指针在 Widget 生命周期内有效。
 */
void assignOrder(std::vector<Widget*> widgetWithOrder);

/**
 * @brief 在当前序列中就地插入或追加 Widget 排序。
 * * @param widgetWithOrder 包含待处理 Widget 指针的向量。
 */
void emplaceOrder(const std::vector<Widget*>& widgetWithOrder);