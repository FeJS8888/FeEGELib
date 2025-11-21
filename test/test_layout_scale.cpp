#include "FeEGELib.h"
using namespace FeEGE;

int main() {
    _setmode(_fileno(stdout), _O_WTEXT);
    init(800, 600);
    
    // Create a flex layout with spacing
    shared_ptr<Layout> layout = FlexLayoutBuilder()
        .setDirection(LayoutDirection::Row)
        .setPadding(20)
        .setSpacing(10)
        .setAlign(LayoutAlign::Start)
        .build();

    // Create three buttons
    Button* btn1 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮1")
        .setRadius(8)
        .build();
    
    Button* btn2 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮2")
        .setRadius(8)
        .build();
    
    Button* btn3 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮3")
        .setRadius(8)
        .build();

    // Create panel with layout, scale = 1.0
    Panel* panel = PanelBuilder()
        .setCenter(400, 200)
        .setSize(300, 100)
        .setRadius(10)
        .setLayout(layout)
        .addChild({btn1, btn2, btn3})
        .setBackground(EGERGB(230, 230, 250))
        .build();

    // Create panel with layout, scale = 1.5
    Panel* panelScaled = PanelBuilder()
        .setCenter(400, 400)
        .setSize(300, 100)
        .setRadius(10)
        .setLayout(layout)
        .setBackground(EGERGB(250, 230, 230))
        .build();

    Button* btn4 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮4")
        .setRadius(8)
        .build();
    
    Button* btn5 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮5")
        .setRadius(8)
        .build();
    
    Button* btn6 = ButtonBuilder()
        .setCenter(0, 0)
        .setSize(80, 40)
        .setContent(L"按钮6")
        .setRadius(8)
        .build();

    panelScaled->addChild(btn4, 0, 0);
    panelScaled->addChild(btn5, 0, 0);
    panelScaled->addChild(btn6, 0, 0);
    
    // Set scale AFTER adding children
    panelScaled->setScale(1.5);

    assignOrder({panel, panelScaled});

    start();
    
    // Cleanup
    delete panel;
    delete panelScaled;
    delete btn1;
    delete btn2;
    delete btn3;
    delete btn4;
    delete btn5;
    delete btn6;
    
    return 0;
}
