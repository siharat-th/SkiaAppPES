//
//  guiDigit.cpp
//  VG2EMB
//
//  Created by Siharat on 24/11/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "guiDigit.hpp"

namespace gui{
    
    Digit::Digit(GUI_View *p, std::string l, std::string format,
                 int x, int y, int w, int h,
                 SDL_Color col, int align, int fnt)
    :GUI_Label( p, "", x, y, w, h, col, align, fnt ),
    _format(format),
    _min(0),
    _max(2),
    _value(1),
    _inc(1),
    valueLabel(nullptr),
    labelWidth(0),
    corners{0,0,0,0},
    value_change_cmd(nullptr)
    {
        setLayout(GUI_LAYOUT_HORIZONTAL);
//        setPadding(1,1,1,1);
//        setPadding(0,0,0,0);
        highlightStroke = defaultTheme.color.highlightStroke;
        
        label = new GUI_Label(this, l.c_str());
        label->setTitle(l, fnt);
        label->setPadding(3, 0, 3, 0);
        labelWidth = (float)label->tw_area.w;
        
        std::string str = guiVAArgsToString(_format.c_str(), getValue());
        valueLabel = new GUI_Label(this, str.c_str(), 0, 0, 48, 0, cDarkGrey, GUI_ALIGN_RIGHT | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL);
        valueLabel->setTitle(str, GUI_FONT_NORMAL);
        valueLabel->setPadding(3, 3, 3, 3);
        valueLabel->bgcol = defaultTheme.color.inputAreaBackground;
        valueLabel->titleColor = defaultTheme.color.label;
        valueLabel->borderColor = defaultTheme.color.border;
        valueLabel->setMargin(0, 0, 0, 3);
        valueLabel->setBorder(1);
        valueLabel->disable();
        labelWidth+=valueLabel->margin[3]*GUI_scale;
        
        btDec = new gui::Button(this, "-",0,0,24,0);
        btDec->setPadding(3,3,3,3);
        btDec->tag = -111;
        btDec->setCorners(0);
        
        btInc = new gui::Button(this, "+",0,0,24,0);
        btInc->setPadding(3,3,3,3);
        btInc->tag = 111;
        btInc->setCorners(0);
        
        setPadding(1,1,1,1);
        
        auto lampda = [=](Button* b){
            isFocus = true;
            if( GUI_mouseCapturedWindow && GUI_mouseCapturedWindow!=this)
                GUI_mouseCapturedWindow->isFocus = false;
            GUI_mouseCapturedWindow = (Digit *)b->parent;
            
            float tmp =getValue();
            float cur = tmp;
            cur = cur+((b->tag==111) ? _inc : -_inc);
            cur = guiClamp(cur, _min, _max);
            if(cur!=tmp){
                _value = cur;
                updateValueLabel();
                updateButtonState();
                
                if(value_change_cmd)
                    value_change_cmd(this, getValue(), tmp);
            }
        };
        
        auto hold_lampda = [=](Button* b, Uint32 holdTime){
//            GUI_LogNotice() << holdTime;
            float tmp = getValue();
            float cur = tmp;
            cur = cur+((b->tag==111) ? _inc : -_inc);
            cur = guiClamp(cur, _min, _max);
            if(cur!=tmp){
                _value = cur;
                updateValueLabel();
            }
        };
        
        auto hold_end_lampda = [=](Button* b, Uint32 holdTime){
            updateButtonState();
            if(holdTime>1000)
                if(value_change_cmd)
                    value_change_cmd(this, getValue(), getValue());
        };
        
        btDec->action_cmd = lampda;
        btInc->action_cmd = lampda;
        btDec->action_hold_cmd = hold_lampda;
        btInc->action_hold_cmd = hold_lampda;
        btDec->action_hold_end_cmd = hold_end_lampda;
        btInc->action_hold_end_cmd = hold_end_lampda;
        
        // TODO: implement Button::action_hold_cmd,action_hold_end_cmd (move from PES3 branch)
//        btDec->action_hold_cmd = hold_lampda;
//        btInc->action_hold_cmd = hold_lampda;
//        btDec->action_hold_end_cmd = hold_end_lampda;
//        btInc->action_hold_end_cmd = hold_end_lampda;
        
        handle_event_cmd = [&](GUI_View* v, SDL_Event* ev)->bool{

            switch(ev->type){
                case SDL_MOUSEBUTTONDOWN:{
                    if( this->isEnable() == false || this->isActive()==false)
                        return false;
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);
                    if( isFocus ) {
                        if( !hitTest(x, y) ) {
                            isFocus = false;
                            GUI_mouseCapturedWindow = NULL;
                            return false;
                        }
                    }
                    return false;
                }

                case SDL_MOUSEMOTION:{
                    return false;
                }

                case SDL_MOUSEBUTTONUP: {
                    if( this->isEnable() == false || this->isActive()==false)
                        return false;
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);
                    if( isFocus ) {
                        if( !hitTest(x, y) ) {
                            isFocus = false;
                            GUI_mouseCapturedWindow = NULL;
                            return false;
                        }
                    }
                    return false;
                }
            }

            return false;
        };
        
        display_cmd = [&](GUI_View* v){
            if(v->isFocus){
                v->setBorder(0);
                v->borderColor = highlightStroke;
            }
            else{
                v->setBorder(0);
                v->borderColor = cClear;
            }
        };
        
        updateButtonState();
    }
    
    Digit::~Digit(){
    }
    
    void Digit::setCorners(int topleft, int topright, int bottomright, int bottomleft){
        corners[0] = topleft;
        corners[1] = topright;
        corners[2] = bottomright;
        corners[3] = bottomleft;
    }
    
    void Digit::setCorners(int r){
        setCorners(r,r,r,r);
    }
    
    void Digit::set(float min, float max, float initial, float inc){
        _min = min;
        _max = max;
        _value = initial;
        _inc = inc;
        updateValueLabel();
        updateButtonState();
    }
    
    void Digit::setValue(float value){
        _value = value;
        updateValueLabel();
        
        if(isEnable()){
            updateButtonState();
        }
    }
    
    float Digit::getValue(){
        _value = guiClamp(_value, _min, _max);
        return _value;
    }
    
    void Digit::updateValueLabel(){
        std::string str = guiVAArgsToString(_format.c_str(), getValue());
        valueLabel->setTitle(str, GUI_FONT_NORMAL);
    }
    
    void Digit::updateButtonState(){
        float f = getValue();
        bool bdec = (f<=_min) ? false : true;
        bool binc = (f>=_max) ? false : true;
        btDec->setActive(bdec);
        btInc->setActive(binc);
    }
    
    void Digit::clear(GUI_Rect *r)
    {
        if(isFocus && GUI_mouseCapturedWindow &&
           (GUI_mouseCapturedWindow==this||(GUI_mouseCapturedWindow->parent && GUI_mouseCapturedWindow->parent==this)))
        {
            guiRectangle rect(labelWidth, 0.0f, tw_area.w-labelWidth, (float)tw_area.h);
            
            int c[4] = {
                corners[0]*GUI_scale,
                corners[1]*GUI_scale,
                corners[2]*GUI_scale,
                corners[3]*GUI_scale
            };

            GUI_FillRoundRect((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, c, highlightStroke);
        }
    }
    
    void Digit::enable(){
        GUI_View::enable();
        btDec->enable();
        btInc->enable();
        label->enable();
        updateButtonState();
    }
    
    void Digit::disable(){
        GUI_View::disable();
        btDec->disable();
        btInc->disable();
        label->disable();
        btDec->setActive(false);
        btInc->setActive(false);
    }
    
}
