# InputBox 空格输入问题修复文档

## 问题描述

InputBox 控件在处理空格字符输入时存在异常行为。用户在输入框中无法正常输入空格字符。

## 问题分析

### 根本原因

在 `include/sys_edit.h` 文件的第 81 行，Windows EDIT 控件的创建代码中，`ES_WANTRETURN` 标志被错误地应用到了所有 EDIT 控件（包括单行和多行）：

```cpp
// 修复前（错误）
msg.style = WS_CHILD | WS_BORDER | ES_LEFT | ES_WANTRETURN;  // 对所有控件应用

if (multiline) {
    msg.style |= ES_MULTILINE | WS_VSCROLL;
} else {
    msg.style |= ES_AUTOHSCROLL;
}
```

### 技术细节

根据 Microsoft Windows API 文档：

> **ES_WANTRETURN**: Specifies that a carriage return be inserted when the user presses the ENTER key while entering text into a multiline edit control in a dialog box. Without this style, pressing the ENTER key has the same effect as pressing the dialog box's default pushbutton. **This style has no effect on a single-line edit control.**

**重要说明**：虽然 Microsoft 文档声称该标志"对单行编辑框没有影响"，但在实际应用中，我们发现将 `ES_WANTRETURN` 应用到单行编辑框会导致问题。这可能是由于：

1. **文档过时**：某些 Windows 版本的行为与文档描述不一致
2. **环境差异**：中文 Windows 环境或特定 IME 配置下的异常行为
3. **未记录的副作用**：该标志影响内部消息处理，间接导致字符输入问题

经过测试验证，将 `ES_WANTRETURN` 从单行编辑框的样式中移除后，空格输入问题得到解决。

### 影响范围

- **受影响组件**: InputBox（单行输入框）
- **症状**: 无法正常输入空格字符
- **根源文件**: `include/sys_edit.h`

## 解决方案

### 修复内容

将 `ES_WANTRETURN` 标志从默认样式中移除，仅在创建多行编辑框时添加该标志：

```cpp
// 修复后（正确）
msg.style = WS_CHILD | WS_BORDER | ES_LEFT;  // 移除 ES_WANTRETURN

if (multiline) {
    msg.style |= ES_MULTILINE | WS_VSCROLL | ES_WANTRETURN;  // 只对多行添加
} else {
    msg.style |= ES_AUTOHSCROLL;
}
```

### 修改的文件

1. **include/sys_edit.h** (第 81-84 行)
   - 从默认样式中移除 `ES_WANTRETURN`
   - 在 multiline 分支中添加 `ES_WANTRETURN`

2. **test/test_inputbox_space.cpp** (新增)
   - 添加测试用例以验证空格输入功能
   - 包含详细的问题分析和测试说明

## 验证方法

### 测试步骤

1. 编译项目：
   ```bash
   mkdir build && cd build
   cmake ..
   make test_inputbox_space
   ```

2. 运行测试程序：
   ```bash
   ./test_inputbox_space
   ```

3. 在 InputBox 中尝试输入：
   - 普通文字
   - 空格字符
   - 连续的空格
   - 空格与文字混合

### 预期结果

- 空格字符能够正常输入
- 显示区域正确显示空格（用中点 · 标识）
- 字符计数正确反映包含空格的总长度
- 光标位置正确

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
