/**
 * @file Widget.h
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
#include "sys_edit.h"
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
    virtual ~Widget();
    
    virtual void handleEvent(const mouse_msg& msg) = 0;
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

protected:
    int cx, cy;  ///< ��������
    double width, height;
    double scale = 1;
};

/**
 * @brief Բ�����ؼ��֧࣬��Ƕ���ӿؼ�
 */
class Panel : public Widget {
public:
    /**
     * @brief ���캯��
     * @param cx ������� x ����
     * @param cy ������� y ����
     * @param w ���
     * @param h �߶�
     * @param r Բ�ǰ뾶
     * @param bg ������ɫ
     */
    Panel(int cx, int cy, double w, double h, double r, color_t bg);
    
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

    void setSize(double w,double h);

    void clearChildren();
    
    void handleEvent(const mouse_msg& msg) override;

    void setAlpha(double a);

private:
    double radius;
    double origin_width, origin_height;
    double origin_radius;
    color_t bgColor;
    double alpha = 255;
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
    double radius;             ///< Բ�ǰ뾶
    double origin_width, origin_height;
    double origin_radius;
    double left, top;          ///< ���Ͻ�����
    std::vector<Ripple> ripples; ///< ����Ч������
    PIMAGE btnLayer = nullptr;  ///< ��ťͼ��
    PIMAGE maskLayer = nullptr; ///< ����ͼ��
    PIMAGE bgLayer = nullptr; ///< ����ͼ��
    std::string content;
    std::function<void(void)> on_click_event = nullptr;
    color_t color;
    bool needRedraw = true;
    PIMAGE icon = nullptr;
    int iconSize = 100;

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
    
    void setIcon(PIMAGE img);

    void setIconSize(int is);
};

/**
 * @brief �����ؼ���
 */
class InputBox : public Widget {
private:
    double radius;            ///< Բ�ǰ뾶
    double origin_width, origin_height;
    double origin_radius;
    double left, top;         ///< ���Ͻ�����
    std::vector<Ripple> ripples; ///< ����Ч������
    PIMAGE btnLayer = nullptr;  ///< ��ťͼ��
    PIMAGE maskLayer = nullptr; ///< ����ͼ��
    std::string content;   ///< ��������
    FeEGE::sys_edit inv;   ///< ����ؼ�
    bool on_focus;         ///< �Ƿ��ý���
    color_t color = EGERGB(245, 245, 235);
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

enum class Orientation {
    Row,    // �з���ˮƽ��
    Column  // �з��򣨴�ֱ��
};

/**
 * @brief �������ؼ���
 */
class Slider : public Widget {
private:
    double left, top;
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
    bool needRedraw = true;
    Orientation m_orientation = Orientation::Row; // ����

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
    bool isInside(int x, int y);

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

    void setOrientation(Orientation ori);
};

class ProgressBar : public Widget {
private:
    double origin_width, origin_height;
    double radius = 6;
    double left, top;

    color_t bgColor = EGERGB(230, 230, 230);
    color_t fgColor = EGERGB(100, 180, 255);

    double currentProgress = 0.0;  // ��ǰ���ƽ���
    double targetProgress = 0.0;   // ʵ������Ŀ��
    bool needRedraw = true;

    PIMAGE barLayer = nullptr;

public:
    ProgressBar(int cx, int cy, double w, double h);
    ~ProgressBar();

    void setProgress(double p);      // Ŀ����ȣ�0~1��
    double getProgress() const;      // ���� target ֵ
    void setColor(color_t fg);       // ����ǰ����ɫ
    void setBackground(color_t bg);  // ���ñ�����ɫ

    void draw(PIMAGE dst, int x, int y) override;
    void draw() override;
    void handleEvent(const mouse_msg& msg) override;
    void setPosition(int x, int y) override;
    void setScale(double s) override;
};

 /**
  * @brief ���ڹ��� Panel ����Ĺ�������
  */
class PanelBuilder {
public:
    PanelBuilder& setIdentifier(const std::string& identifier);
    PanelBuilder& setCenter(int x, int y);
    PanelBuilder& setSize(double w, double h);
    PanelBuilder& setRadius(double r);
    PanelBuilder& setBackground(color_t color);
    PanelBuilder& setScale(double s);
    PanelBuilder& addChild(Widget* child, double offsetX, double offsetY);
    Panel* build();

private:
    std::string identifier;
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
    ButtonBuilder& setIdentifier(const std::string& identifier);
    ButtonBuilder& setCenter(int x, int y);
    ButtonBuilder& setSize(double w, double h);
    ButtonBuilder& setRadius(double r);
    ButtonBuilder& setContent(const std::string& text);
    ButtonBuilder& setScale(double s);
    ButtonBuilder& setOnClick(std::function<void()> func);
    ButtonBuilder& setColor(color_t col);
    ButtonBuilder& setIcon(PIMAGE img);
    ButtonBuilder& setIconSize(int size);
    Button* build();

private:
    std::string identifier;
    int cx = 0, cy = 0;
    double width = 100, height = 50;
    double radius = 8;
    std::string content = "Button";
    double scale = 1.0;
    color_t color = EGERGB(245, 245, 235);
    std::function<void()> onClick = nullptr;
    PIMAGE icon = nullptr;
    int iconSize = 100;
};

/**
 * @brief ���ڹ��� InputBox ����Ĺ�������
 */
class InputBoxBuilder {
public:
    InputBoxBuilder& setIdentifier(const std::string& identifier);
    InputBoxBuilder& setCenter(int x, int y);
    InputBoxBuilder& setSize(double w, double h);
    InputBoxBuilder& setRadius(double r);
    InputBoxBuilder& setContent(const std::string& text);
    InputBoxBuilder& setMaxLength(int maxLen);
    InputBoxBuilder& setScale(double s);
    InputBox* build();

private:
    std::string identifier;
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
    SliderBuilder& setIdentifier(const std::string& identifier);
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

    SliderBuilder& setOrientation(Orientation ori);

    /**
     * @brief ���������� Slider ����
     */
    Slider* build();

private:
    std::string identifier;
    int x = 0, y = 0;
    double width = 200, height = 20;
    color_t bgColor = EGERGB(200, 200, 200);
    color_t fgColor = EGERGB(100, 100, 255);
    double thickness = 4;
    double progress = 0.0;
    double scale = 1.0;
    std::function<void(double)> onChange = nullptr;
    Orientation orientation = Orientation::Row;
};

class ProgressBarBuilder {
public:
    ProgressBarBuilder& setIdentifier(const std::string& identifier);
    ProgressBarBuilder& setCenter(int x, int y);
    ProgressBarBuilder& setSize(double w, double h);
    ProgressBarBuilder& setScale(double s);
    ProgressBarBuilder& setProgress(double p);
    ProgressBarBuilder& setColor(color_t fg);
    ProgressBarBuilder& setBackground(color_t bg);
    ProgressBar* build();

private:
    std::string identifier;
    int cx = 0, cy = 0;
    double width = 200, height = 20;
    double scale = 1.0;
    double progress = 0.0;
    color_t fgColor = EGERGB(100, 180, 255);
    color_t bgColor = EGERGB(230, 230, 230);
};

class Dropdown : public Widget {
public:
    Dropdown(int cx, int cy, double w, double h, double r);

    void addOption(const std::string& text, std::function<void()> onClick);
    void setContent(const std::string& text);
    void setColor(color_t col);
    void setScale(double s) override;
    void setPosition(int x, int y) override;

    void draw(PIMAGE dst, int x, int y) override;
    void draw() override;
    void handleEvent(const mouse_msg& msg) override;
    bool isInside(int x, int y) const;


private:
    Button* mainButton;
    Panel* dropdownPanel;
    std::vector<Button*> options;
    bool expanded = false;
    int cx, cy;
    double width, height, radius;
    double scale = 1.0;
    double fadeAlpha = 0.0;
    bool fadingIn = false;
    bool fadingOut = false;

    color_t color = EGERGB(245, 245, 235);

    void toggleDropdown();
    void updateDropdownLayout();
};

class DropdownBuilder {
public:
    DropdownBuilder& setIdentifier(const std::string& identifier);
    DropdownBuilder& setCenter(int x, int y);
    DropdownBuilder& setSize(double w, double h);
    DropdownBuilder& setRadius(double r);
    DropdownBuilder& setContent(const std::string& text);
    DropdownBuilder& setColor(color_t col);
    DropdownBuilder& setScale(double s);
    DropdownBuilder& addOption(const std::string& text, std::function<void()> onClick);
    Dropdown* build();

private:
    std::string identifier;
    int cx = 0, cy = 0;
    double width = 100, height = 40;
    double radius = 8;
    double scale = 1.0;
    std::string content = "�˵�";
    color_t color = EGERGB(245, 245, 235);

    std::vector<std::pair<std::string, std::function<void()>>> optionList;
};

enum class RadioStyle {
    Filled,  // ʵ�ı���
    Outline  // ����Բ
};

class Radio : public Widget {
public:
    Radio(int cx, int cy, double r, const std::string& val);

    void setOnSelect(std::function<void()> callback);
    std::string getValue() const;
    bool isChecked() const;
    void setPosition(int x, int y) override;
    void setGroupValueRef(std::string* ref);
    void setScale(double s) override;
    void setStyle(RadioStyle s);
    void draw(PIMAGE dst, int x, int y) override;
    void draw() override;
    void handleEvent(const mouse_msg& msg) override;

private:
    int cx, cy;
    double radius, scale = 1.0;
    double origin_radius;
    std::string value;
    std::string groupValue;  // ��ǰ��ѡ�е�ֵ
    std::string* groupValuePtr = nullptr;
    bool animIn = false;
    bool animOut = false;
    double animProgress = 0.0;
    const double animSpeed = 1.0 / 16;  // ���ƶ���֡�ʣ�16֡��
    double lastScale = 1.0;  // �������һ֡ scale�����ڶ����������ȶ���ʾ
    bool wasChecked = false;
    bool keepColor = false;

    bool hovered = false;
    bool animPlaying = false;
    int animFrame = 0; // 0~10
    std::function<void()> onSelect;

    RadioStyle style = RadioStyle::Filled;
};

class RadioBuilder {
public:
    RadioBuilder& setIdentifier(const std::string& identifier);
    RadioBuilder& setCenter(int x, int y);
    RadioBuilder& setRadius(double r);
    RadioBuilder& setValue(const std::string& val);
    RadioBuilder& setScale(double s);
    RadioBuilder& setGroupValueRef(std::string* ref);
    RadioBuilder& setOnSelect(std::function<void()> cb);
    RadioBuilder& setStyle(RadioStyle s);
    Radio* build();

private:
    std::string identifier;
    int cx = 0, cy = 0;
    double radius = 12;
    double scale = 1.0;
    std::string value = "";
    std::string* groupPtr = nullptr;
    std::function<void()> onSelect = nullptr;

    RadioStyle style = RadioStyle::Filled;
};

class RadioController {
public:
    RadioController(int cx, int cy, double r, double gap, double scale, RadioStyle style);

    void addValue(const std::string& val);
    void setDefault(const std::string& val);
    void setOnChange(std::function<void(const std::string&)> cb);
    std::string getValue();
    void build();

private:
    std::string identifier;
    int cx, cy;
    double radius, scale, gap;
    RadioStyle style;
    std::vector<std::string> values;
    std::string currentValue;
    std::function<void(const std::string&)> onChange;
};

class RadioControllerBuilder {
public:
    RadioControllerBuilder& setIdentifier(const std::string& identifier);
    RadioControllerBuilder& setCenter(int x, int y);
    RadioControllerBuilder& setRadius(double r);
    RadioControllerBuilder& setGap(double g);
    RadioControllerBuilder& setScale(double s);
    RadioControllerBuilder& setStyle(RadioStyle s);
    RadioControllerBuilder& add(const std::string& val);
    RadioControllerBuilder& setDefault(const std::string& val);
    RadioControllerBuilder& setOnChange(std::function<void(const std::string&)> cb);
    RadioController* build();

private:
    std::string identifier;
    int cx = 0, cy = 0;
    double radius = 12;
    double gap = 40;
    double scale = 1.0;
    RadioStyle style = RadioStyle::Filled;
    std::vector<std::string> values;
    std::string defaultValue = "";
    std::function<void(const std::string&)> onChange = nullptr;
};

class Toggle : public Widget {
public:
    Toggle(int cx, int cy, double w, double h);

    void setChecked(bool c);
    void toggle();
    bool isChecked() const;
    bool isDisabled() const;
    void setOnToggle(std::function<void(bool)> cb);
    void setScale(double s);
    void setPosition(int x, int y) override;
    void setKeepColor(bool keep);
    void setBaseColor(color_t col);
    void setDisabled(bool d);

    void draw(PIMAGE dst, int x, int y) override;
    void draw() override;
    void handleEvent(const mouse_msg& msg) override;

private:
    int cx, cy;
    double width, height, scale = 1.0;

    bool checked = false;
    double knobOffset = 0.0;     // ������ǰֵ
    bool hovered = false;
    double knobTarget = 0.0;     // ����Ŀ��ֵ
    bool pressedIn = false;
    bool disabled = false;

    const double animationSpeed = 0.1;
    bool keepColor = false;
    color_t baseColor = EGERGB(33, 150, 243);


    std::function<void(bool)> onToggle;
};

class ToggleBuilder {
public:
    ToggleBuilder& setIdentifier(const std::string& identifier);
    ToggleBuilder& setCenter(int x, int y);
    ToggleBuilder& setSize(double w, double h);
    ToggleBuilder& setScale(double s);
    ToggleBuilder& setChecked(bool c);
    ToggleBuilder& setOnToggle(std::function<void(bool)> cb);
    ToggleBuilder& setKeepColor(bool keep);
    ToggleBuilder& setBaseColor(color_t col);
    Toggle* build();

private:
    std::string identifier;
    int cx = 0, cy = 0;
    double w = 60, h = 30, scale = 1.0;
    bool checked = false;
    std::function<void(bool)> onToggle = nullptr;
    bool keepColor = false;
    color_t baseColor = EGERGB(33, 150, 243);
};

enum class TextAlign {
    Left,
    Center,
    Right
};

class Text : public Widget {
public:
    Text(int x, int y, int maxWidth = 0);

    void setContent(const std::string& text);      // UTF-8 / GBK �Զ�ת��
    void setMaxWidth(int width);
    void setFont(int size, const std::string& name = "Consolas");
    void setColor(color_t col);
    void setScale(double s);
    void setAlign(TextAlign a);
    void setLineSpacing(int spacing);

    int getTextWidth() const;
    int getTextHeight() const;
    int getMaxWidth() const;
    TextAlign getAlign() const;
    int getLineSpacing() const;

    void draw(PIMAGE dst, int x, int y) override;
    void draw() override;
    void setPosition(int x, int y) override;
    void handleEvent(const mouse_msg& msg) override;

private:
    void updateLayout();

    int posX = 0, posY = 0;
    int maxWidth = 0;
    double scale = 1.0;
    int fontSize = 16;
    std::string fontName = "Consolas";
    color_t color = BLACK;

    std::wstring contentW;
    std::vector<std::wstring> lines;
    int textWidth = 0;
    int textHeight = 0;
    TextAlign align = TextAlign::Left;
    int lineSpacing = 0; // �����о�
};

class TextBuilder {
public:
    TextBuilder& setIdentifier(const std::string& identifier);
    TextBuilder& setPosition(int x, int y);
    TextBuilder& setMaxWidth(int w);
    TextBuilder& setFont(int size, const std::string& name);
    TextBuilder& setScale(double s);
    TextBuilder& setColor(color_t c);
    TextBuilder& setContent(const std::string& text);
    TextBuilder& setAlign(TextAlign a);
    TextBuilder& setLineSpacing(int px);
    Text* build();

private:
    std::string identifier;
    int x = 0, y = 0;
    int maxWidth = 0;
    int fontSize = 16;
    std::string fontName = "Consolas";
    double scale = 1.0;
    color_t color = BLACK;
    std::string content;
    TextAlign align = TextAlign::Left;
    int lineSpacing = 0;
};

class Knob : public Widget {
public:
    Knob(int cx, int cy, double r);

    void setRange(double minVal, double maxVal);
    void setStep(double step);
    void setValue(double val);
    void setColor(color_t fg, color_t bg);
    void setOnChange(std::function<void(double)> cb);

    double getValue() const;

    void setScale(double s) override;
    void setPosition(int x, int y) override;
    void draw(PIMAGE dst, int x, int y) override;
    void draw() override;
    void handleEvent(const mouse_msg& msg) override;

private:
    int cx, cy;
    double radius;
    double scale = 1.0;

    double minValue = 0.0;
    double maxValue = 100.0;
    double step = 1.0;
    double value = 0.0;

    color_t fgColor = EGERGB(33, 150, 243);
    color_t bgColor = EGERGB(230, 230, 230);

    bool dragging = false;
    int lastY = 0;

    std::function<void(double)> onChange;

    double clamp(double v);
    double valueToAngle(double v) const;
};

extern std::set<Widget*> widgets;
extern std::map<std::string,Widget*> IdToWidget;

Widget* getWidgetById(const std::string& identifier);
