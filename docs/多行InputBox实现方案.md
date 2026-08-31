# FeEGELib 泛用多行 InputBox 实现方案

## 1. 实现目标

将现有 InputBox 扩展为泛用输入框，在创建时指定单行或多行模式。单行和多行继续共用同一个 InputBox 类，通过 multiline 成员区分行为。

示例：

    InputBox* input = InputBoxBuilder()
        .setSize(400, 160)
        .setMultiline(true)
        .setContent(L"初始内容")
        .build();

需要支持显式换行、自动换行、光标、选区、键盘输入、剪贴板、鼠标定位、拖动选择、水平/垂直滚动、IME，以及 Panel/Box 嵌套和缩放。

## 2. 现有架构

当前 InputBox 采用“原生输入，Widget 自绘”的方式：隐藏的 Windows EDIT 负责键盘、剪切、复制、粘贴、焦点和 IME；InputBox 自己负责绘制背景、文字、选区和光标，并处理鼠标交互。

sys_edit::create() 已经支持 multiline 参数，所以不需要重新实现输入系统。主要工作是把 InputBox 当前的单行文字模型升级为多行文本布局模型。

## 3. 创建接口

InputBox 构造函数增加可选的 multiline 参数：

    InputBox(double cx, double cy, double w, double h, double r,
             bool multiline = false);

InputBoxBuilder 增加：

    InputBoxBuilder& setMultiline(bool multiline = true);

多行模式创建隐藏控件时调用 inv.create(true, 0)，单行模式继续调用 inv.create(false, 2)。这样可以保持现有单行 API 兼容，同时复用原生控件的输入法和剪贴板能力。

## 4. 统一文本布局

单行模式可以通过测量字符串前缀得到光标 X 坐标，但多行模式还必须知道光标所在行。因此需要建立统一布局结果，例如：

    struct TextLineLayout {
        int start;
        int end;
        float width;
        bool hardBreak;
    };

同时缓存每行范围、每个字符边界的 X 坐标，以及显示文本索引到真实内容索引的映射。

所有功能都必须依赖同一份布局结果：真实内容经过 IME 临时字符串处理后生成显示文本，再经过布局，最终统一用于文字绘制、光标、选区、鼠标命中和滚动计算。

这是整个实现的核心，不能让绘制、选区和鼠标命中分别使用不同的坐标算法。

## 5. 换行和行高

文本中的换行符直接结束当前行。换行符属于内容索引，但不绘制为可见字符。

自动换行时，以输入框可见宽度为限制，从当前行起点逐字符测量，找到最后一个不超出可见宽度的字符，然后从下一个字符继续布局。第一阶段可以采用字符级换行，后续再增加空格和标点优先的单词级换行。

行高建议使用 textHeight 加上少量 lineSpacing。每行的 Y 坐标为 paddingTop 加上行号乘以 lineHeight，再减去垂直滚动偏移。

## 6. 光标

真实内容中的光标仍使用字符索引 cursor_pos。绘制时，将它转换为所在行和行内列位置，得到 cursorX 和 cursorY。

上下方向键需要保存 preferredCursorX，使光标从长行移动到短行再返回时，尽量保持原来的列位置。

## 7. 内部滚动

Panel 的滚动和 InputBox 的滚动必须分离：Panel 滚动整个 InputBox，InputBox 滚动自己的文字、选区和光标。

InputBox 至少需要维护 scroll_offset_x 和 scroll_offset_y。滚动范围为：

    maxScrollX = max(0, longestLineWidth - visibleWidth)
    maxScrollY = max(0, totalTextHeight - visibleHeight)

所有滚动值都限制在 0 到 maxScroll 的范围内。输入、删除、粘贴、方向键、鼠标点击、IME 更新和拖动选择后，都要调用 ensureCursorVisible()。

多行滚轮只修改垂直偏移，并且只在鼠标位于输入框内部时生效。

## 8. 鼠标命中和拖动选择

多行鼠标命中必须同时使用 X 和 Y：先根据 Y 找到最近的文本行，再根据 X 找到该行最近的字符边界，最后转换为真实内容索引。

建议增加：

    int charPositionFromLocal(float localX, float localY) const;

点击所有行上方时定位到第一行，点击所有行下方时定位到最后一行，点击行左侧或右侧时分别定位到行首或行尾。

拖动选择时记录 dragBegin 和 dragEnd。鼠标移动后重新计算 dragEnd，并将光标同步到选择末端。鼠标拖出输入框边界后，需要根据上下左右边缘自动滚动并继续扩展选区。

## 9. 多行选区绘制

多行选区需要按行拆分：第一行从选区起点绘制到行尾，中间行整行绘制，最后一行从行首绘制到选区终点。

绘制顺序应为背景、选区高亮、文字、光标和 IME 下划线。文字、选区和光标都必须经过 InputBox 内容区域裁剪。

## 10. 键盘行为

多行模式建议支持 Left/Right、Up/Down、Home/End、Ctrl+Home、Ctrl+End、Enter、PageUp、PageDown 和 Ctrl+A。单行模式下 Enter 不插入换行。

内容变化后，从 sys_edit 同步内容、光标和选区，重建布局，修正滚动位置，并请求父 Panel 重绘。

## 11. IME 处理

IME 组合串不直接写入真实内容，而是临时插入光标位置：

    显示文本 = content[0 .. cursor_pos]
             + IMECompositionString
             + content[cursor_pos .. end]

布局时必须区分真实内容索引、显示文本索引、IME 插入点、IME 内部光标位置和提交后的最终光标位置，因此需要显示索引到真实索引的映射。

IME 候选窗口位置必须使用布局后的光标屏幕坐标，并叠加 InputBox、Panel、Box 等父容器的绝对偏移以及内部滚动偏移。

## 12. 绘制流程

InputBox::draw() 建议按以下顺序执行：同步隐藏 EDIT 的内容和选区；构建包含 IME 的显示文本；重建布局；计算滚动范围；确保光标可见；绘制背景；设置内容裁剪；绘制选区；绘制各行文字；绘制光标和 IME 下划线；更新 IME 坐标；绘制波纹和边框；恢复裁剪并合成到目标图像。

所有内容坐标都使用同一套公式：内边距加上行内坐标减去水平滚动偏移，以及内边距加上行号乘以行高减去垂直滚动偏移。

## 13. Panel、Box 和缩放

Panel 和 Box 继续负责子控件布局、位置、外层裁剪、外层滚动和嵌套偏移。InputBox 只负责自己的文本布局、内部滚动、光标、选区和 IME 局部坐标。

缩放时需要同步处理输入框尺寸、圆角、字体、行高、内边距、光标尺寸、滚动偏移和布局缓存。建议缩放后重新创建图层并重新布局，不直接复用旧的文字测量结果。

## 14. 建议的内部接口

    void rebuildTextLayout();
    int displayPositionForContentPosition(int contentPos) const;
    int contentPositionForDisplayPosition(int displayPos) const;
    int lineIndexForDisplayPosition(int displayPos) const;
    float xForDisplayPosition(int displayPos) const;
    int charPositionFromLocal(float localX, float localY) const;
    void ensureCursorVisible();
    void selectAll();
    void scrollBy(double pixels);
    void updateDragAutoScroll(int mouseX, int mouseY);

## 15. 实施顺序

1. 增加 multiline 成员、构造参数和 Builder 接口。
2. 多行模式创建隐藏原生 EDIT，并保证单行行为不变。
3. 实现显式换行、自动换行和布局缓存。
4. 将文字绘制、光标和选区改为使用布局结果。
5. 实现二维鼠标命中和跨行拖动选择。
6. 加入垂直滚动、水平滚动和光标自动跟随。
7. 实现上下方向键、Home/End、PageUp/PageDown 和多行 Enter。
8. 修正 IME 组合文本、光标和候选窗口位置。
9. 验证 Panel/Box 嵌套、外层滚动和缩放。

## 16. 测试重点

应至少验证空文本、长文本、显式换行、自动换行、中英文混排、光标首尾滚动、跨行正向/反向选择、Ctrl+A、删除、粘贴、多行中文输入法、Panel/Box 嵌套、Panel 滚动与 InputBox 内部滚动同时存在，以及缩放后的文字、光标和选区位置。

## 17. 总结

多行输入框的关键不是简单启用 ES_MULTILINE，而是建立统一的文本布局系统。文字绘制位置、光标位置、选区位置、鼠标命中结果、滚动范围和 IME 候选窗口位置，都应该由同一份布局结果决定。

这样才能保证单行、多行、换行、选择、滚动、IME、Panel 嵌套和缩放之间保持一致，避免文字偏移和交互错位。
