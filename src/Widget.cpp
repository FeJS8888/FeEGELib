#include "Widget.h"

using namespace FeEGE;
Widget* mouseOwningFlag = nullptr;
Widget* focusingWidget = nullptr;

vector<Widget*> widgets;
double absolutPosDeltaX = 0,absolutPosDeltaY = 0;
bool PanelScaleChanged = false;

double Widget::getWidth(){
    return width;
}
double Widget::getHeight(){
    return height;
}

void Widget::deleteFocus(){

}

Widget::~Widget() {
    // 从全局widgets集合中移除
    
    // 从IdToWidget映射中移除所有指向this的条目
    for (auto it = IdToWidget.begin(); it != IdToWidget.end(); ) {
        if (it->second == this) {
            it = IdToWidget.erase(it);
        } else {
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
    layer = newimage(w,h);
    maskLayer = newimage(w,h);
	ege_enable_aa(true,layer);
    ege_enable_aa(true,maskLayer);
	
    // 遮罩使用不透明颜色：黑色背景(隐藏)和白色填充(显示)
    // 这在PRGB32模式下更可靠，因为它依赖RGB值而非alpha值进行遮罩
	setbkcolor_f(EGEARGB(255, 0, 0, 0), maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0, 0, width - 0.5, height - 0.5, radius, radius, radius, radius, maskLayer);
}

void Panel::addChild(Widget* child, double offsetX, double offsetY) {
    children.push_back(child);
    childOffsets.push_back(Position{ offsetX, offsetY });
    child->is_global = false;
}

void Panel::draw() {
    draw(nullptr,cx,cy);
}

void Panel::draw(PIMAGE dst, double x, double y) {
    if (layout) layout->apply(*this);  // 自动计算子控件位置
    
    double left = x - width / 2;
    double top = y - height / 2;
    
    // 总是清空并重绘（子控件可能有动态内容）
    // 注意：子控件（如Button, InputBox）内部有自己的缓存机制来避免不必要的工作
    // 使用真正的透明色(PRGB32模式下alpha=0时RGB也应为0)
    setbkcolor_f(EGEARGB(0, 0, 0, 0), layer);
    cleardevice(layer);

    // 绘制自身背景（圆角矩形）
    setfillcolor(bgColor, layer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, layer);

    // 绘制子控件
    if(scaleChanged) PanelScaleChanged = true;
    for (size_t i = 0; i < children.size(); ++i) {
        double childX = width / 2 + childOffsets[i].x * scale;
        double childY = height / 2 + childOffsets[i].y * scale;
        absolutPosDeltaX = left;
        absolutPosDeltaY = top;
        children[i]->setPosition(cx + childOffsets[i].x * scale,cy + childOffsets[i].y * scale);
        children[i]->draw(layer, childX, childY);
        absolutPosDeltaX = 0;
        absolutPosDeltaY = 0;
    }
    PanelScaleChanged = false;
    scaleChanged = false;
    // 粘贴到主窗口
    putimage_alphafilter(dst, layer, left, top, maskLayer, 0, 0, -1, -1);
}

Panel::~Panel(){
	if (layer) delimage(layer);
    if (maskLayer) delimage(maskLayer);
}

void Panel::setPosition(double x,double y){
	cx = x;
	cy = y;
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
	
    if(maskLayer) delimage(maskLayer);
    maskLayer = newimage(width,height);
    if(layer) delimage(layer);
    layer = newimage(width,height);
    ege_enable_aa(true,layer);
    ege_enable_aa(true,maskLayer);

    // 遮罩使用不透明颜色：黑色背景(隐藏)和白色填充(显示)
	setbkcolor_f(EGEARGB(255, 0, 0, 0), maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0, 0, width - 0.5, height - 0.5, radius, radius, radius, radius, maskLayer);
}

double Panel::getScale(){
	return scale;
}

namespace {
// Helper function to check if a widget is a descendant of a list of children
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
} // anonymous namespace

bool Panel::handleEvent(const mouse_msg& msg){
    int mx = msg.x,my = msg.y;
    double left = cx - width / 2;
    double top = cy - height / 2;
    bool isin = mx >= left && mx <= left + width && my >= top && my <= top + height;
    if(!isin) {
        // When clicking outside the Panel, remove focus from any descendant widget that has focus
        if(msg.is_left() && msg.is_down() && focusingWidget != nullptr) {
            if(isDescendant(focusingWidget, children)) {
                focusingWidget->deleteFocus();
            }
        }
        return false;
    }
	for(Widget* w : children){
        bool state = w->handleEvent(msg);
        if(state) return true;
    }
    if(msg.is_left() && msg.is_down()){
        mouseOwningFlag = this;
        return true;
    }
    else if(msg.is_left() && msg.is_up()){
        if(mouseOwningFlag == this){
            mouseOwningFlag = nullptr;
        }
        return false;
    }
    return false;
}

void Panel::setSize(double w,double h){
    origin_width = width = w;
    origin_height = height = h;
    if(layer) delimage(layer);
    if(maskLayer) delimage(maskLayer);
    layer = newimage(width,height);
    maskLayer = newimage(width,height);
    ege_enable_aa(true,layer);
    ege_enable_aa(true,maskLayer);
	
    // 遮罩使用不透明颜色：黑色背景(隐藏)和白色填充(显示)
	setbkcolor_f(EGEARGB(255, 0, 0, 0), maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB((int)alpha, 255, 255, 255), maskLayer);
    ege_fillroundrect(0.25, 0.25, width - 0.5, height - 0.5, radius, radius, radius, radius, maskLayer);
}

void Panel::clearChildren(){
    children.clear();
    childOffsets.clear();
}

void Panel::setAlpha(double a) {
    alpha = clamp(a, 0, 255);
    // 遮罩使用不透明颜色：黑色背景(隐藏)和白色填充(显示)
    setbkcolor_f(EGEARGB(255, 0, 0, 0), maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB((int)alpha, 255, 255, 255), maskLayer);
    ege_fillroundrect(0.25, 0.25, width - 0.5, height - 0.5, radius, radius, radius, radius, maskLayer);
}

std::vector<Widget*>& Panel::getChildren() { 
    return children; 
}

void Panel::setChildrenOffset(int index,Position pos){
    if (index >= 0 && index < static_cast<int>(childOffsets.size())) {
        childOffsets[index] = pos;
    }
}

// Ripple 结构体实现
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
    if (layout) panel->setLayout(layout);
    for(size_t i = 0;i < children.size();++ i){
        panel->addChild(children[i],childOffsets[i].x,childOffsets[i].y);
    }
    return panel;
}

// ButtonBuilder 实现
Ripple::Ripple(int _x, int _y, int _r, int _life,Widget* _p,int _c)
    : x(_x), y(_y), maxRadius(_r), life(_life), parent(_p), counter(_c) {}

bool Ripple::alive() const {
    if (auto btn = dynamic_cast<Button*>(parent)){
        return (btn->getClickState() && (btn->getMCounter() == counter)) || (age < life);
    }
    if (auto ib = dynamic_cast<InputBox*>(parent))
        return (ib->getClickState() && (ib->getMCounter() == counter)) || (age < life);
    return age < life;
}

void Ripple::update() {
    // --- 空指针保护 ---
    if (!parent) {
        age++;
        return;
    }

    // --- 动态类型安全检测 ---
    if (age >= life * 0.75) {
        if (auto btn = dynamic_cast<Button*>(parent)) {
            // cout<<"Ripple Update: Button Click State = " << btn->getClickState() << ", Counter = " << btn->getMCounter() << ", Ripple Counter = " << counter << endl;
            if (btn->getClickState() && (btn->getMCounter() == counter)) return;
        } 
        else if (auto ib = dynamic_cast<InputBox*>(parent)) {
            if (ib->getClickState() && (ib->getMCounter() == counter)) return;
        }
    }

    // --- 正常递增 ---
    age++;
}

void Ripple::draw(PIMAGE dst) const {
    double progress = (double)age / life;
    double r = maxRadius * progress;
    int alpha = static_cast<int>(120 * std::cos(progress * PI / 2));
    setfillcolor(EGEARGB(alpha, 30, 30, 30), dst);
    ege_fillellipse(x - r, y - r, r * 2, r * 2, dst);
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

    btnLayer = newimage(width, height);
    bgLayer = newimage(width, height);
    maskLayer = newimage(width, height);
    ege_enable_aa(true, btnLayer);
    ege_enable_aa(true, maskLayer);
    ege_enable_aa(true, bgLayer);
    
    // 遮罩使用不透明颜色：黑色背景(隐藏)和白色填充(显示)
    setbkcolor_f(EGEARGB(255, 0, 0, 0), maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGERGBA(255,255,255,255), maskLayer);
    ege_fillroundrect(0.25,0.25,width - 0.5,height - 0.5, radius, radius, radius, radius, maskLayer);
}

Button::~Button() {
    if (btnLayer) delimage(btnLayer);
    if (maskLayer) delimage(maskLayer);
    if (bgLayer) delimage(bgLayer);
}

void Button::draw(PIMAGE dst,double x,double y){
    double left = x - width / 2;
    double top = y - height / 2;
    if(!ripples.size() && !needRedraw){
        putimage_withalpha(dst,bgLayer,left,top);
        return;
    }
    // 使用真正的透明色(PRGB32模式下alpha=0时RGB也应为0)
    setbkcolor_f(EGEARGB(0, 0, 0, 0), btnLayer);
    cleardevice(btnLayer);

    // 优化：只绘制一次背景到btnLayer，稍后复制到bgLayer
    setfillcolor(color, btnLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, btnLayer);
                 
    if(icon != nullptr){
	    double iconW = getwidth(icon) * scale * iconSize / 100;
	    double iconH = getheight(icon) * scale * iconSize / 100;
	    double iconX = width / 2 - iconW / 2;
	    double iconY = height / 2 - iconH / 2;
	    putimage_alphablend(btnLayer,icon,iconX,iconY,iconW,iconH,255,0, 0,getwidth(icon), getwidth(icon),true);
	}
    
    // 更新并绘制 ripples
    for (auto& r : ripples) {
        r.update();
        r.draw(btnLayer);
    }
    // 优化：使用C++20 std::erase_if替代erase-remove惯用法
    std::erase_if(ripples, [](const Ripple& r) { return !r.alive(); });

    ege_setfont(23 * scale, L"宋体", btnLayer);
    float w,h;
    measuretext(content.c_str(),&w,&h,btnLayer);
    
    // 按钮文字
    setbkmode(TRANSPARENT, btnLayer);
    settextcolor(BLACK, btnLayer);
    ege_outtextxy(width / 2 - w / 2, 
                 height / 2 - h / 2, 
                 content.c_str(), btnLayer);
    
    // 应用遮罩绘制
    // 使用真正的透明色(PRGB32模式下alpha=0时RGB也应为0)
    setbkcolor_f(EGEARGB(0, 0, 0, 0), bgLayer);
    cleardevice(bgLayer);
    putimage_alphafilter(bgLayer, btnLayer, 0, 0, maskLayer, 0, 0, -1, -1);
    putimage_withalpha(dst,bgLayer,left,top);
    needRedraw = false;
}

void Button::draw(){
    draw(nullptr,cx,cy);
}

bool Button::handleEvent(const mouse_msg& msg) {
    bool inside = isInside(msg.x, msg.y);
    if (msg.is_left() && msg.is_down() && inside) {
        int localX = msg.x - left;
        int localY = msg.y - top;
        m_counter++;
        ripples.emplace_back(localX, localY, 4.00f / 3.00f * std::sqrt(height * height + width * width), 70,dynamic_cast<Widget*>(this),m_counter);
        needRedraw = true;
        m_clicking = true;
        mouseOwningFlag = this;
        return true;
    }
    else if(msg.is_left() && msg.is_up()){
        if(inside && on_click_event != nullptr) on_click_event();
        m_clicking = false;
        mouseOwningFlag = nullptr;
        return false;
    }
    return false;
}

bool Button::isInside(double x, double y) const {
    // 转换为按钮内部坐标系
    double localX = x - left;
    double localY = y - top;

    // 先检查是否在按钮矩形框外
    if (localX < 0 || localX >= width || localY < 0 || localY >= height)
        return false;

    // 中心矩形区（不考虑圆角）直接返回 true
    if (localX >= radius && localX < width - radius)
        return true;
    if (localY >= radius && localY < height - radius)
        return true;

    // 检查四个圆角区域
    int dx, dy;
    // 左上角
    if (localX < radius && localY < radius) {
        dx = radius - localX;
        dy = radius - localY;
        return dx * dx + dy * dy <= radius * radius;
    }
    // 右上角
    if (localX >= width - radius && localY < radius) {
        dx = localX - (width - radius);
        dy = radius - localY;
        return dx * dx + dy * dy <= radius * radius;
    }
    // 左下角
    if (localX < radius && localY >= height - radius) {
        dx = radius - localX;
        dy = localY - (height - radius);
        return dx * dx + dy * dy <= radius * radius;
    }
    // 右下角
    if (localX >= width - radius && localY >= height - radius) {
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
    needRedraw = true;
}

std::wstring Button::getContent(){
    return content;
}

void Button::setPosition(double x,double y){
    if(sgn(left - x + width / 2) == 0 && sgn(top - y + height / 2) == 0) return;
    left = x - width / 2;
	top = y - height / 2;
    needRedraw = true;
}

void Button::setScale(double s){
    if(sgn(scale - s) == 0) return;
	width = origin_width * s;
    height = origin_height * s;
    radius = origin_radius * s;
    scale = s;
    left = cx - width / 2;
    top = cy - height / 2;
    // 遮罩
    if(maskLayer) delimage(maskLayer);
    maskLayer = newimage(width,height);
    if(btnLayer) delimage(btnLayer);
    btnLayer = newimage(width,height);
    if(bgLayer) delimage(bgLayer);
    bgLayer = newimage(width,height);
    ege_enable_aa(true,bgLayer);
    ege_enable_aa(true,maskLayer);
    ege_enable_aa(true,btnLayer);
    // 遮罩使用不透明颜色：黑色背景(隐藏)和白色填充(显示)
    setbkcolor_f(EGEARGB(255, 0, 0, 0), maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0,0,width,height, radius, radius, radius, radius, maskLayer);
    needRedraw = true;
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
}

bool Button::getClickState(){
    return m_clicking;
}

int Button::getMCounter(){
    return m_counter;
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

// InputBoxBuilder 实现
InputBox::InputBox(double cx, double cy, double w, double h, double r) {
    this->cx = cx;
    this->cy = cy;
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = r;
    left = cx - width / 2;
    top = cy - height / 2;
    btnLayer = newimage(width, height);
    maskLayer = newimage(width, height);
    bgLayer = newimage(width,height);
    ege_enable_aa(true, btnLayer);
    ege_enable_aa(true, maskLayer);
    ege_enable_aa(true, bgLayer);
    // 遮罩使用不透明颜色：黑色背景(隐藏)和白色填充(显示)
    setbkcolor_f(EGEARGB(255, 0, 0, 0), maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGERGBA(255,255,255,255), maskLayer);
    ege_fillroundrect(0.25,0.25,width - 0.5,height - 0.5, radius, radius, radius, radius, maskLayer);
    inv.create(false, 2);
    inv.visible(false);
    inv.move(-1, -1);
    inv.size(0, 0);
    inv.setmaxlen(2147483640);
    inv.setparent(this);
    
    on_focus = false;
    inv.killfocus();
}

InputBox::~InputBox() {
    if (btnLayer) delimage(btnLayer);
    if (maskLayer) delimage(maskLayer);
    if (bgLayer) delimage(bgLayer);
}

const double BLINK_FREQUENCY = 0.65;
const double PAUSE_THRESHOLD = 0.6;
double InputBoxSinDoubleForCursor(double time) {
    double sine_value = std::sin(time * 2.0 * M_PI * BLINK_FREQUENCY);
    if (sine_value > PAUSE_THRESHOLD) {
        return 1.0;
    }
    if (sine_value < -PAUSE_THRESHOLD) {
        return 0.0;
    }
    return (sine_value + 1.0) / 2.0;
}

void InputBox::draw(PIMAGE dst, double x, double y) {
    double left = x - width / 2;
    double top = y - height / 2;
    
    if(!on_focus && !ripples.size() && !needRedraw && !scaleChanged && !PanelScaleChanged){
        putimage_withalpha(dst, bgLayer, left, top);
        return;
    }

    ege_setfont(23 * scale, L"宋体", btnLayer);
    if (on_focus) {
        adjustScrollForCursor();
        inv.setfocus();
        wchar_t str[512];
        inv.gettext(512, str);
        setContent(str);
    }

    std::wstring displayContent = IMECompositionString.size() ? 
        (this->content.substr(0,cursor_pos) + IMECompositionString + this->content.substr(cursor_pos)) : 
        (this->content);
        
    // 使用真正的透明色(PRGB32模式下alpha=0时RGB也应为0)
    setbkcolor_f(EGEARGB(0, 0, 0, 0), btnLayer);
    cleardevice(btnLayer);

    // 按钮背景
    setfillcolor(EGEACOLOR(255, color), btnLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, btnLayer);

    // 更新并绘制 ripples
    for (auto& r : ripples) {
        r.update();
        r.draw(btnLayer);
    }
    // 优化：使用C++20 std::erase_if替代erase-remove惯用法
    std::erase_if(ripples, [](const Ripple& r) { return !r.alive(); });

    // 优化：仅在缩放改变时设置字体
    double currentFontScale = scale * text_height;
    
    setbkmode(TRANSPARENT, btnLayer);
    settextcolor(BLACK, btnLayer);

    if(sgn(currentFontScale - 1) >= 0 || scaleChanged || PanelScaleChanged){
        const float padding = 14 * scale;
        
        // 优化：仅在内容改变时重新计算文本宽度
        float cursor_pos_width, cursor_with_ime_width, tmp, full_text_width, cursor_with_full_ime_width;
        if (lastMeasuredContent != displayContent || lastCursorPos != cursor_pos || scaleChanged || PanelScaleChanged) {
            std::wstring cursor_before_cursor = displayContent.substr(0, cursor_pos) + IMECompositionString.substr(0, IMECursorPos);
            std::wstring cursor_before_text = displayContent.substr(0, cursor_pos) + IMECompositionString;
            
            measuretext(displayContent.substr(0, cursor_pos).c_str(), &cursor_pos_width, &tmp, btnLayer);
            measuretext(cursor_before_cursor.c_str(), &cursor_with_ime_width, &tmp, btnLayer);
            measuretext(cursor_before_text.c_str(), &cursor_with_full_ime_width, &tmp, btnLayer);
            measuretext(displayContent.c_str(), &full_text_width, &tmp, btnLayer);
            
            // 缓存所有测量值
            cachedCursorPosWidth = cursor_pos_width;
            cachedCursorWithImeWidth = cursor_with_ime_width;
            cachedCursorWithFullImeWidth = cursor_with_full_ime_width;
            lastMeasuredContent = displayContent;
            lastCursorPos = cursor_pos;
            reflushCursorTick();
        } else {
            // 使用所有缓存的值
            cursor_pos_width = cachedCursorPosWidth;
            cursor_with_ime_width = cachedCursorWithImeWidth;
            cursor_with_full_ime_width = cachedCursorWithFullImeWidth;
            measuretext(displayContent.c_str(), &full_text_width, &tmp, btnLayer);
        }
        
        float _w,_h;
        measuretext("a",&_w,&_h,btnLayer);
        float textRealHeight = tmp ? tmp : _h;
        float text_start_x = padding - scroll_offset;
        
        // 绘制文本
        ege_outtextxy(text_start_x, height / 2 - textRealHeight / 2, 
                    displayContent.c_str(), btnLayer);
        
        if (on_focus) {
            // 聚焦状态遮罩 - 使用圆角矩形匹配输入框形状
            setfillcolor(EGEARGB(50,30,30,30), btnLayer);
            ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, btnLayer);

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
            InputPositionX = left + cursor_draw_x + absolutPosDeltaX;
            InputPositionY = top + height / 2 + textRealHeight / 2 + 2 + absolutPosDeltaY;
        }
    }
    
    // 应用遮罩绘制
    // 使用真正的透明色(PRGB32模式下alpha=0时RGB也应为0)
    setbkcolor_f(EGEARGB(0, 0, 0, 0), bgLayer);
    cleardevice(bgLayer);
    putimage_alphafilter(bgLayer, btnLayer, 0, 0, maskLayer, 0, 0, -1, -1);
    putimage_withalpha(dst,bgLayer,left,top);
    needRedraw = false;
    scaleChanged = false;
}

void InputBox::draw(){
    draw(nullptr,cx,cy);
}

void InputBox::deleteFocus(){
    on_focus = false;
    inv.killfocus();
    needRedraw = true;
    if(mouseOwningFlag == this) mouseOwningFlag = nullptr;
    if(focusingWidget == this) focusingWidget = nullptr;
}

bool InputBox::handleEvent(const mouse_msg& msg) {
    const bool inside = isInside(msg.x, msg.y);
    // 鼠标移入移出处理
    if (inside) {
        setCursor(IDC_IBEAM);
        lastInside = true;
    } else {
        if (lastInside) {
            setCursor(IDC_ARROW);
            needReflushCursor = true;
            lastInside = false;
        }
    }

    // 鼠标左键按下且在输入框内部
    if (msg.is_left() && msg.is_down() && inside) {
        int localX = msg.x - left;
        int localY = msg.y - top;

        if (!on_focus) {
            m_counter ++;
            ripples.emplace_back(localX, localY, 4.00f / 3.00f * std::sqrt(height * height + width * width), 70, dynamic_cast<Widget*>(this),m_counter);
            on_focus = true;
            needRedraw = true;
            inv.setfocus();
            reflushCursorTick();
        }

        // 计算点击位置对应的字符下标（二分法）
        const float padding = 14 * scale;
        float click_x = localX - padding + scroll_offset;
        int l = 0, r = content.length();
        int best_pos = 0;
        float min_dist = 1e9, tmp, char_x = 0;

        while (l <= r) {
            int mid = (l + r) / 2;
            measuretext(content.substr(0, mid).c_str(), &char_x, &tmp, btnLayer);
            float dist = fabs(char_x - click_x);
            if (dist < min_dist) {
                min_dist = dist;
                best_pos = mid;
            }
            if (char_x < click_x) {
                l = mid + 1;
            } else if (char_x > click_x) {
                r = mid - 1;
            } else {
                best_pos = mid;
                break;
            }
        }

        moveCursor(best_pos);
        inv.movecursor(best_pos, best_pos);
        needRedraw = true;
        if(focusingWidget != nullptr && focusingWidget != this){
            focusingWidget->deleteFocus();
        }
        focusingWidget = this;
        mouseOwningFlag = this;
        return true;
    }
    // 鼠标左键按下且不在输入框内
    else if (msg.is_left() && msg.is_down()) {
        deleteFocus();
    }

    if(msg.is_left() && msg.is_up() && mouseOwningFlag == this){
        mouseOwningFlag = nullptr;
    }

    return false;
}

bool InputBox::isInside(double x, double y) const {
    // 转换为按钮内部坐标系
    double localX = x - left;
    double localY = y - top;
    // wcout<<L"IS localX: "<<localX<<L" localY: "<<localY<<L"WHICH is " <<EGEGET_A(getpixel(localX, localY, btnLayer))<<endl;

    // 先检查是否在按钮矩形框外
    if (localX < 0 || localX >= width || localY < 0 || localY >= height)
        return false;

    // 中心矩形区（不考虑圆角）直接返回 true
    if (localX >= radius && localX < width - radius)
        return true;
    if (localY >= radius && localY < height - radius)
        return true;

    // 检查四个圆角区域
    int dx, dy;
    // 左上角
    if (localX < radius && localY < radius) {
        dx = radius - localX;
        dy = radius - localY;
        return dx * dx + dy * dy <= radius * radius;
    }
    // 右上角
    if (localX >= width - radius && localY < radius) {
        dx = localX - (width - radius);
        dy = radius - localY;
        return dx * dx + dy * dy <= radius * radius;
    }
    // 左下角
    if (localX < radius && localY >= height - radius) {
        dx = radius - localX;
        dy = localY - (height - radius);
        return dx * dx + dy * dy <= radius * radius;
    }
    // 右下角
    if (localX >= width - radius && localY >= height - radius) {
        dx = localX - (width - radius);
        dy = localY - (height - radius);
        return dx * dx + dy * dy <= radius * radius;
    }

    // 其余区域为中间的十字形部分
    return true;
}

void InputBox::setContent(const std::wstring& s) {
    if(content == s) return;
    content = s;
    needRedraw = true;
}

void InputBox::setMaxlen(int maxlen) {
    inv.setmaxlen(maxlen);
}

void InputBox::setPosition(double x,double y){
    if(sgn(left - (x - width / 2)) == 0 && sgn(top - (y - height / 2)) == 0) return;
	left = x - width / 2;
	top = y - height / 2;
    needRedraw = true;
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
    if (old_scale > 0) {
        scroll_offset = scroll_offset * (s / old_scale);
    }
    // 遮罩
    if(maskLayer) delimage(maskLayer);
    maskLayer = newimage(width,height);
    if(btnLayer) delimage(btnLayer);
    btnLayer = newimage(width,height);
    if(bgLayer) delimage(bgLayer);
    bgLayer = newimage(width,height);
    ege_enable_aa(true,bgLayer);
    ege_enable_aa(true,maskLayer);
    ege_enable_aa(true,btnLayer);
    // 遮罩使用不透明颜色：黑色背景(隐藏)和白色填充(显示)
    setbkcolor_f(EGEARGB(255, 0, 0, 0), maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, maskLayer);
    needRedraw = true;
    scaleChanged = true;
}

void InputBox::setTextHeight(double height){
    if(sgn(height - text_height) == 0) return;
    text_height = height;
    needRedraw = true;
}

double InputBox::getTextHeight(){
    return text_height;
}

void InputBox::moveCursor(int pos){
    if(cursor_pos == pos) return;
    cursor_pos = pos;
    needRedraw = true;
}

void InputBox::setIMECompositionString(const std::wstring& str){
    IMECompositionString = str;
    needRedraw = true;
}

void InputBox::setIMECursorPos(int pos){
    IMECursorPos = pos;
    needRedraw = true;
}

void InputBox::adjustScrollForCursor() {
    const float padding = 14 * scale;
    const float visible_width = width - 2 * padding;

    float cursor_pixel_pos, tmp;
    measuretext(content.substr(0, cursor_pos).c_str(), &cursor_pixel_pos, &tmp, btnLayer);
    float cursor_with_ime_pos = cursor_pixel_pos;
    if (IMECompositionString.size()) {
        measuretext((content.substr(0, cursor_pos) + IMECompositionString).c_str(), &cursor_with_ime_pos, &tmp, btnLayer);
    }
    float full_text_width;
    std::wstring display_content = IMECompositionString.size() ? 
        (content.substr(0, cursor_pos) + IMECompositionString + content.substr(cursor_pos)) : 
        content;
    measuretext(display_content.c_str(), &full_text_width, &tmp, btnLayer);

    float target_pos = IMECompositionString.size() ? cursor_with_ime_pos : cursor_pixel_pos;

    // 修正：右边界判断为 >=，scroll_offset 公式明确
    if (target_pos - scroll_offset >= visible_width - padding) {
        scroll_offset = target_pos - (visible_width - padding);
        needRedraw = true;
    }
    else if (cursor_pixel_pos - scroll_offset < padding) {
        scroll_offset = cursor_pixel_pos - padding;
        if (scroll_offset < 0) scroll_offset = 0;
        needRedraw = true;
    }
    if (full_text_width <= visible_width) {
        scroll_offset = 0;
        needRedraw = true;
    } else {
        float max_scroll = full_text_width - visible_width;
        if (scroll_offset > max_scroll) {
            scroll_offset = max_scroll;
            needRedraw = true;
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
    return content;
}

int InputBox::getMCounter(){
    return m_counter;
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

InputBox* InputBoxBuilder::build() {
    auto input = new InputBox(cx, cy, width, height, radius);
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
    if (m_pressed) {
        m_scale += (0.8f - m_scale) * 0.2f; // 缓动到 60%
    } else {
        m_scale += (1.0f - m_scale) * 0.2f; // 回弹
    }

    // 平滑过渡进度
    m_progress += (m_finalprogress - m_progress) * 0.15;
    if (fabs(m_progress - m_finalprogress) < 0.005)
        m_progress = m_finalprogress;

    // 背景轨道
    setfillcolor(m_bgColor,dst);
    setlinecolor(m_bgColor,dst);
    
    if (m_orientation == Orientation::Column) {
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

        if (m_pressed) {
            setfillcolor(EGERGBA(80, 80, 80, 80), dst);
            ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2, dst);
        } else if (m_hover && !Lpressed) {
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

        if (m_pressed) {
            setfillcolor(EGERGBA(80, 80, 80, 80), dst);
            ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2, dst);
        } else if (m_hover && !Lpressed) {
            setfillcolor(EGERGBA(80, 80, 80, 40), dst);
            ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2, dst);
        }
    }
}

void Slider::draw(){
	draw(nullptr,cx,cy);
}

bool Slider::isInside(double x, double y){
    double knobX, knobY;
    if (m_orientation == Orientation::Column) {
        knobX = left + width / 2;
        knobY = top + (1.0 - m_progress) * height;
        radius = width / 2;
    } else { // Row
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
    if (step > 0) {
        value = round(value / step) * step;
    }
    return value;
}

bool Slider::handleEvent(const mouse_msg& msg) {
    m_hover = isInside(msg.x, msg.y);
    m_skip = isInsideBar(msg.x, msg.y);

    if(!m_hover && m_skip && msg.is_left() && msg.is_down()){
        m_dragging = true;
        m_pressed = true;
        if (m_orientation == Orientation::Column) {
            int my = clamp(msg.y, top, top + height);
            m_finalprogress = 1.0 - (my - top) / static_cast<double>(height);
        } else { // Row
            int mx = clamp(msg.x, left, left + width);
            m_finalprogress = (mx - left) / static_cast<double>(width);
        }
        if (m_value != fixProgress() && m_onChange != nullptr)
        {
            m_value = fixProgress();
            m_onChange(m_value);
        }
        mouseOwningFlag = this;
        return true;
    }
    else if (msg.is_left() && msg.is_down() && m_hover) {
        m_dragging = true;
        m_pressed = true;
        int knobX, knobY;
        if (m_orientation == Orientation::Column) {
            knobX = left + width / 2;
            knobY = top + static_cast<int>((1.0 - m_progress) * height);
            m_dragOffset = msg.y - knobY;
        } else { // Row
            knobX = left + static_cast<int>(m_progress * width);
            knobY = top + height / 2;
            m_dragOffset = msg.x - knobX;
        }
        mouseOwningFlag = this;
        return true;
    } 
    else if (msg.is_move() && m_dragging) {
        if (m_orientation == Orientation::Column) {
            int my = clamp(msg.y - m_dragOffset, top, top + height);
            m_finalprogress = 1.0 - (my - top) / static_cast<double>(height);
        } else { // Row
            int mx = clamp(msg.x - m_dragOffset, left, left + width);
            m_finalprogress = (mx - left) / static_cast<double>(width);
        }
        if (m_value != fixProgress() && m_onChange != nullptr)
        {
            m_value = fixProgress();
            m_onChange(m_value);
        }
        return true;
    } 
    else if (msg.is_left() && msg.is_up()) {
        m_dragging = false;
        m_pressed = false;
        m_finalprogress = fixProgress();
        if(mouseOwningFlag == this) mouseOwningFlag = nullptr;
    }
    return false;
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
    slider->setProgress(progress);
    slider->setScale(scale);
    slider->setStep(step);
    if (onChange) slider->setOnChange(onChange);
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
    if (barLayer) delimage(barLayer);
}

void ProgressBar::setProgress(double p) {
    p = clamp(p, 0.0, 1.0);
    if (fabs(targetProgress - p) > 1e-6) {
        targetProgress = p;
        needRedraw = true;
    }
}

double ProgressBar::getProgress() const {
    return targetProgress;
}

void ProgressBar::setColor(color_t fg) {
    if (fgColor != fg) {
        fgColor = fg;
        needRedraw = true;
    }
}

void ProgressBar::setBackground(color_t bg) {
    if (bgColor != bg) {
        bgColor = bg;
        needRedraw = true;
    }
}

void ProgressBar::draw(PIMAGE dst, double x, double y) {
    double left = x - width / 2;
    double top = y - height / 2;

    //缓动到目标进度
    currentProgress += (targetProgress - currentProgress) * 0.15;
    if (fabs(currentProgress - targetProgress) < 0.005)
        currentProgress = targetProgress;

    if (!needRedraw && fabs(currentProgress - targetProgress) < 1e-4) {
        putimage_withalpha(dst, barLayer, left, top);
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

    putimage_withalpha(dst, barLayer, left, top);
    needRedraw = fabs(currentProgress - targetProgress) > 1e-4;
}

void ProgressBar::draw() {
    draw(nullptr, cx, cy);
}

bool ProgressBar::handleEvent(const mouse_msg& msg){
    return false;
}

void ProgressBar::setPosition(double x, double y) {
    if (cx == x && cy == y) return;
    cx = x;
    cy = y;
    left = cx - width / 2;
    top = cy - height / 2;
    needRedraw = true;
}

void ProgressBar::setScale(double s) {
    if (fabs(scale - s) < 1e-6) return;
    scale = s;
    width = origin_width * s;
    height = origin_height * s;
    if (barLayer) delimage(barLayer);
    barLayer = newimage(width, height);
    ege_enable_aa(true, barLayer);
    left = cx - width / 2;
    top = cy - height / 2;
    needRedraw = true;
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
    if (mainButton) delete mainButton;
    if (dropdownPanel) delete dropdownPanel;
    for (Button* option : options) {
        if (option) delete option;
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
    if (expanded || fadeAlpha > 0.08) {
        // 更新透明度（渐变）
        if (fadingIn) {
            fadeAlpha += 0.09;
            if (fadeAlpha >= 1.0) {
                fadeAlpha = 1.0;
                fadingIn = false;
            }
        } else if (fadingOut) {
            fadeAlpha -= 0.09;
            if (fadeAlpha <= 0.0) {
                fadeAlpha = 0.0;
                fadingOut = false;
                expanded = false;
            }
        }

        // 优化：只在透明度改变时调用setAlpha
        int actualAlpha = static_cast<int>(fadeAlpha * 255);
        if (lastAppliedAlpha != actualAlpha) {
            dropdownPanel->setAlpha(actualAlpha);
            lastAppliedAlpha = actualAlpha;
        }
        dropdownPanel->draw(dst, cx, cy + height / 2 + height * options.size() / 2 + 4);
    }
}

void Dropdown::draw() {
    draw(nullptr, cx, cy);
}

bool Dropdown::handleEvent(const mouse_msg& msg) {
    if (expanded) {
        dropdownPanel->handleEvent(msg);
        for (Button* btn : options) {
            btn->handleEvent(msg);
        }
    }

    mainButton->handleEvent(msg);

    // 检测点击空白关闭
    if (msg.is_left() && msg.is_down()) {
        if (!isInside(msg.x, msg.y)) {
            // 点到了空白区域，关闭下拉
            fadingOut = true;
            fadingIn = false;
        }
    }
    return false;
}

void Dropdown::toggleDropdown() {
    if (!expanded) {
        expanded = true;
        fadingIn = true;
        fadingOut = false;
    } else {
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
}

bool Dropdown::isInside(double x, double y) const {
    // 检查是否在主按钮内
    if (mainButton->isInside(x, y))
        return true;

    // 如果未展开，无需检查下拉项
    if (!expanded && fadeAlpha <= 0.01)
        return false;

    // 检查是否在下拉选项内
    for (Button* btn : options) {
        if (btn->isInside(x, y))
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
    if (!nowChecked && wasChecked && !animOut) {
        // 被取消选中了，启动缩小动画
        animOut = true;
        animIn = false;
        animProgress = 0.0;
    }
    wasChecked = nowChecked;

    double R = radius;

    // 悬停阴影
    if (hovered) {
        double shadowRadius = R + 6;
        setfillcolor(EGERGBA(50, 50, 50, 32), dst);
        ege_fillellipse(x - shadowRadius, y - shadowRadius, shadowRadius * 2, shadowRadius * 2, dst);
    }

    if(keepColor || nowChecked) setlinecolor(EGERGB(50, 150, 250), dst);
    else setlinecolor(EGERGB(110,110,110), dst);
    setlinewidth(2, dst);

    if (style == RadioStyle::Filled) {
        setfillcolor(WHITE, dst);
        ege_fillellipse(x - R, y - R, R * 2, R * 2, dst);
    } 
    else if (style == RadioStyle::Outline) {
        // 只绘制线框，不填充
        ege_ellipse((float)(x - R), (float)(y - R), (float)(R * 2), (float)(R * 2), dst);
    }

    // 更新动画进度
    if (animIn) {
        animProgress += animSpeed;
        if (animProgress >= 1.0) {
            animProgress = 1.0;
            animIn = false;
        }
    } else if (animOut) {
        animProgress += animSpeed;
        if (animProgress >= 1.0) {
            animOut = false;
        }
    }

    // 动画缩放
    double scaleFactor = 1.0;
    if (animIn) {
        scaleFactor = 0.7 + 0.4 * animProgress;
        lastScale = scaleFactor;
    } else if (animOut) {
        scaleFactor = 1.0 - 0.4 * animProgress;
        lastScale = scaleFactor;
    } else {
        scaleFactor = lastScale;
    }

    if (showDot) {
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

bool Radio::handleEvent(const mouse_msg& msg) {
    int dx = msg.x - cx;
    int dy = msg.y - cy;
    hovered = dx * dx + dy * dy <= radius * radius;

    if (msg.is_left() && msg.is_down() && hovered) {
        if (groupValuePtr && *groupValuePtr != value) {
            *groupValuePtr = value;
            animIn = true;
            animOut = false;
            animProgress = 0.0;
            if (onSelect) onSelect();
            return true;
        } else if (groupValuePtr && *groupValuePtr == value) {
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
    if (groupPtr) radio->setGroupValueRef(groupPtr);
    if (onSelect) radio->setOnSelect(onSelect);
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

void RadioController::build() {
    for (size_t i = 0; i < values.size(); ++i) {
        std::wstring val = values[i];
        RadioBuilder()
            .setCenter(cx, cy + i * gap)
            .setRadius(radius)
            .setScale(scale)
            .setStyle(style)
            .setValue(val)
            .setGroupValueRef(&currentValue)
            .setOnSelect([=,this] {
                currentValue = val;
                if (onChange) onChange(val);
            })
            .build();
    }
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
}

void Toggle::toggle() {
    setChecked(!checked);
    if (onToggle) onToggle(checked);
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

    hovered = (std::abs(dx) <= w / 2 && std::abs(dy) <= h / 2);

    if (disabled) return false;

    if (msg.is_left()) {
        if (msg.is_down()) {
            // 记录是否在区域内按下
            pressedIn = hovered;
            mouseOwningFlag = this;
            return true;
        } else if (msg.is_up()) {
            // 如果松开时还在区域内且是之前按下的
            if (hovered && pressedIn) {
                toggle(); // ? 这里才执行切换动画与回调
            }
            pressedIn = false;
            if(mouseOwningFlag == this){
                mouseOwningFlag = nullptr;
            }
        }
    }
    return false;
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
    if (std::abs(knobOffset - knobTarget) > 1e-3)
        knobOffset += (knobTarget - knobOffset) * animationSpeed;
    else
        knobOffset = knobTarget;

    double w = width * scale;
    double h_track = height * scale * 0.6;
    double r_track = h_track / 2;

    double h_knob = height * scale;
    double r_knob = h_knob / 2;

    // === 滑块位置 ===
    double knobX = x - w / 2 + r_track + knobOffset * (w - h_track);

    // === 轨道颜色 ===
    color_t trackColor;
    if (disabled) {
        trackColor = EGERGB(200, 200, 200); // 禁用轨道
    } else if (keepColor) {
        trackColor = checked ? mixColor(baseColor, WHITE, 0.55)
                            : mixColor(baseColor, WHITE, 0.80);
    } else {
        trackColor = checked ? mixColor(baseColor, WHITE, 0.55)
                            : EGERGB(160, 160, 160);
    }

    setfillcolor(trackColor, dst);
    setlinecolor(TRANSPARENT, dst);
    ege_fillroundrect(x - w / 2, y - h_track / 2, w, h_track, r_track, r_track, r_track, r_track, dst);

    // === 悬浮灰圈背景（hover 时）
    if (hovered && !disabled) {
        double hoverR = r_knob + 7;
        setfillcolor(EGERGBA(100, 100, 100, 32), dst);
        ege_fillellipse(knobX - hoverR, y - hoverR, hoverR * 2, hoverR * 2, dst);
    }

    // === 滑块颜色（关闭时为浅灰而非纯白）
    color_t knobColor;
    if (disabled) {
        knobColor = EGERGB(220, 220, 220); // 灰滑块
    } else if (keepColor) {
        knobColor = checked ? baseColor : EGERGB(245, 245, 245);
    } else {
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
    if (onToggle) toggle->setOnToggle(onToggle);
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
}

void Text::setScale(double s) {
    scale = s;
    updateLayout();
}

void Text::setPosition(double x, double y) {
    posX = x;
    posY = y;
}

int Text::getTextWidth() const {
    return textWidth;
}

int Text::getTextHeight() const {
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
        if (ch == L'\n') {
            lines.push_back(line);
            line.clear();
            continue;
        }

        line += ch;

        float tmp,_w;
        measuretext(line.c_str(),&_w,&tmp);
        if (maxWidth > 0 && _w > maxWidth) {
            line.pop_back();
            lines.push_back(line);
            line = ch;
        }
    }

    if (!line.empty())
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
    width = textWidth;
}

// 默认绘制
void Text::draw() {
    draw(nullptr, posX, posY);
}

// 绘制到目标图像
void Text::draw(PIMAGE dst, double x, double y) {
    // 重要：每次都设置字体，因为dst可能是不同的图像上下文
    // updateLayout()在默认上下文中设置字体，但draw()可能在不同的dst上
    ege_setfont(fontSize * scale, fontName.c_str(), dst);
    settextcolor(color, dst);

    for (size_t i = 0; i < lines.size(); ++i) {
        double x_draw = x;
        
        // 优化：使用缓存的行宽度，避免重复测量
        float lineW = (i < cachedLineWidths.size()) ? cachedLineWidths[i] : 0;
        if (lineW == 0) {
            // 如果缓存失效，重新测量
            float w = 0, h = 0;
            measuretext(lines[i].c_str(), &w, &h, dst);
            lineW = w;
        }

        if (align == TextAlign::Center)
            x_draw = x + (maxWidth - lineW) / 2;
        else if (align == TextAlign::Right)
            x_draw = x + (maxWidth - lineW);

        // 使用缓存的行高
        float tmp,_h;
        measuretext("A",&tmp,&_h,dst);
        double lineHeight = (textHeight > 0 && lines.size() > 0) ? 
            (textHeight / lines.size()) : _h;
        double y_draw = y + i * (lineHeight + lineSpacing);
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
Knob::Knob(double cx, double cy, double r)
    : cx(cx), cy(cy), radius(r), origin_radius(r) {
    // 初始化内部范围为外部范围
    innerMin = minValue;
    innerMax = maxValue;
    // 初始化displayValue与value相同
    displayValue = value;
}

void Knob::setRange(double minVal, double maxVal) {
    minValue = minVal;
    maxValue = maxVal;
    // 同步更新内部范围（如果未单独设置）
    if (innerMin == minValue && innerMax == maxValue) {
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
    if (step > 0) {
        value = applyStep(value);
    }
    // 如果值改变且有回调，触发回调
    if (oldValue != value && onChange) {
        onChange(value);
    }
}

double Knob::getValue() const {
    return value;
}

void Knob::setColor(color_t fg, color_t bg) {
    fgColor = fg;
    bgColor = bg;
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
}

void Knob::setPosition(double x, double y) {
    cx = x;
    cy = y;
}

double Knob::clamp(double v) const {
    if (v < minValue) return minValue;
    if (v > maxValue) return maxValue;
    return v;
}

double Knob::applyStep(double v) const {
    if (step <= 0) return v;
    // 将值对齐到最近的步进点
    double steps = std::round((v - minValue) / step);
    return minValue + steps * step;
}

double Knob::valueToAngle(double v) const {
    // 将值映射到角度范围
    // 完整范围：从 -90° (顶部) 到 +270° (总共 360°)
    // 加上偏移角度
    double range = maxValue - minValue;
    if (range <= 0) return offsetAngle;
    
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
    return dist <= radius;
}

void Knob::draw(PIMAGE dst, double x, double y) {
    double r = radius;
    
    // 缓动到目标值（类似Slider的实现）
    displayValue += (value - displayValue) * 0.09;
    if (fabs(displayValue - value) < 0.01) {
        displayValue = value;
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
    setlinewidth((int)arcThickness, dst);
    
    double startAngle = -90.0 + offsetAngle;
    double currentAngle = valueToAngle(displayValue);
    double progressSweep = currentAngle - startAngle;
    
    // 绘制进度弧
    // 添加小的epsilon以确保完整绘制到边界
    if (progressSweep > 0.01) {
        // 当接近最大值时，确保完整绘制360度
        double drawSweep = progressSweep;
        if (progressSweep > 359.5 && progressSweep < 360.5) {
            drawSweep = 360.0;  // 确保完整的360度弧
        }
        ege_arc((float)(x - r), (float)(y - r), (float)(2 * r), (float)(2 * r), 
                (float)startAngle, (float)drawSweep, dst);
    }
    
    // === 绘制中心填充圆 ===
    color_t centerColor = WHITE;
    if (hovered && !disabled && !readonly) {
        centerColor = EGERGB(250, 250, 250);
    }
    
    setfillcolor(centerColor, dst);
    setlinecolor(disabled ? EGERGB(200, 200, 200) : EGERGB(220, 220, 220), dst);
    setlinewidth(2, dst);
    ege_fillcircle((int)x, (int)y, (int)(r * 0.7), dst);
    
    // === 显示当前值 ===
    if (showValue) {
        // 计算字体大小
        double actualFontSize = fontSize > 0 ? fontSize : (r * 0.35);
        
        // 格式化显示值（显示displayValue而不是value，实现缓动效果）
        wchar_t valueText[64];
        if (step >= 1.0) {
            swprintf(valueText, 64, L"%.0f", value);
        } else if (step >= 0.1) {
            swprintf(valueText, 64, L"%.1f", value);
        } else {
            swprintf(valueText, 64, L"%.2f", value);
        }
        
        // 设置字体和颜色
        ege_setfont(actualFontSize, L"Consolas", dst);
        setcolor(disabled ? EGERGB(150, 150, 150) : BLACK, dst);
        setbkmode(TRANSPARENT, dst);
        
        // 计算文本宽度和高度以居中显示
        float textWidth,textHeight;
        measuretext(valueText,&textWidth,&textHeight,dst);
        
        outtextxy(x - textWidth / 2, y - textHeight / 2, valueText, dst);
    }
}

void Knob::draw() {
    draw(nullptr, cx, cy);
}

bool Knob::handleEvent(const mouse_msg& msg) {
    // 检查是否在旋钮内
    hovered = isInside(msg.x, msg.y);
    
    // 如果禁用或只读，不响应交互
    if (disabled || readonly) {
        return false;
    }
    
    // 按下鼠标左键开始拖动
    if (msg.is_left() && msg.is_down() && hovered) {
        dragging = true;
        lastMouseX = msg.x;
        lastMouseY = msg.y;
        mouseOwningFlag = this;
        return true;
    }
    // 拖动中
    else if (msg.is_move() && dragging) {
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
        if (normalizedAngle < startAngle) {
            normalizedAngle += 360.0;  // 转换到正值范围
        }
        
        double totalSweep = 360.0;
        
        // 将整个360度圆线性映射到值范围
        double ratio = (normalizedAngle - startAngle) / totalSweep;
        double newValue = minValue + ratio * (maxValue - minValue);
        
        // 限制和应用步进
        newValue = clamp(newValue);
        if (step > 0) {
            newValue = applyStep(newValue);
        }
        
        // 更新值并触发回调
        if (newValue != value) {
            value = newValue;
            if (onChange) {
                onChange(value);
            }
        }
        
        lastMouseX = msg.x;
        lastMouseY = msg.y;
        return true;
    }
    // 释放鼠标左键停止拖动
    else if (msg.is_left() && msg.is_up()) {
        dragging = false;
        if(mouseOwningFlag == this){
            mouseOwningFlag = nullptr;
        }
    }
    return false;
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
    
    if (onChange) {
        knob->setOnChange(onChange);
    }
    
    // 注册到全局控件集合
    //widgets.insert(knob);
    
    // 如果有标识符，注册到ID映射
    if (!identifier.empty()) {
        IdToWidget[identifier] = knob;
    }
    
    return knob;
}


Sidebar::Sidebar(double cx, double cy, double w, double h, double r, color_t bg) {
    this->cx = cx;
    this->cy = cy;
    origin_width = width = w;
    origin_height = height = h;
    radius = r;
    bgColor = bg;

    container = new Panel(cx, cy, w, h, r, bg);
}

void Sidebar::addItem(Widget* child, double offsetY) {
    double y = -origin_height / 2 + 40 + items.size() * 60 + offsetY;
    container->addChild(child, 0, y);
    items.push_back(child);
}

void Sidebar::toggle() {
    setExpanded(!expanded);
}

void Sidebar::setExpanded(bool exp) {
    expanded = exp;
    if (expanded) {
        container->setSize(origin_width, origin_height);
    } else {
        container->setSize(collapsedWidth, origin_height);
    }
}

bool Sidebar::isExpanded() const {
    return expanded;
}

void Sidebar::draw(PIMAGE dst, double x, double y) {
    container->draw(dst, x, y);
}

void Sidebar::draw() {
    container->draw();
}

bool Sidebar::handleEvent(const mouse_msg& msg) {
    return container->handleEvent(msg);
}

void Sidebar::setPosition(double x, double y) {
    cx = x; cy = y;
    container->setPosition(x, y);
}

void Sidebar::setScale(double s) {
    width = origin_width * s;
    height = origin_height * s;
    container->setScale(s);
}

Sidebar::~Sidebar(){
    if (container) delete container;
}

// Builder 实现
SidebarBuilder& SidebarBuilder::setCenter(double x, double y) {
    cx = x; cy = y;
    return *this;
}

SidebarBuilder& SidebarBuilder::setSize(double w, double h) {
    width = w; height = h;
    return *this;
}

SidebarBuilder& SidebarBuilder::setRadius(double r) {
    radius = r;
    return *this;
}

SidebarBuilder& SidebarBuilder::setBackground(color_t color) {
    bg = color;
    return *this;
}

SidebarBuilder& SidebarBuilder::addItem(Widget* item) {
    items.push_back(item);
    return *this;
}

Sidebar* SidebarBuilder::build() {
    auto sidebar = new Sidebar(cx, cy, width, height, radius, bg);
    for (auto* item : items) {
        sidebar->addItem(item);
    }
    //widgets.insert(sidebar);
    return sidebar;
}

std::map<std::wstring,Widget*> IdToWidget;

Widget* getWidgetById(const std::wstring& identifier){
    auto it = IdToWidget.find(identifier);
    if (it != IdToWidget.end()) {
        return it->second;
    }
    return nullptr;
}

void assignOrder(std::vector<Widget*> widgetWithOrder){
    swap(widgetWithOrder,widgets);
}