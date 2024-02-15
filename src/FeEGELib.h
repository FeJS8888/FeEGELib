#define version V1.2--upd2024-2-16 
#ifndef _FEEGELIB_
#define _FEEGELIB_

#define OEMRESOURCE
#include<graphics.h>
#include<vector>
#include<thread>
#include<set>
#include<map>
#include<string>
#include<windows.h>
#include<algorithm>
#include<iostream>
#include<fstream>
#include<queue>
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

using namespace std;

double GlobalRating = 1.00f;

class Position;
class Element;
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

int WIDTH;
int HEIGHT;

vector<void(*)(void)> schedule;
int __SIZE__ = 0;
int removesize = 0;
bool closeGraph = false;

PIMAGE pen_image;

map<string,Element*>IdToElement;
map<Element*,bool>ElementIsIn;


//EventType
//Classes

//union TYPE_UNION{
//	int type_int;
//	bool type_bool;
//	string type_string;
//	void* type_pointer;
//	unsigned int type_uint;
//};

//template<typename T>
//Any MakeAny(T Value) {
//	Any* data = new Any;
//	const type_info& type = typeid(T);
//	if(
//	    type == typeid(int) ||
//	    type == typeid(long long) ||
//	    type == typeid(short) ||
//	    type == typeid(double) ||
//	    type == typeid(long double) ||
//	    type == typeid(float)) data->Number = Value;
//	if(type == typeid(string)) data->String = Value;
//	if(type.__is_pointer_p()) data->Pointer = Value;
//	if(type == typeid(bool)) data->Boolean = Value;
//	return *data;
//}

using namespace std;

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
}lastpixel;

namespace FeEGE {
	short getkey(int VB) {
		return GetAsyncKeyState(VB);
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

		//鍒濆鍖栫敾绗?
		WIDTH = getwidth();
		HEIGHT = getheight();
		if(pen_image != nullptr) delimage(pen_image);
		pen_image = newimage(WIDTH,HEIGHT);
		setbkcolor(EGERGBA(1,1,4,0),pen_image);
		setcolor(EGERGB(255,0,0),pen_image);
		setfillcolor(EGERGBA(1,1,4,0),pen_image);
	}
	void push_schedule(auto function) {
		schedule.push_back(function);
	}

	class ClonesEvent {
		public:
			int on_clone = 0x06;
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
	};

	Events EventType;
	
	class PenTypes {
		public:
			int left = 0x01;
			int middle = 0x02;
	};

	PenTypes PenType;
	
	Position& getLastPixel(){
		return lastpixel;
	} 
}

class Element {
	private:
		//Variables
		string id;
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
		map<string,void(*)(Element*)> frame_function_set;
		map<string,void(*)(Element*)> on_mouse_put_on_function_set;
		map<string,void(*)(Element*)> on_mouse_hitting_function_set;
		map<string,void(*)(Element*)> on_mouse_move_away_function_set;
		map<string,void(*)(Element*)> on_click_function_set;
		map<string,void(*)(Element*)> on_clone_function_set;
		map<string,void(*)(Element*)> on_clone_clones_function_set;
		unsigned int current_image = 0;
		long long private_variables[10];
		bool deletedList[MAXCLONESCOUNT] = {};
		int clonecount = 0;
		int nextclonecount = 0;
		bool deleted = false;
		bool image_lock = false;
		bool disabled_draw_to_private_image = false;
		Element** clonequeue = nullptr;
		bool drawed = false;
		int PoolIndex;

		bool PhysicEngineStatu;
		double ForceX;
		double ForceY;
		double SpeedX;
		double SpeedY;
		double ResistanceX;
		double ResistanceY;

		vector<color_t> remove_colors;

		inline void reflush_mouse_statu() {
			/*
				Test click
			*/
			if(this->ismousein()) {
				int statu = this->get_variable(0);
				if(this->ishit()) {
					if(statu == 1) {
						this->set_variable(0,2);
						for(auto it :this->on_mouse_hitting_function_set) it.second(this);
					}
				} else {
					if(statu == 0) {
						this->set_variable(0,1);
						for(auto it : this->on_mouse_put_on_function_set) it.second(this);
					} else if(statu == 2) {
						this->set_variable(0,0);
						for(auto it : this->on_click_function_set) it.second(this);
					}
				}
			} else {
				if(this->get_variable(0) == 1)	{
					for(auto it : this->on_mouse_move_away_function_set) it.second(this);
					this->set_variable(0,0);
				}
			}
		}
		inline bool draw_to_private_image() {
			if(this->drawed) return false;
			if(this->deleted) return false;
			if(this->disabled_draw_to_private_image) return false;
			if(this->image_vector[this->current_image] == nullptr) return false;
			cleardevice(this->__visible_image);
//			 cout<<this->id<<endl;
			putimage_rotatezoom(this->__visible_image,this->image_vector[this->current_image],this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f * PIE,this->scale / 100.00f,1);
			return this->drawed = true;
		}
	public:
		//Functions
		Element(string id,PIMAGE image,Position pos) {
			this->id = id;
//			log("[Construct]ptr: 0x%x id: %s\n",this,this->id.c_str());
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
//			log("[Construct]ptr: 0x%x id: %s\n",this,this->id.c_str());
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
		Element() {

		}
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
//		Element(Element* that) {
//			this->__visible_image = newimage(getwidth(),getheight());
//			setbkcolor(EGERGBA(1,1,4,0),this->__visible_image);
//			this->image_vector = that->image_vector;
//			this->current_image = that->current_image;
//			this->pos = *(new Position(that->pos.x,that->pos.y));
//			this->is_show = true;
//			this->scale = 100;
//			this->angle = 0;
//			this->order = 0;
//			this->clonecount = 0;
//		}
		inline void call() {
			this->backup_pos = pos;
			this->reflush_mouse_statu();
			for(auto it : this->frame_function_set) it.second(this);
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
				cout<<"ok\n";
			}
			//
			this->drawed = false;
			if(this->PhysicEngineStatu) {
				this->UpdatePhysicSpeed();
				this->UpdatePhysicPosition();
			}
//			PIMAGE alpha_image = newimage();
//			getimage(alpha_image,image,0,0,getwidth(image),getheight(image));

//			for(auto color : this->remove_colors) ege_setalpha(color,alpha_image);
//			ege_setalpha(0,alpha_image);

			putimage_rotatezoom(nullptr,this->image_vector[this->current_image],this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f * PIE,this->scale / 100.00f,1,this->alpha);

//			delimage(this->__visible_image);
//			this->__visible_image = newimage(getwidth(),getheight());
//			setbkcolor(EGERGBA(1,1,4,0),this->__visible_image);
//			cleardevice(this->__visible_image);
//			putimage_rotatezoom(this->__visible_image,alpha_image,this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f * PIE,this->scale / 100.00f,1);
//			delimage(alpha_image);
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
		inline color_t get_pixel(int x,int y){
			return getpixel(x,y,this->__visible_image);
		}
		inline short get_scale() {
			return this->scale;
		}
		inline void set_scale(short scale) {
			this->scale = scale;
			this->scale %= 101;
		}
		inline void increase_scale(short scale) {
			if(this->scale + scale <= 100) this->scale += scale;
			else this->scale = 100;
			this->scale %= 101;
			if(this->scale < 0) this->scale += 101;
		}
		inline void decrease_scale(short scale) {
			if(this->scale > scale) this->scale -= scale;
			else this->scale = 0;
			this->scale %= 101;
			if(this->scale < 0) this->scale += 101;
		}
		inline void hide() {
			this->is_show = false;
		}
		inline void show() {
			this->is_show = true;
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
		inline void move_to(double x,double y) {
			this->pos.x = x;
			this->pos.y = y;
		}
		inline void move_to(Position pos) {
			this->pos = pos;
		}
		inline void remove_color(color_t color) {
			this->remove_colors.push_back(color);
		}
		inline bool ismousein() {
			if(GetForegroundWindow() != getHWnd()) return false;
			if(!this->is_show) return false;
			int x,y;
			mousepos(&x,&y);
			if(x < 0 || y < 0 || x > WIDTH || y > HEIGHT) {
//				// cout<<"BECAUSE OUT"<<endl;
				return false;
			}
//			cout<<"test"<<this;
			this->draw_to_private_image();
//			// cout<<x<<" "<<y<<"<><>"<<EGEGET_A(getpixel(x,y,this->__visible_image))<<"<><>"<<getpixel(x,y,this->__visible_image)<<endl;
			//getch();
//			// cout<<"======DEBUG=========\nPRINT __visible_image\n";
//			putimage(0,0,this->__visible_image);
//putimage_rotatezoom(nullptr,this->__visible_image,this->pos.x,this->pos.y,0.5,0.5,this->angle / 180.00f,this->scale / 100.00f,1);
			//getch();
//			if(EGEGET_A(getpixel(x,y,this->__visible_image)) == 255) // cout<<"!= alpha"<<endl;
//			else //getch();
//			// cout<<"GETA"<<EGEGET_A(getpixel(x,y,this->__visible_image)<<endl;
			color_t pixel = getpixel(x,y,this->__visible_image);
			for(int i = 0; i < remove_colors.size(); ++ i) if(pixel == remove_colors[i]) return false;

//			cout<<"p="<<pixel<<"|";
			return ((EGEGET_A(pixel) != 0) || (pixel != 65796)); //EGERGBA(1,1,4,0) = 65796
//			int d_width = getwidth(this->image_vector[this->current_image]) / 2;
//			int d_height = getheight(this->image_vector[this->current_image]);
//			return (x >= this->pos.x - d_width && x <= this->pos.x + d_width && y >= this->pos.y - d_height && y <= this->pos.y + d_height);
		}
		inline bool ishit() {
			if(!GetAsyncKeyState(LeftButton)) return false;
			return this->ismousein();
		}
		inline void set_variable(unsigned int which,long long value) {
			this->private_variables[which] = value;
//			// cout<<which<<" "<<value<<" "<<this<<" "<<this->id<<endl;
		}
		inline long long get_variable(unsigned int which) {
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
		inline void set_image(int order) {
			this->current_image = order;
		}
		inline int get_image_order() {
			return this->current_image;
		}
		inline PIMAGE get_image() {
			return this->image_vector[this->current_image];
		}
		inline bool is_touched_by(Element* that) {
			if(that == nullptr) {
				LPCSTR text = TEXT(("Element::is_touched_by鏂规硶琚敊璇殑浼犲叆浜唍ullptr鍙傛暟\n杩欏彲鑳芥槸鐢变簬getElementById鏌ヨ浜嗕笉瀛樺湪鐨勫璞n\nElement鍚嶇О : " + this->id).c_str());
				MessageBox(getHWnd(),text,"璀﹀憡",MB_ICONWARNING | MB_OK);
			}
			if(!this->is_show || !that->is_show) return false;
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
//				// cout<<this->removeList.size()<<endl;
				for(int j = 0; j < this->removeList.size(); ++ j) {
					if(this->deletedList[i] && (e[i] == this->removeList[j] || FeEGE::getElementByPtr(e[i]) == nullptr)) {
//						// cout<<i<<" "<<e[i]<<" removed"<<endl;
						this->deletedList[i] = false;
//						if(!e[i]->deleted) e[i]->~Element();
						e[i] = nullptr;
					}
				}
//				// cout<<i<<" "<<e[i]<<" "<<!deletedList[i]<<endl;
			}
			this->removeList.clear();
//			// cout<<"SAFE"<<endl;
			for(int i = 0; i < nextclonecount; ++ i) if(!this->deletedList[i]) {
//					// cout<<"USE OLD"<<i<<endl;
					deletedList[i] = true;
					e[i] = newElement(this->id + "_" + to_string(clonecount ++),this->get_image(),this->pos.x,this->pos.y);
					e[i]->angle = this->angle;
					e[i]->scale = this->scale;
//					// cout<<this<<" "<<(long long)this<<endl;
					e[i]->set_variable(1,(long long)this);
//					e[i]->get_variable(1);
					e[i]->disabled_draw_to_private_image = this->disabled_draw_to_private_image;
					e[i]->is_show = this->is_show;
					reg_Element(e[i]);
					for(auto it : this->on_clone_function_set) it.second(this);
					for(auto it : this->on_clone_clones_function_set) it.second(e[i]);
					return e[i];
				}
//			// cout<<"USE NEW"<<nextclonecount<<endl;
			deletedList[nextclonecount] = true;
			e[nextclonecount] = newElement(this->id + "_" + to_string(clonecount ++),this->get_image(),this->pos.x,this->pos.y);
			e[nextclonecount]->angle = this->angle;
			e[nextclonecount]->scale = this->scale;
//			// cout<<this<<" "<<(long long)this<<endl;
			e[nextclonecount]->set_variable(1,(long long)this);
//			// cout<<e[nextclonecount]->get_variable(1)<<endl;
			e[nextclonecount]->is_show = this->is_show;
			reg_Element(e[nextclonecount]);
			for(auto it : this->on_clone_function_set) it.second(this);
			for(auto it : this->on_clone_clones_function_set) it.second(e[nextclonecount]);
			return e[nextclonecount ++];
		}
		inline string getId() {
			return this->id;
		}
		inline void listen(int listen_mode,string identifier,auto function) {
			if(listen_mode == FeEGE::EventType.frame) this->frame_function_set[identifier] = function ;
			if(listen_mode == FeEGE::EventType.on_mouse_put_on) this->on_mouse_put_on_function_set[identifier] = function ;
			if(listen_mode == FeEGE::EventType.on_mouse_hitting) this->on_mouse_hitting_function_set[identifier] = function ;
			if(listen_mode == FeEGE::EventType.on_mouse_move_away) this->on_mouse_move_away_function_set[identifier] = function ;
			if(listen_mode == FeEGE::EventType.on_click) this->on_click_function_set[identifier] = function ;
			if(listen_mode == FeEGE::EventType.on_clone) this->on_clone_function_set[identifier] = function ;
			if(listen_mode == FeEGE::EventType.clones.on_clone) this->on_clone_clones_function_set[identifier] = function ;
		}
		inline void stop(int listen_mode,string identifier) {
			if(listen_mode == FeEGE::EventType.frame) this->frame_function_set.erase(identifier) ;
			if(listen_mode == FeEGE::EventType.on_mouse_put_on) this->on_mouse_put_on_function_set.erase(identifier) ;
			if(listen_mode == FeEGE::EventType.on_mouse_hitting) this->on_mouse_hitting_function_set.erase(identifier) ;
			if(listen_mode == FeEGE::EventType.on_mouse_move_away) this->on_mouse_move_away_function_set.erase(identifier) ;
			if(listen_mode == FeEGE::EventType.on_click) this->on_click_function_set.erase(identifier) ;
			if(listen_mode == FeEGE::EventType.on_clone) this->on_clone_function_set.erase(identifier) ;
			if(listen_mode == FeEGE::EventType.clones.on_clone) this->on_clone_clones_function_set.erase(identifier) ;
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
//			// cout<<this->current_image<<endl;
			do {
				this->current_image ++;
				this->current_image %= 100;
			} while(this->image_vector[current_image] == nullptr);
		}
		inline void loadImage(string ImagePath,int id) {
			thread([this,ImagePath,id]() {
				PIMAGE image = newimage();
				int statu = 0;
				do {
					// cout<<this->id<<" 's "<<id<<" is trying\n";
					statu = getimage(image,TEXT(ImagePath.c_str()));
					this_thread::sleep_for(chrono::milliseconds(10));
				} while(statu != 0);
				while(this->image_lock) this_thread::sleep_for(chrono::milliseconds(1));
				this->image_lock = true;
				this->image_vector[id] = image;
				this->image_lock = false;
				// cout<<this->id<<" 's "<<id<<" ok\n";
			}).detach();
		}
		inline void deleteImage(int id) {
			thread([this,id]() {
				delimage(this->image_vector[id]);
			}).detach();
		}
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
					if(getpixel(i,j,this->image_vector[this->current_image]) == BLACK) putpixel_f(i,j,EGERGBA(0,0,0,0),this->image_vector[this->current_image]);
				}
			}
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
		inline void PhysicRemoveSpeedX(){
			this->SpeedX = 0;
		}
		inline void PhysicRemoveSpeedY(){
			this->SpeedY = 0;
		}

		inline Element* deleteElement();
		~Element() {
		};
};
int current_reg_order = 0;
unsigned long long frame = 0;
queue<Element*>FreeList;

class cmp {
	public:
		bool operator()(Element* _A,Element* _B) {
			if(_A->get_order() == _B->get_order()) return _A->get_reg_order() < _B->get_reg_order();
			return _A->get_order() < _B->get_order();
//			cout<<_A->getId()<<"="<<_A->get_order()<<"&"<<_B->getId()<<"="<<_B->get_order()<<endl;
//			if(_A == nullptr) return false;
//			if(_B == nullptr) return true;
//			if(_A->get_order() < _B->get_order()) return false;
//			else if (_A->get_order() > _B->get_order()) return true;
//			cout<<"Failed\n";
//			return _A->get_reg_order() < _B->get_reg_order();
//			return *_A < *_B;
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
	int penType = FeEGE::PenType.left;
	void print(int x,int y,string str) {
		if(pen_image == nullptr) return;
		if(penType == FeEGE::PenType.middle){
			x -= (str.length() * (fontscale / 2.0));
			y -= (str.length() * (fontscale / 4.0));
		}
		outtextxy(x,y,str.c_str(),pen_image);
	}
	void font(int scale,string fontname) {
		if(pen_image == nullptr) return;
		fontscale = scale;
		setfont(scale,0,fontname.c_str(),pen_image);
	}
	void color(color_t color) {
		if(pen_image == nullptr) return;
		setcolor(color,pen_image);
	}
	void type(int Type){
		penType = Type;
	}
	void clear(int x,int y,int ex,int ey) {
		if(pen_image == nullptr) return;
		bar(x,y,ex,ey,pen_image);
	}
	void clear_char(int x,int y) {
		if(pen_image == nullptr) return;
		bar(x,y,x + fontscale,y + fontscale,pen_image);
	}
	void clear_chars(int x,int y,int charcount) {
		if(pen_image == nullptr) return;
		bar(x,y,x + fontscale * charcount,y + fontscale,pen_image);
	}
	void clear_all() {
		if(pen_image == nullptr) return;
		bar(0,0,getwidth(pen_image),getheight(pen_image),pen_image);
	}
	void setorder(int value) {
		order = value;
	}
}

//Functions

void reg_Element(Element* element) {
	element->set_reg_order(current_reg_order ++);
	Element_queue.insert(element);
//	cout<<"\nfind"<<element<<"="<<(Element_queue.find(element) == Element_queue.end())<<endl;
	__SIZE__ ++;
	ElementIsIn[element] = true;
	IdToElement[element->getId()] = element;
}



Element* FeEGE::getElementById(string ElementId) {
//	for(int i = 0; i < Element_queue.size(); ++ i) {
//		if(Element_queue[i] == nullptr) continue;
//		if(Element_queue[i]->getId().length() != ElementId.length()) continue;
//		if(Element_queue[i]->getId() == ElementId) return Element_queue[i];
//	}
	return IdToElement[ElementId];
}



Element* FeEGE::getElementByPtr(Element* ElementPtr) {
//	for(int i = 0; i < Element_queue.size(); ++ i) {
//		if(Element_queue[i] == nullptr) continue;
//		if(Element_queue[i] == ElementPtr) return Element_queue[i];
//	}
	return ElementIsIn[ElementPtr] ? ElementPtr : nullptr;
}

Element ElementPool[MAXELEMENTCOUNT];
bool ElementPoolUsed[MAXELEMENTCOUNT] = {};

Element* Element::deletethis() {
	FreeList.push(this);
	return this;
}

Element* Element::deleteElement() {
	//	// cout<<"鍒犻櫎"<<endl;
//	// cout<<"p = "<<((Element*)this->get_variable(1))<<endl;
	if(((Element*)this->get_variable(1)) != nullptr) ((Element*)this->get_variable(1))->removeList.push_back(this);
//			log("EMM");
	Element_queue.erase(this);
	this->deleted = true;
	ElementPoolUsed[this->PoolIndex] = false;
//			// cout<<this->PoolIndex<<" : 1->0\n";
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
//	for(int i = 0; i < Element_queue.size(); ++ i) {
//		if(Element_queue[i] == this) {
//			Element_queue[i] = nullptr;
//			removesize ++;
//			needsort = true;
//
//		}
//	}
//			// cout<<"Can not find"<<endl;
}

Element* newElement(string id,string ImagePath,double x = 0,double y = 0) {
	PIMAGE image = newimage();
	getimage(image,TEXT(ImagePath.c_str()));
	for(int i = 0; i < MAXELEMENTCOUNT; ++ i) {
		if(!ElementPoolUsed[i]) {
			// cout<<"鍒嗛厤"<<i<<endl;
			ElementPoolUsed[i] = true;
			Element* e = ElementPool[i].copy(id,image,i,x,y);
			reg_Element(e);
			// cout<<"鍒嗛厤缁撴潫"<<endl;
			return e;
		}
	}
	MessageBox(getHWnd(),"鍒嗛厤Element澶辫触(杈惧埌鏈€澶у閲?","鎻愮ず",MB_OK);
	return nullptr;
}

Element* newElement(string id,PIMAGE image,double x = 0,double y = 0) {
	for(int i = 0; i < MAXELEMENTCOUNT; ++ i) {
		if(!ElementPoolUsed[i]) {
			// cout<<"鍒嗛厤"<<i<<endl;
			ElementPoolUsed[i] = true;
			Element* e = ElementPool[i].copy(id,image,i,x,y);
			reg_Element(e);
			return e;
		}
	}
	MessageBox(getHWnd(),"鍒嗛厤Element澶辫触(杈惧埌鏈€澶у閲?","鎻愮ず",MB_OK);
	return nullptr;
}


void reflush() {
	++ frame;

	while(!FreeList.empty()) {
		FreeList.front()->deleteElement();
		FreeList.pop();
	}

	vector<void(*)(void)>schedule_backup;
	for(int i = 0; i < schedule.size(); ++ i) schedule_backup.push_back(schedule[i]);
	schedule.clear();
	for(int i = 0; i < schedule_backup.size(); ++ i) schedule_backup[i]();
	schedule_backup.clear();
//	if(needsort) {
//		sort(Element_queue.begin(),Element_queue.begin() + __SIZE__,cmp);
//		needsort = false;
//	}
//	__SIZE__ -= removesize;
//	removesize = 0;
	bool pen_nprinted = true;
	cleardevice();

	for(Element* it : Element_queue) {
//		cout<<it->getId()<<" ";
		if(it == nullptr || it == *Element_queue.end()) continue;
		if(pen_nprinted && it->get_order() >= pen::order) {
			pen_nprinted = false;
			putimage_transparent(nullptr,pen_image,0,0,EGERGBA(1,1,4,0));
		}
		it->call();
//		cout<<it<<"="<<it->getId()<<"|";
	}
//	cout<<"\n";
	if(pen_nprinted) putimage_transparent(nullptr,pen_image,0,0,EGERGBA(1,1,4,0));
	flushmouse();
	#ifdef FPS
		char fps[100];
		sprintf(fps,"FPS : %0.2f",getfps());
		setcaption(fps);
	#endif
	delay_ms(1);
}

#define initXY() {WIDTH = getwidth();HEIGHT = getheight();}

void start(int fps) {
	initXY();
	randomize();
	while(!closeGraph && is_run()) {
		reflush();
		if(!reg_pause) continue;
		if(FeEGE::getkey('p') || FeEGE::getkey('P')) {
			// cout<<"pause"<<endl;
			this_thread::sleep_for(chrono::milliseconds(200));
			while(!FeEGE::getkey('p') && !FeEGE::getkey('P')) this_thread::sleep_for(chrono::milliseconds(1));
			this_thread::sleep_for(chrono::milliseconds(200));
		}
//		delay_ms(1);
	}

}


#endif
