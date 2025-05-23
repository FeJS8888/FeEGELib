#include "Control.h"

using namespace FeEGE;

// Ripple 结构体实现
Ripple::Ripple(int _x, int _y, int _r, int _life)
    : x(_x), y(_y), maxRadius(_r), life(_life) {}

bool Ripple::alive() const { return age < life; }

void Ripple::update() { age++; }

void Ripple::draw(PIMAGE dst) const {
    double progress = (double)age / life;
    int r = static_cast<int>(maxRadius * progress);
    int alpha = static_cast<int>(100 * (1.0 - progress));
    setfillcolor(EGEARGB(alpha, 30, 30, 30), dst);
    ege_fillellipse(x - r / 2, y - r / 2, r, r, dst);
}

// Button 类实现
Button::Button(int cx, int cy, int w, int h, int r)
    : centerX(cx), centerY(cy), width(w), height(h), radius(r) {
    left = centerX - width / 2;
    top = centerY - height / 2;

    btnLayer = newimage(width, height);
    maskLayer = newimage(width, height);
    ege_enable_aa(true, btnLayer);
    ege_enable_aa(true, maskLayer);

    setbkcolor_f(TRANSPARENT, btnLayer);
    cleardevice(btnLayer);
    
    // 遮罩
    setbkcolor_f(TRANSPARENT, maskLayer);
    cleardevice(maskLayer);
    setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, maskLayer);
}

Button::~Button() {
    delimage(btnLayer);
    delimage(maskLayer);
}

void Button::draw(const std::string& content) {
    setbkcolor_f(TRANSPARENT, btnLayer);
    cleardevice(btnLayer);

    // 按钮背景
    setfillcolor(EGERGB(245, 245, 235), btnLayer);
    setcolor(EGERGB(245, 245, 235), btnLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, btnLayer);

    // 更新并绘制 ripples
    for (auto& r : ripples) {
        r.update();
        r.draw(btnLayer);
    }
    ripples.erase(std::remove_if(ripples.begin(), ripples.end(),
        [](const Ripple& r) { return !r.alive(); }),
        ripples.end());

    // 按钮文字
    setbkmode(TRANSPARENT, btnLayer);
    settextcolor(BLACK, btnLayer);
    setfont(23, 0, "宋体", btnLayer);
    ege_outtextxy(width / 2 - textwidth(content.c_str(), btnLayer) / 2, 
                 height / 2 - textheight(content.c_str(), btnLayer) / 2, 
                 content.c_str(), btnLayer);
    
    // 应用遮罩绘制
    putimage_alphafilter(nullptr, btnLayer, left, top, maskLayer, 0, 0, -1, -1);
}

void Button::handleEvent(const mouse_msg& msg) {
    if (msg.is_left() && msg.is_down() && isInside(msg.x, msg.y)) {
        int localX = msg.x - left;
        int localY = msg.y - top;
        ripples.emplace_back(localX, localY, std::max(width, height) * 2, 80);
    }
}

bool Button::isInside(int x, int y) {
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

// InputBox 类实现
InputBox::InputBox(int cx, int cy, int w, int h, int r)
    : centerX(cx), centerY(cy), width(w), height(h), radius(r) {
    left = centerX - width / 2;
    top = centerY - height / 2;

    btnLayer = newimage(width, height);
    maskLayer = newimage(width, height);
    ege_enable_aa(true, btnLayer);
    ege_enable_aa(true, maskLayer);

    setbkcolor_f(TRANSPARENT, btnLayer);
    cleardevice(btnLayer);
    
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

void InputBox::draw() {
    if (on_focus) {
        inv.setfocus();
        char str[512];
        inv.gettext(512, str);
        this->content = std::string(str);
    }
    
    setbkcolor_f(TRANSPARENT, btnLayer);
    cleardevice(btnLayer);

    // 按钮背景
    setfillcolor(EGERGB(245, 245, 235), btnLayer);
    setcolor(EGERGB(245, 245, 235), btnLayer);
    ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, btnLayer);

    // 更新并绘制 ripples
    for (auto& r : ripples) {
        r.update();
        r.draw(btnLayer);
    }
    ripples.erase(std::remove_if(ripples.begin(), ripples.end(),
        [](const Ripple& r) { return !r.alive(); }),
        ripples.end());

    // 按钮文字
    setbkmode(TRANSPARENT, btnLayer);
    settextcolor(BLACK, btnLayer);
    setfont(23, 0, "宋体", btnLayer);
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

bool InputBox::isInside(int x, int y) {
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

// Slider 类实现
Slider::Slider()
    : m_x(0), m_y(0), m_w(200), m_h(20), m_value(0.0), m_dragging(false), m_dragOffset(0),
      m_bgColor(EGERGB(200, 200, 200)), m_fgColor(EGERGB(100, 100, 255)) {}

void Slider::create(int x, int y, int w, int h) {
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    m_radius = h / 2;
}

void Slider::draw() {
    setfillcolor(m_bgColor);
    setlinecolor(m_bgColor);
    fillroundrect(m_x, m_y + m_h / 2 - 2, m_x + m_w, m_y + m_h / 2 + 2, 4, 4);

    int knobX = m_x + static_cast<int>(m_value * m_w);
    setfillcolor(m_fgColor);
    setlinecolor(BLACK);
    fillellipse(knobX, m_y + m_h / 2, m_radius, m_radius);
}

void Slider::handleEvent(const mouse_msg& msg) {
    if (msg.is_left() && msg.is_down()) {
        int knobX = m_x + static_cast<int>(m_value * m_w);
        int dy = msg.y - (m_y + m_h / 2);
        int dx = msg.x - knobX;
        if (dx * dx + dy * dy <= m_radius * m_radius) {
            m_dragging = true;
            m_dragOffset = msg.x - knobX; // 记录偏移
        }
    } else if (msg.is_move() && m_dragging) {
        int mx = clamp(msg.x - m_dragOffset, m_x, m_x + m_w);
        m_value = (mx - m_x) / static_cast<double>(m_w);
        if (m_onChange)
            m_onChange(m_value);
    } else if (msg.is_left() && msg.is_up()) {
        m_dragging = false;
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

void Slider::setOnChange(std::function<void(double)> callback) {
    m_onChange = callback;
}

// Text 类实现
void Text::setFont(const std::string& f) { font = f; }
void Text::setScale(float s) { scale = s; }
void Text::setContent(const std::string& str) { content = str; }
void Text::setWrapWidth(int w) { wrapWidth = w; }

void Text::draw(int x, int y) const {
    setfont(scale * 16, 0, font.c_str());
    settextcolor(BLACK);

    std::vector<std::string> chars = utf8_split(content);
    std::string line;
    int drawY = y;

    for (size_t i = 0; i < chars.size(); ) {
        line.clear();
        int w = 0;
        while (i < chars.size()) {
            std::string tmp = line + chars[i];
            int tmpWidth = textwidth(tmp.c_str());
            if (tmpWidth > wrapWidth && !line.empty()) break;
            line = tmp;
            ++i;
        }

        outtextxy(x, drawY, line.c_str());
        drawY += textheight(line.c_str());
    }
}

std::vector<std::string> Text::utf8_split(const std::string& str) {
    std::vector<std::string> result;
    for (size_t i = 0; i < str.size(); ) {
        unsigned char c = str[i];
        int len = 1;
        if ((c & 0x80) == 0x00) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;
        else len = 1;

        result.push_back(str.substr(i, len));
        i += len;
    }
    return result;
}
