/**
 * @file test_pagination.cpp
 * @brief 翻页效果演示
 *
 * 演示了：
 * 1. 利用 assignOrder 实现多页切换效果
 * 2. 底部导航栏（上一页/下一页按钮 + 页码指示）始终可见
 * 3. 每页展示不同类型的 Widget：
 *    - 第 1 页：欢迎页面（Text + Button）
 *    - 第 2 页：表单控件（InputBox + Slider）
 *    - 第 3 页：开关控件（Toggle + Radio）
 *    - 第 4 页：进度控件（ProgressBar + Knob + Dropdown + 滚动列表）
 */

#include "FeEGELib.h"
using namespace FeEGE;

// ──────────────────────────────────────────────
// 全局分页状态
// ──────────────────────────────────────────────
static const int PAGE_COUNT = 4;
static int currentPage = 0;

// 每页的顶层面板
static Panel* pages[PAGE_COUNT];

// 导航控件（始终可见）
static Button* prevBtn  = nullptr;
static Button* nextBtn  = nullptr;
static Text*   pageText = nullptr;

// ──────────────────────────────────────────────
// 辅助：更新页码文字并调用 assignOrder 切换页面
// ──────────────────────────────────────────────
static void switchPage(int idx) {
    currentPage = idx;

    wchar_t buf[32];
    swprintf(buf, 32, L"第 %d / %d 页", currentPage + 1, PAGE_COUNT);
    pageText->setContent(buf);

    // 前后翻页按钮的可用状态通过颜色暗示
    if(currentPage > 0){
        prevBtn->setColor(EGERGB(102, 126, 234));
        prevBtn->enable();
    }
    else{
        prevBtn->setColor(EGERGB(200, 200, 200));
        prevBtn->disable();
    }
    if(currentPage < PAGE_COUNT - 1){
        nextBtn->setColor(EGERGB(102, 126, 234));
        nextBtn->enable();
    }
    else{
        nextBtn->setColor(EGERGB(200, 200, 200));
        nextBtn->disable();
    }
    
    wcout<<pages[currentPage]->getChildren().size()<<endl;
    assignOrder({pages[currentPage], prevBtn, pageText, nextBtn});
}

// ──────────────────────────────────────────────
// 第 1 页：欢迎页面
// ──────────────────────────────────────────────
static Panel* buildPage1() {
    Text* title = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(400, 180)
        .setContent(L"欢迎使用 FeEGELib")
        .setFont(36, L"Microsoft YaHei")
        .setColor(EGERGB(102, 126, 234))
        .build();

    Text* desc = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(400, 250)
        .setMaxWidth(460)
        .setContent(L"这是一个基于 EGE 的现代化 UI 控件库。\n点击右下角的\"下一页\"体验各种控件。")
        .setFont(18, L"Microsoft YaHei")
        .setColor(EGERGB(80, 80, 100))
        .build();

    Button* startBtn = ButtonBuilder()
        .setCenter(400, 360)
        .setSize(180, 52)
        .setRadius(26)
        .setContent(L"开始体验 →")
        .setColor(EGERGB(102, 126, 234))
        .setOnClick([]() { switchPage(1); })
        .build();

    Text* version = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(400, 430)
        .setContent(L"FeEGELib  " FeEGELib_version)
        .setFont(13, L"Consolas")
        .setColor(EGERGB(180, 180, 180))
        .build();

    // 用透明 Box 把内容垂直居中排列
    Panel* panel = PanelBuilder()
        .setCenter(400, 270)
        .setSize(800, 520)
        .setRadius(0)
        .setBackground(EGERGB(248, 249, 255))
        .addChild(title,   0, 0)
        .addChild(desc,    0, 0)
        .addChild(startBtn,0, 0)
        .addChild(version, 0, 0)
        .setLayout(
            FlexLayoutBuilder()
                .setDirection(LayoutDirection::Column)
                .setAlign(LayoutAlign::Center)
                .setSpacing(20)
                .build()
        )
        .build();

    return panel;
}

// ──────────────────────────────────────────────
// 第 2 页：表单控件（InputBox + Slider）
// ──────────────────────────────────────────────
static Panel* buildPage2() {
    Text* header = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(0, 0)
        .setContent(L"📝  表单控件")
        .setFont(26, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();

    Text* lbUser = TextBuilder()
        .setAlign(TextAlign::Left)
        .setPosition(0, 0)
        .setContent(L"用户名")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(90, 90, 120))
        .build();

    InputBox* username = InputBoxBuilder()
        .setSize(300, 42)
        .setRadius(8)
        .setTextHeight(18)
        .build();

    Text* lbPass = TextBuilder()
        .setAlign(TextAlign::Left)
        .setPosition(0, 0)
        .setContent(L"密码")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(90, 90, 120))
        .build();

    InputBox* password = InputBoxBuilder()
        .setSize(300, 42)
        .setRadius(8)
        .setTextHeight(18)
        .build();

    Text* lbSlider = TextBuilder()
        .setAlign(TextAlign::Left)
        .setPosition(0, 0)
        .setContent(L"音量")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(90, 90, 120))
        .build();

    Text* sliderVal = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(0, 0)
        .setContent(L"50%")
        .setFont(15, L"Microsoft YaHei")
        .setColor(EGERGB(102, 126, 234))
        .build();

    Slider* volSlider = SliderBuilder()
        .setSize(300, 32)
        .setProgress(0.5)
        .setOnChange([sliderVal](double v) {
            wchar_t buf[16];
            swprintf(buf, 16, L"%.0f%%", v * 100);
            sliderVal->setContent(buf);
        })
        .build();

    Button* submitBtn = ButtonBuilder()
        .setSize(160, 44)
        .setRadius(22)
        .setContent(L"提 交")
        .setColor(EGERGB(39, 174, 96))
        .setOnClick([]() {
            std::wcout << L"[Page2] 提交表单" << std::endl;
        })
        .build();

    auto layout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .setSpacing(14)
        .setPadding(30)
        .build();

    Panel* panel = PanelBuilder()
        .setCenter(400, 270)
        .setSize(420, 480)
        .setRadius(18)
        .setBackground(EGERGB(255, 255, 255))
        .setLayout(layout)
        .addChild({header, lbUser, username, lbPass, password,
                   lbSlider, volSlider, sliderVal, submitBtn})
        .build();

    // 外层浅色背景
    Panel* bg = PanelBuilder()
        .setCenter(400, 270)
        .setSize(800, 520)
        .setRadius(0)
        .setBackground(EGERGB(240, 244, 255))
        .addChild(panel, 0, 0)
        .build();

    return bg;
}

// ──────────────────────────────────────────────
// 第 3 页：开关控件（Toggle + Radio）
// ──────────────────────────────────────────────
static Panel* buildPage3() {
    Text* header = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(0, 0)
        .setContent(L"🔘  开关控件")
        .setFont(26, L"Microsoft YaHei")
        .setColor(EGERGB(52, 73, 94))
        .build();

    // ── Toggle 演示 ──
    Text* lbToggles = TextBuilder()
        .setAlign(TextAlign::Left)
        .setPosition(0, 0)
        .setContent(L"开关（Toggle）")
        .setFont(17, L"Microsoft YaHei")
        .setColor(EGERGB(70, 70, 100))
        .build();

    Toggle* tog1 = ToggleBuilder()
        .setSize(56, 28)
        .setChecked(true)
        .setBaseColor(EGERGB(102, 126, 234))
        .setOnToggle([](bool v) {
            std::wcout << L"[Page3] Toggle1 = " << v << std::endl;
        })
        .build();

    Toggle* tog2 = ToggleBuilder()
        .setSize(56, 28)
        .setChecked(false)
        .setBaseColor(EGERGB(231, 76, 60))
        .setOnToggle([](bool v) {
            std::wcout << L"[Page3] Toggle2 = " << v << std::endl;
        })
        .build();

    Toggle* tog3 = ToggleBuilder()
        .setSize(56, 28)
        .setChecked(false)
        .setBaseColor(EGERGB(39, 174, 96))
        .setOnToggle([](bool v) {
            std::wcout << L"[Page3] Toggle3 = " << v << std::endl;
        })
        .build();

    auto toggleRow = BoxBuilder()
        .setSize(300, 40)
        .setDirection(LayoutDirection::Row)
        .setAlign(LayoutAlign::Center)
        .setSpacing(20)
        .addChild({tog1, tog2, tog3})
        .build();

    // ── Radio 演示 ──
    Text* lbRadio = TextBuilder()
        .setAlign(TextAlign::Left)
        .setPosition(0, 0)
        .setContent(L"单选（Radio）")
        .setFont(17, L"Microsoft YaHei")
        .setColor(EGERGB(70, 70, 100))
        .build();

    Text* radioResult = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(0, 0)
        .setContent(L"当前选择：选项 A")
        .setFont(15, L"Microsoft YaHei")
        .setColor(EGERGB(102, 126, 234))
        .build();

    RadioController* rc = RadioControllerBuilder()
        .setRadius(13)
        .setGap(60)
        .setStyle(RadioStyle::Filled)
        .add(L"A")
        .add(L"B")
        .add(L"C")
        .setDefault(L"A")
        .setOnChange([radioResult](const std::wstring& val) {
            radioResult->setContent(L"当前选择：选项 " + val);
        })
        .build();

    // RadioController 直接 build，内部自行注册，无需加入 Panel children
    // 但我们仍需把 rc 包裹为可见占位（RadioController 本身不是 Widget），
    // 故用一个空 Box 占位，后文说明见注释。

    auto layout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .setSpacing(18)
        .setPadding(30)
        .build();

    Panel* card = PanelBuilder()
        .setCenter(400, 260)
        .setSize(400, 430)
        .setRadius(18)
        .setBackground(EGERGB(255, 255, 255))
        .setLayout(layout)
        .addChild({header, lbToggles, toggleRow, lbRadio, radioResult})
        .build();

    Panel* bg = PanelBuilder()
        .setCenter(400, 270)
        .setSize(800, 520)
        .setRadius(0)
        .setBackground(EGERGB(245, 248, 255))
        .addChild(card, 0, 0)
        .build();

    return bg;
}

// ──────────────────────────────────────────────
// 第 4 页：进度控件（ProgressBar + Knob + Dropdown + 滚动列表）
// ──────────────────────────────────────────────
static Panel* buildPage4() {
    // ── 左侧卡片：ProgressBar + Knob ──
    Text* lbProgress = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(0, 0)
        .setContent(L"进度条")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(70, 70, 100))
        .build();

    ProgressBar* pb = ProgressBarBuilder()
        .setSize(200, 18)
        .setProgress(0.65)
        .setColor(EGERGB(102, 126, 234))
        .build();

    Slider* pbCtrl = SliderBuilder()
        .setSize(200, 28)
        .setProgress(0.65)
        .setOnChange([pb](double v) { pb->setProgress(v); })
        .build();

    Text* lbKnob = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(0, 0)
        .setContent(L"旋钮")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(70, 70, 100))
        .build();

    Knob* knob = KnobBuilder()
        .setRadius(48)
        .setRange(0, 100)
        .setValue(50)
        .setColor(EGERGB(102, 126, 234), EGERGB(220, 220, 220))
        .setShowValue(true)
        .build();

    auto leftLayout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .setSpacing(12)
        .setPadding(20)
        .build();

    Panel* leftCard = PanelBuilder()
        .setCenter(0, 0)
        .setSize(280, 380)
        .setRadius(14)
        .setBackground(EGERGB(255, 255, 255))
        .setLayout(leftLayout)
        .addChild({lbProgress, pb, pbCtrl, lbKnob, knob})
        .build();

    // ── 右侧卡片：Dropdown + 滚动列表 ──
    Text* lbDrop = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(0, 0)
        .setContent(L"下拉菜单")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(70, 70, 100))
        .build();

    Dropdown* dd = DropdownBuilder()
        .setSize(180, 38)
        .setRadius(8)
        .setContent(L"请选择")
        .addOption(L"苹果 🍎", []() { std::wcout << L"苹果" << std::endl; })
        .addOption(L"香蕉 🍌", []() { std::wcout << L"香蕉" << std::endl; })
        .addOption(L"草莓 🍓", []() { std::wcout << L"草莓" << std::endl; })
        .addOption(L"葡萄 🍇", []() { std::wcout << L"葡萄" << std::endl; })
        .build();

    Text* lbList = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(0, 0)
        .setContent(L"滚动列表")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(70, 70, 100))
        .build();

    std::vector<Widget*> listItems;
    for (int i = 1; i <= 8; ++i) {
        wchar_t buf[32];
        swprintf(buf, 32, L"列表项 %d", i);
        Button* item = ButtonBuilder()
            .setSize(180, 36)
            .setRadius(6)
            .setContent(buf)
            .setColor(EGERGB(235 - i * 5, 240, 250))
            .build();
        listItems.push_back(item);
    }

    auto listLayout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Start)
        .setSpacing(6)
        .setPadding(8)
        .build();

    Panel* listPanel = PanelBuilder()
        .setSize(200, 180)
        .setRadius(8)
        .setBackground(EGERGB(246, 248, 252))
        .setLayout(listLayout)
        .addChild(listItems)
        .setScrollBar(true, 12)
        .build();

    auto rightLayout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setAlign(LayoutAlign::Center)
        .setSpacing(12)
        .setPadding(20)
        .build();

    Panel* rightCard = PanelBuilder()
        .setCenter(0, 0)
        .setSize(280, 380)
        .setRadius(14)
        .setBackground(EGERGB(255, 255, 255))
        .setLayout(rightLayout)
        .addChild({lbDrop, dd, lbList, listPanel})
        .build();

    // 两列横向排布
    auto rowLayout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Row)
        .setAlign(LayoutAlign::Center)
        .setSpacing(24)
        .setPadding(20)
        .build();

    Panel* bg = PanelBuilder()
        .setCenter(400, 270)
        .setSize(800, 520)
        .setRadius(0)
        .setBackground(EGERGB(242, 245, 255))
        .setLayout(rowLayout)
        .addChild({leftCard, rightCard})
        .build();

    return bg;
}

// ──────────────────────────────────────────────
// main
// ──────────────────────────────────────────────
int main() {
    SetProcessDPIAware();
    _setmode(_fileno(stdout), _O_WTEXT);
    init(800, 600);

    // 构建四个页面
    pages[0] = buildPage1();
    pages[1] = buildPage2();
    pages[2] = buildPage3();
    pages[3] = buildPage4();

    // ── 底部导航栏 ──
    // 上一页按钮
    prevBtn = ButtonBuilder()
        .setCenter(90, 560)
        .setSize(120, 40)
        .setRadius(20)
        .setContent(L"← 上一页")
        .setColor(EGERGB(200, 200, 200))   // 初始在第1页，不可用
        .setOnClick([]() {
            if (currentPage > 0)
                switchPage(currentPage - 1);
        })
        .build();

    // 下一页按钮
    nextBtn = ButtonBuilder()
        .setCenter(710, 560)
        .setSize(120, 40)
        .setRadius(20)
        .setContent(L"下一页 →")
        .setColor(EGERGB(102, 126, 234))
        .setOnClick([]() {
            if (currentPage < PAGE_COUNT - 1)
                switchPage(currentPage + 1);
        })
        .build();

    // 页码文字
    pageText = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(400, 562)
        .setContent(L"第 1 / 4 页")
        .setFont(16, L"Microsoft YaHei")
        .setColor(EGERGB(120, 120, 150))
        .build();

    // 第一次显示第 0 页
    switchPage(0);

    start();
    return 0;
}