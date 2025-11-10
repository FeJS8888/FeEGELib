#include "Widget.h"

using namespace FeEGE;
set<Widget*> widgets;

double Widget::getWidth(){
    return width;
}
double Widget::getHeight(){
    return height;
}

Widget::~Widget() {
    // 从全局widgets集合中移除
    widgets.erase(this);
    
    // 从IdToWidget映射中移除所有指向this的条目
    for (auto it = IdToWidget.begin(); it != IdToWidget.end(); ) {
        if (it->second == this) {
            it = IdToWidget.erase(it);
        } else {
            ++it;
        }
    }
}

Panel::Panel(int cx, int cy, double w, double h, double r, color_t bg) {
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
	
	setbkcolor_f(EGEARGB(0, 255, 255, 255), maskLayer);
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

void Panel::draw(PIMAGE dst, int x, int y) {
    if (layout) layout->apply(*this);  // 自动计算子控件位置
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
    if (maskLayer) delimage(maskLayer);
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
	
	setbkcolor_f(EGEARGB(0, 255, 255, 255), maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, maskLayer);
}

double Panel::getScale(){
	return scale;
}

void Panel::handleEvent(const mouse_msg& msg){
	
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
	
	setbkcolor_f(EGERGBA(0,0,0,0), maskLayer);
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
    setbkcolor_f(EGERGBA(0,0,0,0), maskLayer);
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

void Ripple::draw(PIMAGE dst,double s) const {
    double progress = (double)age / life;
    double r = maxRadius * progress * s;
    int alpha = static_cast<int>(120 * std::cos(progress * PI / 2));
    setfillcolor(EGEARGB(alpha, 30, 30, 30), dst);
    ege_fillellipse(x - r, y - r, r * 2, r * 2, dst);
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
    setbkcolor_f(EGERGBA(0,0,0,0), maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGERGBA(255,255,255,255), maskLayer);
    ege_fillroundrect(0.25,0.25,width - 0.5,height - 0.5, radius, radius, radius, radius, maskLayer);
}

Button::~Button() {
    if (btnLayer) delimage(btnLayer);
    if (maskLayer) delimage(maskLayer);
    if (bgLayer) delimage(bgLayer);
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
    bool inside = isInside(msg.x, msg.y);
    if (msg.is_left() && msg.is_down() && inside) {
        int localX = msg.x - left;
        int localY = msg.y - top;
        m_counter++;
        ripples.emplace_back(localX, localY, 4.00f / 3.00f * std::sqrt(height * height + width * width), 70,dynamic_cast<Widget*>(this),m_counter);
        needRedraw = true;
        m_clicking = true;
    }
    else if(msg.is_left() && msg.is_up()){
        if(inside && on_click_event != nullptr) on_click_event();
        m_clicking = false;
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

void Button::setContent(const wstring& str){
    if(content == str) return;
	content = str;
    needRedraw = true;
}

std::wstring Button::getContent(){
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
    setbkcolor_f(EGERGBA(0,0,0,0), maskLayer);
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
    bgLayer = newimage(width,height);
    ege_enable_aa(true, btnLayer);
    ege_enable_aa(true, maskLayer);
    ege_enable_aa(true, bgLayer);
    // 遮罩
    setbkcolor_f(EGERGBA(0,0,0,0), maskLayer);
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

void InputBox::draw(PIMAGE dst, int x, int y) {
    double left = x - width / 2;
    double top = y - height / 2;
    
    if (on_focus) {
        adjustScrollForCursor();
        inv.setfocus();
        wchar_t str[512];
        inv.gettext(512, str);
        setContent(str);
    }
    
    if(!on_focus && !ripples.size() && !needRedraw){
        putimage_withalpha(dst, bgLayer, left, top);
        return;
    }

    std::wstring content = IMECompositionString.size() ? 
        (this->content.substr(0,cursor_pos) + IMECompositionString + this->content.substr(cursor_pos)) : 
        (this->content);
        
    setbkcolor_f(EGEACOLOR(0,color), btnLayer);
    cleardevice(btnLayer);
    setbkcolor_f(EGEACOLOR(0,color), bgLayer);
    cleardevice(bgLayer);

    // 按钮背景
    setfillcolor(EGEACOLOR(255, color), btnLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, btnLayer);

    // 更新并绘制 ripples
    for (auto& r : ripples) {
        r.update();
        r.draw(btnLayer, scale);
    }
    ripples.erase(std::remove_if(ripples.begin(), ripples.end(),
        [](const Ripple& r) { return !r.alive(); }),
        ripples.end());

    // 输入框文字
    // std::wcout << L"FontManager loaded: " << (fontManager.IsLoaded() ? L"YES" : L"NO") << std::endl;
    // std::wcout << L"Font name: " << fontManager.GetFontName() << std::endl;

    // LOGFONTW lf = fontManager.CreateLogFont(40);
    // std::wcout << L"Created font face name: " << lf.lfFaceName << std::endl;
    setbkmode(TRANSPARENT, btnLayer);
    settextcolor(BLACK,btnLayer);
    setfont(scale * text_height,0,L"宋体",btnLayer);
    // ege_outtextxy(0,0,"Hello,World",btnLayer);

    if(sgn(scale * text_height - 1) >= 0){
        // 计算文本显示参数 - 修改开始
        const float padding = 14;
        
        // 修正变量覆盖问题，分别计算各个宽度
        std::wstring cursor_before_cursor = content.substr(0, cursor_pos) + IMECompositionString.substr(0, IMECursorPos);
        std::wstring cursor_before_text = content.substr(0, cursor_pos) + IMECompositionString;
        float cursor_pos_width, cursor_with_ime_width, tmp, full_text_width,cursor_with_full_ime_width;
        measuretext(content.substr(0, cursor_pos).c_str(), &cursor_pos_width, &tmp, btnLayer);
        measuretext(cursor_before_cursor.c_str(), &cursor_with_ime_width, &tmp, btnLayer);
        measuretext(cursor_before_text.c_str(), &cursor_with_full_ime_width, &tmp, btnLayer);
        measuretext(content.c_str(), &full_text_width, &tmp, btnLayer);
        float textRealHeight = tmp ? tmp : textheight("a", btnLayer);
        
        // 使用 scroll_offset 替代复杂的滚动逻辑
        float text_start_x = padding - scroll_offset;
        // 修改结束
        
        // 绘制文本
        ege_outtextxy(text_start_x, height / 2 - textRealHeight / 2, 
                    content.c_str(), btnLayer);
        
        if (on_focus) {
            // 聚焦状态遮罩
            setfillcolor(EGEARGB(50,30,30,30), btnLayer);
            ege_fillrect(0, 0, width, height, btnLayer);

            // 绘制光标
            std::chrono::_V2::system_clock::time_point current_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_time = current_time - start_time;
            double cursor_opacity = InputBoxSinDoubleForCursor(elapsed_time.count());
            setfillcolor(EGEARGB((char)(cursor_opacity * 255),255,255,0), btnLayer);
            
            // 使用新的光标位置计算
            float cursor_draw_x = text_start_x + cursor_with_ime_width;
            
            // 绘制光标
            ege_fillrect(cursor_draw_x, height / 2 - textRealHeight / 2 - 3.5, 
                    2, textRealHeight + 7, btnLayer);
            
            // IME输入下划线
            if(IMECompositionString.size()){
                setlinestyle(DOTTED_LINE, 0U, 1, btnLayer);
                setlinecolor(EGEARGB(255,0,0,0), btnLayer);
                
                // 使用新的位置计算
                float ime_start_x = text_start_x + cursor_pos_width;
                float ime_end_x = text_start_x + cursor_with_full_ime_width;
                
                ege_line(ime_start_x, height / 2 + textRealHeight / 2 + 2,
                    ime_end_x, height / 2 + textRealHeight / 2 + 2, btnLayer);
                setlinestyle(SOLID_LINE, 0U, 1, btnLayer);
            }
            
            // 更新IME位置
            InputPositionX = left + cursor_draw_x;
            InputPositionY = top + height - 20;
        }
    }
    
    // 应用遮罩绘制
    // putimage(0,0,btnLayer);
    putimage_alphafilter(bgLayer, btnLayer, 0, 0, maskLayer, 0, 0, -1, -1);
    putimage_withalpha(dst,bgLayer,left,top);
    needRedraw = false;
}

void InputBox::draw(){
    draw(nullptr,cx,cy);
}

void InputBox::handleEvent(const mouse_msg& msg) {
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
        const float padding = 14;
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
    }
    // 鼠标左键按下且不在输入框内
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
    // cout<<"IS localX: "<<localX<<" localY: "<<localY<<"WHICH is " <<EGEGET_A(getpixel(localX, localY, btnLayer))<<endl;

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
    setbkcolor_f(EGERGBA(0,0,0,0), maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, maskLayer);
    needRedraw = true;
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
    const float padding = 14;
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

bool Slider::isInside(int x, int y){
    int knobX, knobY;
    if (m_orientation == Orientation::Column) {
        knobX = left + width / 2;
        knobY = top + static_cast<int>((1.0 - m_progress) * height);
        radius = width / 2;
    } else { // Row
        knobX = left + static_cast<int>(m_progress * width);
        knobY = top + height / 2;
        radius = height / 2;
    }
    int dx = x - knobX;
    int dy = y - knobY;
    return dx * dx + dy * dy <= radius * radius;
}

bool Slider::isInsideBar(int x, int y){
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

void Slider::handleEvent(const mouse_msg& msg) {
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
    } 
    else if (msg.is_left() && msg.is_up()) {
        m_dragging = false;
        m_pressed = false;
        m_finalprogress = fixProgress();
    }
}

void Slider::setProgress(double v) {
    m_progress = clamp(v, 0.0, 1.0);
    m_value = m_progress = fixProgress();
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

    //缓动到目标进度
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
PanelBuilder& PanelBuilder::setIdentifier(const wstring& id) {
    identifier = id;
    return *this;
}

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

PanelBuilder& PanelBuilder::setLayout(std::shared_ptr<Layout> l) {
    layout = std::move(l);
    return *this;
}

Panel* PanelBuilder::build() {
    auto panel = new Panel(cx, cy, width, height, radius, bg);
    panel->setScale(scale);
    widgets.insert(panel);
    IdToWidget[identifier] = panel;
    if (layout) panel->setLayout(layout);
    for(size_t i = 0;i < children.size();++ i){
        panel->addChild(children[i],childOffsets[i].x,childOffsets[i].y);
    }
    return panel;
}

// ButtonBuilder 实现
ButtonBuilder& ButtonBuilder::setIdentifier(const wstring& id) {
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
    widgets.insert(btn);
    IdToWidget[identifier] = btn;
    return btn;
}

// InputBoxBuilder 实现
InputBoxBuilder& InputBoxBuilder::setIdentifier(const wstring& id) {
    identifier = id;
    return *this;
}

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
    widgets.insert(input);
    IdToWidget[identifier] = input;
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

void Dropdown::setContent(const std::wstring& text) {
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
    widgets.insert(dropdown);
    return dropdown;
}

Radio::Radio(int cx, int cy, double r, const std::wstring& val)
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
    widgets.insert(radio);
    return radio;
}

RadioController::RadioController(int cx, int cy, double r, double gap, double scale, RadioStyle style)
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
    widgets.insert(toggle);
    IdToWidget[identifier] = toggle;
    return toggle;
}

// 构造函数
Text::Text(int x, int y, int maxW)
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

TextBuilder& TextBuilder::setIdentifier(const wstring& id) {
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

    setfont((int)(r * 0.6), 0, L"Consolas");
}

void Knob::draw() {
    draw(nullptr, cx, cy);
}

void Knob::handleEvent(const mouse_msg& msg) {
    // 拖动逻辑将在下一步实现
}


Sidebar::Sidebar(int cx, int cy, double w, double h, double r, color_t bg) {
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

void Sidebar::draw(PIMAGE dst, int x, int y) {
    container->draw(dst, x, y);
}

void Sidebar::draw() {
    container->draw();
}

void Sidebar::handleEvent(const mouse_msg& msg) {
    container->handleEvent(msg);
}

void Sidebar::setPosition(int x, int y) {
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
SidebarBuilder& SidebarBuilder::setCenter(int x, int y) {
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
    widgets.insert(sidebar);
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
