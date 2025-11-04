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
 * @brief 布局基类
 * 
 * 仅负责计算位置，不负责绘制。
 */
class Layout {
public:
    virtual ~Layout() = default;
    virtual void apply(Panel& parent) = 0;
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
    void apply(Panel& parent) override;

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
