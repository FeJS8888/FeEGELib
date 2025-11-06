# FeEGELib 测试套件

本目录包含 FeEGELib 的单元测试。

## 测试文件列表

### 1. test_base.cpp
测试 Base 模块的核心功能：
- Position 类的构造函数、运算符和方法
  - 向量加法、减法、数乘
  - 点积、长度、归一化
  - 垂直向量计算
- SimpleOptional 模板类
  - 空值和有值状态
  - value_or 方法
- 工具函数
  - clamp() - 数值限制函数
  - fixed() - 浮点数转整数

### 2. test_collision.cpp
测试 Collision 模块的碰撞检测功能：
- transformPosition() - 多边形位置变换
- transformShape() - 多边形形状变换（缩放和旋转）
- isPointInConvexPolygon() - 点在凸多边形内判断
- isTouched() - 两个凸多边形碰撞检测
  - 不碰撞情况
  - 碰撞情况
  - 三角形碰撞
- getSeparateDistance() - 获取分离距离
- getSafeDistance() - 获取安全距离

### 3. test_element_basic.cpp
测试 Element 模块的基础功能：
- sgn() - 符号函数
- 全局位置函数（getLastPixel, getMousePos）
- Animate 类构造函数和动画函数
- Ease 命名空间中的缓动动画
- 路径处理函数
  - getCurrentPath()
  - resolvePath()
- 编码检测函数
  - isValidUTF8()
  - detectEncoding()

### 4. test_layout.cpp
测试 Layout 模块的布局功能：
- FlexLayout 类
  - 默认值测试
  - Setter 方法
  - 链式调用
- FlexLayoutBuilder 构建器模式
- LayoutDirection 枚举（Row/Column）
- LayoutAlign 枚举（Start/Center/End/SpaceBetween）

## 编译和运行

### 前置条件
- Windows 操作系统（需要 GDI/GDI+ 支持）
- CMake 3.14+
- C++23 兼容的编译器（如 MinGW-w64 with GCC 13+）
- EGE 图形库（libgraphics.a）

### 编译步骤

1. 创建构建目录：
```bash
mkdir build
cd build
```

2. 配置 CMake：
```bash
cmake ..
```

3. 编译所有测试：
```bash
cmake --build .
```

### 运行测试

编译后，所有测试可执行文件将位于 `build/bin/` 目录下：

```bash
# 运行 Base 模块测试
./build/bin/test_base.exe

# 运行 Collision 模块测试
./build/bin/test_collision.exe

# 运行 Element 基础测试
./build/bin/test_element_basic.exe

# 运行 Layout 模块测试
./build/bin/test_layout.exe
```

## 测试覆盖范围

本测试套件覆盖以下核心功能：

### 数学和几何 (Base)
- ✓ 二维向量运算
- ✓ 向量归一化和垂直向量
- ✓ 数值限制和转换

### 碰撞检测 (Collision)
- ✓ 凸多边形碰撞检测（SAT算法）
- ✓ 点在多边形内判断
- ✓ 多边形变换（平移、缩放、旋转）
- ✓ 分离距离计算

### 元素管理 (Element)
- ✓ 基础工具函数
- ✓ 动画系统基础
- ✓ 路径和编码处理

### 布局系统 (Layout)
- ✓ Flex 布局配置
- ✓ 构建器模式
- ✓ 布局方向和对齐方式

## 注意事项

1. **平台限制**：由于 FeEGELib 依赖 Windows GDI/GDI+ 库，测试必须在 Windows 环境下编译和运行。

2. **图形测试**：某些涉及图形界面的功能（如 Widget 的完整测试）未包含在本测试套件中，因为它们需要图形窗口初始化。

3. **内存管理**：部分测试涉及的类（如 Widget）依赖图形库资源，在实际使用中需要注意资源管理。

## 扩展测试

未来可以添加的测试：
- Widget 完整功能测试（需要图形环境）
- Element 完整生命周期测试
- 事件系统测试
- 动画系统完整测试
- 字体管理器测试

## 问题报告

如果发现测试失败或有建议，请在 GitHub 仓库中提交 Issue。
