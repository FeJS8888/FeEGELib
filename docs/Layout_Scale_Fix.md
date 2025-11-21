# Layout 系统缩放修复说明

## 问题描述

当 Panel 设置了缩放比例（通过 `setScale()` 方法）后，FlexLayout 布局系统无法正确计算子控件的位置，导致布局异常。

## 问题原因

FeEGELib 的布局系统使用了两套坐标系统：

1. **缩放坐标系**：Widget 的 `getWidth()` 和 `getHeight()` 返回的是缩放后的尺寸
2. **未缩放坐标系**：`childOffsets` 存储的是未缩放的偏移量

在 `Panel::draw()` 中，子控件的位置计算如下：
```cpp
int childX = width / 2 + childOffsets[i].x * scale;
```

这意味着 `childOffsets` 存储的是**未缩放**的偏移量，在渲染时才乘以 `scale`。

但是，原来的 `FlexLayout::apply()` 直接使用 `child->getWidth()` 计算布局，这个值已经是缩放后的，导致计算结果错误。

## 解决方案

修改 `FlexLayout::apply()` 方法，将所有计算转换到未缩放坐标系：

```cpp
double panelScale = parent.getScale();

// 子控件尺寸需要除以 scale 转换到未缩放坐标系
double childWidth = child->getWidth() / panelScale;

// Panel 尺寸也需要除以 scale 转换到未缩放坐标系
cursor = -panelWidth / 2 / panelScale + padding_;
```

### 关键修改点

1. 获取 Panel 的缩放比例：`double panelScale = parent.getScale();`
2. 计算子控件尺寸时除以 scale：`child->getWidth() / panelScale`
3. 计算面板边界时除以 scale：`panelWidth / 2 / panelScale`

## 数学验证

### 场景 1：未缩放 (scale = 1.0)

- Panel 宽度：100px（原始）
- 子控件宽度：50px（原始）
- FlexLayout 计算：
  - `childWidth = 50 / 1.0 = 50`
  - `offset = cursor + 50/2 = cursor + 25`（未缩放坐标）
- Panel::draw 渲染：
  - `childX = 100/2 + (cursor + 25) * 1.0 = 50 + cursor + 25` ✓

### 场景 2：缩放 (scale = 2.0)

- Panel 宽度：200px（100 * 2.0）
- 子控件宽度：100px（50 * 2.0，因为子控件也被缩放）
- FlexLayout 计算：
  - `childWidth = 100 / 2.0 = 50`（转换回未缩放）
  - `offset = cursor + 50/2 = cursor + 25`（未缩放坐标，与场景1相同）
- Panel::draw 渲染：
  - `childX = 200/2 + (cursor + 25) * 2.0 = 100 + (cursor + 25) * 2.0` ✓
  - 相对位置保持一致，整体放大 2 倍

## 向后兼容性

此修复完全向后兼容：
- 当 `scale = 1.0` 时，除以 scale 不改变结果
- 现有未使用缩放的代码不受影响
- 只修复了之前缩放时的错误行为

## 测试

参见 `test/test_layout_scale.cpp`，该测试创建了两个 Panel：
1. 一个未缩放 (scale=1.0)
2. 一个缩放 1.5 倍 (scale=1.5)

两个 Panel 都使用相同的 FlexLayout 配置，验证布局在不同缩放比例下的正确性。

## 相关文件

- `src/Layout.cpp`：FlexLayout::apply() 实现
- `src/Widget.cpp`：Panel::draw() 和 Panel::setScale() 实现
- `test/test_layout_scale.cpp`：测试用例

## 注意事项

⚠️ **重要**：`childOffsets` 始终存储**未缩放**的坐标，并在 `Panel::draw()` 中乘以 `scale`。这是布局系统的核心设计原则，未来的修改必须遵守这一约定。
