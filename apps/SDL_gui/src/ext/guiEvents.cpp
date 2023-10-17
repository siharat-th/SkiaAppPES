//
//  guiEvents.cpp
//  PINNPES
//
//  Created by Siharat on 8/12/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "guiEvents.h"

namespace gui {
    
    static CoreEvents __core_events;
    CoreEvents & Events(){
        return __core_events;
    }

    bool GetMousePressed(int button){
        return Events().getMousePressed(button);
    }

    bool GetKeyPressed(int key){
        return Events().getKeyPressed(key);
    }

    int  GetMouseX(){
        return Events().getMouseX();
    }

    int  GetMouseY(){
         return Events().getMouseY();
    }

    int  GetPreviousMouseX(){
         return Events().getPreviousMouseX();
    }

    int  GetPreviousMouseY(){
         return Events().getPreviousMouseY();
    }


    CoreEvents::CoreEvents()
    :currentMouseX(0)
    ,currentMouseY(0)
    ,previousMouseX(0)
    ,previousMouseY(0)
    ,bPreMouseNotSet(false)
    {
        
    }
    
    bool CoreEvents::getMousePressed(int button) const{
        if(button==-1) return pressedMouseButtons.size();
        return pressedMouseButtons.find(button)!=pressedMouseButtons.end();
    }
    
    bool CoreEvents::getKeyPressed(int key) const{
        if(key==-1) return pressedKeys.size();
        return pressedKeys.find(key)!=pressedKeys.end();
    }
    
    int CoreEvents::getMouseX() const{
        return currentMouseX;
    }
    
    int CoreEvents::getMouseY() const{
        return currentMouseY;
    }
    
    int CoreEvents::getPreviousMouseX() const{
        return previousMouseX;
    }
    
    int CoreEvents::getPreviousMouseY() const{
        return previousMouseY;
    }
    
    int CoreEvents::getModifiers() const{
        return modifiers;
    }
    
    bool CoreEvents::notifyKeyPressed(int key, int keycode, int scancode, uint32_t codepoint){
        KeyEventArgs keyEventArgs(KeyEventArgs::Type::Pressed, key);
        return notifyKeyEvent(keyEventArgs);
    }
    
    bool CoreEvents::notifyKeyReleased(int key, int keycode, int scancode, uint32_t codepoint){
        KeyEventArgs keyEventArgs(KeyEventArgs::Type::Released, key);
        return notifyKeyEvent(keyEventArgs);
    }
    
    bool CoreEvents::notifyKeyEvent(KeyEventArgs & e){
        bool attended = false;
        modifiers = e.modifiers;
        switch(e.type){
            case KeyEventArgs::Type::Pressed:
                pressedKeys.insert(e.key);
                return attended;
                
            case KeyEventArgs::Type::Released:
                pressedKeys.erase(e.key);
                    return attended;
        }
        return false;
    }
    
    bool CoreEvents::notifyMousePressed(int x, int y, int button){
        MouseEventArgs mouseEventArgs(MouseEventArgs::Type::Pressed, (float)x, (float)y, button);
        return notifyMouseEvent(mouseEventArgs);
    }
    
    bool CoreEvents::notifyMouseReleased(int x, int y, int button){
        MouseEventArgs mouseEventArgs(MouseEventArgs::Type::Released, (float)x, (float)y, button);
        return notifyMouseEvent(mouseEventArgs);
    }
    
    bool CoreEvents::notifyMouseDragged(int x, int y, int button){
        MouseEventArgs mouseEventArgs(MouseEventArgs::Type::Dragged, (float)x, (float)y, button);
        return notifyMouseEvent(mouseEventArgs);
    }
    
    bool CoreEvents::notifyMouseMoved(int x, int y){
        MouseEventArgs mouseEventArgs(MouseEventArgs::Type::Moved, (float)x, (float)y, 0);
        return notifyMouseEvent(mouseEventArgs);
    }
    
    bool CoreEvents::notifyMouseScrolled(int x, int y, float scrollX, float scrollY){
        MouseEventArgs mouseEventArgs;//(x,y);
        mouseEventArgs.scrollX = scrollX;
        mouseEventArgs.scrollY = scrollY;
        return notifyMouseEvent(mouseEventArgs);
    }
    
    bool CoreEvents::notifyMouseEvent(MouseEventArgs & e){
        modifiers = e.modifiers;
        switch(e.type){
            case MouseEventArgs::Type::Moved:
                if( bPreMouseNotSet ){
                    previousMouseX    = (int)e.x;
                    previousMouseY    = (int)e.y;
                    bPreMouseNotSet    = false;
                }else{
                    previousMouseX = currentMouseX;
                    previousMouseY = currentMouseY;
                }
                
                currentMouseX = (int)e.x;
                currentMouseY = (int)e.y;
                
                return false;
            case MouseEventArgs::Type::Dragged:
                if( bPreMouseNotSet ){
                    previousMouseX    = (int)e.x;
                    previousMouseY    = (int)e.y;
                    bPreMouseNotSet    = false;
                }else{
                    previousMouseX = currentMouseX;
                    previousMouseY = currentMouseY;
                }
                
                currentMouseX = (int)e.x;
                currentMouseY = (int)e.y;
                
                return false;
            case MouseEventArgs::Type::Pressed: {
                if( bPreMouseNotSet ){
                    previousMouseX    = (int)e.x;
                    previousMouseY    = (int)e.y;
                    bPreMouseNotSet    = false;
                }else{
                    previousMouseX = currentMouseX;
                    previousMouseY = currentMouseY;
                }
                
                currentMouseX = (int)e.x;
                currentMouseY = (int)e.y;
                pressedMouseButtons.insert(e.button);
                return false;
            }
            case MouseEventArgs::Type::Released:
                if( bPreMouseNotSet ){
                    previousMouseX    = (int)e.x;
                    previousMouseY    = (int)e.y;
                    bPreMouseNotSet    = false;
                }else{
                    previousMouseX = currentMouseX;
                    previousMouseY = currentMouseY;
                }
                
                currentMouseX = (int)e.x;
                currentMouseY = (int)e.y;
                pressedMouseButtons.erase(e.button);
                return false;
                
                default:
                break;
        }
        return false;
    }
    
}
