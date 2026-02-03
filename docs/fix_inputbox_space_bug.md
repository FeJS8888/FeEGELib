# InputBox 空格输入问题修复文档

## 问题描述

InputBox 控件在处理空格字符输入时存在异常行为：
1. **第一阶段问题（已修复）**：无法输入空格字符
2. **第二阶段问题（本次修复）**：空格可以输入但不显示，鼠标点击位置不准确

## 问题分析

### 第一阶段：输入问题（已解决）

**根本原因**：在 `include/sys_edit.h` 文件的第 81 行，`ES_WANTRETURN` 标志被错误地应用到了所有 EDIT 控件。

**解决方案**：将 `ES_WANTRETURN` 标志移除，仅在多行编辑框时添加。

### 第二阶段：显示和定位问题（本次修复）

**根本原因**：Windows GDI 文本渲染引擎在某些情况下不渲染空格字符，特别是：
- 尾随空格（行末空格）
- 某些字体和渲染模式下的空格

这导致：
1. 空格字符虽然被存储在内容中，但在 InputBox 中不可见
2. `measuretext()` 函数测量文本宽度时也可能不计算空格宽度
3. 鼠标点击定位不准确（因为测量宽度与实际显示不符）

**技术细节**：
- 使用 `ege_outtextxy` 绘制文本时，底层的 Windows GDI TextOut 函数会优化掉尾随空格
- `measuretext` 函数在测量包含空格的文本时，返回的宽度可能不包括空格
- 这是 Windows GDI 的已知行为，不是 bug

**解决方案（迭代优化）**：

**第一版方案（已废弃）**：将所有普通空格（U+0020）替换为不间断空格（U+00A0）
- ❌ **问题**：不间断空格在某些字体下宽度与普通空格不同，导致空格显示过宽

**最终方案（当前实现）**：在字符串末尾添加零宽连接符（U+200D）
- ✅ **优点**：保留普通空格的正常宽度
- ✅ **原理**：零宽字符不可见，但能强制 GDI 渲染所有前面的字符（包括尾随空格）
- ✅ **效果**：空格正常显示，宽度正确，鼠标定位准确

具体实现：
1. **渲染时添加**：在 `ege_outtextxy` 绘制文本前在字符串末尾添加 U+200D
2. **测量时添加**：在所有 `measuretext` 调用时也添加 U+200D，确保测量与渲染一致
3. **鼠标定位时添加**：在计算鼠标点击位置时也添加 U+200D，确保准确定位

## 修复内容

### 修改的文件

**src/Widget.cpp** - InputBox 类的绘制和事件处理

### 具体修改

1. **文本渲染**（第 912-922 行）：
   ```cpp
   // 修复空格显示问题：通过添加零宽字符强制 GDI 渲染尾随空格
   std::wstring renderContent = forceSpaceRendering(displayContent);
   ege_outtextxy(text_start_x, height / 2 - textRealHeight / 2, 
                renderContent.c_str(), btnLayer);
   ```

2. **文本宽度测量**（第 883-909 行）：
   添加 `forceSpaceRendering` 辅助函数，在所有 `measuretext` 调用时添加零宽字符：
   ```cpp
   inline std::wstring forceSpaceRendering(const std::wstring& str) {
       // 在字符串末尾添加零宽连接符（Zero-Width Joiner, U+200D）
       return str + L'\u200D';
   }
   measuretext(forceSpaceRendering(displayContent).c_str(), &full_text_width, &tmp, btnLayer);
   ```

3. **鼠标点击定位**（第 1024-1057 行）：
   在二分查找计算点击位置时也添加零宽字符：
   ```cpp
   measuretext(forceSpaceRendering(content.substr(0, mid)).c_str(), &char_x, &tmp, btnLayer);
   ```

## 为什么使用零宽连接符（U+200D）

零宽连接符（Zero-Width Joiner, ZWJ）是 Unicode 字符 U+200D：
- **不可见**：宽度为 0，不占据任何空间
- **强制渲染**：使 Windows GDI 渲染所有前面的字符，包括尾随空格
- **保持原样**：不改变普通空格的宽度和外观
- **兼容性**：所有现代字体和渲染引擎都支持

相比之前使用不间断空格（U+00A0）的方案：
- ❌ 不间断空格在某些字体下宽度与普通空格不同
- ✅ 零宽连接符完全不可见，不影响空格宽度

## 影响范围

- ✅ 修复了空格字符的显示问题
- ✅ 修复了鼠标点击定位不准确的问题
- ✅ 修复了光标位置计算（通过一致的测量）
- ✅ 不影响内部数据存储（仍使用普通空格 U+0020）
- ✅ 不影响 `getContent()` 返回值
- ✅ 向后兼容，无 API 变更

## 验证方法

### 测试步骤

1. 编译并运行 `test_inputbox_space.cpp`：
   ```bash
   mkdir build && cd build
   cmake ..
   make test_inputbox_space
   ./test_inputbox_space
   ```

2. 测试以下场景：
   - 输入普通文字
   - 输入空格字符
   - 输入连续多个空格
   - 在文字中间插入空格
   - 在文字末尾添加空格
   - 用鼠标点击空格位置，验证光标定位是否准确
   - 用键盘左右键移动光标，验证空格是否被正确检测

### 预期结果

- ✅ 空格字符在 InputBox 中可见显示
- ✅ 字符计数正确包含空格
- ✅ 鼠标点击能准确定位到空格位置
- ✅ 键盘导航能正确跨越空格
- ✅ 光标位置准确

## 相关信息

### Windows EDIT Control 样式标志

- **ES_AUTOHSCROLL**: 自动水平滚动（单行必需）
- **ES_MULTILINE**: 多行模式
- **ES_WANTRETURN**: 处理回车键（仅多行）
- **ES_LEFT**: 左对齐文本
- **WS_CHILD**: 子窗口
- **WS_BORDER**: 带边框

### InputBox 实现

InputBox 使用 `sys_edit` 类封装 Windows 原生 EDIT 控件：
- 默认为单行模式 (`create(false, 2)` 在 Widget.cpp:797)
- 不可见的 EDIT 控件用于接收输入
- 自定义绘制实现可见的输入框外观

## 兼容性

此修复：
- ✅ 不影响多行编辑框功能
- ✅ 不改变 InputBox API
- ✅ 向后兼容现有代码
- ✅ 符合 Windows API 最佳实践

## 参考资料

- [Microsoft Docs: Edit Control Styles](https://docs.microsoft.com/en-us/windows/win32/controls/edit-control-styles)
- [ES_WANTRETURN Flag Documentation](https://docs.microsoft.com/en-us/windows/win32/controls/edit-control-styles#es_wantreturn)
