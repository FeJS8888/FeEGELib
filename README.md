FeEGELib V1.1
upd: 2024/2/15

基于EGE v20.08开发的一套轮子

EGE项目仓库：https://github.com/wysaid/xege

EGE官网：https://xege.org/

主要封装了一个Element组件类
功能包括但不限于：
  ·组件图层之间的遮挡
  ·组件移动旋转等基本控制
  ·事件订阅及取消订阅
  ·异步加载图像
  ·物理引擎（待完善）

使用方法：

1、装载EGE图形库 （参见 https://blog.csdn.net/qq_39151563/article/details/100161986 ）



2、将FeEGELib.h放在与你的代码同一级目录下

[![pFG1jrn.png](https://s11.ax1x.com/2024/02/15/pFG1jrn.png)](https://imgse.com/i/pFG1jrn)

3、测试：将以下代码复制进你的代码并编译运行：
```cpp
#include "FeEGELib.h" 
using namespace std;
using namespace FeEGE;
int main(){
	initgraph(1920 >> 1,1080 >> 1);
	initXY();
	FeEGE::initpen();
	pen::font(90,"幼圆");
	
	pen::print(0,0,"test pen");
	
	start(120);
	return 0;
}
```
运行结果应该如图所示：

[![pFG1vbq.png](https://s11.ax1x.com/2024/02/15/pFG1vbq.png)](https://imgse.com/i/pFG1vbq)

具体代码可查看FeEGELib.h中具体内容
