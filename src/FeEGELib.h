/**
 * @file FeEGELib.h
 * @brief FeEGEͼ�ο���ͷ�ļ�
 * @version 2.0.6.0
 * @date 2025-05-07
 * 
 * ����һ������ EGE ��ͼ�ο�����ܣ��ṩ��Ԫ�ع����������¼�����ȹ���
 */

#ifndef _FEEGELIB_
#define _FEEGELIB_

#define FeEGELib_version "V2.0.6.0--upd2025-05-07"  ///< ��汾��
#define version() FeEGELib_version                   ///< ��ȡ�汾�ŵĺ�

// ������Ҫ��ͷ�ļ�
#include "Base.h"
#include "Collision.h"
#include <graphics.h>
#include <vector>
#include <thread>
#include <set>
#include <map>
#include <unordered_map>
#include <string>
#include <windows.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <queue>
#include <math.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <locale>
#include <mutex>
#include <assert.h>

#ifndef MAXCLONESCOUNT
#define MAXCLONESCOUNT 100   ///< ����¡����Ĭ��ֵ
#endif

#ifndef MAXELEMENTCOUNT
#define MAXELEMENTCOUNT 100  ///< ���Ԫ������Ĭ��ֵ
#endif

#define leftButton VK_LBUTTON  ///< �������궨��
#define log printf             ///< ��־�����
#define pie 3.141592653589793238462643383279502f  ///< Բ���ʶ���

#define initXY() {WIDTH = getwidth();HEIGHT = getheight();}  ///< ��ʼ����Ļ�ߴ��
#define X WIDTH  ///< ��Ļ��Ⱥ�
#define Y HEIGHT ///< ��Ļ�߶Ⱥ�

#ifdef bigScreen
#define getkey GetAsyncKeyState  ///< ����Ļģʽ�µİ�������
#endif

using namespace std;
using namespace FeEGE;

const double eps = 1e-7;       ///< �������ȽϾ���
extern double globalRating;    ///< ȫ�����ֱ���

// ǰ������
class Element;
class InputBox;
class InputBoxSet;

/**
 * @brief ע��Ԫ�ص�����ϵͳ
 * @param element Ҫע���Ԫ��ָ��
 */
void regElement(Element* element);

/**
 * @brief ������Ԫ��(PIMAGE�汾)
 * @param id Ԫ��ID
 * @param image Ԫ��ͼ��
 * @param x x����
 * @param y y����
 * @return ������Ԫ��ָ��
 */
Element* newElement(string,PIMAGE,double,double);

/**
 * @brief ������Ԫ��(�ļ�·���汾)
 * @param id Ԫ��ID
 * @param imagePath ͼ���ļ�·��
 * @param x x����
 * @param y y����
 * @return ������Ԫ��ָ��
 */
Element* newElement(string,string,double,double);

// ȫ�ֱ�������
extern bool isKey;                                   ///< ����״̬��־
extern key_msg keyMsg;                               ///< ������Ϣ
extern bool mouseHit;                                ///< �������־
extern bool needSort;                                ///< ��Ҫ�����־
extern bool regPause;                                ///< ע����ͣ��־
extern mouse_msg mouseInfo;                          ///< �����Ϣ
extern bool keyStatus[360];                          ///< ����״̬����
extern map<string,function<void(void)>> globalListenFrameFunctionSet;      ///< ȫ��֡������������
extern map<string,function<void(void)>> globalListenOnClickFunctionSet;    ///< ȫ�ֵ��������������
extern map<int,bool> vkState;                        ///< �����״̬ӳ��

extern int WIDTH;    ///< ��Ļ���
extern int HEIGHT;   ///< ��Ļ�߶�

extern vector<function<void(void)> > schedule;                          ///< ���Ⱥ����б�
extern map<double,vector<function<void(void)> > > scheduleTimeOut;      ///< ��ʱ���Ⱥ���ӳ��
extern int __SIZE__;                     ///< ��ǰ��С
extern int removeSize;                   ///< �Ƴ���С
extern bool closeGraph;                  ///< �ر�ͼ�α�־

extern PIMAGE pen_image;                 ///< ����ͼ��

extern unordered_map<string,Element*>idToElement;    ///< ID��Ԫ�ص�ӳ��
extern unordered_map<Element*,bool>elementIsIn;      ///< Ԫ�ش��ڱ�־

/**
 * @brief ���ź���
 * @param x ����ֵ
 * @return 1(x>0), -1(x<0), 0(x=0)
 */
int sgn(double x);

/**
 * @namespace FeEGE
 * @brief FeEGE��ĺ��Ĺ��������ռ�
 */
namespace FeEGE {
	extern Position lastPixel, mousePos;  ///< ȫ�ֱ�������һ������λ�ú����λ��
    #ifndef bigScreen
    /**
     * @brief ���ð�����⺯��
     * @return �Ƿ��а�������
     */
    void builtinGetKey();
    
    /**
     * @brief ���ָ������״̬
     * @param VK �������
     * @return �����Ƿ���
     */
    bool getkey(int VK);
    #endif
    
    /**
     * @brief ������ͣ����
     */
    void enablePause();
    
    /**
     * @brief ������ͣ����
     */
    void disablePause();
    
    /**
     * @brief ͨ��ID��ȡԪ��
     * @param id Ԫ��ID
     * @return Ԫ��ָ��
     */
    Element* getElementById(string);
    
    /**
     * @brief ͨ��ָ���ȡԪ��
     * @param ptr Ԫ��ָ��
     * @return Ԫ��ָ��
     */
    Element* getElementByPtr(Element*);
    
    /**
     * @brief ��ʼ������
     */
    void initPen();
    
    /**
     * @brief ��ȡ��ǰ������
     * @return ��ǰ������
     */
    double getMs();
    
    /**
     * @brief ��ӵ��Ⱥ���
     * @param func Ҫ��ӵĺ���
     */
    void pushSchedule(function<void(void)> func);
    
    /**
     * @brief ���ö�ʱ��
     * @param func Ҫִ�еĺ���
     * @param timeMs �ӳ�ʱ��(����)
     */
    void setTimeOut(function<void(void)> func,double timeMs);

    /**
     * @class ClonesEvent
     * @brief ��¡�¼���ض���
     */
    class ClonesEvent {
        public:
            int on_clone = 0x07;  ///< ��¡�¼�����
    };
    
    /**
     * @class InputBoxEvent
     * @brief ������¼���ض���
     */
    class InputBoxEvent {
        public:
            int onSelect = 0x08;  ///< ѡ���¼�����
    };

    /**
     * @class Events
     * @brief �¼����Ͷ���
     */
    class Events {
        public:
            int frame = 0x00;                 ///< ֡�¼�
            int on_mouse_put_on = 0x01;       ///< �����ͣ�¼�
            int on_mouse_hitting = 0x02;      ///< ������¼�
            int on_mouse_move_away = 0x03;    ///< ����ƿ��¼�
            int on_click = 0x04;             ///< ����¼�
            int on_clone = 0x05;             ///< ��¡�¼�
            int on_mouse_hit_but_move_away = 0x06;      ///< ������¼�
            ClonesEvent clones;              ///< ��¡�¼�ʵ��
            InputBoxEvent inputBox;          ///< ������¼�ʵ��
    };

    extern Events EventType;  ///< ȫ���¼�����ʵ��

    /**
     * @class PenTypes
     * @brief �������Ͷ���
     */
    class PenTypes {
        public:
            int left = 0x01;    ///< ���
            int middle = 0x02;  ///< �м�
    };

    extern PenTypes penType;  ///< ȫ�ֻ�������ʵ��

    /**
     * @brief ��ȡ��һ������λ��
     * @return λ������
     */
    Position& getLastPixel();
    
    /**
     * @brief ��ȡ���λ��
     * @return λ������
     */
    Position& getMousePos();
}
using namespace FeEGE;
/**
 * @class Animate
 * @brief �����࣬���嶯����Ϊ
 */
class Animate{
    public:
        function<Position(double)> function;  ///< ��������
        double speed;    ///< �����ٶ�
        double end;      ///< ��������
        
        /**
         * @brief ���캯��
         * @param function ��������
         * @param speed �����ٶ�
         * @param end ��������
         */
        Animate(std::function<Position(double)> function,double speed,double end);
};

/**
 * @namespace Ease
 * @brief �����������������ռ�
 */
namespace Ease{
	
extern Animate easeInOutCubicX;  ///< X�����λ��뻺������
extern Animate easeInOutCubicY;  ///< Y�����λ��뻺������
extern Animate easeOutQuadX;     ///< X����λ�������
extern Animate easeOutQuadY;     ///< Y����λ�������

}
using namespace FeEGE;
/**
 * @class Element
 * @brief Ԫ�ػ��࣬��ʾͼ�ν����еĿ���ʾԪ��
 */
class Element {
protected:
    // ��Ա����
    string id;                  ///< Ԫ��ID
    string elementType;         ///< Ԫ������
    short scale;                ///< ���ű���
    int angle;                  ///< ��ת�Ƕ�
    int order;                  ///< ��ʾ˳��
    short alpha;                ///< ͸����
    int regOrder;               ///< ע��˳��
    Position pos;               ///< ��ǰλ��
    Position backupPos;         ///< ����λ��
    bool isCancelX;             ///< X���ƶ�ȡ����־
    bool isCancelY;             ///< Y���ƶ�ȡ����־
    bool isShow;                ///< ��ʾ״̬
    PIMAGE __visibleImage;      ///< �ɼ�ͼ��
    vector<PIMAGE> imageVector; ///< ͼ������
    vector<Element*> clones;    ///< ��¡���б�
    vector<Element*> removeList; ///< ���Ƴ��б�
    map<string,function<void(Element*)> > frameFunctionSet;               ///< ֡��������
    map<string,function<void(Element*)> > onMousePutOnFunctionSet;        ///< �����ͣ��������
    map<string,function<void(Element*)> > onMouseHittingFunctionSet;      ///< ��갴�º�������
    map<string,function<void(Element*)> > onMouseMoveAwayFunctionSet;    ///< ����ƿ���������
    map<string,function<void(Element*)> > onMouseHitButMoveAwaySet;		 ///< ��갴�µ��뿪�������� 
    map<string,function<void(Element*)> > onClickFunctionSet;            ///< �������������
    map<string,function<void(Element*)> > onCloneFunctionSet;            ///< ��¡��������
    map<string,function<void(Element*)> > onCloneClonesFunctionSet;      ///< ��¡�庯������
    vector<pair<pair<double,pair<double,double> >,Animate> > animations; ///< �����б�
    unordered_map<int,double> animateStates;          ///< ����״̬
    unordered_map<int,function<void(Element*)> > animateCallbacks; ///< �����ص�
    unsigned int currentImage = 0;        ///< ��ǰͼ������
    unordered_map<int,long long> privateVariables; ///< ˽�б���
    bool deletedList[MAXCLONESCOUNT] = {}; ///< ɾ������б�
    int cloneCount = 0;                   ///< ��¡����
    int nextCloneCount = 0;               ///< ��һ����¡����
    bool deleted = false;                 ///< ɾ����־
    bool imageLock = false;               ///< ͼ��������־
    bool disabledDrawToPrivateImage = false; ///< ˽��ͼ����ƽ��ñ�־
    Element** cloneQueue = nullptr;       ///< ��¡����
    bool drawed = false;                  ///< �ѻ��Ʊ�־
    int poolIndex;                        ///< ������
    bool hittingBox = false;              ///< ��ײ�����־
    vector<FeEGE::Polygon> polygonSet;			  ///< ͹����μ��� 
    int HBheight, HBwidth;               ///< ��ײ����ߴ�

    bool physicEngineStatu;   ///< ��������״̬
    double forceX;            ///< X������
    double forceY;            ///< Y������
    double speedX;            ///< X���ٶ�
    double speedY;            ///< Y���ٶ�
    double resistanceX;       ///< X������
    double resistanceY;       ///< Y������

    vector<color_t> removeColors;  ///< Ҫ�Ƴ�����ɫ�б�

    /**
     * @brief ˢ�����״̬
     */
    virtual void reflushMouseStatu();
    
    /**
     * @brief ���Ƶ�˽��ͼ��
     * @return �Ƿ�ɹ�
     */
    bool drawToPrivateImage();
    
public:
    // ���캯������������
    Element(string id,PIMAGE image,Position pos);
    Element(string id,PIMAGE image,double x = 0,double y = 0);
    Element();
    
    /**
     * @brief ����Ԫ��
     * @param id ��Ԫ��ID
     * @param image ͼ��
     * @param index ����
     * @param x x����
     * @param y y����
     * @return ��Ԫ��ָ��
     */
    Element* copy(string id,PIMAGE image,unsigned long long index,double x = 0,double y = 0);
    
    // ������ط���
    /**
	 * @brief ȡ��ָ������
	 * @param animate Ҫȡ���Ķ�������
	 * @param tick ��ǰʱ���
	 * @param x Ŀ��x����
	 * @param y Ŀ��y����
	 */
	void cancelAnimate(const Animate& animate,double tick,double x,double y);
	
	/**
	 * @brief ���ж���
	 * @param animate Ҫ���еĶ�������
	 * @param tick ��ǰʱ���
	 * @param x Ŀ��x����
	 * @param y Ŀ��y����
	 */
	void runAnimate(const Animate& animate,double tick,double x,double y);
	
	/**
	 * @brief �������ж���
	 */
	void callAnimations();
	
	/**
	 * @brief Ԫ����ѭ�����ú���
	 */
	virtual void call();
	
	/**
	 * @brief ����Ԫ������
	 * @param type �����ַ���
	 */
	void setType(const string& type);
	
	/**
	 * @brief ��ȡԪ������
	 * @param type �����ַ���
	 * @return Ԫ������
	 */
	string getType(const string& type);
	
	/**
	 * @brief �����ƶ�Ԫ��
	 * @param pixels �ƶ���������Ĭ��Ϊ0
	 */
	void moveLeft(double pixels = 0);
	
	/**
	 * @brief �����ƶ�Ԫ��
	 * @param pixels �ƶ���������Ĭ��Ϊ0
	 */
	void moveRight(double pixels = 0);
	
	/**
	 * @brief �����ƶ�Ԫ��
	 * @param pixels �ƶ���������Ĭ��Ϊ0
	 */
	void moveUp(double pixels = 0);
	
	/**
	 * @brief �����ƶ�Ԫ��
	 * @param pixels �ƶ���������Ĭ��Ϊ0
	 */
	void moveDown(double pixels = 0);
	
	/**
	 * @brief ��ǰ�ƶ�Ԫ��(��ȷ���)
	 * @param pixels �ƶ���������Ĭ��Ϊ0
	 */
	void moveForward(double pixels = 0);
	
	/**
	 * @brief �ƶ���ָ������
	 * @param x x����
	 * @param y y����
	 */
	void moveTo(double x,double y);
	
	/**
	 * @brief �ƶ���ָ��λ��
	 * @param position λ�ö���
	 */
	void moveTo(Position position);
	
	/**
	 * @brief ��ȡָ��λ��������ɫ
	 * @param x x����
	 * @param y y����
	 * @return ������ɫֵ
	 */
	color_t getPixel(int x,int y);
	
	/**
	 * @brief ��ȡԪ�����ű���
	 * @return ���ű���ֵ
	 */
	double getScale();
	
	/**
	 * @brief �������ű���
	 * @param scale Ҫ���ӵı���ֵ
	 */
	void increaseScale(double scale);
	
	/**
	 * @brief ��С���ű���
	 * @param scale Ҫ��С�ı���ֵ
	 */
	void decreaseScale(double scale);
	
	/**
	 * @brief �������ű���
	 * @param scale �µ����ű���ֵ
	 */
	void setScale(double scale);
	
	/**
	 * @brief ��ʾԪ��
	 */
	void show();
	
	/**
	 * @brief ����Ԫ��
	 */
	void hide();
	
	/**
	 * @brief ������תԪ��
	 * @param angle ��ת�Ƕ�
	 */
	void turnRight(int angle);
	
	/**
	 * @brief ������תԪ��
	 * @param angle ��ת�Ƕ�
	 */
	void turnLeft(int angle);
	
	/**
	 * @brief ��ת��ָ���Ƕ�
	 * @param angle Ŀ��Ƕ�
	 */
	void turnTo(int angle);
	
	/**
	 * @brief ʹԪ�س�����һ��Ԫ��
	 * @param that Ŀ��Ԫ��ָ��
	 * @return �Ƿ�ɹ�
	 */
	bool faceTo(Element* that);
	
	/**
	 * @brief ��ȡ��ǰ�Ƕ�
	 * @return ��ǰ�Ƕ�ֵ
	 */
	int getAngle();
	
	/**
	 * @brief ��ȡ��ǰλ��
	 * @return λ�ö���
	 */
	Position getPosition();
	
	/**
	 * @brief ��ȡ��ʾ״̬
	 * @return �Ƿ���ʾ
	 */
	bool getIsShow();
	
	/**
	 * @brief ���ӻ���˳��
	 * @param count ������
	 */
	void increaseOrder(int count);
	
	/**
	 * @brief ��С����˳��
	 * @param count ��С��
	 */
	void decreaseOrder(int count);
	
	/**
	 * @brief ���û���˳��
	 * @param count �µ�˳��ֵ
	 */
	void setOrder(int count);
	
	/**
	 * @brief ����ע��˳��
	 * @param count �µ�˳��ֵ
	 */
	void setRegOrder(int count);
	
	/**
	 * @brief ��ȡע��˳��
	 * @return ע��˳��ֵ
	 */
	int getRegOrder();
	
	/**
	 * @brief ��ȡ����˳��
	 * @return ˳��ֵ
	 */
	int getOrder();
	
	/**
	 * @brief С�����������
	 * @param _B �Ƚϵ�Ԫ��
	 * @return �ȽϽ��
	 */
	bool operator<(Element _B);
	
	/**
	 * @brief �������������
	 * @param _B �Ƚϵ�Ԫ��
	 * @return �ȽϽ��
	 */
	bool operator>(Element _B);
	
	/**
	 * @brief ����x����
	 * @param x �µ�x����ֵ
	 */
	void setPosX(int x);
	
	/**
	 * @brief ����y����
	 * @param y �µ�y����ֵ
	 */
	void setPosY(int y);
	
	/**
	 * @brief �Ƴ�ָ����ɫ
	 * @param color Ҫ�Ƴ�����ɫֵ
	 */
	void removeColor(color_t color);
	
	/**
	 * @brief �������Ƿ���Ԫ����
	 * @return �Ƿ���Ԫ����
	 */
	bool isMouseIn();
	
	/**
	 * @brief ���Ԫ���Ƿ񱻵��
	 * @return �Ƿ񱻵��
	 */
	bool isHit();
	
	/**
	 * @brief ����˽�б���ֵ
	 * @param which ��������
	 * @param value Ҫ���õ�ֵ
	 */
	void setVariable(unsigned int which,long long value);
	
	/**
	 * @brief ��ȡ˽�б�������
	 * @param which ��������
	 * @return ��������
	 */
	long long& getVariable(unsigned int which);
	
	/**
	 * @brief ����˽�б���ֵ
	 * @param which ��������
	 * @param value ����ֵ
	 */
	void increaseVariable(unsigned int which,long long value);
	
	/**
	 * @brief ����˽�б���ֵ
	 * @param which ��������
	 * @param value ����ֵ
	 */
	void decreaseVariable(unsigned int which,long long value);
	
	/**
	 * @brief ���ͼ��
	 * @param image ͼ��ָ��
	 */
	void addImage(PIMAGE image);
	
	/**
	 * @brief ���õ�ǰͼ��
	 * @param order ͼ������
	 * @return �Ƿ�ɹ�
	 */
	bool setImage(int order);
	
	/**
	 * @brief ��ȡ��ǰͼ������
	 * @return ͼ������
	 */
	int getImageOrder();
	
	/**
	 * @brief ��ȡ��ǰͼ��
	 * @return ͼ��ָ��
	 */
	PIMAGE getImage();
	
	/**
	 * @brief ����Ƿ�����һԪ�ؽӴ�
	 * @param that ��һԪ��ָ��
	 * @return �Ƿ�Ӵ�
	 */
	bool isTouchedBy(Element* that);
	
	/**
	 * @brief ��¡Ԫ��
	 * @return ��Ԫ��ָ��
	 */
	Element* clone();
	
	/**
	 * @brief ��¡��ǰͼ��
	 */
	void cloneImage();
	
	/**
	 * @brief ��ȡԪ��ID
	 * @return ID�ַ���
	 */
	string getId();
	
	/**
	 * @brief ����¼�����
	 * @param listenMode ����ģʽ
	 * @param identifier ��ʶ��
	 * @param function �ص�����
	 */
	virtual void listen(int listenMode,string identifier,function<void(Element*)> function);
	
	/**
	 * @brief �Ƴ��¼�����
	 * @param listenMode ����ģʽ
	 * @param identifier ��ʶ��
	 */
	virtual void stop(int listenMode,string identifier);
	
	/**
	 * @brief ɾ����ǰԪ��
	 * @return Ԫ��ָ��(ͨ��Ϊnullptr)
	 */
	Element* deleteThis();
	
	/**
	 * @brief ȡ��X���ƶ�
	 */
	void cancelX();
	
	/**
	 * @brief ȡ��Y���ƶ�
	 */
	void cancelY();
	
	/**
	 * @brief ȡ�������ƶ�
	 */
	void cancelMove();
	
	/**
	 * @brief ��ȡ͸����
	 * @return ͸����ֵ
	 */
	short getAlpha();
	
	/**
	 * @brief ����͸����
	 * @param alpha �µ�͸����ֵ
	 */
	void setAlpha(short alpha);
	
	/**
	 * @brief ��С͸����
	 * @param alpha ��С��
	 */
	void decreaseAlpha(short alpha);
	
	/**
	 * @brief ����͸����
	 * @param alpha ������
	 */
	void increaseAlpha(short alpha);
	
	/**
	 * @brief �л�����һ��ͼ��
	 */
	void nextImage();
	
	void addPolygon(const FeEGE::Polygon& shape);
	
	#ifndef NO_THREAD
	/**
	 * @brief ����ͼ��
	 * @param imagePath ͼ��·��
	 * @param id ͼ��ID
	 */
	void loadImage(string imagePath,int id);
	
	/**
	 * @brief ɾ��ͼ��
	 * @param id ͼ��ID
	 */
	void deleteImage(int id);
	#endif
	
	/**
	 * @brief ���û��Ƶ�˽��ͼ��
	 */
	void disableDrawToPrivateImage();
	
	/**
	 * @brief ���û��Ƶ�˽��ͼ��
	 */
	void enableDrawToPrivateImage();
	
	/**
	 * @brief ���Ժ�ɫ����(����͸������)
	 */
	void ignoreBlack();
	
	/**
	 * @brief ������ײ����
	 */
	void useGJK();
	
	/**
	 * @brief ������ײ����
	 */
	void stopGJK();
	
	/**
	 * @brief ��ȡ��������� 
	 */
	vector<FeEGE::Polygon> getPolygonSet(); 
	
	/**
	 * @brief ת����������� 
	 */
	FeEGE::Polygon transformPolygon(const FeEGE::Polygon& x);
	
	#ifdef TEST_FUNCTION 
	// ����������ط���
	
	/**
	 * @brief ������������
	 */
	void enablePhysicEngine();
	
	/**
	 * @brief ������������
	 */
	void disablePhysicEngine();
	
	/**
	 * @brief ʩ��������
	 * @param angle ���ĽǶ�
	 * @param force ���Ĵ�С
	 */
	void physicForce(double angle,double force);
	
	/**
	 * @brief ���������ٶ�
	 * @param angle �ٶȽǶ�
	 * @param speed �ٶȴ�С
	 */
	void physicSpeed(double angle,double speed);
	
	/**
	 * @brief ������������
	 * @param angle �����Ƕ�
	 * @param resistance ������С
	 */
	void physicResistance(double angle,double resistance);
	
	/**
	 * @brief ���������ٶ�
	 */
	void updatePhysicSpeed();
	
	/**
	 * @brief ��������λ��
	 */
	void updatePhysicPosition();
	
	/**
	 * @brief ��ȡX���ٶ�
	 * @return X���ٶ�ֵ
	 */
	double getSpeedX();
	
	/**
	 * @brief ��ȡY���ٶ�
	 * @return Y���ٶ�ֵ
	 */
	double getSpeedY();
	
	/**
	 * @brief ����X���ٶ�(ģ���������˶�)
	 */
	void physicArcSpeedX();
	
	/**
	 * @brief ����Y���ٶ�(ģ���������˶�)
	 */
	void physicArcSpeedY();
	
	/**
	 * @brief �Ƴ�X���ٶ�
	 */
	void physicRemoveSpeedX();
	
	/**
	 * @brief �Ƴ�Y���ٶ�
	 */
	void physicRemoveSpeedY();
	
	/**
	 * @brief �Ƴ�X����
	 */
	void physicRemoveForceX();
	
	/**
	 * @brief �Ƴ�Y����
	 */
	void physicRemoveForceY();
	
	/**
	 * @brief ʹ�ö����ƶ�Ԫ��
	 * @param xPixel X���ƶ�����
	 * @param yPixel Y���ƶ�����
	 * @param animate ��������
	 * @param callback ��ɻص�����
	 */
	void moveWithAnimation(double xPixel,double yPixel,const Animate& animate,function<void(Element*)> callback = nullptr);
	#endif
	
	/**
	 * @brief ɾ��Ԫ��
	 * @return Ԫ��ָ��(ͨ��Ϊnullptr)
	 */
	Element* deleteElement();
	
	/**
	 * @brief ��������
	 */
	~Element();
};

/**
 * @class InputBoxSet
 * @brief ����򼯺Ϲ�����
 */
class InputBoxSet{
protected:
    bool isEnabled;             ///< ��ʶ�����Ƿ�������״̬
    set<Element*> childs;       ///< �洢����������Ԫ�ؼ���
    Element* selectedInputBox;  ///< ��ǰ��ѡ�е������ָ��

public:
    /**
     * @brief ���캯������ʼ������򼯺�
     */
    InputBoxSet();

    /**
     * @brief ��������򼯺�
     * @details ���ú󼯺��ڵ�����������Ӧ�����¼�
     */
    void enable();

    /**
     * @brief ��������򼯺�
     * @details ���ú󼯺��ڵ�����򽫲�����Ӧ�����¼�
     */
    void disable();

    /**
     * @brief ��ȡ��������״̬
     * @return true-������ false-�ѽ���
     */
    bool getEnableStatu();

    /**
     * @brief �򼯺�����������
     * @param inputBox Ҫ��ӵ������Ԫ��ָ��
     * @note ���inputBox�Ѵ����ڼ����У��򲻻��ظ����
     */
    void insert(Element* inputBox);

    /**
     * @brief �Ӽ������Ƴ������
     * @param inputBox Ҫ�Ƴ��������Ԫ��ָ��
     * @note ���inputBox�ǵ�ǰѡ�����ͬʱ���ѡ��״̬
     */
    void erase(Element* inputBox);

    /**
     * @brief ѡ��ָ�������
     * @param inputBox Ҫѡ�е������Ԫ��ָ��
     * @details ѡ�к�ᴥ��������onSelect�¼�
     * @note ���inputBox�����ڱ����ϣ��򲻻�ִ��ѡ�в���
     */
    void select(Element* inputBox);

    /**
     * @brief ��ѭ�����ú���
     * @details �������������Ľ����л����߼�
     */
    void call();

    /**
     * @brief ��������
     * @details ���������������������ù�ϵ
     */
    ~InputBoxSet();
};

/**
 * @class InputBox
 * @brief ������࣬�̳���Element
 */
class InputBox : public Element{
protected:
    bool isEnabled;         ///< �ؼ�����״̬��true��ʾ�ɽ�����false��ʾ����
    InputBoxSet* father;    ///< ����������򼯺�ָ�룬���ڹ����������
    Element* submitButton;  ///< �������ύ��ťָ�룬�����ύ�¼�

public:
    /**
     * @brief Ĭ�Ϲ��캯��
     * @details ��ʼ�������Ĭ��״̬Ϊ���ã��޸����Ϻ��ύ��ť
     */
    InputBox();

    /**
     * @brief ���ι��캯��
     * @param father ����������򼯺�
     * @param submitButton �������ύ��ť
     */
    InputBox(InputBoxSet* father, Element* submitButton);

    /**
     * @brief �󶨵�����򼯺�
     * @param father Ҫ�󶨵�����򼯺�
     * @note һ�������ֻ������һ������
     */
    void bind(InputBoxSet* father);

    /**
     * @brief ���������
     * @details ��������û�����ͽ����¼�
     */
    void enable();

    /**
     * @brief ���������
     * @details ��ֹ�����û�����ͽ����¼�
     */
    void disable();

    /**
     * @brief ��ȡ����״̬
     * @return bool ��ǰ����״̬
     * @retval true ������
     * @retval false �ѽ���
     */
    bool getEnableStatu();

    /**
     * @brief ����¼�������
     * @param listenMode ����ģʽ(��������ͣ��)
     * @param identifier ������Ψһ��ʶ
     * @param function �ص�����
     * @override ��дElement���listen����
     */
    virtual void listen(int listenMode, string identifier, 
                       function<void(Element*)> function);

    /**
     * @brief �Ƴ��¼�������
     * @param listenMode ����ģʽ
     * @param identifier Ҫ�Ƴ��ļ�������ʶ
     * @override ��дElement���stop����
     */
    virtual void stop(int listenMode, string identifier);

    /**
     * @brief ˢ�����״̬
     * @details �����꽻��������״̬���������л�
     * @override ��дElement���reflushMouseStatu����
     */
    void reflushMouseStatu() override;

    /**
     * @brief ��ѭ��������
     * @details ����������֡�����߼����¼�����
     * @override ��дElement���call����
     */
    void call() override;

    /**
     * @brief ��������
     * @details ������Դ���Զ��Ӹ��������Ƴ�
     */
    ~InputBox();
};

/**
 * @class Button
 * @brief ��ť�࣬�̳���Element
 */
class Button : public Element{
protected:
    bool isEnabled;  ///< ��ť����״̬��־��true��ʾ���ã�false��ʾ����
    
public:
    /**
     * @brief ���캯��
     * @details ��ʼ����ťԪ�أ�Ĭ��״̬Ϊ����
     */
    Button();
    
    /**
     * @brief ���ð�ť
     * @details ���ð�ťΪ����״̬��ʹ�������Ӧ�����¼�
     */
    void enable();
    
    /**
     * @brief ���ð�ť  
     * @details ���ð�ťΪ����״̬��ʹ�䲻����Ӧ�����¼�
     */
    void disable();
    
    /**
     * @brief ��ȡ��ť����״̬
     * @return bool ���ص�ǰ����״̬��true��ʾ���ã�false��ʾ����
     */
    bool getEnableStatu();
    
    /**
     * @brief ˢ�����״̬����д���෽����
     * @details �������Ƿ���ͣ�ڰ�ť�ϣ���������Ӧ��״̬��־
     * @override ��дElement���reflushMouseStatu����
     */
    void reflushMouseStatu() override;
    
    /**
     * @brief ��ť��ѭ��������д���෽����
     * @details ����ť��֡�����߼�������״̬�����¼�����
     * @override ��дElement���call����  
     */
    void call() override;
    
    /**
     * @brief ��������
     * @details ����ťռ�õ���Դ
     */
    ~Button();
};

// ȫ�ֱ�������
extern int currentRegOrder;                  ///< ��ǰע��˳��
extern unsigned long long frame;             ///< ֡������
extern queue<Element*> freeList;             ///< ����Ԫ���б�

/**
 * @class cmp
 * @brief Ԫ�رȽ��࣬��������
 */
class cmp {
public:
    bool operator()(Element* _A,Element* _B);  ///< �Ƚ����������
};

extern set<Element*,cmp> elementQueue;  ///< Ԫ�ض���

/**
 * @namespace pen
 * @brief ������ع��������ռ�
 */
namespace pen {
	
extern int order;           ///< ����˳��
extern int fontScale;       ///< ��������
extern short penAlpha;      ///< ����͸����
extern int penType;         ///< ��������
extern int charWidth, charHeight;  ///< �ַ���Ⱥ͸߶�

/**
     * @brief ��ָ��λ�ô�ӡ�ַ���(ANSI)
     * @param x ��ʼx����
     * @param y ��ʼy����
     * @param str Ҫ��ӡ���ַ���
     */
    void print(int x, int y, string str);

    /**
     * @brief ��ָ��λ�ô�ӡ���ַ���(Unicode)
     * @param x ��ʼx����
     * @param y ��ʼy����
     * @param str Ҫ��ӡ�Ŀ��ַ���
     */
    void print(int x, int y, wstring str);

    /**
     * @brief ����������ʽ�ʹ�С
     * @param scale �������ű���
     * @param fontName ��������(Ĭ��Ϊ"��Բ")
     */
    void font(int scale, string fontName = "��Բ");

    /**
     * @brief ���û�����ɫ
     * @param color ��ɫֵ(color_t����)
     */
    void color(color_t color);

    /**
     * @brief ���û�������
     * @param type ��������(0x01-���, 0x02-�м���)
     */
    void type(int type);

    /**
     * @brief ���ָ����������
     * @param x ���Ͻ�x����
     * @param y ���Ͻ�y����
     * @param ex ���½�x����
     * @param ey ���½�y����
     */
    void clear(int x, int y, int ex, int ey);

    /**
     * @brief ���ָ��λ�õĵ����ַ�
     * @param x �ַ�x����
     * @param y �ַ�y����
     */
    void clearChar(int x, int y);

    /**
     * @brief �����ָ��λ�ÿ�ʼ�Ķ���ַ�
     * @param x ��ʼx����
     * @param y ��ʼy����
     * @param charCount Ҫ������ַ�����
     */
    void clearChars(int x, int y, int charCount);

    /**
     * @brief �����������
     */
    void clearAll();

    /**
     * @brief ���û���˳��
     * @param value ˳��ֵ(��ֵԽ��Խ�����)
     */
    void setOrder(int value);

    /**
     * @brief ���û���͸����
     * @param alpha ͸����ֵ(0-255)
     */
    void setAlpha(short alpha);

    /**
     * @brief ��ȡ��ǰ����͸����
     * @return ��ǰ͸����ֵ(0-255)
     */
    short getAlpha();

    /**
     * @brief ���ӻ���͸����
     * @param alpha Ҫ���ӵ�͸����ֵ
     */
    void increaseAlpha(short alpha);

    /**
     * @brief ���ͻ���͸����
     * @param alpha Ҫ���͵�͸����ֵ
     */
    void decreaseAlpha(short alpha);

    /**
     * @brief ����ֱ��
     * @param x1 ���x����
     * @param y1 ���y����
     * @param x2 �յ�x����
     * @param y2 �յ�y����
     */
    void printLine(int x1, int y1, int x2, int y2);

    /**
     * @brief ���ƾ�����
     * @param left ��߽�
     * @param top �ϱ߽�
     * @param right �ұ߽�
     * @param bottom �±߽�
     */
    void printBar(int left, int top, int right, int bottom);

}

// ��������
/**
 * @brief ˢ����������
 * @details ǿ���ػ�����Ԫ�أ�������ʾ����
 */
void reflush();

/**
 * @brief ��ʼ��ͼ�δ���
 * @param x ���ڿ��
 * @param y ���ڸ߶�
 * @param mode ��ʼ��ģʽ(Ĭ��ΪINIT_RENDERMANUAL�ֶ���Ⱦģʽ)
 */
void init(int x, int y, int mode = INIT_RENDERMANUAL);

/**
 * @brief �������¼�ѭ��
 * @details ��ʼ���������¼�����Ⱦѭ��
 */
void start();

/**
 * @brief ע��ȫ���¼�����
 * @param listenMode ����ģʽ(��֡�¼�������¼���)
 * @param identifier ������Ψһ��ʶ��
 * @param function �ص�����
 */
void globalListen(int listenMode, string identifier, auto function);

/**
 * @brief ȡ��ȫ���¼�����
 * @param listenMode ����ģʽ
 * @param identifier Ҫ�Ƴ��ļ�������ʶ��
 */
void stopGlobalListen(int listenMode, string identifier);

/**
 * @brief �����°�ťԪ��
 * @param id ��ťΨһID
 * @param imagePath ��ťͼ��·��
 * @param x x����λ��
 * @param y y����λ��
 * @param putOn �����ͣ�ص�(��ѡ)
 * @param moveAway ����ƿ��ص�(��ѡ)
 * @param on_click ����ص�(��ѡ)
 * @return �����İ�ťָ��
 */
Button* newButton(string id, string imagePath, double x, double y,
                 function<void(Element*)> putOn = nullptr,
                 function<void(Element*)> moveAway = nullptr,
                 function<void(Element*)> on_click = nullptr);

/**
 * @brief �����������Ԫ��
 * @param id �����ΨһID
 * @param imagePath �����ͼ��·��
 * @param x x����λ��
 * @param y y����λ��
 * @param submitButton �������ύ��ť
 * @param onSubmit �ύ�ص�(��ѡ)
 * @param onSelected ѡ�лص�(��ѡ)
 * @return �����������ָ��
 */
Element* newInputBox(string id, string imagePath, double x, double y,
                   Element* submitButton,
                   function<void(Element*)> onSubmit = nullptr,
                   function<void(Element*)> onSelected = nullptr);

/**
 * @brief ��ȡ��ǰ����Ŀ¼·��
 * @return ��ǰ·���ַ���
 */
string getCurrentPath();

/**
 * @brief ����·���ַ���
 * @param s ԭʼ·���ַ���
 * @return ������ı�׼·��
 */
string resolvePath(string s);

/**
 * @brief ����Ƿ�Ϊ��ЧUTF-8����
 * @param fileContent �ļ�����
 * @return �Ƿ���ЧUTF-8����
 */
bool isValidUTF8(const string& fileContent);

/**
 * @brief ����ı������ʽ
 * @param fileContent �ļ�����
 * @return ���������ַ���
 */
std::string detectEncoding(const std::string& fileContent);

/**
 * @brief UTF-8תANSI����
 * @param utf8Str UTF-8�ַ���
 * @return ת�����ANSI�ַ���
 */
std::string UTF8ToANSI(const std::string& utf8Str);

/**
 * @brief ִ������ȴ����
 * @param cmd Ҫִ�е������ַ���
 */
void runCmdAwait(const std::string cmd);
#endif
