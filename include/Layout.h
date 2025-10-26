/**
 * @file Layout.h
 * @brief UI布局系统定义（目前提供 FlexLayout）
 * @author FeJS8888
 */

#pragma once

#include "Widget.h"

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

    // 链式配置接口
    FlexLayout& direction(LayoutDirection dir);
    FlexLayout& spacing(double s);
    FlexLayout& padding(double p);
    FlexLayout& align(LayoutAlign align);

    // 应用布局
    void apply(Panel& parent) override;

private:
    LayoutDirection direction_ = LayoutDirection::Row;
    LayoutAlign align_ = LayoutAlign::Start;
    double spacing_ = 0;
    double padding_ = 0;
};
