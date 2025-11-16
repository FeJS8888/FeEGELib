# Knob 组件文档

## 概述

Knob（旋钮）是一个基于 Quasar QKnob 风格实现的圆形进度控件，通过圆形弧线显示当前值，支持鼠标拖拽交互来改变数值。

## 特性

- ✅ 圆形进度弧显示（280° 弧线，从 -140° 到 +140°）
- ✅ 支持最小值（min）、最大值（max）、步进（step）
- ✅ 支持偏移角度（offsetAngle），可旋转圆弧起始位置
- ✅ 支持内部范围限制（innerMin/innerMax）
- ✅ 支持禁用（disabled）和只读（readonly）模式
- ✅ 支持显示/隐藏当前值（showValue）
- ✅ 支持自定义字体大小（fontSize）
- ✅ 鼠标垂直拖拽改变数值
- ✅ 支持缩放（scale）
- ✅ 悬停效果（hover）
- ✅ 值改变回调（onChange）

## 基本使用

### 方式 1：使用 Builder 模式（推荐）

```cpp
#include "FeEGELib.h"
using namespace FeEGE;

// 创建一个基本的 Knob
Knob* knob = KnobBuilder()
    .setIdentifier(L"my_knob")      // 设置标识符
    .setCenter(400, 300)             // 设置中心位置
    .setRadius(80)                   // 设置半径
    .setRange(0, 100)                // 设置值范围
    .setStep(1)                      // 设置步进
    .setValue(50)                    // 设置初始值
    .setShowValue(true)              // 显示当前值
    .setOnChange([](double val) {    // 值改变回调
        printf("Value: %.0f\n", val);
    })
    .build();
```

### 方式 2：直接构造并配置

```cpp
// 创建 Knob 对象
Knob* knob = new Knob(400, 300, 80);

// 配置属性
knob->setRange(0, 100);
knob->setStep(1);
knob->setValue(50);
knob->setShowValue(true);
knob->setOnChange([](double val) {
    printf("Value: %.0f\n", val);
});

// 注册到全局控件集合（如果需要）
widgets.insert(knob);
```

## API 参考

### 构造函数

```cpp
Knob(int cx, int cy, double r);
```

- `cx`: 中心 x 坐标
- `cy`: 中心 y 坐标
- `r`: 半径

### 主要方法

#### setRange

```cpp
void setRange(double minVal, double maxVal);
```

设置值范围。

- `minVal`: 最小值
- `maxVal`: 最大值

**示例**：
```cpp
knob->setRange(0, 360);  // 角度选择器
knob->setRange(-20, 40); // 温度控制
```

#### setStep

```cpp
void setStep(double step);
```

设置步进值。值为 0 表示无步进（连续值）。

**示例**：
```cpp
knob->setStep(1);      // 整数步进
knob->setStep(0.1);    // 小数步进
knob->setStep(5);      // 5 的倍数
```

#### setValue

```cpp
void setValue(double val);
```

设置当前值。值会自动限制在 [min, max] 范围内，并应用步进。

#### getValue

```cpp
double getValue() const;
```

获取当前值。

#### setColor

```cpp
void setColor(color_t fg, color_t bg);
```

设置颜色。

- `fg`: 前景色（进度弧颜色）
- `bg`: 背景色（轨道颜色）

**示例**：
```cpp
knob->setColor(EGERGB(33, 150, 243), EGERGB(230, 230, 230));  // 蓝色主题
knob->setColor(EGERGB(255, 87, 34), EGERGB(255, 224, 178));   // 橙色主题
```

#### setOffsetAngle

```cpp
void setOffsetAngle(double angle);
```

设置偏移角度（度）。可以旋转整个圆弧的起始位置。

**示例**：
```cpp
knob->setOffsetAngle(0);    // 默认位置
knob->setOffsetAngle(90);   // 顺时针旋转 90°
knob->setOffsetAngle(-45);  // 逆时针旋转 45°
```

#### setInnerRange

```cpp
void setInnerRange(double innerMin, double innerMax);
```

设置内部范围限制。用于限制可视化显示的范围（预留功能）。

#### setShowValue

```cpp
void setShowValue(bool show);
```

设置是否在中心显示当前值。

#### setFontSize

```cpp
void setFontSize(int size);
```

设置显示值的字体大小。值为 0 时自动根据半径计算（radius * 0.35）。

#### setDisabled

```cpp
void setDisabled(bool disabled);
```

设置禁用状态。禁用时显示为灰色，且不响应交互。

#### setReadonly

```cpp
void setReadonly(bool readonly);
```

设置只读状态。只读时可以显示，但不能通过拖拽改变值。

#### setOnChange

```cpp
void setOnChange(std::function<void(double)> callback);
```

设置值改变时的回调函数。

**示例**：
```cpp
knob->setOnChange([](double val) {
    printf("New value: %.2f\n", val);
});
```

#### setScale

```cpp
void setScale(double s);
```

设置缩放比例。

#### setPosition

```cpp
void setPosition(int x, int y);
```

设置位置（中心坐标）。

### 继承自 Widget 的方法

```cpp
void draw(PIMAGE dst, int x, int y);  // 绘制到指定图像
void draw();                          // 绘制到默认图像
void handleEvent(const mouse_msg& msg); // 处理鼠标事件
```

## KnobBuilder API

### 所有配置方法

```cpp
KnobBuilder& setIdentifier(const std::wstring& identifier);
KnobBuilder& setCenter(int x, int y);
KnobBuilder& setRadius(double r);
KnobBuilder& setRange(double minVal, double maxVal);
KnobBuilder& setStep(double s);
KnobBuilder& setValue(double val);
KnobBuilder& setColor(color_t fg, color_t bg);
KnobBuilder& setOffsetAngle(double angle);
KnobBuilder& setInnerRange(double innerMin, double innerMax);
KnobBuilder& setShowValue(bool show);
KnobBuilder& setFontSize(int size);
KnobBuilder& setDisabled(bool disabled);
KnobBuilder& setReadonly(bool readonly);
KnobBuilder& setScale(double s);
KnobBuilder& setOnChange(std::function<void(double)> callback);
Knob* build();
```

## 使用示例

### 示例 1：音量控制

```cpp
Knob* volumeKnob = KnobBuilder()
    .setIdentifier(L"volume")
    .setCenter(400, 300)
    .setRadius(80)
    .setRange(0.0, 1.0)
    .setStep(0.05)
    .setValue(0.7)
    .setColor(EGERGB(76, 175, 80), EGERGB(200, 230, 201))
    .setShowValue(true)
    .setOnChange([](double val) {
        // 设置系统音量
        setVolume(val);
    })
    .build();
```

### 示例 2：角度选择器

```cpp
Knob* angleKnob = KnobBuilder()
    .setIdentifier(L"angle_selector")
    .setCenter(600, 300)
    .setRadius(100)
    .setRange(0, 360)
    .setStep(5)
    .setValue(0)
    .setColor(EGERGB(255, 87, 34), EGERGB(255, 224, 178))
    .setShowValue(true)
    .setFontSize(28)
    .build();
```

### 示例 3：温度控制

```cpp
Knob* tempKnob = KnobBuilder()
    .setIdentifier(L"temperature")
    .setCenter(300, 400)
    .setRadius(90)
    .setRange(-20, 40)
    .setStep(0.5)
    .setValue(22)
    .setColor(EGERGB(0, 188, 212), EGERGB(178, 235, 242))
    .setShowValue(true)
    .setOnChange([](double val) {
        printf("Temperature: %.1f°C\n", val);
    })
    .build();
```

### 示例 4：禁用和只读

```cpp
// 禁用状态（灰色，不可交互）
Knob* disabledKnob = KnobBuilder()
    .setCenter(200, 300)
    .setRadius(60)
    .setRange(0, 100)
    .setValue(50)
    .setDisabled(true)
    .build();

// 只读状态（正常显示，不可修改）
Knob* readonlyKnob = KnobBuilder()
    .setCenter(400, 300)
    .setRadius(60)
    .setRange(0, 100)
    .setValue(75)
    .setReadonly(true)
    .build();
```

### 示例 5：偏移角度

```cpp
// 旋转 90 度，使起点在顶部
Knob* rotatedKnob = KnobBuilder()
    .setCenter(500, 300)
    .setRadius(80)
    .setRange(-50, 50)
    .setValue(0)
    .setOffsetAngle(90)
    .setShowValue(true)
    .build();
```

## 交互说明

### 鼠标拖拽

- **按住左键拖拽**：在 Knob 内部按下鼠标左键，然后垂直拖动可以改变值
- **向上拖拽**：增加值
- **向下拖拽**：减少值
- **灵敏度**：移动 100 像素改变整个值范围

### 悬停效果

- 鼠标悬停在 Knob 上时，中心圆会显示轻微的高亮效果

## 边界情况处理

### min = max

当 min 和 max 相等时，Knob 会正确显示固定值，不响应拖拽。

```cpp
Knob* fixedKnob = KnobBuilder()
    .setRange(42, 42)
    .setValue(42)
    .build();
```

### 负值范围

支持负值范围：

```cpp
Knob* negativeKnob = KnobBuilder()
    .setRange(-100, 100)
    .setValue(0)
    .build();
```

### 非整数步进

支持小数步进：

```cpp
Knob* floatKnob = KnobBuilder()
    .setRange(0.0, 1.0)
    .setStep(0.01)
    .setValue(0.5)
    .build();
```

## 样式定制

### 颜色主题

```cpp
// 蓝色主题（默认）
knob->setColor(EGERGB(33, 150, 243), EGERGB(230, 230, 230));

// 绿色主题
knob->setColor(EGERGB(76, 175, 80), EGERGB(200, 230, 201));

// 橙色主题
knob->setColor(EGERGB(255, 87, 34), EGERGB(255, 224, 178));

// 紫色主题
knob->setColor(EGERGB(156, 39, 176), EGERGB(225, 190, 231));
```

### 大小调整

```cpp
// 小型 Knob
Knob* smallKnob = KnobBuilder()
    .setRadius(50)
    .build();

// 大型 Knob
Knob* largeKnob = KnobBuilder()
    .setRadius(120)
    .build();

// 使用缩放
knob->setScale(1.5);  // 放大 1.5 倍
```

## 与 Quasar QKnob 的对应关系

| Quasar QKnob 属性 | Knob 方法 | 说明 |
|------------------|-----------|------|
| `min` | `setRange(min, max)` | 最小值 |
| `max` | `setRange(min, max)` | 最大值 |
| `step` | `setStep(step)` | 步进值 |
| `v-model` / `value` | `setValue()` / `getValue()` | 当前值 |
| `color` | `setColor(fg, bg)` | 颜色（前景/背景） |
| `track-color` | `setColor(fg, bg)` | 轨道颜色 |
| `angle` / `offset-angle` | `setOffsetAngle(angle)` | 偏移角度 |
| `inner-min` | `setInnerRange(innerMin, innerMax)` | 内部最小值 |
| `inner-max` | `setInnerRange(innerMin, innerMax)` | 内部最大值 |
| `disable` | `setDisabled(true)` | 禁用状态 |
| `readonly` | `setReadonly(true)` | 只读状态 |
| `show-value` | `setShowValue(true)` | 显示值 |
| `font-size` | `setFontSize(size)` | 字体大小 |
| `@change` | `setOnChange(callback)` | 值改变事件 |

## 注意事项

1. **Windows 专用**：本组件基于 EGE 图形库，仅支持 Windows 平台
2. **事件循环**：需要在 `start()` 主循环中才能响应交互
3. **内存管理**：使用 Builder 创建的 Knob 会自动注册到全局控件集合，框架会管理其生命周期
4. **线程安全**：不支持多线程访问，所有操作应在主线程进行

## 完整示例程序

参见 `main.cpp` 文件，包含 10 个不同配置的 Knob 示例。

## 更新日志

### v2.12.0.0 (2025-11-15)
- ✨ 完全重新实现 Knob 组件
- ✨ 基于 Quasar QKnob 风格设计
- ✨ 新增 KnobBuilder 构建器
- ✨ 支持更多配置选项（偏移角度、内部范围、字体大小等）
- ✨ 改进拖拽交互体验
- ✨ 新增禁用和只读模式
- ✨ 新增悬停效果
- 📝 完善文档和示例代码
