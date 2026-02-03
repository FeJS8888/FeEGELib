#include "FeEGELib.h"
using namespace FeEGE;

/**
 * 测试 InputBox 空格输入问题
 * 
 * 本测试专门用于测试和诊断 InputBox 对空格字符的处理
 * 
 * 问题分析：
 * - 之前的 sys_edit 创建时对所有 EDIT 控件都使用了 ES_WANTRETURN 标志
 * - ES_WANTRETURN 标志应该只用于多行编辑框，不应用于单行编辑框
 * - 在单行编辑框中使用该标志可能导致空格等字符输入异常
 * 
 * 修复方法：
 * - 将 ES_WANTRETURN 标志从默认样式中移除
 * - 只在 multiline=true 时添加 ES_WANTRETURN 标志
 */
int main() {
    SetProcessDPIAware();
    _setmode(_fileno(stdout), _O_WTEXT);
    init(800, 600);
    
    // 创建一个简单的输入框
    InputBox* testInput = InputBoxBuilder()
        .setCenter(400, 200)
        .setSize(500, 45)
        .setRadius(8)
        .setTextHeight(20)
        .setContent(L"请输入文本并尝试输入空格")
        .build();
    
    // 创建一个文本显示框，用于显示InputBox的内容
    Text* displayText = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(400, 300)
        .setContent(L"当前输入: (空)")
        .setFont(18, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();
    
    // 创建说明文字
    Text* instructions = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(400, 400)
        .setContent(L"请在输入框中输入文字，特别测试空格键的功能")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(127, 140, 141))
        .build();
    
    // 创建字符计数显示
    Text* charCount = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(400, 450)
        .setContent(L"字符数: 0")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(52, 152, 219))
        .build();
    
    assignOrder({testInput, displayText, instructions, charCount});
    
    // 主循环 - 实时显示输入框内容
    std::wstring lastContent;  // 缓存上一次的内容
    
    while (is_run()) {
        std::wstring content = testInput->getContent();
        
        // 更新显示文本，用特殊字符显示空格
        std::wstring displayContent = L"当前输入: [";
        for (wchar_t ch : content) {
            if (ch == L' ') {
                displayContent += L"·";  // 用中点显示空格
            } else {
                displayContent += ch;
            }
        }
        displayContent += L"]";
        displayText->setContent(displayContent);
        
        // 更新字符计数
        std::wstring countText = L"字符数: " + std::to_wstring(content.length());
        charCount->setContent(countText);
        
        // 输出到控制台进行调试（只在内容改变时输出）
        if (content != lastContent && testInput->getMCounter() > 0) {
            std::wcout << L"内容: [" << content << L"] 长度: " << content.length() << std::endl;
            for (size_t i = 0; i < content.length(); i++) {
                std::wcout << L"  [" << i << L"]: " << (int)content[i] << L" (0x" 
                          << std::hex << (int)content[i] << std::dec << L")" << std::endl;
            }
            lastContent = content;
        }
        
        delay_fps(60);
    }
    
    return 0;
}
