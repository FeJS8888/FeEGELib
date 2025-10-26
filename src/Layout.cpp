/**
 * @file Layout.cpp
 * @brief 布局系统实现
 */

#include "Layout.h"
#include "Widget.h"
#include <algorithm>

/**
 * @brief 设置方向
 */
FlexLayout& FlexLayout::direction(LayoutDirection dir) {
    direction_ = dir;
    return *this;
}

/**
 * @brief 设置间距
 */
FlexLayout& FlexLayout::spacing(double s) {
    spacing_ = s;
    return *this;
}

/**
 * @brief 设置内边距
 */
FlexLayout& FlexLayout::padding(double p) {
    padding_ = p;
    return *this;
}

/**
 * @brief 设置对齐方式
 */
FlexLayout& FlexLayout::align(LayoutAlign a) {
    align_ = a;
    return *this;
}

/**
 * @brief 应用布局到指定Panel
 */
void FlexLayout::apply(Panel& parent) {
    // 获取 Panel 的中心与尺寸
    Position panelPos = parent.getPosition();

    std::vector<Widget*>& children = parent.getChildren();
    if (children.empty()) return;

    // 临时假设每个控件都有 width 和 height（Widget 已定义）
    // 计算布局总长度
    double totalLength = padding_ * 2;
    for (size_t i = 0; i < children.size(); ++i) {
        Widget* child = children[i];
        totalLength += (direction_ == LayoutDirection::Row ? child->getWidth() : child->getHeight());
        if (i + 1 < children.size())
            totalLength += spacing_;
    }

    // 布局起始点
    double startX = panelPos.x - parent.getScale() * parent.getScale() * 0.5;
    double startY = panelPos.y - parent.getScale() * parent.getScale() * 0.5;

    // 对齐方式处理
    double offsetX = 0, offsetY = 0;
    if (direction_ == LayoutDirection::Row) {
        switch (align_) {
            case LayoutAlign::Start: offsetX = padding_; break;
            case LayoutAlign::Center: offsetX = -totalLength / 2 + padding_; break;
            case LayoutAlign::End: offsetX = -totalLength + padding_; break;
            case LayoutAlign::SpaceBetween: offsetX = 0; break;
        }
    } else {
        switch (align_) {
            case LayoutAlign::Start: offsetY = padding_; break;
            case LayoutAlign::Center: offsetY = -totalLength / 2 + padding_; break;
            case LayoutAlign::End: offsetY = -totalLength + padding_; break;
            case LayoutAlign::SpaceBetween: offsetY = 0; break;
        }
    }

    // 应用位置
    double cursor = (direction_ == LayoutDirection::Row ? offsetX : offsetY);
    for (auto* child : children) {
        if (direction_ == LayoutDirection::Row) {
            child->setPosition(static_cast<int>(panelPos.x + cursor + child->getWidth() / 2),
                               static_cast<int>(panelPos.y));
            cursor += child->getWidth() + spacing_;
        } else {
            child->setPosition(static_cast<int>(panelPos.x),
                               static_cast<int>(panelPos.y + cursor + child->getHeight() / 2));
            cursor += child->getHeight() + spacing_;
        }
    }
}
