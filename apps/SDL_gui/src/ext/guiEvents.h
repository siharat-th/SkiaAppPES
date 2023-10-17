//
//  guiEvents.h
//  PINNPES
//
//  Created by Siharat on 8/12/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef guiEvents_hpp
#define guiEvents_hpp

#include <stdio.h>
#include <set>
#include <functional>
#include "SDL_gui.h"

namespace gui{
    namespace priv{
        
        class AbstractEventToken{
        public:
            virtual ~AbstractEventToken();
        };
        
        
        
        // -------------------------------------
        enum guiEventOrder{
            GUI_EVENT_ORDER_BEFORE_APP=0,
            GUI_EVENT_ORDER_APP=100,
            GUI_EVENT_ORDER_AFTER_APP=200
        };
        
        // -------------------------------------
        class guiEventListener{
        public:
            guiEventListener(){}
            guiEventListener(std::unique_ptr<gui::priv::AbstractEventToken> && token)
            :token(std::move(token)){}
            void unsubscribe(){
                token.reset();
            }
        private:
            std::unique_ptr<gui::priv::AbstractEventToken> token;
        };
    }
    
    bool GetMousePressed(int button=-1); //by default any button
    bool GetKeyPressed(int key=-1); //by default any key
    
    int   GetMouseX();
    int   GetMouseY();
    
    int   GetPreviousMouseX();
    int   GetPreviousMouseY();
    
    
    
    class EventArgs{};
    
    class KeyEventArgs : public EventArgs {
    public:
        enum struct Type {
            Pressed,
            Released,
        };
        
        KeyEventArgs(Type type, int key)
        :type(type)
        ,key(key)
        ,keycode(0)
        ,scancode(0)
        ,codepoint(0)
        ,isRepeat(false){
            
        }
        
        Type type;
        int key;
        int keycode;
        /// The raw scan code returned by the keyboard, OS and hardware specific.
        int scancode;
        uint32_t codepoint;
        bool isRepeat;
        int modifiers = 0;
        
        bool hasModifier(int modifier){
            return modifiers & modifier;
        }
    };
    
    class MouseEventArgs : public EventArgs, public guiVec2f {
    public:
        enum struct Type {
            Pressed,
            Moved,
            Released,
            Dragged,
            Scrolled,
            Entered,
            Exited
        };
        
        MouseEventArgs()
        :type(Type::Pressed)
        ,button(0)  //OF_MOUSE_BUTTON_LEFT == OF_MOUSE_BUTTON_1 == OF_MOUSE_BUTTON_1 = 0
        ,scrollX(0.f)
        ,scrollY(0.f)
        {}
        
        MouseEventArgs(Type type, float _x, float _y, int button)
        :guiVec2f(_x,_y)
        ,type(type)
        ,button(button)
        ,scrollX(0.f)
        ,scrollY(0.f)
        {}
        
        Type type;
        int button;
        float scrollX;
        float scrollY;
        /// Key modifiers
        int modifiers = 0;
        
        bool hasModifier(int modifier){
            return modifiers & modifier;
        }
    };
    
    
    template <class EventType,typename ArgumentsType>
    inline bool NotifyEvent(EventType & event, ArgumentsType & args){
        return event.notify(args);
    }
    
    
    class CoreEvents{
    public:
        CoreEvents();
        bool getMousePressed(int button=-1) const;
        bool getKeyPressed(int key=-1) const;
        int getMouseX() const;
        int getMouseY() const;
        int getPreviousMouseX() const;
        int getPreviousMouseY() const;
        int getModifiers() const;
        
        bool notifyKeyPressed(int key, int keycode=-1, int scancode=-1, uint32_t codepoint=0);
        bool notifyKeyReleased(int key, int keycode=-1, int scancode=-1, uint32_t codepoint=0);
        bool notifyKeyEvent(KeyEventArgs & keyEvent);
        
        bool notifyMousePressed(int x, int y, int button);
        bool notifyMouseReleased(int x, int y, int button);
        bool notifyMouseDragged(int x, int y, int button);
        bool notifyMouseMoved(int x, int y);
        bool notifyMouseScrolled(int x, int y, float scrollX, float scrollY);
        bool notifyMouseEvent(MouseEventArgs & mouseEvent);
        
    private:
        int    currentMouseX, currentMouseY;
        int    previousMouseX, previousMouseY;
        bool bPreMouseNotSet;
        std::set<int> pressedMouseButtons;
        std::set<int> pressedKeys;
        int modifiers = 0;
    };
    
    CoreEvents & Events();
    
}







#endif /* guiEvents_hpp */
