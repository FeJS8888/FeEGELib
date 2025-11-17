#include "FeEGELib.h"
using namespace FeEGE;

// 测试Widget组件
int main() {
    init(800, 600);
    
    // 创建一个简单的测试
    pen::font(20, "Arial");
    pen::print(10, 10, "Widget Test");
    
    start();
    return 0;
}
