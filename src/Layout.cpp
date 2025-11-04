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

    // 计算所有子项的总长度
    double totalChildrenLength = 0;
    for (auto* child : children)
        totalChildrenLength += (direction_ == LayoutDirection::Row
                                ? child->getWidth()
                                : child->getHeight());
    double totalLength = totalChildrenLength + spacing_ * (children.size() - 1);

    double cursor = 0.0;

    // === 横向布局 ===
    if (direction_ == LayoutDirection::Row) {
        switch (align_) {
            case LayoutAlign::Start:
                cursor = -panelWidth / 2 + padding_;
                break;
            case LayoutAlign::Center:
                cursor = -totalLength / 2.0;
                break;
            case LayoutAlign::End:
                cursor = +panelWidth / 2 - totalLength - padding_;
                break;
            case LayoutAlign::SpaceBetween:
                if (children.size() > 1)
                    spacing_ = (panelWidth - totalChildrenLength - padding_ * 2) / (children.size() - 1);
                cursor = -panelWidth / 2 + padding_;
                break;
        }

        for (int i = 0;i < children.size();++ i) {
            Widget* child = children[i];
            double cx = cursor + child->getWidth() / 2.0;
            parent.setChildrenOffset(i, {cx, 0});
            cursor += child->getWidth() + spacing_;
        }
    }

    // === 纵向布局 ===
    else {
        switch (align_) {
            case LayoutAlign::Start:
                cursor = -panelHeight / 2 + padding_;
                break;
            case LayoutAlign::Center:
                cursor = -totalLength / 2.0;
                break;
            case LayoutAlign::End:
                cursor = +panelHeight / 2 - totalLength - padding_;
                break;
            case LayoutAlign::SpaceBetween:
                if (children.size() > 1)
                    spacing_ = (panelHeight - totalChildrenLength - padding_ * 2) / (children.size() - 1);
                cursor = -panelHeight / 2 + padding_;
                break;
        }

        for (int i = 0;i < children.size();++ i) {
            Widget* child = children[i];
            double cy = cursor + child->getHeight() / 2.0;
            parent.setChildrenOffset(i, {0, cy});
            cursor += child->getHeight() + spacing_;
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
