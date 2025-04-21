/**
 * @file FeEGELib.h
 * @brief FeEGE图形库主头文件
 * @version 2.0.0.0
 * @date 2025-04-18
 * 
 * 这是一个基于 EGE 的图形开发框架，提供了元素管理、动画、事件处理等功能
 */

#ifndef _FEEGELIB_
#define _FEEGELIB_

#define FeEGELib_version "V2.0.0.0--upd2025-04-18"  ///< 库版本号
#define version() FeEGELib_version                   ///< 获取版本号的宏

// 包含必要的头文件
#include<graphics.h>
#include<vector>
#include<thread>
#include<set>
#include<map>
#include<unordered_map>
#include<string>
#include<windows.h>
#include<algorithm>
#include<iostream>
#include<fstream>
#include<queue>
#include<math.h>
#include<stdlib.h>
#include<malloc.h>
#include<time.h>
#include<locale>
#include<mutex>

#ifndef MAXCLONESCOUNT
#define MAXCLONESCOUNT 100   ///< 最大克隆数量默认值
#endif

#ifndef MAXELEMENTCOUNT
#define MAXELEMENTCOUNT 100  ///< 最大元素数量默认值
#endif

#define leftButton VK_LBUTTON  ///< 鼠标左键宏定义
#define log printf             ///< 日志输出宏
#define pie 3.141592653589793238462643383279502f  ///< 圆周率定义

#define initXY() {WIDTH = getwidth();HEIGHT = getheight();}  ///< 初始化屏幕尺寸宏
#define X WIDTH  ///< 屏幕宽度宏
#define Y HEIGHT ///< 屏幕高度宏

#ifdef bigScreen
#define getkey GetAsyncKeyState  ///< 大屏幕模式下的按键检测宏
#endif

using namespace std;

const double eps = 1e-7;       ///< 浮点数比较精度
extern double globalRating;    ///< 全局评分变量

// 前向声明
class Position;
class Element;
class InputBox;
class InputBoxSet;

/**
 * @brief 注册元素到管理系统
 * @param element 要注册的元素指针
 */
void regElement(Element* element);

/**
 * @brief 创建新元素(PIMAGE版本)
 * @param id 元素ID
 * @param image 元素图像
 * @param x x坐标
 * @param y y坐标
 * @return 创建的元素指针
 */
Element* newElement(string,PIMAGE,double,double);

/**
 * @brief 创建新元素(文件路径版本)
 * @param id 元素ID
 * @param imagePath 图像文件路径
 * @param x x坐标
 * @param y y坐标
 * @return 创建的元素指针
 */
Element* newElement(string,string,double,double);

// 全局变量声明
extern bool isKey;                                   ///< 按键状态标志
extern key_msg keyMsg;                               ///< 按键消息
extern bool mouseHit;                                ///< 鼠标点击标志
extern bool needSort;                                ///< 需要排序标志
extern bool regPause;                                ///< 注册暂停标志
extern mouse_msg mouseInfo;                          ///< 鼠标信息
extern bool keyStatus[360];                          ///< 按键状态数组
extern map<string,function<void(void)>> globalListenFrameFunctionSet;      ///< 全局帧监听函数集合
extern map<string,function<void(void)>> globalListenOnClickFunctionSet;    ///< 全局点击监听函数集合
extern map<int,bool> vkState;                        ///< 虚拟键状态映射

extern int WIDTH;    ///< 屏幕宽度
extern int HEIGHT;   ///< 屏幕高度

extern vector<function<void(void)> > schedule;                          ///< 调度函数列表
extern map<double,vector<function<void(void)> > > scheduleTimeOut;      ///< 定时调度函数映射
extern int __SIZE__;                     ///< 当前大小
extern int removeSize;                   ///< 移除大小
extern bool closeGraph;                  ///< 关闭图形标志

extern PIMAGE pen_image;                 ///< 画笔图像

extern unordered_map<string,Element*>idToElement;    ///< ID到元素的映射
extern unordered_map<Element*,bool>elementIsIn;      ///< 元素存在标志

/**
 * @brief 符号函数
 * @param x 输入值
 * @return 1(x>0), -1(x<0), 0(x=0)
 */
int sgn(double x);

/**
 * @class Position
 * @brief 位置类，表示二维坐标
 */
class Position {
    public:
        // 成员变量
        double x;   ///< x坐标
        double y;   ///< y坐标

        // 成员函数
        Position(double x = 0,double y = 0);  ///< 构造函数
        ~Position();                         ///< 析构函数
} extern lastPixel, mousePos;  ///< 全局变量：上一个像素位置和鼠标位置

/**
 * @namespace FeEGE
 * @brief FeEGE库的核心功能命名空间
 */
namespace FeEGE {
    #ifndef bigScreen
    /**
     * @brief 内置按键检测函数
     * @return 是否有按键按下
     */
    bool builtinGetKey();
    
    /**
     * @brief 检测指定按键状态
     * @param VK 虚拟键码
     * @return 按键是否按下
     */
    bool getkey(int VK);
    #endif
    
    /**
     * @brief 启用暂停功能
     */
    void enablePause();
    
    /**
     * @brief 禁用暂停功能
     */
    void disablePause();
    
    /**
     * @brief 通过ID获取元素
     * @param id 元素ID
     * @return 元素指针
     */
    Element* getElementById(string);
    
    /**
     * @brief 通过指针获取元素
     * @param ptr 元素指针
     * @return 元素指针
     */
    Element* getElementByPtr(Element*);
    
    /**
     * @brief 初始化画笔
     */
    void initPen();
    
    /**
     * @brief 获取当前毫秒数
     * @return 当前毫秒数
     */
    double getMs();
    
    /**
     * @brief 添加调度函数
     * @param func 要添加的函数
     */
    void pushSchedule(function<void(void)> func);
    
    /**
     * @brief 设置定时器
     * @param func 要执行的函数
     * @param timeMs 延迟时间(毫秒)
     */
    void setTimeOut(function<void(void)> func,double timeMs);

    /**
     * @class ClonesEvent
     * @brief 克隆事件相关定义
     */
    class ClonesEvent {
        public:
            int on_clone = 0x06;  ///< 克隆事件类型
    };
    
    /**
     * @class InputBoxEvent
     * @brief 输入框事件相关定义
     */
    class InputBoxEvent {
        public:
            int onSelect = 0x06;  ///< 选择事件类型
    };

    /**
     * @class Events
     * @brief 事件类型定义
     */
    class Events {
        public:
            int frame = 0x00;                 ///< 帧事件
            int on_mouse_put_on = 0x01;       ///< 鼠标悬停事件
            int on_mouse_hitting = 0x02;      ///< 鼠标点击事件
            int on_mouse_move_away = 0x03;    ///< 鼠标移开事件
            int on_click = 0x04;             ///< 点击事件
            int on_clone = 0x05;             ///< 克隆事件
            ClonesEvent clones;              ///< 克隆事件实例
            InputBoxEvent inputBox;          ///< 输入框事件实例
    };

    extern Events EventType;  ///< 全局事件类型实例

    /**
     * @class PenTypes
     * @brief 画笔类型定义
     */
    class PenTypes {
        public:
            int left = 0x01;    ///< 左键
            int middle = 0x02;  ///< 中键
    };

    extern PenTypes penType;  ///< 全局画笔类型实例

    /**
     * @brief 获取上一个像素位置
     * @return 位置引用
     */
    Position& getLastPixel();
    
    /**
     * @brief 获取鼠标位置
     * @return 位置引用
     */
    Position& getMousePos();
}

/**
 * @class Animate
 * @brief 动画类，定义动画行为
 */
class Animate{
    public:
        function<Position(double)> function;  ///< 动画函数
        double speed;    ///< 动画速度
        double end;      ///< 结束条件
        
        /**
         * @brief 构造函数
         * @param function 动画函数
         * @param speed 动画速度
         * @param end 结束条件
         */
        Animate(std::function<Position(double)> function,double speed,double end);
};

/**
 * @namespace Ease
 * @brief 缓动动画函数命名空间
 */
namespace Ease{
	
extern Animate easeInOutCubicX;  ///< X轴三次缓入缓出动画
extern Animate easeInOutCubicY;  ///< Y轴三次缓入缓出动画
extern Animate easeOutQuadX;     ///< X轴二次缓出动画
extern Animate easeOutQuadY;     ///< Y轴二次缓出动画

}

/**
 * @class Element
 * @brief 元素基类，表示图形界面中的可显示元素
 */
class Element {
protected:
    // 成员变量
    string id;                  ///< 元素ID
    string elementType;         ///< 元素类型
    short scale;                ///< 缩放比例
    int angle;                  ///< 旋转角度
    int order;                  ///< 显示顺序
    short alpha;                ///< 透明度
    int regOrder;               ///< 注册顺序
    Position pos;               ///< 当前位置
    Position backupPos;         ///< 备份位置
    bool isCancelX;             ///< X轴移动取消标志
    bool isCancelY;             ///< Y轴移动取消标志
    bool isShow;                ///< 显示状态
    PIMAGE __visibleImage;      ///< 可见图像
    vector<PIMAGE> imageVector; ///< 图像向量
    vector<Element*> clones;    ///< 克隆体列表
    vector<Element*> removeList; ///< 待移除列表
    map<string,function<void(Element*)> > frameFunctionSet;               ///< 帧函数集合
    map<string,function<void(Element*)> > onMousePutOnFunctionSet;        ///< 鼠标悬停函数集合
    map<string,function<void(Element*)> > onMouseHittingFunctionSet;      ///< 鼠标点击函数集合
    map<string,function<void(Element*)> > onMouseMoveAwayFunctionSet;    ///< 鼠标移开函数集合
    map<string,function<void(Element*)> > onClickFunctionSet;            ///< 点击函数集合
    map<string,function<void(Element*)> > onCloneFunctionSet;            ///< 克隆函数集合
    map<string,function<void(Element*)> > onCloneClonesFunctionSet;      ///< 克隆体函数集合
    vector<pair<pair<double,pair<double,double> >,Animate> > animations; ///< 动画列表
    unordered_map<int,double> animateStates;          ///< 动画状态
    unordered_map<int,function<void(Element*)> > animateCallbacks; ///< 动画回调
    unsigned int currentImage = 0;        ///< 当前图像索引
    unordered_map<int,long long> privateVariables; ///< 私有变量
    bool deletedList[MAXCLONESCOUNT] = {}; ///< 删除标记列表
    int cloneCount = 0;                   ///< 克隆计数
    int nextCloneCount = 0;               ///< 下一个克隆计数
    bool deleted = false;                 ///< 删除标志
    bool imageLock = false;               ///< 图像锁定标志
    bool disabledDrawToPrivateImage = false; ///< 私有图像绘制禁用标志
    Element** cloneQueue = nullptr;       ///< 克隆队列
    bool drawed = false;                  ///< 已绘制标志
    int poolIndex;                        ///< 池索引
    bool hittingBox = false;              ///< 碰撞检测框标志
    int HBheight, HBwidth;               ///< 碰撞检测框尺寸

    bool physicEngineStatu;   ///< 物理引擎状态
    double forceX;            ///< X轴受力
    double forceY;            ///< Y轴受力
    double speedX;            ///< X轴速度
    double speedY;            ///< Y轴速度
    double resistanceX;       ///< X轴阻力
    double resistanceY;       ///< Y轴阻力

    vector<color_t> removeColors;  ///< 要移除的颜色列表

    /**
     * @brief 刷新鼠标状态
     */
    virtual void reflushMouseStatu();
    
    /**
     * @brief 绘制到私有图像
     * @return 是否成功
     */
    bool drawToPrivateImage();
    
public:
    // 构造函数和析构函数
    Element(string id,PIMAGE image,Position pos);
    Element(string id,PIMAGE image,double x = 0,double y = 0);
    Element();
    
    /**
     * @brief 复制元素
     * @param id 新元素ID
     * @param image 图像
     * @param index 索引
     * @param x x坐标
     * @param y y坐标
     * @return 新元素指针
     */
    Element* copy(string id,PIMAGE image,unsigned long long index,double x = 0,double y = 0);
    
    // 动画相关方法
    /**
	 * @brief 取消指定动画
	 * @param animate 要取消的动画对象
	 * @param tick 当前时间点
	 * @param x 目标x坐标
	 * @param y 目标y坐标
	 */
	void cancelAnimate(const Animate& animate,double tick,double x,double y);
	
	/**
	 * @brief 运行动画
	 * @param animate 要运行的动画对象
	 * @param tick 当前时间点
	 * @param x 目标x坐标
	 * @param y 目标y坐标
	 */
	void runAnimate(const Animate& animate,double tick,double x,double y);
	
	/**
	 * @brief 调用所有动画
	 */
	void callAnimations();
	
	/**
	 * @brief 元素主循环调用函数
	 */
	virtual void call();
	
	/**
	 * @brief 设置元素类型
	 * @param type 类型字符串
	 */
	void setType(const string& type);
	
	/**
	 * @brief 获取元素类型
	 * @param type 类型字符串
	 * @return 元素类型
	 */
	string getType(const string& type);
	
	/**
	 * @brief 向左移动元素
	 * @param pixels 移动像素数，默认为0
	 */
	void moveLeft(double pixels = 0);
	
	/**
	 * @brief 向右移动元素
	 * @param pixels 移动像素数，默认为0
	 */
	void moveRight(double pixels = 0);
	
	/**
	 * @brief 向上移动元素
	 * @param pixels 移动像素数，默认为0
	 */
	void moveUp(double pixels = 0);
	
	/**
	 * @brief 向下移动元素
	 * @param pixels 移动像素数，默认为0
	 */
	void moveDown(double pixels = 0);
	
	/**
	 * @brief 向前移动元素(深度方向)
	 * @param pixels 移动像素数，默认为0
	 */
	void moveForward(double pixels = 0);
	
	/**
	 * @brief 移动到指定坐标
	 * @param x x坐标
	 * @param y y坐标
	 */
	void moveTo(double x,double y);
	
	/**
	 * @brief 移动到指定位置
	 * @param position 位置对象
	 */
	void moveTo(Position position);
	
	/**
	 * @brief 获取指定位置像素颜色
	 * @param x x坐标
	 * @param y y坐标
	 * @return 像素颜色值
	 */
	color_t getPixel(int x,int y);
	
	/**
	 * @brief 获取元素缩放比例
	 * @return 缩放比例值
	 */
	short getScale();
	
	/**
	 * @brief 增加缩放比例
	 * @param scale 要增加的比例值
	 */
	void increaseScale(short scale);
	
	/**
	 * @brief 减小缩放比例
	 * @param scale 要减小的比例值
	 */
	void decreaseScale(short scale);
	
	/**
	 * @brief 设置缩放比例
	 * @param scale 新的缩放比例值
	 */
	void setScale(short scale);
	
	/**
	 * @brief 显示元素
	 */
	void show();
	
	/**
	 * @brief 隐藏元素
	 */
	void hide();
	
	/**
	 * @brief 向右旋转元素
	 * @param angle 旋转角度
	 */
	void turnRight(int angle);
	
	/**
	 * @brief 向左旋转元素
	 * @param angle 旋转角度
	 */
	void turnLeft(int angle);
	
	/**
	 * @brief 旋转到指定角度
	 * @param angle 目标角度
	 */
	void turnTo(int angle);
	
	/**
	 * @brief 使元素朝向另一个元素
	 * @param that 目标元素指针
	 * @return 是否成功
	 */
	bool faceTo(Element* that);
	
	/**
	 * @brief 获取当前角度
	 * @return 当前角度值
	 */
	int getAngle();
	
	/**
	 * @brief 获取当前位置
	 * @return 位置对象
	 */
	Position getPosition();
	
	/**
	 * @brief 获取显示状态
	 * @return 是否显示
	 */
	bool getIsShow();
	
	/**
	 * @brief 增加绘制顺序
	 * @param count 增加量
	 */
	void increaseOrder(int count);
	
	/**
	 * @brief 减小绘制顺序
	 * @param count 减小量
	 */
	void decreaseOrder(int count);
	
	/**
	 * @brief 设置绘制顺序
	 * @param count 新的顺序值
	 */
	void setOrder(int count);
	
	/**
	 * @brief 设置注册顺序
	 * @param count 新的顺序值
	 */
	void setRegOrder(int count);
	
	/**
	 * @brief 获取注册顺序
	 * @return 注册顺序值
	 */
	int getRegOrder();
	
	/**
	 * @brief 获取绘制顺序
	 * @return 顺序值
	 */
	int getOrder();
	
	/**
	 * @brief 小于运算符重载
	 * @param _B 比较的元素
	 * @return 比较结果
	 */
	bool operator<(Element _B);
	
	/**
	 * @brief 大于运算符重载
	 * @param _B 比较的元素
	 * @return 比较结果
	 */
	bool operator>(Element _B);
	
	/**
	 * @brief 设置x坐标
	 * @param x 新的x坐标值
	 */
	void setPosX(int x);
	
	/**
	 * @brief 设置y坐标
	 * @param y 新的y坐标值
	 */
	void setPosY(int y);
	
	/**
	 * @brief 移除指定颜色
	 * @param color 要移除的颜色值
	 */
	void removeColor(color_t color);
	
	/**
	 * @brief 检查鼠标是否在元素内
	 * @return 是否在元素内
	 */
	bool isMouseIn();
	
	/**
	 * @brief 检查元素是否被点击
	 * @return 是否被点击
	 */
	bool isHit();
	
	/**
	 * @brief 设置私有变量值
	 * @param which 变量索引
	 * @param value 要设置的值
	 */
	void setVariable(unsigned int which,long long value);
	
	/**
	 * @brief 获取私有变量引用
	 * @param which 变量索引
	 * @return 变量引用
	 */
	long long& getVariable(unsigned int which);
	
	/**
	 * @brief 增加私有变量值
	 * @param which 变量索引
	 * @param value 增加值
	 */
	void increaseVariable(unsigned int which,long long value);
	
	/**
	 * @brief 减少私有变量值
	 * @param which 变量索引
	 * @param value 减少值
	 */
	void decreaseVariable(unsigned int which,long long value);
	
	/**
	 * @brief 添加图像
	 * @param image 图像指针
	 */
	void addImage(PIMAGE image);
	
	/**
	 * @brief 设置当前图像
	 * @param order 图像索引
	 * @return 是否成功
	 */
	bool setImage(int order);
	
	/**
	 * @brief 获取当前图像索引
	 * @return 图像索引
	 */
	int getImageOrder();
	
	/**
	 * @brief 获取当前图像
	 * @return 图像指针
	 */
	PIMAGE getImage();
	
	/**
	 * @brief 检查是否与另一元素接触
	 * @param that 另一元素指针
	 * @return 是否接触
	 */
	bool isTouchedBy(Element* that);
	
	/**
	 * @brief 克隆元素
	 * @return 新元素指针
	 */
	Element* clone();
	
	/**
	 * @brief 克隆当前图像
	 */
	void cloneImage();
	
	/**
	 * @brief 获取元素ID
	 * @return ID字符串
	 */
	string getId();
	
	/**
	 * @brief 添加事件监听
	 * @param listenMode 监听模式
	 * @param identifier 标识符
	 * @param function 回调函数
	 */
	virtual void listen(int listenMode,string identifier,function<void(Element*)> function);
	
	/**
	 * @brief 移除事件监听
	 * @param listenMode 监听模式
	 * @param identifier 标识符
	 */
	virtual void stop(int listenMode,string identifier);
	
	/**
	 * @brief 删除当前元素
	 * @return 元素指针(通常为nullptr)
	 */
	Element* deleteThis();
	
	/**
	 * @brief 取消X轴移动
	 */
	void cancelX();
	
	/**
	 * @brief 取消Y轴移动
	 */
	void cancelY();
	
	/**
	 * @brief 取消所有移动
	 */
	void cancelMove();
	
	/**
	 * @brief 获取透明度
	 * @return 透明度值
	 */
	short getAlpha();
	
	/**
	 * @brief 设置透明度
	 * @param alpha 新的透明度值
	 */
	void setAlpha(short alpha);
	
	/**
	 * @brief 减小透明度
	 * @param alpha 减小量
	 */
	void decreaseAlpha(short alpha);
	
	/**
	 * @brief 增加透明度
	 * @param alpha 增加量
	 */
	void increaseAlpha(short alpha);
	
	/**
	 * @brief 切换到下一张图像
	 */
	void nextImage();
	
	#ifndef NO_THREAD
	/**
	 * @brief 加载图像
	 * @param imagePath 图像路径
	 * @param id 图像ID
	 */
	void loadImage(string imagePath,int id);
	
	/**
	 * @brief 删除图像
	 * @param id 图像ID
	 */
	void deleteImage(int id);
	#endif
	
	/**
	 * @brief 禁用绘制到私有图像
	 */
	void disableDrawToPrivateImage();
	
	/**
	 * @brief 启用绘制到私有图像
	 */
	void enableDrawToPrivateImage();
	
	/**
	 * @brief 忽略黑色像素(用于透明处理)
	 */
	void ignoreBlack();
	
	#ifdef TEST_FUNCTION 
	/**
	 * @brief 启用碰撞检测框
	 */
	void useHittingBox();
	
	/**
	 * @brief 禁用碰撞检测框
	 */
	void stopHittingBox();
	
	// 物理引擎相关方法
	
	/**
	 * @brief 启用物理引擎
	 */
	void enablePhysicEngine();
	
	/**
	 * @brief 禁用物理引擎
	 */
	void disablePhysicEngine();
	
	/**
	 * @brief 施加物理力
	 * @param angle 力的角度
	 * @param force 力的大小
	 */
	void physicForce(double angle,double force);
	
	/**
	 * @brief 设置物理速度
	 * @param angle 速度角度
	 * @param speed 速度大小
	 */
	void physicSpeed(double angle,double speed);
	
	/**
	 * @brief 设置物理阻力
	 * @param angle 阻力角度
	 * @param resistance 阻力大小
	 */
	void physicResistance(double angle,double resistance);
	
	/**
	 * @brief 更新物理速度
	 */
	void updatePhysicSpeed();
	
	/**
	 * @brief 更新物理位置
	 */
	void updatePhysicPosition();
	
	/**
	 * @brief 获取X轴速度
	 * @return X轴速度值
	 */
	double getSpeedX();
	
	/**
	 * @brief 获取Y轴速度
	 * @return Y轴速度值
	 */
	double getSpeedY();
	
	/**
	 * @brief 弧形X轴速度(模拟抛物线运动)
	 */
	void physicArcSpeedX();
	
	/**
	 * @brief 弧形Y轴速度(模拟抛物线运动)
	 */
	void physicArcSpeedY();
	
	/**
	 * @brief 移除X轴速度
	 */
	void physicRemoveSpeedX();
	
	/**
	 * @brief 移除Y轴速度
	 */
	void physicRemoveSpeedY();
	
	/**
	 * @brief 移除X轴力
	 */
	void physicRemoveForceX();
	
	/**
	 * @brief 移除Y轴力
	 */
	void physicRemoveForceY();
	
	/**
	 * @brief 使用动画移动元素
	 * @param xPixel X轴移动距离
	 * @param yPixel Y轴移动距离
	 * @param animate 动画对象
	 * @param callback 完成回调函数
	 */
	void moveWithAnimation(double xPixel,double yPixel,const Animate& animate,function<void(Element*)> callback = nullptr);
	#endif
	
	/**
	 * @brief 删除元素
	 * @return 元素指针(通常为nullptr)
	 */
	Element* deleteElement();
	
	/**
	 * @brief 析构函数
	 */
	~Element();
};

/**
 * @class InputBoxSet
 * @brief 输入框集合管理类
 */
class InputBoxSet{
protected:
    bool isEnabled;             ///< 启用状态
    set<Element*> childs;       ///< 子元素集合
    Element* selectedInputBox;  ///< 当前选中的输入框
    
public:
    InputBoxSet();
    void enable();
    void disable();
    bool getEnableStatu();
    void insert(Element* inputBox);
    void erase(Element* inputBox);
    void select(Element* inputBox);
    void call();
    ~InputBoxSet();
};

/**
 * @class InputBox
 * @brief 输入框类，继承自Element
 */
class InputBox : public Element{
protected:
    bool isEnabled;         ///< 启用状态
    InputBoxSet* father;    ///< 父集合
    Element* submitButton;  ///< 提交按钮
public:
    InputBox();
    InputBox(InputBoxSet* father,Element* submitButton);
    void bind(InputBoxSet* father);
    void enable();
    void disable();
    bool getEnableStatu();
    virtual void listen(int listenMode,string identifier,function<void(Element*)> function);
    virtual void stop(int listenMode,string identifier);
    void reflushMouseStatu() override;
    void call() override;
    ~InputBox();
};

/**
 * @class Button
 * @brief 按钮类，继承自Element
 */
class Button : public Element{
protected:
    bool isEnabled;  ///< 启用状态
public:
    Button();
    void enable();
    void disable();
    bool getEnableStatu();
    void reflushMouseStatu() override;
    void call() override;
    ~Button();
};

// 全局变量声明
extern int currentRegOrder;                  ///< 当前注册顺序
extern unsigned long long frame;             ///< 帧计数器
extern queue<Element*> freeList;             ///< 空闲元素列表

/**
 * @class cmp
 * @brief 元素比较类，用于排序
 */
class cmp {
public:
    bool operator()(Element* _A,Element* _B);  ///< 比较运算符重载
};

extern set<Element*,cmp> elementQueue;  ///< 元素队列

/**
 * @namespace pen
 * @brief 画笔相关功能命名空间
 */
namespace pen {
	
extern int order;           ///< 绘制顺序
extern int fontScale;       ///< 字体缩放
extern short penAlpha;      ///< 画笔透明度
extern int penType;         ///< 画笔类型
extern int charWidth, charHeight;  ///< 字符宽度和高度

void print(int x,int y,string str);
void print(int x,int y,wstring str);
void font(int scale,string fontName = "幼圆");
void color(color_t color);
void type(int type);
void clear(int x,int y,int ex,int ey);
void clearChar(int x,int y);
void clearChars(int x,int y,int charCount);
void clearAll();
void setOrder(int value);
void setAlpha(short alpha);
short getAlpha();
void increaseAlpha(short alpha);
void decreaseAlpha(short alpha);
void printLine(int x1,int y1,int x2,int y2);
void printBar(int left,int top,int right,int bottom);

}

// 函数声明
void reflush();
void init(int x,int y,int mode = INIT_RENDERMANUAL);
void start();
void globalListen(int listenMode,string identifier,auto function);
void stopGlobalListen(int listenMode,string identifier);
Button* newButton(string id,string imagePath,double x,double y,function<void(Element*)> putOn = nullptr,function<void(Element*)> moveAway = nullptr,function<void(Element*)> on_click = nullptr);
Element* newInputBox(string id,string imagePath,double x,double y,Element* submitButton,function<void(Element*)> onSubmit = nullptr,function<void(Element*)> onSelected = nullptr);
string getCurrentPath();
string resolvePath(string s);
bool isValidUTF8(const string& fileContent);
std::string detectEncoding(const std::string& fileContent);
std::string UTF8ToANSI(const std::string& utf8Str);
void runCmdAwait(const std::string cmd);

#endif
