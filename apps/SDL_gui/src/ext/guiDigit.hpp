//
//  guiDigit.hpp
//  VG2EMB
//
//  Created by Siharat on 24/11/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef guiDigit_hpp
#define guiDigit_hpp

#include "GUI_Label.h"
#include "guiTheme.h"
#include "guiButton.hpp"

namespace gui{
    
    class Digit : public GUI_Label{
    public:
        Digit(GUI_View *parent, std::string label, std::string format, int x=0, int y=0, int width=0, int height=0,
              SDL_Color color=defaultTitleColor, int align=GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        virtual ~Digit() override;
        
        void set(float min, float max, float initial, float inc);
        void setValue(float value);
        
        float getValue();
        
        virtual void clear(GUI_Rect *rect) override;
        virtual void enable() override;
        virtual void disable() override;
        
        void setCorners(int topleft, int topright, int bottomright, int bottomleft);
        void setCorners(int r);
        
        std::function<void(gui::Digit*, float newValue, float oldValue)> value_change_cmd;
        
    protected:
        void updateValueLabel();
        void updateButtonState();
        
        float _min, _max, _value, _inc;
        std::string _format;
        GUI_Label* label;
        GUI_Label* valueLabel;
        gui::Button *btDec, *btInc;
        float labelWidth;
        
        SDL_Color highlightStroke;
        
        int corners[4];
        GUI_Path boundPath;
        
    };
    
}

#endif /* guiDigit_hpp */
