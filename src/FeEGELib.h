#ifndef _FEEGELIB_
#define _FEEGELIB_

#define FeEGELib_version "V1.3.1.0--upd2025-04-08"
#define version() FeEGELib_version

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
#define MAXCLONESCOUNT 100
#endif

#ifndef MAXELEMENTCOUNT
#define MAXELEMENTCOUNT 100
#endif

#define leftButton VK_LBUTTON
#define log printf
#define pie 3.141592653589793238462643383279502f

#define initXY() {WIDTH = getwidth();HEIGHT = getheight();}
#define X WIDTH
#define Y HEIGHT

#ifdef bigScreen
#define getkey GetAsyncKeyState
#endif

using namespace std;

const double eps = 1e-7;
extern double globalRating;

class Position;
class Element;
class InputBox;
class InputBoxSet;
void regElement(Element* element);
Element* newElement(string,PIMAGE,double,double);
Element* newElement(string,string,double,double);

extern bool isKey;
extern key_msg keyMsg;
extern bool mouseHit;
extern bool needSort;
extern bool regPause;
extern mouse_msg mouseInfo;
extern bool keyStatus[360];
extern map<string,function<void(void)>> globalListenFrameFunctionSet;
extern map<string,function<void(void)>> globalListenOnClickFunctionSet;
extern map<int,bool> vkState;

extern int WIDTH;
extern int HEIGHT;

extern vector<function<void(void)> > schedule;
extern map<double,vector<function<void(void)> > > scheduleTimeOut;
extern int __SIZE__;
extern int removeSize;
extern bool closeGraph;

extern PIMAGE pen_image;

extern unordered_map<string,Element*>idToElement;
extern unordered_map<Element*,bool>elementIsIn;

int sgn(double x);

class Position {
    public:
        //Variables
        double x;
        double y;

        // Functions
        Position(double x = 0,double y = 0);
        ~Position();
} extern lastPixel, mousePos;

namespace FeEGE {
    #ifndef bigScreen
    bool builtinGetKey();
    bool getkey(int VK);
    #endif
    void enablePause();
    void disablePause();
    Element* getElementById(string);
    Element* getElementByPtr(Element*);
    void initPen();
    
    double getMs();
    
    void pushSchedule(function<void(void)> func);
    
    void setTimeOut(function<void(void)> func,double timeMs);

    class ClonesEvent {
        public:
            int on_clone = 0x06;
    };
    
    class InputBoxEvent {
        public:
            int onSelect = 0x06;
    };

    class Events {
        public:
            int frame = 0x00;
            int on_mouse_put_on = 0x01;
            int on_mouse_hitting = 0x02;
            int on_mouse_move_away = 0x03;
            int on_click = 0x04;
            int on_clone = 0x05;
            ClonesEvent clones;
            InputBoxEvent inputBox;
    };

    extern Events EventType;

    class PenTypes {
        public:
            int left = 0x01;
            int middle = 0x02;
    };

    extern PenTypes penType;

    Position& getLastPixel();
    Position& getMousePos();
}

class Animate{
    public:
        function<Position(double)> function;
        double speed;
        double end;
        
        Animate(std::function<Position(double)> function,double speed,double end);
};

namespace Ease{
    extern Animate easeInOutCubicX;
    extern Animate easeInOutCubicY;
    extern Animate easeOutQuadX;
    extern Animate easeOutQuadY;
}

class Element {
    protected:
        //Variables
        string id;
        string elementType;
        short scale;
        int angle;
        int order;
        short alpha;
        int regOrder;
        Position pos;
        Position backupPos;
        bool isCancelX;
        bool isCancelY;
        bool isShow;
        PIMAGE __visibleImage;
        vector<PIMAGE> imageVector;
        vector<Element*> clones;
        vector<Element*> removeList;
        map<string,function<void(Element*)> > frameFunctionSet;
        map<string,function<void(Element*)> > onMousePutOnFunctionSet;
        map<string,function<void(Element*)> > onMouseHittingFunctionSet;
        map<string,function<void(Element*)> > onMouseMoveAwayFunctionSet;
        map<string,function<void(Element*)> > onClickFunctionSet;
        map<string,function<void(Element*)> > onCloneFunctionSet;
        map<string,function<void(Element*)> > onCloneClonesFunctionSet;
        vector<pair<pair<double,pair<double,double> >,Animate> > animations;
        unordered_map<int,double> animateStates;
        unordered_map<int,function<void(Element*)> > animateCallbacks;
        unsigned int currentImage = 0;
        unordered_map<int,long long> privateVariables;
        bool deletedList[MAXCLONESCOUNT] = {};
        int cloneCount = 0;
        int nextCloneCount = 0;
        bool deleted = false;
        bool imageLock = false;
        bool disabledDrawToPrivateImage = false;
        Element** cloneQueue = nullptr;
        bool drawed = false;
        int poolIndex;
        bool hittingBox = false;
        int HBheight,HBwidth;

        bool physicEngineStatu;
        double forceX;
        double forceY;
        double speedX;
        double speedY;
        double resistanceX;
        double resistanceY;

        vector<color_t> removeColors;

        virtual void reflushMouseStatu();
        bool drawToPrivateImage();
    public:
        //Functions
        Element(string id,PIMAGE image,Position pos);
        Element(string id,PIMAGE image,double x = 0,double y = 0);
        Element();
        Element* copy(string id,PIMAGE image,unsigned long long index,double x = 0,double y = 0);
        
        void cancelAnimate(const Animate& animate,double tick,double x,double y);
        void runAnimate(const Animate& animate,double tick,double x,double y);
        void callAnimations();
        
        virtual void call();
        void setType(const string& type);
        string getType(const string& type);
        void moveLeft(double pixels = 0);
        void moveRight(double pixels = 0);
        void moveUp(double pixels = 0);
        void moveDown(double pixels = 0);
        void moveForward(double pixels = 0);
        void moveTo(double x,double y);
        void moveTo(Position position);
        color_t getPixel(int x,int y);
        short getScale();
        void increaseScale(short scale);
        void decreaseScale(short scale);
        void setScale(short scale);
        void show();
        void hide();
        void turnRight(int angle);
        void turnLeft(int angle);
        void turnTo(int angle);
        bool faceTo(Element* that);
        int getAngle();
        Position getPosition();
        bool getIsShow();
        void increaseOrder(int count);
        void decreaseOrder(int count);
        void setOrder(int count);
        void setRegOrder(int count);
        int getRegOrder();
        int getOrder();
        bool operator<(Element _B);
        bool operator>(Element _B);
        void setPosX(int x);
        void setPosY(int y);
        void removeColor(color_t color);
        bool isMouseIn();
        bool isHit();
        void setVariable(unsigned int which,long long value);
        long long& getVariable(unsigned int which);
        void increaseVariable(unsigned int which,long long value);
        void decreaseVariable(unsigned int which,long long value);
        void addImage(PIMAGE image);
        bool setImage(int order);
        int getImageOrder();
        PIMAGE getImage();
        bool isTouchedBy(Element* that);
        Element* clone();
        void cloneImage();
        string getId();
        virtual void listen(int listenMode,string identifier,function<void(Element*)> function);
        virtual void stop(int listenMode,string identifier);
        Element* deleteThis();
        void cancelX();
        void cancelY();
        void cancelMove();
        short getAlpha();
        void setAlpha(short alpha);
        void decreaseAlpha(short alpha);
        void increaseAlpha(short alpha);
        void nextImage();
#ifndef NO_THREAD
        void loadImage(string imagePath,int id);
        void deleteImage(int id);
#endif
        void disableDrawToPrivateImage();
        void enableDrawToPrivateImage();
        void ignoreBlack();
#ifdef TEST_FUNCTION 
        void useHittingBox();
        void stopHittingBox();

        // PhysicEngine
        void enablePhysicEngine();
        void disablePhysicEngine();
        void physicForce(double angle,double force);
        void physicSpeed(double angle,double speed);
        void physicResistance(double angle,double resistance);
        void updatePhysicSpeed();
        void updatePhysicPosition();
        double getSpeedX();
        double getSpeedY();
        void physicArcSpeedX();
        void physicArcSpeedY();
        void physicRemoveSpeedX();
        void physicRemoveSpeedY();
        void physicRemoveForceX();
        void physicRemoveForceY();
        
        // Move Animated
        void moveWithAnimation(double xPixel,double yPixel,const Animate& animate,function<void(Element*)> callback = nullptr);
#endif

        Element* deleteElement();
        ~Element();
};

class InputBoxSet{
    protected:
        bool isEnabled;
        set<Element*> childs;
        Element* selectedInputBox;
        
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

class InputBox : public Element{
    protected:
        bool isEnabled;
        InputBoxSet* father;
        Element* submitButton;
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

class Button : public Element{
    protected:
        bool isEnabled;
    public:
        Button();
        void enable();
        void disable();
        bool getEnableStatu();
        void reflushMouseStatu() override;
        void call() override;
        ~Button();
};

extern int currentRegOrder;
extern unsigned long long frame;
extern queue<Element*> freeList;

class cmp {
    public:
        bool operator()(Element* _A,Element* _B);
};

extern set<Element*,cmp> elementQueue;

namespace pen {
    extern int order;
    extern int fontScale;
    extern short penAlpha;
    extern int penType;
    extern int charWidth,charHeight;
    void print(int x,int y,string str);
    void print(int x,int y,wstring str);
    void font(int scale,string fontName = "свт╡");
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

//Function declarations
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
