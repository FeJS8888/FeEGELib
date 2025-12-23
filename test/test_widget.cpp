#include "FeEGELib.h"
using namespace FeEGE;

int main() {
    SetProcessDPIAware();
    _setmode(_fileno(stdout), _O_WTEXT);
    init(800, 600);
    
    shared_ptr<Layout> layout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Column)
        .setPadding(30)
        .setSpacing(30)
        .setAlign(LayoutAlign::Start)
        .build();

    // 标题文字
    Text* title = TextBuilder()
        .setAlign(TextAlign::Center)
        .setPosition(250, 150)
        .setContent(L"用户登录")
        .setFont(32, L"Microsoft YaHei")
        .setColor(EGERGB(102, 126, 234))
        .build();
    
    // 用户名输入框
    InputBox* username = InputBoxBuilder()
        .setCenter(400, 250)
        .setSize(300, 45)
        .setRadius(8)
        .setTextHeight(20)
        .build();
    
    // 密码输入框
    InputBox* password = InputBoxBuilder()
        .setCenter(400, 320)
        .setSize(300, 45)
        .setRadius(8)
        .setTextHeight(20)
        .build();
    
    // 登录按钮
    Button* loginBtn = ButtonBuilder()
        .setCenter(400, 400)
        .setSize(200, 50)
        .setRadius(25)
        .setContent(L"登录")
        .setColor(EGERGB(102, 126, 234))
        .setOnClick([]() {
            // 处理登录逻辑
            std::wcout << L"点击了" << std::endl;
        })
        .build();

    // 创建主面板
    Panel* loginPanel = PanelBuilder()
        .setCenter(400, 300)
        .setSize(400, 350)
        .setRadius(15)
        .setLayout(layout)
        .addChild({title,username,password,loginBtn})
        .setBackground(EGERGB(250, 250, 250))
        .build();

    Slider* ss = SliderBuilder()
        .setSize(400,35)
        .setCenter(400,425)
        .setOnChange([=](double f){
            loginPanel->setScale(f * 2);
        })
        .setProgress(0.50f)
        .build();

    assignOrder({ss,loginPanel});

    start();
    
    return 0;
}
