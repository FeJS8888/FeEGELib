#include "Widget.h"

using namespace FeEGE;
set<Widget*> widgets;

Widget::~Widget() {}

Panel::Panel(int cx, int cy, double w, double h, double r, color_t bg) {
    this->cx = cx;
    this->cy = cy;
    this->width = width;
    this->height = height;
    this->bgColor = bgColor;
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = r;
	ege_enable_aa(true,layer);
    ege_enable_aa(true,maskLayer);
	
	setbkcolor_f(TRANSPARENT, maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB((int)alpha, 255, 255, 255), maskLayer);
    ege_fillroundrect(0.25, 0.25, width - 0.5, height - 0.5, radius, radius, radius, radius, maskLayer);
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
    setfillcolor(EGEARGB((int)alpha, 255, 255, 255), maskLayer);
    ege_fillroundrect(0.25, 0.25, width - 0.5, height - 0.5, radius, radius, radius, radius, maskLayer);
}

double Panel::getScale(){
	return scale;
}

void Panel::handleEvent(const mouse_msg& msg){
	
}

void Panel::setSize(double w,double h){
    origin_width = width = w;
    origin_height = height = h;
    delimage(layer);
    delimage(maskLayer);
    layer = newimage(width,height);
    maskLayer = newimage(width,height);
    ege_enable_aa(true,layer);
    ege_enable_aa(true,maskLayer);
	
	setbkcolor_f(TRANSPARENT, maskLayer);
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
    setbkcolor_f(TRANSPARENT, maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB((int)alpha, 255, 255, 255), maskLayer);
    ege_fillroundrect(0.25, 0.25, width - 0.5, height - 0.5, radius, radius, radius, radius, maskLayer);
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
Button::Button(int cx, int cy, double w, double h, double r): radius(r) {
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
    
    // 遮罩
    setbkcolor_f(TRANSPARENT, maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGERGBA(255,255,255,255), maskLayer);
    ege_fillroundrect(0.25,0.25,width - 0.5,height - 0.5, radius, radius, radius, radius, maskLayer);
}

Button::~Button() {
    delimage(btnLayer);
    delimage(maskLayer);
}

void Button::draw(PIMAGE dst,int x,int y){
    int left = x - width / 2;
    int top = y - height / 2;
    if(!ripples.size() && !needRedraw){
        putimage_withalpha(dst,bgLayer,left,top);
        return;
    }
    setbkcolor_f(EGEACOLOR(0,color), btnLayer);
    setbkcolor_f(EGEACOLOR(0,color), bgLayer);
    cleardevice(btnLayer);
    cleardevice(bgLayer);

    // 按钮背景
    setfillcolor(color, btnLayer);
    setfillcolor(color, bgLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, btnLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, bgLayer);
                 
    if(icon != nullptr){
	    int iconW = getwidth(icon) * scale * iconSize / 100;
	    int iconH = getheight(icon) * scale * iconSize / 100;
	    int iconX = width / 2 - iconW / 2;
	    int iconY = height / 2 - iconH / 2;
	    putimage_alphablend(btnLayer,icon,iconX,iconY,iconW,iconH,255,0, 0,getwidth(icon), getwidth(icon),true);
	}
    
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
    putimage_alphafilter(bgLayer, btnLayer, 0, 0, maskLayer, 0, 0, -1, -1);
    putimage_withalpha(dst,bgLayer,left,top);
    needRedraw = false;
}

void Button::draw(){
    draw(nullptr,cx,cy);
}

void Button::handleEvent(const mouse_msg& msg) {
    if (msg.is_left() && msg.is_down() && isInside(msg.x, msg.y)) {
        int localX = msg.x - left;
        int localY = msg.y - top;
        ripples.emplace_back(localX, localY, std::max(width, height) * 1.8, 40);
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

// InputBox 类实现
InputBox::InputBox(int cx, int cy, double w, double h, double r) {
    this->cx = cx;
    this->cy = cy;
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = r;
    left = cx - width / 2;
    top = cy - height / 2;

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

void InputBox::draw(PIMAGE dst,int x,int y) {
	int left = x - width / 2;
    int top = y - height / 2;
    if (on_focus) {
        inv.setfocus();
        char str[512];
        inv.gettext(512, str);
        setContent(str);
    }
    if(!ripples.size() && !needRedraw){
        putimage_withalpha(dst, btnLayer, left, top);
        return;
    }
    
    setbkcolor_f(EGEACOLOR(0,color), btnLayer);
    cleardevice(btnLayer);

    // 按钮背景
    setfillcolor(color, btnLayer);
    ege_fillroundrect(0, 0, width, height,radius,radius,radius,radius, btnLayer);

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
    needRedraw = false;
}

void InputBox::draw(){
    draw(nullptr,cx,cy);
}

void InputBox::handleEvent(const mouse_msg& msg) {
    if (msg.is_left() && msg.is_down() && isInside(msg.x, msg.y)) {
        int localX = msg.x - left;
        int localY = msg.y - top;
        ripples.emplace_back(localX, localY, std::max(width, height) * 1.8, 40);
        on_focus = true;
        needRedraw = true;
        inv.setfocus();
    }
    else if (msg.is_left() && msg.is_down()) {
        on_focus = false;
        inv.killfocus();
        needRedraw = true;
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
    if(content == s) return;
    content = s;
    needRedraw = true;
}

void InputBox::setMaxlen(int maxlen) {
    inv.setmaxlen(maxlen);
}

void InputBox::setPosition(int x,int y){
    if(sgn(left - (x - width / 2)) == 0 && sgn(top - (y - height / 2)) == 0) return;
	left = x - width / 2;
	top = y - height / 2;
    needRedraw = true;
}

void InputBox::setScale(double s){
    if(sgn(scale - s) == 0) return;
	width = origin_width * s;
    height = origin_height * s;
    radius = origin_radius * s;
    scale = s;
    // 遮罩
    setbkcolor_f(TRANSPARENT, maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, maskLayer);
    needRedraw = true;
}

// Slider 类实现
Slider::Slider()
    : left(0), top(0), m_value(0.0), m_dragging(false), m_dragOffset(0),
      m_bgColor(EGERGB(200, 200, 200)), m_fgColor(EGERGB(100, 100, 255)) {
        this->width = 0;
        this->height = 0;
      }

void Slider::create(int x, int y, double w, double h) {
    cx = x;
    cy = y;
    left = x - w / 2;
    top = y - h / 2;
    origin_width = width = w;
    origin_height = height = h;
    origin_radius = radius = h / 2;
    origin_thickness = thickness = 4;
}

void Slider::draw(PIMAGE dst,int x,int y){
	int left = x - width / 2;
    int top = y - height / 2;
    // 动态更新缩放比例
    if (m_pressed) {
        m_scale += (0.8f - m_scale) * 0.2f; // 缓动到 60%
    } else {
        m_scale += (1.0f - m_scale) * 0.2f; // 回弹
    }

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
        int knobY = top + static_cast<int>((1.0 - m_value) * height);
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

        int knobX = left + static_cast<int>(m_value * width);
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

bool Slider::isInside(int x, int y){
    int knobX, knobY;
    if (m_orientation == Orientation::Column) {
        knobX = left + width / 2;
        knobY = top + static_cast<int>((1.0 - m_value) * height);
        radius = width / 2;
    } else { // Row
        knobX = left + static_cast<int>(m_value * width);
        knobY = top + height / 2;
        radius = height / 2;
    }
    int dx = x - knobX;
    int dy = y - knobY;
    return dx * dx + dy * dy <= radius * radius;
}

void Slider::handleEvent(const mouse_msg& msg) {
    m_hover = isInside(msg.x, msg.y);

    if (msg.is_left() && msg.is_down() && m_hover) {
        m_dragging = true;
        m_pressed = true;
        int knobX, knobY;
        if (m_orientation == Orientation::Column) {
            knobX = left + width / 2;
            knobY = top + static_cast<int>((1.0 - m_value) * height);
            m_dragOffset = msg.y - knobY;
        } else { // Row
            knobX = left + static_cast<int>(m_value * width);
            knobY = top + height / 2;
            m_dragOffset = msg.x - knobX;
        }
    } 
    else if (msg.is_move() && m_dragging) {
        if (m_orientation == Orientation::Column) {
            int my = clamp(msg.y - m_dragOffset, top, top + height);
            m_value = 1.0 - (my - top) / static_cast<double>(height);
        } else { // Row
            int mx = clamp(msg.x - m_dragOffset, left, left + width);
            m_value = (mx - left) / static_cast<double>(width);
        }
        if (m_onChange)
            m_onChange(m_value);
    } 
    else if (msg.is_left() && msg.is_up()) {
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

void Slider::setOrientation(Orientation ori){
    m_orientation = ori;
}

ProgressBar::ProgressBar(int cx, int cy, double w, double h):
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

void ProgressBar::draw(PIMAGE dst, int x, int y) {
    int left = x - width / 2;
    int top = y - height / 2;

    // 缓动到目标进度
    currentProgress += (targetProgress - currentProgress) * 0.15;
    if (fabs(currentProgress - targetProgress) < 0.005)
        currentProgress = targetProgress;

    if (!needRedraw && fabs(currentProgress - targetProgress) < 1e-4) {
        putimage_withalpha(dst, barLayer, left, top);
        return;
    }

    // 重绘
    setbkcolor_f(EGEACOLOR(0, bgColor), barLayer);
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

void ProgressBar::handleEvent(const mouse_msg& msg){

}

void ProgressBar::setPosition(int x, int y) {
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
    needRedraw = true;
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
ButtonBuilder& ButtonBuilder::setIdentifier(const string& id) {
    identifier = id;
    return *this;
}

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
    widgets.insert(btn);
    IdToWidget[identifier] = btn;
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

SliderBuilder& SliderBuilder::setOrientation(Orientation ori){
    orientation = ori;
    return *this;
}

Slider* SliderBuilder::build() {
    auto slider = new Slider();
    slider->create(x,y, width, height);
    slider->setColor(bgColor, fgColor);
    slider->setThickness(thickness);
    slider->setProgress(progress);
    slider->setScale(scale);
    if (onChange) slider->setOnChange(onChange);
    slider->setOrientation(orientation);
    widgets.insert(slider);
    return slider;
}

ProgressBarBuilder& ProgressBarBuilder::setCenter(int x, int y) {
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

ProgressBar* ProgressBarBuilder::build() {
    auto bar = new ProgressBar(cx, cy, width, height);
    bar->setColor(fgColor);
    bar->setBackground(bgColor);
    bar->setProgress(progress);
    bar->setScale(scale);
    widgets.insert(bar);
    return bar;
}

Dropdown::Dropdown(int cx, int cy, double w, double h, double r)
    : cx(cx), cy(cy), width(w), height(h), radius(r) {
    mainButton = new Button(cx, cy, w, h, r);
    mainButton->setOnClickEvent([this] { 
        toggleDropdown(); 
    });
    dropdownPanel = new Panel(cx, cy + h, w, 0, r, color);  // 初始化为高度0
    dropdownPanel->setScale(scale);
}

void Dropdown::addOption(const std::string& text, std::function<void()> onClick) {
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

void Dropdown::draw(PIMAGE dst, int x, int y) {
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

        // 设置透明度并绘制下拉面板
        int actualAlpha = static_cast<int>(fadeAlpha * 255);
        dropdownPanel->setAlpha(actualAlpha);
        dropdownPanel->draw(dst, cx, cy + height / 2 + height * options.size() / 2 + 4);
    }
}

void Dropdown::draw() {
    draw(nullptr, cx, cy);
}

void Dropdown::handleEvent(const mouse_msg& msg) {
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

void Dropdown::setPosition(int x, int y) {
    cx = x; cy = y;
    mainButton->setPosition(x, y);
    updateDropdownLayout();
}

void Dropdown::setScale(double s) {
    scale = s;
    mainButton->setScale(s);
    updateDropdownLayout();
}

void Dropdown::setContent(const std::string& text) {
    mainButton->setContent(text);
}

void Dropdown::setColor(color_t col) {
    color = col;
    mainButton->setColor(col);
    for (auto& opt : options)
        opt->setColor(col);
}

bool Dropdown::isInside(int x, int y) const {
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

DropdownBuilder& DropdownBuilder::setCenter(int x, int y) {
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

DropdownBuilder& DropdownBuilder::setContent(const std::string& text) {
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

DropdownBuilder& DropdownBuilder::addOption(const std::string& text, std::function<void()> onClick) {
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
    widgets.insert(dropdown);
    return dropdown;
}

Radio::Radio(int cx, int cy, double r, const std::string& val)
    : cx(cx), cy(cy), radius(r), origin_radius(r), value(val) {}

void Radio::setPosition(int x, int y) {
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

void Radio::setGroupValueRef(std::string* ref) {
    groupValuePtr = ref;
}

void Radio::setOnSelect(std::function<void()> callback) {
    onSelect = callback;
}

std::string Radio::getValue() const {
    return value;
}

bool Radio::isChecked() const {
    return groupValuePtr && *groupValuePtr == value;
}

void Radio::draw(PIMAGE dst, int x, int y) {
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

void Radio::handleEvent(const mouse_msg& msg) {
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
        } else if (groupValuePtr && *groupValuePtr == value) {
            // cout<<"awa\n";
            // animOut = true;
            // animIn = false;
            // animProgress = 0.0;
        }
    }
}

RadioBuilder& RadioBuilder::setCenter(int x, int y) {
    cx = x; cy = y; return *this;
}
RadioBuilder& RadioBuilder::setRadius(double r) {
    radius = r; return *this;
}
RadioBuilder& RadioBuilder::setScale(double s) {
    scale = s; return *this;
}
RadioBuilder& RadioBuilder::setValue(const std::string& val) {
    value = val; return *this;
}
RadioBuilder& RadioBuilder::setGroupValueRef(std::string* ref) {
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
    widgets.insert(radio);
    return radio;
}

RadioController::RadioController(int cx, int cy, double r, double gap, double scale, RadioStyle style)
    : cx(cx), cy(cy), radius(r), gap(gap), scale(scale), style(style) {}

void RadioController::addValue(const std::string& val) {
    values.push_back(val);
}

void RadioController::setDefault(const std::string& val) {
    currentValue = val;
}

void RadioController::setOnChange(std::function<void(const std::string&)> cb) {
    onChange = cb;
}

std::string RadioController::getValue(){
    return currentValue;
}

void RadioController::build() {
    for (size_t i = 0; i < values.size(); ++i) {
        std::string val = values[i];
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

RadioControllerBuilder& RadioControllerBuilder::setCenter(int x, int y) {
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

RadioControllerBuilder& RadioControllerBuilder::add(const std::string& val) {
    values.push_back(val);
    return *this;
}

RadioControllerBuilder& RadioControllerBuilder::setDefault(const std::string& val) {
    defaultValue = val;
    return *this;
}

RadioControllerBuilder& RadioControllerBuilder::setOnChange(std::function<void(const std::string&)> cb) {
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

Toggle::Toggle(int cx, int cy, double w, double h)
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

void Toggle::setPosition(int x, int y) {
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

void Toggle::handleEvent(const mouse_msg& msg) {
    double w = width * scale;
    double h = height * scale;
    double r = h / 2;

    int dx = msg.x - cx;
    int dy = msg.y - cy;

    hovered = (std::abs(dx) <= w / 2 && std::abs(dy) <= h / 2);

    if (disabled) return;

    if (msg.is_left()) {
        if (msg.is_down()) {
            // 记录是否在区域内按下
            pressedIn = hovered;
        } else if (msg.is_up()) {
            // 如果松开时还在区域内且是之前按下的
            if (hovered && pressedIn) {
                toggle(); // ? 这里才执行切换动画与回调
            }
            pressedIn = false;
        }
    }
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

void Toggle::draw(PIMAGE dst, int x, int y) {
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

ToggleBuilder& ToggleBuilder::setCenter(int x, int y) {
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

ToggleBuilder& ToggleBuilder::setIdentifier(const std::string& id){
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
    widgets.insert(toggle);
    IdToWidget[identifier] = toggle;
    return toggle;
}

// 构造函数
Text::Text(int x, int y, int maxW)
    : posX(x), posY(y), maxWidth(maxW) {}

// 设置文本
void Text::setContent(const std::string& text) {
    contentW = autoToWString(text);
    updateLayout();
}

void Text::setMaxWidth(int width) {
    maxWidth = width;
    updateLayout();
}

void Text::setFont(int size, const std::string& name) {
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

void Text::setPosition(int x, int y) {
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
    textWidth = 0;
    textHeight = 0;

    setfont((int)(fontSize * scale), 0, fontName.c_str());

    std::wstring line;
    for (wchar_t ch : contentW) {
        if (ch == L'\n') {
            lines.push_back(line);
            line.clear();
            continue;
        }

        line += ch;

        if (maxWidth > 0 && textwidth(line.c_str()) > maxWidth) {
            line.pop_back();
            lines.push_back(line);
            line = ch;
        }
    }

    if (!line.empty())
        lines.push_back(line);

    for (const auto& l : lines)
        textWidth = std::max(textWidth, textwidth(l.c_str()));

    textHeight = lines.size() * textheight("A");
}

// 默认绘制
void Text::draw() {
    draw(nullptr, posX, posY);
}

// 绘制到目标图像
void Text::draw(PIMAGE dst, int x, int y) {
    setfont((int)(fontSize * scale), 0, fontName.c_str(),dst);
    settextcolor(color,dst);

    for (size_t i = 0; i < lines.size(); ++i) {
        int x_draw = x;
        int lineW = textwidth(lines[i].c_str(),dst);

        if (align == TextAlign::Center)
            x_draw = x + (maxWidth - lineW) / 2;
        else if (align == TextAlign::Right)
            x_draw = x + (maxWidth - lineW);

        int y_draw = y + (int)(i * (textheight("A",dst) + lineSpacing));
        ege_outtextxy(x_draw, y_draw, lines[i].c_str(), dst);
    }
}

void Text::handleEvent(const mouse_msg& msg) {
    // Text 是纯展示控件，不处理事件
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

TextBuilder& TextBuilder::setIdentifier(const string& id) {
    identifier = id;
    return *this;
}

TextBuilder& TextBuilder::setPosition(int px, int py) {
    x = px; y = py;
    return *this;
}

TextBuilder& TextBuilder::setMaxWidth(int w) {
    maxWidth = w;
    return *this;
}

TextBuilder& TextBuilder::setFont(int size, const std::string& name) {
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

TextBuilder& TextBuilder::setContent(const std::string& text) {
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
    widgets.insert(txt);
    return txt;
}

Knob::Knob(int cx, int cy, double r)
    : cx(cx), cy(cy), radius(r) {}

void Knob::setRange(double minVal, double maxVal) {
    minValue = minVal;
    maxValue = maxVal;
}

void Knob::setStep(double s) {
    step = s;
}

void Knob::setValue(double val) {
    value = clamp(val);
}

double Knob::getValue() const {
    return value;
}

void Knob::setColor(color_t fg, color_t bg) {
    fgColor = fg;
    bgColor = bg;
}

void Knob::setScale(double s) {
    scale = s;
}

void Knob::setPosition(int x, int y) {
    cx = x;
    cy = y;
}

void Knob::setOnChange(std::function<void(double)> cb) {
    onChange = cb;
}

double Knob::clamp(double v) {
    if (v < minValue) return minValue;
    if (v > maxValue) return maxValue;
    return v;
}

// 从值计算角度 (起始为 135°, 终止为 405°)
double Knob::valueToAngle(double v) const {
    double ratio = (v - minValue) / (maxValue - minValue);
    return 135.0 + ratio * 270.0;
}

void Knob::draw(PIMAGE dst, int x, int y) {
    double r = radius * scale;
    double cx = x, cy = y;

    // === 背景圆环 ===
    setlinecolor(bgColor, dst);
    setlinewidth((int)(r * 0.2), dst);
    ege_arc((float)(cx - r), (float)(cy - r), (float)(2 * r), (float)(2 * r), 135.0f, 270.0f, dst);


    // === 前景圆环（值）===
    setlinecolor(fgColor, dst);
    setlinewidth((int)(r * 0.2), dst);
    double angle = valueToAngle(value);
    ege_arc((float)(cx - r), (float)(cy - r), (float)(2 * r), (float)(2 * r), 135.0f, angle - 135.0f, dst);

    // === 内部圆 ===
    setfillcolor(WHITE, dst);
    setlinecolor(TRANSPARENT, dst);
    fillellipse((int)(cx), (int)(cy), (int)(r * 1.2), (int)(r * 1.2), dst);

    // === 当前值显示 ===
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << value;
    std::string valStr = oss.str();

    setfont((int)(r * 0.6), 0, "Consolas");

    settextcolor(BLACK);
    int tw = textwidth(valStr.c_str());
    int th = textheight(valStr.c_str());
    outtextxy((int)(cx - tw / 2), (int)(cy - th / 2), valStr.c_str());
}

void Knob::draw() {
    draw(nullptr, cx, cy);
}

void Knob::handleEvent(const mouse_msg& msg) {
    // 拖动逻辑将在下一步实现
}

std::map<std::string,Widget*> IdToWidget;

Widget* getWidgetById(const std::string& identifier){
    return IdToWidget[identifier];
}
