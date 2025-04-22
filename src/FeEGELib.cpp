#include "FeEGELib.h"
using namespace FeEGE;

// Initialize global variables
double globalRating = 1.00f;

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
mutex M;

PIMAGE pen_image;

unordered_map<string,Element*>idToElement;
unordered_map<Element*,bool>elementIsIn;

int sgn(double x){
	if(fabs(x) < eps) return 0;
	return x > 0 ? 1 : -1;
}

namespace FeEGE {
	
Position lastPixel, mousePos;
Events EventType;
PenTypes penType;

}

Position& FeEGE::getLastPixel() {
	return lastPixel;
}

Position& FeEGE::getMousePos(){
	return mousePos;
}

#ifndef bigScreen
bool FeEGE::builtinGetKey(){
	for(int i = 0;i <= 0xFF;++ i){
		vkState[i] = GetAsyncKeyState(i) & 0x8000;
	}
}
bool FeEGE::getkey(int VK) {
	return vkState[VK];
}
#endif
void FeEGE::enablePause() {
	regPause = true;
}
void FeEGE::disablePause() {
	regPause = false;
}
Element* FeEGE::getElementById(string elementId) {
	M.lock();
	Element* res = idToElement[elementId];
	M.unlock();
	return res;
}

Element* FeEGE::getElementByPtr(Element* elementPtr) {
	return (elementIsIn.find(elementPtr) != elementIsIn.end()) ? elementPtr : nullptr;
}
void FeEGE::initPen() {
	WIDTH = getwidth();
	HEIGHT = getheight();
	if(pen_image != nullptr) delimage(pen_image);
	pen_image = newimage(WIDTH,HEIGHT);
	setbkcolor(EGERGBA(1,1,4,0),pen_image);
	setcolor(EGERGB(255,0,0),pen_image);
	setfillcolor(EGERGBA(1,1,4,0),pen_image);
}

double FeEGE::getMs(){
	return (double)clock() / CLOCKS_PER_SEC * 1000;
}

void FeEGE::pushSchedule(function<void(void)> func) {
	schedule.push_back(func);
}

void FeEGE::setTimeOut(function<void(void)> func,double timeMs){
	scheduleTimeOut[getMs() + timeMs].emplace_back(func);
}

namespace Ease{
    Animate easeInOutCubicX([&](double tick) -> Position { return {tick <= 0.5 ? 4 * tick * tick * tick : 1 - pow(-2 * tick + 2,3) / 2,0}; },0.002,1);
    Animate easeInOutCubicY([&](double tick) -> Position { return {0,tick <= 0.5 ? 4 * tick * tick * tick : 1 - pow(-2 * tick + 2,3) / 2}; },0.002,1);
    Animate easeOutQuadX([&](double tick) -> Position { return {1 - (1 - tick) * (1 - tick),0}; },0.002,1);
    Animate easeOutQuadY([&](double tick) -> Position { return {0,1 - (1 - tick) * (1 - tick)}; },0.002,1);
}

int currentRegOrder = 0;
unsigned long long frame = 0;
queue<Element*> freeList;

set<Element*,cmp> elementQueue;

bool cmp::operator()(Element* _A,Element* _B) {
	if(_A->getOrder() == _B->getOrder()) return _A->getRegOrder() < _B->getRegOrder();
	return _A->getOrder() < _B->getOrder();
}

namespace pen {
    int order = 0;
    int fontScale = 1;
    short penAlpha = 255;
    int penType = FeEGE::penType.left;
    int charWidth,charHeight;
}

void pen::print(int x,int y,string str) {
	if(pen_image == nullptr) return;
	if(penType == FeEGE::penType.middle) {
		x -= charWidth * str.length() >> 1;
		y -= charHeight >> 1;
	}
	outtextxy(x,y,str.c_str(),pen_image);
}
void pen::print(int x,int y,wstring str) {
	if(pen_image == nullptr) return;
	if(penType == FeEGE::penType.middle) {
		x -= charWidth * str.length() >> 1;
		y -= charHeight >> 1;
	}
	outtextxy(x,y,str.c_str(),pen_image);
}
void pen::font(int scale,string fontName) {
	if(pen_image == nullptr) return;
	fontScale = scale;
	setfont(scale,0,fontName.c_str(),pen_image);
	charWidth = textwidth('t',pen_image);
	charHeight = textheight('t',pen_image);
}
void pen::color(color_t color) {
	if(pen_image == nullptr) return;
	setcolor(color,pen_image);
}
void pen::type(int type) {
	penType = type;
}
void pen::clear(int x,int y,int ex,int ey) {
	if(pen_image == nullptr) return;
	bar(x,y,ex,ey,pen_image);
}
void pen::clearChar(int x,int y) {
	if(pen_image == nullptr) return;
	bar(x,y,x + charWidth,y + charHeight,pen_image);
}
void pen::clearChars(int x,int y,int charCount) {
	if(pen_image == nullptr) return;
	bar(x,y,x + charWidth * charCount,y + charWidth,pen_image);
}
void pen::clearAll() {
	if(pen_image == nullptr) return;
	bar(0,0,getwidth(pen_image),getheight(pen_image),pen_image);
}
void pen::setOrder(int value) {
	order = value;
}
void pen::setAlpha(short alpha) {
	penAlpha = alpha;
	penAlpha %= 256;
	if(penAlpha < 0) penAlpha += 256;
}
short pen::getAlpha() {
	return penAlpha;
}
void pen::increaseAlpha(short alpha){
	penAlpha += alpha;
	penAlpha %= 256;
	if(penAlpha < 0) penAlpha += 256;
}
void pen::decreaseAlpha(short alpha){
	penAlpha -= alpha;
	penAlpha %= 256;
	if(penAlpha < 0) penAlpha += 256;
}
void pen::printLine(int x1,int y1,int x2,int y2){
	line(x1,y1,x2,y2,pen_image);
}
void pen::printBar(int left,int top,int right,int bottom){
	bar(left,top,right,bottom,pen_image);
}

Element elementPool[MAXELEMENTCOUNT];
bool elementPoolUsed[MAXELEMENTCOUNT] = {};

// Animate class implementation
Animate::Animate(std::function<Position(double)> function,double speed,double end): 
    function(function),speed(speed),end(end) {};

// Element class implementation
Element::Element(string id,PIMAGE image,Position pos) {
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

Element::Element(string id,PIMAGE image,double x, double y) {
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

Element::Element() { }

Element* Element::copy(string id,PIMAGE image,unsigned long long index,double x,double y) {
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

void Element::reflushMouseStatu() {
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
bool Element::drawToPrivateImage(){
	if(this->drawed) return false;
	if(this->deleted) return false;
	if(this->disabledDrawToPrivateImage) return false;
	if(this->imageVector[this->currentImage] == nullptr) return false;
	cleardevice(this->__visibleImage);
	putimage_rotatezoom(this->__visibleImage,this->imageVector[this->currentImage],this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f * pie,this->scale / 100.00f,1);
	return this->drawed = true;
}

void Element::cancelAnimate(const Animate& animate,double tick,double x,double y){
	Position p = animate.function(tick);
	this->pos.x -= p.x * x;
	this->pos.y -= p.y * y;
}

void Element::runAnimate(const Animate& animate,double tick,double x,double y){
	Position p = animate.function(tick);
	this->pos.x += p.x * x;
	this->pos.y += p.y * y;
}

void Element::callAnimations(){
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

void Element::call() {
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
void Element::setType(const string& type){
	this->elementType = type;
}
string Element::getType(const string& type){
	return this->elementType;
}
void Element::moveLeft(double pixels) {
	this->pos.x -= pixels;
}
void Element::moveRight(double pixels) {
	this->pos.x += pixels;
}
void Element::moveUp(double pixels) {
	this->pos.y -= pixels;
}
void Element::moveDown(double pixels) {
	this->pos.y += pixels;
}
void Element::moveForward(double pixels) {
	this->pos.x -= sin(this->angle * pie / 180.00f) * pixels;
	this->pos.y -= cos(this->angle * pie / 180.00f) * pixels;
}
void Element::moveTo(double x,double y) {
	this->pos.x = x;
	this->pos.y = y;
}
void Element::moveTo(Position position) {
	this->pos = position;
}
color_t Element::getPixel(int x,int y) {
	return getpixel(x,y,this->__visibleImage);
}
short Element::getScale() {
	return this->scale;
}
void Element::increaseScale(short scale) {
	if(this->scale + scale <= 255) this->scale += scale;
	else this->scale = 255;
	this->scale %= 255;
	if(this->scale < 0) this->scale += 255;
}
void Element::decreaseScale(short scale) {
	if(this->scale > scale) this->scale -= scale;
	else this->scale = 0;
	this->scale %= 255;
	if(this->scale < 0) this->scale += 101;
}
void Element::setScale(short scale) {
	this->scale = scale;
	this->scale %= 255;
}
void Element::show() {
	this->isShow = true;
}
void Element::hide() {
	this->isShow = false;
}
void Element::turnRight(int angle) {
	this->angle = (this->angle - angle) % 360;
}
void Element::turnLeft(int angle) {
	this->angle = (this->angle + angle) % 360;
}
void Element::turnTo(int angle) {
	this->angle = angle % 360;
}
bool Element::faceTo(Element* that){
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
int Element::getAngle() {
	return this->angle;
}
Position Element::getPosition() {
	return this->pos;
}
bool Element::getIsShow() {
	return this->isShow;
}
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
void Element::setRegOrder(int count) {
	this->regOrder = count;
}
int Element::getRegOrder() {
	return this->regOrder;
}
int Element::getOrder() {
	return this->order;
}
bool Element::operator<(Element _B) {
	if(this->getOrder() > _B.getOrder()) return false;
	else if (this->getOrder() < _B.getOrder()) return true;
	return this->getRegOrder() < _B.getRegOrder();
}
bool Element::operator>(Element _B) {
	if(this->getOrder() < _B.getOrder()) return false;
	else if (this->getOrder() > _B.getOrder()) return true;
	return this->getRegOrder() > _B.getRegOrder();
}
void Element::setPosX(int x) {
	this->pos.x = x;
}
void Element::setPosY(int y) {
	this->pos.y = y;
}
void Element::removeColor(color_t color) {
	this->removeColors.push_back(color);
}
bool Element::isMouseIn() {
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
bool Element::isHit() {
	if(!getkey(leftButton)) return false;
	return this->isMouseIn();
}
void Element::setVariable(unsigned int which,long long value) {
	this->privateVariables[which] = value;
//			// cout<<which<<" "<<value<<" "<<this<<" "<<this->id<<endl;
}
long long& Element::getVariable(unsigned int which) {
	return this->privateVariables[which];
}
void Element::increaseVariable(unsigned int which,long long value) {
	this->privateVariables[which] += value;
}
void Element::decreaseVariable(unsigned int which,long long value) {
	this->privateVariables[which] -= value;
}
void Element::addImage(PIMAGE image) {
	this->imageVector.push_back(image);
}
bool Element::setImage(int order) {
	if(this->imageVector[order] != nullptr){
		this->currentImage = order;
		return true;
	}
	return false;
}
int Element::getImageOrder() {
	return this->currentImage;
}
PIMAGE Element::getImage() {
	return this->imageVector[this->currentImage];
}
bool Element::isTouchedBy(Element* that) {
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
Element* Element::clone() {
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
void Element::cloneImage(){
	for(int i = 0;i < this->imageVector.size();++ i){
		if(this->imageVector[i] == nullptr) continue;
		PIMAGE img = newimage();
		getimage(img,this->imageVector[i],0,0,getwidth(this->imageVector[i]),getheight(this->imageVector[i]));
		this->imageVector[i] = img;
	}
}
string Element::getId() {
	return this->id;
}
void Element::listen(int listenMode,string identifier,function<void(Element*)> function) {
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
void Element::stop(int listenMode,string identifier) {
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
void Element::cancelX() {
	this->isCancelX = true;
}
void Element::cancelY() {
	this->isCancelY = true;
}
void Element::cancelMove() {
	this->isCancelX = true;
	this->isCancelY = true;
}
short Element::getAlpha() {
	return this->alpha;
}
void Element::setAlpha(short alpha) {
	this->alpha = alpha;
	this->alpha %= 256;
	if(this->alpha < 0) this->alpha += 256;
}
void Element::decreaseAlpha(short alpha) {
	if(this->alpha - alpha < 0) this->alpha = 0;
	else this->alpha -= alpha;
	this->alpha %= 256;
	if(this->alpha < 0) this->alpha += 256;
}
void Element::increaseAlpha(short alpha) {
	if(this->alpha + alpha > 255) this->alpha = 255;
	else this->alpha += alpha;
	this->alpha %= 256;
	if(this->alpha < 0) this->alpha += 256;
}
void Element::nextImage() {
	do {
		this->currentImage ++;
		this->currentImage %= 100;
	} while(this->imageVector[currentImage] == nullptr);
}
#ifndef NO_THREAD
void Element::loadImage(string imagePath,int id) {
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
void Element::deleteImage(int id) {
	thread([this,id]() {
		delimage(this->imageVector[id]);
	}).detach();
}
#endif
void Element::disableDrawToPrivateImage() {
	this->disabledDrawToPrivateImage = true;
}
void Element::enableDrawToPrivateImage() {
	this->disabledDrawToPrivateImage = false;
}
void Element::ignoreBlack() {
	int XX = getwidth(this->imageVector[this->currentImage]);
	int YY = getheight(this->imageVector[this->currentImage]);
	for(int i = 0; i < XX; ++ i) {
		for(int j = 0; j < YY; ++ j) {
			if(getpixel(i,j,this->imageVector[this->currentImage]) == (color_t)BLACK) putpixel_f(i,j,EGERGBA(0,0,0,0),this->imageVector[this->currentImage]);
		}
	}
}
#ifdef TEST_FUNCTION 
void Element::useHittingBox(){
	this->hittingBox = true;
} 
void Element::stopHittingBox(){
	this->hittingBox = false;
}

// PhysicEngine
void Element::enablePhysicEngine() {
	this->physicEngineStatu = true;
}
void Element::disablePhysicEngine() {
	this->physicEngineStatu = false;
}
void Element::physicForce(double angle,double force) {
	this->forceX += cos(angle * pie / 180.00) * force;
	this->forceY -= sin(angle * pie / 180.00) * force;
}
void Element::physicSpeed(double angle,double speed) {
	this->speedX += cos(angle * pie / 180.00) * speed;
	this->speedY -= sin(angle * pie / 180.00) * speed;
}
void Element::physicResistance(double angle,double resistance) {
	this->resistanceX += cos(angle * pie / 180.00) * resistance;
	this->resistanceY -= sin(angle * pie / 180.00) * resistance;
}
void Element::updatePhysicSpeed() {
	this->speedX += forceX;
	this->speedY += forceY;
	bool XX = this->speedX * resistanceX > 0;
	bool YY = this->speedY * resistanceY > 0;
	if(XX && fabs(this->speedX) >= fabs(this->resistanceX)) this->speedX -= this->resistanceX;
	else if(XX)this->speedX = 0;
	if(YY && fabs(this->speedY) >= fabs(this->resistanceY)) this->speedY -= this->resistanceY;
	else if(YY) this->speedY = 0;
}
void Element::updatePhysicPosition() {
	this->pos.x += speedX * globalRating;
	this->pos.y += speedY * globalRating;
//			cout<<"updated\n";
}
double Element::getSpeedX() {
	return this->speedX;
}
double Element::getSpeedY() {
	return this->speedY;
}
void Element::physicArcSpeedX() {
	this->speedX = -this->speedX;
}
void Element::physicArcSpeedY() {
	this->speedY = -this->speedY;
}
void Element::physicRemoveSpeedX() {
	this->speedX = 0;
}
void Element::physicRemoveSpeedY() {
	this->speedY = 0;
}
void Element::physicRemoveForceX() {
	this->forceX = 0;
}
void Element::physicRemoveForceY() {
	this->forceY = 0;
}

// Move Animated
void Element::moveWithAnimation(double xPixel,double yPixel,const Animate& animate,function<void(Element*)> callback = nullptr){
	static int cur = 0;
	animateStates[++ cur] = 0;
	Position p = animate.function(animate.end);
	animations.emplace_back(make_pair(make_pair(cur,make_pair((p.x == 0 ? 0 : xPixel / p.x),(p.y == 0 ? 0 : yPixel / p.y))),animate));
	if(callback == nullptr) return;
	this->animateCallbacks[cur] = callback;
}
#endif

Element::~Element() {
	
};

// ... (continue with all other method implementations)

// InputBoxSet class implementation
InputBoxSet::InputBoxSet(){
    this->isEnabled = false;
}

// ... (continue with all other class implementations)

// Global functions implementation
string getCurrentPath() {
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

string resolvePath(string s){
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

void regElement(Element* element) {
    element->setRegOrder(currentRegOrder ++);
    elementQueue.insert(element);
    __SIZE__ ++;
    elementIsIn[element] = true;
    idToElement[element->getId()] = element;
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

void init(int x,int y,int mode){
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

// ... (continue with all other function implementations)
