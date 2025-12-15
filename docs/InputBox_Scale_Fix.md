# InputBox 缩放修复说明

## 问题描述

当 Panel 设置了缩放比例（通过 `setScale()` 方法）并包含 InputBox 子控件时，InputBox 内的文本位置不正确。文本会显得过于靠近左边缘，光标位置和点击位置也会出现偏差。

## 问题原因

InputBox 在渲染时使用了多个未缩放的常量：

1. **文本 padding**：14 像素的左右内边距
2. **光标尺寸**：宽度 2 像素，垂直偏移 3.5 和 7 像素
3. **IME 下划线偏移**：2 像素

当 InputBox 被缩放时：
- `width` 和 `height` 会乘以 `scale`
- 字体大小会乘以 `scale`（`setfont(23 * scale, ...)`）
- `btnLayer` 图层的尺寸也是缩放后的

但是这些常量没有被缩放，导致：
- 文本相对位置不正确
- 光标视觉效果不协调
- 点击位置计算偏差
- IME 输入提示位置不准确

## 解决方案

将所有影响视觉定位和尺寸的常量都乘以 `scale` 因子：

### 1. 文本 padding

```cpp
// 在三个位置修改
const float padding = 14 * scale;  // 之前是 14
```

修改位置：
- `InputBox::draw()`：用于文本渲染位置
- `InputBox::handleEvent()`：用于点击位置计算
- `InputBox::adjustScrollForCursor()`：用于滚动计算

### 2. 光标尺寸

```cpp
ege_fillrect(cursor_draw_x, 
             height / 2 - textRealHeight / 2 - 3.5 * scale,  // 之前是 -3.5
             2 * scale,                                       // 之前是 2
             textRealHeight + 7 * scale,                      // 之前是 +7
             btnLayer);
```

### 3. IME 相关偏移

```cpp
// IME 下划线位置
ege_line(ime_start_x, 
         height / 2 + textRealHeight / 2 + 2 * scale,  // 之前是 +2
         ime_end_x, 
         height / 2 + textRealHeight / 2 + 2 * scale,  // 之前是 +2
         btnLayer);

// IME 输入法位置
InputPositionY = top + height / 2 + textRealHeight / 2 + 2 * scale + absolutPosDeltaY;  // 之前是 +2
```

## 数学验证

### 场景 1：未缩放 (scale = 1.0)

- InputBox 宽度：160px（原始）
- padding：14 * 1.0 = 14px
- 文本位置：padding - scroll_offset = 14 - 0 = 14px
- 相对位置：14 / 160 = 8.75%（距离左边缘）

### 场景 2：缩放 2.0 倍

- InputBox 宽度：320px（160 * 2.0）
- padding：14 * 2.0 = 28px
- 文本位置：28 - 0 = 28px
- 相对位置：28 / 320 = 8.75%（与场景1相同）✓

### 场景 3：缩放 1.5 倍

- InputBox 宽度：240px（160 * 1.5）
- padding：14 * 1.5 = 21px
- 文本位置：21 - 0 = 21px
- 相对位置：21 / 240 = 8.75%（与场景1相同）✓

## 向后兼容性

此修复完全向后兼容：
- 当 `scale = 1.0` 时，乘以 scale 不改变结果
- 现有未使用缩放的代码不受影响
- 只修复了之前缩放时的错误行为

## 测试

参见 `test/test_inputbox_scale.cpp`，该测试创建了三个 Panel：
1. 未缩放 (scale=1.0)
2. 缩放 1.5 倍 (scale=1.5)
3. 缩放 2.0 倍 (scale=2.0)

每个 Panel 包含一个 InputBox，验证文本位置在不同缩放比例下的正确性。

## 相关文件

- `src/Widget.cpp`：InputBox::draw()、handleEvent() 和 adjustScrollForCursor() 实现
- `test/test_inputbox_scale.cpp`：测试用例
- `docs/Layout_Scale_Fix.md`：相关的 Layout 缩放修复文档

## 注意事项

⚠️ **重要**：InputBox 中所有影响视觉定位和尺寸的常量都必须乘以 `scale` 因子。这包括：
- padding（内边距）
- 光标相关的尺寸
- IME 相关的偏移
- 任何其他固定像素值

未来添加新的渲染或交互功能时，必须遵守这一原则。

## 设计原则

FeEGELib 的缩放系统基于以下原则：
1. **Widget 尺寸已缩放**：`getWidth()` 和 `getHeight()` 返回缩放后的尺寸
2. **图层尺寸已缩放**：渲染图层（如 `btnLayer`）使用缩放后的尺寸
3. **字体已缩放**：字体大小乘以 `scale`
4. **所有常量必须缩放**：任何像素值常量都需要乘以 `scale`

这确保了控件在任何缩放比例下都能保持一致的视觉效果和交互行为。
