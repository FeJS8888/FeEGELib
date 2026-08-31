#include "FeEGELib.h"

using namespace FeEGE;

/**
 * Interactive regression test for the unified InputBox implementation.
 *
 * Checks, by interaction:
 *   - single-line and multi-line creation through the builder;
 *   - explicit and automatic line wrapping;
 *   - caret movement, selection and clipboard operations;
 *   - wheel scrolling and caret-follow scrolling inside a multi-line box;
 *   - InputBox nested in Panel and Box containers;
 *   - scaling and outer-container positioning.
 */
int main() {
    SetProcessDPIAware();
    _setmode(_fileno(stdout), _O_WTEXT);
    init(1200, 800);

    Text* title = TextBuilder()
        .setPosition(600, 42)
        .setAlign(TextAlign::Center)
        .setContent(L"InputBox multiline regression test")
        .setFont(28, L"Microsoft YaHei")
        .setColor(EGERGB(45, 55, 72))
        .build();

    Text* instructions = TextBuilder()
        .setPosition(600, 82)
        .setAlign(TextAlign::Center)
        .setContent(L"Click either editor. Try Enter, wheel scrolling, drag selection, Ctrl+A, paste, and arrow keys.")
        .setFont(15, L"Microsoft YaHei")
        .setColor(EGERGB(90, 100, 115))
        .build();

    const std::wstring sampleText =
        L"First explicit line.\n"
        L"Second line contains enough text to exercise automatic wrapping when the editor is narrower than this sentence.\n"
        L"Third line.\n"
        L"Fourth line contains mixed ASCII punctuation: [] {} () / \\ + - = * & % and numbers 0123456789.\n"
        L"Fifth line.\n"
        L"Sixth line.\n"
        L"Seventh line.\n"
        L"Eighth line.\n"
        L"Ninth line.\n"
        L"Tenth line.\n"
        L"The last line is intentionally long so the internal vertical scroll range is visible.";

    InputBox* mainEditor = InputBoxBuilder()
        .setIdentifier(L"multilineMainEditor")
        .setCenter(0, 0)
        .setSize(560, 250)
        .setRadius(10)
        .setTextHeight(19)
        .setMaxLength(4096)
        .setMultiline(true)
        .setContent(sampleText)
        .build();

    Panel* editorPanel = PanelBuilder()
        .setIdentifier(L"editorPanel")
        .setCenter(390, 365)
        .setSize(640, 335)
        .setRadius(14)
        .setBackground(EGERGB(238, 242, 247))
        .addChild(mainEditor, 0, 0)
        .build();

    Text* mainLabel = TextBuilder()
        .setPosition(390, 190)
        .setAlign(TextAlign::Center)
        .setContent(L"Multi-line InputBox in Panel")
        .setFont(19, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();

    InputBox* nestedEditor = InputBoxBuilder()
        .setIdentifier(L"multilineNestedEditor")
        .setCenter(0, 0)
        .setSize(245, 210)
        .setRadius(9)
        .setTextHeight(17)
        .setMultiline(true)
        .setContent(L"Nested editor.\n\nThis editor is inside a Box.\nResize the window or change the parent scale in a debugger to check offsets.")
        .build();

    Box* editorBox = new Box(900, 365, 285, 335);
    editorBox->addChild(nestedEditor, 0, 0);

    Text* nestedLabel = TextBuilder()
        .setPosition(900, 190)
        .setAlign(TextAlign::Center)
        .setContent(L"Multi-line InputBox in Box")
        .setFont(19, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();

    InputBox* singleLineEditor = InputBoxBuilder()
        .setIdentifier(L"singleLineCompatibilityEditor")
        .setCenter(600, 745)
        .setSize(420, 42)
        .setRadius(8)
        .setTextHeight(18)
        .setContent(L"Single-line compatibility editor")
        .build();

    Text* singleLabel = TextBuilder()
        .setPosition(330, 745)
        .setAlign(TextAlign::Center)
        .setContent(L"Single-line:")
        .setFont(17, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();

    assignOrder({
        title,
        instructions,
        mainLabel,
        editorPanel,
        nestedLabel,
        editorBox,
        singleLabel,
        singleLineEditor
    });

    start();
    return 0;
}
