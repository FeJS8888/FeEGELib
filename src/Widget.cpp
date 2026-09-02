#include "Widget.h"
#include "../3rdparty/xege/src/feege.h"

using namespace FeEGE; 
Widget* mouseOwningFlag = nullptr;
Widget* focusingWidget = nullptr;

vector<Widget*> widgets;
unordered_set<Widget*> widgetBackendRedraw;
double absolutPosDeltaX = 0,absolutPosDeltaY = 0;
bool PanelScaleChanged = false;

// 当前可绘制区域（全局坐标系），Panel绘制时逐层收窄
static constexpr double DRAWING_BOUND_MIN = -1e9;
static constexpr double DRAWING_BOUND_MAX = 1e9;
double globalDrawingLeft = DRAWING_BOUND_MIN;
double globalDrawingRight = DRAWING_BOUND_MAX;
double globalDrawingTop = DRAWING_BOUND_MIN;
double globalDrawingBottom = DRAWING_BOUND_MAX;

double Widget::getWidth(){
    return width;
}
double Widget::getHeight(){
    return height;
}

void Widget::deleteFocus(const mouse_msg& msg){

}

void Widget::releaseMouseOwningFlag(const mouse_msg& msg){

}

void Widget::catchMouseOwningFlag(const mouse_msg& msg){

}

void Widget::setParent(Widget* p){
    this->parent = p;
}

Widget* Widget::getParent(){
    return this->parent;
}

void Widget::setDrawing(bool d) {
    this->m_drawing += d ? 1 : -1;
    if(this->parent != nullptr) {
        this->parent->setDrawing(d);
    }
}

int Widget::getDrawingState() const {
    return this->m_drawing;
}

bool Widget::isBackendDirty() const {
    return needRedraw || getDrawingState() != 0;
}

void Widget::reset() {
    
}

Widget::~Widget() {
    // 从全局widgets集合中移除
    
    // 从IdToWidget映射中移除所有指向this的条目
    for (auto it = IdToWidget.begin(); it != IdToWidget.end(); ) {
        if(it->second == this) {
            it = IdToWidget.erase(it);
        }
        else {
            ++it;
        }
    }
}

Panel::Panel(double cx, double cy, double w, double h, double r, color_t bg) {
    this->cx = cx;
    this->cy = cy;
    this->width = w;
    this->height = h;
    this->bgColor = bg;
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = r;
    layer = newimage(w + 8,h + 8);
	ege_enable_aa(true,layer);
    ege_path_reset(&clippath);
    ege_path_addroundrect(&clippath,4,4,width,height,radius);
}

void Panel::addChild(Widget* child, double offsetX, double offsetY) {
    children.push_back(child);
    child->setParent(this);
    childOffsets.push_back(Position{ offsetX, offsetY });
    child->is_global = false;
    needRedraw = true;
    if(this->parent != nullptr) {
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void Panel::draw() {
    draw(nullptr,cx,cy);
}

void Panel::draw(PIMAGE dst, double x, double y) {
    double left = x - width / 2 - 4;
    double top = y - height / 2 - 4;
    double layerWidth = this->width + 8;
    double layerHeight = this->height + 8;

    if(!needRedraw && !needRedrawAlways){
        if(!BackendFlag) {
            putimage_withalpha(dst,layer,left,top);
        }
        return;
    }

    // 计算滚动偏移（未缩放坐标系），传递给Layout
    double layoutScrollOffset = 0;
    if(scrollBarEnabled_ && scrollBar_ && layout) {
        // 先不带偏移执行一次布局，获取内容范围
        LayoutResult extentResult = layout->apply(*this, 0);
        double contentH = (extentResult.contentMaxY - extentResult.contentMinY) * scale;
        double viewH = height;
        scrollBar_->setSize(scrollBar_->getWidth(), height);
        scrollBar_->setContentRange(contentH, viewH);

        if(scrollBar_->isNeeded()) {
            double maxScroll = contentH - viewH;
            // 初始偏移：让内容顶部对齐视口顶部
            double initialOffset = extentResult.contentMinY * scale + viewH / 2.0;
            scrollOffset_ = initialOffset + scrollBar_->getScrollPosition() * maxScroll;
            layoutScrollOffset = scrollOffset_ / scale;  // 转换到未缩放坐标系
        }
        else {
            scrollOffset_ = 0;
        }
    }

    // 应用布局（带滚动偏移）
    if(layout) layout->apply(*this, layoutScrollOffset);

    // 总是清空并重绘（子控件可能有动态内容）
    // 注意：子控件（如Button, InputBox）内部有自己的缓存机制来避免不必要的工作
    // 使用真正的透明色(PRGB32模式下alpha=0时RGB也应为0)
    setbkcolor_f(EGEARGB(0, 0, 0, 0), layer);
    cleardevice(layer);

    // 绘制自身背景（圆角矩形）
    ege_setclippath(&clippath,layer);
    setfillcolor(EGEACOLOR(255,bgColor), layer);
    setcolor(EGEACOLOR(255,RED), layer);
    setlinewidth(1,layer);
    ege_fillrect(0,0,layerWidth,layerHeight,layer);

    // 设置GDI级别裁剪区域（用于裁剪子控件的putimage_withalpha调用）
    // ege_setclippath仅影响GDI+绘图，putimage_withalpha使用GDI的AlphaBlend，
    // 需要通过SelectClipRgn设置GDI裁剪区域
    int ellipseSize = static_cast<int>(radius * 2);
    HRGN hRgn = CreateRoundRectRgn(4, 4, static_cast<int>(4 + width) + 1,
        static_cast<int>(4 + height) + 1, ellipseSize, ellipseSize);
    HDC layerDC = getHDC(layer);
    SelectClipRgn(layerDC, hRgn);

    // 绘制子控件（偏移已由Layout处理，无需手动减scrollOffset_）
    if(scaleChanged) PanelScaleChanged = true;

    // 收窄全局可绘制区域到本Panel范围
    double oldDrawingLeft = globalDrawingLeft, oldDrawingRight = globalDrawingRight;
    double oldDrawingTop = globalDrawingTop, oldDrawingBottom = globalDrawingBottom;
    globalDrawingLeft = std::max(globalDrawingLeft, cx - width / 2);
    globalDrawingRight = std::min(globalDrawingRight, cx + width / 2);
    globalDrawingTop = std::max(globalDrawingTop, cy - height / 2);
    globalDrawingBottom = std::min(globalDrawingBottom, cy + height / 2);

    double savedAbsPosX = absolutPosDeltaX;
    double savedAbsPosY = absolutPosDeltaY;
    needRedraw = false;
    for (int i = children.size() - 1; i >= 0; -- i) {
        double childX = layerWidth / 2 + childOffsets[i].x * scale;
        double childY = layerHeight / 2 + childOffsets[i].y * scale;
        // 累积父容器的屏幕偏移量，确保嵌套容器（如Panel>Box>InputBox）中的IME位置正确
        absolutPosDeltaX = savedAbsPosX + left;
        absolutPosDeltaY = savedAbsPosY + top;
        children[i]->setPosition(cx + childOffsets[i].x * scale,cy + childOffsets[i].y * scale);

        // 检查子控件是否在可绘制区域内，或有正在进行的动画需要继续更新
        double childCX = cx + childOffsets[i].x * scale;
        double childCY = cy + childOffsets[i].y * scale;
        double halfW = children[i]->getWidth() / 2.0;
        double halfH = children[i]->getHeight() / 2.0;
        bool withinBounds = (childCX + halfW > globalDrawingLeft) &&
                            (childCX - halfW < globalDrawingRight) &&
                            (childCY + halfH > globalDrawingTop) &&
                            (childCY - halfH < globalDrawingBottom);
        if(withinBounds || children[i]->getDrawingState() != 0) {
            children[i]->draw(layer, childX, childY);
        }

        absolutPosDeltaX = savedAbsPosX;
        absolutPosDeltaY = savedAbsPosY;
    }

    // 恢复全局可绘制区域
    globalDrawingLeft = oldDrawingLeft;
    globalDrawingRight = oldDrawingRight;
    globalDrawingTop = oldDrawingTop;
    globalDrawingBottom = oldDrawingBottom;

    PanelScaleChanged = false;
    scaleChanged = false;

    // 绘制滚动条（在clip path内）
    if(scrollBarEnabled_ && scrollBar_ && scrollBar_->isNeeded()) {
        double sbX = layerWidth - 4 - scrollBar_->getWidth();
        double sbY = 4;
        scrollBar_->draw(layer, sbX, sbY, scale);
    }

    // 移除GDI裁剪区域
    SelectClipRgn(layerDC, NULL);
    DeleteObject(hRgn);
    
    // 粘贴到主窗口
    ege_resetclippath(layer);
    setlinewidth(1,layer);
    setlinecolor(EGEACOLOR(255,bgColor), layer);
    ege_drawpath(&clippath,layer);
    if(!BackendFlag) {
        putimage_withalpha(dst,layer,left,top);
    }
}

bool Panel::isBackendDirty() const {
    return needRedraw || needRedrawAlways != 0 || getDrawingState() != 0;
}

Panel::~Panel(){
	if(layer) delimage(layer);
    if(scrollBar_) delete scrollBar_;
}

void Panel::setAlwaysDirty(bool d) {
    this->needRedrawAlways += ((int)d + d - 1); 
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setAlwaysDirty(d);
        }
    }
}

void Panel::setDirty() {
    this->needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

int Panel::getAlwaysDirtyState() {
    return this->needRedrawAlways;
}

void Panel::setPosition(double x,double y){
	cx = x;
	cy = y;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

Position Panel::getPosition(){
	return Position{cx * 1.00f,cy * 1.00f};
}

void Panel::setScale(double s){
    if(sgn(s - scale) == 0) return;
    scaleChanged = true;
	width = origin_width * s;
    height = origin_height * s;
    radius = origin_radius * s;
	scale = s;
	for (size_t i = 0; i < children.size(); ++i) {
        children[i]->setScale(s);
        children[i]->setPosition(cx + childOffsets[i].x * scale,cy + childOffsets[i].y * scale);
    }
	
    if(layer) delimage(layer);
    layer = newimage(width + 8,height + 8);
	ege_enable_aa(true,layer);
    ege_path_reset(&clippath);
    ege_path_addroundrect(&clippath,4,4,width,height,radius);

    if(scrollBar_) {
        scrollBar_->setSize(scrollBar_->getWidth(), height);
    }

    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

double Panel::getScale(){
	return scale;
}

bool isDescendant(Widget* target, const std::vector<Widget*>& children) {
    for(Widget* child : children) {
        if(child == target) {
            return true;
        }
        // Check if child is a Panel with its own children
        if(Panel* childPanel = dynamic_cast<Panel*>(child)) {
            if(isDescendant(target, childPanel->getChildren())) {
                return true;
            }
        }
    }
    return false;
}

bool Panel::isInside(double x, double y) const {
    double left = cx - width / 2;
    double top = cy - height / 2;
    // 转换为按钮内部坐标系
    double localX = x - left;
    double localY = y - top;

    // 先检查是否在按钮矩形框外
    if(localX < 0 || localX >= width || localY < 0 || localY >= height)
        return false;

    // 中心矩形区（不考虑圆角）直接返回 true
    if(localX >= radius && localX < width - radius)
        return true;
    if(localY >= radius && localY < height - radius)
        return true;

    // 检查四个圆角区域
    int dx, dy;
    // 左上角
    if(localX < radius && localY < radius) {
        dx = radius - localX;
        dy = radius - localY;
        return dx * dx + dy * dy <= radius * radius;
    }
    // 右上角
    if(localX >= width - radius && localY < radius) {
        dx = localX - (width - radius);
        dy = radius - localY;
        return dx * dx + dy * dy <= radius * radius;
    }
    // 左下角
    if(localX < radius && localY >= height - radius) {
        dx = radius - localX;
        dy = localY - (height - radius);
        return dx * dx + dy * dy <= radius * radius;
    }
    // 右下角
    if(localX >= width - radius && localY >= height - radius) {
        dx = localX - (width - radius);
        dy = localY - (height - radius);
        return dx * dx + dy * dy <= radius * radius;
    }

    // 其余区域为中间的十字形部分
    return true;
}

void Panel::deleteFocus(const mouse_msg& msg) {
    focusingWidget = nullptr;
}

bool Panel::handleEvent(const mouse_msg& msg){
    int mx = msg.x,my = msg.y;
    double left = cx - width / 2;
    double top = cy - height / 2;
    bool isin = isInside(mx, my);

    // 处理滚动条拖动（即使鼠标在面板外也要处理）
    if(scrollBarEnabled_ && scrollBar_ && scrollBar_->isNeeded()) {
        // 非滚轮事件（拖拽等）始终处理；滚轮事件仅在鼠标位于面板内时处理，
        // 不受子控件焦点状态影响（否则InputBox获焦后无法滚动面板）
        if(!msg.is_wheel() || (msg.is_wheel() && isin)){
            double sbLeft = left + width - scrollBar_->getWidth();
            double sbTop = top;
            // 拖动时始终处理
            if(scrollBar_->handleEvent(msg, sbLeft, sbTop, scale)) {
                if(msg.is_left() && msg.is_down() && focusingWidget == nullptr){
                    focusingWidget = this;
                }
                return true;
            }
        }
    }

    if(!isin) {
        // When clicking outside the Panel, remove focus from any descendant widget that has focus
        if(msg.is_left() && msg.is_down() && focusingWidget != nullptr) {
            if(focusingWidget == this || isDescendant(focusingWidget, children)) {
                focusingWidget->deleteFocus(msg);
            }
        }
        // 鼠标移出Panel区域时，通知子控件以便重置鼠标指针形状
        // （如InputBox的IDC_IBEAM→IDC_ARROW），仅在无拖动操作时执行。
        // 使用远离屏幕的合成坐标，而非实际鼠标坐标，以避免滚动超出视口的
        // 不可见子控件恰好命中真实鼠标位置而错误触发IDC_IBEAM。
        if(msg.is_move() && (mouseOwningFlag == nullptr || mouseOwningFlag == this)) {
            mouse_msg leaveMsg = msg;
            leaveMsg.x = -9999;
            leaveMsg.y = -9999;
            for (Widget* w : children) {
                w->handleEvent(leaveMsg);
            }
        }
        return false;
    }
    // When clicking inside this Panel, if the focused widget is in another Panel, remove its focus
    if(msg.is_left() && msg.is_down() && focusingWidget != nullptr) {
        if(!isDescendant(focusingWidget, children)) {
            focusingWidget->deleteFocus(msg);
        }
    }
    
	for(Widget* w : children){
        bool state = w->handleEvent(msg);
        if(state){
            if(msg.is_left() && msg.is_down() && focusingWidget == nullptr){
                focusingWidget = this;
            }
            return true;
        }
    }
    if(msg.is_left() && msg.is_down()){
        mouseOwningFlag = this;
        return true;
    }
    else if(msg.is_left() && msg.is_up()){
        if(mouseOwningFlag == this){
            mouseOwningFlag = nullptr;
            focusingWidget = this;
        }
        else if(mouseOwningFlag != nullptr){
            mouseOwningFlag->releaseMouseOwningFlag(msg);
        }
        return true;
    }
    else{
        if(mouseOwningFlag != nullptr && mouseOwningFlag != this){
            mouseOwningFlag->catchMouseOwningFlag(msg);
        }
    }
    return true;
}

void Panel::setSize(double w,double h){
    origin_width = width = w;
    origin_height = height = h;
    if(layer) delimage(layer);
    layer = newimage(width,height);
    ege_enable_aa(true,layer);
    ege_path p;
    ege_path_addroundrect(&p,0,0,width,height,radius);
    ege_setclippath(&p,layer);

	needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void Panel::clearChildren(){
    children.clear();
    childOffsets.clear();
}

void Panel::setAlpha(double a) {
    double alpha1 = clamp(a, 0, 255);
    if(sgn(alpha1 - alpha) == 0) return;
    alpha = alpha1;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

std::vector<Widget*>& Panel::getChildren() { 
    return children; 
}

void Panel::reset(){
    for(Widget* w : children){
        w->reset();
        w->setNeedRedraw(true);
    }
    setDirty();
}

void Panel::setChildrenOffset(int index,Position pos){
    if(index >= 0 && index < static_cast<int>(childOffsets.size())) {
        childOffsets[index] = pos;
    }
}

PanelBuilder& PanelBuilder::setIdentifier(const wstring& id) {
    identifier = id;
    return *this;
}

PanelBuilder& PanelBuilder::setCenter(double x, double y) {
    cx = x; cy = y;
    return *this;
}

PanelBuilder& PanelBuilder::setSize(double w, double h) {
    width = w; height = h;
    return *this;
}

PanelBuilder& PanelBuilder::setRadius(double r) {
    radius = r;
    return *this;
}

PanelBuilder& PanelBuilder::setBackground(color_t color) {
    bg = color;
    return *this;
}

PanelBuilder& PanelBuilder::setScale(double s) {
    scale = s;
    return *this;
}

PanelBuilder& PanelBuilder::addChild(Widget* child, double offsetX, double offsetY) {
    children.push_back(child);
    childOffsets.push_back(Position{ offsetX, offsetY });
    return *this;
}

PanelBuilder& PanelBuilder::addChild(const std::vector<Widget*>& child, const std::vector<double>& offsetX, const std::vector<double>& offsetY) {
    for(size_t i = 0; i < child.size(); ++i){
        children.push_back(child[i]);
        if(offsetX.size() > i && offsetY.size() > i){
            childOffsets.push_back(Position{ offsetX[i], offsetY[i] });
        }
        else{
            childOffsets.push_back(Position{ 0, 0 });
        }
    }
    return *this;
}

PanelBuilder& PanelBuilder::setLayout(std::shared_ptr<Layout> l) {
    layout = std::move(l);
    return *this;
}

Panel* PanelBuilder::build() {
    auto panel = new Panel(cx, cy, width, height, radius, bg);
    panel->setScale(scale);
    //widgets.insert(panel);
    IdToWidget[identifier] = panel;
    if(layout) panel->setLayout(layout);
    for(size_t i = 0;i < children.size();++ i){
        panel->addChild(children[i],childOffsets[i].x,childOffsets[i].y);
    }
    if(scrollBarEnabled) {
        panel->enableScrollBar(true, scrollBarWidth);
    }
    return panel;
}

// ButtonBuilder 实现
Ripple::Ripple(int _x, int _y, int _r, int _life,Widget* _p,int _c)
    : x(_x), y(_y), maxRadius(_r), life(_life), parent(_p), counter(_c) {}

bool Ripple::alive() const {    
    if(auto btn = dynamic_cast<Button*>(parent)){
        if(btn->getParent() != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(btn->getParent())) {
                if(!m_setDirtyState){
                    p->setAlwaysDirty(true);
                    btn->setDrawing(true);
                    m_setDirtyState = true;
                }
                p->setDirty();
            }
            else if (Box* b = dynamic_cast<Box*>(btn->getParent())) {
                if(!m_setDirtyState){
                    b->setAlwaysDirty(true);
                    btn->setDrawing(true);
                    m_setDirtyState = true;
                }
                b->setDirty();
            }
            else assert(false && "Button's parent is not a Panel or a Box");
        }
        bool state = (btn->getClickState() && (btn->getMCounter() == counter)) || (age < life);
        if(!state){
            if(btn->getParent() != nullptr){
                if(Panel* p = dynamic_cast<Panel*>(btn->getParent())) {
                    p->setAlwaysDirty(false);
                    btn->setDrawing(false);
                }
                else if(Box* b = dynamic_cast<Box*>(btn->getParent())) {
                    b->setAlwaysDirty(false);
                    btn->setDrawing(false);
                }
                else assert(false && "Button's parent is not a Panel or a Box");
            }
        }
        return state;
    }
    if(auto ib = dynamic_cast<InputBox*>(parent)){
        if(ib->getParent() != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(ib->getParent())) {
                if(!m_setDirtyState){
                    p->setAlwaysDirty(true);
                    ib->setDrawing(true);
                    m_setDirtyState = true;
                }
                p->setDirty();
            }
            else if(Box* b = dynamic_cast<Box*>(ib->getParent())) {
                if(!m_setDirtyState){
                    b->setAlwaysDirty(true);
                    ib->setDrawing(true);
                    m_setDirtyState = true;
                }
                b->setDirty();
            }
            else assert(false && "InputBox's parent is not a Panel or a Box");
        }
        bool state =  (ib->getClickState() && (ib->getMCounter() == counter)) || (age < life);
        if(!state){
            if(ib->getParent() != nullptr){
                if(Panel* p = dynamic_cast<Panel*>(ib->getParent())) {
                    p->setAlwaysDirty(false);
                    ib->setDrawing(false);
                }
                else if(Box* b = dynamic_cast<Box*>(ib->getParent())) {
                    b->setAlwaysDirty(false);
                    ib->setDrawing(false);
                }
                else assert(false && "InputBox's parent is not a Panel or a Box");
            }
        }
        return state;
    }

    return age < life;
}

void Ripple::update() {
    // --- 空指针保护 ---
    if(!parent) {
        age ++;
        return;
    }

    // --- 动态类型安全检测 ---
    if(age >= life * 0.75) {
        if(auto btn = dynamic_cast<Button*>(parent)) {
            if(btn->getClickState() && (btn->getMCounter() == counter)) return;
        }
        else if(auto ib = dynamic_cast<InputBox*>(parent)) {
            if(ib->getClickState() && (ib->getMCounter() == counter)) return;
        }
    }

    // --- 正常递增 ---
    age++;
}

void Ripple::draw(PIMAGE dst) const {
    if(BackendFlag) return;
    double progress = (double)age / life;
    double r = maxRadius * progress;
    int alpha = static_cast<int>(120 * std::cos(progress * PI / 2));
    setfillcolor(EGEARGB(alpha, 30, 30, 30), dst);
    ege_fillellipse(x - r, y - r, r * 2, r * 2, dst);
}

void Ripple::draw_aa(PIMAGE dst) const {
    if(BackendFlag) return;
    double progress = (double)age / life;
    double r = maxRadius * progress;
    int alpha = static_cast<int>(120 * std::cos(progress * PI / 2));
    setlinecolor(EGEARGB(alpha, 30, 30, 30), dst);
    ege_ellipse(x - r, y - r, r * 2, r * 2, dst);
}

// Button 类实现
Button::Button(double cx, double cy, double w, double h, double r): radius(r) {
    this->cx = cx;
    this->cy = cy;
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = r;
    left = cx - width / 2;
    top = cy - height / 2;

    btnLayer = newimage(width + 8, height + 8);
    ege_enable_aa(true, btnLayer);
    ege_path_reset(&clippath);
    ege_path_addroundrect(&clippath,4,4,width,height,radius);
}

Button::~Button() {
    if(btnLayer) delimage(btnLayer);
}

void Button::draw(PIMAGE dst,double x,double y){
    double left = x - width / 2 - 4;
    double top = y - height / 2 - 4;
    double width = this->width + 8;
    double height = this->height + 8;
    if(!ripples.size() && !needRedraw){
        if(!BackendFlag) {
            putimage_withalpha(dst,btnLayer,left,top);
        }
        return;
    }
    // 使用真正的透明色(PRGB32模式下alpha=0时RGB也应为0)
    setbkcolor_f(EGEARGB(0, 0, 0, 0), btnLayer);
    cleardevice(btnLayer);
    
    ege_setclippath(&clippath,btnLayer);

    // 优化：只绘制一次背景到btnLayer，稍后复制到bgLayer
    setfillcolor(EGEACOLOR(255,color), btnLayer);
    ege_fillrect(0, 0, width, height, btnLayer);
                 
    if(icon != nullptr){
	    double iconW = getwidth(icon) * scale * iconSize / 100;
	    double iconH = getheight(icon) * scale * iconSize / 100;
	    double iconX = width / 2 - iconW / 2;
	    double iconY = height / 2 - iconH / 2;
	    putimage_alphablend(btnLayer,icon,iconX,iconY,iconW,iconH,255,0, 0,getwidth(icon), getheight(icon),true);
	}
    
    // 更新并绘制 ripples
    for (auto& r : ripples) {
        r.update();
    }
    // 优化：使用C++20 std::erase_if替代erase-remove惯用法
    std::erase_if(ripples, [](const Ripple& r) { return !r.alive(); });

    // 按钮文字
    setbkmode(TRANSPARENT, btnLayer);
    settextcolor(BLACK, btnLayer);
    ege_outtextxy(width / 2 - contentWidth / 2, 
                 height / 2 - contentHeight / 2, 
                 content.c_str(), btnLayer);
    
    // ege_resetclippath(btnLayer);
    

    // ege_setclippath(&clippath,btnLayer);
    for (auto& r : ripples) {
        r.draw(btnLayer);
    }

    ege_resetclippath(btnLayer);
    setlinewidth(0.5,btnLayer);
    setlinecolor(EGEACOLOR(255,color), btnLayer);
    ege_drawpath(&clippath,btnLayer);

    if(!BackendFlag) {
        putimage_withalpha(dst,btnLayer,left,top);
    }
    
    needRedraw = false;
}

void Button::draw(){
    draw(nullptr,cx,cy);
}

void Button::releaseMouseOwningFlag(const mouse_msg& msg){
    if(!msg.is_left() || !msg.is_up() || !m_clicking) return;
    bool inside = isInside(msg.x, msg.y);
    if(inside && on_click_event != nullptr) on_click_event();
    m_clicking = false;
    mouseOwningFlag = nullptr;
}

void Button::catchMouseOwningFlag(const mouse_msg& msg){
    // Button不需要处理拖动中的移动事件
    // 只需要在releaseMouseOwningFlag中处理点击即可
}

bool Button::handleEvent(const mouse_msg& msg) {
    bool inside = isInside(msg.x, msg.y);
    if(disabled){
        if(inside){
            setCursor(IDC_NO);
            lastInside = true;
            return true;
        }
        else if(lastInside){
            setCursor(IDC_ARROW);
            lastInside = false;
            return false;
        }
    }

    if(inside){
        lastInside = true;
    }
    else if(lastInside){
        setCursor(IDC_ARROW);
        lastInside = false;
    }
    
    // 处理其它控件焦点
    if(msg.is_left() && msg.is_up()){
        if(mouseOwningFlag != nullptr && mouseOwningFlag != this){
            mouseOwningFlag->releaseMouseOwningFlag(msg);
        }
    }

    if(msg.is_left() && msg.is_down() && inside) {
        int localX = msg.x - left;
        int localY = msg.y - top;
        m_counter++;
        ripples.emplace_back(localX, localY, 4.00f / 3.00f * std::sqrt(height * height + width * width), 70, dynamic_cast<Widget*>(this), m_counter);
        needRedraw = true;
        if(this->parent != nullptr){
            Panel* p = dynamic_cast<Panel*>(this->parent);
            if(p != nullptr) {
                p->setDirty();
            }
        }
        m_clicking = true;
        mouseOwningFlag = this;
        return true;
    }
    else if(msg.is_left() && msg.is_up() && m_clicking){
        if(inside && on_click_event != nullptr) on_click_event();
        m_clicking = false;
        mouseOwningFlag = nullptr;
        return true;
    }
    return inside;
}

bool Button::isInside(double x, double y) const {
    // 转换为按钮内部坐标系
    double localX = x - left;
    double localY = y - top;

    // 先检查是否在按钮矩形框外
    if(localX < 0 || localX >= width || localY < 0 || localY >= height)
        return false;

    // 中心矩形区（不考虑圆角）直接返回 true
    if(localX >= radius && localX < width - radius)
        return true;
    if(localY >= radius && localY < height - radius)
        return true;

    // 检查四个圆角区域
    int dx, dy;
    // 左上角
    if(localX < radius && localY < radius) {
        dx = radius - localX;
        dy = radius - localY;
        return dx * dx + dy * dy <= radius * radius;
    }
    // 右上角
    if(localX >= width - radius && localY < radius) {
        dx = localX - (width - radius);
        dy = radius - localY;
        return dx * dx + dy * dy <= radius * radius;
    }
    // 左下角
    if(localX < radius && localY >= height - radius) {
        dx = radius - localX;
        dy = localY - (height - radius);
        return dx * dx + dy * dy <= radius * radius;
    }
    // 右下角
    if(localX >= width - radius && localY >= height - radius) {
        dx = localX - (width - radius);
        dy = localY - (height - radius);
        return dx * dx + dy * dy <= radius * radius;
    }

    // 其余区域为中间的十字形部分
    return true;
}

void Button::setContent(const wstring& str){
    if(content == str) return;
	content = str;
    ege_setfont(23 * scale, L"宋体", btnLayer);
    measuretext(content.c_str(),&contentWidth,&contentHeight,btnLayer);
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

std::wstring Button::getContent(){
    return content;
}

void Button::setPosition(double x,double y){
    if(sgn(left - x + width / 2) == 0 && sgn(top - y + height / 2) == 0) return;
    left = x - width / 2;
	top = y - height / 2;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void Button::setScale(double s){
    if(sgn(scale - s) == 0) return;
	width = origin_width * s;
    height = origin_height * s;
    radius = origin_radius * s;
    scale = s;
    left = cx - width / 2;
    top = cy - height / 2;

    if(btnLayer) delimage(btnLayer);
    btnLayer = newimage(width + 8,height + 8);
    ege_enable_aa(true,btnLayer);
    ege_path_reset(&clippath);
    ege_path_addroundrect(&clippath,4,4,width,height,radius);

    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void Button::setIcon(PIMAGE img){
	icon = img;
}

void Button::setIconSize(int is){
	iconSize = is;
}

void Button::setOnClickEvent(std::function<void(void)> func){
	on_click_event = func;
}

void Button::setColor(color_t col){
    color = col;
    this->needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

bool Button::getClickState(){
    return m_clicking;
}

int Button::getMCounter(){
    return m_counter;
}

void Button::reset(){
    ripples.clear();
    ripples.shrink_to_fit();
}

void Button::disable(){
    disabled = true;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void Button::enable(){
    disabled = false;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

// InputBox 类实现
ButtonBuilder& ButtonBuilder::setIdentifier(const wstring& id) {
    identifier = id;
    return *this;
}

ButtonBuilder& ButtonBuilder::setCenter(double x, double y) {
    cx = x; cy = y;
    return *this;
}

ButtonBuilder& ButtonBuilder::setSize(double w, double h) {
    width = w; height = h;
    return *this;
}

ButtonBuilder& ButtonBuilder::setRadius(double r) {
    radius = r;
    return *this;
}

ButtonBuilder& ButtonBuilder::setContent(const std::wstring& text) {
    content = text;
    return *this;
}

ButtonBuilder& ButtonBuilder::setScale(double s) {
    scale = s;
    return *this;
}

ButtonBuilder& ButtonBuilder::setOnClick(std::function<void()> func) {
    onClick = [func]() {
        pushSchedule(func);
    };
    return *this;
}

ButtonBuilder& ButtonBuilder::setColor(color_t col){
    color = col;
    return *this;
}

ButtonBuilder& ButtonBuilder::setIcon(PIMAGE img){
	icon = img;
	return *this;
}

ButtonBuilder& ButtonBuilder::setIconSize(int is){
	iconSize = is;
	return *this;
}

Button* ButtonBuilder::build() {
    auto btn = new Button(cx, cy, width, height, radius);
    btn->setContent(content);
    btn->setScale(scale);
    btn->setColor(color);
    if(onClick) btn->setOnClickEvent(onClick);
    if(icon) btn->setIcon(icon);
    btn->setIconSize(iconSize);
    //widgets.insert(btn);
    IdToWidget[identifier] = btn;
    return btn;
}

class InputBox::TextBuffer {
    struct Piece {
        bool fromAdditions = false;
        int offset = 0;
        int length = 0;
    };

    std::wstring base;
    std::wstring additions;
    std::vector<Piece> pieces;
    std::vector<Piece> scratchPieces;
    int textLength = 0;

    const std::wstring& sourceFor(const Piece& piece) const {
        return piece.fromAdditions ? additions : base;
    }

    static void appendPiece(std::vector<Piece>& target, bool fromAdditions,
                            int offset, int length) {
        if(length <= 0) return;
        if(!target.empty()) {
            Piece& previous = target.back();
            if(previous.fromAdditions == fromAdditions &&
               previous.offset + previous.length == offset) {
                previous.length += length;
                return;
            }
        }
        target.push_back({fromAdditions, offset, length});
    }

    void compact() {
        std::wstring merged;
        materialize(merged);
        base.swap(merged);
        additions.clear();
        pieces.clear();
        if(!base.empty()) pieces.push_back({false, 0, (int)base.size()});
        textLength = (int)base.size();
    }

public:
    int size() const { return textLength; }

    void assign(const std::wstring& text) {
        base = text;
        additions.clear();
        pieces.clear();
        textLength = (int)base.size();
        if(textLength > 0) pieces.push_back({false, 0, textLength});
    }

    wchar_t characterAt(int position) const {
        if(position < 0 || position >= textLength) return L'\0';
        int current = 0;
        for(const Piece& piece : pieces) {
            if(position < current + piece.length) {
                return sourceFor(piece)[piece.offset + position - current];
            }
            current += piece.length;
        }
        return L'\0';
    }

    void appendRange(int position, int length, std::wstring& destination) const {
        position = std::max(0, std::min(position, textLength));
        length = std::max(0, std::min(length, textLength - position));
        if(length == 0) return;

        const int rangeEnd = position + length;
        int current = 0;
        for(const Piece& piece : pieces) {
            const int pieceEnd = current + piece.length;
            if(pieceEnd <= position) {
                current = pieceEnd;
                continue;
            }
            if(current >= rangeEnd) break;
            const int begin = std::max(position, current);
            const int end = std::min(rangeEnd, pieceEnd);
            const std::wstring& source = sourceFor(piece);
            destination.append(source, (size_t)(piece.offset + begin - current),
                (size_t)(end - begin));
            current = pieceEnd;
        }
    }

    void copyRange(int position, int length, std::wstring& destination) const {
        position = std::max(0, std::min(position, textLength));
        length = std::max(0, std::min(length, textLength - position));
        destination.clear();
        destination.reserve((size_t)length);
        appendRange(position, length, destination);
    }

    void materialize(std::wstring& destination) const {
        destination.clear();
        if(textLength == 0) return;
        destination.reserve((size_t)textLength);
        for(const Piece& piece : pieces) {
            const std::wstring& source = sourceFor(piece);
            destination.append(source, (size_t)piece.offset, (size_t)piece.length);
        }
    }

    bool equals(const std::wstring& text) const {
        if((int)text.size() != textLength) return false;
        int position = 0;
        for(const Piece& piece : pieces) {
            const std::wstring& source = sourceFor(piece);
            if(!std::equal(source.begin() + piece.offset,
                source.begin() + piece.offset + piece.length, text.begin() + position)) {
                return false;
            }
            position += piece.length;
        }
        return true;
    }

    void replace(int begin, int end, const std::wstring& replacement) {
        begin = std::max(0, std::min(begin, textLength));
        end = std::max(begin, std::min(end, textLength));

        const int replacementOffset = (int)additions.size();
        if(!replacement.empty()) additions += replacement;

        scratchPieces.clear();
        scratchPieces.reserve(pieces.size() + 3);
        std::vector<Piece>& updated = scratchPieces;
        bool inserted = false;
        int current = 0;
        for(const Piece& piece : pieces) {
            const int pieceEnd = current + piece.length;
            if(pieceEnd <= begin) {
                appendPiece(updated, piece.fromAdditions, piece.offset, piece.length);
            }
            else if(current >= end) {
                if(!inserted) {
                    appendPiece(updated, true, replacementOffset, (int)replacement.size());
                    inserted = true;
                }
                appendPiece(updated, piece.fromAdditions, piece.offset, piece.length);
            }
            else {
                if(current < begin) {
                    appendPiece(updated, piece.fromAdditions, piece.offset, begin - current);
                }
                if(!inserted) {
                    appendPiece(updated, true, replacementOffset, (int)replacement.size());
                    inserted = true;
                }
                if(pieceEnd > end) {
                    appendPiece(updated, piece.fromAdditions,
                        piece.offset + end - current, pieceEnd - end);
                }
            }
            current = pieceEnd;
        }
        if(!inserted) appendPiece(updated, true, replacementOffset, (int)replacement.size());

        textLength += (int)replacement.size() - (end - begin);
        pieces.swap(updated);

        // Random edits can otherwise grow the piece list without bound.  The
        // threshold stays well above normal typing at one caret, where adjacent
        // additions coalesce into one piece.
        if(pieces.size() > 2048) compact();
    }
};

// InputBoxBuilder 实现
InputBox::InputBox(double cx, double cy, double w, double h, double r, bool multiline) {
    this->multiline = multiline;
    textBuffer = std::make_unique<TextBuffer>();
    this->cx = cx;
    this->cy = cy;
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = r;
    left = cx - width / 2;
    top = cy - height / 2;
    btnLayer = newimage(width + 8, height + 8);
    ege_enable_aa(true, btnLayer);
    ege_path_reset(&clippath);
    ege_path_addroundrect(&clippath,4,4,width,height,radius);

    inv.create(multiline, multiline ? 0 : 2);
    inv.visible(false);
    inv.move(-1, -1);
    // A standard multiline EDIT with a zero-sized client area rejects
    // WM_CHAR once its text contains a line break. Keep the hidden input/IME
    // proxy large enough to have a valid formatting rectangle. AUTOHSCROLL
    // prevents this small native control from owning the visible wrapping.
    inv.size(multiline ? 64 : 0, multiline ? 64 : 0);
    inv.setmaxlen(2147483640);
    inv.setparent(this);
    inv.killfocus();
    
    on_focus = false;

    float _w,_h;
    measuretext("a",&_w,&_h,btnLayer);
    m_ime_pos_x = left;
    m_ime_pos_y = top + height / 2 + _h / 2 + 2;
}

InputBox::~InputBox() {
    if(btnLayer) delimage(btnLayer);
}

const double BLINK_FREQUENCY = 0.65;
const double PAUSE_THRESHOLD = 0.6;
double InputBoxSinDoubleForCursor(double time) {
    double sine_value = std::sin(time * 2.0 * M_PI * BLINK_FREQUENCY);
    if(sine_value > PAUSE_THRESHOLD) {
        return 1.0;
    }
    if(sine_value < -PAUSE_THRESHOLD) {
        return 0.0;
    }
    return (sine_value + 1.0) / 2.0;
}

int InputBox::contentLength() const {
    return textBuffer ? textBuffer->size() : (int)content.size();
}

wchar_t InputBox::contentCharacterAt(int position) const {
    if(textBuffer) return textBuffer->characterAt(position);
    return position >= 0 && position < (int)content.size() ? content[position] : L'\0';
}

void InputBox::copyContentRange(int position, int length, std::wstring& destination) const {
    if(textBuffer) {
        textBuffer->copyRange(position, length, destination);
        return;
    }
    position = std::max(0, std::min(position, (int)content.size()));
    length = std::max(0, std::min(length, (int)content.size() - position));
    destination.assign(content, (size_t)position, (size_t)length);
}

std::wstring InputBox::copyContentRange(int position, int length) const {
    std::wstring result;
    copyContentRange(position, length, result);
    return result;
}

int InputBox::displayTextLength() const {
    return contentLength() + (int)IMECompositionString.size();
}

wchar_t InputBox::displayCharacterAt(int position) const {
    const int length = displayTextLength();
    if(position < 0 || position >= length) return L'\0';
    if(IMECompositionString.empty()) return contentCharacterAt(position);

    const int compositionStart = std::max(0, std::min(cursor_pos, contentLength()));
    const int compositionEnd = compositionStart + (int)IMECompositionString.size();
    if(position < compositionStart) return contentCharacterAt(position);
    if(position < compositionEnd) return IMECompositionString[position - compositionStart];
    return contentCharacterAt(position - (int)IMECompositionString.size());
}

void InputBox::copyDisplayRange(int position, int length, std::wstring& destination) const {
    const int displayLength = displayTextLength();
    position = std::max(0, std::min(position, displayLength));
    length = std::max(0, std::min(length, displayLength - position));
    destination.clear();
    destination.reserve((size_t)length);
    if(length == 0) return;

    if(IMECompositionString.empty()) {
        if(textBuffer) textBuffer->appendRange(position, length, destination);
        else destination.append(content, (size_t)position, (size_t)length);
        return;
    }

    const int compositionStart = std::max(0, std::min(cursor_pos, contentLength()));
    const int compositionEnd = compositionStart + (int)IMECompositionString.size();
    const int rangeEnd = position + length;
    auto appendContent = [&](int contentPosition, int count) {
        if(count <= 0) return;
        if(textBuffer) textBuffer->appendRange(contentPosition, count, destination);
        else destination.append(content, (size_t)contentPosition, (size_t)count);
    };

    if(position < compositionStart) {
        appendContent(position, std::min(rangeEnd, compositionStart) - position);
    }
    if(rangeEnd > compositionStart && position < compositionEnd) {
        const int start = std::max(position, compositionStart) - compositionStart;
        const int end = std::min(rangeEnd, compositionEnd) - compositionStart;
        destination.append(IMECompositionString, (size_t)start, (size_t)(end - start));
    }
    if(rangeEnd > compositionEnd) {
        const int displayStart = std::max(position, compositionEnd);
        appendContent(displayStart - (int)IMECompositionString.size(), rangeEnd - displayStart);
    }
}

std::wstring InputBox::copyDisplayRange(int position, int length) const {
    std::wstring result;
    copyDisplayRange(position, length, result);
    return result;
}

const std::wstring& InputBox::materializedContent() const {
    if(textBuffer && contentCacheDirty) {
        textBuffer->materialize(content);
        contentCacheDirty = false;
    }
    return content;
}

bool InputBox::contentEquals(const std::wstring& text) const {
    return textBuffer ? textBuffer->equals(text) : content == text;
}

void InputBox::assignContentModel(const std::wstring& text) {
    if(textBuffer) textBuffer->assign(text);
    else content = text;
    content = text;
    contentCacheDirty = false;
}

void InputBox::replaceContentRange(int begin, int end, const std::wstring& replacement) {
    const int length = contentLength();
    begin = std::max(0, std::min(begin, length));
    end = std::max(begin, std::min(end, length));
    if(textBuffer) {
        textBuffer->replace(begin, end, replacement);
        contentCacheDirty = true;
        return;
    }
    content.replace((size_t)begin, (size_t)(end - begin), replacement);
}

void InputBox::invalidateTextLayout() {
    textLayoutDirty = true;
    pendingTextLayoutChange.active = false;
    stableTextLayoutSuffix.clear();
    stableTextLayoutSuffixStart = -1;
    textLayoutComplete = false;
    textLayoutBuildCursor = 0;
    clearCaretVisualLineHint();
    invalidatePendingVerticalNavigation();
    ++textLayoutVersion;
    if(textLayoutVersion == 0) textLayoutVersion = 1;
}

int InputBox::paragraphStartForPosition(const std::wstring& text, int position) {
    const int textLength = (int)text.size();
    position = std::max(0, std::min(position, textLength));
    if(position < textLength && position > 0 && text[position] == L'\n' && text[position - 1] == L'\r') {
        ++position;
    }
    for(int pos = position - 1; pos >= 0; --pos) {
        if(text[pos] == L'\n') return pos + 1;
        if(text[pos] == L'\r') {
            return pos + 1 < textLength && text[pos + 1] == L'\n' ? pos + 2 : pos + 1;
        }
    }
    return 0;
}

int InputBox::paragraphEndForPosition(const std::wstring& text, int position) {
    const int textLength = (int)text.size();
    position = std::max(0, std::min(position, textLength));
    for(int pos = position; pos < textLength; ++pos) {
        if(text[pos] == L'\r') {
            return pos + 1 < textLength && text[pos + 1] == L'\n' ? pos + 2 : pos + 1;
        }
        if(text[pos] == L'\n') return pos + 1;
    }
    return textLength;
}

int InputBox::textLineIndexAtOrAfter(int displayPosition) const {
    const auto first = std::lower_bound(textLines.begin(), textLines.end(), displayPosition,
        [](const TextLineLayout& line, int position) { return line.start < position; });
    return (int)(first - textLines.begin());
}

void InputBox::invalidateTextLayoutForContentChange(const std::wstring& oldText,
                                                    const std::wstring& newText,
                                                    int oldChangedStart,
                                                    int oldChangedEnd,
                                                    int newChangedStart,
                                                    int newChangedEnd) {
    const bool layoutIsCurrent = multiline && IMECompositionString.empty() &&
        !textLayoutDirty && builtTextLayoutVersion == textLayoutVersion && !textLines.empty();
    const bool oldLayoutComplete = textLayoutComplete;
    const int oldBuildCursor = textLayoutBuildCursor;

    const int oldLength = (int)oldText.size();
    const int newLength = (int)newText.size();
    oldChangedStart = std::max(0, std::min(oldChangedStart, oldLength));
    oldChangedEnd = std::max(oldChangedStart, std::min(oldChangedEnd, oldLength));
    newChangedStart = std::max(0, std::min(newChangedStart, newLength));
    newChangedEnd = std::max(newChangedStart, std::min(newChangedEnd, newLength));

    textLayoutDirty = true;
    pendingTextLayoutChange.active = false;
    ++textLayoutVersion;
    if(textLayoutVersion == 0) textLayoutVersion = 1;
    if(!layoutIsCurrent) return;

    constexpr int immediateParagraphCharacterLimit = 4096;

    auto hasNearbyParagraphBoundary = [](const std::wstring& text, int position,
                                         bool searchBackward) {
        position = std::max(0, std::min(position, (int)text.size()));
        constexpr int limit = 4097;
        if(searchBackward) {
            const int first = std::max(0, position - limit);
            for(int index = position - 1; index >= first; --index) {
                if(text[index] == L'\r' || text[index] == L'\n') return true;
            }
            return first == 0;
        }

        const int last = std::min((int)text.size(), position + limit);
        for(int index = position; index < last; ++index) {
            if(text[index] == L'\r' || text[index] == L'\n') return true;
        }
        return last == (int)text.size();
    };

    const bool canMeasureWholeParagraphLocally =
        hasNearbyParagraphBoundary(oldText, oldChangedStart, true) &&
        hasNearbyParagraphBoundary(oldText, oldChangedEnd, false) &&
        hasNearbyParagraphBoundary(newText, newChangedStart, true) &&
        hasNearbyParagraphBoundary(newText, newChangedEnd, false);

    // Small explicit paragraphs can be replaced atomically.  The native EDIT
    // selection tells us the change range, so this path never scans a shared
    // prefix or suffix of the entire document.
    if(oldLayoutComplete && canMeasureWholeParagraphLocally) {
        const int oldParagraphStart = paragraphStartForPosition(oldText, oldChangedStart);
        const int oldParagraphEnd = paragraphEndForPosition(oldText, oldChangedEnd);
        const int newParagraphStart = paragraphStartForPosition(newText, newChangedStart);
        const int newParagraphEnd = paragraphEndForPosition(newText, newChangedEnd);
        if(oldParagraphStart == newParagraphStart &&
           std::max(oldParagraphEnd - oldParagraphStart, newParagraphEnd - newParagraphStart) <=
               immediateParagraphCharacterLimit) {
            const int oldFirstLine = textLineIndexAtOrAfter(oldParagraphStart);
            const int oldAfterLastLine = oldParagraphEnd == oldLength
                ? (int)textLines.size()
                : textLineIndexAtOrAfter(oldParagraphEnd);
            if(oldFirstLine < (int)textLines.size() &&
               textLines[oldFirstLine].start == oldParagraphStart &&
               oldAfterLastLine >= oldFirstLine && oldAfterLastLine <= (int)textLines.size() &&
               (oldAfterLastLine == (int)textLines.size() ||
                textLines[oldAfterLastLine].start == oldParagraphEnd)) {
                pendingTextLayoutChange.active = true;
                pendingTextLayoutChange.progressive = false;
                pendingTextLayoutChange.oldStart = oldParagraphStart;
                pendingTextLayoutChange.oldEnd = oldParagraphEnd;
                pendingTextLayoutChange.newStart = newParagraphStart;
                pendingTextLayoutChange.newEnd = newParagraphEnd;
                pendingTextLayoutChange.rebuildStart = oldParagraphStart;
                pendingTextLayoutChange.oldFirstLine = oldFirstLine;
                pendingTextLayoutChange.oldAfterLastLine = oldAfterLastLine;
                return;
            }
        }
    }

    // A long logical paragraph can cause every following visual line to
    // change after one insertion.  Keep the stable prefix and resume from a
    // visual-line boundary in small frame budgets instead of doing that work
    // in the key message that triggered the edit.
    int firstLine = 0;
    int rebuildStart = 0;
    if(oldLayoutComplete || oldChangedStart < oldBuildCursor) {
        firstLine = lineIndexForDisplayPosition(oldChangedStart);
        firstLine = std::max(0, std::min(firstLine, (int)textLines.size() - 1));
        rebuildStart = textLines[firstLine].start;
    }
    else {
        // The edit is after the already materialized prefix.  That prefix is
        // still exact, so continue from its end without throwing it away.
        firstLine = (int)textLines.size();
        rebuildStart = std::max(0, std::min(oldBuildCursor, newLength));
    }

    pendingTextLayoutChange.active = true;
    pendingTextLayoutChange.progressive = true;
    pendingTextLayoutChange.oldStart = oldChangedStart;
    pendingTextLayoutChange.oldEnd = oldChangedEnd;
    pendingTextLayoutChange.newStart = newChangedStart;
    pendingTextLayoutChange.newEnd = newChangedEnd;
    pendingTextLayoutChange.rebuildStart = rebuildStart;
    pendingTextLayoutChange.oldFirstLine = firstLine;
    int suffixFirstLine = (int)textLines.size();
    const bool preservesHardBreaks =
        oldText.find_first_of(L"\r\n", (size_t)oldChangedStart) >= (size_t)oldChangedEnd &&
        newText.find_first_of(L"\r\n", (size_t)newChangedStart) >= (size_t)newChangedEnd;
    if(preservesHardBreaks) {
        for(int lineIndex = firstLine; lineIndex < (int)textLines.size(); ++lineIndex) {
            if(textLines[lineIndex].hardBreak && textLines[lineIndex].end >= oldChangedEnd) {
                suffixFirstLine = lineIndex + 1;
                break;
            }
        }
    }
    pendingTextLayoutChange.oldAfterLastLine = suffixFirstLine;
}

void InputBox::invalidateTextLayoutForKnownChange(int oldChangedStart, int oldChangedEnd,
                                                  const std::wstring& replacement,
                                                  bool allowLocalReplacement) {
    const bool layoutIsCurrent = multiline && IMECompositionString.empty() &&
        !textLayoutDirty && builtTextLayoutVersion == textLayoutVersion && !textLines.empty();
    const bool oldLayoutComplete = textLayoutComplete;
    const int oldBuildCursor = textLayoutBuildCursor;
    const int oldLength = contentLength();
    oldChangedStart = std::max(0, std::min(oldChangedStart, oldLength));
    oldChangedEnd = std::max(oldChangedStart, std::min(oldChangedEnd, oldLength));

    textLayoutDirty = true;
    pendingTextLayoutChange.active = false;
    ++textLayoutVersion;
    if(textLayoutVersion == 0) textLayoutVersion = 1;
    if(!layoutIsCurrent) return;

    constexpr int immediateParagraphCharacterLimit = 4096;
    auto hasBreak = [](const std::wstring& text) {
        return text.find_first_of(L"\r\n") != std::wstring::npos;
    };
    auto rangeHasBreak = [this](int begin, int end) {
        constexpr int limit = 4097;
        end = std::max(begin, std::min(end, contentLength()));
        if(end - begin > limit) return true;
        copyContentRange(begin, end - begin, paragraphScanScratch);
        return paragraphScanScratch.find_first_of(L"\r\n") != std::wstring::npos;
    };
    auto paragraphStart = [this](int position, int& result) {
        constexpr int limit = 4097;
        position = std::max(0, std::min(position, contentLength()));
        const int first = std::max(0, position - limit);
        copyContentRange(first, position - first, paragraphScanScratch);
        for(int index = (int)paragraphScanScratch.size() - 1; index >= 0; --index) {
            const wchar_t character = paragraphScanScratch[index];
            if(character == L'\n') {
                result = first + index + 1;
                return true;
            }
            if(character == L'\r') {
                const int absolute = first + index;
                result = absolute + 1 < contentLength() && contentCharacterAt(absolute + 1) == L'\n'
                    ? absolute + 2 : absolute + 1;
                return true;
            }
        }
        if(first == 0) {
            result = 0;
            return true;
        }
        return false;
    };
    auto paragraphEnd = [this](int position, int& result) {
        constexpr int limit = 4097;
        const int length = contentLength();
        position = std::max(0, std::min(position, length));
        const int last = std::min(length, position + limit);
        copyContentRange(position, last - position, paragraphScanScratch);
        for(int index = 0; index < (int)paragraphScanScratch.size(); ++index) {
            const wchar_t character = paragraphScanScratch[index];
            const int absolute = position + index;
            if(character == L'\r') {
                result = absolute + 1 < length && contentCharacterAt(absolute + 1) == L'\n'
                    ? absolute + 2 : absolute + 1;
                return true;
            }
            if(character == L'\n') {
                result = absolute + 1;
                return true;
            }
        }
        if(last == length) {
            result = length;
            return true;
        }
        return false;
    };

    // Fast local repair stays available for short explicit paragraphs.  All
    // searches are capped, so a long unbroken paragraph never turns a key
    // event into a document-length scan.
    int oldParagraphStart = 0;
    int oldParagraphEnd = 0;
    const int characterDelta = (int)replacement.size() - (oldChangedEnd - oldChangedStart);
    if(allowLocalReplacement && oldLayoutComplete && !hasBreak(replacement) &&
       !rangeHasBreak(oldChangedStart, oldChangedEnd) &&
       paragraphStart(oldChangedStart, oldParagraphStart) &&
       paragraphEnd(oldChangedEnd, oldParagraphEnd) &&
       oldParagraphEnd - oldParagraphStart <= immediateParagraphCharacterLimit &&
       oldParagraphEnd - oldParagraphStart + characterDelta <= immediateParagraphCharacterLimit) {
        const int oldFirstLine = textLineIndexAtOrAfter(oldParagraphStart);
        const int oldAfterLastLine = oldParagraphEnd == oldLength
            ? (int)textLines.size()
            : textLineIndexAtOrAfter(oldParagraphEnd);
        if(oldFirstLine < (int)textLines.size() &&
           textLines[oldFirstLine].start == oldParagraphStart &&
           oldAfterLastLine >= oldFirstLine && oldAfterLastLine <= (int)textLines.size() &&
           (oldAfterLastLine == (int)textLines.size() ||
            textLines[oldAfterLastLine].start == oldParagraphEnd)) {
            pendingTextLayoutChange.active = true;
            pendingTextLayoutChange.progressive = false;
            pendingTextLayoutChange.oldStart = oldParagraphStart;
            pendingTextLayoutChange.oldEnd = oldParagraphEnd;
            pendingTextLayoutChange.newStart = oldParagraphStart;
            pendingTextLayoutChange.newEnd = oldParagraphEnd + characterDelta;
            pendingTextLayoutChange.rebuildStart = oldParagraphStart;
            pendingTextLayoutChange.oldFirstLine = oldFirstLine;
            pendingTextLayoutChange.oldAfterLastLine = oldAfterLastLine;
            return;
        }
    }

    int firstLine = 0;
    int rebuildStart = 0;
    if(oldLayoutComplete || oldChangedStart < oldBuildCursor) {
        firstLine = lineIndexForDisplayPosition(oldChangedStart);
        firstLine = std::max(0, std::min(firstLine, (int)textLines.size() - 1));
        rebuildStart = textLines[firstLine].start;
    }
    else {
        firstLine = (int)textLines.size();
        rebuildStart = std::max(0, std::min(oldBuildCursor, oldLength));
    }

    pendingTextLayoutChange.active = true;
    pendingTextLayoutChange.progressive = true;
    pendingTextLayoutChange.oldStart = oldChangedStart;
    pendingTextLayoutChange.oldEnd = oldChangedEnd;
    pendingTextLayoutChange.newStart = oldChangedStart;
    pendingTextLayoutChange.newEnd = oldChangedStart + (int)replacement.size();
    pendingTextLayoutChange.rebuildStart = rebuildStart;
    pendingTextLayoutChange.oldFirstLine = firstLine;
    int suffixFirstLine = (int)textLines.size();
    if(!hasBreak(replacement) && !rangeHasBreak(oldChangedStart, oldChangedEnd)) {
        for(int lineIndex = firstLine; lineIndex < (int)textLines.size(); ++lineIndex) {
            if(textLines[lineIndex].hardBreak && textLines[lineIndex].end >= oldChangedEnd) {
                suffixFirstLine = lineIndex + 1;
                break;
            }
        }
    }
    pendingTextLayoutChange.oldAfterLastLine = suffixFirstLine;
}

void InputBox::appendTextLines(const std::wstring& displayContent, int begin, int end,
                               bool includeTrailingLine, float availableWidth,
                               std::vector<TextLineLayout>& lines) const {
    begin = std::max(0, std::min(begin, (int)displayContent.size()));
    end = std::max(begin, std::min(end, (int)displayContent.size()));

    auto appendLine = [&](int lineStart, int lineEnd, float lineWidth, bool hardBreak) {
        TextLineLayout line;
        line.start = lineStart;
        line.end = lineEnd;
        line.width = lineWidth;
        line.hardBreak = hardBreak;
        lines.push_back(line);
    };
    auto measureRange = [&](int lineStart, int lineEnd) {
        if(lineEnd <= lineStart) return 0.0f;
        float measuredWidth = 0.0f, measuredHeight = 0.0f;
        measuretext(displayContent.substr(lineStart, lineEnd - lineStart).c_str(),
                    &measuredWidth, &measuredHeight, btnLayer);
        return measuredWidth;
    };
    auto appendWrappedSpan = [&](int spanStart, int spanEnd, bool hardBreak) {
        if(spanStart == spanEnd) {
            appendLine(spanStart, spanEnd, 0.0f, hardBreak);
            return;
        }

        const int spanLength = spanEnd - spanStart;
        std::vector<int> advances(spanLength);
        SIZE ignoredSize = {};
        const HDC dc = getHDC(btnLayer);
        const BOOL haveAdvances = dc && ::GetTextExtentExPointW(dc, displayContent.data() + spanStart,
            spanLength, std::numeric_limits<int>::max(), nullptr, advances.data(), &ignoredSize);
        const size_t lineBegin = lines.size();
        if(haveAdvances) {
            const int maxWidth = std::max(1, (int)std::floor(availableWidth));
            int localStart = 0;
            while(localStart < spanLength) {
                const int baseWidth = localStart == 0 ? 0 : advances[localStart - 1];
                const auto firstTooWide = std::upper_bound(advances.begin() + localStart, advances.end(),
                    baseWidth + maxWidth);
                int localEnd = (int)(firstTooWide - advances.begin());
                if(localEnd <= localStart) localEnd = localStart + 1;
                appendLine(spanStart + localStart, spanStart + localEnd,
                    (float)(advances[localEnd - 1] - baseWidth), false);
                localStart = localEnd;
            }
        }
        else {
            int lineStart = spanStart;
            int pos = spanStart;
            while(pos < spanEnd) {
                if(measureRange(lineStart, pos + 1) > availableWidth && pos > lineStart) {
                    appendLine(lineStart, pos, measureRange(lineStart, pos), false);
                    lineStart = pos;
                    continue;
                }
                ++pos;
            }
            appendLine(lineStart, spanEnd, measureRange(lineStart, spanEnd), false);
        }
        if(hardBreak && lines.size() > lineBegin) lines.back().hardBreak = true;
    };

    int lineStart = begin;
    int pos = begin;
    while(pos < end) {
        if(displayContent[pos] == L'\r' || displayContent[pos] == L'\n') {
            appendWrappedSpan(lineStart, pos, true);
            if(displayContent[pos] == L'\r' && pos + 1 < end && displayContent[pos + 1] == L'\n') {
                pos += 2;
            }
            else {
                ++pos;
            }
            lineStart = pos;
            continue;
        }
        ++pos;
    }
    if(lineStart < end || includeTrailingLine) {
        appendWrappedSpan(lineStart, end, false);
    }
}

void InputBox::appendNextTextLine(int& position, float availableWidth,
                                  std::vector<TextLineLayout>& lines) const {
    const int textLength = displayTextLength();
    position = std::max(0, std::min(position, textLength));
    if(position >= textLength) return;

    const int lineStart = position;
    constexpr int maximumMeasuredCharacters = 4096;
    // A normal visual line contains a small fraction of 4096 UTF-16 units.
    // Probe close to the viewport width first, then expand only when every
    // probed character fits (for unusually narrow or zero-width text).
    int probeLength = std::min(maximumMeasuredCharacters,
        std::max(64, (int)std::ceil(availableWidth / 4.0f) + 32));
    std::wstring& measurementText = layoutMeasureScratch;
    int spanEnd = lineStart;
    int spanLength = 0;
    int fittedCharacters = 0;
    bool hasHardBreak = false;
    bool measured = false;
    const HDC dc = getHDC(btnLayer);
    for(;;) {
        const int measurementEnd = std::min(textLength, lineStart + probeLength);
        copyDisplayRange(lineStart, measurementEnd - lineStart, measurementText);
        int localSpanEnd = 0;
        while(localSpanEnd < (int)measurementText.size() && measurementText[localSpanEnd] != L'\r' &&
              measurementText[localSpanEnd] != L'\n') {
            ++localSpanEnd;
        }
        spanEnd = lineStart + localSpanEnd;
        spanLength = localSpanEnd;
        hasHardBreak = spanEnd < textLength &&
            (displayCharacterAt(spanEnd) == L'\r' || displayCharacterAt(spanEnd) == L'\n');

        fittedCharacters = spanLength;
        measured = false;
        if(dc && spanLength > 0) {
            SIZE ignoredSize = {};
            fittedCharacters = 0;
            measured = ::GetTextExtentExPointW(dc, measurementText.data(),
                spanLength, std::max(1, (int)std::floor(availableWidth)),
                &fittedCharacters, nullptr, &ignoredSize) != FALSE;
        }
        if(!measured) {
            auto measureRange = [&](int length) {
                float measuredWidth = 0.0f, measuredHeight = 0.0f;
                if(length > 0) {
                    measuretext(measurementText.substr(0, length).c_str(),
                        &measuredWidth, &measuredHeight, btnLayer);
                }
                return measuredWidth;
            };
            int low = 0;
            int high = spanLength;
            while(low < high) {
                const int middle = low + (high - low + 1) / 2;
                if(measureRange(middle) <= availableWidth) low = middle;
                else high = middle - 1;
            }
            fittedCharacters = low;
        }

        const bool mayNeedMore = fittedCharacters >= spanLength && !hasHardBreak &&
            measurementEnd < textLength && probeLength < maximumMeasuredCharacters;
        if(!mayNeedMore) break;
        probeLength = std::min(maximumMeasuredCharacters, probeLength * 2);
    }

    if(spanEnd == lineStart && hasHardBreak) {
        TextLineLayout line;
        line.start = lineStart;
        line.end = lineStart;
        line.hardBreak = true;
        lines.push_back(line);
        position = lineStart + (displayCharacterAt(lineStart) == L'\r' &&
            lineStart + 1 < textLength && displayCharacterAt(lineStart + 1) == L'\n' ? 2 : 1);
        return;
    }

    float lineWidth = 0.0f;

    if(fittedCharacters <= 0 && spanLength > 0) fittedCharacters = 1;
    fittedCharacters = std::min(fittedCharacters, spanLength);
    const int lineEnd = lineStart + fittedCharacters;
    if(dc && fittedCharacters > 0) {
        SIZE measuredSize = {};
        if(::GetTextExtentPoint32W(dc, measurementText.data(),
            fittedCharacters, &measuredSize)) {
            lineWidth = (float)measuredSize.cx;
        }
    }
    if(!dc || (fittedCharacters > 0 && lineWidth == 0.0f)) {
        float measuredHeight = 0.0f;
        measuretext(measurementText.substr(0, fittedCharacters).c_str(),
            &lineWidth, &measuredHeight, btnLayer);
    }

    TextLineLayout line;
    line.start = lineStart;
    line.end = lineEnd;
    line.width = lineWidth;
    line.hardBreak = hasHardBreak && fittedCharacters == spanLength;
    lines.push_back(line);

    if(line.hardBreak) {
        position = spanEnd + (displayCharacterAt(spanEnd) == L'\r' &&
            spanEnd + 1 < textLength && displayCharacterAt(spanEnd + 1) == L'\n' ? 2 : 1);
    }
    else {
        position = lineEnd;
    }
}

void InputBox::beginProgressiveTextLayout(int firstLine, int displayStart,
                                          int suffixFirstLine, int characterDelta) {
    firstLine = std::max(0, std::min(firstLine, (int)textLines.size()));
    suffixFirstLine = std::max(firstLine, std::min(suffixFirstLine, (int)textLines.size()));
    stableTextLayoutSuffix.assign(textLines.begin() + suffixFirstLine, textLines.end());
    for(TextLineLayout& line : stableTextLayoutSuffix) {
        line.start += characterDelta;
        line.end += characterDelta;
    }
    stableTextLayoutSuffixStart = stableTextLayoutSuffix.empty()
        ? -1 : stableTextLayoutSuffix.front().start;
    textLines.erase(textLines.begin() + firstLine, textLines.end());
    textLayoutBuildCursor = std::max(0, std::min(displayStart, displayTextLength()));
    textLayoutComplete = false;
}

void InputBox::continueTextLayout(float availableWidth) {
    if(textLayoutComplete) return;

    const int textLength = displayTextLength();
    const auto startedAt = std::chrono::steady_clock::now();
    constexpr auto timeBudget = std::chrono::microseconds(2000);
    constexpr int maximumLineBudget = 96;
    int builtLines = 0;
    while(textLayoutBuildCursor < textLength && builtLines < maximumLineBudget) {
        if(stableTextLayoutSuffixStart >= 0 &&
           textLayoutBuildCursor == stableTextLayoutSuffixStart) {
            textLines.insert(textLines.end(), stableTextLayoutSuffix.begin(),
                stableTextLayoutSuffix.end());
            stableTextLayoutSuffix.clear();
            stableTextLayoutSuffixStart = -1;
            textLayoutBuildCursor = textLength;
            textLayoutComplete = true;
            return;
        }
        if(stableTextLayoutSuffixStart >= 0 &&
           textLayoutBuildCursor > stableTextLayoutSuffixStart) {
            stableTextLayoutSuffix.clear();
            stableTextLayoutSuffixStart = -1;
        }
        const int oldCursor = textLayoutBuildCursor;
        appendNextTextLine(textLayoutBuildCursor, availableWidth, textLines);
        if(textLayoutBuildCursor <= oldCursor) break;
        ++builtLines;
        if(std::chrono::steady_clock::now() - startedAt >= timeBudget) break;
    }

    if(textLayoutBuildCursor >= textLength) {
        textLayoutBuildCursor = textLength;
        if(textLines.empty() || textLines.back().hardBreak) {
            TextLineLayout trailingLine;
            trailingLine.start = textLength;
            trailingLine.end = textLength;
            textLines.push_back(trailingLine);
        }
        textLayoutComplete = true;
        stableTextLayoutSuffix.clear();
        stableTextLayoutSuffixStart = -1;
    }
}

bool InputBox::applyPendingTextLayoutChange(float availableWidth) {
    if(!pendingTextLayoutChange.active || pendingTextLayoutChange.progressive ||
       !IMECompositionString.empty()) return false;

    const PendingTextLayoutChange change = pendingTextLayoutChange;
    if(change.oldFirstLine < 0 || change.oldAfterLastLine < change.oldFirstLine ||
        change.oldAfterLastLine > (int)textLines.size() || change.newStart != change.oldStart ||
        change.newEnd < change.newStart || change.newEnd > displayTextLength()) {
        return false;
    }

    std::vector<TextLineLayout> replacement;
    std::wstring replacementText = copyDisplayRange(change.newStart, change.newEnd - change.newStart);
    appendTextLines(replacementText, 0, (int)replacementText.size(),
        change.newEnd == displayTextLength(), availableWidth, replacement);
    for(TextLineLayout& line : replacement) {
        line.start += change.newStart;
        line.end += change.newStart;
    }
    textLines.erase(textLines.begin() + change.oldFirstLine, textLines.begin() + change.oldAfterLastLine);
    textLines.insert(textLines.begin() + change.oldFirstLine, replacement.begin(), replacement.end());

    const int characterDelta = change.newEnd - change.oldEnd;
    const int firstSuffixLine = change.oldFirstLine + (int)replacement.size();
    for(int lineIndex = firstSuffixLine; lineIndex < (int)textLines.size(); ++lineIndex) {
        textLines[lineIndex].start += characterDelta;
        textLines[lineIndex].end += characterDelta;
    }
    pendingTextLayoutChange.active = false;
    textLayoutBuildCursor = displayTextLength();
    textLayoutComplete = true;
    return true;
}

void InputBox::ensureTextLayout() {
    if(!multiline || !btnLayer) {
        return;
    }

    const bool needsVersionBuild = textLayoutDirty || builtTextLayoutVersion != textLayoutVersion;
    if(!needsVersionBuild && textLayoutComplete) return;

    ege_setfont((float)std::max(1.0, text_height * scale), L"宋体", btnLayer);
    layoutPaddingX = 14.0 * scale;
    layoutPaddingY = std::max(6.0, 8.0 * scale);

    float fontWidth = 0.0f, fontHeight = 0.0f;
    measuretext(L"a", &fontWidth, &fontHeight, btnLayer);
    layoutLineHeight = std::max((double)fontHeight, text_height * scale) + 4.0;

    const float availableWidth = std::max(1.0f, (float)(width - 2.0 * layoutPaddingX));
    if(needsVersionBuild) {
        bool layoutReused = false;
        if(pendingTextLayoutChange.active) {
            if(pendingTextLayoutChange.progressive) {
                beginProgressiveTextLayout(pendingTextLayoutChange.oldFirstLine,
                    pendingTextLayoutChange.rebuildStart,
                    pendingTextLayoutChange.oldAfterLastLine,
                    (pendingTextLayoutChange.newEnd - pendingTextLayoutChange.newStart) -
                    (pendingTextLayoutChange.oldEnd - pendingTextLayoutChange.oldStart));
                layoutReused = true;
            }
            else {
                layoutReused = applyPendingTextLayoutChange(availableWidth);
            }
        }

        if(!layoutReused) {
            pendingTextLayoutChange.active = false;
            textLines.clear();
            stableTextLayoutSuffix.clear();
            stableTextLayoutSuffixStart = -1;
            textLayoutBuildCursor = 0;
            textLayoutComplete = false;
        }

        pendingTextLayoutChange.active = false;
        textLayoutDirty = false;
        builtTextLayoutVersion = textLayoutVersion;
    }

    if(!textLayoutComplete) continueTextLayout(availableWidth);
    if(textLines.empty()) textLines.push_back({});

    const double viewportHeight = std::max(1.0, height - 2.0 * layoutPaddingY);
    if(textLayoutComplete) {
        const double maxScroll = std::max(0.0, textLines.size() * layoutLineHeight - viewportHeight);
        scroll_offset_y = std::max(0.0, std::min(scroll_offset_y, maxScroll));
    }
}

std::pair<int, int> InputBox::visibleTextLineRange() const {
    if(textLines.empty()) return {0, 0};

    const double lineHeight = std::max(1.0, layoutLineHeight);
    const double viewportHeight = std::max(1.0, height - 2.0 * layoutPaddingY);
    const int first = std::max(0, (int)std::floor(scroll_offset_y / lineHeight) - 1);
    const int last = std::min((int)textLines.size(),
        (int)std::ceil((scroll_offset_y + viewportHeight) / lineHeight) + 1);
    return {first, std::max(first, last)};
}

int InputBox::displayPositionForContentPosition(int contentPos) const {
    contentPos = std::max(0, std::min(contentPos, contentLength()));
    if(IMECompositionString.empty() || contentPos <= cursor_pos) return contentPos;
    return contentPos + (int)IMECompositionString.size();
}

int InputBox::contentPositionForDisplayPosition(int displayPos) const {
    displayPos = std::max(0, displayPos);
    if(IMECompositionString.empty()) return std::min(displayPos, contentLength());

    const int compositionStart = std::max(0, std::min(cursor_pos, contentLength()));
    const int compositionEnd = compositionStart + (int)IMECompositionString.size();
    if(displayPos <= compositionStart) return displayPos;
    if(displayPos < compositionEnd) return compositionStart;
    return std::min(displayPos - (int)IMECompositionString.size(), contentLength());
}

int InputBox::lineIndexForDisplayPosition(int displayPos) const {
    if(textLines.empty()) return 0;
    displayPos = std::max(0, std::min(displayPos, displayTextLength()));
    const auto nextLine = std::lower_bound(textLines.begin(), textLines.end(), displayPos,
        [](const TextLineLayout& line, int position) { return line.start < position; });
    if(nextLine == textLines.end()) return (int)textLines.size() - 1;
    if(nextLine == textLines.begin() || nextLine->start == displayPos) {
        return (int)(nextLine - textLines.begin());
    }
    return (int)(nextLine - textLines.begin() - 1);
}

bool InputBox::caretVisualLineHintIsValid(int displayPos) const {
    if(caretVisualLineHint < 0 || caretVisualLineHint >= (int)textLines.size() ||
       caretVisualLineLayoutVersion != builtTextLayoutVersion) {
        return false;
    }
    const TextLineLayout& line = textLines[caretVisualLineHint];
    return displayPos >= line.start && displayPos <= line.end;
}

int InputBox::lineIndexForCaretDisplayPosition(int displayPos) const {
    return caretVisualLineHintIsValid(displayPos)
        ? caretVisualLineHint : lineIndexForDisplayPosition(displayPos);
}

void InputBox::clearCaretVisualLineHint() {
    caretVisualLineHint = -1;
    caretVisualLineLayoutVersion = 0;
}

void InputBox::setCaretVisualLineHint(int lineIndex, int displayPos) {
    if(lineIndex < 0 || lineIndex >= (int)textLines.size() ||
       displayPos < textLines[lineIndex].start || displayPos > textLines[lineIndex].end) {
        clearCaretVisualLineHint();
        return;
    }
    caretVisualLineHint = lineIndex;
    caretVisualLineLayoutVersion = builtTextLayoutVersion;
}

float InputBox::xForDisplayPositionOnLine(int displayPos, int lineIndex) const {
    if(textLines.empty()) return 0.0f;
    lineIndex = std::max(0, std::min(lineIndex, (int)textLines.size() - 1));
    const TextLineLayout& line = textLines[lineIndex];
    const int position = std::max(line.start, std::min(displayPos, line.end));
    if(position <= line.start) return 0.0f;
    float x = 0.0f, h = 0.0f;
    const std::wstring prefix = copyDisplayRange(line.start, position - line.start);
    measuretext(prefix.c_str(), &x, &h, btnLayer);
    return x;
}

float InputBox::xForDisplayPosition(int displayPos) const {
    return xForDisplayPositionOnLine(displayPos, lineIndexForDisplayPosition(displayPos));
}

int InputBox::charPositionFromLocal(float localX, float localY, int* visualLine) const {
    if(textLines.empty()) return 0;
    const double contentY = localY - layoutPaddingY + (multiline ? scroll_offset_y : 0.0);
    int lineIndex = (int)std::floor(contentY / std::max(1.0, layoutLineHeight));
    lineIndex = std::max(0, std::min(lineIndex, (int)textLines.size() - 1));
    if(visualLine) *visualLine = lineIndex;
    const TextLineLayout& line = textLines[lineIndex];
    float targetX = localX - (float)layoutPaddingX + (multiline ? 0.0f : scroll_offset);
    targetX = std::max(0.0f, targetX);

    int bestDisplayPos = line.start;
    float bestDistance = std::numeric_limits<float>::max();
    for(int pos = line.start; pos <= line.end; ++pos) {
        const float charX = xForDisplayPositionOnLine(pos, lineIndex);
        const float distance = std::fabs(charX - targetX);
        if(distance < bestDistance) {
            bestDistance = distance;
            bestDisplayPos = pos;
        }
    }
    return contentPositionForDisplayPosition(bestDisplayPos);
}

void InputBox::ensureCursorVisible() {
    if(!multiline) {
        adjustScrollForCursor();
        cursorVisibilityDirty = false;
        return;
    }

    ensureTextLayout();

    int caretDisplayPos = displayPositionForContentPosition(cursor_pos);
    if(!IMECompositionString.empty()) {
        const int compositionStart = std::max(0, std::min(cursor_pos, contentLength()));
        caretDisplayPos = compositionStart + std::max(0, std::min(IMECursorPos, (int)IMECompositionString.size()));
    }
    if(!textLayoutComplete && caretDisplayPos > textLayoutBuildCursor) {
        // Do not expose progressive layout as a scrolling animation. Keep the
        // viewport stable until the caret's exact visual line is available,
        // then perform the normal single visibility adjustment below.
        cursorVisibilityDirty = true;
        return;
    }
    const int lineIndex = lineIndexForCaretDisplayPosition(caretDisplayPos);
    const double caretTop = lineIndex * layoutLineHeight;
    const double viewportHeight = std::max(1.0, height - 2.0 * layoutPaddingY);
    if(caretTop - scroll_offset_y < 0.0) scroll_offset_y = caretTop;
    if(caretTop + layoutLineHeight - scroll_offset_y > viewportHeight) {
        scroll_offset_y = caretTop + layoutLineHeight - viewportHeight;
    }
    const double maxScroll = std::max(0.0, textLines.size() * layoutLineHeight - viewportHeight);
    scroll_offset_y = std::max(0.0, std::min(scroll_offset_y, maxScroll));
    cursorVisibilityDirty = false;
}

void InputBox::scrollBy(double pixels) {
    if(!multiline) return;
    ensureTextLayout();
    const double viewportHeight = std::max(1.0, height - 2.0 * layoutPaddingY);
    const double maxScroll = std::max(0.0, textLines.size() * layoutLineHeight - viewportHeight);
    scroll_offset_y = std::max(0.0, std::min(scroll_offset_y + pixels, maxScroll));
    cursorVisibilityDirty = false;
    needRedraw = true;
    if(Panel* p = dynamic_cast<Panel*>(parent)) p->setDirty();
}

void InputBox::flushPendingNativeEditState() {
    // Keyboard messages can arrive in a short burst before the posted sync is
    // dispatched.  Consume that state before a visual Up/Down calculation so
    // the layout and the hidden EDIT always start from the same selection.
    if(nativeSyncMessagePosted || nativeTextSyncPending || nativeSelectionSyncPending) {
        syncNativeEditState();
    }
}

void InputBox::selectAllFromNativeEdit() {
    if(!inv.m_hwnd) return;

    cancelDrag();
    const int length = contentLength();
    dragBegin = 0;
    dragEnd = length;
    cursor_pos = length;
    verticalNavigationXValid = false;
    clearCaretVisualLineHint();
    invalidatePendingVerticalNavigation();
    cursorVisibilityDirty = true;
    inv.movecursor(0, length);
    reflushCursorTick();
    needRedraw = true;
    if(Panel* p = dynamic_cast<Panel*>(parent)) p->setDirty();
}

bool InputBox::tryMoveCursorVertically(int direction, bool extendSelection) {
    if(textLines.empty()) return true;

    const int length = contentLength();
    const int selectionStart = std::max(0, std::min(std::min(dragBegin, dragEnd), length));
    const int selectionEnd = std::max(0, std::min(std::max(dragBegin, dragEnd), length));

    // Match EDIT's non-Shift behavior first: an existing range collapses to
    // its visual leading/trailing end.  Shift keeps the native anchor.
    int anchor = std::max(0, std::min(dragBegin, length));
    int caret = std::max(0, std::min(cursor_pos, length));
    if(!extendSelection && selectionStart != selectionEnd) {
        caret = direction < 0 ? std::min(selectionStart, selectionEnd)
                              : std::max(selectionStart, selectionEnd);
        anchor = caret;
        clearCaretVisualLineHint();
    }
    else if(extendSelection) {
        caret = std::max(0, std::min(dragEnd, length));
    }

    const int displayCaret = displayPositionForContentPosition(caret);
    const bool haveLineHint = caretVisualLineHintIsValid(displayCaret);
    // At the progressive build cursor the same logical position can also be
    // the start of the next, not-yet-built visual line. Do not guess.
    if(!textLayoutComplete && !haveLineHint && displayCaret >= textLayoutBuildCursor) {
        return false;
    }
    int sourceLine = lineIndexForCaretDisplayPosition(displayCaret);
    sourceLine = std::max(0, std::min(sourceLine, (int)textLines.size() - 1));
    const int targetLine = sourceLine + direction;
    if(targetLine >= (int)textLines.size() && !textLayoutComplete) return false;
    if(targetLine < 0 || targetLine >= (int)textLines.size()) {
        // Even at the document boundary, collapse a native selection in the
        // same way the visible editor does.  This prevents later WM_CHAR from
        // replacing an unexpected hidden selection.
        dragBegin = anchor;
        dragEnd = caret;
        cursor_pos = caret;
        inv.movecursor(anchor, caret);
        cursorVisibilityDirty = true;
        ensureCursorVisible();
        needRedraw = true;
        if(Panel* p = dynamic_cast<Panel*>(parent)) p->setDirty();
        return true;
    }

    if(!verticalNavigationXValid) {
        verticalNavigationX = xForDisplayPositionOnLine(displayCaret, sourceLine);
        verticalNavigationXValid = true;
    }

    const TextLineLayout& destination = textLines[targetLine];
    int bestDisplayPosition = destination.start;
    float bestDistance = std::numeric_limits<float>::max();
    for(int position = destination.start; position <= destination.end; ++position) {
        const float distance = std::fabs(
            xForDisplayPositionOnLine(position, targetLine) - verticalNavigationX);
        if(distance < bestDistance) {
            bestDistance = distance;
            bestDisplayPosition = position;
        }
    }

    const int newCaret = contentPositionForDisplayPosition(bestDisplayPosition);
    if(!extendSelection) anchor = newCaret;
    dragBegin = anchor;
    dragEnd = newCaret;
    cursor_pos = newCaret;
    setCaretVisualLineHint(targetLine, bestDisplayPosition);
    cursorVisibilityDirty = true;
    inv.movecursor(anchor, newCaret);
    ensureCursorVisible();
    reflushCursorTick();
    needRedraw = true;
    if(Panel* p = dynamic_cast<Panel*>(parent)) p->setDirty();
    return true;
}

void InputBox::processPendingVerticalNavigation() {
    std::vector<PendingVerticalNavigation> moves;
    {
        std::lock_guard<std::mutex> lock(pendingVerticalNavigationMutex);
        moves.swap(pendingVerticalNavigation);
    }

    constexpr size_t maximumMovesPerFrame = 32;
    size_t processed = 0;
    while(processed < moves.size() && processed < maximumMovesPerFrame) {
        const PendingVerticalNavigation move = moves[processed];
        if(move.generation != verticalNavigationGeneration.load(std::memory_order_acquire)) {
            ++processed;
            continue;
        }

        if(lastProcessedVerticalNavigationSequence.load(std::memory_order_acquire) == 0) {
            const int length = contentLength();
            const int selectionStart = std::max(0, std::min(move.selectionStart, length));
            const int selectionEnd = std::max(selectionStart, std::min(move.selectionEnd, length));
            if(selectionStart == selectionEnd) {
                dragBegin = dragEnd = cursor_pos = selectionStart;
            }
            else if(std::min(dragBegin, dragEnd) != selectionStart ||
                    std::max(dragBegin, dragEnd) != selectionEnd) {
                dragBegin = selectionStart;
                dragEnd = cursor_pos = selectionEnd;
                clearCaretVisualLineHint();
            }
        }

        if(!tryMoveCursorVertically(move.direction, move.extendSelection)) break;
        lastProcessedVerticalNavigationSequence.store(move.sequence, std::memory_order_release);
        ++processed;
    }

    if(processed < moves.size()) {
        std::lock_guard<std::mutex> lock(pendingVerticalNavigationMutex);
        pendingVerticalNavigation.insert(pendingVerticalNavigation.begin(),
            moves.begin() + processed, moves.end());
    }
    if(hasPendingVerticalNavigation()) needRedraw = true;
}

void InputBox::invalidatePendingVerticalNavigation() {
    std::lock_guard<std::mutex> lock(pendingVerticalNavigationMutex);
    pendingVerticalNavigation.clear();
    verticalNavigationGeneration.fetch_add(1, std::memory_order_acq_rel);
    nextVerticalNavigationSequence = 1;
    lastProcessedVerticalNavigationSequence.store(0, std::memory_order_release);
}

bool InputBox::hasPendingVerticalNavigation() const {
    std::lock_guard<std::mutex> lock(pendingVerticalNavigationMutex);
    return !pendingVerticalNavigation.empty();
}

bool InputBox::enqueueVerticalNavigationFromNativeEdit(int direction, bool extendSelection) {
    if(!multiline || !inv.m_hwnd || (direction != -1 && direction != 1)) return false;

    DWORD rawStart = 0;
    DWORD rawEnd = 0;
    ::SendMessageW(inv.m_hwnd, EM_GETSEL, (WPARAM)&rawStart, (LPARAM)&rawEnd);
    {
        std::lock_guard<std::mutex> lock(pendingVerticalNavigationMutex);
        pendingVerticalNavigation.push_back({
            direction,
            extendSelection,
            (int)std::min<DWORD>(rawStart, INT_MAX),
            (int)std::min<DWORD>(rawEnd, INT_MAX),
            verticalNavigationGeneration.load(std::memory_order_acquire),
            nextVerticalNavigationSequence++
        });
    }
    return true;
}

void InputBox::updateDragAutoScroll(int mouseX, int mouseY) {
    if(!multiline || !dragging) return;
    dragVerticalSide = mouseY < (int)top ? -1 : (mouseY > (int)(top + height) ? 1 : 0);
    if(dragVerticalSide != 0) {
        scrollBy(dragVerticalSide * layoutLineHeight * 0.75);
        lastDragTick = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
}

void InputBox::noteNativeTextChange(int selectionStart, int selectionEnd, bool exactChange) {
    if(writingNativeEdit) return;
    if(!exactChange) {
        nativeTextChangeHintPending = false;
        nativeTextPatches.clear();
        return;
    }

    // A posted sync may coalesce the WM_KEYDOWN and WM_CHAR parts of one
    // edit.  Retain the first pre-edit selection; it describes the complete
    // replacement range once the native EDIT reports its final caret.
    if(nativeTextChangeHintPending) return;
    const int length = contentLength();
    nativeTextChangeOldStart = std::max(0, std::min(selectionStart, length));
    nativeTextChangeOldEnd = std::max(nativeTextChangeOldStart,
        std::min(selectionEnd, length));
    nativeTextChangeHintPending = true;
}

void InputBox::noteNativeTextReplacement(int selectionStart, int selectionEnd,
                                         const std::wstring& replacement) {
    if(writingNativeEdit) return;
    noteNativeTextChange(selectionStart, selectionEnd, true);
    const int length = contentLength();
    selectionStart = std::max(0, std::min(selectionStart, length));
    selectionEnd = std::max(selectionStart, std::min(selectionEnd, length));
    nativeTextPatches.push_back({selectionStart, selectionEnd, replacement});
}

void InputBox::noteNativeDelete(int selectionStart, int selectionEnd, bool backspace) {
    const int length = contentLength();
    selectionStart = std::max(0, std::min(selectionStart, length));
    selectionEnd = std::max(selectionStart, std::min(selectionEnd, length));
    if(selectionStart == selectionEnd) {
        if(backspace && selectionStart > 0) --selectionStart;
        else if(!backspace && selectionEnd < length) ++selectionEnd;
    }
    noteNativeTextReplacement(selectionStart, selectionEnd, L"");
}

void InputBox::noteNativeCharacter(int selectionStart, int selectionEnd, wchar_t character) {
    if(character == L'\b') {
        noteNativeDelete(selectionStart, selectionEnd, true);
        return;
    }
    if(character == L'\r' && multiline) {
        noteNativeTextReplacement(selectionStart, selectionEnd, L"\r\n");
        return;
    }
    if(character >= L' ' && character != 0x7f) {
        noteNativeTextReplacement(selectionStart, selectionEnd, std::wstring(1, character));
        return;
    }
    // Control characters outside the normal text/edit commands are filtered
    // differently by EDIT styles and host accelerators.  Do not let an older
    // known patch mask such a change; the conservative native read is cheap
    // compared with accepting an ambiguous model update.
    noteNativeTextChange(selectionStart, selectionEnd, false);
}

void InputBox::requestNativeEditSync(bool textMayHaveChanged) {
    if(writingNativeEdit) return;

    nativeTextSyncPending = nativeTextSyncPending || textMayHaveChanged;
    nativeSelectionSyncPending = true;
    if(!nativeSyncMessagePosted && inv.m_hwnd) {
        nativeSyncMessagePosted = ::PostMessageW(inv.m_hwnd, WM_USER + 100 + 1, 0, 0) != FALSE;
    }
}

void InputBox::applyNativeTextChange(const std::wstring& nativeText, int oldStart,
                                      int oldEnd, int newChangedEnd, bool exactChange) {
    const std::wstring& oldText = materializedContent();
    if(exactChange) {
        invalidateTextLayoutForContentChange(oldText, nativeText, oldStart, oldEnd,
            oldStart, newChangedEnd);
    }
    else {
        invalidateTextLayout();
    }
    assignContentModel(nativeText);
    const int length = contentLength();
    cursor_pos = std::max(0, std::min(cursor_pos, length));
    dragBegin = std::max(0, std::min(dragBegin, length));
    dragEnd = std::max(0, std::min(dragEnd, length));
    verticalNavigationXValid = false;
    clearCaretVisualLineHint();
    invalidatePendingVerticalNavigation();
    cursorVisibilityDirty = true;
}

bool InputBox::canApplyNativeTextPatches(int selectionStart, int selectionEnd) const {
    if(nativeTextPatches.empty()) return false;

    int expectedLength = contentLength();
    int expectedCaret = -1;
    for(const NativeTextPatch& patch : nativeTextPatches) {
        if(patch.start < 0 || patch.end < patch.start || patch.end > expectedLength) return false;
        expectedLength += (int)patch.replacement.size() - (patch.end - patch.start);
        if(expectedLength < 0) return false;
        expectedCaret = patch.start + (int)patch.replacement.size();
    }
    return expectedCaret >= 0 && selectionStart == expectedCaret && selectionEnd == expectedCaret;
}

void InputBox::applyNativeTextPatches() {
    if(nativeTextPatches.empty()) return;

    bool hasVisibleChange = false;
    for(const NativeTextPatch& patch : nativeTextPatches) {
        if(patch.start != patch.end || !patch.replacement.empty()) {
            hasVisibleChange = true;
            break;
        }
    }
    if(!hasVisibleChange) return;

    if(nativeTextPatches.size() == 1) {
        const NativeTextPatch& patch = nativeTextPatches.front();
        invalidateTextLayoutForKnownChange(patch.start, patch.end, patch.replacement, true);
        replaceContentRange(patch.start, patch.end, patch.replacement);
    }
    else {
        int earliestChange = nativeTextPatches.front().start;
        for(const NativeTextPatch& patch : nativeTextPatches) {
            earliestChange = std::min(earliestChange, patch.start);
        }
        // Several queued key messages are still local edits, but their
        // selections are expressed in successive native states.  Rebuild from
        // the first affected visual line once, rather than invalidating and
        // shifting the cached suffix for every queued character.
        invalidateTextLayoutForKnownChange(earliestChange, earliestChange, L"", false);
        for(const NativeTextPatch& patch : nativeTextPatches) {
            replaceContentRange(patch.start, patch.end, patch.replacement);
        }
    }
    cursorVisibilityDirty = true;
    verticalNavigationXValid = false;
    clearCaretVisualLineHint();
    invalidatePendingVerticalNavigation();
}

void InputBox::syncNativeEditState() {
    nativeSyncMessagePosted = false;
    const bool readText = nativeTextSyncPending;
    const bool readSelection = nativeSelectionSyncPending;
    const bool hasExactChangeHint = nativeTextChangeHintPending;
    const int hintedOldStart = nativeTextChangeOldStart;
    const int hintedOldEnd = nativeTextChangeOldEnd;
    nativeTextSyncPending = false;
    nativeSelectionSyncPending = false;
    nativeTextChangeHintPending = false;
    if(!readText && !readSelection) return;

    if(dragging) cancelDrag();
    bool stateChanged = false;
    const int previousAnchor = dragBegin;
    const int previousCaret = dragEnd;

    DWORD rawSelectionStart = 0;
    DWORD rawSelectionEnd = 0;
    const bool shouldReadSelection = readSelection || (readText && hasExactChangeHint);
    if(shouldReadSelection) {
        ::SendMessageW(inv.m_hwnd, EM_GETSEL, (WPARAM)&rawSelectionStart,
            (LPARAM)&rawSelectionEnd);
    }

    if(readText) {
        const int nativeSelectionStart = (int)std::min<DWORD>(rawSelectionStart, INT_MAX);
        const int nativeSelectionEnd = (int)std::min<DWORD>(rawSelectionEnd, INT_MAX);
        if(canApplyNativeTextPatches(nativeSelectionStart, nativeSelectionEnd)) {
            bool patchChangesText = false;
            for(const NativeTextPatch& patch : nativeTextPatches) {
                if(patch.start != patch.end || !patch.replacement.empty()) {
                    patchChangesText = true;
                    break;
                }
            }
            applyNativeTextPatches();
            stateChanged = stateChanged || patchChangesText;
        }
        else {
            const int textLength = inv.gettextlength();
            if(textLength >= 0) {
                const int newSelectionStart = std::max(0,
                    std::min(nativeSelectionStart, textLength));
                const int newSelectionEnd = std::max(0,
                    std::min(nativeSelectionEnd, textLength));
                // Complex commands, external WM_SETTEXT, clipboard paths and
                // IME results deliberately retain native EDIT as the source of
                // truth.  This is the only path that copies the full document.
                std::vector<wchar_t> nativeBuffer(textLength + 1, L'\0');
                inv.gettext((int)nativeBuffer.size(), nativeBuffer.data());
                std::wstring nativeText(nativeBuffer.data(), textLength);
                const std::wstring& oldText = materializedContent();
                const int oldLength = (int)oldText.size();
                const int oldSelectionLength = std::max(0, hintedOldEnd - hintedOldStart);
                const long long expectedNewChangedEnd64 = (long long)hintedOldStart +
                    ((long long)textLength - oldLength + oldSelectionLength);
                const int expectedNewChangedEnd = expectedNewChangedEnd64 >= 0 &&
                    expectedNewChangedEnd64 <= textLength ? (int)expectedNewChangedEnd64 : -1;
                bool exactChange = hasExactChangeHint && shouldReadSelection &&
                    hintedOldStart >= 0 && hintedOldEnd >= hintedOldStart &&
                    hintedOldEnd <= oldLength && expectedNewChangedEnd >= hintedOldStart &&
                    expectedNewChangedEnd <= textLength &&
                    newSelectionStart == expectedNewChangedEnd &&
                    newSelectionEnd == expectedNewChangedEnd;

                if(exactChange) {
                    // Validate a small unchanged window on both sides.  This
                    // catches a coalesced unrelated command without reintroducing
                    // the old whole-document prefix/suffix scan.
                    constexpr int boundaryValidationCharacters = 16;
                    const int prefixLength = std::min(boundaryValidationCharacters,
                        hintedOldStart);
                    const int suffixLength = std::min({boundaryValidationCharacters,
                        oldLength - hintedOldEnd, textLength - expectedNewChangedEnd});
                    const bool prefixMatches = std::equal(oldText.begin() + hintedOldStart - prefixLength,
                        oldText.begin() + hintedOldStart,
                        nativeText.begin() + hintedOldStart - prefixLength);
                    const bool suffixMatches = std::equal(oldText.begin() + hintedOldEnd,
                        oldText.begin() + hintedOldEnd + suffixLength,
                        nativeText.begin() + expectedNewChangedEnd);
                    exactChange = prefixMatches && suffixMatches;
                }

                bool textChanged = textLength != oldLength;
                if(!textChanged && exactChange) {
                    const int replacementLength = expectedNewChangedEnd - hintedOldStart;
                    textChanged = replacementLength != oldSelectionLength ||
                        !std::equal(oldText.begin() + hintedOldStart,
                            oldText.begin() + hintedOldEnd,
                            nativeText.begin() + hintedOldStart,
                            nativeText.begin() + expectedNewChangedEnd);
                }
                if(!textChanged && !exactChange) {
                    textChanged = oldText != nativeText;
                }
                if(textChanged) {
                    applyNativeTextChange(nativeText, hintedOldStart, hintedOldEnd,
                        expectedNewChangedEnd, exactChange);
                    stateChanged = true;
                }
            }
        }
        ::SendMessageW(inv.m_hwnd, EM_SETMODIFY, FALSE, 0);
    }
    nativeTextPatches.clear();

    if(shouldReadSelection) {
        const int newStart = std::max(0,
            std::min((int)rawSelectionStart, contentLength()));
        const int newEnd = std::max(0,
            std::min((int)rawSelectionEnd, contentLength()));
        int newAnchor = newStart;
        int newCaret = newEnd;
        if(newStart == newEnd) {
            newAnchor = newCaret = newStart;
        }
        else if(previousAnchor == newStart) {
            newAnchor = newStart;
            newCaret = newEnd;
        }
        else if(previousAnchor == newEnd) {
            newAnchor = newEnd;
            newCaret = newStart;
        }
        else if(previousCaret == newStart) {
            newAnchor = newEnd;
            newCaret = newStart;
        }
        else if(previousCaret == newEnd) {
            newAnchor = newStart;
            newCaret = newEnd;
        }
        if(dragBegin != newAnchor || dragEnd != newCaret || cursor_pos != newCaret) {
            dragBegin = newAnchor;
            dragEnd = newCaret;
            if(cursor_pos != newCaret) moveCursor(newCaret);
            else {
                clearCaretVisualLineHint();
                invalidatePendingVerticalNavigation();
            }
            verticalNavigationXValid = false;
            stateChanged = true;
        }
    }

    if(stateChanged) {
        needRedraw = true;
        reflushCursorTick();
        if(Panel* p = dynamic_cast<Panel*>(parent)) p->setDirty();
    }
}

void InputBox::writeNativeTextFromModel() {
    if(!inv.m_hwnd) return;

    writingNativeEdit = true;
    inv.settext(materializedContent().c_str());
    writingNativeEdit = false;
    nativeTextSyncPending = false;
    nativeSelectionSyncPending = false;
    nativeTextChangeHintPending = false;
    nativeTextPatches.clear();
    ::SendMessageW(inv.m_hwnd, EM_SETMODIFY, FALSE, 0);
}

void InputBox::draw(PIMAGE dst, double x, double y) {
    double left = x - width / 2 - 4;
    double top = y - height / 2 - 4;
    double width = this->width + 8;
    double height = this->height + 8;
    
    if(!on_focus && !ripples.size() && !needRedraw && !scaleChanged && !PanelScaleChanged &&
       !hasPendingVerticalNavigation()){
        if(!BackendFlag) {
            putimage_withalpha(dst, btnLayer, left, top);
        }
        return;
    }

    ege_setfont((float)std::max(1.0, text_height * scale), L"宋体", btnLayer);
    if(on_focus) {
        inv.setfocus();
        if(!multiline && cursorVisibilityDirty) ensureCursorVisible();
    }
        
    // 使用真正的透明色(PRGB32模式下alpha=0时RGB也应为0)
    setbkcolor_f(EGEARGB(0, 0, 0, 0), btnLayer);
    cleardevice(btnLayer);

    ege_setclippath(&clippath,btnLayer);

    // 优化：只绘制一次背景到btnLayer，稍后复制到bgLayer
    setfillcolor(EGEACOLOR(255,color), btnLayer);
    ege_fillrect(0, 0, width, height, btnLayer);

    // 更新并绘制 ripples
    for (auto& r : ripples) {
        r.update();
    }
    // 优化：使用C++20 std::erase_if替代erase-remove惯用法
    std::erase_if(ripples, [](const Ripple& r) { return !r.alive(); });

    if(multiline) {
        if(dragging && dragVerticalSide != 0 && lastDragMouseX >= 0 && lastDragMouseY >= 0) {
            const double now = std::chrono::duration<double>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            if(now - lastDragTick >= DRAG_ADVANCE_INTERVAL) {
                scrollBy(dragVerticalSide * layoutLineHeight * 0.75);
                lastDragTick = now;
                int dragLine = -1;
                const int dragPos = charPositionFromLocal(
                    (float)(lastDragMouseX - (int)this->left),
                    (float)(lastDragMouseY - (int)this->top), &dragLine);
                dragEnd = dragPos;
                if(cursor_pos != dragPos) moveCursor(dragPos);
                setCaretVisualLineHint(dragLine, displayPositionForContentPosition(dragPos));
                inv.movecursor(dragBegin, dragEnd);
            }
        }
        ensureTextLayout();
        processPendingVerticalNavigation();
        if(on_focus && cursorVisibilityDirty) ensureCursorVisible();
        const auto [firstVisibleLine, lastVisibleLine] = visibleTextLineRange();

        setbkmode(TRANSPARENT, btnLayer);
        settextcolor(BLACK, btnLayer);
        if(on_focus) {
            setfillcolor(EGEARGB(50, 30, 30, 30), btnLayer);
            ege_fillrect(0, 0, width, height, btnLayer);
        }

        float unusedWidth = 0.0f, textRealHeight = 0.0f;
        measuretext(L"a", &unusedWidth, &textRealHeight, btnLayer);
        HDC contentDC = getHDC(btnLayer);
        int savedDC = SaveDC(contentDC);
        IntersectClipRect(contentDC,
            (int)layoutPaddingX, (int)layoutPaddingY,
            (int)std::max(layoutPaddingX + 1, this->width - layoutPaddingX),
            (int)std::max(layoutPaddingY + 1, this->height - layoutPaddingY));

        if(on_focus && dragBegin != dragEnd) {
            const int selStart = std::max(0, std::min(std::min(dragBegin, dragEnd), contentLength()));
            const int selEnd = std::max(0, std::min(std::max(dragBegin, dragEnd), contentLength()));
            const int displayStart = displayPositionForContentPosition(selStart);
            const int displayEnd = displayPositionForContentPosition(selEnd);
            setfillcolor(EGEARGB(180, 0, 120, 215), btnLayer);
            for(int lineIndex = firstVisibleLine; lineIndex < lastVisibleLine; ++lineIndex) {
                const TextLineLayout& line = textLines[lineIndex];
                const int begin = std::max(displayStart, line.start);
                const int end = std::min(displayEnd, line.end);
                if(begin >= end) continue;
                const float x1 = (float)(layoutPaddingX + xForDisplayPositionOnLine(begin, lineIndex));
                const float x2 = (float)(layoutPaddingX + xForDisplayPositionOnLine(end, lineIndex));
                const double lineY = layoutPaddingY + lineIndex * layoutLineHeight - scroll_offset_y;
                ege_fillrect(x1, lineY, std::max(2.0f, x2 - x1), layoutLineHeight, btnLayer);
            }
        }

        for(int lineIndex = firstVisibleLine; lineIndex < lastVisibleLine; ++lineIndex) {
            const TextLineLayout& line = textLines[lineIndex];
            const double lineY = layoutPaddingY + lineIndex * layoutLineHeight - scroll_offset_y;
            const std::wstring lineText = copyDisplayRange(line.start, line.end - line.start);
            ege_outtextxy((float)layoutPaddingX, (float)lineY, lineText.c_str(), btnLayer);
        }

        if(on_focus) {
            int caretDisplayPos = displayPositionForContentPosition(cursor_pos);
            if(!IMECompositionString.empty()) {
                const int compositionStart = std::max(0, std::min(cursor_pos, contentLength()));
                caretDisplayPos = compositionStart + std::max(0, std::min(IMECursorPos, (int)IMECompositionString.size()));
            }
            const int caretLine = lineIndexForCaretDisplayPosition(caretDisplayPos);
            const double caretX = layoutPaddingX +
                xForDisplayPositionOnLine(caretDisplayPos, caretLine);
            const double caretY = layoutPaddingY + caretLine * layoutLineHeight - scroll_offset_y;
            const std::chrono::duration<double> elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
            const double cursor_opacity = InputBoxSinDoubleForCursor(elapsed_time.count());
            setfillcolor(EGEARGB((char)(cursor_opacity * 255), 255, 255, 0), btnLayer);
            ege_fillrect(caretX, caretY - 2, 2, textRealHeight + 7, btnLayer);

            if(!IMECompositionString.empty()) {
                const int imeStart = displayPositionForContentPosition(cursor_pos);
                const int imeEnd = imeStart + (int)IMECompositionString.size();
                setlinestyle(DOTTED_LINE, 0U, 1, btnLayer);
                setlinecolor(EGEARGB(255, 0, 0, 0), btnLayer);
                for(int lineIndex = firstVisibleLine; lineIndex < lastVisibleLine; ++lineIndex) {
                    const TextLineLayout& line = textLines[lineIndex];
                    const int begin = std::max(imeStart, line.start);
                    const int end = std::min(imeEnd, line.end);
                    if(begin >= end) continue;
                    const double yLine = layoutPaddingY + lineIndex * layoutLineHeight - scroll_offset_y + textRealHeight + 2;
                    ege_line((float)(layoutPaddingX + xForDisplayPositionOnLine(begin, lineIndex)), (float)yLine,
                             (float)(layoutPaddingX + xForDisplayPositionOnLine(end, lineIndex)), (float)yLine, btnLayer);
                }
                setlinestyle(SOLID_LINE, 0U, 1, btnLayer);
            }

            // 与单行路径一致：只更新缓存坐标，IME 定位仍由既有逻辑调用。
            m_ime_pos_x = left + caretX + absolutPosDeltaX;
            m_ime_pos_y = top + caretY + textRealHeight + 2 + absolutPosDeltaY;
        }

        RestoreDC(contentDC, savedDC);
        for(auto& r : ripples) r.draw(btnLayer);
        ege_resetclippath(btnLayer);
        setlinewidth(1, btnLayer);
        setlinecolor(EGEACOLOR(255, color), btnLayer);
        ege_drawpath(&clippath, btnLayer);
        if(!BackendFlag) putimage_withalpha(dst, btnLayer, left, top);
        needRedraw = !textLayoutComplete;
        if(needRedraw) {
            if(Panel* p = dynamic_cast<Panel*>(parent)) p->setDirty();
        }
        scaleChanged = false;
        return;
    }

    const std::wstring& modelContent = materializedContent();
    std::wstring displayContentWithIme;
    if(!IMECompositionString.empty()) {
        const int compositionStart = std::max(0, std::min(cursor_pos, (int)modelContent.size()));
        displayContentWithIme = modelContent.substr(0, compositionStart) +
            IMECompositionString + modelContent.substr(compositionStart);
    }
    const std::wstring& displayContent = IMECompositionString.empty() ? modelContent : displayContentWithIme;

    // 优化：仅在缩放改变时设置字体
    double currentFontScale = scale * text_height;
    
    setbkmode(TRANSPARENT, btnLayer);
    settextcolor(BLACK, btnLayer);

    if(sgn(currentFontScale - 1) >= 0 || scaleChanged || PanelScaleChanged){
        const float padding = 14 * scale;
        
        float cursor_pos_width, cursor_with_ime_width, full_text_width, cursor_with_full_ime_width;
        const bool textMetricsDirty =
            lastMeasuredTextLayoutVersion != textLayoutVersion ||
            lastCursorPos != cursor_pos ||
            lastMeasuredIMECursorPos != IMECursorPos ||
            scaleChanged || PanelScaleChanged;
        if(textMetricsDirty) {
            const int compositionStart = std::max(0, std::min(cursor_pos, (int)modelContent.size()));
            std::wstring cursorBeforeCursor = modelContent.substr(0, compositionStart);
            cursorBeforeCursor += IMECompositionString.substr(0, IMECursorPos);
            std::wstring cursorBeforeText = modelContent.substr(0, compositionStart);
            cursorBeforeText += IMECompositionString;
            float tmp = 0.0f;

            measuretext(modelContent.substr(0, compositionStart).c_str(), &cursor_pos_width, &tmp, btnLayer);
            measuretext(cursorBeforeCursor.c_str(), &cursor_with_ime_width, &tmp, btnLayer);
            measuretext(cursorBeforeText.c_str(), &cursor_with_full_ime_width, &tmp, btnLayer);
            measuretext(displayContent.c_str(), &full_text_width, &tmp, btnLayer);

            cachedCursorPosWidth = cursor_pos_width;
            cachedCursorWithImeWidth = cursor_with_ime_width;
            cachedCursorWithFullImeWidth = cursor_with_full_ime_width;
            cachedFullTextWidth = full_text_width;
            lastCursorPos = cursor_pos;
            lastMeasuredIMECursorPos = IMECursorPos;
            lastMeasuredTextLayoutVersion = textLayoutVersion;
            reflushCursorTick();
        }
        else {
            cursor_pos_width = cachedCursorPosWidth;
            cursor_with_ime_width = cachedCursorWithImeWidth;
            cursor_with_full_ime_width = cachedCursorWithFullImeWidth;
            full_text_width = cachedFullTextWidth;
        }
        
        float _w,_h;
        measuretext("a",&_w,&_h,btnLayer);
        float textRealHeight = _h;
        float text_start_x = padding - scroll_offset;

        // 绘制选区高亮（文本下方，仅聚焦且有选区时）
        if(on_focus && dragBegin != dragEnd) {
            int sel_s = std::min(dragBegin, dragEnd);
            int sel_e = std::max(dragBegin, dragEnd);
            sel_s = std::max(0, std::min(sel_s, (int)modelContent.size()));
            sel_e = std::max(0, std::min(sel_e, (int)modelContent.size()));
            float sel_s_px, sel_e_px, sel_tmp;
            measuretext(modelContent.substr(0, sel_s).c_str(), &sel_s_px, &sel_tmp, btnLayer);
            measuretext(modelContent.substr(0, sel_e).c_str(), &sel_e_px, &sel_tmp, btnLayer);
            setfillcolor(EGEARGB(180, 0, 120, 215), btnLayer);
            ege_fillrect(text_start_x + sel_s_px,
                         height / 2 - textRealHeight / 2 - 3.5f,
                         sel_e_px - sel_s_px,
                         textRealHeight + 7,
                         btnLayer);
        }
        
        // 绘制文本
        ege_outtextxy(text_start_x, height / 2 - textRealHeight / 2, 
                    displayContent.c_str(), btnLayer);
        
        if(on_focus) {
            setfillcolor(EGEARGB(50,30,30,30), btnLayer);
            ege_fillrect(0, 0, width, height, btnLayer);

            // 绘制光标
            std::chrono::_V2::system_clock::time_point current_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_time = current_time - start_time;
            double cursor_opacity = InputBoxSinDoubleForCursor(elapsed_time.count());
            setfillcolor(EGEARGB((char)(cursor_opacity * 255),255,255,0), btnLayer);
            
            float cursor_draw_x = text_start_x + cursor_with_ime_width;
            ege_fillrect(cursor_draw_x, height / 2 - textRealHeight / 2 - 3.5, 
                    2, textRealHeight + 7, btnLayer);
            
            // IME输入下划线
            if(IMECompositionString.size()){
                setlinestyle(DOTTED_LINE, 0U, 1, btnLayer);
                setlinecolor(EGEARGB(255,0,0,0), btnLayer);
                
                float ime_start_x = text_start_x + cursor_pos_width;
                float ime_end_x = text_start_x + cursor_with_full_ime_width;
                
                ege_line(ime_start_x, height / 2 + textRealHeight / 2 + 2,
                    ime_end_x, height / 2 + textRealHeight / 2 + 2, btnLayer);
                setlinestyle(SOLID_LINE, 0U, 1, btnLayer);
            }
            
            // 更新IME位置
            InputPositionX = m_ime_pos_x = left + cursor_draw_x + absolutPosDeltaX;
            InputPositionY = m_ime_pos_y = top + height / 2 + textRealHeight / 2 + 2 + absolutPosDeltaY;
            SetIMEPosition(getHWnd(), InputPositionX, InputPositionY);
        }
    }
    
    for (auto& r : ripples) {
        r.draw(btnLayer);
    }

    ege_resetclippath(btnLayer);
    setlinewidth(1,btnLayer);
    setlinecolor(EGEACOLOR(255,color), btnLayer);
    ege_drawpath(&clippath,btnLayer);

    if(!BackendFlag) {
        putimage_withalpha(dst,btnLayer,left,top);
    }
    needRedraw = false;
    scaleChanged = false;
}

void InputBox::draw(){
    draw(nullptr,cx,cy);
}

void InputBox::deleteFocus(const mouse_msg& msg){
    on_focus = false;
    dragging = false;
    dragSide = 0;
    dragVerticalSide = 0;
    lastDragMouseX = -1;
    lastDragMouseY = -1;
    lastDragTick = 0.0;
    dragBegin = 0;
    dragEnd = 0;
    verticalNavigationXValid = false;
    clearCaretVisualLineHint();
    invalidatePendingVerticalNavigation();
    inv.killfocus();
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
            p->setAlwaysDirty(false);
        }
    }
    this->setDrawing(false);
    if(mouseOwningFlag == this) mouseOwningFlag = nullptr;
    if(focusingWidget == this) focusingWidget = nullptr;
}

// todo: 输入法位置会在 Win11 的输入的第一帧无法做到对齐
void InputBox::updateIMEPosition() {
    InputPositionX = m_ime_pos_x;
    InputPositionY = m_ime_pos_y;
    SetIMEPosition(getHWnd(), InputPositionX, InputPositionY);
}

bool InputBox::haveIMEString() const {
    return !IMECompositionString.empty();
}

bool InputBox::handleEvent(const mouse_msg& msg) {
    const bool inside = isInside(msg.x, msg.y);

    // 鼠标移入移出处理
    if(disabled) {
        if(inside) {
            setCursor(IDC_NO);
            lastInside = true;
            return true;
        }
        else if(lastInside) {
            setCursor(IDC_ARROW);
            lastInside = false;
            return false;
        }
    }

    if(inside) {
        setCursor(IDC_IBEAM);
        lastInside = true;
    }
    else if(lastInside){
        if(lastInside) {
            setCursor(IDC_ARROW);
            needReflushCursor = true;
            lastInside = false;
        }
    }

    if(disabled) return false;

    // 处理其它控件焦点
    if(msg.is_left() && msg.is_up()){
        if(mouseOwningFlag != nullptr && mouseOwningFlag != this){
            mouseOwningFlag->releaseMouseOwningFlag(msg);
        }
        // 鼠标抬起时结束拖动选择
        if(dragging) {
            dragging = false;
            dragSide = 0;
            dragVerticalSide = 0;
            lastDragMouseX = -1;
            lastDragMouseY = -1;
            lastDragTick = 0.0;
            mouseOwningFlag = nullptr;
        }
    }

    // 鼠标左键按下且在输入框内部
    if(msg.is_left() && msg.is_down() && inside) {
        // 无论当前 IME 叠加串是否已同步到 IMECompositionString，
        // 鼠标按下都先同步结束 IME 组合，再立即进入勾选流程，
        // 避免“有候选时第一下无法开始拖选”的状态竞态。
        ::SendMessageW(inv.m_hwnd, WM_USER + 100 + 2, 0, 0);

        int localX = msg.x - left;
        int localY = msg.y - top;

        if(!on_focus) {
            m_counter ++;
            ripples.emplace_back(localX, localY, 4.00f / 3.00f * std::sqrt(height * height + width * width), 70, dynamic_cast<Widget*>(this),m_counter);
            on_focus = true;
            needRedraw = true;
            if(this->parent != nullptr){
                if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                    p->setAlwaysDirty(true);
                }
            }
            this->setDrawing(true);
            inv.setfocus();
            reflushCursorTick();
        }

        // 多行命中同时依据 X/Y；单行继续沿用原有横向命中逻辑。
        if(multiline) ensureTextLayout();
        int clickedVisualLine = -1;
        int best_pos = multiline
            ? charPositionFromLocal((float)localX, (float)localY, &clickedVisualLine)
            : charPositionFromLocalX((float)localX);

        moveCursor(best_pos);
        if(multiline) {
            verticalNavigationXValid = false;
            setCaretVisualLineHint(clickedVisualLine, displayPositionForContentPosition(best_pos));
        }
        cursorVisibilityDirty = true;
        if(multiline) ensureCursorVisible();
        // 开始拖动选择，锚点与光标初始相同
        dragBegin = best_pos;
        dragEnd = best_pos;
        dragging = true;
        lastDragMouseX = msg.x; // 记录点击时的屏幕 X，防止后续合成 MOUSEMOVE 误触发
        lastDragMouseY = msg.y;
        lastDragTick = 0.0;
        inv.movecursor(dragBegin, dragEnd);
        needRedraw = true;
        if(this->parent != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                p->setDirty();
            }
        }
        if(focusingWidget != nullptr && focusingWidget != this){
            focusingWidget->deleteFocus(msg);
        }
        focusingWidget = this;
        mouseOwningFlag = this;
        updateIMEPosition();
        return true;
    }
    // 鼠标左键按下且不在输入框内
    else if(msg.is_left() && msg.is_down() && on_focus) {
        deleteFocus(msg);
    }

    // 鼠标移动时更新拖动选择范围
    if(msg.is_move() && dragging && on_focus) {
        applyDragMove(msg.x, msg.y);
        return true;
    }

    // 优先由多行 InputBox 消费内部滚轮；这样其内容滚动不会误传给外层 Panel/Box。
    if(multiline && msg.is_wheel() && inside) {
        ensureTextLayout();
        const double viewportHeight = std::max(1.0, height - 2.0 * layoutPaddingY);
        const double maxScroll = std::max(0.0, textLines.size() * layoutLineHeight - viewportHeight);
        if(maxScroll > 0.0) {
            scrollBy((double)(msg.wheel / -120.0) * layoutLineHeight * 3.0);
            return true;
        }
    }

    return false;
}

bool InputBox::isInside(double x, double y) const {
    // 转换为按钮内部坐标系
    double localX = x - left;
    double localY = y - top;

    // 先检查是否在按钮矩形框外
    if(localX < 0 || localX >= width || localY < 0 || localY >= height)
        return false;

    // 中心矩形区（不考虑圆角）直接返回 true
    if(localX >= radius && localX < width - radius)
        return true;
    if(localY >= radius && localY < height - radius)
        return true;

    // 检查四个圆角区域
    int dx, dy;
    // 左上角
    if(localX < radius && localY < radius) {
        dx = radius - localX;
        dy = radius - localY;
        return dx * dx + dy * dy <= radius * radius;
    }
    // 右上角
    if(localX >= width - radius && localY < radius) {
        dx = localX - (width - radius);
        dy = radius - localY;
        return dx * dx + dy * dy <= radius * radius;
    }
    // 左下角
    if(localX < radius && localY >= height - radius) {
        dx = radius - localX;
        dy = localY - (height - radius);
        return dx * dx + dy * dy <= radius * radius;
    }
    // 右下角
    if(localX >= width - radius && localY >= height - radius) {
        dx = localX - (width - radius);
        dy = localY - (height - radius);
        return dx * dx + dy * dy <= radius * radius;
    }

    // 其余区域为中间的十字形部分
    return true;
}

void InputBox::setContent(const std::wstring& s,bool flag) {
    std::wstring normalized;
    const std::wstring* normalizedContent = &s;
    if(!flag && multiline) {
        normalized = s;
        std::wstring converted;
        converted.reserve(normalized.size() + 8);
        for(size_t i = 0; i < normalized.size(); ++i) {
            if(normalized[i] == L'\r') {
                converted += L'\r';
                if(i + 1 >= normalized.size() || normalized[i + 1] != L'\n') converted += L'\n';
            }
            else if(normalized[i] == L'\n') {
                if(i == 0 || normalized[i - 1] != L'\r') converted += L'\r';
                converted += L'\n';
            }
            else {
                converted += normalized[i];
            }
        }
        normalized.swap(converted);
    }
    else if(!flag) {
        normalized = s;
        // ES_AUTOHSCROLL 的单行 EDIT 不接受换行；保持模型与原生后端一致。
        normalized.erase(std::remove(normalized.begin(), normalized.end(), L'\r'), normalized.end());
        normalized.erase(std::remove(normalized.begin(), normalized.end(), L'\n'), normalized.end());
    }
    if(!flag) normalizedContent = &normalized;
    if(contentEquals(*normalizedContent)) return;
    assignContentModel(*normalizedContent);
    const int length = contentLength();
    cursor_pos = std::max(0, std::min(cursor_pos, length));
    dragBegin = std::max(0, std::min(dragBegin, length));
    dragEnd = std::max(0, std::min(dragEnd, length));
    invalidateTextLayout();
    cursorVisibilityDirty = true;
    if(!flag) writeNativeTextFromModel();
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void InputBox::setMaxlen(int maxlen) {
    inv.setmaxlen(maxlen);
}

void InputBox::setPosition(double x,double y){
    if(sgn(left - (x - width / 2)) == 0 && sgn(top - (y - height / 2)) == 0) return;
	left = x - width / 2;
	top = y - height / 2;

    float _w,_h;
    measuretext("a",&_w,&_h,btnLayer);
    m_ime_pos_x = left;
    m_ime_pos_y = top + height / 2 + _h / 2 + 2;

    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void InputBox::setScale(double s){
    if(sgn(scale - s) == 0) return;
    // 保存旧缩放比例，用于按比例缩放滚动偏移
    double old_scale = scale;
	width = origin_width * s;
    height = origin_height * s;
    radius = origin_radius * s;
    scale = s;
    left = cx - width / 2;
    top = cy - height / 2;
    // 按比例缩放滚动偏移，保持文本相对位置
    if(old_scale > 0) {
        scroll_offset = scroll_offset * (s / old_scale);
        scroll_offset_y = scroll_offset_y * (s / old_scale);
    }

    if(btnLayer) delimage(btnLayer);
    btnLayer = newimage(width + 8,height + 8);
    ege_enable_aa(true,btnLayer);
    ege_path_reset(&clippath);
    ege_path_addroundrect(&clippath,4,4,width,height,radius);

    invalidateTextLayout();
    needRedraw = true;
    cursorVisibilityDirty = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
    scaleChanged = true;
}

void InputBox::setTextHeight(double height){
    if(sgn(height - text_height) == 0) return;
    text_height = height;
    invalidateTextLayout();
    cursorVisibilityDirty = true;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

double InputBox::getTextHeight(){
    return text_height;
}

void InputBox::moveCursor(int pos){
    pos = std::max(0, std::min(pos, contentLength()));
    if(cursor_pos == pos) return;
    cursor_pos = pos;
    verticalNavigationXValid = false;
    clearCaretVisualLineHint();
    invalidatePendingVerticalNavigation();
    if(!IMECompositionString.empty()) invalidateTextLayout();
    cursorVisibilityDirty = true;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void InputBox::setIMECompositionString(const std::wstring& str){
    if(IMECompositionString == str) return;
    IMECompositionString = str;
    IMECursorPos = std::max(0, std::min(IMECursorPos, (int)IMECompositionString.size()));
    invalidateTextLayout();
    cursorVisibilityDirty = true;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void InputBox::setIMECursorPos(int pos){
    const int newPos = std::max(0, std::min(pos, (int)IMECompositionString.size()));
    if(IMECursorPos == newPos) return;
    IMECursorPos = newPos;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void InputBox::adjustScrollForCursor() {
    const std::wstring& modelContent = materializedContent();
    const float padding = 14 * scale;
    const float visible_width = width - 2 * padding;

    float cursor_pixel_pos, tmp;
    measuretext(modelContent.substr(0, cursor_pos).c_str(), &cursor_pixel_pos, &tmp, btnLayer);
    float cursor_with_ime_pos = cursor_pixel_pos;
    if(IMECompositionString.size()) {
        measuretext((modelContent.substr(0, cursor_pos) + IMECompositionString).c_str(), &cursor_with_ime_pos, &tmp, btnLayer);
    }
    float full_text_width;
    std::wstring display_content = IMECompositionString.size() ?
        (modelContent.substr(0, cursor_pos) + IMECompositionString + modelContent.substr(cursor_pos)) :
        modelContent;
    measuretext(display_content.c_str(), &full_text_width, &tmp, btnLayer);

    float target_pos = IMECompositionString.size() ? cursor_with_ime_pos : cursor_pixel_pos;

    // 修正：右边界判断为 >=，scroll_offset 公式明确
    if(target_pos - scroll_offset >= visible_width - padding) {
        scroll_offset = target_pos - (visible_width - padding);
        needRedraw = true;
        if(this->parent != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                p->setDirty();
            }
        }
    }
    else if(cursor_pixel_pos - scroll_offset < padding) {
        scroll_offset = cursor_pixel_pos - padding;
        if(scroll_offset < 0) scroll_offset = 0;
        needRedraw = true;
        if(this->parent != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                p->setDirty();
            }
        }
    }
    if(full_text_width <= visible_width) {
        scroll_offset = 0;
        needRedraw = true;
        if(this->parent != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                p->setDirty();
            }
        }
    }
    else {
        float max_scroll = full_text_width - visible_width;
        if(scroll_offset > max_scroll) {
            scroll_offset = max_scroll;
            needRedraw = true;
            if(this->parent != nullptr){
                if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                    p->setDirty();
                }
            }
        }
    }
}

void InputBox::reflushCursorTick(){
    start_time = std::chrono::high_resolution_clock::now();
}

bool InputBox::getClickState(){
    return m_clicking;
}

const std::wstring& InputBox::getContent(){
    return materializedContent();
}

int InputBox::getMCounter(){
    return m_counter;
}

int InputBox::charPositionFromLocalX(float localX) const {
    if(multiline) {
        return charPositionFromLocal(localX, (float)(height / 2.0));
    }
    const float padding = 14 * scale;
    float click_x = localX - padding + scroll_offset;

    // When IME composition is active the display string differs from content:
    //   display = content[0..cursor_pos] + IMECompositionString + content[cursor_pos..]
    // Searching against content alone causes clicks after the IME overlay to be
    // mapped to positions that are too small (the IME string's pixel width is
    // ignored).  We therefore search against the display string and convert the
    // result back to a content index.
    bool imeActive = !IMECompositionString.empty();
    const std::wstring& modelContent = materializedContent();
    std::wstring displayContent;
    int cp = 0; // IME insertion point in content
    if(imeActive) {
        cp = std::max(0, std::min(cursor_pos, (int)modelContent.size()));
        displayContent = modelContent.substr(0, cp) + IMECompositionString + modelContent.substr(cp);
    }
    const std::wstring& searchText = imeActive ? displayContent : modelContent;

    int l = 0, r = (int)searchText.length();
    int best_pos = 0;
    float min_dist = 1e9f, tmp, char_x = 0;
    while (l <= r) {
        int mid = (l + r) / 2;
        measuretext(searchText.substr(0, mid).c_str(), &char_x, &tmp, btnLayer);
        float dist = fabsf(char_x - click_x);
        if(dist < min_dist) { min_dist = dist; best_pos = mid; }
        if(char_x < click_x) l = mid + 1;
        else if(char_x > click_x) r = mid - 1;
        else { best_pos = mid; break; }
    }

    // Convert display position back to a content position.
    if(imeActive) {
        int imeLen = (int)IMECompositionString.size();
        if(best_pos <= cp) {
            // Before or at IME start: 1-to-1 mapping with content.
            return best_pos;
        }
        else if(best_pos < cp + imeLen) {
            // Inside the IME composition area: clamp to its start so that the
            // click target is the IME insertion point.
            return cp;
        }
        else {
            // After the IME composition: subtract the IME string length.
            return best_pos - imeLen;
        }
    }
    return best_pos;
}

void InputBox::applyDragMove(int mouseX, int mouseY) {
    // 跳过鼠标未实际移动的合成 MOUSEMOVE（由 SetCursorPos 每帧触发）。
    // 若 IME 提交刚刚改变了文本内容，相同像素 X 会映射到新内容中不同的字符下标，
    // 从而产生虚假选区。只有鼠标真正移动后才重新计算。
    if(mouseX == lastDragMouseX && mouseY == lastDragMouseY) return;
    lastDragMouseX = mouseX;
    lastDragMouseY = mouseY;

    if(multiline) {
        ensureTextLayout();
        updateDragAutoScroll(mouseX, mouseY);
    }
    float localX = (float)(mouseX - (int)left);
    float localY = (float)(mouseY - (int)top);
    int dragVisualLine = -1;
    int best_pos = multiline
        ? charPositionFromLocal(localX, localY, &dragVisualLine)
        : charPositionFromLocalX(localX);
    dragEnd = best_pos;
    // 光标跟随选区末端
    if(cursor_pos != dragEnd) moveCursor(dragEnd);
    if(multiline) {
        verticalNavigationXValid = false;
        setCaretVisualLineHint(dragVisualLine, displayPositionForContentPosition(dragEnd));
    }
    // 同步选区到 sys_edit（EM_SETSEL），保证后续键盘操作在正确范围内进行
    inv.movecursor(dragBegin, dragEnd);
    // 记录是否超出输入框边界（用于自动滚动推进）
    if(mouseX < (int)left) dragSide = -1;
    else if(mouseX > (int)(left + width)) dragSide = 1;
    else dragSide = 0;
    needRedraw = true;
    if(this->parent != nullptr) {
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) p->setDirty();
    }
}

void InputBox::releaseMouseOwningFlag(const mouse_msg& msg){
    if(msg.is_left() && msg.is_up()) {
        // 鼠标抬起：正式结束拖动选择
        dragging = false;
        dragSide = 0;
        dragVerticalSide = 0;
        lastDragMouseX = -1;
        lastDragMouseY = -1;
        lastDragTick = 0.0;
        mouseOwningFlag = nullptr;
    }
    else if(msg.is_move() && dragging && on_focus) {
        // 鼠标移动到所有控件外部时，仍继续更新拖动选择
        applyDragMove(msg.x, msg.y);
    }
}

void InputBox::catchMouseOwningFlag(const mouse_msg& msg){
    // 当鼠标移动到包含本控件的内层 Panel 之外、但仍在外层 Panel 内时，
    // 外层 Panel 会通过 catchMouseOwningFlag 通知 mouseOwningFlag。
    // 此处继续处理拖动选择，保证选区可以延伸到内层 Panel 边界之外。
    if(msg.is_move() && dragging && on_focus) {
        applyDragMove(msg.x, msg.y);
    }
}

void InputBox::deleteSelectedText() {
    if(dragBegin == dragEnd) return;
    int sel_s = std::min(dragBegin, dragEnd);
    int sel_e = std::max(dragBegin, dragEnd);
    sel_s = std::max(0, std::min(sel_s, contentLength()));
    sel_e = std::max(0, std::min(sel_e, contentLength()));
    replaceContentRange(sel_s, sel_e, L"");
    cursor_pos = sel_s;
    dragBegin = dragEnd = sel_s;
    invalidateTextLayout();
    cursorVisibilityDirty = true;
    writeNativeTextFromModel();
    inv.movecursor(sel_s, sel_s);
    needRedraw = true;
    if(this->parent != nullptr) {
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) p->setDirty();
    }
}

void InputBox::cancelDrag() {
    if(!dragging) return;
    dragging = false;
    dragSide = 0;
    dragVerticalSide = 0;
    lastDragMouseX = -1;
    lastDragMouseY = -1;
    lastDragTick = 0.0;
    if(mouseOwningFlag == this) mouseOwningFlag = nullptr;
    // 不修改 EDIT 的选区；延迟同步会在键盘命令完成后读取实际状态。
}

void InputBox::markIMEStart() {
    imeStartPos = cursor_pos;
}

void InputBox::commitIMEString(const std::wstring& compStr) {
    setIMECompositionString(L"");
    if(compStr.empty()) return;

    int insertAt = std::max(0, std::min(imeStartPos, contentLength()));

    // cursor_pos may already have been updated to the new click target (path 2:
    // click inside same box) before this is called.  Shift it past the inserted
    // text when it falls at or after the insertion point so the position stays
    // consistent.  For the WM_KILLFOCUS path (path 1: click outside), cursor_pos
    // still equals imeStartPos, so finalPos naturally lands right after the insert.
    int savedPos = cursor_pos;
    int finalPos = (savedPos < insertAt) ? savedPos : savedPos + (int)compStr.size();
    finalPos = std::max(0, std::min(finalPos, contentLength() + (int)compStr.size()));

    replaceContentRange(insertAt, insertAt, compStr);
    cursor_pos = finalPos;
    dragEnd = finalPos;
    invalidateTextLayout();
    cursorVisibilityDirty = true;
    // Shift dragBegin past the inserted text if it was at or after the insertion point.
    // Do NOT unconditionally override dragBegin: the click handler has already set it to
    // the correct drag anchor (click position). We only need to adjust it for the insertion.
    if(dragBegin >= insertAt) dragBegin += (int)compStr.size();

    writeNativeTextFromModel();
    inv.movecursor(dragBegin, dragEnd);

    needRedraw = true;
    if(parent != nullptr) {
        if(Panel* p = dynamic_cast<Panel*>(parent)) p->setDirty();
    }
}

void InputBox::reset(){
    ripples.clear();
    ripples.shrink_to_fit();
    if(multiline) {
        scroll_offset_y = 0;
        dragVerticalSide = 0;
        lastDragMouseY = -1;
        cursorVisibilityDirty = true;
    }
}

void InputBox::disable(){
    disabled = true;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void InputBox::enable(){
    disabled = false;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

// Slider 类实现
InputBoxBuilder& InputBoxBuilder::setIdentifier(const wstring& id) {
    identifier = id;
    return *this;
}

InputBoxBuilder& InputBoxBuilder::setCenter(double x, double y) {
    cx = x; cy = y;
    return *this;
}

InputBoxBuilder& InputBoxBuilder::setSize(double w, double h) {
    width = w; height = h;
    return *this;
}

InputBoxBuilder& InputBoxBuilder::setRadius(double r) {
    radius = r;
    return *this;
}

InputBoxBuilder& InputBoxBuilder::setContent(const std::wstring& text) {
    content = text;
    return *this;
}

InputBoxBuilder& InputBoxBuilder::setMaxLength(int maxLen) {
    maxLength = maxLen;
    return *this;
}

InputBoxBuilder& InputBoxBuilder::setTextHeight(double height){
    text_height = height;
    return *this;
}

InputBoxBuilder& InputBoxBuilder::setScale(double s) {
    scale = s;
    return *this;
}

InputBoxBuilder& InputBoxBuilder::setMultiline(bool value) {
    multiline = value;
    return *this;
}

InputBox* InputBoxBuilder::build() {
    auto input = new InputBox(cx, cy, width, height, radius, multiline);
    input->setContent(content);
    input->setMaxlen(maxLength);
    input->setScale(scale);
    input->setTextHeight(text_height);
    //widgets.insert(input);
    IdToWidget[identifier] = input;
    return input;
}

Slider::Slider()
    : left(0), top(0), m_value(0.0), m_dragging(false), m_dragOffset(0),
      m_bgColor(EGERGB(200, 200, 200)), m_fgColor(EGERGB(100, 100, 255)) {
        this->width = 0;
        this->height = 0;
      }

void Slider::create(double x, double y, double w, double h) {
    cx = x;
    cy = y;
    left = x - w / 2;
    top = y - h / 2;
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = h / 2;
    origin_thickness = thickness = 4;
}

void Slider::draw(PIMAGE dst,double x,double y){
	double left = x - width / 2;
    double top = y - height / 2;
    // 动态更新缩放比例
    if(m_pressed) {
        m_scale += (0.8f - m_scale) * 0.2f; // 缓动到 60%
    }
    else {
        m_scale += (1.0f - m_scale) * 0.2f; // 回弹
    }

    // 平滑过渡进度
    m_progress += (m_finalprogress - m_progress) * 0.15;
    if(fabs(m_progress - m_finalprogress) < 0.005)
        m_progress = m_finalprogress;

    // 检查是否有动画正在进行，需要持续重绘
    bool isAnimating = (fabs(m_scale - (m_pressed ? 0.8f : 1.0f)) > 0.01) || 
                       (fabs(m_progress - m_finalprogress) > 0.005);
    
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            if(isAnimating){
                if(!m_animatingDirty){
                    p->setAlwaysDirty(true);
                    this->setDrawing(true);
                    m_animatingDirty = true;
                }
                p->setDirty();
            }
            else if(m_animatingDirty) {
                p->setAlwaysDirty(false);
                this->setDrawing(false);
                m_animatingDirty = false;
            }
        }
    }

    if(BackendFlag) {
        return;
    }

    // 背景轨道
    setfillcolor(m_bgColor,dst);
    setlinecolor(m_bgColor,dst);
    
    if(m_orientation == Orientation::Column) {
        radius = width / 2;
        // 竖直轨道
        ege_fillroundrect(left + width / 2 - thickness, top,
                            thickness * 2, height,
                            thickness, thickness, thickness, thickness, dst);

        int knobX = left + width / 2;
        int knobY = top + static_cast<int>((1.0 - m_progress) * height);
        double r = radius * m_scale;

        setfillcolor(m_fgColor, dst);
        setlinecolor(BLACK, dst);
        ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2, dst);

        if(m_pressed) {
            setfillcolor(EGERGBA(80, 80, 80, 80), dst);
            ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2, dst);
        }
        else if(m_hover && !Lpressed) {
            setfillcolor(EGERGBA(80, 80, 80, 40), dst);
            ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2, dst);
        }
    }
    else {
        radius = height / 2;
        // 水平轨道
        ege_fillroundrect(left, top + height / 2 - thickness,
                            width, thickness * 2,
                            thickness, thickness, thickness, thickness, dst);

        int knobX = left + static_cast<int>(m_progress * width);
        int knobY = top + height / 2;
        double r = radius * m_scale;

        setfillcolor(m_fgColor, dst);
        setlinecolor(BLACK, dst);
        ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2, dst);

        if(m_pressed) {
            setfillcolor(EGERGBA(80, 80, 80, 80), dst);
            ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2, dst);
        }
        else if(m_hover && !Lpressed) {
            setfillcolor(EGERGBA(80, 80, 80, 40), dst);
            ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2, dst);
        }
    }
}

void Slider::draw(){
	draw(nullptr,cx,cy);
}

bool Slider::isBackendDirty() const {
    return getDrawingState() != 0;
}

bool Slider::isInside(double x, double y){
    double knobX, knobY;
    if(m_orientation == Orientation::Column) {
        knobX = left + width / 2;
        knobY = top + (1.0 - m_progress) * height;
        radius = width / 2;
    }
    else { // Row
        knobX = left + m_progress * width;
        knobY = top + height / 2;
        radius = height / 2;
    }
    double dx = x - knobX;
    double dy = y - knobY;
    return dx * dx + dy * dy <= radius * radius;
}

bool Slider::isInsideBar(double x, double y){
    return x >= left && x <= left + width && 
           y >= top && y <= top + height;
}

void Slider::setStep(double s){
    step = s;
}

double Slider::fixProgress() {
    double value = m_finalprogress;
    if(step > 0) {
        value = round(value / step) * step;
    }
    return value;
}

bool Slider::handleEvent(const mouse_msg& msg) {
    m_hover = isInside(msg.x, msg.y);
    m_skip = isInsideBar(msg.x, msg.y);

    // 处理其它控件焦点
    if(msg.is_left() && msg.is_up()){
        if(mouseOwningFlag != nullptr && mouseOwningFlag != this){
            mouseOwningFlag->releaseMouseOwningFlag(msg);
        }
    }

    if(!m_hover && m_skip && msg.is_left() && msg.is_down()){
        m_dragging = true;
        m_pressed = true;
        if(m_orientation == Orientation::Column) {
            int my = clamp(msg.y, top, top + height);
            m_finalprogress = 1.0 - (my - top) / static_cast<double>(height);
        }
        else{ // Row
            int mx = clamp(msg.x, left, left + width);
            m_finalprogress = (mx - left) / static_cast<double>(width);
        }
        if(m_value != fixProgress() && m_onChange != nullptr)
        {
            m_value = fixProgress();
            m_onChange(m_value);
        }
        // 通知父容器需要重绘
        if(this->parent != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                p->setDirty();
            }
        }
        mouseOwningFlag = this;
        return true;
    }
    else if(msg.is_left() && msg.is_down() && m_hover) {
        m_dragging = true;
        m_pressed = true;
        int knobX, knobY;
        if(m_orientation == Orientation::Column) {
            knobX = left + width / 2;
            knobY = top + static_cast<int>((1.0 - m_progress) * height);
            m_dragOffset = msg.y - knobY;
        }
        else { // Row
            knobX = left + static_cast<int>(m_progress * width);
            knobY = top + height / 2;
            m_dragOffset = msg.x - knobX;
        }
        mouseOwningFlag = this;
        return true;
    }
    else if(msg.is_move() && m_dragging) {
        if(m_orientation == Orientation::Column) {
            int my = clamp(msg.y - m_dragOffset, top, top + height);
            m_finalprogress = 1.0 - (my - top) / static_cast<double>(height);
        }
        else { // Row
            int mx = clamp(msg.x - m_dragOffset, left, left + width);
            m_finalprogress = (mx - left) / static_cast<double>(width);
        }
        if(m_value != fixProgress() && m_onChange != nullptr)
        {
            m_value = fixProgress();
            m_onChange(m_value);
        }
        // 通知父容器需要重绘
        if(this->parent != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                p->setDirty();
            }
        }
        return true;
    }
    else if(msg.is_left() && msg.is_up()) {
        m_dragging = false;
        m_pressed = false;
        m_finalprogress = fixProgress();
        if(mouseOwningFlag == this) mouseOwningFlag = nullptr;
        m_dragOffset = 0;
    }
    return m_hover;
}

void Slider::releaseMouseOwningFlag(const mouse_msg& msg){
    // 清理拖动状态
    m_dragging = false;
    m_pressed = false;
    m_finalprogress = fixProgress();
    mouseOwningFlag = nullptr;
    m_dragOffset = 0;
}

void Slider::catchMouseOwningFlag(const mouse_msg& msg){
    // 处理持续拖动
    if(!msg.is_move() || !m_dragging) return;
    
    if(m_orientation == Orientation::Column) {
        int my = clamp(msg.y - m_dragOffset, top, top + height);
        m_finalprogress = 1.0 - (my - top) / static_cast<double>(height);
    }
    else { // Row
        int mx = clamp(msg.x - m_dragOffset, left, left + width);
        m_finalprogress = (mx - left) / static_cast<double>(width);
    }
    if(m_value != fixProgress() && m_onChange != nullptr)
    {
        m_value = fixProgress();
        m_onChange(m_value);
    }
    // 通知父容器需要重绘
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void Slider::setProgress(double v) {
    m_finalprogress = m_value = m_progress = clamp(v, 0.0, 1.0);
    m_finalprogress = m_value = m_progress = fixProgress();
}

double Slider::getProgress() const {
    return m_value;
}

void Slider::setColor(color_t bg, color_t fg) {
    m_bgColor = bg;
    m_fgColor = fg;
    this->needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void Slider::setThickness(double t){
	origin_thickness = thickness = t;
}

void Slider::setOnChange(std::function<void(double)> callback) {
    m_onChange = callback;
}

void Slider::setPosition(double x,double y){
	left = x - width / 2;
	top = y - height / 2;
}

void Slider::setScale(double s){
	width = origin_width * s;
    height = origin_height * s;
    radius = origin_radius * s;
    thickness = origin_thickness * s;
    radius = height / 2;
	scale = s;
    left = cx - width / 2;
    top = cy - height / 2;
}

void Slider::setOrientation(Orientation ori){
    m_orientation = ori;
}

SliderBuilder& SliderBuilder::setCenter(double x_, double y_) {
    x = x_; y = y_;
    return *this;
}

SliderBuilder& SliderBuilder::setSize(double w, double h) {
    width = w; height = h;
    return *this;
}

SliderBuilder& SliderBuilder::setColor(color_t bg, color_t fg) {
    bgColor = bg;
    fgColor = fg;
    return *this;
}

SliderBuilder& SliderBuilder::setThickness(double t) {
    thickness = t;
    return *this;
}

SliderBuilder& SliderBuilder::setProgress(double v) {
    progress = v;
    return *this;
}

SliderBuilder& SliderBuilder::setScale(double s) {
    scale = s;
    return *this;
}

SliderBuilder& SliderBuilder::setOnChange(std::function<void(double)> callback) {
    onChange = callback;
    return *this;
}

SliderBuilder& SliderBuilder::setOrientation(Orientation ori){
    orientation = ori;
    return *this;
}

SliderBuilder& SliderBuilder::setIdentifier(const wstring& id) {
    identifier = id;
    return *this;
}

SliderBuilder& SliderBuilder::setStep(double s) {
    step = s;
    return *this;
}

Slider* SliderBuilder::build() {
    auto slider = new Slider();
    IdToWidget[identifier] = slider;
    slider->create(x,y, width, height);
    slider->setColor(bgColor, fgColor);
    slider->setThickness(thickness);
    slider->setScale(scale);
    slider->setStep(step);
    if(onChange) slider->setOnChange(onChange);
    slider->setProgress(progress);
    slider->setOrientation(orientation);
    //widgets.insert(slider);
    return slider;
}

ProgressBar::ProgressBar(double cx, double cy, double w, double h):
      origin_width(w), origin_height(h) {
    this->cx = cx;
    this->cy = cy;
    width = w;
    height = h;
    left = cx - width / 2;
    top = cy - height / 2;
    barLayer = newimage(width, height);
    ege_enable_aa(true, barLayer);
}

ProgressBar::~ProgressBar() {
    if(barLayer) delimage(barLayer);
}

void ProgressBar::setProgress(double p) {
    p = clamp(p, 0.0, 1.0);
    if(fabs(targetProgress - p) > 1e-6) {
        targetProgress = p;
        needRedraw = true;
        if(this->parent != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                p->setDirty();
            }
        }
    }
}

double ProgressBar::getProgress() const {
    return targetProgress;
}

void ProgressBar::setColor(color_t fg) {
    if(fgColor != fg) {
        fgColor = fg;
        needRedraw = true;
        if(this->parent != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                p->setDirty();
            }
        }
    }
}

void ProgressBar::setBackground(color_t bg) {
    if(bgColor != bg) {
        bgColor = bg;
        needRedraw = true;
        if(this->parent != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                p->setDirty();
            }
        }
    }
}

void ProgressBar::draw(PIMAGE dst, double x, double y) {
    double left = x - width / 2;
    double top = y - height / 2;

    //缓动到目标进度
    currentProgress += (targetProgress - currentProgress) * 0.15;
    if(fabs(currentProgress - targetProgress) < 0.005)
        currentProgress = targetProgress;
    
    needRedraw = fabs(currentProgress - targetProgress) > 1e-4;
    if(needRedraw && this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
            p->setNeedRedraw(true);
        }
    }

    if(!needRedraw && fabs(currentProgress - targetProgress) < 1e-4) {
        if(!BackendFlag) {
            putimage_withalpha(dst, barLayer, left, top);
        }
        return;
    }

    // 重绘
    // 使用真正的透明色(PRGB32模式下alpha=0时RGB也应为0)
    setbkcolor_f(EGEARGB(0, 0, 0, 0), barLayer);
    cleardevice(barLayer);

    setfillcolor(bgColor, barLayer);
    ege_fillrect(0, 0, width, height, barLayer);

    setfillcolor(fgColor, barLayer);
    ege_fillrect(0, 0, width * currentProgress, height, barLayer);

    if(!BackendFlag) {
        putimage_withalpha(dst, barLayer, left, top);
    }
}

void ProgressBar::draw() {
    draw(nullptr, cx, cy);
}

bool ProgressBar::isBackendDirty() const {
    return needRedraw || fabs(currentProgress - targetProgress) > 1e-4;
}

bool ProgressBar::handleEvent(const mouse_msg& msg){
    return false;
}

void ProgressBar::setPosition(double x, double y) {
    if(cx == x && cy == y) return;
    cx = x;
    cy = y;
    left = cx - width / 2;
    top = cy - height / 2;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void ProgressBar::setScale(double s) {
    if(fabs(scale - s) < 1e-6) return;
    scale = s;
    width = origin_width * s;
    height = origin_height * s;
    if(barLayer) delimage(barLayer);
    barLayer = newimage(width, height);
    ege_enable_aa(true, barLayer);
    left = cx - width / 2;
    top = cy - height / 2;
    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

// PanelBuilder 实现
ProgressBarBuilder& ProgressBarBuilder::setCenter(double x, double y) {
    cx = x; cy = y;
    return *this;
}

ProgressBarBuilder& ProgressBarBuilder::setSize(double w, double h) {
    width = w; height = h;
    return *this;
}

ProgressBarBuilder& ProgressBarBuilder::setScale(double s) {
    scale = s;
    return *this;
}

ProgressBarBuilder& ProgressBarBuilder::setProgress(double p) {
    progress = p;
    return *this;
}

ProgressBarBuilder& ProgressBarBuilder::setColor(color_t fg) {
    fgColor = fg;
    return *this;
}

ProgressBarBuilder& ProgressBarBuilder::setBackground(color_t bg) {
    bgColor = bg;
    return *this;
}

ProgressBarBuilder& ProgressBarBuilder::setIdentifier(const wstring& id) {
    identifier = id;
    return *this;
}

ProgressBar* ProgressBarBuilder::build() {
    auto bar = new ProgressBar(cx, cy, width, height);
    IdToWidget[identifier] = bar;
    bar->setColor(fgColor);
    bar->setBackground(bgColor);
    bar->setProgress(progress);
    bar->setScale(scale);
    //widgets.insert(bar);
    return bar;
}

Dropdown::Dropdown(double cx, double cy, double w, double h, double r)
    : cx(cx), cy(cy), width(w), height(h), radius(r) {
    mainButton = new Button(cx, cy, w, h, r);
    mainButton->setOnClickEvent([this] { 
        toggleDropdown(); 
    });
    dropdownPanel = new Panel(cx, cy + h, w, 0, r, color);  // 初始化为高度0
    dropdownPanel->setScale(scale);
}

Dropdown::~Dropdown() {
    if(mainButton) delete mainButton;
    if(dropdownPanel) delete dropdownPanel;
    for (Button* option : options) {
        if(option) delete option;
    }
    options.clear();
}

void Dropdown::addOption(const std::wstring& text, std::function<void()> onClick) {
    double optionHeight = height;
    Button* option = new Button(cx, cy, width, optionHeight, radius);
    option->setContent(text);
    option->setColor(color);
    option->setOnClickEvent([this, onClick] {
        // toggleDropdown();
        onClick();
    });
    options.push_back(option);
    updateDropdownLayout();
}

void Dropdown::updateDropdownLayout() {
    double optionHeight = height;
    dropdownPanel->setSize(width, optionHeight * options.size());
    dropdownPanel->setPosition(cx, cy + height / 2 + optionHeight * options.size() / 2 + 4);
    dropdownPanel->setScale(scale);
    dropdownPanel->clearChildren();

    for (size_t i = 0; i < options.size(); ++i) {
        options[i]->setScale(scale);
        dropdownPanel->addChild(options[i], 0, (-(double)options.size() / 2.0 + i + 0.5) * optionHeight);
    }
}

void Dropdown::draw(PIMAGE dst, double x, double y) {
    // 主按钮正常绘制
    mainButton->draw(dst, x, y);

    // 如果展开中，或正在淡入淡出动画
    if(expanded || fadeAlpha > 0.08) {
        // 更新透明度（渐变）
        if(fadingIn) {
            fadeAlpha += 0.09;
            if(fadeAlpha >= 1.0) {
                fadeAlpha = 1.0;
                fadingIn = false;
            }
        }
        else if(fadingOut) {
            fadeAlpha -= 0.09;
            if(fadeAlpha <= 0.0) {
                fadeAlpha = 0.0;
                fadingOut = false;
                expanded = false;
            }
        }

        // 优化：只在透明度改变时调用setAlpha
        int actualAlpha = static_cast<int>(fadeAlpha * 255);
        if(lastAppliedAlpha != actualAlpha) {
            dropdownPanel->setAlpha(actualAlpha);
            lastAppliedAlpha = actualAlpha;
        }
        dropdownPanel->draw(dst, cx, cy + height / 2 + height * options.size() / 2 + 4);
    }
}

void Dropdown::draw() {
    draw(nullptr, cx, cy);
}

bool Dropdown::isBackendDirty() const {
    if(mainButton && mainButton->isBackendDirty()) return true;
    if(dropdownPanel && dropdownPanel->isBackendDirty()) return true;
    return expanded || fadingIn || fadingOut || fadeAlpha > 0.08;
}

bool Dropdown::handleEvent(const mouse_msg& msg) {
    if(expanded) {
        dropdownPanel->handleEvent(msg);
        for (Button* btn : options) {
            btn->handleEvent(msg);
        }
    }

    mainButton->handleEvent(msg);

    // 检测点击空白关闭
    if(msg.is_left() && msg.is_down()) {
        if(!isInside(msg.x, msg.y)) {
            // 点到了空白区域，关闭下拉
            fadingOut = true;
            fadingIn = false;
        }
    }
    return false;
}

void Dropdown::toggleDropdown() {
    if(!expanded) {
        expanded = true;
        fadingIn = true;
        fadingOut = false;
    }
    else {
        fadingOut = true;
        fadingIn = false;
    }
}

void Dropdown::setPosition(double x, double y) {
    cx = x; cy = y;
    mainButton->setPosition(x, y);
    updateDropdownLayout();
}

void Dropdown::setScale(double s) {
    scale = s;
    mainButton->setScale(s);
    updateDropdownLayout();
}

void Dropdown::setContent(const std::wstring& text) {
    mainButton->setContent(text);
}

void Dropdown::setColor(color_t col) {
    color = col;
    mainButton->setColor(col);
    for (auto& opt : options)
        opt->setColor(col);
    this->needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

bool Dropdown::isInside(double x, double y) const {
    // 检查是否在主按钮内
    if(mainButton->isInside(x, y))
        return true;

    // 如果未展开，无需检查下拉项
    if(!expanded && fadeAlpha <= 0.01)
        return false;

    // 检查是否在下拉选项内
    for (Button* btn : options) {
        if(btn->isInside(x, y))
            return true;
    }

    return false;
}

DropdownBuilder& DropdownBuilder::setCenter(double x, double y) {
    cx = x; cy = y;
    return *this;
}

DropdownBuilder& DropdownBuilder::setSize(double w, double h) {
    width = w; height = h;
    return *this;
}

DropdownBuilder& DropdownBuilder::setRadius(double r) {
    radius = r;
    return *this;
}

DropdownBuilder& DropdownBuilder::setContent(const std::wstring& text) {
    content = text;
    return *this;
}

DropdownBuilder& DropdownBuilder::setColor(color_t col) {
    color = col;
    return *this;
}

DropdownBuilder& DropdownBuilder::setScale(double s) {
    scale = s;
    return *this;
}

DropdownBuilder& DropdownBuilder::addOption(const std::wstring& text, std::function<void()> onClick) {
    optionList.emplace_back(text, onClick);
    return *this;
}

Dropdown* DropdownBuilder::build() {
    auto dropdown = new Dropdown(cx, cy, width, height, radius);
    dropdown->setContent(content);
    dropdown->setColor(color);
    dropdown->setScale(scale);
    for (const auto& pair : optionList) {
        dropdown->addOption(pair.first, pair.second);
    }
    //widgets.insert(dropdown);
    return dropdown;
}

Radio::Radio(double cx, double cy, double r, const std::wstring& val)
    : cx(cx), cy(cy), radius(r), origin_radius(r), value(val) {}

void Radio::setPosition(double x, double y) {
    cx = x;
    cy = y;
}

void Radio::setScale(double s) {
    scale = s;
    radius = origin_radius * scale;
    width = radius * 2;
    height = radius * 2;
}

void Radio::setStyle(RadioStyle s) {
    style = s;
}

void Radio::setGroupValueRef(std::wstring* ref) {
    groupValuePtr = ref;
}

void Radio::setOnSelect(std::function<void()> callback) {
    onSelect = callback;
}

std::wstring Radio::getValue() const {
    return value;
}

bool Radio::isChecked() const {
    return groupValuePtr && *groupValuePtr == value;
}

void Radio::draw(PIMAGE dst, double x, double y) {
    bool nowChecked = isChecked();
    bool showDot = isChecked() || (animOut && animProgress < 1.0);
    if(!nowChecked && wasChecked && !animOut) {
        // 被取消选中了，启动缩小动画
        animOut = true;
        animIn = false;
        animProgress = 0.0;
    }
    wasChecked = nowChecked;

    double R = radius;

    if(!BackendFlag) {
        // 悬停阴影
        if(hovered) {
            double shadowRadius = R + 6;
            setfillcolor(EGERGBA(50, 50, 50, 32), dst);
            ege_fillellipse(x - shadowRadius, y - shadowRadius, shadowRadius * 2, shadowRadius * 2, dst);
        }

        if(keepColor || nowChecked) setlinecolor(EGERGB(50, 150, 250), dst);
        else setlinecolor(EGERGB(110,110,110), dst);
        setlinewidth(2, dst);

        if(style == RadioStyle::Filled) {
            setfillcolor(WHITE, dst);
            ege_fillellipse(x - R, y - R, R * 2, R * 2, dst);
        }
        else if(style == RadioStyle::Outline) {
            // 只绘制线框，不填充
            ege_ellipse((float)(x - R), (float)(y - R), (float)(R * 2), (float)(R * 2), dst);
        }
    }

    // 更新动画进度
    if(animIn) {
        animProgress += animSpeed;
        if(animProgress >= 1.0) {
            animProgress = 1.0;
            animIn = false;
        }
    }
    else if(animOut) {
        animProgress += animSpeed;
        if(animProgress >= 1.0) {
            animOut = false;
        }
    }

    // 动画缩放
    double scaleFactor = 1.0;
    if(animIn) {
        scaleFactor = 0.7 + 0.4 * animProgress;
        lastScale = scaleFactor;
    }
    else if(animOut) {
        scaleFactor = 1.0 - 0.4 * animProgress;
        lastScale = scaleFactor;
    }
    else {
        scaleFactor = lastScale;
    }

    if(showDot && !BackendFlag) {
        double r_in = R * 0.5 * scaleFactor;
        if(keepColor || nowChecked){
            setlinecolor(EGERGB(50, 150, 250), dst);
            setfillcolor(EGERGB(50, 150, 250), dst);
        }
        else{
            setlinecolor(EGERGB(110,110,110), dst);
            setfillcolor(EGERGB(110,110,110), dst);
        }
        setlinecolor(EGERGB(245, 245, 235), dst);
        ege_fillellipse(x - r_in, y - r_in, r_in * 2, r_in * 2, dst);
    }
}

void Radio::draw() {
    draw(nullptr, cx, cy);
}

bool Radio::isBackendDirty() const {
    return animIn || animOut;
}

bool Radio::handleEvent(const mouse_msg& msg) {
    int dx = msg.x - cx;
    int dy = msg.y - cy;
    hovered = dx * dx + dy * dy <= radius * radius;

    if(msg.is_left() && msg.is_down() && hovered) {
        if(groupValuePtr && *groupValuePtr != value) {
            *groupValuePtr = value;
            animIn = true;
            animOut = false;
            animProgress = 0.0;
            if(onSelect) onSelect();
            return true;
        }
        else if(groupValuePtr && *groupValuePtr == value) {
            // cout<<"awa\n";
            // animOut = true;
            // animIn = false;
            // animProgress = 0.0;
        }
    }
    return false;
}

RadioBuilder& RadioBuilder::setCenter(double x, double y) {
    cx = x; cy = y; return *this;
}
RadioBuilder& RadioBuilder::setRadius(double r) {
    radius = r; return *this;
}
RadioBuilder& RadioBuilder::setScale(double s) {
    scale = s; return *this;
}
RadioBuilder& RadioBuilder::setValue(const std::wstring& val) {
    value = val; return *this;
}
RadioBuilder& RadioBuilder::setGroupValueRef(std::wstring* ref) {
    groupPtr = ref; return *this;
}
RadioBuilder& RadioBuilder::setOnSelect(std::function<void()> cb) {
    onSelect = cb; return *this;
}
RadioBuilder& RadioBuilder::setStyle(RadioStyle s) {
    style = s;
    return *this;
}
Radio* RadioBuilder::build() {
    auto radio = new Radio(cx, cy, radius, value);
    radio->setScale(scale);
    if(groupPtr) radio->setGroupValueRef(groupPtr);
    if(onSelect) radio->setOnSelect(onSelect);
    radio->setStyle(style); // ? 设置样式
    //widgets.insert(radio);
    return radio;
}

RadioController::RadioController(double cx, double cy, double r, double gap, double scale, RadioStyle style)
    : cx(cx), cy(cy), radius(r), gap(gap), scale(scale), style(style) {}

void RadioController::addValue(const std::wstring& val) {
    values.push_back(val);
}

void RadioController::setDefault(const std::wstring& val) {
    currentValue = val;
}

void RadioController::setOnChange(std::function<void(const std::wstring&)> cb) {
    onChange = cb;
}

std::wstring RadioController::getValue(){
    return currentValue;
}

Box* RadioController::build() {
    Box* bx = BoxBuilder()
        .setCenter(cx, cy)
        .setAlign(LayoutAlign::Start)
        .setPadding(5)
        .setSpacing(gap)
        .setDirection(LayoutDirection::Column)
        .setSize(radius * 2 + 10, (radius * 2 + gap) * values.size() - gap + 10)
        .build();
    
    for (size_t i = 0; i < values.size(); ++i) {
        std::wstring val = values[i];
        Radio* r = RadioBuilder()
            .setCenter(cx, cy + i * gap)
            .setRadius(radius)
            .setScale(scale)
            .setStyle(style)
            .setValue(val)
            .setGroupValueRef(&currentValue)
            .setOnSelect([=,this] {
                currentValue = val;
                if(onChange) onChange(val);
            })
            .build();
        bx->addChild(r,0,0);
    }
    return bx;
}

RadioControllerBuilder& RadioControllerBuilder::setCenter(double x, double y) {
    cx = x; cy = y;
    return *this;
}

RadioControllerBuilder& RadioControllerBuilder::setRadius(double r) {
    radius = r;
    return *this;
}

RadioControllerBuilder& RadioControllerBuilder::setGap(double g) {
    gap = g;
    return *this;
}

RadioControllerBuilder& RadioControllerBuilder::setScale(double s) {
    scale = s;
    return *this;
}

RadioControllerBuilder& RadioControllerBuilder::setStyle(RadioStyle s) {
    style = s;
    return *this;
}

RadioControllerBuilder& RadioControllerBuilder::add(const std::wstring& val) {
    values.push_back(val);
    return *this;
}

RadioControllerBuilder& RadioControllerBuilder::setDefault(const std::wstring& val) {
    defaultValue = val;
    return *this;
}

RadioControllerBuilder& RadioControllerBuilder::setOnChange(std::function<void(const std::wstring&)> cb) {
    onChange = cb;
    return *this;
}

RadioController* RadioControllerBuilder::build() {
    auto controller = new RadioController(cx, cy, radius, gap, scale, style);
    controller->setDefault(defaultValue);
    controller->setOnChange(onChange);
    for (const auto& val : values) {
        controller->addValue(val);
    }
    controller->build();
    return controller;
}

Toggle::Toggle(double cx, double cy, double w, double h)
    : cx(cx), cy(cy), width(w), height(h) {}

void Toggle::setChecked(bool c) {
    checked = c;
    knobTarget = c ? 1.0 : 0.0;
    if(this->parent != nullptr) {
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void Toggle::toggle() {
    setChecked(!checked);
    if(onToggle) onToggle(checked);
}

bool Toggle::isChecked() const {
    return checked;
}

void Toggle::setOnToggle(std::function<void(bool)> cb) {
    onToggle = cb;
}

void Toggle::setScale(double s) {
    scale = s;
}

void Toggle::setPosition(double x, double y) {
    cx = x; cy = y;
}

void Toggle::setKeepColor(bool keep) {
    keepColor = keep;
}

void Toggle::setBaseColor(color_t col) {
    baseColor = col;
}

void Toggle::setDisabled(bool d) {
    disabled = d;
}

bool Toggle::isDisabled() const {
    return disabled;
}

bool Toggle::handleEvent(const mouse_msg& msg) {
    double w = width * scale;
    double h = height * scale;
    double r = h / 2;

    int dx = msg.x - cx;
    int dy = msg.y - cy;

    bool hoverChanged = false;
    bool prevHovered = hovered;
    hovered = (std::abs(dx) <= w / 2 && std::abs(dy) <= h / 2);
    hoverChanged = (prevHovered != hovered);
    if(hoverChanged && this->parent != nullptr) {
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }

    if(disabled) return false;
    if(!hovered) return false;

    if(msg.is_left()) {
        if(msg.is_down()) {
            // 记录是否在区域内按下
            pressedIn = hovered;
            mouseOwningFlag = this;
            return true;
        }
        else if(msg.is_up()) {
            // 如果松开时还在区域内且是之前按下的
            if(hovered && pressedIn) {
                toggle(); // ? 这里才执行切换动画与回调
            }
            if(mouseOwningFlag == this){
                releaseMouseOwningFlag(msg);
            }
        }
    }
    return false;
}

void Toggle::releaseMouseOwningFlag(const mouse_msg& msg){
    // 清理按下状态
    pressedIn = false;
    mouseOwningFlag = nullptr;
}

void Toggle::catchMouseOwningFlag(const mouse_msg& msg){
    // Toggle不需要处理拖动中的移动事件
    // 只需要在releaseMouseOwningFlag和handleEvent中处理状态即可
}

color_t removeAlpha(color_t c) {
    return EGERGB(GetRValue(c), GetGValue(c), GetBValue(c));
}


color_t mixColor(color_t c1, color_t c2, double ratio) {
    // 确保去除 alpha 通道
    c1 = removeAlpha(c1);
    c2 = removeAlpha(c2);
    int r = (int)(GetRValue(c1) * (1 - ratio) + GetRValue(c2) * ratio);
    int g = (int)(GetGValue(c1) * (1 - ratio) + GetGValue(c2) * ratio);
    int b = (int)(GetBValue(c1) * (1 - ratio) + GetBValue(c2) * ratio);
    return EGERGB(r, g, b);
}

void Toggle::draw(PIMAGE dst, double x, double y) {
    // === 动画推进 ===
    if(std::abs(knobOffset - knobTarget) > 1e-3)
        knobOffset += (knobTarget - knobOffset) * animationSpeed;
    else
        knobOffset = knobTarget;

    bool isAnimating = std::abs(knobOffset - knobTarget) > 1e-3;
    if(this->parent != nullptr) {
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            if(isAnimating) {
                if(!m_animatingDirty) {
                    p->setAlwaysDirty(true);
                    this->setDrawing(true);
                    m_animatingDirty = true;
                }
                p->setDirty();
            }
            else if(m_animatingDirty) {
                p->setAlwaysDirty(false);
                this->setDrawing(false);
                m_animatingDirty = false;
            }
        }
    }

    if(BackendFlag) {
        return;
    }

    double w = width * scale;
    double h_track = height * scale * 0.6;
    double r_track = h_track / 2;

    double h_knob = height * scale;
    double r_knob = h_knob / 2;

    // === 滑块位置 ===
    double knobX = x - w / 2 + r_track + knobOffset * (w - h_track);

    // === 轨道颜色 ===
    color_t trackColor;
    if(disabled) {
        trackColor = EGERGB(200, 200, 200); // 禁用轨道
    }
    else if(keepColor) {
        trackColor = checked ? mixColor(baseColor, WHITE, 0.55)
                            : mixColor(baseColor, WHITE, 0.80);
    }
    else {
        trackColor = checked ? mixColor(baseColor, WHITE, 0.55)
                            : EGERGB(160, 160, 160);
    }

    setfillcolor(trackColor, dst);
    setlinecolor(TRANSPARENT, dst);
    ege_fillroundrect(x - w / 2, y - h_track / 2, w, h_track, r_track, r_track, r_track, r_track, dst);

    // === 悬浮灰圈背景（hover 时）
    if(hovered && !disabled) {
        double hoverR = r_knob + 7;
        setfillcolor(EGERGBA(100, 100, 100, 32), dst);
        ege_fillellipse(knobX - hoverR, y - hoverR, hoverR * 2, hoverR * 2, dst);
    }

    // === 滑块颜色（关闭时为浅灰而非纯白）
    color_t knobColor;
    if(disabled) {
        knobColor = EGERGB(220, 220, 220); // 灰滑块
    }
    else if(keepColor) {
        knobColor = checked ? baseColor : EGERGB(245, 245, 245);
    }
    else {
        knobColor = checked ? baseColor : EGERGB(240, 240, 240);
    }

    // === 滑块绘制（加边框以增强可见性）
    setfillcolor(knobColor, dst);
    setlinecolor(EGERGB(180, 180, 180), dst);  // 灰边框
    setlinewidth(1, dst);
    ege_fillellipse(knobX - r_knob, y - r_knob, h_knob, h_knob, dst);
}

void Toggle::draw() {
    draw(nullptr, cx, cy);
}

bool Toggle::isBackendDirty() const {
    return fabs(knobOffset - knobTarget) > 1e-3;
}

double Toggle::getWidth(){
    return width * scale;
}

double Toggle::getHeight(){
    return height * scale;
}

ToggleBuilder& ToggleBuilder::setCenter(double x, double y) {
    cx = x; cy = y;
    return *this;
}

ToggleBuilder& ToggleBuilder::setSize(double w_, double h_) {
    w = w_; h = h_;
    return *this;
}

ToggleBuilder& ToggleBuilder::setScale(double s) {
    scale = s;
    return *this;
}

ToggleBuilder& ToggleBuilder::setChecked(bool c) {
    checked = c;
    return *this;
}

ToggleBuilder& ToggleBuilder::setOnToggle(std::function<void(bool)> cb) {
    onToggle = cb;
    return *this;
}

ToggleBuilder& ToggleBuilder::setKeepColor(bool keep) {
    keepColor = keep;
    return *this;
}

ToggleBuilder& ToggleBuilder::setBaseColor(color_t col) {
    baseColor = col;
    return *this;
}

ToggleBuilder& ToggleBuilder::setIdentifier(const std::wstring& id){
    identifier = id;
    return *this;
}

Toggle* ToggleBuilder::build() {
    auto toggle = new Toggle(cx, cy, w, h);
    toggle->setScale(scale);
    toggle->setChecked(checked);
    if(onToggle) toggle->setOnToggle(onToggle);
    toggle->setKeepColor(keepColor);
    toggle->setBaseColor(baseColor);
    //widgets.insert(toggle);
    IdToWidget[identifier] = toggle;
    return toggle;
}

// 构造函数
Text::Text(double x, double y, int maxW)
    : posX(x), posY(y), maxWidth(maxW) {}

// 设置文本
void Text::setContent(const std::wstring& text) {
    contentW = text;
    updateLayout();
}

void Text::setMaxWidth(int width) {
    maxWidth = width;
    updateLayout();
}

void Text::setFont(int size, const std::wstring& name) {
    fontSize = size;
    fontName = name;
    updateLayout();
}

void Text::setColor(color_t col) {
    color = col;
    this->needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void Text::setScale(double s) {
    scale = s;
    updateLayout();
}

void Text::setPosition(double x, double y) {
    posX = x;
    posY = y;
    cx = x;
    cy = y;
}

int Text::getTextWidth() const {
    return textWidth;
}

int Text::getTextHeight() const {
    return textHeight;
}

int Text::getWidth() const {
    return textWidth;
}

int Text::getHeight() const {
    return textHeight;
}

int Text::getMaxWidth() const {
    return maxWidth;
}

// 布局计算
void Text::updateLayout() {
    lines.clear();
    cachedLineWidths.clear();
    textWidth = 0;
    textHeight = 0;

    ege_setfont(fontSize * scale, fontName.c_str());
    lastFontScale = fontSize * scale;

    std::wstring line;
    for (wchar_t ch : contentW) {
        if(ch == L'\n') {
            lines.push_back(line);
            line.clear();
            continue;
        }

        line += ch;

        float tmp,_w;
        measuretext(line.c_str(),&_w,&tmp);
        if(maxWidth > 0 && _w > maxWidth) {
            line.pop_back();
            lines.push_back(line);
            line = ch;
        }
    }

    if(!line.empty())
        lines.push_back(line);

    // 优化：缓存每行的宽度
    cachedLineWidths.reserve(lines.size());
    for (const auto& l : lines) {
        float w,tmp;
        measuretext(l.c_str(),&w,&tmp);
        cachedLineWidths.push_back(w);
        textWidth = ((textWidth < w) ? w : textWidth);
    }

    float _h,tmp;
    measuretext("A",&tmp,&_h);
    textHeight = lines.size() * _h;

    height = textHeight;
    width = (maxWidth > 0) ? maxWidth : textWidth;

    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
        if(Box* b = dynamic_cast<Box*>(this->parent)) {
            b->setDirty();
        }
    }
}

// 默认绘制
void Text::draw() {
    draw(nullptr, posX, posY);
}

bool Text::isBackendDirty() const {
    return false;
}

void Text::draw(PIMAGE dst, double x, double y) {
    if(BackendFlag) return;
    ege_setfont(fontSize * scale, fontName.c_str(), dst);
    settextcolor(color, dst);

    // 将中心坐标转换为左上角坐标，与其他控件（Button、Panel等）保持一致
    double originX = x - width / 2;
    double originY = y - height / 2;

    for (size_t i = 0; i < lines.size(); ++i) {
        double x_draw = originX;
        
        float lineW = (i < cachedLineWidths.size()) ? cachedLineWidths[i] : 0;
        if(lineW == 0) {
            float w = 0, h = 0;
            measuretext(lines[i].c_str(), &w, &h, dst);
            lineW = w;
        }

        if(align == TextAlign::Center)
            x_draw = originX + (width - lineW) / 2;
        else if(align == TextAlign::Right)
            x_draw = originX + (width - lineW);

        float tmp, _h;
        measuretext("A", &tmp, &_h, dst);
        double lineHeight = (textHeight > 0 && lines.size() > 0) ? 
            (textHeight / lines.size()) : _h;
        
        double y_draw = originY + i * (lineHeight + lineSpacing);
        ege_outtextxy(x_draw, y_draw, lines[i].c_str(), dst);
    }
}

bool Text::handleEvent(const mouse_msg& msg) {
    // Text 是纯展示控件，不处理事件
    return false;
}

void Text::setAlign(TextAlign a) {
    align = a;
}

void Text::setLineSpacing(int spacing) {
    lineSpacing = spacing;
}

TextAlign Text::getAlign() const {
    return align;
}

int Text::getLineSpacing() const {
    return lineSpacing;
}

TextBuilder& TextBuilder::setIdentifier(const wstring& id) {
    identifier = id;
    return *this;
}

TextBuilder& TextBuilder::setPosition(double px, double py) {
    x = px; y = py;
    return *this;
}

TextBuilder& TextBuilder::setMaxWidth(int w) {
    maxWidth = w;
    return *this;
}

TextBuilder& TextBuilder::setFont(int size, const std::wstring& name) {
    fontSize = size;
    fontName = name;
    return *this;
}

TextBuilder& TextBuilder::setScale(double s) {
    scale = s;
    return *this;
}

TextBuilder& TextBuilder::setColor(color_t c) {
    color = c;
    return *this;
}

TextBuilder& TextBuilder::setContent(const std::wstring& text) {
    content = text;
    return *this;
}

TextBuilder& TextBuilder::setAlign(TextAlign a) {
    align = a;
    return *this;
}

TextBuilder& TextBuilder::setLineSpacing(int px) {
    lineSpacing = px;
    return *this;
}

Text* TextBuilder::build() {
    Text* txt = new Text(x, y, maxWidth);
    txt->setFont(fontSize, fontName);
    txt->setScale(scale);
    txt->setColor(color);
    txt->setContent(content);
    txt->setAlign(align);
    txt->setLineSpacing(lineSpacing);
    IdToWidget[identifier] = txt;
    //widgets.insert(txt);
    return txt;
}
// fix: 在鼠标拖动时，需要持续显示阴影
Knob::Knob(double cx, double cy, double r)
    : cx(cx), cy(cy), radius(r), origin_radius(r) {
    // 初始化内部范围为外部范围
    innerMin = minValue;
    innerMax = maxValue;
    // 初始化displayValue与value相同
    displayValue = value;
    height = r * 2;
    width = r * 2;
}

void Knob::setRange(double minVal, double maxVal) {
    minValue = minVal;
    maxValue = maxVal;
    // 同步更新内部范围（如果未单独设置）
    if(innerMin == minValue && innerMax == maxValue) {
        innerMin = minVal;
        innerMax = maxVal;
    }
}

void Knob::setStep(double s) {
    step = s;
}

void Knob::setValue(double val) {
    double oldValue = value;
    value = clamp(val);
    if(step > 0) {
        value = applyStep(value);
    }
    // 如果值改变且有回调，触发回调
    if(oldValue != value && onChange) {
        onChange(value);
    }
}

double Knob::getValue() const {
    return value;
}

void Knob::setColor(color_t fg, color_t bg) {
    fgColor = fg;
    bgColor = bg;
    this->needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

void Knob::setOnChange(std::function<void(double)> cb) {
    onChange = cb;
}

void Knob::setOffsetAngle(double angle) {
    offsetAngle = angle;
}

void Knob::setInnerRange(double innerMinVal, double innerMaxVal) {
    innerMin = innerMinVal;
    innerMax = innerMaxVal;
}

void Knob::setShowValue(bool show) {
    showValue = show;
}

void Knob::setFontSize(int size) {
    fontSize = size;
}

void Knob::setDisabled(bool dis) {
    disabled = dis;
}

void Knob::setReadonly(bool ro) {
    readonly = ro;
}

void Knob::setScale(double s) {
    scale = s;
    radius = origin_radius * s;
    height = radius * 2;
    width = radius * 2;
}

void Knob::setPosition(double x, double y) {
    cx = x;
    cy = y;
}

double Knob::clamp(double v) const {
    if(v < minValue) return minValue;
    if(v > maxValue) return maxValue;
    return v;
}

double Knob::applyStep(double v) const {
    if(step <= 0) return v;
    // 将值对齐到最近的步进点
    double steps = std::round((v - minValue) / step);
    return minValue + steps * step;
}

double Knob::valueToAngle(double v) const {
    // 将值映射到角度范围
    // 完整范围：从 -90° (顶部) 到 +270° (总共 360°)
    // 加上偏移角度
    double range = maxValue - minValue;
    if(range <= 0) return offsetAngle;
    
    double ratio = (v - minValue) / range;
    // 从顶部开始，顺时针旋转完整360度
    // 起始角度为 -90° (顶部)，终止角度为 +270° (顶部)
    double startAngle = -90.0 + offsetAngle;
    double endAngle = 270.0 + offsetAngle;
    double totalSweep = endAngle - startAngle;
    
    return startAngle + ratio * totalSweep;
}

double Knob::angleToValue(double angle) const {
    // 将角度映射回值
    double startAngle = -90.0 + offsetAngle;
    double endAngle = 270.0 + offsetAngle;
    double totalSweep = endAngle - startAngle;
    
    // 标准化角度到 [-180, 180] 范围
    while (angle > 180) angle -= 360;
    while (angle < -180) angle += 360;
    
    double ratio = (angle - startAngle) / totalSweep;
    ratio = std::max(0.0, std::min(1.0, ratio));
    
    return minValue + ratio * (maxValue - minValue);
}

double Knob::calculateAngle(double x, double y) const {
    // 计算鼠标相对于中心的角度
    double dx = x - cx;
    double dy = y - cy;
    
    // atan2 返回 [-PI, PI]，转换为度数
    double angle = std::atan2(dy, dx) * 180.0 / 3.14159265359;
    
    return angle;
}

bool Knob::isInside(double x, double y) const {
    double dx = x - cx;
    double dy = y - cy;
    double dist = std::sqrt(dx * dx + dy * dy);
    return dist <= radius * 1.075;
}

void Knob::draw(PIMAGE dst, double x, double y) {
    ege_enable_aa(true,dst);
    double r = radius;
    
    // 缓动到目标值（类似Slider的实现）
    displayValue += (value - displayValue) * 0.09;
    if(fabs(displayValue - value) < 0.01) {
        displayValue = value;
    }
    
    // 检查是否有动画正在进行，需要持续重绘
    bool isAnimating = fabs(displayValue - value) >= 0.01;
    
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            if(isAnimating){
                if(!dirtyFlag) {
                    p->setAlwaysDirty(true);
                    p->setDirty();
                    this->setDrawing(true);
                    dirtyFlag = true;
                }
            }
            else if(dirtyFlag) {
                p->setAlwaysDirty(false);
                this->setDrawing(false);
                dirtyFlag = false;
            }
        }
    }

    if(BackendFlag) {
        return;
    }
    
    // 如果禁用，使用灰色
    color_t currentFgColor = disabled ? EGERGB(180, 180, 180) : fgColor;
    color_t currentBgColor = disabled ? EGERGB(240, 240, 240) : bgColor;
    
    // 圆弧粗细
    double arcThickness = r * 0.15;
    
    // === 绘制背景轨道（完整360度圆）===
    setlinecolor(currentBgColor, dst);
    setlinewidth((int)arcThickness, dst);
    
    // 绘制完整的圆形轨道
    ege_arc((float)(x - r), (float)(y - r), (float)(2 * r), (float)(2 * r), 
            0.0f, 360.0f, dst);
    
    // === 绘制前景进度弧（当前值，使用displayValue实现缓动）===
    setlinecolor(currentFgColor, dst);
    setlinewidth(arcThickness, dst);
    
    double startAngle = -90.0 + offsetAngle;
    double currentAngle = valueToAngle(displayValue);
    double progressSweep = currentAngle - startAngle;
    
    // 绘制进度弧
    // 添加小的epsilon以确保完整绘制到边界
    if(progressSweep > 0.01) {
        // 当接近最大值时，确保完整绘制360度
        double drawSweep = progressSweep;
        if(progressSweep > 359.5 && progressSweep < 360.5) {
            drawSweep = 360.0;  // 确保完整的360度弧
        }
        ege_arc((float)(x - r), (float)(y - r), (float)(2 * r), (float)(2 * r), 
                (float)startAngle, (float)drawSweep, dst);
    }
    
    // === 绘制中心填充圆 ===
    color_t centerColor = WHITE;
    if(hovered && !disabled && !readonly && !(mouseOwningFlag != nullptr && mouseOwningFlag != this)) {
        centerColor = EGERGB(250, 250, 250);
    }
    
    setfillcolor(centerColor, dst);
    setlinecolor(disabled ? EGERGB(200, 200, 200) : EGERGB(220, 220, 220), dst);
    setlinewidth(2, dst);
    ege_fillcircle(x, y, (r * 0.7), dst);
    
    // === 显示当前值 ===
    if(showValue) {
        // 计算字体大小
        double actualFontSize = fontSize > 0 ? fontSize : (r * 0.35);
        
        // 格式化显示值（显示displayValue而不是value，实现缓动效果）
        wchar_t valueText[64];
        if(step >= 1.0) {
            swprintf(valueText, 64, L"%.0f", value);
        }
        else if(step >= 0.1) {
            swprintf(valueText, 64, L"%.1f", value);
        }
        else {
            swprintf(valueText, 64, L"%.2f", value);
        }
        
        // 设置字体和颜色
        ege_setfont(actualFontSize, L"Consolas", dst);
        setcolor(disabled ? EGERGB(150, 150, 150) : BLACK, dst);
        setbkmode(TRANSPARENT, dst);
        
        // 计算文本宽度和高度以居中显示
        float textWidth,textHeight;
        measuretext(valueText,&textWidth,&textHeight,dst);
        
        ege_outtextxy(x - textWidth / 2, y - textHeight / 2, valueText, dst);
    }
}

void Knob::draw() {
    draw(nullptr, cx, cy);
}

bool Knob::isBackendDirty() const {
    return getDrawingState() != 0;
}

void Knob::releaseMouseOwningFlag(const mouse_msg& msg){
    dragging = false;
    mouseOwningFlag = nullptr;
}

void Knob::catchMouseOwningFlag(const mouse_msg& msg){
    if(!msg.is_move()) return;
    // 计算鼠标相对于中心的角度
    double dx = msg.x - cx;
    double dy = msg.y - cy;
    
    // 使用 atan2 计算角度 (屏幕坐标系：Y向下为正)
    // 在屏幕坐标系中：
    // 0° = 右侧 (3点钟方向)
    // 90° = 下方 (6点钟方向) 
    // -90° = 上方 (12点钟方向，起始位置)
    // ±180° = 左侧 (9点钟方向)
    double angle = std::atan2(dy, dx) * 180.0 / 3.14159265359;
    
    // Knob 的值范围映射到完整的 -90° 到 +270° (360度，从顶部开始)
    // 整个360度圆都可以交互，完整映射到值范围
    
    // 应用偏移角度
    angle -= offsetAngle;
    
    // 标准化到 [-180, 180]
    while (angle > 180) angle -= 360;
    while (angle < -180) angle += 360;
    
    // 定义值范围对应的角度（从顶部-90°开始）
    double startAngle = -90.0;   // 对应 minValue (顶部)
    double endAngle = 270.0;     // 对应 maxValue (顶部)
    
    // 处理跨越180°边界的情况
    double normalizedAngle = angle;
    if(normalizedAngle < startAngle) {
        normalizedAngle += 360.0;  // 转换到正值范围
    }
    
    double totalSweep = 360.0;
    
    // 将整个360度圆线性映射到值范围
    double ratio = (normalizedAngle - startAngle) / totalSweep;
    double newValue = minValue + ratio * (maxValue - minValue);
    
    // 限制和应用步进
    newValue = clamp(newValue);
    if(step > 0) {
        newValue = applyStep(newValue);
    }
    
    // 更新值并触发回调
    if(newValue != value) {
        value = newValue;
        if(onChange) {
            onChange(value);
        }
        // 通知父容器需要重绘
        if(this->parent != nullptr){
            if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                p->setDirty();
            }
        }
    }
    
    lastMouseX = msg.x;
    lastMouseY = msg.y;
}

bool Knob::handleEvent(const mouse_msg& msg) {
    // 检查是否在旋钮内
    hovered = isInside(msg.x, msg.y);

    // 处理其它控件焦点
    if(msg.is_left() && msg.is_up()){
        if(mouseOwningFlag != nullptr && mouseOwningFlag != this){
            mouseOwningFlag->releaseMouseOwningFlag(msg);
        }
    }
    
    // 按下鼠标左键开始拖动
    if(msg.is_left() && msg.is_down() && hovered) {
        dragging = true;
        lastMouseX = msg.x;
        lastMouseY = msg.y;
        mouseOwningFlag = this;
        return true;
    }
    // 拖动中
    else if(msg.is_move() && dragging) {
        // 计算鼠标相对于中心的角度
        double dx = msg.x - cx;
        double dy = msg.y - cy;
        
        // 使用 atan2 计算角度 (屏幕坐标系：Y向下为正)
        // 在屏幕坐标系中：
        // 0° = 右侧 (3点钟方向)
        // 90° = 下方 (6点钟方向) 
        // -90° = 上方 (12点钟方向，起始位置)
        // ±180° = 左侧 (9点钟方向)
        double angle = std::atan2(dy, dx) * 180.0 / 3.14159265359;
        
        // Knob 的值范围映射到完整的 -90° 到 +270° (360度，从顶部开始)
        // 整个360度圆都可以交互，完整映射到值范围
        
        // 应用偏移角度
        angle -= offsetAngle;
        
        // 标准化到 [-180, 180]
        while (angle > 180) angle -= 360;
        while (angle < -180) angle += 360;
        
        // 定义值范围对应的角度（从顶部-90°开始）
        double startAngle = -90.0;   // 对应 minValue (顶部)
        double endAngle = 270.0;     // 对应 maxValue (顶部)
        
        // 处理跨越180°边界的情况
        double normalizedAngle = angle;
        if(normalizedAngle < startAngle) {
            normalizedAngle += 360.0;  // 转换到正值范围
        }
        
        double totalSweep = 360.0;
        
        // 将整个360度圆线性映射到值范围
        double ratio = (normalizedAngle - startAngle) / totalSweep;
        double newValue = minValue + ratio * (maxValue - minValue);
        
        // 限制和应用步进
        newValue = clamp(newValue);
        if(step > 0) {
            newValue = applyStep(newValue);
        }
        
        // 更新值并触发回调
        if(newValue != value) {
            value = newValue;
            if(onChange) {
                onChange(value);
            }
            // 通知父容器需要重绘
            if(this->parent != nullptr){
                if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
                    p->setDirty();
                }
            }
        }
        
        lastMouseX = msg.x;
        lastMouseY = msg.y;
        return true;
    }
    // 释放鼠标左键停止拖动
    else if(msg.is_left() && msg.is_up()) {
        dragging = false;
        if(mouseOwningFlag == this){
            mouseOwningFlag = nullptr;
        }
    }
    return hovered;
}

// ===================== KnobBuilder Implementation =====================

KnobBuilder& KnobBuilder::setIdentifier(const wstring& id) {
    identifier = id;
    return *this;
}

KnobBuilder& KnobBuilder::setCenter(double x, double y) {
    cx = x;
    cy = y;
    return *this;
}

KnobBuilder& KnobBuilder::setRadius(double r) {
    radius = r;
    return *this;
}

KnobBuilder& KnobBuilder::setRange(double minVal, double maxVal) {
    minValue = minVal;
    maxValue = maxVal;
    return *this;
}

KnobBuilder& KnobBuilder::setStep(double s) {
    step = s;
    return *this;
}

KnobBuilder& KnobBuilder::setValue(double val) {
    value = val;
    return *this;
}

KnobBuilder& KnobBuilder::setColor(color_t fg, color_t bg) {
    fgColor = fg;
    bgColor = bg;
    return *this;
}

KnobBuilder& KnobBuilder::setOffsetAngle(double angle) {
    offsetAngle = angle;
    return *this;
}

KnobBuilder& KnobBuilder::setInnerRange(double innerMinVal, double innerMaxVal) {
    innerMin = innerMinVal;
    innerMax = innerMaxVal;
    return *this;
}

KnobBuilder& KnobBuilder::setShowValue(bool show) {
    showValue = show;
    return *this;
}

KnobBuilder& KnobBuilder::setFontSize(int size) {
    fontSize = size;
    return *this;
}

KnobBuilder& KnobBuilder::setDisabled(bool dis) {
    disabled = dis;
    return *this;
}

KnobBuilder& KnobBuilder::setReadonly(bool ro) {
    readonly = ro;
    return *this;
}

KnobBuilder& KnobBuilder::setScale(double s) {
    scale = s;
    return *this;
}

KnobBuilder& KnobBuilder::setOnChange(std::function<void(double)> callback) {
    onChange = callback;
    return *this;
}

Knob* KnobBuilder::build() {
    Knob* knob = new Knob(cx, cy, radius);
    
    // 应用所有配置
    knob->setRange(minValue, maxValue);
    knob->setStep(step);
    knob->setValue(value);
    knob->setColor(fgColor, bgColor);
    knob->setOffsetAngle(offsetAngle);
    knob->setInnerRange(innerMin, innerMax);
    knob->setShowValue(showValue);
    knob->setFontSize(fontSize);
    knob->setDisabled(disabled);
    knob->setReadonly(readonly);
    knob->setScale(scale);
    
    if(onChange) {
        knob->setOnChange(onChange);
    }
    
    // 注册到全局控件集合
    //widgets.insert(knob);
    
    // 如果有标识符，注册到ID映射
    if(!identifier.empty()) {
        IdToWidget[identifier] = knob;
    }
    
    return knob;
}

std::map<std::wstring,Widget*> IdToWidget;

Widget* getWidgetById(const std::wstring& identifier){
    auto it = IdToWidget.find(identifier);
    if(it != IdToWidget.end()) {
        return it->second;
    }
    return nullptr;
}

void assignOrder(std::vector<Widget*> widgetWithOrder){
    for (Widget* w : widgets) {
        // 被取出了
        widgetBackendRedraw.insert(w);
    }
    for (Widget* w : widgetWithOrder) {
        // 被放入了
        widgetBackendRedraw.erase(w);
    }
    swap(widgets, widgetWithOrder);
    for(auto w : widgets){
        assert(widgetBackendRedraw.find(w) == widgetBackendRedraw.end());
        if(auto p = dynamic_cast<Panel*>(w)){
            p->setDirty();
        }
        if(auto b = dynamic_cast<Box*>(w)){
            b->setDirty();
        }
    }
}

void emplaceOrder(const std::vector<Widget*>& widgetWithOrder){
    widgets.insert(widgets.end(),widgetWithOrder.begin(),widgetWithOrder.end());
    for(auto w : widgets){
        widgetBackendRedraw.erase(w);
    }
    for(auto w : widgets){
        assert(widgetBackendRedraw.find(w) == widgetBackendRedraw.end());
        if(auto p = dynamic_cast<Panel*>(w)){
            p->setDirty();
        }
        if(auto b = dynamic_cast<Box*>(w)){
            b->setDirty();
        }
    }
}

// ============ ScrollBar 实现 ============

ScrollBar::ScrollBar(double barWidth, double barHeight)
    : barWidth_(barWidth), barHeight_(barHeight) {
}

double ScrollBar::getButtonSize(double scale) const {
    return barWidth_;  // 正方形按钮
}

void ScrollBar::getThumbRect(double scale, double& outY, double& outH) const {
    double btnSize = getButtonSize(scale);
    double trackHeight = barHeight_ - btnSize * 2;  // 轨道高度

    if(contentHeight_ <= viewHeight_ || contentHeight_ <= 0) {
        outY = 0;
        outH = trackHeight;
        return;
    }

    // 滑块高度与可见比例成正比，最小高度为20像素
    double ratio = viewHeight_ / contentHeight_;
    outH = trackHeight * ratio;
    if(outH < 20) outH = 20;

    // 滑块位置
    double scrollRange = trackHeight - outH;
    outY = scrollPos_ * scrollRange;
}

void ScrollBar::drawArrow(PIMAGE dst, double centerX, double centerY, double size, bool up, color_t color) {
    // 绘制三角形箭头
    ege_point points[3];
    if(up) {
        points[0] = {static_cast<float>(centerX), static_cast<float>(centerY - size * 0.4)};
        points[1] = {static_cast<float>(centerX - size * 0.4), static_cast<float>(centerY + size * 0.3)};
        points[2] = {static_cast<float>(centerX + size * 0.4), static_cast<float>(centerY + size * 0.3)};
    }
    else {
        points[0] = {static_cast<float>(centerX), static_cast<float>(centerY + size * 0.4)};
        points[1] = {static_cast<float>(centerX - size * 0.4), static_cast<float>(centerY - size * 0.3)};
        points[2] = {static_cast<float>(centerX + size * 0.4), static_cast<float>(centerY - size * 0.3)};
    }
    setfillcolor(color, dst);
    ege_fillpoly(3, points, dst);
}

void ScrollBar::draw(PIMAGE dst, double x, double y, double scale) {
    if(!isNeeded()) return;

    // 长按按钮或轨道时持续滚动（三阶段）
    bool anyPressed = topBtnPressed_ || bottomBtnPressed_ || trackPressed_;
    if(anyPressed) {
        double elapsed = getMs() - pressStartTime_;
        // 阶段1: 0~100ms 初始慢速滚动（首次点击已在handleEvent中设置了targetScrollPos_）
        // 阶段2: 100~400ms 停顿，不滚动
        // 阶段3: 400ms+ 快速持续滚动
        double step = 0;
        if(elapsed > 400) {
            step = 0.012;
        }
        else if(elapsed <= 100) {
            step = 0.004;
        }
        // 100~400ms 停顿期 step=0

        if(step > 0) {
            if(topBtnPressed_) {
                targetScrollPos_ -= step;
            }
            else if(bottomBtnPressed_) {
                targetScrollPos_ += step;
            }
            else if(trackPressed_) {
                targetScrollPos_ += step * trackScrollDir_;
                // 不超过点击位置
                if(trackScrollDir_ > 0 && targetScrollPos_ > trackClickPos_) targetScrollPos_ = trackClickPos_;
                if(trackScrollDir_ < 0 && targetScrollPos_ < trackClickPos_) targetScrollPos_ = trackClickPos_;
            }
            if(targetScrollPos_ < 0) targetScrollPos_ = 0;
            if(targetScrollPos_ > 1.0) targetScrollPos_ = 1.0;
            if(parentPanel_) parentPanel_->setDirty();
        }

        if(parentPanel_ && !btnScrollActive_) {
            parentPanel_->setAlwaysDirty(true);
            btnScrollActive_ = true;
        }
    }
    else if(btnScrollActive_) {
        if(parentPanel_) {
            parentPanel_->setAlwaysDirty(false);
        }
        btnScrollActive_ = false;
    }

    // 平滑滚动：scrollPos_ 向 targetScrollPos_ 插值
    double diff = targetScrollPos_ - scrollPos_;
    if(diff != 0) {
        double lerpFactor = 0.18;  // 插值系数，越大越快到达目标
        double maxScroll = contentHeight_ - viewHeight_;
        if(maxScroll <= 0 || std::abs(diff) * maxScroll < 1.0) {
            scrollPos_ = targetScrollPos_;  // 接近目标时直接到达（像素级精度）
        }
        else {
            scrollPos_ += diff * lerpFactor;
        }
        if(parentPanel_) parentPanel_->setDirty();
        // 动画进行中，保持持续重绘
        if(!smoothScrollActive_ && parentPanel_) {
            parentPanel_->setAlwaysDirty(true);
            smoothScrollActive_ = true;
        }
    }
    else if(smoothScrollActive_) {
        if(parentPanel_) {
            parentPanel_->setAlwaysDirty(false);
        }
        smoothScrollActive_ = false;
    }

    if(BackendFlag) {
        return;
    }

    double btnSize = getButtonSize(scale);
    double w = barWidth_;

    // 滚动条背景
    setfillcolor(EGEARGB(255, 240, 240, 240), dst);
    ege_fillrect(x, y, w, barHeight_, dst);

    // === 顶部按钮 ===
    if(topBtnPressed_) {
        setfillcolor(EGEARGB(255, 96, 96, 96), dst);
    }
    else if(topBtnHovered_) {
        setfillcolor(EGEARGB(255, 218, 218, 218), dst);
    }
    else {
        setfillcolor(EGEARGB(255, 240, 240, 240), dst);
    }
    ege_fillrect(x, y, w, btnSize, dst);

    // 顶部按钮阴影效果
    if(topBtnHovered_ && !topBtnPressed_) {
        setfillcolor(EGEARGB(30, 0, 0, 0), dst);
        ege_fillrect(x, y, w, btnSize, dst);
    }

    // 顶部三角
    color_t arrowColor = topBtnPressed_ ? EGEARGB(255, 255, 255, 255) : EGEARGB(255, 96, 96, 96);
    drawArrow(dst, x + w / 2, y + btnSize / 2, w * 0.6, true, arrowColor);

    // === 底部按钮 ===
    double bottomBtnY = y + barHeight_ - btnSize;
    if(bottomBtnPressed_) {
        setfillcolor(EGEARGB(255, 96, 96, 96), dst);
    }
    else if(bottomBtnHovered_) {
        setfillcolor(EGEARGB(255, 218, 218, 218), dst);
    }
    else {
        setfillcolor(EGEARGB(255, 240, 240, 240), dst);
    }
    ege_fillrect(x, bottomBtnY, w, btnSize, dst);

    // 底部按钮阴影效果
    if(bottomBtnHovered_ && !bottomBtnPressed_) {
        setfillcolor(EGEARGB(30, 0, 0, 0), dst);
        ege_fillrect(x, bottomBtnY, w, btnSize, dst);
    }

    // 底部三角
    arrowColor = bottomBtnPressed_ ? EGEARGB(255, 255, 255, 255) : EGEARGB(255, 96, 96, 96);
    drawArrow(dst, x + w / 2, bottomBtnY + btnSize / 2, w * 0.6, false, arrowColor);

    // === 轨道区域 ===
    double trackY = y + btnSize;
    double trackHeight = barHeight_ - btnSize * 2;
    setfillcolor(EGEARGB(255, 234, 234, 234), dst);
    ege_fillrect(x, trackY, w, trackHeight, dst);

    // === 滑块 ===
    double thumbY, thumbH;
    getThumbRect(scale, thumbY, thumbH);

    if(thumbPressed_) {
        setfillcolor(EGEARGB(255, 136, 136, 136), dst);
    }
    else if(thumbHovered_) {
        setfillcolor(EGEARGB(255, 168, 168, 168), dst);
    }
    else {
        setfillcolor(EGEARGB(255, 205, 205, 205), dst);
    }
    ege_fillrect(x + 1, trackY + thumbY, w - 2, thumbH, dst);

    // 滑块阴影效果
    if(thumbHovered_ && !thumbPressed_) {
        setfillcolor(EGEARGB(25, 0, 0, 0), dst);
        ege_fillrect(x + 1, trackY + thumbY, w - 2, thumbH, dst);
    }
    else if(thumbPressed_) {
        setfillcolor(EGEARGB(40, 0, 0, 0), dst);
        ege_fillrect(x + 1, trackY + thumbY, w - 2, thumbH, dst);
    }

    // 轨道分隔线
    setlinecolor(EGEARGB(255, 218, 218, 218), dst);
    ege_line(x, y + btnSize, x + w, y + btnSize, dst);
    ege_line(x, bottomBtnY, x + w, bottomBtnY, dst);

    // 左边界线（分隔滚动条与面板内容）
    setlinecolor(EGEARGB(255, 210, 210, 210), dst);
    ege_line(x, y, x, y + barHeight_, dst);
}

bool ScrollBar::handleEvent(const mouse_msg& msg, double scrollBarLeft, double scrollBarTop, double scale) {
    if(!isNeeded()) return false;

    double mx = msg.x;
    double my = msg.y;
    double w = barWidth_;
    double btnSize = getButtonSize(scale);

    // 检查鼠标是否在滚动条区域内
    bool inScrollBar = mx >= scrollBarLeft && mx <= scrollBarLeft + w &&
                       my >= scrollBarTop && my <= scrollBarTop + barHeight_;

    // 顶部按钮区域
    bool inTopBtn = mx >= scrollBarLeft && mx <= scrollBarLeft + w &&
                    my >= scrollBarTop && my <= scrollBarTop + btnSize;

    // 底部按钮区域
    double bottomBtnY = scrollBarTop + barHeight_ - btnSize;
    bool inBottomBtn = mx >= scrollBarLeft && mx <= scrollBarLeft + w &&
                       my >= bottomBtnY && my <= bottomBtnY + btnSize;

    // 处理其它控件焦点
    if(msg.is_left() && msg.is_up()){
        if(mouseOwningFlag != nullptr){
            mouseOwningFlag->releaseMouseOwningFlag(msg);
        }
    }

    if(mouseOwningFlag != nullptr){
        // 如果当前事件被其他控件捕获，则不处理
        return inScrollBar;
    }

    // 滑块区域
    double trackTop = scrollBarTop + btnSize;
    double thumbY, thumbH;
    getThumbRect(scale, thumbY, thumbH);
    double absoluteThumbTop = trackTop + thumbY;
    bool inThumb = mx >= scrollBarLeft && mx <= scrollBarLeft + w &&
                   my >= absoluteThumbTop && my <= absoluteThumbTop + thumbH;

    // 更新悬停状态，并检测是否发生变化以触发重绘
    bool oldTopHover = topBtnHovered_, oldBottomHover = bottomBtnHovered_, oldThumbHover = thumbHovered_;
    topBtnHovered_ = inTopBtn && !thumbDragging_;
    bottomBtnHovered_ = inBottomBtn && !thumbDragging_;
    thumbHovered_ = inThumb && !thumbDragging_;
    if(thumbDragging_) thumbHovered_ = true;  // 拖动中始终显示悬停
    bool hoverChanged = (topBtnHovered_ != oldTopHover) || (bottomBtnHovered_ != oldBottomHover) || (thumbHovered_ != oldThumbHover);
    if(hoverChanged && parentPanel_) parentPanel_->setDirty();

    // 处理事件
    if(msg.is_left() && msg.is_down()) {
        if(inTopBtn) {
            topBtnPressed_ = true;
            pressStartTime_ = getMs();
            double step = 0.05;
            targetScrollPos_ = scrollPos_ - step;
            if(targetScrollPos_ < 0) targetScrollPos_ = 0;
            if(parentPanel_) parentPanel_->setDirty();
            return true;
        }
        if(inBottomBtn) {
            bottomBtnPressed_ = true;
            pressStartTime_ = getMs();
            double step = 0.05;
            targetScrollPos_ = scrollPos_ + step;
            if(targetScrollPos_ > 1.0) targetScrollPos_ = 1.0;
            if(parentPanel_) parentPanel_->setDirty();
            return true;
        }
        if(inThumb) {
            thumbPressed_ = true;
            thumbDragging_ = true;
            dragOffset_ = my - absoluteThumbTop;
            mouseOwningFlag = nullptr;  // 滚动条自行管理拖动
            if(parentPanel_) parentPanel_->setDirty();
            return true;
        }
        // 点击轨道空白区域，开始持续滚动
        if(inScrollBar && !inTopBtn && !inBottomBtn) {
            trackPressed_ = true;
            pressStartTime_ = getMs();
            // 判断点击在滑块上方还是下方
            trackScrollDir_ = (my < absoluteThumbTop) ? -1 : 1;
            // 计算点击位置对应的滚动比例（滑块中心对齐到点击位置）
            double trackHeight = barHeight_ - btnSize * 2;
            double ratio = viewHeight_ / contentHeight_;
            double thumbHeight = trackHeight * ratio;
            if(thumbHeight < 20) thumbHeight = 20;
            double scrollRange = trackHeight - thumbHeight;
            if(scrollRange > 0) {
                trackClickPos_ = (my - trackTop - thumbHeight / 2) / scrollRange;
                if(trackClickPos_ < 0) trackClickPos_ = 0;
                if(trackClickPos_ > 1.0) trackClickPos_ = 1.0;
            }
            // 首次点击设置目标位置
            double step = 0.05;
            targetScrollPos_ = scrollPos_ + step * trackScrollDir_;
            if(targetScrollPos_ < 0) targetScrollPos_ = 0;
            if(targetScrollPos_ > 1.0) targetScrollPos_ = 1.0;
            // 不超过点击位置
            if(trackScrollDir_ > 0 && targetScrollPos_ > trackClickPos_) targetScrollPos_ = trackClickPos_;
            if(trackScrollDir_ < 0 && targetScrollPos_ < trackClickPos_) targetScrollPos_ = trackClickPos_;
            if(parentPanel_) parentPanel_->setDirty();
            return true;
        }
    }
    else if(msg.is_left() && msg.is_up()) {
        topBtnPressed_ = false;
        bottomBtnPressed_ = false;
        trackPressed_ = false;
        trackScrollDir_ = 0;
        thumbPressed_ = false;
        thumbDragging_ = false;
        if(parentPanel_) parentPanel_->setDirty();
        return inScrollBar;
    }
    else if(msg.is_move()) {
        if(thumbDragging_) {
            double trackHeight = barHeight_ - btnSize * 2;
            double ratio = viewHeight_ / contentHeight_;
            double thumbHeight = trackHeight * ratio;
            if(thumbHeight < 20) thumbHeight = 20;
            double scrollRange = trackHeight - thumbHeight;
            if(scrollRange > 0) {
                double newThumbTop = my - dragOffset_ - trackTop;
                scrollPos_ = newThumbTop / scrollRange;
                if(scrollPos_ < 0) scrollPos_ = 0;
                if(scrollPos_ > 1.0) scrollPos_ = 1.0;
                targetScrollPos_ = scrollPos_;
            }
            if(parentPanel_) parentPanel_->setDirty();
            return true;
        }
        if(parentPanel_ && inScrollBar) parentPanel_->setDirty();
        return inScrollBar;
    }
    else if(msg.is_wheel()) {
        double maxScroll = contentHeight_ - viewHeight_;
        if(maxScroll > 0) {
            double fixedPixels = 60.0;  // 每次滚轮固定滚动60像素
            double step = fixedPixels / maxScroll * (msg.wheel / -120.0);
            targetScrollPos_ += step;
            if(targetScrollPos_ < 0) targetScrollPos_ = 0;
            if(targetScrollPos_ > 1.0) targetScrollPos_ = 1.0;
            if(parentPanel_) parentPanel_->setDirty();
        }
        return true;
    }

    return inScrollBar;
}

void ScrollBar::setContentRange(double contentHeight, double viewHeight) {
    contentHeight_ = contentHeight;
    viewHeight_ = viewHeight;
}

void ScrollBar::setScrollPosition(double pos) {
    if(pos < 0) pos = 0;
    if(pos > 1.0) pos = 1.0;
    scrollPos_ = pos;
    targetScrollPos_ = pos;
}

double ScrollBar::getScrollPosition() const {
    return scrollPos_;
}

double ScrollBar::getWidth() const {
    return barWidth_;
}

void ScrollBar::setSize(double w, double h) {
    barWidth_ = w;
    barHeight_ = h;
}

bool ScrollBar::isNeeded() const {
    return contentHeight_ > viewHeight_ && contentHeight_ > 0;
}

void ScrollBar::setParentPanel(Panel* p) {
    parentPanel_ = p;
}

// ============ Panel ScrollBar 集成 ============

void Panel::enableScrollBar(bool enable, double scrollBarWidth) {
    scrollBarEnabled_ = enable;
    if(enable && !scrollBar_) {
        scrollBar_ = new ScrollBar(scrollBarWidth, height);
        scrollBar_->setParentPanel(this);
    }
    else if(!enable && scrollBar_) {
        delete scrollBar_;
        scrollBar_ = nullptr;
    }
    needRedraw = true;
}

ScrollBar* Panel::getScrollBar() {
    return scrollBar_;
}

double Panel::getScrollOffset() const {
    return scrollOffset_;
}

PanelBuilder& PanelBuilder::setScrollBar(bool enable, double w) {
    scrollBarEnabled = enable;
    scrollBarWidth = w;
    return *this;
}

// ============ Box 实现 ============

Box::Box(double cx, double cy, double w, double h) 
    : Panel(cx, cy, w, h, 0, EGEARGB(0, 0, 0, 0)) {  // 透明背景，无圆角
    // 创建并设置内置的FlexLayout
    auto flexLayout = std::make_shared<FlexLayout>();
    setLayout(flexLayout);
}

Box::~Box(){
    // Panel的析构函数会处理清理
}

void Box::draw(PIMAGE dst, double x, double y) {
    double left = x - width / 2 - 4;
    double top = y - height / 2 - 4;
    double layerWidth = this->width + 8;
    double layerHeight = this->height + 8;

    if(!needRedraw && !needRedrawAlways){
        if(!BackendFlag) {
            putimage_withalpha(dst,layer,left,top);
        }
        return;
    }

    // 计算滚动偏移并进行平滑插值（与Panel/ScrollBar实现保持一致，但Box无滚动条）
    double layoutScrollOffset = 0;
    if(layout) {
        LayoutResult extentResult = layout->apply(*this, 0);
        double contentH = extentResult.contentMaxY - extentResult.contentMinY;
        double viewH = height / (scale > 0 ? scale : 1.0);
        double maxScroll = contentH - viewH;

        if(maxScroll > 0) {
            // 平滑滚动：boxScrollPos_ 向 targetBoxScrollPos_ 插值（同ScrollBar逻辑）
            double diff = targetBoxScrollPos_ - boxScrollPos_;
            if(diff != 0) {
                double lerpFactor = 0.18;
                if(std::abs(diff) * maxScroll < 1.0) {
                    boxScrollPos_ = targetBoxScrollPos_;  // 接近目标时直接到达（像素级精度）
                }
                else {
                    boxScrollPos_ += diff * lerpFactor;
                }
                this->setDirty();
                if(!smoothScrollActive_) {
                    this->setAlwaysDirty(true);
                    smoothScrollActive_ = true;
                }
            }
            else if(smoothScrollActive_) {
                this->setAlwaysDirty(false);
                smoothScrollActive_ = false;
            }
            layoutScrollOffset = boxScrollPos_ * maxScroll;
        }
        else {
            // 内容未超出视口，重置滚动状态
            boxScrollPos_ = 0;
            targetBoxScrollPos_ = 0;
            if(smoothScrollActive_) {
                this->setAlwaysDirty(false);
                smoothScrollActive_ = false;
            }
        }
    }

    // 应用布局（带滚动偏移）
    if(layout) layout->apply(*this, layoutScrollOffset);

    // 使用真正的透明色(PRGB32模式下alpha=0时RGB也应为0)
    setbkcolor_f(EGEARGB(0, 0, 0, 0), layer);
    cleardevice(layer);

    // Box不绘制背景，直接绘制子控件
    // 绘制子控件 - 子控件相对于自己的中心缩放，位置不随scale变化
    if(scaleChanged) PanelScaleChanged = true;

    // 收窄全局可绘制区域到本Box范围
    double oldDrawingLeft = globalDrawingLeft, oldDrawingRight = globalDrawingRight;
    double oldDrawingTop = globalDrawingTop, oldDrawingBottom = globalDrawingBottom;
    globalDrawingLeft = std::max(globalDrawingLeft, cx - width / 2);
    globalDrawingRight = std::min(globalDrawingRight, cx + width / 2);
    globalDrawingTop = std::max(globalDrawingTop, cy - height / 2);
    globalDrawingBottom = std::min(globalDrawingBottom, cy + height / 2);

    double savedAbsPosX = absolutPosDeltaX;
    double savedAbsPosY = absolutPosDeltaY;
    needRedraw = false;
    for (int i = children.size() - 1; i >= 0; -- i) {
        double childX = layerWidth / 2 + childOffsets[i].x;
        double childY = layerHeight / 2 + childOffsets[i].y;
        // 累积父容器的屏幕偏移量，确保嵌套容器（如Panel>Box>InputBox）中的IME位置正确
        absolutPosDeltaX = savedAbsPosX + left;
        absolutPosDeltaY = savedAbsPosY + top;
        children[i]->setPosition(cx + childOffsets[i].x, cy + childOffsets[i].y);

        // 检查子控件是否在可绘制区域内，或有正在进行的动画需要继续更新
        double childCX = cx + childOffsets[i].x;
        double childCY = cy + childOffsets[i].y;
        double halfW = children[i]->getWidth() / 2.0;
        double halfH = children[i]->getHeight() / 2.0;
        bool withinBounds = (childCX + halfW > globalDrawingLeft) &&
                            (childCX - halfW < globalDrawingRight) &&
                            (childCY + halfH > globalDrawingTop) &&
                            (childCY - halfH < globalDrawingBottom);
        if(withinBounds || children[i]->getDrawingState() != 0) {
            children[i]->draw(layer, childX, childY);
        }

        absolutPosDeltaX = savedAbsPosX;
        absolutPosDeltaY = savedAbsPosY;
    }

    
    if(boundingBoxEnabled) {
        // 绘制边框
        setlinecolor(RED, layer);
        setcolor(RED, layer);
        setlinestyle(PS_SOLID, 0U,1,layer);
        ege_line(4, 4, layerWidth - 4, 4, layer);
        ege_line(layerWidth - 4, 4, layerWidth - 4, layerHeight - 4, layer);
        ege_line(layerWidth - 4, layerHeight - 4, 4, layerHeight - 4, layer);
        ege_line(4, layerHeight - 4, 4, 4, layer);
        // 在四角绘制加粗一点点的线
        setlinecolor(GREEN, layer);
        setlinestyle(PS_SOLID, 0U,4,layer);
        ege_line(4, 4, 14, 4, layer);
        ege_line(4, 4, 4, 14, layer);
        ege_line(layerWidth - 14, 4, layerWidth - 4, 4, layer);
        ege_line(layerWidth - 4, 4, layerWidth - 4, 14, layer);
        ege_line(4, layerHeight - 14, 4, layerHeight - 4, layer);
        ege_line(4, layerHeight - 4, 14, layerHeight - 4, layer);
        ege_line(layerWidth - 14, layerHeight - 4, layerWidth - 4, layerHeight - 4, layer);
        ege_line(layerWidth - 4, layerHeight - 4, layerWidth - 4, layerHeight - 14, layer);
    }

    // 恢复全局可绘制区域
    globalDrawingLeft = oldDrawingLeft;
    globalDrawingRight = oldDrawingRight;
    globalDrawingTop = oldDrawingTop;
    globalDrawingBottom = oldDrawingBottom;

    PanelScaleChanged = false;
    scaleChanged = false;
    
    // 粘贴到主窗口
    if(!BackendFlag) {
        putimage_withalpha(dst,layer,left,top);
    }
}

void Box::draw() {
    draw(nullptr, cx, cy);
}

void Box::setScale(double s){
    if(sgn(s - scale) == 0) return;
    scaleChanged = true;
    
    // Box特殊缩放行为：不缩放Box自身的宽高，只缩放子控件
    // 子控件相对于自己的中心缩放，位置保持不变
    scale = s;
    for (size_t i = 0; i < children.size(); ++i) {
        children[i]->setScale(s);
        children[i]->setPosition(cx + childOffsets[i].x, cy + childOffsets[i].y);
    }

    needRedraw = true;
    if(this->parent != nullptr){
        if(Panel* p = dynamic_cast<Panel*>(this->parent)) {
            p->setDirty();
        }
    }
}

bool Box::handleEvent(const mouse_msg& msg) {
    // 滚轮事件：无滚动条的平滑滚动（与Panel/ScrollBar实现保持一致）
    if(msg.is_wheel() && isInside(msg.x, msg.y)) {
        if(layout) {
            LayoutResult extentResult = layout->apply(*this, 0);
            double contentH = extentResult.contentMaxY - extentResult.contentMinY;
            double viewH = height / (scale > 0 ? scale : 1.0);
            double maxScroll = contentH - viewH;
            if(maxScroll > 0) {
                double fixedPixels = 60.0;  // 每次滚轮固定滚动60像素（同ScrollBar）
                double step = fixedPixels / maxScroll * (msg.wheel / -120.0);
                targetBoxScrollPos_ += step;
                if(targetBoxScrollPos_ < 0) targetBoxScrollPos_ = 0;
                if(targetBoxScrollPos_ > 1.0) targetBoxScrollPos_ = 1.0;
                this->setDirty();
            }
        }
        return true;
    }
    return Panel::handleEvent(msg);
}

void Box::enableBoundingBox() {
    boundingBoxEnabled = true;
    needRedraw = true;
}

void Box::disableBoundingBox() {
    boundingBoxEnabled = false;
    needRedraw = true;
}

void Box::reset(){
    for(Widget* w : children){
        w->reset();
        w->setNeedRedraw(true);
    }
    setDirty();
}

// ============ BoxBuilder 实现 ============

BoxBuilder& BoxBuilder::setIdentifier(const std::wstring& id) {
    identifier = id;
    return *this;
}

BoxBuilder& BoxBuilder::setCenter(double x, double y) {
    cx = x; 
    cy = y;
    return *this;
}

BoxBuilder& BoxBuilder::setSize(double w, double h) {
    width = w; 
    height = h;
    return *this;
}

BoxBuilder& BoxBuilder::setScale(double s) {
    scale = s;
    return *this;
}

BoxBuilder& BoxBuilder::setDirection(LayoutDirection dir) {
    direction = dir;
    return *this;
}

BoxBuilder& BoxBuilder::setAlign(LayoutAlign a) {
    align = a;
    return *this;
}

BoxBuilder& BoxBuilder::setSpacing(double s) {
    spacing = s;
    return *this;
}

BoxBuilder& BoxBuilder::setPadding(double p) {
    padding = p;
    return *this;
}

BoxBuilder& BoxBuilder::addChild(Widget* child) {
    children.push_back(child);
    return *this;
}

BoxBuilder& BoxBuilder::addChild(const std::vector<Widget*>& childList) {
    children.insert(children.end(), childList.begin(), childList.end());
    return *this;
}

Box* BoxBuilder::build() {
    auto box = new Box(cx, cy, width, height);
    box->setScale(scale);
    
    // 配置内置的FlexLayout
    auto flexLayout = std::dynamic_pointer_cast<FlexLayout>(box->getLayout());
    if(flexLayout) {
        flexLayout->setDirection(direction);
        flexLayout->setAlign(align);
        flexLayout->setSpacing(spacing);
        flexLayout->setPadding(padding);
    }
    
    // 添加子控件
    for (auto* child : children) {
        box->addChild(child, 0, 0);  // 初始偏移为0，由layout计算
    }
    
    if(!identifier.empty()) {
        IdToWidget[identifier] = box;
    }
    
    return box;
}
