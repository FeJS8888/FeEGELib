# InputBox 空格输入问题解决方案

## 问题 (Problem)
InputBox 控件无法正常处理空格字符输入。  
The InputBox control could not properly handle space character input.

## 根本原因 (Root Cause)
`include/sys_edit.h:81` 中，`ES_WANTRETURN` 标志被错误地应用到所有 EDIT 控件（包括单行和多行），而该标志应该只用于多行编辑框。  
The `ES_WANTRETURN` flag was incorrectly applied to all EDIT controls (both single-line and multiline), when it should only be used for multiline edit controls.

## 解决方案 (Solution)
仅用 **2 行代码更改** 修复了问题：
Fixed with **only 2 lines of code changed**:

```diff
- msg.style = WS_CHILD | WS_BORDER | ES_LEFT | ES_WANTRETURN;
+ msg.style = WS_CHILD | WS_BORDER | ES_LEFT;

  if (multiline) {
-     msg.style |= ES_MULTILINE | WS_VSCROLL;
+     msg.style |= ES_MULTILINE | WS_VSCROLL | ES_WANTRETURN;
  }
```

## 影响 (Impact)
✅ 修复了 InputBox 中的空格输入问题  
✅ Fixed space input in InputBox

✅ 不影响多行编辑框功能  
✅ No impact on multiline edit controls

✅ 向后兼容，无 API 变更  
✅ Backward compatible, no API changes

✅ 符合 Windows API 最佳实践  
✅ Follows Windows API best practices

## 文件更改 (Files Changed)
- `include/sys_edit.h` - **核心修复 (Core fix)**
- `test/test_inputbox_space.cpp` - 测试用例 (Test case)
- `docs/fix_inputbox_space_bug.md` - 中文文档 (Chinese docs)
- `docs/fix_inputbox_space_bug_en.md` - 英文文档 (English docs)

## 验证 (Verification)
- ✅ 代码审查通过 (Code review passed)
- ✅ CodeQL 安全扫描通过 (Security scan passed)  
- ⏳ 需要 Windows 环境手动测试 (Manual testing requires Windows)

## 参考 (References)
- Microsoft Docs: [Edit Control Styles - ES_WANTRETURN](https://docs.microsoft.com/en-us/windows/win32/controls/edit-control-styles#es_wantreturn)
