# Widget 渲染性能分析与优化方案

## 1. 性能问题分析

### 1.1 图层重复创建和删除问题

#### 问题描述
在多个Widget类中，当缩放比例（scale）发生变化时，会删除并重新创建图层（PIMAGE），这是一个昂贵的操作。

#### 影响的类
- **Panel**: 在 `setScale()` 中删除并重新创建 `layer` 和 `maskLayer`
- **Button**: 在 `setScale()` 中删除并重新创建 `btnLayer`, `maskLayer`, `bgLayer`
- **InputBox**: 在 `setScale()` 中删除并重新创建 `btnLayer`, `maskLayer`, `bgLayer`
- **ProgressBar**: 在 `setScale()` 中删除并重新创建 `barLayer`

#### 代码示例
```cpp
// Panel::setScale() - 每次缩放都重建图层
void Panel::setScale(double s){
    // ...
    if(maskLayer) delimage(maskLayer);
    maskLayer = newimage(width,height);
    if(layer) delimage(layer);
    layer = newimage(width,height);
    // ...
}
```

#### 性能影响
- **高频率操作**: 用户拖动Slider改变缩放时，每帧都可能触发图层重建
- **内存分配开销**: `newimage()` 和 `delimage()` 涉及内存分配/释放
- **初始化开销**: 每次创建后需要重新设置抗锯齿、背景色等属性

### 1.2 不必要的重绘

#### 问题描述
许多控件没有正确使用脏标记（dirty flag）来避免不必要的重绘。

#### 具体表现

**Panel类**:
```cpp
void Panel::draw(PIMAGE dst, double x, double y) {
    // 总是清空并重绘（注释说明子控件可能有动态内容）
    setbkcolor_f(EGEARGB(0, 0, 0, 0), layer);
    cleardevice(layer);
    // ...
    for (size_t i = 0; i < children.size(); ++i) {
        children[i]->draw(layer, childX, childY);  // 每帧都绘制所有子控件
    }
}
```

**问题点**:
- Panel每次都清空并重绘，即使内容没有变化
- 即使子控件没有变化，也会每帧绘制
- 子控件的needRedraw标记没有被Panel检查

**Button类**:
```cpp
void Button::draw(PIMAGE dst,double x,double y){
    if(!ripples.size() && !needRedraw){
        putimage_withalpha(dst,bgLayer,left,top);
        return;  // 优化：有缓存机制
    }
    // ...
}
```

**优点**: Button已经实现了基本的缓存机制
**不足**: 只有在没有ripples且不需要重绘时才使用缓存

### 1.3 文本测量性能问题

#### 问题描述
频繁调用 `measuretext()` 进行文本宽度测量，尤其是在每帧绘制时。

#### 影响的类

**InputBox类**:
```cpp
void InputBox::draw(PIMAGE dst, double x, double y) {
    // 每次都进行多次文本测量
    measuretext(displayContent.substr(0, cursor_pos).c_str(), &cursor_pos_width, &tmp, btnLayer);
    measuretext(cursor_before_cursor.c_str(), &cursor_with_ime_width, &tmp, btnLayer);
    measuretext(cursor_before_text.c_str(), &cursor_with_full_ime_width, &tmp, btnLayer);
    measuretext(displayContent.c_str(), &full_text_width, &tmp, btnLayer);
}
```

**已有的优化**:
InputBox已经实现了缓存机制：
```cpp
if (lastMeasuredContent != displayContent || lastCursorPos != cursor_pos || scaleChanged || PanelScaleChanged) {
    // 只有内容改变时才重新测量
    // 缓存测量结果
}
```

**Text类**:
```cpp
void Text::draw(PIMAGE dst, double x, double y) {
    for (size_t i = 0; i < lines.size(); ++i) {
        // 每次都重新测量
        measuretext(lines[i].c_str(), &w, &h, dst);
    }
}
```

**改进**: Text类已经有缓存机制：
```cpp
// 优化：使用缓存的行宽度，避免重复测量
float lineW = (i < cachedLineWidths.size()) ? cachedLineWidths[i] : 0;
```

### 1.4 波纹动画开销

#### 问题描述
Button和InputBox的波纹（Ripple）效果每帧都需要更新和绘制。

#### 实现分析
```cpp
// Button::draw()
for (auto& r : ripples) {
    r.update();  // 每个波纹每帧更新
    r.draw(btnLayer);  // 每个波纹每帧绘制
}
std::erase_if(ripples, [](const Ripple& r) { return !r.alive(); });
```

**波纹绘制**:
```cpp
void Ripple::draw(PIMAGE dst) const {
    double progress = (double)age / life;
    double r = maxRadius * progress;
    int alpha = static_cast<int>(120 * std::cos(progress * PI / 2));
    setfillcolor(EGEARGB(alpha, 30, 30, 30), dst);
    ege_fillellipse(x - r, y - r, r * 2, r * 2, dst);  // 每帧绘制椭圆
}
```

#### 性能影响
- 每个活跃的波纹每帧都需要绘制一个半透明椭圆
- 多次点击会产生多个波纹，叠加效果影响性能
- 即使波纹已经完成动画，仍需要检查和清理

### 1.5 遮罩操作开销

#### 问题描述
多个控件使用 `putimage_alphafilter()` 应用遮罩，这是一个像素级的操作，性能开销较大。

#### 影响的类
```cpp
// Panel::draw()
putimage_alphafilter(dst, layer, left, top, maskLayer, 0, 0, -1, -1);

// Button::draw()
putimage_alphafilter(bgLayer, btnLayer, 0, 0, maskLayer, 0, 0, -1, -1);

// InputBox::draw()
putimage_alphafilter(bgLayer, btnLayer, 0, 0, maskLayer, 0, 0, -1, -1);
```

#### 性能特点
- `putimage_alphafilter` 需要遍历每个像素
- 对于大尺寸控件，像素数量大，开销显著
- 每帧都执行此操作，即使内容没有变化

### 1.6 字体设置重复

#### 问题描述
每次绘制时都设置字体，即使字体没有改变。

#### 示例
```cpp
// Button::draw()
ege_setfont(23 * scale, L"宋体", btnLayer);  // 每次绘制都设置

// Text::draw()
ege_setfont(fontSize * scale, fontName.c_str(), dst);  // 每次都设置
```

#### 性能影响
- `ege_setfont()` 可能涉及字体加载和资源查找
- 在高频率绘制时累积开销明显

### 1.7 Dropdown动画性能

#### 问题描述
Dropdown的渐变动画每帧都需要更新透明度并重绘Panel。

```cpp
void Dropdown::draw(PIMAGE dst, double x, double y) {
    if (fadingIn) {
        fadeAlpha += 0.09;
        // ...
    }
    // 每次都调用setAlpha
    int actualAlpha = static_cast<int>(fadeAlpha * 255);
    if (lastAppliedAlpha != actualAlpha) {
        dropdownPanel->setAlpha(actualAlpha);
    }
    dropdownPanel->draw(dst, ...);  // 每帧绘制Panel
}
```

#### 优化点
- 已经实现了 `lastAppliedAlpha` 缓存，避免重复设置
- 但仍需要每帧检查和绘制

### 1.8 Slider动画缓动

#### 问题描述
Slider的缩放和进度缓动每帧都更新，即使变化很小。

```cpp
void Slider::draw(PIMAGE dst, double x, double y) {
    // 每帧更新缩放比例
    if (m_pressed) {
        m_scale += (0.8f - m_scale) * 0.2f;
    } else {
        m_scale += (1.0f - m_scale) * 0.2f;
    }
    
    // 每帧更新进度
    m_progress += (m_finalprogress - m_progress) * 0.15;
    
    // 每帧绘制
    ege_fillroundrect(...);
    ege_fillellipse(...);
}
```

#### 性能特点
- 缓动计算本身开销不大
- 但导致每帧都需要重绘，即使视觉变化很小
- 可以设置一个阈值，当变化小于阈值时停止更新

---

## 2. 优化方案

### 2.1 图层池化和复用

#### 方案描述
不要在每次缩放时删除并重新创建图层，而是复用现有图层或使用图层池。

#### 实施细节

**方案A：尺寸检查和条件重建**
```cpp
void Panel::setScale(double s) {
    if(sgn(s - scale) == 0) return;
    
    double newWidth = origin_width * s;
    double newHeight = origin_height * s;
    
    // 仅在尺寸改变时重建图层
    if (width != newWidth || height != newHeight) {
        width = newWidth;
        height = newHeight;
        radius = origin_radius * s;
        
        // 重建图层（尺寸改变）
        if(maskLayer) delimage(maskLayer);
        maskLayer = newimage(width, height);
        if(layer) delimage(layer);
        layer = newimage(width, height);
        
        // 初始化
        ege_enable_aa(true, layer);
        ege_enable_aa(true, maskLayer);
        setbkcolor_f(EGEARGB(255, 0, 0, 0), maskLayer);
        cleardevice(maskLayer);
        setfillcolor(EGEARGB(255, 255, 255, 255), maskLayer);
        ege_fillroundrect(0, 0, width - 0.5, height - 0.5, radius, radius, radius, radius, maskLayer);
    } else {
        // 仅更新缩放相关属性
        radius = origin_radius * s;
    }
    
    scale = s;
    scaleChanged = true;
    
    // 更新子控件
    for (size_t i = 0; i < children.size(); ++i) {
        children[i]->setScale(s);
        children[i]->setPosition(cx + childOffsets[i].x * scale, cy + childOffsets[i].y * scale);
    }
}
```

**方案B：图层池（更激进）**
```cpp
class ImagePool {
    struct PooledImage {
        PIMAGE image;
        int width, height;
        bool in_use;
    };
    
    std::vector<PooledImage> pool;
    
public:
    PIMAGE acquire(int width, int height) {
        // 查找相同尺寸的空闲图层
        for (auto& pi : pool) {
            if (!pi.in_use && pi.width == width && pi.height == height) {
                pi.in_use = true;
                return pi.image;
            }
        }
        
        // 没有合适的，创建新的
        PIMAGE img = newimage(width, height);
        pool.push_back({img, width, height, true});
        return img;
    }
    
    void release(PIMAGE img) {
        for (auto& pi : pool) {
            if (pi.image == img) {
                pi.in_use = false;
                return;
            }
        }
    }
};
```

#### 优先级
**高** - 这是最明显的性能问题之一

#### 预期收益
- 减少内存分配/释放开销：50-80%
- 减少初始化开销：30-50%
- 特别是在动态缩放场景下（如拖动Slider）

---

### 2.2 改进脏标记系统

#### 方案描述
实现更完善的脏标记系统，避免不必要的重绘。

#### 实施细节

**Panel改进**:
```cpp
class Panel : public Widget {
private:
    bool needRedraw = true;
    bool childrenNeedRedraw = true;
    
    // 检查子控件是否需要重绘
    bool checkChildrenNeedRedraw() {
        for (auto* child : children) {
            if (auto* btn = dynamic_cast<Button*>(child)) {
                if (btn->needsRedraw()) return true;
            }
            // 其他控件类型的检查...
        }
        return false;
    }
    
public:
    void draw(PIMAGE dst, double x, double y) override {
        if (layout) layout->apply(*this);
        
        double left = x - width / 2;
        double top = y - height / 2;
        
        // 检查是否需要重绘
        bool shouldRedraw = needRedraw || scaleChanged || checkChildrenNeedRedraw();
        
        if (!shouldRedraw) {
            // 使用缓存
            putimage_alphafilter(dst, layer, left, top, maskLayer, 0, 0, -1, -1);
            return;
        }
        
        // 执行重绘
        setbkcolor_f(EGEARGB(0, 0, 0, 0), layer);
        cleardevice(layer);
        setfillcolor(bgColor, layer);
        ege_fillroundrect(0, 0, width, height, radius, radius, radius, radius, layer);
        
        // 绘制子控件
        if(scaleChanged) PanelScaleChanged = true;
        for (size_t i = 0; i < children.size(); ++i) {
            double childX = width / 2 + childOffsets[i].x * scale;
            double childY = height / 2 + childOffsets[i].y * scale;
            absolutPosDeltaX = left;
            absolutPosDeltaY = top;
            children[i]->setPosition(cx + childOffsets[i].x * scale, cy + childOffsets[i].y * scale);
            children[i]->draw(layer, childX, childY);
            absolutPosDeltaX = 0;
            absolutPosDeltaY = 0;
        }
        PanelScaleChanged = false;
        scaleChanged = false;
        needRedraw = false;
        
        putimage_alphafilter(dst, layer, left, top, maskLayer, 0, 0, -1, -1);
    }
};
```

**Widget基类添加接口**:
```cpp
class Widget {
public:
    virtual bool needsRedraw() const { return true; }  // 默认总是需要重绘
    virtual void markClean() {}  // 标记为已绘制
};

class Button : public Widget {
public:
    bool needsRedraw() const override {
        return needRedraw || !ripples.empty();
    }
    
    void markClean() override {
        needRedraw = false;
    }
};
```

#### 优先级
**高** - Panel是容器控件，优化它能带来连锁反应

#### 预期收益
- 减少重绘次数：60-90%（取决于场景）
- CPU使用率降低：40-70%
- 特别是在静态内容场景下效果显著

---

### 2.3 优化文本测量

#### 方案描述
扩展现有的缓存机制，减少 `measuretext()` 调用。

#### 实施细节

**Text类改进**:
```cpp
class Text : public Widget {
private:
    double lastFontScale = -1;
    std::vector<float> cachedLineWidths;
    bool layoutDirty = true;
    
    void updateLayout() {
        if (!layoutDirty) return;  // 已经是最新的
        
        lines.clear();
        cachedLineWidths.clear();
        // ... 原有的布局计算 ...
        
        layoutDirty = false;
    }
    
public:
    void setContent(const std::wstring& text) {
        if (contentW == text) return;
        contentW = text;
        layoutDirty = true;  // 标记需要重新布局
    }
    
    void setScale(double s) {
        if (fabs(scale - s) < 1e-6) return;
        scale = s;
        layoutDirty = true;
    }
    
    void draw(PIMAGE dst, double x, double y) override {
        updateLayout();  // 仅在需要时更新
        
        // 使用缓存的宽度
        for (size_t i = 0; i < lines.size(); ++i) {
            float lineW = cachedLineWidths[i];
            // ... 绘制逻辑 ...
        }
    }
};
```

**Button类改进**:
```cpp
class Button : public Widget {
private:
    float cachedTextWidth = -1;
    float cachedTextHeight = -1;
    std::wstring lastMeasuredContent;
    double lastMeasuredScale = -1;
    
public:
    void draw(PIMAGE dst, double x, double y) override {
        // ...
        
        // 仅在内容或缩放改变时测量
        if (content != lastMeasuredContent || scale != lastMeasuredScale) {
            ege_setfont(23 * scale, L"宋体", btnLayer);
            measuretext(content.c_str(), &cachedTextWidth, &cachedTextHeight, btnLayer);
            lastMeasuredContent = content;
            lastMeasuredScale = scale;
        }
        
        // 使用缓存值
        ege_outtextxy(width / 2 - cachedTextWidth / 2, 
                     height / 2 - cachedTextHeight / 2, 
                     content.c_str(), btnLayer);
    }
};
```

#### 优先级
**中** - 已经有基础的缓存机制，这是进一步优化

#### 预期收益
- 减少文本测量次数：70-90%
- CPU使用率降低：10-20%（取决于文本控件数量）

---

### 2.4 波纹动画优化

#### 方案描述
优化波纹动画的渲染和更新逻辑。

#### 实施细节

**方案A：限制波纹数量**
```cpp
class Button : public Widget {
private:
    static constexpr int MAX_RIPPLES = 3;  // 最多3个同时存在的波纹
    
public:
    bool handleEvent(const mouse_msg& msg) override {
        if (msg.is_left() && msg.is_down() && inside) {
            // 限制波纹数量
            if (ripples.size() >= MAX_RIPPLES) {
                ripples.erase(ripples.begin());  // 移除最旧的
            }
            
            int localX = msg.x - left;
            int localY = msg.y - top;
            m_counter++;
            ripples.emplace_back(localX, localY, 
                                4.00f / 3.00f * std::sqrt(height * height + width * width), 
                                70, this, m_counter);
            needRedraw = true;
            // ...
        }
        // ...
    }
};
```

**方案B：预渲染波纹**
```cpp
class RippleCache {
    std::vector<PIMAGE> frames;  // 预渲染的波纹帧
    int maxRadius;
    int life;
    
public:
    RippleCache(int maxRadius, int life) : maxRadius(maxRadius), life(life) {
        // 预渲染所有帧
        for (int age = 0; age <= life; ++age) {
            PIMAGE frame = newimage(maxRadius * 2, maxRadius * 2);
            setbkcolor_f(EGEARGB(0, 0, 0, 0), frame);
            cleardevice(frame);
            
            double progress = (double)age / life;
            double r = maxRadius * progress;
            int alpha = static_cast<int>(120 * std::cos(progress * PI / 2));
            
            setfillcolor(EGEARGB(alpha, 30, 30, 30), frame);
            ege_fillellipse(maxRadius - r, maxRadius - r, r * 2, r * 2, frame);
            
            frames.push_back(frame);
        }
    }
    
    PIMAGE getFrame(int age) {
        if (age < 0 || age >= frames.size()) return nullptr;
        return frames[age];
    }
};

// 在Ripple::draw中使用预渲染帧
void Ripple::draw(PIMAGE dst) const {
    static RippleCache cache(maxRadius, life);  // 静态缓存
    PIMAGE frame = cache.getFrame(age);
    if (frame) {
        putimage_withalpha(dst, frame, x - maxRadius, y - maxRadius);
    }
}
```

**方案C：使用Shader（如果支持）**
如果EGE库支持shader，可以用GPU加速波纹效果。

#### 优先级
**中低** - 波纹是视觉效果，优化影响有限

#### 预期收益
- 减少绘制次数：30-50%（限制数量）
- CPU使用率降低：5-15%
- 方案B可以显著降低CPU使用，但增加内存使用

---

### 2.5 遮罩操作优化

#### 方案描述
减少遮罩操作的频率，或使用更高效的方式。

#### 实施细节

**方案A：缓存遮罩后的结果**
```cpp
class Button : public Widget {
private:
    PIMAGE cachedFinalImage = nullptr;  // 缓存应用遮罩后的最终图像
    bool finalImageDirty = true;
    
public:
    void draw(PIMAGE dst, double x, double y) override {
        double left = x - width / 2;
        double top = y - height / 2;
        
        // 如果有缓存且不需要重绘，直接使用
        if (!finalImageDirty && !ripples.size() && !needRedraw) {
            putimage_withalpha(dst, cachedFinalImage, left, top);
            return;
        }
        
        // 重绘到btnLayer
        setbkcolor_f(EGEARGB(0, 0, 0, 0), btnLayer);
        cleardevice(btnLayer);
        // ... 绘制逻辑 ...
        
        // 应用遮罩到bgLayer
        setbkcolor_f(EGEARGB(0, 0, 0, 0), bgLayer);
        cleardevice(bgLayer);
        putimage_alphafilter(bgLayer, btnLayer, 0, 0, maskLayer, 0, 0, -1, -1);
        
        // 缓存最终结果
        if (!cachedFinalImage) {
            cachedFinalImage = newimage(width, height);
        }
        getimage(cachedFinalImage, bgLayer, 0, 0, width, height);
        finalImageDirty = false;
        
        putimage_withalpha(dst, bgLayer, left, top);
        needRedraw = false;
    }
    
    void setScale(double s) override {
        // ...
        finalImageDirty = true;  // 标记缓存失效
    }
};
```

**方案B：仅在必要时应用遮罩**
```cpp
// 对于简单的矩形控件，考虑不使用遮罩
// 或者在创建时一次性应用遮罩到背景图层
```

#### 优先级
**中** - 遮罩操作确实昂贵，但已经有基础优化

#### 预期收益
- CPU使用率降低：15-30%
- 特别是对大尺寸控件效果明显

---

### 2.6 字体设置优化

#### 方案描述
缓存当前字体设置，避免重复设置。

#### 实施细节

**全局字体管理器**:
```cpp
class FontManager {
private:
    struct FontState {
        int size;
        std::wstring name;
        PIMAGE target;
    };
    
    std::unordered_map<PIMAGE, FontState> currentFonts;
    
public:
    void setFont(int size, const std::wstring& name, PIMAGE img) {
        auto it = currentFonts.find(img);
        if (it != currentFonts.end()) {
            if (it->second.size == size && it->second.name == name) {
                return;  // 已经是当前字体，无需设置
            }
        }
        
        // 设置字体
        ege_setfont(size, name.c_str(), img);
        currentFonts[img] = {size, name, img};
    }
    
    void invalidate(PIMAGE img) {
        currentFonts.erase(img);
    }
};

static FontManager g_fontManager;
```

**Widget中使用**:
```cpp
void Button::draw(PIMAGE dst, double x, double y) override {
    // ...
    g_fontManager.setFont(23 * scale, L"宋体", btnLayer);
    // ...
}
```

#### 优先级
**低** - 字体设置开销相对较小

#### 预期收益
- CPU使用率降低：2-5%
- 实现简单，风险低

---

### 2.7 动画阈值优化

#### 方案描述
为缓动动画设置停止阈值，避免微小变化导致持续重绘。

#### 实施细节

```cpp
class Slider : public Widget {
private:
    static constexpr double ANIMATION_THRESHOLD = 0.001;  // 阈值
    bool isAnimating = false;
    
public:
    void draw(PIMAGE dst, double x, double y) override {
        // 更新缩放比例
        double targetScale = m_pressed ? 0.8 : 1.0;
        double scaleDiff = targetScale - m_scale;
        
        if (fabs(scaleDiff) > ANIMATION_THRESHOLD) {
            m_scale += scaleDiff * 0.2;
            isAnimating = true;
        } else {
            m_scale = targetScale;
            isAnimating = false;
        }
        
        // 更新进度
        double progressDiff = m_finalprogress - m_progress;
        if (fabs(progressDiff) > ANIMATION_THRESHOLD) {
            m_progress += progressDiff * 0.15;
            isAnimating = true;
        } else {
            m_progress = m_finalprogress;
            // isAnimating 可能已经被缩放动画设置
        }
        
        // 仅在动画进行中时绘制（如果有缓存机制）
        if (!isAnimating && cachedImage != nullptr) {
            putimage_withalpha(dst, cachedImage, left, top);
            return;
        }
        
        // 执行绘制
        // ...
    }
};
```

**应用到其他控件**:
- Knob的displayValue缓动
- Dropdown的fadeAlpha动画
- Toggle的knobOffset动画

#### 优先级
**中** - 简单有效，适合有动画的控件

#### 预期收益
- 减少不必要的绘制：30-50%
- CPU使用率降低：10-20%（动画场景）

---

### 2.8 批量更新优化

#### 方案描述
添加批量更新模式，延迟重绘到批量操作完成。

#### 实施细节

```cpp
class Widget {
protected:
    static int batchUpdateDepth;
    static std::unordered_set<Widget*> dirtyWidgets;
    
public:
    static void beginBatchUpdate() {
        batchUpdateDepth++;
    }
    
    static void endBatchUpdate() {
        if (--batchUpdateDepth == 0) {
            // 批量更新结束，触发所有脏控件的重绘
            for (auto* widget : dirtyWidgets) {
                widget->forceRedraw();
            }
            dirtyWidgets.clear();
        }
    }
    
    void markDirty() {
        if (batchUpdateDepth > 0) {
            dirtyWidgets.insert(this);
        } else {
            forceRedraw();
        }
    }
    
    virtual void forceRedraw() {
        // 子类实现
    }
};

// 使用示例
void updateMultipleWidgets() {
    Widget::beginBatchUpdate();
    
    button1->setContent(L"新文本");
    button2->setScale(1.5);
    slider->setProgress(0.8);
    
    Widget::endBatchUpdate();  // 此时才触发重绘
}
```

#### 优先级
**中低** - 需要用户主动使用，适合特定场景

#### 预期收益
- 减少重绘次数：取决于批量操作规模
- 在大量控件同时更新时效果显著

---

## 3. 实施优先级建议

### 3.1 高优先级（立即实施）

1. **图层池化和复用** (2.1)
   - 影响范围：Panel, Button, InputBox, ProgressBar
   - 实施难度：中
   - 预期收益：高

2. **改进脏标记系统** (2.2)
   - 影响范围：主要是Panel和Button
   - 实施难度：中
   - 预期收益：高

### 3.2 中优先级（短期内实施）

3. **优化文本测量** (2.3)
   - 影响范围：Text, Button
   - 实施难度：低
   - 预期收益：中

4. **遮罩操作优化** (2.5)
   - 影响范围：所有使用遮罩的控件
   - 实施难度：中
   - 预期收益：中

5. **动画阈值优化** (2.7)
   - 影响范围：Slider, Knob, Toggle, Dropdown
   - 实施难度：低
   - 预期收益：中

### 3.3 低优先级（可选/后续优化）

6. **波纹动画优化** (2.4)
   - 影响范围：Button, InputBox
   - 实施难度：中-高（取决于方案）
   - 预期收益：中低

7. **字体设置优化** (2.6)
   - 影响范围：所有绘制文本的控件
   - 实施难度：低
   - 预期收益：低

8. **批量更新优化** (2.8)
   - 影响范围：需要用户代码配合
   - 实施难度：中
   - 预期收益：场景相关

---

## 4. 测试建议

### 4.1 性能测试场景

1. **静态内容场景**
   - 多个Panel嵌套，包含多个Button和Text
   - 测量FPS和CPU使用率
   - 评估脏标记系统的效果

2. **动态缩放场景**
   - 使用Slider动态改变Panel缩放
   - 测量图层重建的开销
   - 评估图层池化的效果

3. **波纹密集场景**
   - 快速点击多个Button
   - 测量波纹动画的CPU使用
   - 评估波纹优化的效果

4. **大量文本场景**
   - 创建多个Text控件
   - 测量文本测量的开销
   - 评估文本测量缓存的效果

### 4.2 性能指标

- **FPS (帧率)**：目标60 FPS
- **CPU使用率**：目标 < 20%（空闲时）
- **内存使用**：监控内存增长
- **绘制次数**：使用计数器统计实际绘制次数

### 4.3 基准测试工具

```cpp
class PerformanceMonitor {
private:
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    std::deque<double> frameTimes;
    int drawCount = 0;
    
public:
    void beginFrame() {
        lastFrameTime = std::chrono::high_resolution_clock::now();
    }
    
    void endFrame() {
        auto now = std::chrono::high_resolution_clock::now();
        double frameTime = std::chrono::duration<double, std::milli>(now - lastFrameTime).count();
        frameTimes.push_back(frameTime);
        if (frameTimes.size() > 60) {
            frameTimes.pop_front();
        }
    }
    
    double getAverageFPS() {
        double avg = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0) / frameTimes.size();
        return 1000.0 / avg;
    }
    
    void recordDraw() {
        drawCount++;
    }
    
    int getDrawCount() {
        int count = drawCount;
        drawCount = 0;
        return count;
    }
};
```

---

## 5. 风险评估

### 5.1 图层池化 (2.1)

**风险**：
- 内存泄漏风险（如果池管理不当）
- 可能的图层状态污染

**缓解措施**：
- 仔细的生命周期管理
- 清理和重置机制
- 充分的测试

### 5.2 脏标记系统 (2.2)

**风险**：
- 逻辑复杂，容易出现状态不一致
- 可能遗漏某些需要重绘的情况

**缓解措施**：
- 保守的默认值（默认总是重绘）
- 逐步迁移，先实施简单控件
- 完善的测试用例

### 5.3 缓存机制 (各种)

**风险**：
- 缓存失效逻辑错误导致显示错误
- 增加内存使用

**缓解措施**：
- 明确的缓存失效规则
- 内存使用监控
- 可配置的缓存策略

---

## 6. 后续工作

### 6.1 性能分析工具

开发专门的性能分析工具：
- 可视化的绘制次数统计
- 热点函数的性能分析
- 内存使用追踪

### 6.2 文档更新

- 更新性能优化指南
- 添加最佳实践文档
- 提供性能测试示例

### 6.3 持续优化

- 定期进行性能回归测试
- 监控新功能的性能影响
- 根据实际使用反馈调整优化策略

---

## 7. 总结

### 7.1 主要问题

1. **图层频繁重建**：最严重的性能问题
2. **过度重绘**：缺乏有效的脏标记系统
3. **重复计算**：文本测量、字体设置等

### 7.2 优化路线

**短期（1-2周）**：
- 实施图层池化
- 改进Panel的脏标记系统

**中期（1个月）**：
- 扩展脏标记到其他控件
- 优化文本测量和遮罩操作
- 添加动画阈值

**长期（持续）**：
- 完善性能监控工具
- 根据实际使用情况持续优化
- 考虑更激进的优化（如GPU加速）

### 7.3 预期总体收益

实施所有高优先级和中优先级优化后：
- **FPS提升**：50-100%（取决于场景）
- **CPU使用率降低**：40-60%
- **内存使用增加**：< 10%（由于缓存）
- **响应性提升**：显著（尤其是动态缩放场景）

---

**文档版本**: 1.0  
**创建日期**: 2025-01-XX  
**最后更新**: 2025-01-XX  
**作者**: GitHub Copilot Analysis  
**状态**: 待实施
