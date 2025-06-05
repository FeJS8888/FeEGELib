#include "Control.h"

using namespace FeEGE;
set<Widget*> widgets;

Panel::Panel(int centerX, int centerY, double w, double h, double r, color_t bg)
    : cx(centerX), cy(centerY), width(w),height(h), bgColor(bg) {
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = r;
	layer = newimage(width, height);	
	ege_enable_aa(true,layer);
	
	setbkcolor_f(TRANSPARENT, maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, maskLayer);
}

void Panel::addChild(Widget* child, double offsetX, double offsetY) {
    children.push_back(child);
    childOffsets.push_back(Position{ offsetX, offsetY });
    child->is_global = false;
}

void Panel::draw() {
	double left = cx - width / 2;
	double top = cy - height / 2;
    setbkcolor_f(EGEACOLOR(0,bgColor), layer);
    cleardevice(layer);

    // 绘制自身背景（圆角矩形）
    setfillcolor(bgColor, layer);
    ege_fillrect(0, 0, width, height, layer);

    // 绘制子控件
    for (size_t i = 0; i < children.size(); ++i) {
        int childX = width / 2 + childOffsets[i].x * scale;
        int childY = height / 2 + childOffsets[i].y * scale;
        children[i]->setPosition(cx + childOffsets[i].x * scale,cy + childOffsets[i].y * scale);
        children[i]->draw(layer, childX, childY);
    }

    // 粘贴到主窗口
    putimage_alphafilter(nullptr, layer, left, top, maskLayer, 0, 0, -1, -1);
}

void Panel::draw(PIMAGE dst, int x, int y) {
    double left = x - width / 2;
	double top = y - height / 2;
    setbkcolor_f(EGEACOLOR(0,bgColor), layer);
    cleardevice(layer);

    // 绘制自身背景（圆角矩形）
    setfillcolor(bgColor, layer);
    ege_fillrect(0, 0, width, height, layer);

    // 绘制子控件
    for (size_t i = 0; i < children.size(); ++i) {
        int childX = width / 2 + childOffsets[i].x * scale;
        int childY = height / 2 + childOffsets[i].y * scale;
        children[i]->setPosition(cx + childOffsets[i].x * scale,cy + childOffsets[i].y * scale);
        children[i]->draw(layer, childX, childY);
    }

    // 粘贴到主窗口
    putimage_alphafilter(dst, layer, left, top, maskLayer, 0, 0, -1, -1);
}

Panel::~Panel(){
	if (layer) delimage(layer);
}

void Panel::setPosition(int x,int y){
	cx = x;
	cy = y;
}

Position Panel::getPosition(){
	return Position{cx * 1.00f,cy * 1.00f};
}

void Panel::setScale(double s){
	width = origin_width * s;
    height = origin_height * s;
    radius = origin_radius * s;
	scale = s;
	for (size_t i = 0; i < children.size(); ++i) {
        int childX = width / 2 + childOffsets[i].x * scale;
        int childY = height / 2 + childOffsets[i].y * scale;
        children[i]->setScale(s);
        children[i]->setPosition(cx + childOffsets[i].x * scale,cy + childOffsets[i].y * scale);
    }
	
	setbkcolor_f(TRANSPARENT, maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0.25, 0.25, width - 0.5, height - 0.5, radius, radius, radius, radius, maskLayer);
}

double Panel::getScale(){
	return scale;
}

void Panel::handleEvent(const mouse_msg& msg){
	
}

// Ripple 结构体实现
Ripple::Ripple(int _x, int _y, int _r, int _life)
    : x(_x), y(_y), maxRadius(_r), life(_life) {}

bool Ripple::alive() const { return age < life; }

void Ripple::update() { age++; }

void Ripple::draw(PIMAGE dst,double s) const {
    double progress = (double)age / life;
    double r = maxRadius * progress * s;
    int alpha = static_cast<int>(100 * (1.0 - progress));
    setfillcolor(EGEARGB(alpha, 30, 30, 30), dst);
    ege_fillellipse(x - r / 2, y - r / 2, r, r, dst);
}

// Button 类实现
Button::Button(int cx, int cy, double w, double h, double r)
    : centerX(cx), centerY(cy), width(w), height(h), radius(r) {
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = r;
    left = centerX - width / 2;
    top = centerY - height / 2;

    btnLayer = newimage(width, height);
    maskLayer = newimage(width, height);
    ege_enable_aa(true, btnLayer);
    ege_enable_aa(true, maskLayer);
    
    // 遮罩
    setbkcolor_f(TRANSPARENT, maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0,0,width,height, radius, radius, radius, radius, maskLayer);
}

Button::~Button() {
    delimage(btnLayer);
    delimage(maskLayer);
}

void Button::draw(PIMAGE dst,int x,int y){
    int left = x - width / 2;
    int top = y - height / 2;
    if(!ripples.size() && !needRedraw){
        // setfillcolor(EGERGB(245, 245, 235), dst);
        // setcolor(EGERGB(245, 245, 235), dst);
        // ege_fillroundrect(left, top, width, height, radius, radius, radius, radius, dst);
        // setbkmode(TRANSPARENT, dst);
        // settextcolor(BLACK, dst);
        // setfont(23 * scale, 0, "宋体", dst);
        // ege_outtextxy(x - textwidth(content.c_str(), btnLayer) / 2, 
        //               y - textheight(content.c_str(), btnLayer) / 2, 
        //               content.c_str(), dst);
        putimage_withalpha(dst,btnLayer,left,top);
        // (dst, btnLayer, x - width / 2, y - height / 2, maskLayer, 0, 0, -1, -1);
        return;
    }
    setbkcolor_f(EGEACOLOR(0,color), btnLayer);
    cleardevice(btnLayer);

    // 按钮背景
    setfillcolor(color, btnLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, btnLayer);

    // 更新并绘制 ripples
    for (auto& r : ripples) {
        r.update();
        r.draw(btnLayer,scale);
    }
    ripples.erase(std::remove_if(ripples.begin(), ripples.end(),
        [](const Ripple& r) { return !r.alive(); }),
        ripples.end());

    // 按钮文字
    setbkmode(TRANSPARENT, btnLayer);
    settextcolor(BLACK, btnLayer);
    setfont(23 * scale, 0, "宋体", btnLayer);
    ege_outtextxy(width / 2 - textwidth(content.c_str(), btnLayer) / 2, 
                 height / 2 - textheight(content.c_str(), btnLayer) / 2, 
                 content.c_str(), btnLayer);
    
    // 应用遮罩绘制
    putimage_alphafilter(dst, btnLayer, left, top, maskLayer, 0, 0, -1, -1);
    needRedraw = false;
}

void Button::draw(){
    if(!ripples.size() && !needRedraw){
        putimage_alphafilter(nullptr, btnLayer, left, top, maskLayer, 0, 0, -1, -1);
        return;
    }
    setbkcolor_f(EGEACOLOR(0,color), btnLayer);
    cleardevice(btnLayer);

    // 按钮背景
    setfillcolor(color, btnLayer);
    ege_fillrect(0, 0, width, height, btnLayer);

    // 更新并绘制 ripples
    for (auto& r : ripples) {
        r.update();
        r.draw(btnLayer,scale);
    }
    ripples.erase(std::remove_if(ripples.begin(), ripples.end(),
        [](const Ripple& r) { return !r.alive(); }),
        ripples.end());

    // 按钮文字
    setbkmode(TRANSPARENT, btnLayer);
    settextcolor(BLACK, btnLayer);
    setfont(23 * scale, 0, "宋体", btnLayer);
    ege_outtextxy(width / 2 - textwidth(content.c_str(), btnLayer) / 2, 
                 height / 2 - textheight(content.c_str(), btnLayer) / 2, 
                 content.c_str(), btnLayer);
    
    // 应用遮罩绘制
    putimage_alphafilter(nullptr, btnLayer, left, top, maskLayer, 0, 0, -1, -1);
    needRedraw = false;
}

void Button::handleEvent(const mouse_msg& msg) {
    if (msg.is_left() && msg.is_down() && isInside(msg.x, msg.y)) {
        int localX = msg.x - left;
        int localY = msg.y - top;
        ripples.emplace_back(localX, localY, std::max(width, height) * 2, 40);
        if(on_click_event != nullptr) on_click_event();
        needRedraw = true;
    }
}

bool Button::isInside(int x, int y) const {
    // 转换为按钮内部坐标系
    int localX = x - left;
    int localY = y - top;

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

void Button::setContent(const string& str){
    if(content == str) return;
	content = str;
    needRedraw = true;
}

std::string Button::getContent(){
    return content;
}

void Button::setPosition(int x,int y){
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
    // 遮罩
    setbkcolor_f(TRANSPARENT, maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0,0,width,height, radius, radius, radius, radius, maskLayer);
    needRedraw = true;
}

void Button::setOnClickEvent(std::function<void(void)> func){
	on_click_event = func;
}

void Button::setColor(color_t col){
    color = col;
}

// InputBox 类实现
InputBox::InputBox(int cx, int cy, double w, double h, double r)
    : centerX(cx), centerY(cy) {
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = r;
    left = centerX - width / 2;
    top = centerY - height / 2;

    btnLayer = newimage(width, height);
    maskLayer = newimage(width, height);
    ege_enable_aa(true, btnLayer);
    ege_enable_aa(true, maskLayer);
    
    // 遮罩
    setbkcolor_f(TRANSPARENT, maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, maskLayer);
    
    inv.create(false, 2);
    inv.visible(false);
    inv.move(-1, -1);
    inv.size(0, 0);
    inv.setmaxlen(2147483640);
    
    on_focus = false;
    inv.killfocus();
}

InputBox::~InputBox() {
    delimage(btnLayer);
    delimage(maskLayer);
}

void InputBox::draw(PIMAGE dst,int x,int y)  {
	left = x - width / 2;
    top = y - height / 2;
    if (on_focus) {
        inv.setfocus();
        char str[512];
        inv.gettext(512, str);
        this->content = std::string(str);
    }
    
    setbkcolor_f(EGEACOLOR(0,color), btnLayer);
    cleardevice(btnLayer);

    // 按钮背景
    setfillcolor(color, btnLayer);
    ege_fillrect(0, 0, width, height, btnLayer);

    // 更新并绘制 ripples
    for (auto& r : ripples) {
        r.update();
        r.draw(btnLayer,scale);
    }
    ripples.erase(std::remove_if(ripples.begin(), ripples.end(),
        [](const Ripple& r) { return !r.alive(); }),
        ripples.end());

    // 按钮文字
    setbkmode(TRANSPARENT, btnLayer);
    settextcolor(BLACK, btnLayer);
    setfont(23 * scale, 0, "宋体", btnLayer);
    ege_outtextxy(14, height / 2 - textheight(content.c_str(), btnLayer) / 2 - 1, 
                 content.c_str(), btnLayer);
    
    if (on_focus) {
        setfillcolor(EGEARGB(50, 30, 30, 30), btnLayer);
        ege_fillrect(0, 0, width, height, btnLayer);
    }
    // 应用遮罩绘制
    putimage_alphafilter(dst, btnLayer, left, top, maskLayer, 0, 0, -1, -1);
}

void InputBox::draw(){
    if (on_focus) {
        inv.setfocus();
        char str[512];
        inv.gettext(512, str);
        this->content = std::string(str);
    }
    
    setbkcolor_f(EGEACOLOR(0,color), btnLayer);
    cleardevice(btnLayer);

    // 按钮背景
    setfillcolor(color, btnLayer);
    ege_fillrect(0, 0, width, height, btnLayer);

    // 更新并绘制 ripples
    for (auto& r : ripples) {
        r.update();
        r.draw(btnLayer,scale);
    }
    ripples.erase(std::remove_if(ripples.begin(), ripples.end(),
        [](const Ripple& r) { return !r.alive(); }),
        ripples.end());

    // 按钮文字
    setbkmode(TRANSPARENT, btnLayer);
    settextcolor(BLACK, btnLayer);
    setfont(23 * scale, 0, "宋体", btnLayer);
    ege_outtextxy(14, height / 2 - textheight(content.c_str(), btnLayer) / 2 - 1, 
                 content.c_str(), btnLayer);
    
    if (on_focus) {
        setfillcolor(EGEARGB(50, 30, 30, 30), btnLayer);
        ege_fillrect(0, 0, width, height, btnLayer);
    }
    // 应用遮罩绘制
    putimage_alphafilter(nullptr, btnLayer, left, top, maskLayer, 0, 0, -1, -1);
}

void InputBox::handleEvent(const mouse_msg& msg) {
    if (msg.is_left() && msg.is_down() && isInside(msg.x, msg.y)) {
        int localX = msg.x - left;
        int localY = msg.y - top;
        ripples.emplace_back(localX, localY, std::max(width, height) * 2, 80);
        on_focus = true;
        inv.setfocus();
    }
    else if (msg.is_left() && msg.is_down()) {
        on_focus = false;
        inv.killfocus();
    }
}

bool InputBox::isInside(int x, int y) const {
    // 转换为按钮内部坐标系
    int localX = x - left;
    int localY = y - top;

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

void InputBox::setContent(const std::string& s) {
    content = s;
}

void InputBox::setMaxlen(int maxlen) {
    inv.setmaxlen(maxlen);
}

void InputBox::setPosition(int x,int y){
	left = x - width / 2;
	top = y - height / 2;
}

void InputBox::setScale(double s){
	width = origin_width * s;
    height = origin_height * s;
    radius = origin_radius * s;
    scale = s;
    // 遮罩
    setbkcolor_f(TRANSPARENT, maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, maskLayer);
}

// Slider 类实现
Slider::Slider()
    : left(0), top(0), width(200), height(20), m_value(0.0), m_dragging(false), m_dragOffset(0),
      m_bgColor(EGERGB(200, 200, 200)), m_fgColor(EGERGB(100, 100, 255)) {}

void Slider::create(int x, int y, double w, double h) {
    left = x;
    top = y;
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = h / 2;
    origin_thickness = thickness = 4;
}

void Slider::draw(PIMAGE dst,int x,int y){
	int left = x - width / 2;
    int top = y - height / 2;
    // if(m_pressed){
        // onPress();
    // }
    // 动态更新缩放比例
    if (m_pressed) {
        m_scale += (0.8f - m_scale) * 0.2f; // 缓动到 60%
    } else {
        m_scale += (1.0f - m_scale) * 0.2f; // 回弹
    }

    // 背景轨道
    setfillcolor(m_bgColor,dst);
    setlinecolor(m_bgColor,dst);
    
    ege_fillroundrect(left,top + height / 2 - thickness,width,thickness * 2,thickness,thickness,thickness,thickness,dst);

    // 计算滑块位置
    int knobX = left + static_cast<int>(m_value * width);
    int knobY = top + height / 2;
    double r = radius * m_scale;
    // 前景滑块
    setfillcolor(m_fgColor,dst);
    setlinecolor(BLACK,dst);
    ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2,dst);
    // 鼠标悬停时，在圆圈上加一层半透明灰色蒙版
	if (m_pressed) {
	    setfillcolor(EGERGBA(80, 80, 80, 80),dst);  // 灰色遮罩，80 alpha
	    ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2,dst);
	}
	else if((m_hover && !Lpressed)){
		setfillcolor(EGERGBA(80, 80, 80, 40),dst);  // 灰色遮罩，80 alpha
	    ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2,dst);
	}
}

void Slider::draw(){
	PIMAGE dst = nullptr;
    // if(m_pressed){
        // onPress();
    // }
    // 动态更新缩放比例
    if (m_pressed) {
        m_scale += (0.8f - m_scale) * 0.2f; // 缓动到 60%
    } else {
        m_scale += (1.0f - m_scale) * 0.2f; // 回弹
    }

    // 背景轨道
    setfillcolor(m_bgColor,dst);
    setlinecolor(m_bgColor,dst);
    
    ege_fillroundrect(left,top + height / 2 - thickness,width,thickness * 2,thickness,thickness,thickness,thickness,dst);

    // 计算滑块位置
    int knobX = left + static_cast<int>(m_value * width);
    int knobY = top + height / 2;
    double r = radius * m_scale;
    // 前景滑块
    setfillcolor(m_fgColor,dst);
    setlinecolor(BLACK,dst);
    ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2,dst);
    // 鼠标悬停时，在圆圈上加一层半透明灰色蒙版
	if (m_pressed) {
	    setfillcolor(EGERGBA(80, 80, 80, 80),dst);  // 灰色遮罩，80 alpha
	    ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2,dst);
	}
	else if((m_hover && !Lpressed)){
		setfillcolor(EGERGBA(80, 80, 80, 40),dst);  // 灰色遮罩，80 alpha
	    ege_fillellipse(knobX - r, knobY - r, r * 2, r * 2,dst);
	}
}

bool Slider::isInside(int x, int y) const {
    int knobX = left + static_cast<int>(m_value * width);
    int dx = x - knobX;
    int dy = y - (top + height / 2);
    return dx * dx + dy * dy <= radius * radius;
}

void Slider::handleEvent(const mouse_msg& msg) {
    m_hover = isInside(msg.x, msg.y);

    if (msg.is_left() && msg.is_down() && m_hover) {
        m_dragging = true;
        m_pressed = true;
        int knobX = left + static_cast<int>(m_value * width);
        m_dragOffset = msg.x - knobX;
    } else if (msg.is_move() && m_dragging) {
        int mx = clamp(msg.x - m_dragOffset, left, left + width);
        m_value = (mx - left) / static_cast<double>(width);
        if (m_onChange)
            m_onChange(m_value);
    } else if (msg.is_left() && msg.is_up()) {
        m_dragging = false;
        m_pressed = false;
    }
}

void Slider::setProgress(double v) {
    m_value = clamp(v, 0.0, 1.0);
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

void Slider::setPosition(int x,int y){
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
}

// PanelBuilder 实现
PanelBuilder& PanelBuilder::setCenter(int x, int y) {
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

Panel* PanelBuilder::build() {
    auto panel = new Panel(cx, cy, width, height, radius, bg);
    panel->setScale(scale);
    widgets.insert(panel);
    for(size_t i = 0;i < children.size();++ i){
        panel->addChild(children[i],childOffsets[i].x,childOffsets[i].y);
    }
    return panel;
}

// ButtonBuilder 实现
ButtonBuilder& ButtonBuilder::setCenter(int x, int y) {
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

ButtonBuilder& ButtonBuilder::setContent(const std::string& text) {
    content = text;
    return *this;
}

ButtonBuilder& ButtonBuilder::setScale(double s) {
    scale = s;
    return *this;
}

ButtonBuilder& ButtonBuilder::setOnClick(std::function<void()> func) {
    onClick = func;
    return *this;
}

ButtonBuilder& ButtonBuilder::setColor(color_t col){
    color = col;
    return *this;
}

Button* ButtonBuilder::build() {
    auto btn = new Button(cx, cy, width, height, radius);
    btn->setContent(content);
    btn->setScale(scale);
    btn->setColor(color);
    if (onClick) btn->setOnClickEvent(onClick);
    widgets.insert(btn);
    return btn;
}

// InputBoxBuilder 实现
InputBoxBuilder& InputBoxBuilder::setCenter(int x, int y) {
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

InputBoxBuilder& InputBoxBuilder::setContent(const std::string& text) {
    content = text;
    return *this;
}

InputBoxBuilder& InputBoxBuilder::setMaxLength(int maxLen) {
    maxLength = maxLen;
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
    widgets.insert(input);
    return input;
}

SliderBuilder& SliderBuilder::setCenter(int x_, int y_) {
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

Slider* SliderBuilder::build() {
    auto slider = new Slider();
    slider->create(x, y, width, height);
    slider->setColor(bgColor, fgColor);
    slider->setThickness(thickness);
    slider->setProgress(progress);
    slider->setScale(scale);
    if (onChange) slider->setOnChange(onChange);
    widgets.insert(slider);
    return slider;
}
