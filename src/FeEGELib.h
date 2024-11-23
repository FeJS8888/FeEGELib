#ifndef _FEEGELIB_
#define _FEEGELIB_

#define FeEGELib_version "V1.2.13.0--upd2024-11-23"
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

#ifndef MAXCLONESCOUNT
#define MAXCLONESCOUNT 100
#endif

#ifndef MAXELEMENTCOUNT
#define MAXELEMENTCOUNT 100
#endif

#define LeftButton VK_LBUTTON
#define log printf
#define PIE 3.141592653589793238462643383279502f

#define initXY() {WIDTH = getwidth();HEIGHT = getheight();}
#define X WIDTH
#define Y HEIGHT

using namespace std;

const double eps = 1e-7;
double GlobalRating = 1.00f;

class Position;
class Element;
class InputBox;
class InputBoxSet;
void reg_Element(Element* element);
Element* newElement(string,PIMAGE,double,double);
Element* newElement(string,string,double,double);

bool iskey = false;
key_msg keymsg;
bool mousehit = false;
bool needsort = true;
bool reg_pause = false;
mouse_msg mouseinfo;
bool keystatus[360];
map<string,function<void(void)>> globalListen_frame_function_set;
map<string,function<void(void)>> globalListen_on_click_function_set;

int WIDTH;
int HEIGHT;

vector<function<void(void)> > schedule;
map<double,vector<function<void(void)> > > schedule_timeOut;
int __SIZE__ = 0;
int removesize = 0;
bool closeGraph = false;

PIMAGE pen_image;

unordered_map<string,Element*>IdToElement;
unordered_map<Element*,bool>ElementIsIn;

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
} lastpixel;

namespace FeEGE {
	short getkey(int VK) {
		return GetAsyncKeyState(VK);
	}
	void enable_pause() {
		reg_pause = true;
	}
	void  disable_pause() {
		reg_pause = false;
	}
	Element* getElementById(string);
	Element* getElementByPtr(Element*);
	void initpen() {
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
	
	void push_schedule(function<void(void)> func) {
		schedule.push_back(func);
	}
	
	void setTimeOut(function<void(void)> func,double time_ms){
		schedule_timeOut[getMs() + time_ms].emplace_back(func);
	}

	class ClonesEvent {
		public:
			int on_clone = 0x06;
	};
	
	class InputBoxEvent {
		public:
			int on_select = 0x06;
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
			InputBoxEvent InputBox;
	};

	Events EventType;

	class PenTypes {
		public:
			int left = 0x01;
			int middle = 0x02;
	};

	PenTypes PenType;

	Position& getLastPixel() {
		return lastpixel;
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
	Animate EaseInOutCubicX([&](double tick) -> Position { return {tick <= 0.5 ? 4 * tick * tick * tick : 1 - pow(-2 * tick + 2,3) / 2,0}; },0.002,1);
	Animate EaseInOutCubicY([&](double tick) -> Position { return {0,tick <= 0.5 ? 4 * tick * tick * tick : 1 - pow(-2 * tick + 2,3) / 2}; },0.002,1);
	Animate EaseOutQuadX([&](double tick) -> Position { return {1 - (1 - tick) * (1 - tick),0}; },0.002,1);
	Animate EaseOutQuadY([&](double tick) -> Position { return {0,1 - (1 - tick) * (1 - tick)}; },0.002,1);
}

class Element {
	protected:
		//Variables
		string id;
		string ElementType;
		short scale;
		int angle;
		int order;
		short alpha;
		int reg_order;
		Position pos;
		Position backup_pos;
		bool is_cancel_x;
		bool is_cancel_y;
		bool is_show;
		PIMAGE __visible_image;
		vector<PIMAGE> image_vector;
		vector<Element*> clones;
		vector<Element*> removeList;
		map<string,function<void(Element*)> > frame_function_set;
		map<string,function<void(Element*)> > on_mouse_put_on_function_set;
		map<string,function<void(Element*)> > on_mouse_hitting_function_set;
		map<string,function<void(Element*)> > on_mouse_move_away_function_set;
		map<string,function<void(Element*)> > on_click_function_set;
		map<string,function<void(Element*)> > on_clone_function_set;
		map<string,function<void(Element*)> > on_clone_clones_function_set;
		vector<pair<pair<double,pair<double,double> >,Animate> > animations;
		unordered_map<int,double> animate_states;
		unordered_map<int,function<void(Element*)> > animate_callbacks;
		unsigned int current_image = 0;
		unordered_map<int,long long> private_variables;
		bool deletedList[MAXCLONESCOUNT] = {};
		int clonecount = 0;
		int nextclonecount = 0;
		bool deleted = false;
		bool image_lock = false;
		bool disabled_draw_to_private_image = false;
		Element** clonequeue = nullptr;
		bool drawed = false;
		int PoolIndex;
		bool HittingBox = false;
		int HBheight,HBwidth;

		bool PhysicEngineStatu;
		double ForceX;
		double ForceY;
		double SpeedX;
		double SpeedY;
		double ResistanceX;
		double ResistanceY;

		vector<color_t> remove_colors;

		inline virtual void reflush_mouse_statu() {
			/*
				Test click
			*/
			if(this->ismousein()){
				int statu = this->get_variable(0);
				if(this->ishit()){
					if(statu == 1){
						this->set_variable(0,2);
						for(auto it :this->on_mouse_hitting_function_set) it.second(this);
					}
				} 
				else {
					if(statu == 0){
						this->set_variable(0,1);
						for(auto it : this->on_mouse_put_on_function_set) it.second(this);
					} 
					else if(statu == 2) {
						this->set_variable(0,0);
						for(auto it : this->on_click_function_set) it.second(this);
					}
				}
			} 
			else {
				if(this->get_variable(0) == 1){
					this->set_variable(0,0);
					for(auto it : this->on_mouse_move_away_function_set) it.second(this);
				}
				else if(this->get_variable(0) == 2 && !GetAsyncKeyState(LeftButton)){
					this->set_variable(0,0);
					for(auto it : this->on_mouse_move_away_function_set) it.second(this);
				}
			}
		}
		inline bool draw_to_private_image() {
			if(this->drawed) return false;
			if(this->deleted) return false;
			if(this->disabled_draw_to_private_image) return false;
			if(this->image_vector[this->current_image] == nullptr) return false;
			cleardevice(this->__visible_image);
			putimage_rotatezoom(this->__visible_image,this->image_vector[this->current_image],this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f * PIE,this->scale / 100.00f,1);
			return this->drawed = true;
		}
	public:
		//Functions
		Element(string id,PIMAGE image,Position pos) {
			this->id = id;
			this->__visible_image = newimage(getwidth(),getheight());
			setbkcolor(EGERGBA(1,1,4,0),this->__visible_image);
			this->pos = pos;
			this->image_vector.resize(100);
			this->image_vector[0] = image;
			this->is_show = true;
			this->scale = 100;
			this->angle = 0;
			this->order = 0;
			this->alpha = 0xFF;
			this->deleted= false;
			this->PhysicEngineStatu = false;
			this->ForceX = 0.00;
			this->ForceY = 0.00;
			this->SpeedX = 0.00;
			this->SpeedY = 0.00;
			this->ResistanceX = 0.00;
			this->ResistanceY = 0.00;
			for(int i = 0; i < 10; ++ i) this->private_variables[i] = 0;
		}
		Element(string id,PIMAGE image,double x = 0,double y = 0) {
			this->id = id;
			this->__visible_image = newimage(getwidth(),getheight());
			setbkcolor(EGERGBA(1,1,4,0),this->__visible_image);
			this->pos = *(new Position(x,y));
			this->image_vector.resize(100);
			this->image_vector[0] = image;
			this->is_show = true;
			this->scale = 100;
			this->angle = 0;
			this->order = 0;
			this->alpha = 0xFF;
			this->deleted= false;
			this->PhysicEngineStatu = false;
			this->ForceX = 0.00;
			this->ForceY = 0.00;
			this->SpeedX = 0.00;
			this->SpeedY = 0.00;
			for(int i = 0; i < 10; ++ i) this->private_variables[i] = 0;
		}
		Element() { }
		inline Element* copy(string id,PIMAGE image,unsigned long long index,double x = 0,double y = 0) {
			this->id = id;
			this->__visible_image = newimage(getwidth(),getheight());
			setbkcolor(EGERGBA(1,1,4,0),this->__visible_image);
			this->pos.x = x;
			this->pos.y = y;
			this->image_vector.resize(100);
			this->image_vector[0] = image;
			this->is_show = true;
			this->scale = 100;
			this->angle = 0;
			this->order = 0;
			this->alpha = 0xFF;
			this->deleted= false;
			for(int i = 0; i < 10; ++ i) this->private_variables[i] = 0;
			this->PoolIndex = index;
			this->PhysicEngineStatu = false;
			this->ForceX = 0.00;
			this->ForceY = 0.00;
			this->SpeedX = 0.00;
			this->SpeedY = 0.00;
			return this;
		}
		
		inline void cancel_animate(const Animate& animate,double tick,double x,double y){
			Position p = animate.function(tick);
			this->pos.x -= p.x * x;
			this->pos.y -= p.y * y;
		}
		
		inline void run_animate(const Animate& animate,double tick,double x,double y){
			Position p = animate.function(tick);
			this->pos.x += p.x * x;
			this->pos.y += p.y * y;
		}
		
		inline void call_animations(){
			vector<pair<pair<double,pair<double,double> >,Animate> > q;
			for(auto& p : this->animations){
				int w = p.first.first;
				Animate& animate = p.second;
				cancel_animate(animate,this->animate_states[w],p.first.second.first,p.first.second.second);
				this->animate_states[w] += animate.speed;
				if(this->animate_states[w] >= animate.end){
					run_animate(animate,animate.end,p.first.second.first,p.first.second.second);
					this->animate_states.erase(w);
					auto it = this->animate_callbacks.find(w);
					if(it == this->animate_callbacks.end()) continue;
					it->second(this);
					this->animate_callbacks.erase(it);
					continue;
				}
				run_animate(animate,this->animate_states[w],p.first.second.first,p.first.second.second);
				q.emplace_back(p);
			}
			this->animations.swap(q);
		}
		
		inline virtual void call() {
			this->backup_pos = pos;
			this->reflush_mouse_statu();
			for(auto it : this->frame_function_set) it.second(this);
			if(this->deleted) return;
			this->call_animations();
			if(!this->is_show) return;

			//backup
			if(this->is_cancel_x) {
				this->is_cancel_x = false;
				this->pos.x = this->backup_pos.x;
			}
			if(this->is_cancel_y) {
				this->is_cancel_y = false;
				this->pos.y = this->backup_pos.y;
			}
			
			this->drawed = false;
#ifdef TEST_FUNCTION
			if(this->PhysicEngineStatu) {
				this->UpdatePhysicSpeed();
				this->UpdatePhysicPosition();
			}
#endif
			putimage_rotatezoom(nullptr,this->image_vector[this->current_image],this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f * PIE,this->scale / 100.00f,1,this->alpha);
		}
		inline void set_type(const string& type){
			this->ElementType = type;
		}
		inline string get_type(const string& type){
			return this->ElementType;
		}
		inline void move_left(double pixels = 0) {
			this->pos.x -= pixels;
		}
		inline void move_right(double pixels = 0) {
			this->pos.x += pixels;
		}
		inline void move_up(double pixels = 0) {
			this->pos.y -= pixels;
		}
		inline void move_down(double pixels = 0) {
			this->pos.y += pixels;
		}
		inline void move_forward(double pixels = 0) {
			this->pos.x -= sin(this->angle * PIE / 180.00f) * pixels;
			this->pos.y -= cos(this->angle * PIE / 180.00f) * pixels;
		}
		inline void move_to(double x,double y) {
			this->pos.x = x;
			this->pos.y = y;
		}
		inline void move_to(Position position) {
			this->pos = position;
		}
		inline color_t get_pixel(int x,int y) {
			return getpixel(x,y,this->__visible_image);
		}
		inline short get_scale() {
			return this->scale;
		}
		inline void increase_scale(short scale) {
			if(this->scale + scale <= 255) this->scale += scale;
			else this->scale = 255;
			this->scale %= 255;
			if(this->scale < 0) this->scale += 255;
		}
		inline void decrease_scale(short scale) {
			if(this->scale > scale) this->scale -= scale;
			else this->scale = 0;
			this->scale %= 255;
			if(this->scale < 0) this->scale += 101;
		}
		inline void set_scale(short scale) {
			this->scale = scale;
			this->scale %= 255;
		}
		inline void show() {
			this->is_show = true;
		}
		inline void hide() {
			this->is_show = false;
		}
		inline void turn_right(int angle) {
			this->angle = (this->angle - angle) % 360;
		}
		inline void turn_left(int angle) {
			this->angle = (this->angle + angle) % 360;
		}
		inline void turn_to(int angle) {
			this->angle = angle % 360;
		}
		inline bool face_to(Element* that){
			if(that == nullptr) {
				LPCSTR text = TEXT(("Element::face_to方法被错误的传入了nullptr参数\n这可能是由于getElementById查询了不存在的对象\n\nElement名称 : " + this->id).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
				return false; 
			}
			Position pos = that->get_position();
			double dx = pos.x - this->pos.x;
			double dy = pos.y - this->pos.y;
			if(!sgn(dx)) return true;
			if(!sgn(dy)){
				if(dx > 0) this->angle = 0;
				else this->angle = 180;
				return true;
			}
			this->angle = atan2(dy,dx) / PIE * 180.00f;
			return true;
		}
		inline int get_angle() {
			return this->angle;
		}
		inline Position get_position() {
			return this->pos;
		}
		inline bool getisshow() {
			return this->is_show;
		}
		inline void increase_order(int count) ;
		inline void decrease_order(int count);
		inline void set_order(int count);
		inline void set_reg_order(int count) {
			this->reg_order = count;
		}
		inline int get_reg_order() {
			return this->reg_order;
		}
		inline int get_order() {
			return this->order;
		}
		bool operator<(Element _B) {
			if(this->get_order() > _B.get_order()) return false;
			else if (this->get_order() < _B.get_order()) return true;
			return this->get_reg_order() < _B.get_reg_order();
		}
		bool operator>(Element _B) {
			if(this->get_order() < _B.get_order()) return false;
			else if (this->get_order() > _B.get_order()) return true;
			return this->get_reg_order() > _B.get_reg_order();
		}
		inline void set_posx(int x) {
			this->pos.x = x;
		}
		inline void set_posy(int y) {
			this->pos.y = y;
		}
		inline void remove_color(color_t color) {
			this->remove_colors.push_back(color);
		}
		inline bool ismousein() {
			if(GetForegroundWindow() != getHWnd()) return false;
			if(!this->is_show) return false;
			int x,y;
			mousepos(&x,&y);
			if(x < 0 || y < 0 || x > WIDTH || y > HEIGHT) return false;
			this->draw_to_private_image();
			color_t pixel = getpixel(x,y,this->__visible_image);
			int size = remove_colors.size();
			for(int i = 0; i < size; ++ i) if(pixel == remove_colors[i]) return false;
			return ((EGEGET_A(pixel) != 0) || (pixel != 65796)); //EGERGBA(1,1,4,0) = 65796
		}
		inline bool ishit() {
			if(!GetAsyncKeyState(LeftButton)) return false;
			return this->ismousein();
		}
		inline void set_variable(unsigned int which,long long value) {
			this->private_variables[which] = value;
//			// cout<<which<<" "<<value<<" "<<this<<" "<<this->id<<endl;
		}
		inline long long& get_variable(unsigned int which) {
			return this->private_variables[which];
		}
		inline void increase_variable(unsigned int which,long long value) {
			this->private_variables[which] += value;
		}
		inline void decrease_variable(unsigned int which,long long value) {
			this->private_variables[which] -= value;
		}
		inline void add_image(PIMAGE image) {
			this->image_vector.push_back(image);
		}
		inline bool set_image(int order) {
			if(this->image_vector[order] != nullptr){
				this->current_image = order;
				return true;
			}
			return false;
		}
		inline int get_image_order() {
			return this->current_image;
		}
		inline PIMAGE get_image() {
			return this->image_vector[this->current_image];
		}
		inline bool is_touched_by(Element* that) {
			if(that == nullptr) {
				LPCSTR text = TEXT(("Element::is_touched_by方法被错误的传入了nullptr参数\n这可能是由于getElementById查询了不存在的对象\n\nElement名称 : " + this->id).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
				return false; 
			}
			if(!this->is_show || !that->is_show) return false;
			
			if(this->HittingBox ^ that->HittingBox){
				LPCSTR text = TEXT(("Element::is_touched_by方法中两个对象的碰撞箱状态不一致\n\nElement名称 : " + this->id).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
				return false;
			}
			if(this->HittingBox){
				double thisleft = this->pos.x - (this->HBwidth >> 1) * this->scale / 100.00;
				double thatleft = that->pos.x - (that->HBwidth >> 1) * that->scale / 100.00;
				double thisright = this->pos.x + (this->HBwidth >> 1) * this->scale / 100.00;
				double thatright = that->pos.x + (that->HBwidth >> 1) * that->scale / 100.00;
				double thistop = this->pos.y - (this->HBheight >> 1) * this->scale / 100.00;
				double thattop = that->pos.y - (that->HBheight >> 1) * that->scale / 100.00;
				double thisbottom = this->pos.y + (this->HBheight >> 1) * this->scale / 100.00;
				double thatbottom = that->pos.y + (that->HBheight >> 1) * that->scale / 100.00;
				if((thisleft <= thatright && (thistop <= thatbottom && thisbottom >= thattop)) || (thisright >= thatleft && (thistop <= thatbottom && thisbottom >= thattop))) return true;
				return false;
			}
			
			this->draw_to_private_image();
			for(int x = this->pos.x - getwidth(this->image_vector[this->current_image]); x <= this->pos.x + getwidth(this->image_vector[this->current_image]); ++ x) {
				for(int y = this->pos.y - getheight(this->image_vector[this->current_image]); y <= this->pos.y + getheight(this->image_vector[this->current_image]); ++ y) {
					if(x < 0 || y < 0 || x >= getwidth() || y >= getheight()) continue;
					if(getpixel(x,y,this->__visible_image) == 65796) continue;
					if(getpixel(x,y,that->__visible_image) == 65796) continue;
					lastpixel = {(double)x,(double)y};
					return true;
				}
			}
		}
		inline Element* clone() {
			static Element* e[MAXCLONESCOUNT];
			for(int i = 0; i < nextclonecount; ++ i) {
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
			for(int i = 0; i < nextclonecount; ++ i) if(!this->deletedList[i]) {
					deletedList[i] = true;
					e[i] = newElement(this->id + "_" + to_string(clonecount ++),this->get_image(),this->pos.x,this->pos.y);
					e[i]->angle = this->angle;
					e[i]->scale = this->scale;
					e[i]->set_variable(1,(long long)this);
					e[i]->disabled_draw_to_private_image = this->disabled_draw_to_private_image;
					e[i]->is_show = this->is_show;
					reg_Element(e[i]);
					for(auto it : this->on_clone_function_set) it.second(this);
					for(auto it : this->on_clone_clones_function_set) it.second(e[i]);
					return e[i];
				}
			deletedList[nextclonecount] = true;
			e[nextclonecount] = newElement(this->id + "_" + to_string(clonecount ++),this->get_image(),this->pos.x,this->pos.y);
			e[nextclonecount]->angle = this->angle;
			e[nextclonecount]->scale = this->scale;
			e[nextclonecount]->set_variable(1,(long long)this);
			e[nextclonecount]->is_show = this->is_show;
			reg_Element(e[nextclonecount]);
			for(auto it : this->on_clone_function_set) it.second(this);
			for(auto it : this->on_clone_clones_function_set) it.second(e[nextclonecount]);
			return e[nextclonecount ++];
		}
		inline string getId() {
			return this->id;
		}
		inline virtual void listen(int listen_mode,string identifier,function<void(Element*)> function) {
			if(listen_mode == FeEGE::EventType.frame) this->frame_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.on_mouse_put_on) this->on_mouse_put_on_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.on_mouse_hitting) this->on_mouse_hitting_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.on_mouse_move_away) this->on_mouse_move_away_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.on_click) this->on_click_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.on_clone) this->on_clone_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.clones.on_clone) this->on_clone_clones_function_set[identifier] = function ;
			else{
				LPCSTR text = TEXT(("Element::listen方法中被传入了不恰当的事件\n\nElement名称 : " + this->id + "    事件id ：" + to_string(listen_mode)).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
			}
		}
		inline virtual void stop(int listen_mode,string identifier) {
			if(listen_mode == FeEGE::EventType.frame) this->frame_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.on_mouse_put_on) this->on_mouse_put_on_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.on_mouse_hitting) this->on_mouse_hitting_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.on_mouse_move_away) this->on_mouse_move_away_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.on_click) this->on_click_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.on_clone) this->on_clone_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.clones.on_clone) this->on_clone_clones_function_set.erase(identifier) ;
			else{
				LPCSTR text = TEXT(("Element::stop方法中被传入了不恰当的事件\n\nElement名称 : " + this->id + "    事件id ：" + to_string(listen_mode)).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
			}
		}
		inline Element* deletethis();
		inline void cancel_x() {
			this->is_cancel_x = true;
		}
		inline void cancel_y() {
			this->is_cancel_y = true;
		}
		inline void cancel_move() {
			this->is_cancel_x = true;
			this->is_cancel_y = true;
		}
		inline short get_alpha() {
			return this->alpha;
		}
		inline void set_alpha(short alpha) {
			this->alpha = alpha;
			this->alpha %= 256;
			if(this->alpha < 0) this->alpha += 256;
		}
		inline void decrease_alpha(short alpha) {
			if(this->alpha - alpha < 0) this->alpha = 0;
			else this->alpha -= alpha;
			this->alpha %= 256;
			if(this->alpha < 0) this->alpha += 256;
		}
		inline void increase_alpha(short alpha) {
			if(this->alpha + alpha > 255) this->alpha = 255;
			else this->alpha += alpha;
			this->alpha %= 256;
			if(this->alpha < 0) this->alpha += 256;
		}
		inline void nextimage() {
			do {
				this->current_image ++;
				this->current_image %= 100;
			} while(this->image_vector[current_image] == nullptr);
		}
#ifndef NO_THREAD
		inline void loadImage(string ImagePath,int id) {
			thread([this,ImagePath,id]() {
				PIMAGE image = newimage();
				int statu = 0;
				do {
					statu = getimage(image,TEXT(ImagePath.c_str()));
					this_thread::sleep_for(chrono::milliseconds(10));
				} while(statu != 0);
				while(this->image_lock) this_thread::sleep_for(chrono::milliseconds(1));
				this->image_lock = true;
				this->image_vector[id] = image;
				this->image_lock = false;
			}).detach();
		}
		inline void deleteImage(int id) {
			thread([this,id]() {
				delimage(this->image_vector[id]);
			}).detach();
		}
#endif
		inline void disable_draw_to_private_image() {
			this->disabled_draw_to_private_image = true;
		}
		inline void enable_draw_to_private_image() {
			this->disabled_draw_to_private_image = false;
		}
		inline void ignoreBlack() {
			int XX = getwidth(this->image_vector[this->current_image]);
			int YY = getheight(this->image_vector[this->current_image]);
			for(int i = 0; i < XX; ++ i) {
				for(int j = 0; j < YY; ++ j) {
					if(getpixel(i,j,this->image_vector[this->current_image]) == (color_t)BLACK) putpixel_f(i,j,EGERGBA(0,0,0,0),this->image_vector[this->current_image]);
				}
			}
		}
#ifdef TEST_FUNCTION 
		inline void useHittingBox(){
			this->HittingBox = true;
		} 
		inline void stopHittingBox(){
			this->HittingBox = false;
		}

		// PhysicEngine
		inline void enablePhysicEngine() {
			this->PhysicEngineStatu = true;
		}
		inline void disablePhysicEngine() {
			this->PhysicEngineStatu = false;
		}
		inline void PhysicForce(double angle,double force) {
			this->ForceX += cos(angle * PIE / 180.00) * force;
			this->ForceY -= sin(angle * PIE / 180.00) * force;
		}
		inline void PhysicSpeed(double angle,double speed) {
			this->SpeedX += cos(angle * PIE / 180.00) * speed;
			this->SpeedY -= sin(angle * PIE / 180.00) * speed;
		}
		inline void PhysicResistance(double angle,double resistance) {
			this->ResistanceX += cos(angle * PIE / 180.00) * resistance;
			this->ResistanceY -= sin(angle * PIE / 180.00) * resistance;
		}
		inline void UpdatePhysicSpeed() {
			this->SpeedX += ForceX;
			this->SpeedY += ForceY;
			bool XX = this->SpeedX * ResistanceX > 0;
			bool YY = this->SpeedY * ResistanceY > 0;
			if(XX && fabs(this->SpeedX) >= fabs(this->ResistanceX)) this->SpeedX -= this->ResistanceX;
			else if(XX)this->SpeedX = 0;
			if(YY && fabs(this->SpeedY) >= fabs(this->ResistanceY)) this->SpeedY -= this->ResistanceY;
			else if(YY) this->SpeedY = 0;
		}
		inline void UpdatePhysicPosition() {
			this->pos.x += SpeedX * GlobalRating;
			this->pos.y += SpeedY * GlobalRating;
//			cout<<"updated\n";
		}
		inline double getSpeedX() {
			return this->SpeedX;
		}
		inline double getSpeedY() {
			return this->SpeedY;
		}
		inline void PhysicArcSpeedX() {
			this->SpeedX = -this->SpeedX;
		}
		inline void PhysicArcSpeedY() {
			this->SpeedY = -this->SpeedY;
		}
		inline void PhysicRemoveSpeedX() {
			this->SpeedX = 0;
		}
		inline void PhysicRemoveSpeedY() {
			this->SpeedY = 0;
		}
		inline void PhysicRemoveForceX() {
			this->ForceX = 0;
		}
		inline void PhysicRemoveForceY() {
			this->ForceY = 0;
		}
		
		// Move Animated
		inline void move_with_animation(double xpixel,double ypixel,const Animate& animate,function<void(Element*)> callback = nullptr){
			static int cur = 0;
			animate_states[++ cur] = 0;
			Position p = animate.function(animate.end);
			animations.emplace_back(make_pair(make_pair(cur,make_pair((p.x == 0 ? 0 : xpixel / p.x),(p.y == 0 ? 0 : ypixel / p.y))),animate));
			if(callback == nullptr) return;
			this->animate_callbacks[cur] = callback;
		}
#endif

		inline Element* deleteElement();
		~Element() { };
};

class InputBoxSet{
	protected:
		bool is_enabled;
		set<Element*> childs;
		Element* selected_InputBox;
		
	public:
		InputBoxSet(){
			this->is_enabled = false;
		}
		inline void enable(){
			this->is_enabled = true;
		}
		inline void disable(){
			this->is_enabled = false;
		}
		inline bool get_enable_statu(){
			return this->is_enabled;
		}
		inline void insert(Element* inputbox){
			this->childs.insert(inputbox);
		}
		inline void erase(Element* inputbox){
			this->childs.erase(inputbox); 
		}
		inline void select(Element* inputbox){
			
		}
		inline void call(){
			for(Element* inputbox : childs){
				inputbox->call();
			}
		}
		~InputBoxSet(){	}
};

class InputBox : public Element{
	protected:
		bool is_enabled;
		InputBoxSet* father;
		Element* submit_button;
	public:
		InputBox(){
			this->father = nullptr;
			this->submit_button = nullptr;
		}
		InputBox(InputBoxSet* father,Element* submit_button): father(father),submit_button(submit_button) { };
		inline void bind(InputBoxSet* father){
			if(this->father != nullptr) this->father->erase(this);
			this->father = father;
		}
		inline void enable(){
			this->is_enabled = true;
		}
		inline void disable(){
			this->is_enabled = false;
		}
		inline bool get_enable_statu(){
			return this->is_enabled;
		}
		inline virtual void listen(int listen_mode,string identifier,function<void(Element*)> function) {
			if(listen_mode == FeEGE::EventType.frame) this->frame_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.on_mouse_put_on) this->on_mouse_put_on_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.on_mouse_hitting) this->on_mouse_hitting_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.on_mouse_move_away) this->on_mouse_move_away_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.on_click) this->on_click_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.on_clone) this->on_clone_function_set[identifier] = function ;
			else if(listen_mode == FeEGE::EventType.clones.on_clone) this->on_clone_clones_function_set[identifier] = function ;
			else{
				LPCSTR text = TEXT(("InputBox::listen方法中被传入了不恰当的事件\n\nInputBox名称 : " + this->id + "    事件id ：" + to_string(listen_mode)).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
			}
		}
		inline virtual void stop(int listen_mode,string identifier) {
			if(listen_mode == FeEGE::EventType.frame) this->frame_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.on_mouse_put_on) this->on_mouse_put_on_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.on_mouse_hitting) this->on_mouse_hitting_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.on_mouse_move_away) this->on_mouse_move_away_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.on_click) this->on_click_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.on_clone) this->on_clone_function_set.erase(identifier) ;
			else if(listen_mode == FeEGE::EventType.clones.on_clone) this->on_clone_clones_function_set.erase(identifier) ;
			else{
				LPCSTR text = TEXT(("InputBox::stop方法中被传入了不恰当的事件\n\nInputBox名称 : " + this->id + "    事件id ：" + to_string(listen_mode)).c_str());
				MessageBox(getHWnd(),text,"警告",MB_ICONWARNING | MB_OK);
			}
		}
		inline void reflush_mouse_statu() override {
			/*
				Test click
			*/
			if(this->ismousein()) {
				int statu = this->get_variable(0);
				if(this->ishit()) {
					if(statu == 1) {
						this->set_variable(0,2);
						if(this->is_enabled)
							for(auto it :this->on_mouse_hitting_function_set) it.second(this);
					}
				} else {
					if(statu == 0) {
						this->set_variable(0,1);
						if(this->is_enabled)
							for(auto it : this->on_mouse_put_on_function_set) it.second(this);
					} else if(statu == 2) {
						this->set_variable(0,0);
						if(this->is_enabled){
							this->father->select(this);
							for(auto it : this->on_click_function_set) it.second(this);
						}
					}
				}
			} else {
				if(this->get_variable(0) == 1)	{
					for(auto it : this->on_mouse_move_away_function_set) it.second(this);
					if(this->is_enabled)
							this->set_variable(0,0);
				}
			}
		}
		inline void call() override {
			this->backup_pos = pos;
			this->reflush_mouse_statu();
			for(auto it : this->frame_function_set) it.second(this);
			if(this->ishit())
			if(this->deleted) return;
			if(!this->is_show) return;

			//backup
			if(this->is_cancel_x) {
				this->is_cancel_x = false;
				this->pos.x = this->backup_pos.x;
			}
			if(this->is_cancel_y) {
				this->is_cancel_y = false;
				this->pos.y = this->backup_pos.y;
			}
			
			this->drawed = false;
			putimage_rotatezoom(nullptr,this->image_vector[this->current_image],this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f * PIE,this->scale / 100.00f,1,this->alpha);
		}
		~InputBox(){
			this->father->erase(this);
		};
};

class Button : public Element{
	protected:
		bool is_enabled;
	public:
		Button(){
			this->is_enabled = false;
		}
		inline void enable(){
			this->is_enabled = true;
		}
		inline void disable(){
			this->is_enabled = false;
		}
		inline bool get_enable_statu(){
			return this->is_enabled;
		}
		inline void reflush_mouse_statu() override {
			/*
				Test click
			*/
			if(this->ismousein()) {
				int statu = this->get_variable(0);
				if(this->ishit()) {
					if(statu == 1) {
						this->set_variable(0,2);
						if(this->is_enabled)
							for(auto it :this->on_mouse_hitting_function_set) it.second(this);
					}
				} else {
					if(statu == 0) {
						this->set_variable(0,1);
						if(this->is_enabled)
							for(auto it : this->on_mouse_put_on_function_set) it.second(this);
					} else if(statu == 2) {
						this->set_variable(0,0);
						if(this->is_enabled)
							for(auto it : this->on_click_function_set) it.second(this);
					}
				}
			} else {
				if(this->get_variable(0) == 1)	{
					for(auto it : this->on_mouse_move_away_function_set) it.second(this);
					if(this->is_enabled)
							this->set_variable(0,0);
				}
			}
		}
		inline void call() override {
			this->backup_pos = pos;
			this->reflush_mouse_statu();
			for(auto it : this->frame_function_set) it.second(this);
			if(this->ishit())
			if(this->deleted) return;
			if(!this->is_show) return;

			//backup
			if(this->is_cancel_x) {
				this->is_cancel_x = false;
				this->pos.x = this->backup_pos.x;
			}
			if(this->is_cancel_y) {
				this->is_cancel_y = false;
				this->pos.y = this->backup_pos.y;
			}
			
			this->drawed = false;
			putimage_rotatezoom(nullptr,this->image_vector[this->current_image],this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f * PIE,this->scale / 100.00f,1,this->alpha);
		}
		~Button(){ }
};

int current_reg_order = 0;
unsigned long long frame = 0;
queue<Element*>FreeList;

class cmp {
	public:
		bool operator()(Element* _A,Element* _B) {
			if(_A->get_order() == _B->get_order()) return _A->get_reg_order() < _B->get_reg_order();
			return _A->get_order() < _B->get_order();
		}
};

set<Element*,cmp>Element_queue;

void Element::increase_order(int count) {
	Element_queue.erase(this);
	this->order += count;
	Element_queue.insert(this);
}

void Element::decrease_order(int count) {
	Element_queue.erase(this);
	this->order -= count;
	Element_queue.insert(this);
}

void Element::set_order(int count) {
	Element_queue.erase(this);
	this->order = count;
	Element_queue.insert(this);
}

namespace pen {
	int order = 0;
	int fontscale = 1;
	short penalpha = 255;
	int penType = FeEGE::PenType.left;
	int charwidth,charheight;
	inline void print(int x,int y,string str) {
		if(pen_image == nullptr) return;
		if(penType == FeEGE::PenType.middle) {
			x -= charwidth * str.length() >> 1;
			y -= charheight >> 1;
		}
		outtextxy(x,y,str.c_str(),pen_image);
	}
	inline void font(int scale,string fontname = "幼圆") {
		if(pen_image == nullptr) return;
		fontscale = scale;
		setfont(scale,0,fontname.c_str(),pen_image);
		charwidth = textwidth('t',pen_image);
		charheight = textheight('t',pen_image);
	}
	inline void color(color_t color) {
		if(pen_image == nullptr) return;
		setcolor(color,pen_image);
	}
	inline void type(int Type) {
		penType = Type;
	}
	inline void clear(int x,int y,int ex,int ey) {
		if(pen_image == nullptr) return;
		bar(x,y,ex,ey,pen_image);
	}
	inline void clear_char(int x,int y) {
		if(pen_image == nullptr) return;
		bar(x,y,x + charwidth,y + charheight,pen_image);
	}
	inline void clear_chars(int x,int y,int charcount) {
		if(pen_image == nullptr) return;
		bar(x,y,x + charwidth * charcount,y + charwidth,pen_image);
	}
	inline void clear_all() {
		if(pen_image == nullptr) return;
		bar(0,0,getwidth(pen_image),getheight(pen_image),pen_image);
	}
	inline void setorder(int value) {
		order = value;
	}
	inline void set_alpha(short alpha) {
		penalpha = alpha;
		penalpha %= 256;
		if(penalpha < 0) penalpha += 256;
	}
	inline short get_alpha() {
		return penalpha;
	}
	inline void increase_alpha(short alpha){
		penalpha += alpha;
		penalpha %= 256;
		if(penalpha < 0) penalpha += 256;
	}
	inline void decrease_alpha(short alpha){
		penalpha -= alpha;
		penalpha %= 256;
		if(penalpha < 0) penalpha += 256;
	}
	inline void print_line(int x1,int y1,int x2,int y2){
		line(x1,y1,x2,y2,pen_image);
	}
	inline void print_bar(int left,int top,int right,int bottom){
		bar(left,top,right,bottom,pen_image);
	}
}

//Functions

void reg_Element(Element* element) {
	element->set_reg_order(current_reg_order ++);
	Element_queue.insert(element);
	__SIZE__ ++;
	ElementIsIn[element] = true;
	IdToElement[element->getId()] = element;
}



Element* FeEGE::getElementById(string ElementId) {
	return IdToElement[ElementId];
}

Element* FeEGE::getElementByPtr(Element* ElementPtr) {
	return (ElementIsIn.find(ElementPtr) != ElementIsIn.end()) ? ElementPtr : nullptr;
}

Element ElementPool[MAXELEMENTCOUNT];
bool ElementPoolUsed[MAXELEMENTCOUNT] = {};

Element* Element::deletethis() {
	FreeList.push(this);
	return this;
}

Element* Element::deleteElement() {
	if(((Element*)this->get_variable(1)) != nullptr) ((Element*)this->get_variable(1))->removeList.push_back(this);
	Element_queue.erase(this);
	this->deleted = true;
	ElementPoolUsed[this->PoolIndex] = false;
	this->frame_function_set.clear();
	if((Element*)this->private_variables[1] == nullptr) for(auto i : this->image_vector) delimage(i);
	this->image_vector.clear();
	this->clones.clear();
	this->removeList.clear();
	this->on_mouse_put_on_function_set.clear();
	this->on_mouse_hitting_function_set.clear();
	this->on_mouse_move_away_function_set.clear();
	this->on_click_function_set.clear();
	this->on_clone_function_set.clear();
	this->on_clone_clones_function_set.clear();
	this->deleted = true;
	ElementIsIn[this] = false;
	IdToElement[this->id] = nullptr;
	return this;
}

Element* newElement(string id,string ImagePath,double x = 0,double y = 0) {
	PIMAGE image = newimage();
	if(ImagePath != "") getimage(image,TEXT(ImagePath.c_str()));
	for(int i = 0; i < MAXELEMENTCOUNT; ++ i) {
		if(!ElementPoolUsed[i]) {
			ElementPoolUsed[i] = true;
			Element* e = ElementPool[i].copy(id,image,i,x,y);
			reg_Element(e);
			e->set_type("Normal@native");
			return e;
		}
	}
	MessageBox(getHWnd(),"分配Element失败(达到最大容量)","提示",MB_OK);
	return nullptr;
}

Button* newButton(string id,string ImagePath,double x,double y,function<void(Element*)> put_on = nullptr,function<void(Element*)> move_away = nullptr,function<void(Element*)> on_click = nullptr) {
	Button* button = (Button*)newElement(id,ImagePath,x,y);
	if(button == nullptr) return nullptr;
	button->set_type("Button@native");
	button->listen(FeEGE::EventType.on_click,"click@native",on_click);
	button->listen(FeEGE::EventType.on_mouse_put_on,"hit@native",put_on);
	button->listen(FeEGE::EventType.on_mouse_move_away,"hit@native",move_away);
	return button;
}

Element* newInputBox(string id,string ImagePath,double x,double y,Element* submit_button,function<void(Element*)> on_submit = nullptr,function<void(Element*)> on_selected = nullptr){
	Element* inputBox = newElement(id,ImagePath,x,y);
	if(inputBox == nullptr) return nullptr;
	inputBox->set_type("InputBox@native");
	return inputBox;
}

#if true
Element* newElement(string id,PIMAGE image,double x = 0,double y = 0) {
	for(int i = 0; i < MAXELEMENTCOUNT; ++ i) {
		if(!ElementPoolUsed[i]) {
			ElementPoolUsed[i] = true;
			Element* e = ElementPool[i].copy(id,image,i,x,y);
			reg_Element(e);
			return e;
		}
	}
	MessageBox(getHWnd(),"分配Element失败(达到最大容量)","提示",MB_OK);
	return nullptr;
}
#endif

void globalListen(int listen_mode,string identifier,auto function){
	if(listen_mode == FeEGE::EventType.frame) globalListen_frame_function_set[identifier] = function ;
	if(listen_mode == FeEGE::EventType.on_click) globalListen_on_click_function_set[identifier] = function ;
}

void stopGlobalListen(int listen_mode,string identifier,auto function){
	if(listen_mode == FeEGE::EventType.frame) globalListen_frame_function_set.erase(identifier) ;
	if(listen_mode == FeEGE::EventType.on_click) globalListen_on_click_function_set.erase(identifier) ;
}

Position get_mouse_position(){
	int x,y;
	mousepos(&x,&y);
	return Position{(double)x,(double)y};
}

void reflush() {
	++ frame;

	while(!FreeList.empty()) {
		FreeList.front()->deleteElement();
		FreeList.pop();
	}
	
	// run schedule
	vector<function<void(void)>> schedule_backup;
	schedule_backup.swap(schedule);
	for(auto func : schedule_backup) func();
	
	// check timeOut
	double currentTime = getMs();
	while(!schedule_timeOut.empty() && (*schedule_timeOut.begin()).first < currentTime){
		auto vec = (*schedule_timeOut.begin()).second;
		schedule_timeOut.erase(schedule_timeOut.begin());
		for(const auto& func : vec) func();
	}
	
	bool pen_nprinted = true;
	cleardevice();

	for(Element* it : Element_queue) {
		if(it == nullptr || it == *Element_queue.end()) continue;
		if(pen_nprinted && it->get_order() >= pen::order) {
			pen_nprinted = false;
			putimage_alphatransparent(nullptr,pen_image,0,0,EGERGBA(1,1,4,0),pen::penalpha);
		}
		it->call();
	}
	if(pen_nprinted) putimage_alphatransparent(nullptr,pen_image,0,0,EGERGBA(1,1,4,0),pen::penalpha);
	flushmouse();
#ifdef FPS
	static char fps[32];
	memset(fps,0,sizeof(fps));
	sprintf(fps,"FPS : %0.2f",getfps());
	setcaption(fps);
#endif
	delay_ms(1);
}

void init(int x,int y){
	setinitmode(INIT_RENDERMANUAL);
	initgraph(x,y);
	initXY();
	FeEGE::initpen();
}

void start() {
	closeGraph = false;
	initXY();
	randomize();
	while(!closeGraph && is_run()) {
		reflush();
		for(auto it : globalListen_frame_function_set) it.second();
		if(!reg_pause) continue;
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
