//
//  guiButton.hpp
//  VG2EMB
//
//  Created by Siharat on 24/11/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef guiButton_hpp
#define guiButton_hpp

#include "GUI_Label.h"
#include "guiTheme.h"
#include "Shaper.hpp"

namespace gui {
    
    class Button : public GUI_Label{
        
    public:
        
        Button(GUI_View *parent, const std::string & title, int x=0, int y=0, int width=0, int height=0,
               SDL_Color color=defaultTitleColor, int align=GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        virtual ~Button() override;
        SDL_Color color_down;
        SDL_Color color_selected;
        
        void setCorners(int topleft, int topright, int bottomright, int bottomleft);
        void setCorners(int r);
        
        virtual void draw() override;
        virtual void clear(GUI_Rect *rect) override;
        virtual bool handleEvents( SDL_Event *ev ) override;
        
        void setActive(bool active);
        void setActive(bool active, SDL_Color bg, SDL_Color fg);
        virtual bool isActive() override;
        
        bool toggle();
        bool getToggleState();
        
        gui::Theme theme;
        Uint32 touchTime, touchHoldTime, holdTime;
        std::function<void(Button*)> action_cmd;
        std::function<void(Button*, Uint32 holdTime)> action_hold_cmd;
        std::function<void(Button*, Uint32 holdTime)> action_hold_end_cmd;
        
        int corners[4];
        
    protected:
        bool isDown;
        bool bActive;
        bool toggle_state;
//        GUI_Path boundPath;
    };
    
    class TexturedButton : public Button {
    public:
        
        TexturedButton(GUI_View *parent, SDL_Texture *icon, bool isTextureColored, int x=0, int y=0, int width=0, int height=0);
        virtual ~TexturedButton() override;
        
        virtual bool handleEvents( SDL_Event *ev ) override;
        virtual bool toggle();
        
        bool isTextureColored;
        
    private:
        bool focus, press, down;
    };
    
    
    
    class PopupButton : public Button {
    public:
        PopupButton(GUI_View* parent, std::string default_title, int x=0, int y=0, int width=0, int height=0, int align=GUI_ALIGN_RIGHT|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        
        virtual ~PopupButton() override;
        
        virtual bool handleEvents( SDL_Event *ev ) override;
        virtual void clear(GUI_Rect *rect) override;
        virtual void updateSubLayout() override;
        
    protected:
        GUI_Path caretPath;
        GUI_Rect caret_tw_area;
        
        bool isHighlightFocus;
    };
    
}

#endif /* guiButton_hpp */
