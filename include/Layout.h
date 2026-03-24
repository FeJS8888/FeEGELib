/**
 * @file Layout.h
 * @brief UI布局系统定义（目前提供 FlexLayout）
 * @author FeJS8888
 */

#pragma once

#include <memory>

class Widget;
class Panel;

/**
 * @brief 布局方向
 */
enum class LayoutDirection {
    Row,    ///< 水平布局
    Column  ///< 垂直布局
};

/**
 * @brief 对齐方式
 */
enum class LayoutAlign {
    Start,
    Center,
    End,
    SpaceBetween
};

/**
 * @brief 布局计算结果
 * 
 * 包含内容区域的实际范围信息，用于滚动条等需要了解内容大小的场景。
 */
struct LayoutResult {
    double contentMinY = 0;   ///< 内容区域最上端（包含padding，相对于Panel中心，未缩放坐标系）
    double contentMaxY = 0;   ///< 内容区域最下端（包含padding，相对于Panel中心，未缩放坐标系）
};

/**
 * @brief 布局基类
 * 
 * 仅负责计算位置，不负责绘制。
 * @param parent 父Panel
 * @param scrollOffset 滚动偏移量（未缩放坐标系），布局时直接应用到子控件的偏移中
 */
class Layout {
public:
    virtual ~Layout() = default;
    virtual LayoutResult apply(Panel& parent, double scrollOffset = 0) = 0;
};

/**
 * @brief Flex布局（类似Quasar的Row/Column）
 */
class FlexLayout : public Layout {
public:
    FlexLayout() = default;

    // 普通链式配置接口
    FlexLayout& direction(LayoutDirection dir);
    FlexLayout& spacing(double s);
    FlexLayout& padding(double p);
    FlexLayout& align(LayoutAlign align);

    // 应用布局
    LayoutResult apply(Panel& parent, double scrollOffset = 0) override;

    // --- Getter (供 Builder 使用) ---
    LayoutDirection getDirection() const { return direction_; }
    LayoutAlign getAlign() const { return align_; }
    double getSpacing() const { return spacing_; }
    double getPadding() const { return padding_; }

    // --- Setter (供 Builder 使用) ---
    void setDirection(LayoutDirection dir) { direction_ = dir; }
    void setAlign(LayoutAlign align) { align_ = align; }
    void setSpacing(double s) { spacing_ = s; }
    void setPadding(double p) { padding_ = p; }

private:
    LayoutDirection direction_ = LayoutDirection::Row;
    LayoutAlign align_ = LayoutAlign::Start;
    double spacing_ = 0;
    double padding_ = 0;
};

/**
 * @brief FlexLayout 构建器类
 * 
 * 负责配置并生成 FlexLayout 对象
 */
class FlexLayoutBuilder {
public:
    FlexLayoutBuilder& setDirection(LayoutDirection dir);
    FlexLayoutBuilder& setAlign(LayoutAlign align);
    FlexLayoutBuilder& setSpacing(double s);
    FlexLayoutBuilder& setPadding(double p);

    std::shared_ptr<FlexLayout> build();

private:
    LayoutDirection direction_ = LayoutDirection::Row;
    LayoutAlign align_ = LayoutAlign::Start;
    double spacing_ = 0;
    double padding_ = 0;
};
