# InputBox Space Input Bug Fix Documentation

## Problem Description

The InputBox control exhibited abnormal behavior when handling space character input. Users were unable to properly input space characters in the input field.

## Problem Analysis

### Root Cause

In the `include/sys_edit.h` file at line 81, the Windows EDIT control creation code incorrectly applied the `ES_WANTRETURN` flag to all EDIT controls (both single-line and multi-line):

```cpp
// Before fix (incorrect)
msg.style = WS_CHILD | WS_BORDER | ES_LEFT | ES_WANTRETURN;  // Applied to all

if (multiline) {
    msg.style |= ES_MULTILINE | WS_VSCROLL;
} else {
    msg.style |= ES_AUTOHSCROLL;
}
```

### Technical Details

According to Microsoft Windows API documentation:

> **ES_WANTRETURN**: Specifies that a carriage return be inserted when the user presses the ENTER key while entering text into a multiline edit control in a dialog box. Without this style, pressing the ENTER key has the same effect as pressing the dialog box's default pushbutton. **This style has no effect on a single-line edit control.**

**Important Note**: While Microsoft documentation claims this flag "has no effect on a single-line edit control," we found that applying `ES_WANTRETURN` to single-line edit controls causes issues in practice. This may be due to:

1. **Outdated documentation**: Certain Windows versions behave inconsistently with the documentation
2. **Environment differences**: Abnormal behavior in Chinese Windows environments or specific IME configurations
3. **Undocumented side effects**: The flag affects internal message processing, indirectly causing character input issues

After testing, removing `ES_WANTRETURN` from single-line edit control styles resolved the space input problem.

### Impact Scope

- **Affected component**: InputBox (single-line input field)
- **Symptom**: Unable to properly input space characters
- **Source file**: `include/sys_edit.h`

## Solution

### Fix Implementation

Remove the `ES_WANTRETURN` flag from the default style and only add it when creating multi-line edit controls:

```cpp
// After fix (correct)
msg.style = WS_CHILD | WS_BORDER | ES_LEFT;  // Removed ES_WANTRETURN

if (multiline) {
    msg.style |= ES_MULTILINE | WS_VSCROLL | ES_WANTRETURN;  // Only for multiline
} else {
    msg.style |= ES_AUTOHSCROLL;
}
```

### Modified Files

1. **include/sys_edit.h** (lines 81-84)
   - Removed `ES_WANTRETURN` from default style
   - Added `ES_WANTRETURN` in multiline branch

2. **test/test_inputbox_space.cpp** (new file)
   - Added test case to validate space input functionality
   - Includes detailed problem analysis and test documentation

3. **docs/fix_inputbox_space_bug.md** (new file)
   - Chinese documentation
   
4. **docs/fix_inputbox_space_bug_en.md** (this file)
   - English documentation

## Verification

### Test Steps

1. Compile the project:
   ```bash
   mkdir build && cd build
   cmake ..
   make test_inputbox_space
   ```

2. Run the test program:
   ```bash
   ./test_inputbox_space
   ```

3. Test input in the InputBox:
   - Regular text
   - Space characters
   - Multiple consecutive spaces
   - Mixed text and spaces

### Expected Results

- Space characters input normally
- Display area correctly shows spaces (marked with middle dot ·)
- Character count correctly reflects total length including spaces
- Cursor position is accurate

## Related Information

### Windows EDIT Control Style Flags

- **ES_AUTOHSCROLL**: Auto horizontal scroll (required for single-line)
- **ES_MULTILINE**: Multi-line mode
- **ES_WANTRETURN**: Handle Enter key (multiline only)
- **ES_LEFT**: Left-align text
- **WS_CHILD**: Child window
- **WS_BORDER**: With border

### InputBox Implementation

InputBox uses the `sys_edit` class to wrap Windows native EDIT control:
- Defaults to single-line mode (`create(false, 2)` at Widget.cpp:797)
- Invisible EDIT control receives input
- Custom rendering implements visible input box appearance

## Compatibility

This fix:
- ✅ Does not affect multi-line edit control functionality
- ✅ Does not change InputBox API
- ✅ Backward compatible with existing code
- ✅ Follows Windows API best practices

## References

- [Microsoft Docs: Edit Control Styles](https://docs.microsoft.com/en-us/windows/win32/controls/edit-control-styles)
- [ES_WANTRETURN Flag Documentation](https://docs.microsoft.com/en-us/windows/win32/controls/edit-control-styles#es_wantreturn)
