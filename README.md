[![Page Views Count](https://badges.toozhao.com/badges/01JH01V78N1S0P0A9RVFJ5SWXZ/blue.svg)](https://badges.toozhao.com/stats/01JH01V78N1S0P0A9RVFJ5SWXZ "Get your own page views count badge on badges.toozhao.com")

FeEGELib V2.0.2.0

upd: 2025/04/24

基于 EGE V24.04 开发的一套轮子

EGE项目仓库：https://github.com/wysaid/xege

EGE官网：https://xege.org/

-----------------------------------------------------------------

主要封装了一个 Element 组件类

功能包括但不限于：

  + 组件图层之间的遮挡
  
  + 组件移动旋转等基本控制
  
  + 事件订阅及取消订阅
  
  + 异步加载图像

  + 物理引擎（待完善，目前的速度十分鬼畜）

  + 缓动动画
  
-----------------------------------------------------------------

使用方法：

1、装载EGE图形库 （参见 https://blog.csdn.net/qq_39151563/article/details/100161986 ）

快捷编译命令：
```
-std=c++14 -O2
```

快捷连接器命令：
```
-static -static-libgcc -lgraphics -lgdi32 -lgdiplus
```

2、将FeEGELib.h放在与你的代码同一级目录下

[![pFG1jrn.png](https://s11.ax1x.com/2024/02/15/pFG1jrn.png)](https://imgse.com/i/pFG1jrn)

3、测试：将以下代码复制进你的代码并编译运行：
```cpp
#include "FeEGELib.h" 
using namespace FeEGE;
int main(){
	init(1920 >> 1,1080 >> 1);
	pen::font(90,"幼圆");
	
	pen::print(0,0,"test pen");
	
	start();
	return 0;
}
```
运行结果应该如图所示：

[![pFG1vbq.png](https://s11.ax1x.com/2024/02/15/pFG1vbq.png)](https://imgse.com/i/pFG1vbq)

具体代码可查看FeEGELib.h中具体内容
