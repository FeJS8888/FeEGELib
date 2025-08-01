#include "Element.h"
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
set<string> ElementListenStoppedSet;

int WIDTH;
int HEIGHT;

vector<function<void(void)> > schedule;
map<double,vector<function<void(void)> > > scheduleTimeOut;
int __SIZE__ = 0;
int removeSize = 0;
bool closeGraph = false;
mutex M;

PIMAGE pen_image;
PIMAGE textpen_image;

unordered_map<string,Element*>idToElement;
unordered_map<Element*,bool>elementIsIn;

int sgn(double x){
	if(fabs(x) < eps) return 0;
	return x > 0 ? 1 : -1;
}

namespace FeEGE {
	
Position lastPixel, mousePos;
Events EventType;
PenTypes PenType;

}

Position& FeEGE::getLastPixel() {
	return lastPixel;
}

Position& FeEGE::getMousePos(){
	return mousePos;
}

#ifndef bigScreen
void FeEGE::builtinGetKey(){
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

void FeEGE::initTextPen() {
	WIDTH = getwidth();
	HEIGHT = getheight();
	if(textpen_image != nullptr) delimage(textpen_image);
	textpen_image = newimage(WIDTH,HEIGHT);
	setbkcolor(EGERGBA(1,1,4,0),textpen_image);
	setcolor(EGERGB(255,0,0),textpen_image);
	setfillcolor(EGERGBA(1,1,4,0),textpen_image);
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
    Animate easeInOutCubicX([](double tick) -> Position { return {tick <= 0.5 ? 4 * tick * tick * tick : 1 - pow(-2 * tick + 2,3) / 2,0}; },0.002,1);
    Animate easeInOutCubicY([](double tick) -> Position { return {0,tick <= 0.5 ? 4 * tick * tick * tick : 1 - pow(-2 * tick + 2,3) / 2}; },0.002,1);
    Animate easeOutQuadX([](double tick) -> Position { return {1 - (1 - tick) * (1 - tick),0}; },0.002,1);
    Animate easeOutQuadY([](double tick) -> Position { return {0,1 - (1 - tick) * (1 - tick)}; },0.002,1);
}

int currentRegOrder = 0;
unsigned long long frame = 0;
queue<Element*> freeList;

set<Element*,cmp> elementQueue;

bool cmp::operator()(Element* _A,Element* _B) const {
	if(_A->getOrder() == _B->getOrder()) return _A->getRegOrder() < _B->getRegOrder();
	return _A->getOrder() < _B->getOrder();
}

namespace pen {
    int order = 0;
    int fontScale = 1;
    short penAlpha = 255;
    int penType = FeEGE::PenType.left;
    int charWidth,charHeight;
}

namespace textpen {
    int order = 0;
    int fontScale = 1;
    short penAlpha = 255;
    int penType = FeEGE::PenType.left;
    int charWidth,charHeight;
}

void pen::print(int x,int y,string str) {
	if(pen_image == nullptr) return;
	if(penType == FeEGE::PenType.middle) {
		x -= charWidth * str.length() >> 1;
		y -= charHeight >> 1;
	}
	ege_outtextxy(x,y,str.c_str(),pen_image);
}
void pen::print(int x,int y,wstring str) {
	if(pen_image == nullptr) return;
	if(penType == FeEGE::PenType.middle) {
		x -= charWidth * str.length() >> 1;
		y -= charHeight >> 1;
	}
	ege_outtextxy(x,y,str.c_str(),pen_image);
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

void textpen::print(int x,int y,string str) {
	if(textpen_image == nullptr) return;
	if(penType == FeEGE::PenType.middle) {
		x -= charWidth * str.length() >> 1;
		y -= charHeight >> 1;
	}
	ege_outtextxy(x,y,str.c_str(),textpen_image);
}
void textpen::print(int x,int y,wstring str) {
	if(textpen_image == nullptr) return;
	if(penType == FeEGE::PenType.middle) {
		x -= charWidth * str.length() >> 1;
		y -= charHeight >> 1;
	}
	ege_outtextxy(x,y,str.c_str(),textpen_image);
}
void textpen::font(int scale,string fontName) {
	if(textpen_image == nullptr) return;
	fontScale = scale;
	setfont(scale,0,fontName.c_str(),textpen_image);
	charWidth = textwidth('t',textpen_image);
	charHeight = textheight('t',textpen_image);
}
void textpen::color(color_t color) {
	if(textpen_image == nullptr) return;
	setcolor(color,textpen_image);
}
void textpen::type(int type) {
	penType = type;
}
void textpen::clear(int x,int y,int ex,int ey) {
	if(textpen_image == nullptr) return;
	bar(x,y,ex,ey,textpen_image);
}
void textpen::clearChar(int x,int y) {
	if(textpen_image == nullptr) return;
	bar(x,y,x + charWidth,y + charHeight,textpen_image);
}
void textpen::clearChars(int x,int y,int charCount) {
	if(textpen_image == nullptr) return;
	bar(x,y,x + charWidth * charCount,y + charWidth,textpen_image);
}
void textpen::clearAll() {
	if(textpen_image == nullptr) return;
	bar(0,0,getwidth(textpen_image),getheight(textpen_image),textpen_image);
}
void textpen::setOrder(int value) {
	order = value;
}
void textpen::setAlpha(short alpha) {
	penAlpha = alpha;
	penAlpha %= 256;
	if(penAlpha < 0) penAlpha += 256;
}
short textpen::getAlpha() {
	return penAlpha;
}
void textpen::increaseAlpha(short alpha){
	penAlpha += alpha;
	penAlpha %= 256;
	if(penAlpha < 0) penAlpha += 256;
}
void textpen::decreaseAlpha(short alpha){
	penAlpha -= alpha;
	penAlpha %= 256;
	if(penAlpha < 0) penAlpha += 256;
}
void textpen::printLine(int x1,int y1,int x2,int y2){
	line(x1,y1,x2,y2,textpen_image);
}
void textpen::printBar(int left,int top,int right,int bottom){
	bar(left,top,right,bottom,textpen_image);
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
    ege_enable_aa(this->__visibleImage);
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
				for(auto it :this->onMouseHittingFunctionSet){
					if(ElementListenStoppedSet.find(this->id + "@" + "FeEGE::EventType.on_mouse_hitting" + "@" + it.first) != ElementListenStoppedSet.end()) continue;
					it.second(this);
				}
			}
			#endif
			
			if(statu == 1){
				this->setVariable(0,2);
				for(auto it :this->onMouseHittingFunctionSet){
					if(ElementListenStoppedSet.find(this->id + "@" + "FeEGE::EventType.on_mouse_hitting" + "@" + it.first) != ElementListenStoppedSet.end()) continue;
					it.second(this);
				} 
			}
		} 
		else {
			if(statu == 0){
				this->setVariable(0,1);
				for(auto it : this->onMousePutOnFunctionSet){
					if(ElementListenStoppedSet.find(this->id + "@" + "FeEGE::EventType.on_mouse_put_on" + "@" + it.first) != ElementListenStoppedSet.end()) continue;
					it.second(this);
				} 
			} 
			else if(statu == 2) {
				this->setVariable(0,0);
				for(auto it : this->onClickFunctionSet){
					if(ElementListenStoppedSet.find(this->id + "@" + "FeEGE::EventType.on_click" + "@" + it.first) != ElementListenStoppedSet.end()) continue;
					it.second(this);
				}
			}
		}
	} 
	else {
		if(this->getVariable(0) == 1){
			this->setVariable(0,0);
			for(auto it : this->onMouseMoveAwayFunctionSet){
				if(ElementListenStoppedSet.find(this->id + "@" + "FeEGE::EventType.on_mouse_move_away" + "@" + it.first) != ElementListenStoppedSet.end()) continue;
				it.second(this);
			}
		}
		else if(this->getVariable(0) == 2 && !getkey(leftButton)){
			this->setVariable(0,0);
			for(auto it : this->onMouseHitButMoveAwaySet){
				if(ElementListenStoppedSet.find(this->id + "@" + "FeEGE::EventType.on_mouse_hit_but_move_away" + "@" + it.first) != ElementListenStoppedSet.end()) continue;
				it.second(this);
			}
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
	for(auto it : this->frameFunctionSet){
		if(ElementListenStoppedSet.find(this->id + "@" + "FeEGE::EventType.frame" + "@" + it.first) != ElementListenStoppedSet.end()) continue;
		it.second(this);
	}
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
double Element::moveSafely(Position pixelsDir,const vector<Element*>& limit){
	Position backup = this->pos;
	this->pos = this->pos + pixelsDir;
	for(const auto& ptr : limit){
		if(this->isTouchedBy(ptr)){
			Position tr = pixelsDir.normalize() * getSeparateDistance(this->transformPolygon(this->getPolygonSet()[0]),ptr->transformPolygon(ptr->getPolygonSet()[0]),pixelsDir.normalize());
			this->pos = this->pos - tr;
			this->pos = this->pos - (pixelsDir.normalize()) * 0.005;
		}
	}
	return (this->pos - backup).length();
}
color_t Element::getPixel(int x,int y) {
	return getpixel(x,y,this->__visibleImage);
}
double Element::getScale() {
	return this->scale;
}
void Element::increaseScale(double scale) {
	if(this->scale + scale <= 1000) this->scale += scale;
	else this->scale = 1000;
	if(this->scale < 0) this->scale = 0;
	if(this->scale > 1000) this->scale = 1000;
}
void Element::decreaseScale(double scale) {
	if(this->scale > scale) this->scale -= scale;
	else this->scale = 0;
	if(this->scale < 0) this->scale = 0;
	if(this->scale > 1000) this->scale = 1000;
}
void Element::setScale(double scale) {
	this->scale = scale;
	this->scale = fmod(this->scale,1000);
}
void Element::show() {
	this->isShow = true;
}
void Element::hide() {
	this->isShow = false;
}
void Element::turnRight(double angle) {
	this->angle = fmod((this->angle - angle),360);
}
void Element::turnLeft(double angle) {
	this->angle = fmod(this->angle + angle,360);
}
void Element::turnTo(double angle) {
	this->angle = fmod(angle,360);
}
bool Element::faceTo(Element* that){
	if(that == nullptr) {
		LPCSTR text = TEXT(("Element::faceTo����������Ĵ�����nullptr����\n�����������getElementById��ѯ�˲����ڵĶ���\n\nElement���� : " + this->id).c_str());
		MessageBox(getHWnd(),text,"����",MB_ICONWARNING | MB_OK);
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
bool Element::faceTo(const Position& pos){
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
double Element::getAngle() {
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
	if(this->hittingBox){
		for(const auto& a : this->polygonSet){
			vector<Position> shape = transformPolygon(a);
			if(isPointInConvexPolygon(shape,Position{(double)x,(double)y})){
				return true;
			}
		}
		return false;
	}
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
		LPCSTR text = TEXT(("Element::isTouchedBy����������Ĵ�����nullptr����\n�����������getElementById��ѯ�˲����ڵĶ���\n\nElement���� : " + this->id).c_str());
		MessageBox(getHWnd(),text,"����",MB_ICONWARNING | MB_OK);
		return false; 
	}
	if(!this->isShow || !that->isShow) return false;
	
	if(this->hittingBox ^ that->hittingBox){
		LPCSTR text = TEXT(("Element::isTouchedBy�����������������ײ��״̬��һ��\n\nElement���� : " + this->id).c_str());
		MessageBox(getHWnd(),text,"����",MB_ICONWARNING | MB_OK);
		return false;
	}
	if(this->hittingBox){
		for(FeEGE::Polygon& x : this->polygonSet){
			vector<Position> shapeX = transformPolygon(x);
			for(FeEGE::Polygon& y : that->polygonSet){
				vector<Position> shapeY = that->transformPolygon(y);
				if(isTouched(shapeX,shapeY)) return true;
			}
		}
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
	
	return false; 
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
vector<Element*> Element::getClones(){
	return this->clones;
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
	if(listenMode == FeEGE::EventType.frame) this->frameFunctionSet[identifier] = function;
	else if(listenMode == FeEGE::EventType.on_mouse_put_on) this->onMousePutOnFunctionSet[identifier] = function;
	else if(listenMode == FeEGE::EventType.on_mouse_hitting) this->onMouseHittingFunctionSet[identifier] = function;
	else if(listenMode == FeEGE::EventType.on_mouse_move_away) this->onMouseMoveAwayFunctionSet[identifier] = function;
	else if(listenMode == FeEGE::EventType.on_click) this->onClickFunctionSet[identifier] = function;
	else if(listenMode == FeEGE::EventType.on_clone) this->onCloneFunctionSet[identifier] = function;
	else if(listenMode == FeEGE::EventType.on_mouse_hit_but_move_away) this->onMouseHitButMoveAwaySet[identifier] = function;
	else if(listenMode == FeEGE::EventType.clones.on_clone) this->onCloneClonesFunctionSet[identifier] = function;
	else{
		LPCSTR text = TEXT(("Element::listen�����б������˲�ǡ�����¼�\n\nElement���� : " + this->id + "    �¼�id ��" + to_string(listenMode)).c_str());
		MessageBox(getHWnd(),text,"����",MB_ICONWARNING | MB_OK);
		return;
	}
}
void Element::unlisten(int listenMode,string identifier) {
	if(listenMode == FeEGE::EventType.frame) this->frameFunctionSet.erase(identifier);
	else if(listenMode == FeEGE::EventType.on_mouse_put_on) this->onMousePutOnFunctionSet.erase(identifier);
	else if(listenMode == FeEGE::EventType.on_mouse_hitting) this->onMouseHittingFunctionSet.erase(identifier);
	else if(listenMode == FeEGE::EventType.on_mouse_move_away) this->onMouseMoveAwayFunctionSet.erase(identifier);
	else if(listenMode == FeEGE::EventType.on_click) this->onClickFunctionSet.erase(identifier);
	else if(listenMode == FeEGE::EventType.on_clone) this->onCloneFunctionSet.erase(identifier);
	else if(listenMode == FeEGE::EventType.on_mouse_hit_but_move_away) this->onMouseHitButMoveAwaySet.erase(identifier);
	else if(listenMode == FeEGE::EventType.clones.on_clone) this->onCloneClonesFunctionSet.erase(identifier);
	else{
		LPCSTR text = TEXT(("Element::unlisten�����б������˲�ǡ�����¼�\n\nElement���� : " + this->id + "    �¼�id ��" + to_string(listenMode)).c_str());
		MessageBox(getHWnd(),text,"����",MB_ICONWARNING | MB_OK);
		return;
	}
}
void Element::begin(int listenMode,string identifier) {
	string res = this->id; 
	res += "@";
	if(listenMode == FeEGE::EventType.frame) res += "FeEGE::EventType.frame";
	else if(listenMode == FeEGE::EventType.on_mouse_put_on) res += "FeEGE::EventType.on_mouse_put_on";
	else if(listenMode == FeEGE::EventType.on_mouse_hitting) res += "FeEGE::EventType.on_mouse_hitting";
	else if(listenMode == FeEGE::EventType.on_mouse_move_away) res += "FeEGE::EventType.on_mouse_move_away";
	else if(listenMode == FeEGE::EventType.on_click) res += "FeEGE::EventType.on_click";
	else if(listenMode == FeEGE::EventType.on_clone) res += "FeEGE::EventType.on_clone";
	else if(listenMode == FeEGE::EventType.on_mouse_hit_but_move_away) res += "FeEGE::EventType.on_mouse_hit_but_move_away";
	else if(listenMode == FeEGE::EventType.clones.on_clone) res += "FeEGE::EventType.clones.on_clone";
	else{
		LPCSTR text = TEXT(("Element::begin�����б������˲�ǡ�����¼�\n\nElement���� : " + this->id + "    �¼�id ��" + to_string(listenMode)).c_str());
		MessageBox(getHWnd(),text,"����",MB_ICONWARNING | MB_OK);
		return;
	}
	res += "@";
	res += identifier;
	ElementListenStoppedSet.erase(res);
}
void Element::stop(int listenMode,string identifier) {
	string res = this->id; 
	res += "@";
	if(listenMode == FeEGE::EventType.frame) res += "FeEGE::EventType.frame";
	else if(listenMode == FeEGE::EventType.on_mouse_put_on) res += "FeEGE::EventType.on_mouse_put_on";
	else if(listenMode == FeEGE::EventType.on_mouse_hitting) res += "FeEGE::EventType.on_mouse_hitting";
	else if(listenMode == FeEGE::EventType.on_mouse_move_away) res += "FeEGE::EventType.on_mouse_move_away";
	else if(listenMode == FeEGE::EventType.on_click) res += "FeEGE::EventType.on_click";
	else if(listenMode == FeEGE::EventType.on_clone) res += "FeEGE::EventType.on_clone";
	else if(listenMode == FeEGE::EventType.on_mouse_hit_but_move_away) res += "FeEGE::EventType.on_mouse_hit_but_move_away";
	else if(listenMode == FeEGE::EventType.clones.on_clone) res += "FeEGE::EventType.clones.on_clone";
	else{
		LPCSTR text = TEXT(("Element::stop�����б������˲�ǡ�����¼�\n\nElement���� : " + this->id + "    �¼�id ��" + to_string(listenMode)).c_str());
		MessageBox(getHWnd(),text,"����",MB_ICONWARNING | MB_OK);
		return;
	}
	res += "@";
	res += identifier;
	ElementListenStoppedSet.insert(res);
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

void Element::useGJK(){
	this->hittingBox = true;
	this->disabledDrawToPrivateImage = true; 
} 
void Element::unlistenGJK(){
	this->hittingBox = false;
	this->disabledDrawToPrivateImage = false;
}

void Element::addPolygon(const FeEGE::Polygon& shape){
	this->polygonSet.push_back(shape);
}

vector<FeEGE::Polygon> Element::getPolygonSet(){
	return this->polygonSet;
}

FeEGE::Polygon Element::transformPolygon(const FeEGE::Polygon& x){
	return transformPosition(transformShape(x,this->scale / 100.00f,Position{getwidth(this->imageVector[this->currentImage]) / 2.00f,getheight(this->imageVector[this->currentImage]) / 2.00f},this->angle / 180.00f *  PI),Position{getwidth(this->imageVector[this->currentImage]) / 2.00f,getheight(this->imageVector[this->currentImage]) / 2.00f},this->pos);
}

#ifdef TEST_FUNCTION 
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
#endif

// Move Animated
void Element::moveWithAnimation(double xPixel,double yPixel,const Animate& animate,function<void(Element*)> callback){
	static int cur = 0;
	animateStates[++ cur] = 0;
	Position p = animate.function(animate.end);
	animations.emplace_back(make_pair(make_pair(cur,make_pair((p.x == 0 ? 0 : xPixel / p.x),(p.y == 0 ? 0 : yPixel / p.y))),animate));
	if(callback == nullptr) return;
	this->animateCallbacks[cur] = callback;
}

Element::~Element() {
	
};

// ... (continue with all other method implementations)

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
    // ��鲢ȥ�� BOM
    std::string utf8StrClean = utf8Str;
    if (utf8StrClean.size() >= 3 &&
        static_cast<unsigned char>(utf8StrClean[0]) == 0xEF &&
        static_cast<unsigned char>(utf8StrClean[1]) == 0xBB &&
        static_cast<unsigned char>(utf8StrClean[2]) == 0xBF) {
        utf8StrClean.erase(0, 3); // ȥ�� BOM
    }

    // ��ȡ���������ȵķ���ֵ
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8StrClean.c_str(), -1, nullptr, 0);
    if (wideLen == 0) {
        throw std::runtime_error("MultiByteToWideChar failed");
    }

    // �� UTF-8 ת��Ϊ���ַ���UTF-16��
    std::wstring wideStr(wideLen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8StrClean.c_str(), -1, &wideStr[0], wideLen);

    // �����ַ���UTF-16��ת��Ϊ ANSI
    int ansiLen = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (ansiLen == 0) {
        throw std::runtime_error("WideCharToMultiByte failed");
    }

    std::string ansiStr(ansiLen, 0);
    WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), -1, &ansiStr[0], ansiLen, nullptr, nullptr);

    // ȥ��ĩβ�Ŀ��ַ�
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

void showText(string identifier,function<string(int)> stringSolver,function<Position(int)> positionSolver,function<color_t(int)> colorSolver,function<int(int)> scaleSolver,const string& fontName,int pType,int milliseconds){
	double beginTime = getMs();
	globalListen(EventType.frame,identifier,[=](){
		if(getMs() - beginTime > milliseconds){
			pushSchedule([=](){
				globalUnlisten(EventType.frame,identifier);
			});
		}
		Position pos = positionSolver(getMs() - beginTime);
		string str = stringSolver(getMs() - beginTime);
		color_t color = colorSolver(getMs() - beginTime);
		int scale = scaleSolver(getMs() - beginTime);
		textpen::type(pType);
		textpen::color(color);
		textpen::font(scale,fontName);
		textpen::print(pos.x,pos.y,str.c_str());
	});
}

ShowTextBuilder& ShowTextBuilder::setIdentifier(std::string id) {
    identifier = std::move(id);
    return *this;
}

ShowTextBuilder& ShowTextBuilder::setStringSolver(std::function<std::string(int)> f) {
    stringSolver = std::move(f);
    return *this;
}

ShowTextBuilder& ShowTextBuilder::setPositionSolver(std::function<Position(int)> f) {
    positionSolver = std::move(f);
    return *this;
}

ShowTextBuilder& ShowTextBuilder::setColorSolver(std::function<color_t(int)> f) {
    colorSolver = std::move(f);
    return *this;
}

ShowTextBuilder& ShowTextBuilder::setScaleSolver(std::function<int(int)> f) {
    scaleSolver = std::move(f);
    return *this;
}

ShowTextBuilder& ShowTextBuilder::setFontName(std::string name) {
    fontName = std::move(name);
    return *this;
}

ShowTextBuilder& ShowTextBuilder::setPType(int type) {
    pType = type;
    return *this;
}

ShowTextBuilder& ShowTextBuilder::setMilliseconds(int ms) {
	if(ms == -1) ms = 2147483645;
    milliseconds = ms;
    return *this;
}

void ShowTextBuilder::show() {
    // --- ���������� ---
    if (!stringSolver) {
        throw std::runtime_error("ShowTextBuilder error: stringSolver is not set.");
    }
    if (!positionSolver) {
        throw std::runtime_error("ShowTextBuilder error: positionSolver is not set.");
    }

    // identifier ����Ϊ��
    if (identifier.empty()) {
        throw std::runtime_error("ShowTextBuilder error: identifier is not set.");
    }

    // --- ����Ĭ�ϲ��� ---
    if (!colorSolver) {
        colorSolver = [](int) { return BLACK; };
    }

    if (!scaleSolver) {
        scaleSolver = [](int) { return 25; };
    }

    if (fontName.empty()) {
        fontName = "����";
    }

    if (pType == 0) {
        pType = FeEGE::PenType.middle;
    }

    // --- ��ѡ���ԣ������Լ�飨�����ڿɿ����� ---
    assert(stringSolver);
    assert(positionSolver);
    assert(!fontName.empty());

    // --- ���õײ� showText ---
    showText(identifier, stringSolver, positionSolver, colorSolver, scaleSolver, fontName, pType, milliseconds);
}
 
void regElement(Element* element) {
    element->setRegOrder(currentRegOrder ++);
    elementQueue.insert(element);
    __SIZE__ ++;
    elementIsIn[element] = true;
    idToElement[element->getId()] = element;
}

Element* newElement(string id,string imagePath,double x,double y) {
	imagePath = resolvePath(imagePath);
	PIMAGE image = getImage(imagePath);
	for(int i = 0; i < MAXELEMENTCOUNT; ++ i) {
		if(!elementPoolUsed[i]) {
			elementPoolUsed[i] = true;
			Element* e = elementPool[i].copy(id,image,i,x,y);
			regElement(e);
			e->setType("Normal@native");
			return e;
		}
	}
	MessageBox(getHWnd(),"����Elementʧ��(�ﵽ�������)","��ʾ",MB_OK);
	return nullptr;
}

#if true
Element* newElement(string id,PIMAGE image,double x,double y) {
	for(int i = 0; i < MAXELEMENTCOUNT; ++ i) {
		if(!elementPoolUsed[i]) {
			elementPoolUsed[i] = true;
			Element* e = elementPool[i].copy(id,image,i,x,y);
			regElement(e);
			return e;
		}
	}
	MessageBox(getHWnd(),"����Elementʧ��(�ﵽ�������)","��ʾ",MB_OK);
	return nullptr;
}
#endif

PIMAGE getImage(const string& imagePath){
	PIMAGE image = newimage();
	if(imagePath != "") getimage(image,TEXT(imagePath.c_str()));
	return image;
}

void globalListen(int listenMode, string identifier, std::function<void(void)> function){
	if(listenMode == FeEGE::EventType.frame) globalListenFrameFunctionSet[identifier] = function ;
	if(listenMode == FeEGE::EventType.on_click) globalListenOnClickFunctionSet[identifier] = function ;
}

void globalUnlisten(int listenMode,string identifier){
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
	if(!is_run()) return;
	cleardevice();

	for(Element* it : elementQueue) {
		if(it == nullptr || it == *elementQueue.end()) continue;
		if(penNprinted && it->getOrder() >= pen::order) {
			penNprinted = false;
			if(!is_run()) return;
			putimage_alphatransparent(nullptr,pen_image,0,0,EGERGBA(1,1,4,0),pen::penAlpha);
		}
		it->call();
	}
	if(!is_run()) return;
	if(penNprinted) putimage_alphatransparent(nullptr,pen_image,0,0,EGERGBA(1,1,4,0),pen::penAlpha);
	if(!is_run()) return;
	putimage_alphatransparent(nullptr,textpen_image,0,0,EGERGBA(1,1,4,0),textpen::penAlpha);
	#ifdef FPS
	static char fps[32];
	memset(fps,0,sizeof(fps));
	sprintf(fps,"FPS : %0.2f",getfps());
	setcaption(fps);
	#endif
	#ifndef bigScreen
	builtinGetKey();
	#endif
	
	// UI
	mouse_msg msg;
    while (mousemsg()) {
        msg = getmouse();
        reflushMouseStatu(msg);
        for(Widget* w : widgets){
        	w->handleEvent(msg);
		}
    }
    for(Widget* w : widgets){
		if(!w->is_global) continue;
		if(!is_run()) return;
    	w->draw();
	}
	
	POINT pt;
	::GetCursorPos(&pt);            // ��ȡ��ǰ���λ��
	::SetCursorPos(pt.x, pt.y);    // ���ûص�ǰ���λ�ã���������¼�
}

void init(int x,int y,int mode){
	setinitmode(mode);
	initgraph(x,y,mode);
	initXY();
	FeEGE::initPen();
	FeEGE::initTextPen();
	ege_enable_aa(true);
}

void start() {
try {
	closeGraph = false;
	initXY();
	randomize();
	while(!closeGraph && is_run()) {
		reflush();
		textpen::clearAll();
		if(!is_run()) return;
		for(auto it : globalListenFrameFunctionSet) it.second();
#ifndef NO_THREAD
#ifdef Ppause
		if(regPause){
			if(FeEGE::getkey('p') || FeEGE::getkey('P')) {
				this_thread::sleep_for(chrono::milliseconds(200));
				while(!FeEGE::getkey('p') && !FeEGE::getkey('P')) this_thread::sleep_for(chrono::milliseconds(1));
				this_thread::sleep_for(chrono::milliseconds(200));
			}
		}
#endif
#ifdef ESCexit
		if(FeEGE::getkey(VK_ESCAPE)) break;
#endif
#endif
		if(!is_run()) return;
		delay_fps(120);
		if(!is_run()) return;
	}

	closegraph();
}
catch (...) {
    // ���Լ�¼��־�������ʾ��Ϣ
    std::cout << "ͼ����Դ�����쳣�����ڿ����ѹرա�\n";
}
}

// ... (continue with all other function implementations)
