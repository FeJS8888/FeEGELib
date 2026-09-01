#include "sys_edit.h"
#include "FeEGELib.h"

namespace FeEGE {

namespace {

constexpr LONG MAX_IME_COMPOSITION_BYTES = 16L * 1024L * 1024L;
constexpr UINT SYNC_NATIVE_EDIT_STATE_MESSAGE = WM_USER + 100 + 3;

bool isControlShortcutCharacter(WPARAM character) {
    return character == 1 || character == 3 || character == 22 ||
        character == 24 || character == 26;
}

std::wstring clipboardUnicodeText(HWND owner) {
    std::wstring text;
    if (!::OpenClipboard(owner)) return text;

    HANDLE handle = ::GetClipboardData(CF_UNICODETEXT);
    bool hasUnicodeText = handle != nullptr;
    if (handle) {
        const wchar_t* source = static_cast<const wchar_t*>(::GlobalLock(handle));
        SIZE_T bytes = ::GlobalSize(handle);
        if (source && bytes >= sizeof(wchar_t)) {
            const size_t maxChars = std::min(
                static_cast<size_t>(bytes / sizeof(wchar_t)),
                static_cast<size_t>(MAX_IME_COMPOSITION_BYTES / sizeof(wchar_t)));
            size_t length = 0;
            while (length < maxChars && source[length] != L'\0') ++length;
            text.assign(source, length);
        }
        if (source) ::GlobalUnlock(handle);
    }

    // EDIT accepts legacy ANSI clipboard data too. Preserve that behavior for
    // applications which provide CF_TEXT but not CF_UNICODETEXT.
    if (!hasUnicodeText) {
        handle = ::GetClipboardData(CF_TEXT);
        if (handle) {
            const char* source = static_cast<const char*>(::GlobalLock(handle));
            SIZE_T bytes = ::GlobalSize(handle);
            if (source && bytes > 0) {
                const size_t maxBytes = std::min(static_cast<size_t>(bytes),
                    static_cast<size_t>(MAX_IME_COMPOSITION_BYTES));
                size_t length = 0;
                while (length < maxBytes && source[length] != '\0') ++length;
                if (length > 0) {
                    const int required = ::MultiByteToWideChar(
                        CP_ACP, 0, source, static_cast<int>(length), nullptr, 0);
                    if (required > 0) {
                        std::vector<wchar_t> converted(static_cast<size_t>(required));
                        if (::MultiByteToWideChar(CP_ACP, 0, source,
                            static_cast<int>(length), converted.data(), required) > 0) {
                            text.assign(converted.begin(), converted.end());
                        }
                    }
                }
            }
            if (source) ::GlobalUnlock(handle);
        }
    }

    ::CloseClipboard();
    return text;
}

int imeCursorPosition(HWND hwnd) {
    if (!hwnd || !::IsWindow(hwnd)) return 0;
    HIMC hIMC = ::ImmGetContext(hwnd);
    if (!hIMC) return 0;

    LONG position = ::ImmGetCompositionStringW(hIMC, GCS_CURSORPOS, nullptr, 0);
    ::ImmReleaseContext(hwnd, hIMC);
    return position >= 0 ? static_cast<int>(position) : 0;
}

} // namespace

void PrintStringAsInts(const std::wstring& str) {
    if (str.empty()) {
        std::wcout << "(empty)" << std::endl;
        return;
    }
    for (wchar_t character : str) {
        std::wcout << character << " ";
    }
    std::wcout << std::endl;
}

int GetTextAscent(HWND hwnd) {
    if (!hwnd) return 0;
    HDC dc = ::GetDC(hwnd);
    if (!dc) return 0;

    TEXTMETRIC metrics = {};
    const BOOL measured = ::GetTextMetrics(dc, &metrics);
    ::ReleaseDC(hwnd, dc);
    return measured ? metrics.tmAscent : 0;
}

void sys_edit::queueNativeState(const std::wstring& text, int selectionBegin,
    int selectionEnd) {
    if (m_destroying.load(std::memory_order_acquire)) return;

    {
        std::lock_guard<std::mutex> lock(m_pendingNativeStateMutex);
        m_pendingNativeState.text = text;
        m_pendingNativeState.selectionBegin = selectionBegin;
        m_pendingNativeState.selectionEnd = selectionEnd;
        m_hasPendingNativeState = true;
    }

    // Post rather than SendMessage: the UI thread is the only thread that
    // decides whether it is safe to change the hidden EDIT while IME is
    // active.  Repeated requests intentionally coalesce to the latest model.
    HWND hwnd = m_hwnd;
    if (hwnd && ::IsWindow(hwnd)) {
        ::PostMessageW(hwnd, SYNC_NATIVE_EDIT_STATE_MESSAGE, 0, 0);
    }
}

void sys_edit::applyPendingNativeState() {
    // This method is called exclusively from the EDIT's UI-thread window
    // procedure.  Keeping the check and the native calls on that same thread
    // removes the "checked idle, IME started immediately afterwards" race.
    if (m_destroying.load(std::memory_order_acquire) || m_imeCancelling ||
        m_imeComposing.load(std::memory_order_acquire)) {
        return;
    }

    HWND hwnd = m_hwnd;
    LONG_PTR callback = m_callback;
    if (!hwnd || !::IsWindow(hwnd) || !callback) return;

    PendingNativeState state;
    {
        std::lock_guard<std::mutex> lock(m_pendingNativeStateMutex);
        if (!m_hasPendingNativeState) return;
        state = std::move(m_pendingNativeState);
        m_pendingNativeState = PendingNativeState{};
        m_hasPendingNativeState = false;
    }

    // Call the original EDIT procedure directly.  Sending these messages back
    // through our subclass would only re-enter onMessage() and reintroduce a
    // cross-thread synchronisation point.
    WNDPROC nativeProc = reinterpret_cast<WNDPROC>(callback);
    nativeProc(hwnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(state.text.c_str()));
    nativeProc(hwnd, EM_SETSEL, state.selectionBegin, state.selectionEnd);
}

void sys_edit::killIME() {
    HWND hwnd = m_hwnd;
    if (!hwnd || !::IsWindow(hwnd) || m_imeCancelling ||
        !m_imeComposing.load(std::memory_order_acquire)) return;

    // CPS_CANCEL may synchronously dispatch nested IME messages. Mark the
    // transition first so those messages cannot enqueue a duplicate result.
    m_imeCancelling = true;
    std::wstring composition;

    HIMC hIMC = ::ImmGetContext(hwnd);
    if (hIMC) {
        LONG byteSize = ::ImmGetCompositionStringW(hIMC, GCS_COMPSTR, nullptr, 0);
        if (byteSize > 0 && byteSize <= MAX_IME_COMPOSITION_BYTES &&
            byteSize % static_cast<LONG>(sizeof(wchar_t)) == 0) {
            const size_t charCount = static_cast<size_t>(byteSize) / sizeof(wchar_t);
            std::vector<wchar_t> buffer(charCount + 1, L'\0');
            LONG copiedBytes = ::ImmGetCompositionStringW(
                hIMC, GCS_COMPSTR, buffer.data(), byteSize);
            if (copiedBytes > 0) {
                const size_t copiedChars = std::min(
                    charCount, static_cast<size_t>(copiedBytes) / sizeof(wchar_t));
                composition.assign(buffer.data(), copiedChars);
            }
        }

        // The native EDIT owns only the transient backend composition.  Cancel
        // it before the application mirrors its authoritative model back,
        // avoiding an EDIT-side insertion at a stale selection position.
        ::ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
        ::ImmReleaseContext(hwnd, hIMC);
    }

    m_imeComposing.store(false, std::memory_order_release);
    m_imeCancelling = false;

    PendingInputEvent event;
    event.type = PendingInputEvent::Type::ImeCommit;
    event.text = std::move(composition);
    enqueueInputEvent(std::move(event));
}

LRESULT sys_edit::onMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    auto forwardToNative = [this](UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
        HWND hwnd = m_hwnd;
        if (!hwnd || !::IsWindow(hwnd)) return 0;
        LONG_PTR callback = m_callback;
        if (!callback) return ::DefWindowProcW(hwnd, msg, wp, lp);
        return reinterpret_cast<WNDPROC>(callback)(hwnd, msg, wp, lp);
    };

    auto enqueueKey = [this](unsigned int key, bool shift, bool ctrl) {
        PendingInputEvent event;
        event.type = PendingInputEvent::Type::KeyDown;
        event.key = key;
        event.shift = shift;
        event.ctrl = ctrl;
        enqueueInputEvent(std::move(event));
    };

    switch (message) {
    case WM_CTLCOLOREDIT: {
        HDC dc = reinterpret_cast<HDC>(wParam);
        if (!dc) return 0;
        HBRUSH brush = ::CreateSolidBrush(ARGBTOZBGR(m_bgcolor));
        if (!brush) return 0;

        ::SetBkColor(dc, ARGBTOZBGR(m_bgcolor));
        ::SetTextColor(dc, ARGBTOZBGR(m_color));
        if (m_hBrush) ::DeleteObject(m_hBrush);
        m_hBrush = brush;
        return reinterpret_cast<LRESULT>(brush);
    }

    case WM_SETFOCUS:
        m_bInputFocus = 1;
        m_focus.store(true, std::memory_order_release);
        return forwardToNative(message, wParam, lParam);

    case WM_KILLFOCUS: {
        m_bInputFocus = 0;
        m_focus.store(false, std::memory_order_release);
        m_suppressedBackspaceChars = 0;
        m_suppressedReturnChars = 0;
        killIME();

        PendingInputEvent event;
        event.type = PendingInputEvent::Type::FocusLost;
        enqueueInputEvent(std::move(event));
        return forwardToNative(message, wParam, lParam);
    }

    case WM_IME_STARTCOMPOSITION: {
        // The hidden EDIT must still see the native IME lifecycle.  It is not
        // the source of truth for InputBox text, but bypassing its WndProc
        // here makes some IMEs lose/corrupt a multi-character result string.
        // The application only consumes the queued snapshot below.
        if (m_imeCancelling) return forwardToNative(message, wParam, lParam);
        // Apply any model update queued before this transaction starts.  This
        // gives the native IME backend the newest selection without allowing
        // an application-thread WM_SETTEXT to race this start message.
        applyPendingNativeState();
        m_imeComposing.store(true, std::memory_order_release);

        PendingInputEvent event;
        event.type = PendingInputEvent::Type::ImeStart;
        enqueueInputEvent(std::move(event));

        // The application thread updates the candidate position while drawing.
        // Do not read InputPositionX/Y here: they are application-thread state
        // and reading them from the UI thread would reintroduce a data race.
        return forwardToNative(message, wParam, lParam);
    }

    case WM_IME_COMPOSITION: {
        if (m_imeCancelling) return forwardToNative(message, wParam, lParam);
        // A result message does not mean that the native transaction has
        // ended.  In particular, a multi-character Chinese result can still
        // be followed by WM_IME_ENDCOMPOSITION and IME-generated WM_CHAR
        // messages.  Keep m_imeComposing true until that end message so the
        // application will not reset the hidden EDIT in the middle of it.
        if (!m_imeComposing.load(std::memory_order_acquire)) {
            return forwardToNative(message, wParam, lParam);
        }

        if (lParam & GCS_RESULTSTR) {
            PendingInputEvent event;
            event.type = PendingInputEvent::Type::ImeResult;
            event.text = GetImeCompositionString(m_hwnd, GCS_RESULTSTR);
            enqueueInputEvent(std::move(event));
            // Do not clear m_imeComposing here.  WM_IME_ENDCOMPOSITION is
            // the only reliable boundary at which it is safe for the
            // application thread to mirror content back into the native EDIT.
            return forwardToNative(message, wParam, lParam);
        }

        if (lParam & (GCS_COMPSTR | GCS_CURSORPOS)) {
            PendingInputEvent event;
            event.type = PendingInputEvent::Type::ImeUpdate;
            event.text = GetImeCompositionString(m_hwnd, GCS_COMPSTR);
            event.imeCursorPos = imeCursorPosition(m_hwnd);
            enqueueInputEvent(std::move(event));
        }
        return forwardToNative(message, wParam, lParam);
    }

    case WM_IME_ENDCOMPOSITION: {
        // Forward first, while m_imeComposing is still true, so any nested
        // WM_CHAR generated by the native EDIT cannot be mistaken for normal
        // text input and duplicate a confirmed IME result.
        const LRESULT nativeResult = forwardToNative(message, wParam, lParam);
        if (!m_imeCancelling &&
            m_imeComposing.exchange(false, std::memory_order_acq_rel)) {
            PendingInputEvent event;
            event.type = PendingInputEvent::Type::ImeEnd;
            enqueueInputEvent(std::move(event));
        }
        // An application update may have been deferred while the transaction
        // was active.  It is safe to apply it now, on this UI thread.
        applyPendingNativeState();
        return nativeResult;
    }

    case WM_KEYDOWN: {
        // While an IME owns a composition, it must still receive navigation
        // keys for candidate selection. Any EDIT-side selection change is
        // overwritten by the application thread when the composition ends.
        if (m_imeComposing || m_imeCancelling) {
            return forwardToNative(message, wParam, lParam);
        }

        const bool shift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
        const bool ctrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
        const unsigned int key = static_cast<unsigned int>(wParam);

        if (ctrl) {
            PendingInputEvent event;
            switch (key) {
            case 'A':
                enqueueKey(key, shift, true);
                return 0;
            case 'C':
                event.type = PendingInputEvent::Type::Copy;
                enqueueInputEvent(std::move(event));
                return 0;
            case 'X':
                event.type = PendingInputEvent::Type::Cut;
                enqueueInputEvent(std::move(event));
                return 0;
            case 'V':
                event.type = PendingInputEvent::Type::TextInput;
                event.text = clipboardUnicodeText(m_hwnd);
                enqueueInputEvent(std::move(event));
                return 0;
            case 'Z':
                // The native EDIT cannot be allowed to mutate its private
                // undo buffer, because InputBox is the authoritative model.
                return 0;
            default:
                break;
            }
        }

        switch (key) {
        case VK_BACK:
            ++m_suppressedBackspaceChars;
            enqueueKey(key, shift, ctrl);
            return 0;
        case VK_DELETE:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_HOME:
        case VK_END:
        case VK_UP:
        case VK_DOWN:
        case VK_PRIOR:
        case VK_NEXT:
            enqueueKey(key, shift, ctrl);
            return 0;
        case VK_RETURN:
            if (m_multiline) {
                ++m_suppressedReturnChars;
                PendingInputEvent event;
                event.type = PendingInputEvent::Type::TextInput;
                event.text = L"\r\n";
                enqueueInputEvent(std::move(event));
            }
            return 0;
        default:
            return forwardToNative(message, wParam, lParam);
        }
    }

    case WM_KEYUP:
        // A translated WM_CHAR normally consumes these counters before key-up.
        // Reset them here as well so an interrupted key sequence cannot make
        // a later Backspace/Enter disappear.
        if (wParam == VK_BACK) m_suppressedBackspaceChars = 0;
        if (wParam == VK_RETURN) m_suppressedReturnChars = 0;
        return forwardToNative(message, wParam, lParam);

    case WM_CHAR: {
        // Let the hidden native EDIT consume characters generated as part of
        // its own IME transaction.  Some IMEs emit one WM_CHAR per UTF-16
        // unit while finalising a multi-character result; swallowing those
        // messages prevents the EDIT from completing its internal state.
        // They are deliberately not queued as TextInput: InputBox receives
        // the authoritative, whole GCS_RESULTSTR snapshot instead, and the
        // native text is mirrored back only after WM_IME_ENDCOMPOSITION.
        if (m_imeComposing || m_imeCancelling) {
            return forwardToNative(message, wParam, lParam);
        }
        if (isControlShortcutCharacter(wParam) &&
            ((::GetKeyState(VK_CONTROL) & 0x8000) != 0)) {
            return 0;
        }
        if (wParam == VK_BACK) {
            if (m_suppressedBackspaceChars > 0) {
                --m_suppressedBackspaceChars;
                return 0;
            }
            enqueueKey(VK_BACK, false, false);
            return 0;
        }
        if (wParam == L'\r') {
            if (m_suppressedReturnChars > 0) {
                --m_suppressedReturnChars;
                return 0;
            }
            if (m_multiline) {
                PendingInputEvent event;
                event.type = PendingInputEvent::Type::TextInput;
                event.text = L"\r\n";
                enqueueInputEvent(std::move(event));
            }
            return 0;
        }
        if (wParam < 0x20) return 0;

        PendingInputEvent event;
        event.type = PendingInputEvent::Type::TextInput;
        event.text.assign(1, static_cast<wchar_t>(wParam));
        enqueueInputEvent(std::move(event));
        return 0;
    }

    case WM_PASTE: {
        if (m_imeComposing || m_imeCancelling) return 0;
        PendingInputEvent event;
        event.type = PendingInputEvent::Type::TextInput;
        event.text = clipboardUnicodeText(m_hwnd);
        enqueueInputEvent(std::move(event));
        return 0;
    }

    case WM_COPY: {
        if (m_imeComposing || m_imeCancelling) return 0;
        PendingInputEvent event;
        event.type = PendingInputEvent::Type::Copy;
        enqueueInputEvent(std::move(event));
        return 0;
    }

    case WM_CUT: {
        if (m_imeComposing || m_imeCancelling) return 0;
        PendingInputEvent event;
        event.type = PendingInputEvent::Type::Cut;
        enqueueInputEvent(std::move(event));
        return 0;
    }

    case WM_CLEAR: {
        if (m_imeComposing || m_imeCancelling) return 0;
        PendingInputEvent event;
        event.type = PendingInputEvent::Type::DeleteSelection;
        enqueueInputEvent(std::move(event));
        return 0;
    }

    case WM_UNDO:
        return 0;

    case WM_SETTEXT:
    case EM_SETSEL:
        // Direct callers may still use these messages.  They must never
        // create a feedback event back into InputBox; model snapshots use
        // applyPendingNativeState() to call the original procedure directly.
        return forwardToNative(message, wParam, lParam);

    case WM_USER + 100 + 1:
        // Kept as a harmless sink for delayed messages posted by older builds.
        return TRUE;

    case WM_USER + 100 + 2:
        killIME();
        return TRUE;

    case SYNC_NATIVE_EDIT_STATE_MESSAGE:
        applyPendingNativeState();
        return TRUE;

    default:
        return forwardToNative(message, wParam, lParam);
    }
}

void sys_edit::updatecursor() {
    // Cursor ownership moved to InputBox's application-thread event consumer.
    // Retain this method for source compatibility with existing callers.
}

} // namespace FeEGE
