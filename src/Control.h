/**
 * @file Control.h
 * @brief ��������UI�ؼ�����������ť������򡢻��������ı��ȣ�
 * @author FeJS8888
 */

#pragma once

#include <graphics.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <iostream>
#include <functional>
#include <set>
#include "sys_edit.h"  // mouse_msg �Ƚṹ��
#include "Element.h"
#include "Base.h"

using namespace FeEGE;

/**
 * @brief ���пɻ��ƿؼ��Ļ���
 */
class Widget {
public:
    /**
     * @brief ����������֤����������������
     */
    virtual ~Widget() {}
    
    virtual void handleEvent(const mouse_msg& msg);
    bool is_global = true;

    /**
     * @brief ���ƿؼ���ָ��ͼ��
     * @param dst ����Ŀ��ͼ��
     * @param offsetX �ؼ����� x ���꣨����� dst��
     * @param offsetY �ؼ����� y ���꣨����� dst��
     */
    virtual void draw(PIMAGE dst, int offsetX, int offsetY) = 0;
    virtual void draw() = 0;
    
    virtual void setPosition(int x,int y) = 0;
    
    virtual void setScale(double s) = 0;
};

/**
 * @brief Բ�����ؼ��֧࣬��Ƕ���ӿؼ�
 */
class Panel : public Widget {
public:
    /**
     * @brief ���캯��
     * @param centerX ������� x ����
     * @param centerY ������� y ����
     * @param w ���
     * @param h �߶�
     * @param r Բ�ǰ뾶
     * @param bg ������ɫ
     */
    Panel(int centerX, int centerY, double w, double h, double r, color_t bg);
    
    ~Panel();

    /**
     * @brief ����ӿؼ�
     * @param child �ؼ�ָ��
     * @param offsetX �����������ĵ� x ƫ��
     * @param offsetY �����������ĵ� y ƫ��
     */
    void addChild(Widget* child, double offsetX, double offsetY);

    /**
     * @brief ���� Panel ��ָ��ͼ��
     * @param dst Ŀ��ͼ��
     * @param x ճ���� dst �е����� x ����
     * @param y ճ���� dst �е����� y ����
     */
    void draw(PIMAGE dst, int x, int y) override;
    void draw() override;
    
    void setPosition(int x,int y) override;
    
    Position getPosition();
    
    void setScale(double s) override;
    
    double getScale();
    
    void handleEvent(const mouse_msg& msg) override;

private:
    int cx, cy;  ///< ��������
    double width, height;
    double radius;
    double origin_width, origin_height;
    double origin_radius;
    color_t bgColor;
    double scale = 1;
    PIMAGE layer = newimage(width, height);
    PIMAGE maskLayer = newimage(width,height);

    std::vector<Widget*> children;
    std::vector<Position> childOffsets;  ///< ÿ���ӿؼ������ƫ�ƣ����������Ϊ�ο���
};

/**
 * @brief ���ƶ���Ч���ṹ��
 */
struct Ripple{
    int x, y;           ///< ������������
    int maxRadius;      ///< ���뾶
    int life;           ///< ��������
    int age = 0;        ///< ��ǰ����

    /**
     * @brief ���캯��
     * @param _x ����x����
     * @param _y ����y����
     * @param _r ���뾶
     * @param _life ��������
     */
    Ripple(int _x, int _y, int _r, int _life);

    /**
     * @brief ��鲨���Ƿ���
     * @return �Ƿ���
     */
    bool alive() const;

    /**
     * @brief ���²���״̬
     */
    void update();

    /**
     * @brief ���Ʋ���
     * @param dst Ŀ��ͼ��
     */
    void draw(PIMAGE dst,double scale) const;
};

/**
 * @brief ��ť�ؼ���
 */
class Button : public Widget {
private:
    int centerX, centerY;   ///< ��������
    double width, height;      ///< ���
    double radius;             ///< Բ�ǰ뾶
    double origin_width, origin_height;
    double origin_radius;
    double scale = 1;
    double left, top;          ///< ���Ͻ�����
    std::vector<Ripple> ripples; ///< ����Ч������
    PIMAGE btnLayer = nullptr;  ///< ��ťͼ��
    PIMAGE maskLayer = nullptr; ///< ����ͼ��
    std::string content;
    std::function<void(void)> on_click_event = nullptr;
    color_t color;
    bool needRedraw = true;

public:
    /**
     * @brief ���캯��
     * @param cx ����x����
     * @param cy ����y����
     * @param w ���
     * @param h �߶�
     * @param r Բ�ǰ뾶
     */
    Button(int cx, int cy, double w, double h, double r);

    /**
     * @brief ��������
     */
    virtual ~Button();

    /**
     * @brief ���ư�ť
     * @param content ��ť�ı�
     */
    virtual void draw(PIMAGE dst,int x,int y) override;
    virtual void draw() override;

    /**
     * @brief ��������¼�
     * @param msg �����Ϣ
     */
    virtual void handleEvent(const mouse_msg& msg);
    
    void setContent(const std::string& str);
    std::string getContent();

    /**
     * @brief �����Ƿ��ڰ�ť��
     * @param x x����
     * @param y y����
     * @return �Ƿ��ڰ�ť��
     */
    bool isInside(int x, int y) const;
    
    void setPosition(int x,int y) override;
    
    void setScale(double s) override;
    
    void setOnClickEvent(std::function<void(void)> func);

    void setColor(color_t col);
};

/**
 * @brief �����ؼ���
 */
class InputBox : public Widget {
private:
    int centerX, centerY;   ///< ��������
    double width, height;     ///< ���
    double radius;            ///< Բ�ǰ뾶
    double origin_width, origin_height;
    double origin_radius;
    double scale = 1;
    double left, top;         ///< ���Ͻ�����
    std::vector<Ripple> ripples; ///< ����Ч������
    PIMAGE btnLayer = nullptr;  ///< ��ťͼ��
    PIMAGE maskLayer = nullptr; ///< ����ͼ��
    std::string content;   ///< ��������
    FeEGE::sys_edit inv;   ///< ����ؼ�
    bool on_focus;         ///< �Ƿ��ý���
    color_t color = EGERGB(245, 245, 235);

public:
    /**
     * @brief ���캯��
     * @param cx ����x����
     * @param cy ����y����
     * @param w ���
     * @param h �߶�
     * @param r Բ�ǰ뾶
     */
    InputBox(int cx, int cy, double w, double h, double r);

    /**
     * @brief ��������
     */
    virtual ~InputBox();

    /**
     * @brief ���������
     */
    virtual void draw(PIMAGE dst,int x,int y) override;
    virtual void draw() override;

    /**
     * @brief ��������¼�
     * @param msg �����Ϣ
     */
    virtual void handleEvent(const mouse_msg& msg);

    /**
     * @brief �����Ƿ����������
     * @param x x����
     * @param y y����
     * @return �Ƿ����������
     */
    bool isInside(int x, int y) const;

    /**
     * @brief ������������
     * @param s �����ַ���
     */
    void setContent(const std::string& s);

    /**
     * @brief ����������볤��
     * @param maxlen ��󳤶�
     */
    void setMaxlen(int maxlen);
    
    void setPosition(int x,int y) override;
    
    void setScale(double s) override;
};

/**
 * @brief �������ؼ���
 */
class Slider : public Widget {
private:
    double left, top;
	double width, height;    ///< λ�úͳߴ�
    double radius;              ///< ����뾶
    double origin_width, origin_height;
    double origin_radius;
    int m_dragOffset;          ///< �϶�ƫ����
    double m_value;            ///< ��ǰֵ(0.0-1.0)
    bool m_dragging;           ///< �Ƿ������϶�
    color_t m_bgColor, m_fgColor; ///< ������ǰ��ɫ
    std::function<void(double)> m_onChange; ///< ֵ�ı�ص�
	bool m_hover = false;            ///< ����Ƿ���ͣ
	bool m_pressed = false;          ///< �Ƿ����ڰ�ס
	float m_scale = 1.0f;            ///< ��ǰ���ű���
	double thickness = 4;
	double origin_thickness = 4;
	double scale = 1;


public:
    /**
     * @brief Ĭ�Ϲ��캯��
     */
    Slider();

    /**
     * @brief ����������
     * @param x x����
     * @param y y����
     * @param w ���
     * @param h �߶�
     */
    void create(int x, int y, double w, double h);

    /**
     * @brief ���ƻ�����
     */
    virtual void draw(PIMAGE dst,int x,int y) override ;
    virtual void draw();
    
    /**
     * @brief �����Ƿ��ڻ�����
     * @param x x����
     * @param y y����
     * @return �Ƿ����������
     */
    bool isInside(int x, int y) const;

    /**
     * @brief ��������¼�
     * @param msg �����Ϣ
     */
    virtual void handleEvent(const mouse_msg& msg);

    /**
     * @brief ���ý���ֵ
     * @param v ����ֵ(0.0-1.0)
     */
    void setProgress(double v);

    /**
     * @brief ��ȡ��ǰ����ֵ
     * @return ��ǰ����ֵ
     */
    double getProgress() const;

    /**
     * @brief ������ɫ
     * @param bg ����ɫ
     * @param fg ǰ��ɫ
     */
    void setColor(color_t bg, color_t fg);
    
    /**
     * @brief ���ô�ϸ
     * @param t ��ϸ
     */
    void setThickness(double t);

    /**
     * @brief ����ֵ�ı�ص�
     * @param callback �ص�����
     */
    void setOnChange(std::function<void(double)> callback);
    
    void setPosition(int x,int y) override;
    
    void setScale(double s) override;
};

 /**
  * @brief ���ڹ��� Panel ����Ĺ�������
  */
class PanelBuilder {
public:
    PanelBuilder& setCenter(int x, int y);
    PanelBuilder& setSize(double w, double h);
    PanelBuilder& setRadius(double r);
    PanelBuilder& setBackground(color_t color);
    PanelBuilder& setScale(double s);
    PanelBuilder& addChild(Widget* child, double offsetX, double offsetY);
    Panel* build();

private:
    int cx = 0, cy = 0;
    double width = 100, height = 50;
    double radius = 5;
    color_t bg = EGERGB(240, 240, 240);
    double scale = 1.0;
    std::vector<Widget*> children;
    std::vector<Position> childOffsets;  ///< ÿ���ӿؼ������ƫ�ƣ����������Ϊ�ο���
};

/**
 * @brief ���ڹ��� Button ����Ĺ�������
 */
class ButtonBuilder {
public:
    ButtonBuilder& setCenter(int x, int y);
    ButtonBuilder& setSize(double w, double h);
    ButtonBuilder& setRadius(double r);
    ButtonBuilder& setContent(const std::string& text);
    ButtonBuilder& setScale(double s);
    ButtonBuilder& setOnClick(std::function<void()> func);
    ButtonBuilder& setColor(color_t col);
    Button* build();

private:
    int cx = 0, cy = 0;
    double width = 100, height = 50;
    double radius = 8;
    std::string content = "Button";
    double scale = 1.0;
    color_t color = EGERGB(245, 245, 235);
    std::function<void()> onClick = nullptr;
};

/**
 * @brief ���ڹ��� InputBox ����Ĺ�������
 */
class InputBoxBuilder {
public:
    InputBoxBuilder& setCenter(int x, int y);
    InputBoxBuilder& setSize(double w, double h);
    InputBoxBuilder& setRadius(double r);
    InputBoxBuilder& setContent(const std::string& text);
    InputBoxBuilder& setMaxLength(int maxLen);
    InputBoxBuilder& setScale(double s);
    InputBox* build();

private:
    int cx = 0, cy = 0;
    double width = 160, height = 40;
    double radius = 6;
    std::string content = "";
    int maxLength = 100;
    double scale = 1.0;
};

/**
 * @brief ���ڹ��� Slider ����Ĺ�������
 */
class SliderBuilder {
public:
    /**
     * @brief ����λ��
     * @param cx ���� x ����
     * @param cy ���� y ����
     */
    SliderBuilder& setCenter(int cx, int cy);

    /**
     * @brief ���óߴ�
     * @param w ���
     * @param h �߶�
     */
    SliderBuilder& setSize(double w, double h);

    /**
     * @brief ������ɫ
     * @param bg ����ɫ
     * @param fg ǰ��ɫ
     */
    SliderBuilder& setColor(color_t bg, color_t fg);

    /**
     * @brief ���ô�ϸ
     * @param t ��ϸֵ
     */
    SliderBuilder& setThickness(double t);

    /**
     * @brief ���ó�ʼ����
     * @param v ��ʼֵ (0.0~1.0)
     */
    SliderBuilder& setProgress(double v);

    /**
     * @brief �������ű���
     * @param s ���ű���
     */
    SliderBuilder& setScale(double s);

    /**
     * @brief ����ֵ�ı�ʱ�Ļص�����
     * @param callback �ص�����
     */
    SliderBuilder& setOnChange(std::function<void(double)> callback);

    /**
     * @brief ���������� Slider ����
     */
    Slider* build();

private:
    int x = 0, y = 0;
    double width = 200, height = 20;
    color_t bgColor = EGERGB(200, 200, 200);
    color_t fgColor = EGERGB(100, 100, 255);
    double thickness = 4;
    double progress = 0.0;
    double scale = 1.0;
    std::function<void(double)> onChange = nullptr;
};

extern std::set<Widget*> widgets;
