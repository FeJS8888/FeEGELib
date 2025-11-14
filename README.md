# FeEGELib V2.12.0.0

[![Page Views Count](https://badges.toozhao.com/badges/01JH01V78N1S0P0A9RVFJ5SWXZ/blue.svg)](https://badges.toozhao.com/stats/01JH01V78N1S0P0A9RVFJ5SWXZ "Get your own page views count badge on badges.toozhao.com")

[![Repo Card](https://github-readme-stats.vercel.app/api/pin/?username=FeJS8888\&repo=FeEGELib\&show_owner=true\&theme=catppuccin_latte)](https://github.com/FeJS8888/FeEGELib)

> 更新日期：2025/11/14
> 基于 [EGE V24.04](https://xege.org/) 开发的一套图形开发框架

---

## 🔗 相关资源

* **EGE 项目仓库**：[wysaid/xege](https://github.com/wysaid/xege)
* **EGE 官方网站**：[https://xege.org](https://xege.org)
* **快速上手 EGE 图形库教程**：[CSDN 教程](https://blog.csdn.net/qq_39151563/article/details/100161986)

---

## 🔧 功能介绍

`FeEGELib` 封装了一个基于 EGE 的 `Element` 组件类，提供简洁灵活的图形开发体验，功能包括但不限于：

* 🎯 组件图层管理（遮挡控制）
* ✋ 支持组件移动、旋转、缩放等控制操作
* 🔧 事件订阅 / 取消订阅机制
* 🔧 异步图像加载
* 🔧 碰撞检测系统
* 🌈 缓动动画系统
* 🔧 UI 组件集（Button / Panel / InputBox / Toggle / Slider / Progressbar 等）
* 🔧 简易物理系统（待完善，目前存在 bug）

---

## 🚀 使用方法

### 1️⃣ 配置编译环境

编译参数：

```bash
-std=c++23 -O2
```

链接参数：

```bash
-static -static-libgcc -lgraphics -lgdi32 -lgdiplus
```

确保已正确配置 EGE 图形库。

---

### 2️⃣ 使用 VSCode + CMake 编译（推荐）

#### 前置要求

* 已安装 **Visual Studio Code**
* 已安装 **CMake** (版本 >= 3.14)
* 已安装 **MinGW-w64** 或其他 C++ 编译器
* 已安装 VSCode 扩展：**C/C++** 和 **CMake Tools**

#### 配置步骤

1. **克隆仓库**

   ```bash
   git clone https://github.com/FeJS8888/FeEGELib.git
   cd FeEGELib
   ```

2. **使用 VSCode 打开项目**

   ```bash
   code .
   ```

3. **配置 CMake**

   * 打开 VSCode 命令面板（`Ctrl+Shift+P`）
   * 输入 `CMake: Configure` 并选择编译器（推荐 MinGW）

4. **编译项目**

   * 按 `F7` 或使用命令面板输入 `CMake: Build`
   * 编译完成后可执行文件位于 `build/` 目录

5. **运行程序**

   * 按 `Shift+F5` 或在终端运行：
     ```bash
     ./build/FeEGEApp.exe
     ```

#### 注意事项

* 确保 `CMakeLists.txt` 中的路径正确
* 如果遇到链接错误，检查 `lib/libgraphics.a` 是否存在
* 编译需要 Windows 环境（EGE 库依赖 GDI/GDI+）

---

### 3️⃣ 引入头文件

将 `FeEGELib.h` 放入代码目录，并包含：

```cpp
#include "FeEGELib.h"
using namespace FeEGE;
```

---

### 4️⃣ 快速测试

将以下示例代码复制并编译运行：

```cpp
#include "FeEGELib.h"
using namespace FeEGE;

int main() {
    init(1920 >> 1, 1080 >> 1);       // 初始化窗口
    pen::font(90, "幼圆");            // 设置字体
    pen::print(0, 0, "test pen");     // 输出文字
    start();                          // 启动事件循环
    return 0;
}
```

#### 运行效果预览：

![运行截图](https://s11.ax1x.com/2024/02/15/pFG1vbq.png)

---

## 📦 项目文件结构

```
FeEGELib/
├── CMakeLists.txt           # CMake 构建配置文件
├── LICENSE                  # 许可证文件
├── README.md                # 项目说明文档
├── main.cpp                 # 主程序入口（用户编写）
├── docs/                    # 文档目录
├── include/                 # 头文件目录
│   ├── FeEGELib.h          # 主头文件（包含所有其他头文件）
│   ├── Base.h              # 基础类和工具函数
│   ├── Element.h           # Element 组件类
│   ├── Widget.h            # UI 组件类
│   ├── Collision.h         # 碰撞检测系统
│   ├── Layout.h            # 布局管理器
│   ├── ege.h               # EGE 图形库主头文件
│   ├── ege.zh_CN.h         # EGE 中文支持
│   ├── graphics.h          # 图形接口
│   ├── font_manager.h      # 字体管理器
│   ├── sys_edit.h          # 系统编辑控件
│   ├── json.hpp            # JSON 解析库
│   └── ege/                # EGE 扩展头文件
├── lib/                     # 库文件目录
│   └── libgraphics.a       # EGE 图形库静态库
└── src/                     # 源代码实现目录
    ├── Base.cpp            # 基础类实现
    ├── Element.cpp         # Element 组件实现
    ├── Widget.cpp          # UI 组件实现
    ├── Collision.cpp       # 碰撞检测实现
    ├── Layout.cpp          # 布局管理器实现
    ├── font_manager.cpp    # 字体管理器实现
    └── sys_edit.cpp        # 系统编辑控件实现
```

---

## 📮 示例界面

![UI 示例](https://s11.ax1x.com/2024/02/15/pFG1jrn.png)

---

## 📮 联系作者

欢迎 issue / PR / Star！

GitHub: [@FeJS8888](https://github.com/FeJS8888)
