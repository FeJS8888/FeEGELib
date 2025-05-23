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
#include <iostream>
#include <functional>
#include "sys_edit.h"  // mouse_msg �Ƚṹ��
#include "FeEGELib.h"

using namespace FeEGE;

/**
 * @brief ���ƶ���Ч���ṹ��
 */
struct Ripple {
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
    void draw(PIMAGE dst) const;
};

/**
 * @brief ��ť�ؼ���
 */
class Button {
private:
    int centerX, centerY;   ///< ��������
    int width, height;      ///< ���
    int radius;             ///< Բ�ǰ뾶
    int left, top;          ///< ���Ͻ�����
    std::vector<Ripple> ripples; ///< ����Ч������
    PIMAGE btnLayer = nullptr;  ///< ��ťͼ��
    PIMAGE maskLayer = nullptr; ///< ����ͼ��

public:
    /**
     * @brief ���캯��
     * @param cx ����x����
     * @param cy ����y����
     * @param w ���
     * @param h �߶�
     * @param r Բ�ǰ뾶
     */
    Button(int cx, int cy, int w, int h, int r);

    /**
     * @brief ��������
     */
    virtual ~Button();

    /**
     * @brief ���ư�ť
     * @param content ��ť�ı�
     */
    virtual void draw(const std::string& content);

    /**
     * @brief ��������¼�
     * @param msg �����Ϣ
     */
    virtual void handleEvent(const mouse_msg& msg);

    /**
     * @brief �����Ƿ��ڰ�ť��
     * @param x x����
     * @param y y����
     * @return �Ƿ��ڰ�ť��
     */
    bool isInside(int x, int y);
};

/**
 * @brief �����ؼ���
 */
class InputBox {
private:
    int centerX, centerY;   ///< ��������
    int width, height;     ///< ���
    int radius;            ///< Բ�ǰ뾶
    int left, top;         ///< ���Ͻ�����
    std::vector<Ripple> ripples; ///< ����Ч������
    PIMAGE btnLayer = nullptr;  ///< ��ťͼ��
    PIMAGE maskLayer = nullptr; ///< ����ͼ��
    std::string content;   ///< ��������
    FeEGE::sys_edit inv;   ///< ����ؼ�
    bool on_focus;         ///< �Ƿ��ý���

public:
    /**
     * @brief ���캯��
     * @param cx ����x����
     * @param cy ����y����
     * @param w ���
     * @param h �߶�
     * @param r Բ�ǰ뾶
     */
    InputBox(int cx, int cy, int w, int h, int r);

    /**
     * @brief ��������
     */
    virtual ~InputBox();

    /**
     * @brief ���������
     */
    virtual void draw();

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
    bool isInside(int x, int y);

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
};

/**
 * @brief �������ؼ���
 */
class Slider {
private:
    int m_x, m_y, m_w, m_h;    ///< λ�úͳߴ�
    int m_radius;              ///< ����뾶
    int m_dragOffset;          ///< �϶�ƫ����
    double m_value;            ///< ��ǰֵ(0.0-1.0)
    bool m_dragging;           ///< �Ƿ������϶�
    color_t m_bgColor, m_fgColor; ///< ������ǰ��ɫ
    std::function<void(double)> m_onChange; ///< ֵ�ı�ص�

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
    void create(int x, int y, int w, int h);

    /**
     * @brief ���ƻ�����
     */
    void draw();

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
     * @brief ����ֵ�ı�ص�
     * @param callback �ص�����
     */
    void setOnChange(std::function<void(double)> callback);
};

/**
 * @brief �ı���ʾ�ؼ���
 */
class Text {
private:
    std::string content;   ///< �ı�����
    float scale = 1.0f;    ///< ���ű���
    std::string font;      ///< ����
    int wrapWidth = 300;   ///< �Զ����п��

    /**
     * @brief UTF8�ַ����ָ�
     * @param str �����ַ���
     * @return �ָ����ַ�����
     */
    static std::vector<std::string> utf8_split(const std::string& str);

public:
    /**
     * @brief ��������
     * @param f ��������
     */
    void setFont(const std::string& f);

    /**
     * @brief �������ű���
     * @param s ���ű���
     */
    void setScale(float s);

    /**
     * @brief ��������
     * @param str �ı�����
     */
    void setContent(const std::string& str);

    /**
     * @brief ���û��п��
     * @param w ���ֵ
     */
    void setWrapWidth(int w);

    /**
     * @brief �����ı�
     * @param x x����
     * @param y y����
     */
    void draw(int x, int y) const;
};
