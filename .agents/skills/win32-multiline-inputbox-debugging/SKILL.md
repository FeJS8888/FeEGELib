---
name: win32-multiline-inputbox-debugging
description: "Diagnose and fix Win32-backed multiline InputBox editing failures and long-text regressions. Trigger keywords: InputBox, multiline, Win32 EDIT, sys_edit, TextBuffer, IME, GCS_COMPSTR, GCS_RESULTSTR, WM_CHAR, EM_LIMITTEXT, ES_AUTOVSCROLL, cannot type, committed text disappears, multi-line input, long-text lag, cursor scrolling."
---

# Win32 Multiline InputBox Debugging

Use this skill when a self-drawn input widget delegates keyboard or IME input to a hidden Win32 `EDIT`. It is especially useful when typing, selection, IME commit, wrapping, or scrolling changes according to line count or document size.

Do not assume an IME-looking failure originates in IME handling. First identify the earliest layer whose state does not change.

## System Model

Trace the complete data path before changing code:

```text
keyboard / IME
    -> hidden Win32 EDIT
    -> subclass message handler (sys_edit)
    -> synchronization or local patch validation
    -> editable model (TextBuffer)
    -> visual-line cache and scroll state
    -> self-drawn InputBox
```

Inspect the repository's actual equivalents of each layer. Record which component is authoritative for normal characters, clipboard operations, undo, and IME results; these may differ.

## Debugging Workflow

### 1. Reduce the Trigger

Replace vague conditions such as "large text" with the smallest discriminating case. Test at least:

- empty text, one line, two lines, and many hard lines;
- short and long logical paragraphs;
- insertion at the beginning, middle, and end;
- English `WM_CHAR`, Backspace/Delete, Enter, and IME composition/commit;
- default builder limits and explicitly configured limits.

Prefer a boundary that flips behavior. For example, "one line works; two lines fail; deleting back to one line restores input" points to native multiline formatting or scrolling, not document-scale layout performance.

### 2. Inspect Native State Before Blaming Synchronization

For the hidden `EDIT`, query and record:

- `GetClientRect` and `GetWindowRect`;
- `GWL_STYLE` and `GWL_EXSTYLE`;
- `EM_GETLIMITTEXT`;
- `EM_GETSEL` before and after input;
- `WM_GETTEXTLENGTH` and `WM_GETTEXT` before and after input;
- `EM_GETLINECOUNT`, focus, enabled state, and read-only state when relevant.

Send one deterministic `WM_CHAR` and classify the result:

| Native text | Model text | Meaning |
| --- | --- | --- |
| unchanged | unchanged | Native control rejected input; inspect style, formatting rectangle, limits, focus, and message forwarding. |
| changed | unchanged | Native-to-model synchronization or patch validation failed. |
| changed | changed | Input succeeded; inspect layout invalidation, clipping, drawing, and scrolling. |
| unchanged | changed | Model bypasses native state; verify intentional ownership and resynchronization. |

If a local patch requires the native caret to advance, a rejected native character will make validation fail. A fallback full-text read then correctly returns the unchanged native text; changing patch validation would mask rather than fix the rejection.

### 3. Build a Raw Win32 Matrix

Use `scripts/probe_multiline_edit.ps1` to separate Windows behavior from framework behavior:

```powershell
powershell -ExecutionPolicy Bypass -File .agents/skills/win32-multiline-inputbox-debugging/scripts/probe_multiline_edit.ps1
```

Compare the raw result with the real widget. Reproduce all relevant details: font height, border and extended border, client dimensions, hard line count, and edit styles. A probe using one line or the default font is not representative of a failing multi-line proxy.

### 4. Test the Real Message Chain

After the raw control is understood, create a focused regression around the real widget rather than relying only on a Win32 sample:

1. Construct the actual multiline `InputBox`.
2. Assign text containing CRLF hard breaks.
3. Set the native and model caret to the same position.
4. Send `WM_CHAR` to the real subclassed native handle.
5. Flush the normal pending synchronization path.
6. Assert both native content and public/model content.

Use distinct failure codes or assertions for native and model failures. If needed, call the saved original window procedure once to distinguish native behavior from subclass interception.

### 5. Diagnose IME Only After the Character Path

`GCS_COMPSTR` may be drawn directly by the widget while committed text still depends on the native `EDIT`. Therefore:

- visible composition proves only that the overlay path works;
- a disappearing `GCS_RESULTSTR` can mean the underlying native insertion was rejected;
- verify native text and selection after commit before changing result-string ownership;
- avoid direct model insertion until duplicate insertion, undo history, selection replacement, maximum length, and native resynchronization are specified.

## Decision Logic

Use these branches in order:

1. **Behavior changes at a hard-line boundary:** inspect the formatting rectangle and vertical scrolling styles.
2. **Behavior changes at a character count:** query `EM_GETLIMITTEXT`; do not infer the limit from the builder argument.
3. **Native content changes but model does not:** inspect posted-message ordering, coalesced patches, expected caret calculation, CRLF indexing, and fallback reads.
4. **Both contents change but the screen does not:** inspect layout versioning, progressive rebuild boundaries, visible-line range, clipping, dirty flags, and scroll anchoring.
5. **Only IME fails:** retrieve result data at the correct message time, then inspect whether native default processing accepted it and whether clearing the overlay races with synchronization.

## Known Win32 Failure and Repair

A hidden multiline standard `EDIT` can reject all character input when hard lines exceed its formatting rectangle and it has no vertical auto-scroll capability. This remains true even when the caret is on the first line.

For a hidden proxy whose visible wrapping and scrolling are owned by `InputBox`, use:

```cpp
ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL
```

- `ES_AUTOHSCROLL` prevents the narrow hidden control from becoming the visible soft-wrap authority.
- `ES_AUTOVSCROLL` permits editing after hard lines exceed the formatting rectangle without displaying a native scrollbar.
- Give the native window a nonzero formatting rectangle large enough for its configured font and borders; keep it hidden and outside the visible UI.

Do not replace `ES_AUTOVSCROLL` with a visible scrollbar unless native scrollbar UI is intended. Do not solve the rejection by making the proxy document-sized; that can restore expensive native layout work.

This repair is appropriate only after the native text is proven unchanged. If native text already changes, continue with synchronization or rendering diagnosis instead.

## Long-Text Performance Checks

When correctness is restored, verify that the hidden control and self-drawn layout do not reintroduce latency:

- prevent native soft wrapping when the custom renderer owns wrapping;
- apply known character edits as local model patches rather than copying the full document;
- preserve stable visual-line prefixes and suffixes across local edits;
- bound progressive layout by elapsed time, not only line count;
- never expose progressive layout progress as repeated scroll-offset changes;
- keep full-text reads for commands whose replacement range is genuinely unknown.

Treat responsiveness and total layout completion separately. A pathological unbroken paragraph may require linear total work after a leading edit, but no single input frame should perform that entire job.

## Verification Standard

A fix is complete only when all applicable checks pass:

- English insertion works with one, two, and many hard lines.
- Beginning, middle, and end insertions update both native and model text exactly once.
- Enter, Backspace, Delete, selection replacement, clipboard commands, and undo remain correct.
- IME composition is visible and the committed candidate remains in the model.
- Deleting from multiple lines down to one line does not change whether input is accepted.
- Maximum-length behavior is verified through `EM_GETLIMITTEXT`, including any library-specific negative-value convention.
- Long-text input remains responsive and scrolling does not follow incremental layout construction.
- The focused regression exits successfully and the repository's complete build passes.

Document failed hypotheses and the evidence that ruled them out. In this class of bug, a precise negative result is often what prevents an attractive but incorrect IME or synchronization rewrite.
