# InputBox 空格输入问题解决方案

## 问题 (Problem)
InputBox 控件空格字符处理有三个阶段的问题：
1. **阶段一**：无法输入空格字符
2. **阶段二**：可以输入但不显示，鼠标点击定位不准确
3. **阶段三**：可以显示但空格过宽

The InputBox control had a three-phase space handling issue:
1. **Phase 1**: Could not input space characters
2. **Phase 2**: Spaces accepted but not displayed, mouse clicks inaccurate
3. **Phase 3**: Spaces displayed but too wide

## 根本原因 (Root Cause)

### 阶段一 (Phase 1) ✅ 已解决
`include/sys_edit.h:81` 中，`ES_WANTRETURN` 标志被错误地应用到所有 EDIT 控件。
The `ES_WANTRETURN` flag was incorrectly applied to all EDIT controls.

### 阶段二 (Phase 2) ✅ 已解决  
Windows GDI 在某些情况下不渲染普通空格（U+0020），特别是尾随空格。
Windows GDI doesn't render regular spaces (U+0020) in certain scenarios, especially trailing spaces.

### 阶段三 (Phase 3) ✅ 已解决
使用不间断空格（U+00A0）后，空格宽度在某些字体下与普通空格不同，导致显示过宽。
Using non-breaking space (U+00A0) caused spaces to appear wider than normal in some fonts.

## 解决方案 (Solution)

### 阶段一：ES_WANTRETURN 标志修复
**2 行代码更改** (2 lines changed):

```diff
File: include/sys_edit.h

- msg.style = WS_CHILD | WS_BORDER | ES_LEFT | ES_WANTRETURN;  // ❌ 错误
+ msg.style = WS_CHILD | WS_BORDER | ES_LEFT;                  // ✅ 修复

  if (multiline) {
-     msg.style |= ES_MULTILINE | WS_VSCROLL;                  // ❌ 缺少标志
+     msg.style |= ES_MULTILINE | WS_VSCROLL | ES_WANTRETURN;  // ✅ 修复
  }
```

### 阶段二 & 三：空格显示修复（最终方案）
**创建辅助函数** (Helper function):

```cpp
// src/Widget.cpp 第 12-18 行
inline std::wstring forceSpaceRendering(const std::wstring& str) {
    // 在字符串末尾添加零宽连接符（Zero-Width Joiner, U+200D）
    // 这样可以保留普通空格的正常宽度，同时强制 GDI 渲染尾随空格
    return str + L'\u200D';
}
```

**应用位置** (Applied in):
1. 文本渲染 (Text rendering) - `ege_outtextxy()`
2. 宽度测量 (Width measurement) - `measuretext()`
3. 鼠标定位 (Mouse positioning) - click position calculation

## 为什么使用零宽连接符 (Why Zero-Width Joiner?)
- ✅ 宽度为 0，完全不可见 (Width: 0, completely invisible)
- ✅ 强制 GDI 渲染所有字符包括空格 (Forces GDI to render all chars)
- ✅ 保留普通空格的正常宽度 (Preserves normal space width)
- ✅ 所有字体支持 (Supported by all fonts)

**方案对比** (Comparison):
- ❌ **不间断空格 (U+00A0)**: 宽度可能不同，显示过宽
- ✅ **零宽连接符 (U+200D)**: 不可见，不影响空格宽度

## 影响 (Impact)
✅ 修复了空格输入功能 (Fixed space input)  
✅ 修复了空格显示问题 (Fixed space display)  
✅ 修复了空格宽度问题 (Fixed space width)  
✅ 修复了鼠标点击定位 (Fixed mouse positioning)  
✅ 向后兼容，无 API 变更 (Backward compatible, no API changes)  
✅ 符合 Windows API 最佳实践 (Follows Windows API best practices)

## 文件更改 (Files Changed)
1. ✅ **include/sys_edit.h** - 阶段一修复 (Phase 1 fix)
2. ✅ **src/Widget.cpp** - 阶段二&三修复 + 辅助函数 (Phase 2&3 fix + helper)
3. ✅ **test/test_inputbox_space.cpp** - 测试用例 (Test case)
4. ✅ **docs/fix_inputbox_space_bug.md** - 详细文档 (Detailed docs)

## 提交历史 (Commits)
1. `61aedc9` - 移除单行编辑框的 ES_WANTRETURN 标志
2. `adc4ff6` - 将空格转换为不间断空格（后被替换）
3. `3901a79` - 重构为可重用辅助函数
4. `22334d0` - **最终修复**：使用零宽连接符替代不间断空格

## 用户反馈 (User Feedback)
1. **问题1**: 空格无法显示 ✅ 已修复 (`adc4ff6`)
2. **问题2**: 空格过宽 ✅ 已修复 (`22334d0`)

## 验证 (Verification)
- ✅ 代码审查通过 (Code review passed)
- ✅ CodeQL 安全扫描通过 (Security scan passed)  
- ✅ 用户确认问题已解决 (User confirmed fix)

## 参考 (References)
- Microsoft Docs: [Edit Control Styles - ES_WANTRETURN](https://docs.microsoft.com/en-us/windows/win32/controls/edit-control-styles#es_wantreturn)
- Unicode 零宽连接符 U+200D (Zero-Width Joiner)
- Windows GDI 空格渲染特性 (GDI space rendering behavior)
