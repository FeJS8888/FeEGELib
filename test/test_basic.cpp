#include "FeEGELib.h"
using namespace FeEGE;

int main() {
    init(1920 >> 1, 1080 >> 1);       // 初始化窗口
    pen::font(90, "幼圆");            // 设置字体
    pen::print(0, 0, "test pen");     // 输出文字
    start();                          // 启动事件循环
    return 0;
}
