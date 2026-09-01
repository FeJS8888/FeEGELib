# FeEGELib 多行 InputBox 设计说明

## 1. 目标与结论

多行 InputBox 不是简单地给 Windows `EDIT` 增加 `ES_MULTILINE`。真正需要解决的是：文字绘制、自动换行、光标、选区、鼠标命中、内部滚动、IME 候选窗口定位，以及 Panel/Box 嵌套与缩放，都必须使用同一套文本坐标体系。

当前实现采用“原生输入后端 + Widget 自绘模型”的混合架构：

```text
应用线程
  InputBox::content / 光标 / 选区 / 布局 / 绘制
        ↑                                  ↓
   消费待处理输入事件                 提交原生状态快照
        ↑                                  ↓
UI 消息线程
  隐藏 Windows EDIT：键盘、剪贴板、IME、候选窗口后端
```

职责边界如下：

- `InputBox` 是文本、光标、选区和布局的唯一权威来源。
- 隐藏的 Windows `EDIT` 只负责接收系统输入与维持原生 IME 会话。
- UI 消息线程不得直接修改 `InputBox`。
- 应用线程不得在原生 IME 正在组合时直接向 `EDIT` 发送 `WM_SETTEXT` 或 `EM_SETSEL`。

这种设计保留了系统 IME、剪贴板与候选框能力，同时支持完全自定义的多行渲染和控件嵌套。

## 2. 对外接口

单行与多行共用同一个 `InputBox` 类型，只通过 `multiline` 区分行为：

```cpp
InputBox(double cx, double cy, double w, double h, double r,
         bool multiline = false);
```

推荐使用 Builder：

```cpp
InputBox* input = InputBoxBuilder()
    .setCenter(400, 300)
    .setSize(500, 220)
    .setRadius(10)
    .setTextHeight(18)
    .setMultiline(true)
    .setContent(L"第一行\n第二行")
    .build();
```

兼容的单行用法不变：

```cpp
InputBox* input = InputBoxBuilder()
    .setSize(320, 42)
    .setMultiline(false)
    .build();
```

多行模式的隐藏 EDIT 会使用 `ES_MULTILINE | ES_AUTOVSCROLL`；它不参与最终可见绘制。

## 3. 状态模型

`InputBox` 在应用线程维护以下核心状态：

| 状态 | 含义 |
| --- | --- |
| `content` | 真实、已确认的文本内容。 |
| `cursor_pos` | 真实内容中的 UTF-16 插入位置。 |
| `dragBegin` / `dragEnd` | 选区锚点与活动端；选区语义为 `[start, end)`。 |
| `IMECompositionString` | 尚未确认的 IME 临时组合串，不直接写入 `content`。 |
| `IMECursorPos` | IME 组合串内部光标位置。 |
| `imeStartPos` | 当前 IME 会话开始时的内容插入位置。 |
| `textLines` | 显示文本按显式换行和自动换行得到的行布局。 |
| `scroll_offset_y` | 多行文本内部垂直滚动偏移。 |

其中最重要的原则是：原生 EDIT 的私有文本不是模型的反向同步来源。应用层不再在 `draw()` 中通过 `WM_GETTEXT` 覆盖 `content`。

## 4. 显示文本与真实内容映射

非 IME 状态下，显示文本就是内容本身：

```text
layoutDisplayContent = content
```

IME 组合期间，组合串只作为临时显示层插入光标处：

```text
layoutDisplayContent = content[0, cursor_pos)
                     + IMECompositionString
                     + content[cursor_pos, end)
```

例如：

```text
真实内容：      A B C
光标位置：          ↑
组合串：          nihao

显示文本：      A B nihao C
真实内容下标：  0 1  2     2
显示文本下标：  0 1  2...6 7
```

因此布局缓存还维护 `displayToContent` 映射：

- 组合串前的显示位置与内容位置相同；
- 组合串内部的位置都映射到同一个内容插入点；
- 组合串后的显示位置减去组合串长度后，得到真实内容位置。

相关接口包括：

```cpp
int displayPositionForContentPosition(int contentPos) const;
int contentPositionForDisplayPosition(int displayPos) const;
int lineIndexForDisplayPosition(int displayPos) const;
float xForDisplayPositionOnLine(int displayPos, int lineIndex) const;
int charPositionFromLocal(float localX, float localY) const;
```

所有绘制和交互必须通过这份映射，而不是分别根据 `content` 或临时字符串自行计算位置。

## 5. 文本布局与换行

每一行由 `TextLineLayout` 表示：

```cpp
struct TextLineLayout {
    int start;
    int end;
    float width;
    bool hardBreak;
};
```

布局输入是 `layoutDisplayContent`。构建规则如下：

1. `\r`、`\n`、`\r\n` 都视为显式换行；换行符保留在内容索引中，但不作为可见文字绘制。
2. 逐字符测量当前行宽度；下一个字符超出可用宽度时，在该字符前自动换行。
3. 每行保存其显示文本区间、测得宽度及是否由显式换行结束。
4. 空文本和末尾显式换行都要保留可放置光标的空行。

当前多行模式自动换行，因此只需内部垂直滚动；单行模式继续使用原有水平滚动偏移。

### 自动换行边界

自动换行处，上一行末尾和下一行开头可能共享同一个显示位置：

```text
第一行： [start, end)
第二行： [end, ...)
```

因此不能只用字符下标求 X。必须使用“显示位置 + 具体行号”计算：

```cpp
xForDisplayPositionOnLine(position, lineIndex)
```

这能避免选区刚好停在换行边界时，错误地把下一行开头高亮出来，也能让光标在“行尾”和“下一行行首”显示在正确的物理位置。

## 6. 绘制、光标与选区

多行绘制顺序为：

1. 消费原生输入事件。
2. 重建显示文本与行布局。
3. 计算滚动范围，并确保光标可见。
4. 绘制背景并裁剪到输入框内容区域。
5. 按行绘制选区高亮。
6. 按行绘制文字。
7. 绘制光标和 IME 下划线。
8. 更新 IME 候选窗口位置。
9. 绘制波纹、边框并合成图层。

每行的绘制 Y 坐标为：

```text
lineY = paddingY + lineIndex * lineHeight - scroll_offset_y
```

选区先从真实内容区间转换为显示区间，再按行求交集。每行都使用该行的起止 X，因此跨行选区自然会分为“首行局部、中间整行、末行局部”。

光标使用真实位置 `cursor_pos`。IME 组合存在时，显示光标位置会额外加上 `IMECursorPos`，但真实内容的光标不会提前移动。

## 7. 鼠标、键盘和滚动

### 鼠标命中

多行点击需要二维反查：

1. 将屏幕坐标转换为 InputBox 局部坐标。
2. 加回 `scroll_offset_y`，根据 Y 找到对应文本行。
3. 在该行逐位置比较 X，找到最近的字符边界。
4. 用 `displayToContent` 映射回真实内容位置。

点击文本左侧定位到行首，右侧定位到行尾；点击视口外的上下区域会被限制在第一行或最后一行。

### 拖动选择

拖选持续维护 `dragBegin` 与 `dragEnd`。即使鼠标离开输入框，控件仍保持鼠标所有权并继续更新选区。

多行拖选越过上下边缘时，会按行高自动滚动；滚动后会再次按鼠标位置反查字符位置，使选区能持续向上或向下延伸。

### 键盘

键盘操作统一修改应用线程模型：

- `Backspace` / `Delete`：删除选区，或删除前后一个输入边界；
- `Left` / `Right`：移动一个 UTF-16 输入边界；
- `Up` / `Down`：通过 `preferredCursorX` 尽可能保持视觉列位置；
- `Home` / `End`：移动到当前视觉行的首尾；
- `PageUp` / `PageDown`：按可见行数移动；
- `Ctrl+A`：选中全部；
- 多行 `Enter`：插入规范化后的 `\r\n`；单行模式不插入换行。

删除对 CRLF 和 UTF-16 代理项对做了基础边界处理，避免把换行或 emoji 的高低代理项拆开。完整字素簇编辑仍可作为后续优化。

## 8. 跨线程输入事件模型

xEGE 下，原生 EDIT 的窗口过程运行在 UI 消息线程，而 Widget 绘制和大部分状态运行在应用线程。若 UI 线程直接调用 `InputBox::deleteSelectedText()`、`commitIMEString()` 或 `moveCursor()`，会造成 `std::wstring`、选区和布局缓存的数据竞争。

因此 `sys_edit` 使用带互斥锁的 `PendingInputEvent` 队列。UI 线程只采集事件：

```text
WM_KEYDOWN / WM_CHAR / WM_PASTE / WM_CUT
WM_IME_STARTCOMPOSITION
WM_IME_COMPOSITION（组合串或结果串）
WM_IME_ENDCOMPOSITION
WM_KILLFOCUS
```

应用线程在 `InputBox::processPendingNativeEvents()` 中按顺序消费事件，并调用：

```cpp
insertInputText(...)
deleteBackward()
deleteForward()
deleteSelectedText()
beginIMEComposition()
commitIMEString(...)
```

这保证 `content`、光标、选区、IME 显示层和布局缓存始终由同一线程更新。

## 9. 原生 EDIT 状态镜像

应用模型仍需要镜像给隐藏 EDIT，原因包括：下一次 IME 会话需要正确的后端文本/选区，系统需要维持输入上下文。

但不能采取以下不安全模式：

```text
应用线程检查“IME 未组合”
  → 直接 SendMessage(WM_SETTEXT)
  → UI 线程恰好开始新的 IME 组合
```

检查与发送之间存在竞态，可能打断新会话。

当前方案是快照投递：

```text
应用线程 InputBox::syncNativeEditState()
  → sys_edit::queueNativeState(content, selectionBegin, selectionEnd)
  → 保存最新快照并 PostMessage 给隐藏 EDIT

UI 线程 sys_edit::applyPendingNativeState()
  → 仅在没有 IME 组合时调用原生 EDIT 的 WM_SETTEXT / EM_SETSEL
```

多个请求会合并为最新快照。`WM_IME_STARTCOMPOSITION` 前会先应用已有快照；`WM_IME_ENDCOMPOSITION` 后也会尝试应用组合期间延后的快照。

这样“是否安全”和实际原生调用发生在同一个 UI 线程，消除了跨线程检查后的窗口期，也不会在持锁状态下等待 UI 线程。

## 10. IME 设计

IME 同时需要满足两件事：

1. InputBox 需要自己绘制组合串和提交后的真实内容。
2. 隐藏 EDIT 仍应完成原生 IME 生命周期，否则部分输入法会损坏多字结果串或候选状态。

当前流程：

1. `WM_IME_STARTCOMPOSITION`
   - UI 线程先应用安全的旧快照；
   - 标记原生 IME 进入组合；
   - 入队 `ImeStart`；
   - 转发消息给原生 EDIT。
2. `WM_IME_COMPOSITION`
   - `GCS_COMPSTR`：复制组合串和 IME 内部光标位置，入队 `ImeUpdate`；
   - `GCS_RESULTSTR`：复制完整结果字符串，入队 `ImeResult`；
   - 转发消息给原生 EDIT，以维持其内部状态。
3. `WM_IME_ENDCOMPOSITION`
   - 先转发给原生 EDIT；
   - 再标记会话结束并入队 `ImeEnd`；
   - 尝试应用延后的模型快照。
4. 应用线程处理 `ImeResult`
   - 将完整结果串插入 `imeStartPos`；
   - 更新真实光标和选区；
   - 请求镜像最新模型，但该镜像由 UI 线程选择安全时机执行。

### 多汉字确认问题

一次确认多个汉字时，不能在收到 `GCS_RESULTSTR` 后立刻认为 IME 已结束。结果消息之后通常仍会有 `WM_IME_ENDCOMPOSITION`，某些 IME 还会在结束过程中产生内部 `WM_CHAR`。

若此时应用线程直接重置隐藏 EDIT 的文本，IME 的内部组合缓冲可能被打断，表现为多个字重复为最后一个字、候选异常或重复提交。

因此实现中：

- 原生组合状态持续到真正的 `WM_IME_ENDCOMPOSITION`；
- 组合期内的原生 `WM_CHAR` 仅转发给隐藏 EDIT，不作为普通 `TextInput` 再次写入模型；
- InputBox 只使用独立复制的、完整 `GCS_RESULTSTR` 更新 `content`；
- 文本/选区镜像经 UI 线程快照队列延后执行。

## 11. 焦点、取消和生命周期

失焦或鼠标重新点击时，原生 IME 可能还处于组合状态。`killIME()` 会读取当前组合串、取消原生组合，并将需要提交的字符串作为事件入队；应用线程随后决定如何更新模型。

销毁时，`sys_edit` 不再持有指向 `InputBox` 的反向裸指针。销毁中的 UI 消息至多写入 `sys_edit` 自己的队列，不会回调已经析构的 InputBox。

队列和待应用的原生状态快照在 `sys_edit::destroy()` 中清理，以避免窗口销毁后遗留消息继续访问旧状态。

## 12. Panel、Box、缩放与 IME 位置

InputBox 的内部文本布局只处理自己的内边距、行高和内部滚动；Panel/Box 继续负责控件整体的位置、外层裁剪、外层滚动与缩放。

IME 候选框的位置由最终绘制出的光标位置计算，需叠加：

- InputBox 自身位置；
- Panel/Box 的绝对位置偏移；
- InputBox 内部滚动偏移；
- 当前缩放比例；
- 外层绘制偏移。

缩放变化后应重新创建图层、重新设置字体、重新测量文本并重建布局，而不能复用旧字号下的行宽缓存。

## 13. 已知边界与后续优化

当前实现优先保证输入正确性与线程安全，后续可继续改进：

- 布局目前以逐字符测量为主，超长文本可增加前缀宽度缓存或更高效的断行算法；
- 自动换行可扩展为空格、标点或单词优先断行；
- 编辑边界可从 UTF-16 代理项对升级为完整 Unicode 字素簇；
- IME 事件可增加会话编号，进一步过滤极端情况下延迟到达的旧事件；
- 可加入 Windows GUI 自动化测试，覆盖不同输入法、候选词切换和失焦提交。

## 14. 回归验证清单

至少应验证以下场景：

- 单行：普通输入、Backspace、Delete、选区删除、Ctrl+A、复制、剪切、粘贴和方向键；
- 多行：显式换行、自动换行、跨行选区、行首行尾删除、滚轮滚动、光标自动滚动；
- 中文 IME：单字确认、一次确认多个汉字、连续词组确认、候选词切换、选区替换、点击其他位置、失焦提交与重新聚焦；
- 布局环境：Panel 嵌套、Box 嵌套、多层偏移、缩放、多行滚动后的 IME 候选框位置；
- 生命周期：输入过程中删除控件、窗口关闭、快速连续删除和快速输入。

交互式回归入口为：

```text
test/test_multiline_input.cpp
```

## 15. 总结

多行 InputBox 的核心不是启用 `ES_MULTILINE`，而是建立统一的“内容索引—显示索引—行布局—像素坐标”体系，并严格区分应用线程模型与 UI 线程的原生输入后端。

只要文字、光标、选区、鼠标命中、滚动、IME 下划线和候选窗口位置都由同一份布局结果推导，且所有模型修改都回到应用线程处理，多行、缩放、嵌套和中文输入才能保持一致、可维护且不易崩溃。
