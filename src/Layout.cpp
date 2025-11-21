#include "Layout.h"
#include "Widget.h"

// ============ FlexLayout ============

FlexLayout& FlexLayout::direction(LayoutDirection dir) {
    direction_ = dir;
    return *this;
}

FlexLayout& FlexLayout::spacing(double s) {
    spacing_ = s;
    return *this;
}

FlexLayout& FlexLayout::padding(double p) {
    padding_ = p;
    return *this;
}

FlexLayout& FlexLayout::align(LayoutAlign align) {
    align_ = align;
    return *this;
}

void FlexLayout::apply(Panel& parent) {
    std::vector<Widget*>& children = parent.getChildren();
    if (children.empty()) return;

    double panelWidth = parent.getWidth();
    double panelHeight = parent.getHeight();
    double panelScale = parent.getScale();

    // 计算所有子项的总长度
    // 注意：子项的宽高已经是缩放后的值，需要转换到未缩放的坐标系
    double totalChildrenLength = 0;
    for (auto* child : children)
        totalChildrenLength += (direction_ == LayoutDirection::Row
                                ? child->getWidth()
                                : child->getHeight()) / panelScale;
    double totalLength = totalChildrenLength + spacing_ * (children.size() - 1);

    double cursor = 0.0;

    // === 横向布局 ===
    if (direction_ == LayoutDirection::Row) {
        switch (align_) {
            case LayoutAlign::Start:
                cursor = -panelWidth / 2 / panelScale + padding_;
                break;
            case LayoutAlign::Center:
                cursor = -totalLength / 2.0;
                break;
            case LayoutAlign::End:
                cursor = +panelWidth / 2 / panelScale - totalLength - padding_;
                break;
            case LayoutAlign::SpaceBetween:
                if (children.size() > 1)
                    spacing_ = (panelWidth / panelScale - totalChildrenLength - padding_ * 2) / (children.size() - 1);
                cursor = -panelWidth / 2 / panelScale + padding_;
                break;
        }

        for (int i = 0;i < children.size();++ i) {
            Widget* child = children[i];
            double childWidth = child->getWidth() / panelScale;
            double cx = cursor + childWidth / 2.0;
            parent.setChildrenOffset(i, {cx, 0});
            cursor += childWidth + spacing_;
        }
    }

    // === 纵向布局 ===
    else {
        switch (align_) {
            case LayoutAlign::Start:
                cursor = -panelHeight / 2 / panelScale + padding_;
                break;
            case LayoutAlign::Center:
                cursor = -totalLength / 2.0;
                break;
            case LayoutAlign::End:
                cursor = +panelHeight / 2 / panelScale - totalLength - padding_;
                break;
            case LayoutAlign::SpaceBetween:
                if (children.size() > 1)
                    spacing_ = (panelHeight / panelScale - totalChildrenLength - padding_ * 2) / (children.size() - 1);
                cursor = -panelHeight / 2 / panelScale + padding_;
                break;
        }

        for (int i = 0;i < children.size();++ i) {
            Widget* child = children[i];
            double childHeight = child->getHeight() / panelScale;
            double cy = cursor + childHeight / 2.0;
            parent.setChildrenOffset(i, {0, cy});
            cursor += childHeight + spacing_;
        }
    }
}

// ============ FlexLayoutBuilder ============

FlexLayoutBuilder& FlexLayoutBuilder::setDirection(LayoutDirection dir) {
    direction_ = dir;
    return *this;
}

FlexLayoutBuilder& FlexLayoutBuilder::setAlign(LayoutAlign align) {
    align_ = align;
    return *this;
}

FlexLayoutBuilder& FlexLayoutBuilder::setSpacing(double s) {
    spacing_ = s;
    return *this;
}

FlexLayoutBuilder& FlexLayoutBuilder::setPadding(double p) {
    padding_ = p;
    return *this;
}

std::shared_ptr<FlexLayout> FlexLayoutBuilder::build() {
    auto layout = std::make_shared<FlexLayout>();
    layout->setDirection(direction_);
    layout->setAlign(align_);
    layout->setSpacing(spacing_);
    layout->setPadding(padding_);
    return layout;
}
