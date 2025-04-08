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
#include <locale>
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
double globalRating = 1.00f;

class Position;
class Element;
class InputBox;
class InputBoxSet;
void regElement(Element* element);
Element* newElement(string,PIMAGE,double,double);
Element* newElement(string,string,double,double);

bool isKey = false;
key_msg keyMsg;
bool mouseHit = false;
bool needSort = true;
bool regPause = false;
mouse_msg mouseInfo;
bool keyStatus[360];
map<string,function<void(void)>> globalListenFrameFunctionSet;
map<string,function<void(void)>> globalListenOnClickFunctionSet;
map<int,bool> vkState;

int WIDTH;
int HEIGHT;

vector<function<void(void)> > schedule;
map<double,vector<function<void(void)> > > scheduleTimeOut;
int __SIZE__ = 0;
int removeSize = 0;
bool closeGraph = false;

PIMAGE pen_image;

unordered_map<string,Element*>idToElement;
unordered_map<Element*,bool>elementIsIn;

inline int sgn(double x){
	if(fabs(x) < eps) return 0;
	return x > 0 ? 1 : -1;
}

class Position {
	public:
		//Variables
		double x;
		double y;

		// Functions
		Position(double x = 0,double y = 0) {
			this->x = x;
			this->y = y;
		}
		~Position() {}
} lastPixel,mousePos;

namespace FeEGE {
	#ifndef bigScreen
	bool builtinGetKey(){
		for(int i = 0;i <= 0xFF;++ i){
			vkState[i] = GetAsyncKeyState(i) & 0x8000;
		}
	}
	bool getkey(int VK) {
		return vkState[VK];
	}
	#endif
	void enablePause() {
		regPause = true;
	}
	void disablePause() {
		regPause = false;
	}
	Element* getElementById(string);
	Element* getElementByPtr(Element*);
	void initPen() {
		WIDTH = getwidth();
		HEIGHT = getheight();
		if(pen_image != nullptr) delimage(pen_image);
		pen_image = newimage(WIDTH,HEIGHT);
		setbkcolor(EGERGBA(1,1,4,0),pen_image);
		setcolor(EGERGB(255,0,0),pen_image);
		setfillcolor(EGERGBA(1,1,4,0),pen_image);
	}
	
	double getMs(){
		return (double)clock() / CLOCKS_PER_SEC * 1000;
	}
	
	void pushSchedule(function<void(void)> func) {
		schedule.push_back(func);
	}
	
	void setTimeOut(function<void(void)> func,double timeMs){
		scheduleTimeOut[getMs() + timeMs].emplace_back(func);
	}

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

	Events EventType;

	class PenTypes {
		public:
			int left = 0x01;
			int middle = 0x02;
	};

	PenTypes penType;

	Position& getLastPixel() {
		return lastPixel;
	}
	
	Position& getMousePos(){
		return mousePos;
	}
}
using namespace FeEGE;

class Animate{
	public:
		function<Position(double)> function;
		double speed;
		double end;
		
		Animate(std::function<Position(double)> function,double speed,double end): function(function),speed(speed),end(end) {	};
};

namespace Ease{
	Animate easeInOutCubicX([&](double tick) -> Position { return {tick <= 0.5 ? 4 * tick * tick * tick : 1 - pow(-2 * tick + 2,3) / 2,0}; },0.002,1);
	Animate easeInOutCubicY([&](double tick) -> Position { return {0,tick <= 0.5 ? 4 * tick * tick * tick : 1 - pow(-2 * tick + 2,3) / 2}; },0.002,1);
	Animate easeOutQuadX([&](double tick) -> Position { return {1 - (1 - tick) * (1 - tick),0}; },0.002,1);
	Animate easeOutQuadY([&](double tick) -> Position { return {0,1 - (1 - tick) * (1 - tick)}; },0.002,1);
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

		inline virtual void reflushMouseStatu() {
			/*
				Test click
			*/
			if(this->isMouseIn()){
				int statu = this->getVariable(0);
				if(this->isHit()){
					#ifdef bigScreen
					if(statu == 0){
						this->setVariable(0,2);
						for(auto it :this->onMouseHittingFunctionSet) it.second(this);
					}
					#endif
					
					if(statu == 1){
						this->setVariable(0,2);
						for(auto it :this->onMouseHittingFunctionSet) it.second(this);
					}
				} 
				else {
					if(statu == 0){
						this->setVariable(0,1);
						for(auto it : this->onMousePutOnFunctionSet) it.second(this);
					} 
					else if(statu == 2) {
						this->setVariable(0,0);
						for(auto it : this->onClickFunctionSet) it.second(this);
					}
				}
			} 
			else {
				if(this->getVariable(0) == 1){
					this->setVariable(0,0);
					for(auto it : this->onMouseMoveAwayFunctionSet) it.second(this);
				}
				else if(this->getVariable(0) == 2 && !getkey(leftButton)){
					this->setVariable(0,0);
					for(auto it : this->onMouseMoveAwayFunctionSet) it.second(this);
				}
			}
		}
		inline bool drawToPrivateImage() {
			if(this->drawed) return false;
			if(this->deleted) return false;
			if(this->disabledDrawToPrivateImage) return false;
			if(this->imageVector[this->currentImage] == nullptr) return false;
			cleardevice(this->__visibleImage);
			putimage_rotatezoom(this->__visibleImage,this->imageVector[this->currentImage],this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f * pie,this->scale / 100.00f,1);
			return this->drawed = true;
		}
	public:
		//Functions
		Element(string id,PIMAGE image,Position pos) {
			this->id = id;
			this->__visibleImage = newimage(getwidth(),getheight());
			setbkcolor(EGERGBA(1,1,4,0),this->__visibleImage);
			this->pos = pos;
			this->imageVector.resize(100);
			this->imageVector[0] = image;
			this->isShow = true;
			this->scale = 100;
			this->angle = 0;
			this->order = 0;
			this->alpha = 0xFF;
			this->deleted= false;
			this->physicEngineStatu = false;
			this->forceX = 0.00;
			this->forceY = 0.00;
			this->speedX = 0.00;
			this->speedY = 0.00;
			this->resistanceX = 0.00;
			this->resistanceY = 0.00;
			for(int i = 0; i < 10; ++ i) this->privateVariables[i] = 0;
		}
		Element(string id,PIMAGE image,double x = 0,double y = 0) {
			this->id = id;
			this->__visibleImage = newimage(getwidth(),getheight());
			setbkcolor(EGERGBA(1,1,4,0),this->__visibleImage);
			this->pos = *(new Position(x,y));
			this->imageVector.resize(100);
			this->imageVector[0] = image;
			this->isShow = true;
			this->scale = 100;
			this->angle = 0;
			this->order = 0;
			this->alpha = 0xFF;
			this->deleted= false;
			this->physicEngineStatu = false;
			this->forceX = 0.00;
			this->forceY = 0.00;
			this->speedX = 0.00;
			this->speedY = 0.00;
			for(int i = 0; i < 10; ++ i) this->privateVariables[i] = 0;
		}
		Element() { }
		inline Element* copy(string id,PIMAGE image,unsigned long long index,double x = 0,double y = 0) {
			this->id = id;
			this->__visibleImage = newimage(getwidth(),getheight());
			setbkcolor(EGERGBA(1,1,4,0),this->__visibleImage);
			this->pos.x = x;
			this->pos.y = y;
			this->imageVector.resize(100);
			this->imageVector[0] = image;
			this->isShow = true;
			this->scale = 100;
			this->angle = 0;
			this->order = 0;
			this->alpha = 0xFF;
			this->deleted= false;
			this->poolIndex = index;
			this->physicEngineStatu = false;
			this->forceX = 0.00;
			this->forceY = 0.00;
			this->speedX = 0.00;
			this->speedY = 0.00;
			return this;
		}
		
		inline void cancelAnimate(const Animate& animate,double tick,double x,double y){
			Position p = animate.function(tick);
			this->pos.x -= p.x * x;
			this->pos.y -= p.y * y;
		}
		
		inline void runAnimate(const Animate& animate,double tick,double x,double y){
			Position p = animate.function(tick);
			this->pos.x += p.x * x;
			this->pos.y += p.y * y;
		}
		
		inline void callAnimations(){
			vector<pair<pair<double,pair<double,double> >,Animate> > q;
			for(auto& p : this->animations){
				int w = p.first.first;
				Animate& animate = p.second;
				cancelAnimate(animate,this->animateStates[w],p.first.second.first,p.first.second.second);
				this->animateStates[w] += animate.speed;
				if(this->animateStates[w] >= animate.end){
					runAnimate(animate,animate.end,p.first.second.first,p.first.second.second);
					this->animateStates.erase(w);
					auto it = this->animateCallbacks.find(w);
					if(it == this->animateCallbacks.end()) continue;
					it->second(this);
					this->animateCallbacks.erase(it);
					continue;
				}
				runAnimate(animate,this->animateStates[w],p.first.second.first,p.first.second.second);
				q.emplace_back(p);
			}
			this->animations.swap(q);
		}
		
		inline virtual void call() {
			this->backupPos = pos;
			this->reflushMouseStatu();
			for(auto it : this->frameFunctionSet) it.second(this);
			if(this->deleted) return;
			this->callAnimations();
			if(!this->isShow) return;

			//backup
			if(this->isCancelX) {
				this->isCancelX = false;
				this->pos.x = this->backupPos.x;
			}
			if(this->isCancelY) {
				this->isCancelY = false;
				this->pos.y = this->backupPos.y;
			}
			
			this->drawed = false;
#ifdef TEST_FUNCTION
			if(this->physicEngineStatu) {
				this->updatePhysicSpeed();
				this->updatePhysicPosition();
			}
#endif
			putimage_rotatezoom(nullptr,this->imageVector[this->currentImage],this->pos.x,this->pos.y,0.5,0.5,-(this->angle / 180.00f * pie),this->scale / 100.00f,1,this->alpha);
		}
		inline void setType(const string& type){
			this->elementType = type;
		}
		inline string getType(const string& type){
			return this->elementType;
		}
		inline void moveLeft(double pixels = 0) {
			this->pos.x -= pixels;
		}
		inline void moveRight(double pixels = 0) {
			this->pos.x += pixels;
		}
		inline void moveUp(double pixels = 0) {
			this->pos.y -= pixels;
		}
		inline void moveDown(double pixels = 0) {
			this->pos.y += pixels;
		}
		inline void moveForward(double pixels = 0) {
			this->pos.x -= sin(this->angle * pie / 180.00f) * pixels;
			this->pos.y -= cos(this->angle * pie / 180.00f) * pixels;
		}
		inline void moveTo(double x,double y) {
			this->pos.x = x;
			this->pos.y = y;
		}
		inline void moveTo(Position position) {
			this->pos = position;
		}
		inline color_t getPixel(int x,int y) {
			return getpixel(x,y,this->__visibleImage);
		}
		inline short getScale() {
			return this->scale;
		}
		inline void increaseScale(short scale) {
			if(this->scale + scale <= 255) this->scale += scale;
			else this->scale = 255;
			this->scale %= 255;
			if(this->scale < 0) this->scale += 255;
		}
		inline void decreaseScale(short scale) {
			if(this->scale > scale) this->scale -= scale;
			else this->scale = 0;
			this->scale %= 255;
			if(this->scale < 0) this->scale += 101;
		}
		inline void setScale(short scale) {
			this->scale = scale;
			this->scale %= 255;
		}
		inline void show() {
			this->isShow = true;
		}
		inline void hide() {
			this->isShow = false;
		}
		inline void turnRight(int angle) {
			this->angle = (this->angle - angle) % 360;
		}
		inline void turnLeft(int angle) {
			this->angle = (this->angle + angle) % 360;
		}
		inline void turnTo(int angle) {
			this->angle = angle % 360;
		}
		inline bool faceTo(Element* that){
			if(that == nullptr) {
				LPCSTR text = TEXT(("Element::faceTo方法被错误的传入了nullptr参数\n这可能是由于getElementById查询了不存在的对象\n\nElement名称 : " + this->id).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
				return false; 
			}
			Position pos = that->getPosition();
			double dx = pos.x - this->pos.x;
			double dy = pos.y - this->pos.y;
			if(!sgn(dx)) return true;
			if(!sgn(dy)){
				if(dx > 0) this->angle = 0;
				else this->angle = 180;
				return true;
			}
			this->angle = atan2(dy,dx) / pie * 180.00f;
			return true;
		}
		inline int getAngle() {
			return this->angle;
		}
		inline Position getPosition() {
			return this->pos;
		}
		inline bool getIsShow() {
			return this->isShow;
		}
		inline void increaseOrder(int count);
		inline void decreaseOrder(int count);
		inline void setOrder(int count);
		inline void setRegOrder(int count) {
			this->regOrder = count;
		}
		inline int getRegOrder() {
			return this->regOrder;
		}
		inline int getOrder() {
			return this->order;
		}
		bool operator<(Element _B) {
			if(this->getOrder() > _B.getOrder()) return false;
			else if (this->getOrder() < _B.getOrder()) return true;
			return this->getRegOrder() < _B.getRegOrder();
		}
		bool operator>(Element _B) {
			if(this->getOrder() < _B.getOrder()) return false;
			else if (this->getOrder() > _B.getOrder()) return true;
			return this->getRegOrder() > _B.getRegOrder();
		}
		inline void setPosX(int x) {
			this->pos.x = x;
		}
		inline void setPosY(int y) {
			this->pos.y = y;
		}
		inline void removeColor(color_t color) {
			this->removeColors.push_back(color);
		}
		inline bool isMouseIn() {
			if(GetForegroundWindow() != getHWnd()) return false;
			if(!this->isShow) return false;
			int x = getMousePos().x,y = getMousePos().y;
			if(x < 0 || y < 0 || x > WIDTH || y > HEIGHT) return false;
			this->drawToPrivateImage();
			color_t pixel = getpixel(x,y,this->__visibleImage);
			int size = removeColors.size();
			for(int i = 0; i < size; ++ i) if(pixel == removeColors[i]) return false;
			return ((EGEGET_A(pixel) != 0) || (pixel != 65796)); //EGERGBA(1,1,4,0) = 65796
		}
		inline bool isHit() {
			if(!getkey(leftButton)) return false;
			return this->isMouseIn();
		}
		inline void setVariable(unsigned int which,long long value) {
			this->privateVariables[which] = value;
//			// cout<<which<<" "<<value<<" "<<this<<" "<<this->id<<endl;
		}
		inline long long& getVariable(unsigned int which) {
			return this->privateVariables[which];
		}
		inline void increaseVariable(unsigned int which,long long value) {
			this->privateVariables[which] += value;
		}
		inline void decreaseVariable(unsigned int which,long long value) {
			this->privateVariables[which] -= value;
		}
		inline void addImage(PIMAGE image) {
			this->imageVector.push_back(image);
		}
		inline bool setImage(int order) {
			if(this->imageVector[order] != nullptr){
				this->currentImage = order;
				return true;
			}
			return false;
		}
		inline int getImageOrder() {
			return this->currentImage;
		}
		inline PIMAGE getImage() {
			return this->imageVector[this->currentImage];
		}
		inline bool isTouchedBy(Element* that) {
			if(that == nullptr) {
				LPCSTR text = TEXT(("Element::isTouchedBy方法被错误的传入了nullptr参数\n这可能是由于getElementById查询了不存在的对象\n\nElement名称 : " + this->id).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
				return false; 
			}
			if(!this->isShow || !that->isShow) return false;
			
			if(this->hittingBox ^ that->hittingBox){
				LPCSTR text = TEXT(("Element::isTouchedBy方法中两个对象的碰撞箱状态不一致\n\nElement名称 : " + this->id).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
				return false;
			}
			if(this->hittingBox){
				double thisLeft = this->pos.x - (this->HBwidth >> 1) * this->scale / 100.00;
				double thatLeft = that->pos.x - (that->HBwidth >> 1) * that->scale / 100.00;
				double thisRight = this->pos.x + (this->HBwidth >> 1) * this->scale / 100.00;
				double thatRight = that->pos.x + (that->HBwidth >> 1) * that->scale / 100.00;
				double thisTop = this->pos.y - (this->HBheight >> 1) * this->scale / 100.00;
				double thatTop = that->pos.y - (that->HBheight >> 1) * that->scale / 100.00;
				double thisBottom = this->pos.y + (this->HBheight >> 1) * this->scale / 100.00;
				double thatBottom = that->pos.y + (that->HBheight >> 1) * that->scale / 100.00;
				if((thisLeft <= thatRight && (thisTop <= thatBottom && thisBottom >= thatTop)) || (thisRight >= thatLeft && (thisTop <= thatBottom && thisBottom >= thatTop))) return true;
				return false;
			}
			
			this->drawToPrivateImage();
			for(int x = this->pos.x - getwidth(this->imageVector[this->currentImage]); x <= this->pos.x + getwidth(this->imageVector[this->currentImage]); ++ x) {
				for(int y = this->pos.y - getheight(this->imageVector[this->currentImage]); y <= this->pos.y + getheight(this->imageVector[this->currentImage]); ++ y) {
					if(x < 0 || y < 0 || x >= getwidth() || y >= getheight()) continue;
					if(getpixel(x,y,this->__visibleImage) == 65796) continue;
					if(getpixel(x,y,that->__visibleImage) == 65796) continue;
					lastPixel = {(double)x,(double)y};
					return true;
				}
			}
		}
		inline Element* clone() {
			static Element* e[MAXCLONESCOUNT];
			for(int i = 0; i < nextCloneCount; ++ i) {
				if(e[i] == nullptr) continue;
				if(!this->deletedList[i]) continue;
				int size = this->removeList.size();
				for(int j = 0; j < size; ++ j) {
					if(this->deletedList[i] && (e[i] == this->removeList[j] || FeEGE::getElementByPtr(e[i]) == nullptr)) {
						this->deletedList[i] = false;
						e[i] = nullptr;
					}
				}
			}
			this->removeList.clear();
			for(int i = 0; i < nextCloneCount; ++ i) if(!this->deletedList[i]) {
					deletedList[i] = true;
					e[i] = newElement(this->id + "_" + to_string(cloneCount ++),"",this->pos.x,this->pos.y);
					e[i]->angle = this->angle;
					e[i]->scale = this->scale;
					e[i]->setVariable(1,(long long)this);
					e[i]->disabledDrawToPrivateImage = this->disabledDrawToPrivateImage;
					e[i]->isShow = this->isShow;
					e[i]->imageVector = this->imageVector;
					regElement(e[i]);
					for(auto it : this->onCloneFunctionSet) it.second(this);
					for(auto it : this->onCloneClonesFunctionSet) it.second(e[i]);
					return e[i];
				}
			deletedList[nextCloneCount] = true;
			e[nextCloneCount] = newElement(this->id + "_" + to_string(cloneCount ++),"",this->pos.x,this->pos.y);
			e[nextCloneCount]->imageVector = this->imageVector;
			e[nextCloneCount]->angle = this->angle;
			e[nextCloneCount]->scale = this->scale;
			e[nextCloneCount]->setVariable(1,(long long)this);
			e[nextCloneCount]->isShow = this->isShow;
			regElement(e[nextCloneCount]);
			for(auto it : this->onCloneFunctionSet) it.second(this);
			for(auto it : this->onCloneClonesFunctionSet) it.second(e[nextCloneCount]);
			return e[nextCloneCount ++];
		}
		inline void cloneImage(){
			for(int i = 0;i < this->imageVector.size();++ i){
				if(this->imageVector[i] == nullptr) continue;
				PIMAGE img = newimage();
				getimage(img,this->imageVector[i],0,0,getwidth(this->imageVector[i]),getheight(this->imageVector[i]));
				this->imageVector[i] = img;
			}
		}
		inline string getId() {
			return this->id;
		}
		inline virtual void listen(int listenMode,string identifier,function<void(Element*)> function) {
			if(listenMode == FeEGE::EventType.frame) this->frameFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.on_mouse_put_on) this->onMousePutOnFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.on_mouse_hitting) this->onMouseHittingFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.on_mouse_move_away) this->onMouseMoveAwayFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.on_click) this->onClickFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.on_clone) this->onCloneFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.clones.on_clone) this->onCloneClonesFunctionSet[identifier] = function ;
			else{
				LPCSTR text = TEXT(("Element::listen方法中被传入了不恰当的事件\n\nElement名称 : " + this->id + "    事件id ：" + to_string(listenMode)).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
			}
		}
		inline virtual void stop(int listenMode,string identifier) {
			if(listenMode == FeEGE::EventType.frame) this->frameFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.on_mouse_put_on) this->onMousePutOnFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.on_mouse_hitting) this->onMouseHittingFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.on_mouse_move_away) this->onMouseMoveAwayFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.on_click) this->onClickFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.on_clone) this->onCloneFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.clones.on_clone) this->onCloneClonesFunctionSet.erase(identifier) ;
			else{
				LPCSTR text = TEXT(("Element::stop方法中被传入了不恰当的事件\n\nElement名称 : " + this->id + "    事件id ：" + to_string(listenMode)).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
			}
		}
		inline Element* deleteThis();
		inline void cancelX() {
			this->isCancelX = true;
		}
		inline void cancelY() {
			this->isCancelY = true;
		}
		inline void cancelMove() {
			this->isCancelX = true;
			this->isCancelY = true;
		}
		inline short getAlpha() {
			return this->alpha;
		}
		inline void setAlpha(short alpha) {
			this->alpha = alpha;
			this->alpha %= 256;
			if(this->alpha < 0) this->alpha += 256;
		}
		inline void decreaseAlpha(short alpha) {
			if(this->alpha - alpha < 0) this->alpha = 0;
			else this->alpha -= alpha;
			this->alpha %= 256;
			if(this->alpha < 0) this->alpha += 256;
		}
		inline void increaseAlpha(short alpha) {
			if(this->alpha + alpha > 255) this->alpha = 255;
			else this->alpha += alpha;
			this->alpha %= 256;
			if(this->alpha < 0) this->alpha += 256;
		}
		inline void nextImage() {
			do {
				this->currentImage ++;
				this->currentImage %= 100;
			} while(this->imageVector[currentImage] == nullptr);
		}
#ifndef NO_THREAD
		inline void loadImage(string imagePath,int id) {
			thread([this,imagePath,id]() {
				PIMAGE image = newimage();
				int statu = 0;
				do {
					statu = getimage(image,TEXT(imagePath.c_str()));
					this_thread::sleep_for(chrono::milliseconds(10));
				} while(statu != 0);
				while(this->imageLock) this_thread::sleep_for(chrono::milliseconds(1));
				this->imageLock = true;
				this->imageVector[id] = image;
				this->imageLock = false;
			}).detach();
		}
		inline void deleteImage(int id) {
			thread([this,id]() {
				delimage(this->imageVector[id]);
			}).detach();
		}
#endif
		inline void disableDrawToPrivateImage() {
			this->disabledDrawToPrivateImage = true;
		}
		inline void enableDrawToPrivateImage() {
			this->disabledDrawToPrivateImage = false;
		}
		inline void ignoreBlack() {
			int XX = getwidth(this->imageVector[this->currentImage]);
			int YY = getheight(this->imageVector[this->currentImage]);
			for(int i = 0; i < XX; ++ i) {
				for(int j = 0; j < YY; ++ j) {
					if(getpixel(i,j,this->imageVector[this->currentImage]) == (color_t)BLACK) putpixel_f(i,j,EGERGBA(0,0,0,0),this->imageVector[this->currentImage]);
				}
			}
		}
#ifdef TEST_FUNCTION 
		inline void useHittingBox(){
			this->hittingBox = true;
		} 
		inline void stopHittingBox(){
			this->hittingBox = false;
		}

		// PhysicEngine
		inline void enablePhysicEngine() {
			this->physicEngineStatu = true;
		}
		inline void disablePhysicEngine() {
			this->physicEngineStatu = false;
		}
		inline void physicForce(double angle,double force) {
			this->forceX += cos(angle * pie / 180.00) * force;
			this->forceY -= sin(angle * pie / 180.00) * force;
		}
		inline void physicSpeed(double angle,double speed) {
			this->speedX += cos(angle * pie / 180.00) * speed;
			this->speedY -= sin(angle * pie / 180.00) * speed;
		}
		inline void physicResistance(double angle,double resistance) {
			this->resistanceX += cos(angle * pie / 180.00) * resistance;
			this->resistanceY -= sin(angle * pie / 180.00) * resistance;
		}
		inline void updatePhysicSpeed() {
			this->speedX += forceX;
			this->speedY += forceY;
			bool XX = this->speedX * resistanceX > 0;
			bool YY = this->speedY * resistanceY > 0;
			if(XX && fabs(this->speedX) >= fabs(this->resistanceX)) this->speedX -= this->resistanceX;
			else if(XX)this->speedX = 0;
			if(YY && fabs(this->speedY) >= fabs(this->resistanceY)) this->speedY -= this->resistanceY;
			else if(YY) this->speedY = 0;
		}
		inline void updatePhysicPosition() {
			this->pos.x += speedX * globalRating;
			this->pos.y += speedY * globalRating;
//			cout<<"updated\n";
		}
		inline double getSpeedX() {
			return this->speedX;
		}
		inline double getSpeedY() {
			return this->speedY;
		}
		inline void physicArcSpeedX() {
			this->speedX = -this->speedX;
		}
		inline void physicArcSpeedY() {
			this->speedY = -this->speedY;
		}
		inline void physicRemoveSpeedX() {
			this->speedX = 0;
		}
		inline void physicRemoveSpeedY() {
			this->speedY = 0;
		}
		inline void physicRemoveForceX() {
			this->forceX = 0;
		}
		inline void physicRemoveForceY() {
			this->forceY = 0;
		}
		
		// Move Animated
		inline void moveWithAnimation(double xPixel,double yPixel,const Animate& animate,function<void(Element*)> callback = nullptr){
			static int cur = 0;
			animateStates[++ cur] = 0;
			Position p = animate.function(animate.end);
			animations.emplace_back(make_pair(make_pair(cur,make_pair((p.x == 0 ? 0 : xPixel / p.x),(p.y == 0 ? 0 : yPixel / p.y))),animate));
			if(callback == nullptr) return;
			this->animateCallbacks[cur] = callback;
		}
#endif

		inline Element* deleteElement();
		~Element() { };
};

class InputBoxSet{
	protected:
		bool isEnabled;
		set<Element*> childs;
		Element* selectedInputBox;
		
	public:
		InputBoxSet(){
			this->isEnabled = false;
		}
		inline void enable(){
			this->isEnabled = true;
		}
		inline void disable(){
			this->isEnabled = false;
		}
		inline bool getEnableStatu(){
			return this->isEnabled;
		}
		inline void insert(Element* inputBox){
			this->childs.insert(inputBox);
		}
		inline void erase(Element* inputBox){
			this->childs.erase(inputBox); 
		}
		inline void select(Element* inputBox){
			
		}
		inline void call(){
			for(Element* inputBox : childs){
				inputBox->call();
			}
		}
		~InputBoxSet(){	}
};

class InputBox : public Element{
	protected:
		bool isEnabled;
		InputBoxSet* father;
		Element* submitButton;
	public:
		InputBox(){
			this->father = nullptr;
			this->submitButton = nullptr;
		}
		InputBox(InputBoxSet* father,Element* submitButton): father(father),submitButton(submitButton) { };
		inline void bind(InputBoxSet* father){
			if(this->father != nullptr) this->father->erase(this);
			this->father = father;
		}
		inline void enable(){
			this->isEnabled = true;
		}
		inline void disable(){
			this->isEnabled = false;
		}
		inline bool getEnableStatu(){
			return this->isEnabled;
		}
		inline virtual void listen(int listenMode,string identifier,function<void(Element*)> function) {
			if(listenMode == FeEGE::EventType.frame) this->frameFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.on_mouse_put_on) this->onMousePutOnFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.on_mouse_hitting) this->onMouseHittingFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.on_mouse_move_away) this->onMouseMoveAwayFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.on_click) this->onClickFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.on_clone) this->onCloneFunctionSet[identifier] = function ;
			else if(listenMode == FeEGE::EventType.clones.on_clone) this->onCloneClonesFunctionSet[identifier] = function ;
			else{
				LPCSTR text = TEXT(("InputBox::listen方法中被传入了不恰当的事件\n\nInputBox名称 : " + this->id + "    事件id ：" + to_string(listenMode)).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
			}
		}
		inline virtual void stop(int listenMode,string identifier) {
			if(listenMode == FeEGE::EventType.frame) this->frameFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.on_mouse_put_on) this->onMousePutOnFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.on_mouse_hitting) this->onMouseHittingFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.on_mouse_move_away) this->onMouseMoveAwayFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.on_click) this->onClickFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.on_clone) this->onCloneFunctionSet.erase(identifier) ;
			else if(listenMode == FeEGE::EventType.clones.on_clone) this->onCloneClonesFunctionSet.erase(identifier) ;
			else{
				LPCSTR text = TEXT(("InputBox::stop方法中被传入了不恰当的事件\n\nInputBox名称 : " + this->id + "    事件id ：" + to_string(listenMode)).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
			}
		}
		inline void reflushMouseStatu() override {
			/*
				Test click
			*/
			if(this->isMouseIn()) {
				int statu = this->getVariable(0);
				if(this->isHit()) {
					if(statu == 1) {
						this->setVariable(0,2);
						if(this->isEnabled)
							for(auto it :this->onMouseHittingFunctionSet) it.second(this);
					}
				} else {
					if(statu == 0) {
						this->setVariable(0,1);
						if(this->isEnabled)
							for(auto it : this->onMousePutOnFunctionSet) it.second(this);
					} else if(statu == 2) {
						this->setVariable(0,0);
						if(this->isEnabled){
							this->father->select(this);
							for(auto it : this->onClickFunctionSet) it.second(this);
						}
					}
				}
			} else {
				if(this->getVariable(0) == 1)	{
					for(auto it : this->onMouseMoveAwayFunctionSet) it.second(this);
					if(this->isEnabled)
							this->setVariable(0,0);
				}
			}
		}
		inline void call() override {
			this->backupPos = pos;
			this->reflushMouseStatu();
			for(auto it : this->frameFunctionSet) it.second(this);
			if(this->isHit())
			if(this->deleted) return;
			if(!this->isShow) return;

			//backup
			if(this->isCancelX) {
				this->isCancelX = false;
				this->pos.x = this->backupPos.x;
			}
			if(this->isCancelY) {
				this->isCancelY = false;
				this->pos.y = this->backupPos.y;
			}
			
			this->drawed = false;
			putimage_rotatezoom(nullptr,this->imageVector[this->currentImage],this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f * pie,this->scale / 100.00f,1,this->alpha);
		}
		~InputBox(){
			this->father->erase(this);
		};
};

class Button : public Element{
	protected:
		bool isEnabled;
	public:
				Button(){
			this->isEnabled = false;
		}
		inline void enable(){
			this->isEnabled = true;
		}
		inline void disable(){
			this->isEnabled = false;
		}
		inline bool getEnableStatu(){
			return this->isEnabled;
		}
		inline void reflushMouseStatu() override {
			/*
				Test click
			*/
			if(this->isMouseIn()) {
				int statu = this->getVariable(0);
				if(this->isHit()) {
					if(statu == 1) {
						this->setVariable(0,2);
						if(this->isEnabled)
							for(auto it :this->onMouseHittingFunctionSet) it.second(this);
					}
				} else {
					if(statu == 0) {
						this->setVariable(0,1);
						if(this->isEnabled)
							for(auto it : this->onMousePutOnFunctionSet) it.second(this);
					} else if(statu == 2) {
						this->setVariable(0,0);
						if(this->isEnabled)
							for(auto it : this->onClickFunctionSet) it.second(this);
					}
				}
			} else {
				if(this->getVariable(0) == 1)	{
					for(auto it : this->onMouseMoveAwayFunctionSet) it.second(this);
					if(this->isEnabled)
							this->setVariable(0,0);
				}
			}
		}
		inline void call() override {
			this->backupPos = pos;
			this->reflushMouseStatu();
			for(auto it : this->frameFunctionSet) it.second(this);
//			if(this->isHit())
			if(this->deleted) return;
			if(!this->isShow) return;

			//backup
			if(this->isCancelX) {
				this->isCancelX = false;
				this->pos.x = this->backupPos.x;
			}
			if(this->isCancelY) {
				this->isCancelY = false;
				this->pos.y = this->backupPos.y;
			}
			
			this->drawed = false;
			putimage_rotatezoom(nullptr,this->imageVector[this->currentImage],this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f * pie,this->scale / 100.00f,1,this->alpha);
		}
		~Button(){ }
};

int currentRegOrder = 0;
unsigned long long frame = 0;
queue<Element*> freeList;

class cmp {
	public:
		bool operator()(Element* _A,Element* _B) {
			if(_A->getOrder() == _B->getOrder()) return _A->getRegOrder() < _B->getRegOrder();
			return _A->getOrder() < _B->getOrder();
		}
};

set<Element*,cmp> elementQueue;

void Element::increaseOrder(int count) {
	elementQueue.erase(this);
	this->order += count;
	elementQueue.insert(this);
}

void Element::decreaseOrder(int count) {
	elementQueue.erase(this);
	this->order -= count;
	elementQueue.insert(this);
}

void Element::setOrder(int count) {
	elementQueue.erase(this);
	this->order = count;
	elementQueue.insert(this);
}

namespace pen {
	int order = 0;
	int fontScale = 1;
	short penAlpha = 255;
	int penType = FeEGE::penType.left;
	int charWidth,charHeight;
	inline void print(int x,int y,string str) {
		if(pen_image == nullptr) return;
		if(penType == FeEGE::penType.middle) {
			x -= charWidth * str.length() >> 1;
			y -= charHeight >> 1;
		}
		outtextxy(x,y,str.c_str(),pen_image);
	}
	inline void print(int x,int y,wstring str) {
		if(pen_image == nullptr) return;
		if(penType == FeEGE::penType.middle) {
			x -= charWidth * str.length() >> 1;
			y -= charHeight >> 1;
		}
		outtextxy(x,y,str.c_str(),pen_image);
	}
	inline void font(int scale,string fontName = "幼圆") {
		if(pen_image == nullptr) return;
		fontScale = scale;
		setfont(scale,0,fontName.c_str(),pen_image);
		charWidth = textwidth('t',pen_image);
		charHeight = textheight('t',pen_image);
	}
	inline void color(color_t color) {
		if(pen_image == nullptr) return;
		setcolor(color,pen_image);
	}
	inline void type(int type) {
		penType = type;
	}
	inline void clear(int x,int y,int ex,int ey) {
		if(pen_image == nullptr) return;
		bar(x,y,ex,ey,pen_image);
	}
	inline void clearChar(int x,int y) {
		if(pen_image == nullptr) return;
		bar(x,y,x + charWidth,y + charHeight,pen_image);
	}
	inline void clearChars(int x,int y,int charCount) {
		if(pen_image == nullptr) return;
		bar(x,y,x + charWidth * charCount,y + charWidth,pen_image);
	}
	inline void clearAll() {
		if(pen_image == nullptr) return;
		bar(0,0,getwidth(pen_image),getheight(pen_image),pen_image);
	}
	inline void setOrder(int value) {
		order = value;
	}
	inline void setAlpha(short alpha) {
		penAlpha = alpha;
		penAlpha %= 256;
		if(penAlpha < 0) penAlpha += 256;
	}
	inline short getAlpha() {
		return penAlpha;
	}
	inline void increaseAlpha(short alpha){
		penAlpha += alpha;
		penAlpha %= 256;
		if(penAlpha < 0) penAlpha += 256;
	}
	inline void decreaseAlpha(short alpha){
		penAlpha -= alpha;
		penAlpha %= 256;
		if(penAlpha < 0) penAlpha += 256;
	}
	inline void printLine(int x1,int y1,int x2,int y2){
		line(x1,y1,x2,y2,pen_image);
	}
	inline void printBar(int left,int top,int right,int bottom){
		bar(left,top,right,bottom,pen_image);
	}
}

//Functions

void regElement(Element* element) {
	element->setRegOrder(currentRegOrder ++);
	elementQueue.insert(element);
	__SIZE__ ++;
	elementIsIn[element] = true;
	idToElement[element->getId()] = element;
}


mutex M;
Element* FeEGE::getElementById(string elementId) {
	M.lock();
	Element* res = idToElement[elementId];
	M.unlock();
	return res;
}

Element* FeEGE::getElementByPtr(Element* elementPtr) {
	return (elementIsIn.find(elementPtr) != elementIsIn.end()) ? elementPtr : nullptr;
}

Element elementPool[MAXELEMENTCOUNT];
bool elementPoolUsed[MAXELEMENTCOUNT] = {};

Element* Element::deleteThis() {
	freeList.push(this);
	return this;
}

Element* Element::deleteElement() {
	if(((Element*)this->getVariable(1)) != nullptr){
		((Element*)this->getVariable(1))->removeList.push_back(this);
	} 
	elementQueue.erase(this);
	this->deleted = true;
	elementPoolUsed[this->poolIndex] = false;
	this->frameFunctionSet.clear();
	if((Element*)this->privateVariables[1] == nullptr){
		for(auto i : this->imageVector) delimage(i);
	}
	if(this->__visibleImage != nullptr) delimage(this->__visibleImage);
	this->imageVector.clear();
	this->clones.clear();
	this->removeList.clear();
	this->onMousePutOnFunctionSet.clear();
	this->onMouseHittingFunctionSet.clear();
	this->onMouseMoveAwayFunctionSet.clear();
	this->onClickFunctionSet.clear();
	this->onCloneFunctionSet.clear();
	this->onCloneClonesFunctionSet.clear();
	this->deleted = true;
	elementIsIn[this] = false;
	idToElement[this->id] = nullptr;
	return this;
}

inline string getCurrentPath() {
    char buffer[MAX_PATH];
    if (GetModuleFileNameA(NULL, buffer, MAX_PATH)) {
        string fullPath(buffer);
        size_t lastSlash = fullPath.find_last_of("\\/");
        if (lastSlash != std::string::npos) {
            return fullPath.substr(0, lastSlash);
        }
    }
    return "";
}

inline string resolvePath(string s){
	if(s[0] == '.'){
		s = s.substr(1);
		s = getCurrentPath() + s;
	}
	return s;
}

bool isValidUTF8(const string& fileContent) {
    size_t i = 0;
    while (i < fileContent.size()) {
        unsigned char byte = fileContent[i];
        
        if (byte <= 0x7F) {
            i++;
        }
        else if ((byte >= 0xC2 && byte <= 0xDF) && i + 1 < fileContent.size() && (fileContent[i+1] & 0xC0) == 0x80) {
            i += 2;
        }
        else if ((byte >= 0xE0 && byte <= 0xEF) && i + 2 < fileContent.size() &&
                 (fileContent[i+1] & 0xC0) == 0x80 && (fileContent[i+2] & 0xC0) == 0x80) {
            i += 3;
        }
        else if ((byte >= 0xF0 && byte <= 0xF7) && i + 3 < fileContent.size() &&
                 (fileContent[i+1] & 0xC0) == 0x80 && (fileContent[i+2] & 0xC0) == 0x80 && (fileContent[i+3] & 0xC0) == 0x80) {
            i += 4;
        }
        else {
            return false;
        }
    }
    return true;
}

std::string detectEncoding(const std::string& fileContent) {
    if (isValidUTF8(fileContent)) {
        return "UTF-8";
    } else {
        return "ANSI";
    }
}

std::string UTF8ToANSI(const std::string& utf8Str) {
    // 检查并去除 BOM
    std::string utf8StrClean = utf8Str;
    if (utf8StrClean.size() >= 3 &&
        static_cast<unsigned char>(utf8StrClean[0]) == 0xEF &&
        static_cast<unsigned char>(utf8StrClean[1]) == 0xBB &&
        static_cast<unsigned char>(utf8StrClean[2]) == 0xBF) {
        utf8StrClean.erase(0, 3); // 去除 BOM
    }

    // 获取缓冲区长度的返回值
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8StrClean.c_str(), -1, nullptr, 0);
    if (wideLen == 0) {
        throw std::runtime_error("MultiByteToWideChar failed");
    }

    // 将 UTF-8 转换为宽字符（UTF-16）
    std::wstring wideStr(wideLen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8StrClean.c_str(), -1, &wideStr[0], wideLen);

    // 将宽字符（UTF-16）转换为 ANSI
    int ansiLen = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (ansiLen == 0) {
        throw std::runtime_error("WideCharToMultiByte failed");
    }

    std::string ansiStr(ansiLen, 0);
    WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, &ansiStr[0], ansiLen, nullptr, nullptr);

    // 去掉末尾的空字符
    ansiStr.pop_back();
    return ansiStr;
}

void runCmdAwait(const std::string cmd){
	STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWDEFAULT;
    const char* command = cmd.c_str();
    CreateProcess(NULL, (LPSTR)command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}


Element* newElement(string id,string imagePath,double x = 0,double y = 0) {
	imagePath = resolvePath(imagePath);
	PIMAGE image = newimage();
	if(imagePath != "") getimage(image,TEXT(imagePath.c_str()));
	else delimage(image);
	for(int i = 0; i < MAXELEMENTCOUNT; ++ i) {
		if(!elementPoolUsed[i]) {
			elementPoolUsed[i] = true;
			Element* e = elementPool[i].copy(id,image,i,x,y);
			regElement(e);
			e->setType("Normal@native");
			return e;
		}
	}
	MessageBox(getHWnd(),"分配Element失败(达到最大容量)","提示",MB_OK);
	return nullptr;
}

Button* newButton(string id,string imagePath,double x,double y,function<void(Element*)> putOn = nullptr,function<void(Element*)> moveAway = nullptr,function<void(Element*)> on_click = nullptr) {
	Button* button = (Button*)newElement(id,imagePath,x,y);
	if(button == nullptr) return nullptr;
	button->setType("Button@native");
	button->listen(FeEGE::EventType.on_click,"click@native",on_click);
	button->listen(FeEGE::EventType.on_mouse_put_on,"hit@native",putOn);
	button->listen(FeEGE::EventType.on_mouse_move_away,"hit@native",moveAway);
	return button;
}

void selectInputBox(InputBox* inputBox){
	
}

Element* newInputBox(string id,string imagePath,double x,double y,Element* submitButton,function<void(Element*)> onSubmit = nullptr,function<void(Element*)> onSelected = nullptr){
	Element* inputBox = newElement(id,imagePath,x,y);
	if(inputBox == nullptr) return nullptr;
	inputBox->setType("InputBox@native");
	submitButton->listen(EventType.on_click,id + "SUBMIT EVENT",onSubmit);
	inputBox->listen(EventType.inputBox.onSelect,"SELECT",onSelected);
	return inputBox;
}

#if true
Element* newElement(string id,PIMAGE image,double x = 0,double y = 0) {
	for(int i = 0; i < MAXELEMENTCOUNT; ++ i) {
		if(!elementPoolUsed[i]) {
			elementPoolUsed[i] = true;
			Element* e = elementPool[i].copy(id,image,i,x,y);
			regElement(e);
			return e;
		}
	}
	MessageBox(getHWnd(),"分配Element失败(达到最大容量)","提示",MB_OK);
	return nullptr;
}
#endif

void globalListen(int listenMode,string identifier,auto function){
	if(listenMode == FeEGE::EventType.frame) globalListenFrameFunctionSet[identifier] = function ;
	if(listenMode == FeEGE::EventType.on_click) globalListenOnClickFunctionSet[identifier] = function ;
}

void stopGlobalListen(int listenMode,string identifier){
	if(listenMode == FeEGE::EventType.frame) globalListenFrameFunctionSet.erase(identifier) ;
	if(listenMode == FeEGE::EventType.on_click) globalListenOnClickFunctionSet.erase(identifier) ;
}

void reflush() {
	++ frame;
	int x,y;
	mousepos(&x,&y);
	mousePos = {(double)x,(double)y};

	while(!freeList.empty()) {
		freeList.front()->deleteElement();
		freeList.pop();
	}
	
	// run schedule
	vector<function<void(void)>> scheduleBackup;
	scheduleBackup.swap(schedule);
	for(auto func : scheduleBackup) func();
	
	// check timeOut
	double currentTime = getMs();
	while(!scheduleTimeOut.empty() && (*scheduleTimeOut.begin()).first < currentTime){
		auto vec = (*scheduleTimeOut.begin()).second;
		scheduleTimeOut.erase(scheduleTimeOut.begin());
		for(const auto& func : vec) func();
	}
	
	bool penNprinted = true;
	cleardevice();

	for(Element* it : elementQueue) {
		if(it == nullptr || it == *elementQueue.end()) continue;
		if(penNprinted && it->getOrder() >= pen::order) {
			penNprinted = false;
			putimage_alphatransparent(nullptr,pen_image,0,0,EGERGBA(1,1,4,0),pen::penAlpha);
		}
		it->call();
	}
	if(penNprinted) putimage_alphatransparent(nullptr,pen_image,0,0,EGERGBA(1,1,4,0),pen::penAlpha);
	flushmouse();
	#ifdef FPS
	static char fps[32];
	memset(fps,0,sizeof(fps));
	sprintf(fps,"FPS : %0.2f",getfps());
	setcaption(fps);
	#endif
	#ifndef bigScreen
	builtinGetKey();
	#endif
	delay_ms(1);
}

void init(int x,int y,int mode = INIT_RENDERMANUAL){
	setinitmode(mode);
	initgraph(x,y);
	initXY();
	FeEGE::initPen();
}

void start() {
	closeGraph = false;
	initXY();
	randomize();
	while(!closeGraph && is_run()) {
		reflush();
		for(auto it : globalListenFrameFunctionSet) it.second();
		if(!regPause) continue;
#ifndef NO_THREAD
#ifdef Ppause
		if(FeEGE::getkey('p') || FeEGE::getkey('P')) {
			this_thread::sleep_for(chrono::milliseconds(200));
			while(!FeEGE::getkey('p') && !FeEGE::getkey('P')) this_thread::sleep_for(chrono::milliseconds(1));
			this_thread::sleep_for(chrono::milliseconds(200));
		}
#endif
#ifdef ESCexit
		if(FeEGE::getkey(VK_ESCAPE)) break;
#endif
#endif
		delay_jfps(120);
	}
}

#endif
