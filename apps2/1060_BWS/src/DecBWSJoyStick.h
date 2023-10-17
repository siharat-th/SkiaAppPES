#ifndef __DECBWSJOYSTICK_LOADER_H__
#define __DECBWSJOYSTICK_LOADER_H__

#include "DeclarativeUI.h"

class DecBWSJoyStick : public DecContainer {
public:    
    DecBWSJoyStick();
    virtual ~DecBWSJoyStick() override;
    virtual std::string type() override { return "DecBWSJoyStick"; }
    inline virtual DecWidget& text(const char* t) override {
        _text = std::string(t);
        if (_text == "gamepadconnected") 
            bGamepadConnected = true;
        else if (_text == "gamepaddisconnected") 
            bGamepadConnected = false;
        return *this;
    }

    virtual void update() override;

    virtual void _draw_content(SkCanvas& canvas) override;    

    void drawD( SkCanvas &canvas, int x, int y, int degree, SkPaint &paint );
    bool bGamepadConnected;
    bool bJoyUp;
    bool bJoyLeft;
    bool bJoyRight;
    bool bJoyDown;

    bool bDUp;
    bool bDLeft;
    bool bDRight;
    bool bDDown;

    bool bA;
    bool bB;

    bool bR2;

    float jX;
    float jY;
};

DecBWSJoyStick &BWSJoyStick();

#endif //__DECBWSJOYSTICK_LOADER_H__