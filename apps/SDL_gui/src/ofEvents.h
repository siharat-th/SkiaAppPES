//
//  ofEvents.h
//  EMBrilliance
//
//  Created by Siharat on 7/27/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef ofEvents_h
#define ofEvents_h

#ifdef _OF_EVENTS_

#ifndef _ofCoreEvents_
#define _ofCoreEvents_
class ofCoreEvents {
public:
    ofCoreEvents();
    ofEvent<ofEventArgs> 		setup;
    ofEvent<ofEventArgs> 		update;
    ofEvent<ofEventArgs> 		draw;
    ofEvent<ofEventArgs> 		exit;
    
    ofEvent<ofResizeEventArgs> 	windowResized;
    
    ofEvent<ofKeyEventArgs> 	keyPressed;
    ofEvent<ofKeyEventArgs> 	keyReleased;
    
    ofEvent<ofMouseEventArgs> 	mouseMoved;
    ofEvent<ofMouseEventArgs> 	mouseDragged;
    ofEvent<ofMouseEventArgs> 	mousePressed;
    ofEvent<ofMouseEventArgs> 	mouseReleased;
    ofEvent<ofMouseEventArgs> 	mouseScrolled;
    ofEvent<ofMouseEventArgs> 	mouseEntered;
    ofEvent<ofMouseEventArgs> 	mouseExited;
    
    ofEvent<ofTouchEventArgs>	touchDown;
    ofEvent<ofTouchEventArgs>	touchUp;
    ofEvent<ofTouchEventArgs>	touchMoved;
    ofEvent<ofTouchEventArgs>	touchDoubleTap;
    ofEvent<ofTouchEventArgs>	touchCancelled;
    
    ofEvent<ofMessage>			messageEvent;
//    ofEvent<ofDragInfo>			fileDragEvent;
    
    void disable();
    void enable();
    
//    void setFrameRate(int _targetRate);
//    float getFrameRate() const;
//    float getTargetFrameRate() const;
//    double getLastFrameTime() const;
//    uint64_t getFrameNum() const;
    
    bool getMousePressed(int button=-1) const;
    bool getKeyPressed(int key=-1) const;
    int getMouseX() const;
    int getMouseY() const;
    int getPreviousMouseX() const;
    int getPreviousMouseY() const;
    
    //  event notification only for internal OF use
    void notifySetup();
    void notifyUpdate();
    void notifyDraw();
    
    void notifyKeyPressed(int key, int keycode=-1, int scancode=-1, int codepoint=-1);
    void notifyKeyReleased(int key, int keycode=-1, int scancode=-1, int codepoint=-1);
    void notifyKeyEvent(const ofKeyEventArgs & keyEvent);
    
    void notifyMousePressed(int x, int y, int button);
    void notifyMouseReleased(int x, int y, int button);
    void notifyMouseDragged(int x, int y, int button);
    void notifyMouseMoved(int x, int y);
    void notifyMouseScrolled(int x, int y, float scrollX, float scrollY);
    void notifyMouseEntered(int x, int y);
    void notifyMouseExited(int x, int y);
    void notifyMouseEvent(const ofMouseEventArgs & mouseEvent);
    
    void notifyExit();
    void notifyWindowResized(int width, int height);
    
    void notifyDragEvent(ofDragInfo info);
    
private:
    float targetRate;
    bool bFrameRateSet;
    ofTimer timer;
    ofFpsCounter fps;
    
    int	currentMouseX, currentMouseY;
    int	previousMouseX, previousMouseY;
    bool bPreMouseNotSet;
    set<int> pressedMouseButtons;
    set<int> pressedKeys;
};

void ofSendMessage(ofMessage msg);
void ofSendMessage(string messageString);

ofCoreEvents & ofEvents(); // 1-4-all

template<class ListenerClass>
void ofRegisterMouseEvents(ListenerClass * listener, int prio=OF_EVENT_ORDER_AFTER_APP){
    ofAddListener(ofEvents().mouseDragged,listener,&ListenerClass::mouseDragged,prio);
    ofAddListener(ofEvents().mouseMoved,listener,&ListenerClass::mouseMoved,prio);
    ofAddListener(ofEvents().mousePressed,listener,&ListenerClass::mousePressed,prio);
    ofAddListener(ofEvents().mouseReleased,listener,&ListenerClass::mouseReleased,prio);
    ofAddListener(ofEvents().mouseScrolled,listener,&ListenerClass::mouseScrolled,prio);
    ofAddListener(ofEvents().mouseEntered,listener,&ListenerClass::mouseEntered,prio);
    ofAddListener(ofEvents().mouseExited,listener,&ListenerClass::mouseExited,prio);
}

template<class ListenerClass>
void ofRegisterKeyEvents(ListenerClass * listener, int prio=OF_EVENT_ORDER_AFTER_APP){
    ofAddListener(ofEvents().keyPressed, listener, &ListenerClass::keyPressed,prio);
    ofAddListener(ofEvents().keyReleased, listener, &ListenerClass::keyReleased,prio);
}

template<class ListenerClass>
void ofRegisterTouchEvents(ListenerClass * listener, int prio=OF_EVENT_ORDER_AFTER_APP){
    ofAddListener(ofEvents().touchDoubleTap, listener, &ListenerClass::touchDoubleTap,prio);
    ofAddListener(ofEvents().touchDown, listener, &ListenerClass::touchDown,prio);
    ofAddListener(ofEvents().touchMoved, listener, &ListenerClass::touchMoved,prio);
    ofAddListener(ofEvents().touchUp, listener, &ListenerClass::touchUp,prio);
    ofAddListener(ofEvents().touchCancelled, listener, &ListenerClass::touchCancelled,prio);
}

template<class ListenerClass>
void ofRegisterGetMessages(ListenerClass * listener, int prio=OF_EVENT_ORDER_AFTER_APP){
    ofAddListener(ofEvents().messageEvent, listener, &ListenerClass::gotMessage,prio);
}

//template<class ListenerClass>
//void ofRegisterDragEvents(ListenerClass * listener, int prio=OF_EVENT_ORDER_AFTER_APP){
//    ofAddListener(ofEvents().fileDragEvent, listener, &ListenerClass::dragEvent,prio);
//}

template<class ListenerClass>
void ofUnregisterMouseEvents(ListenerClass * listener, int prio=OF_EVENT_ORDER_AFTER_APP){
    ofRemoveListener(ofEvents().mouseDragged,listener,&ListenerClass::mouseDragged,prio);
    ofRemoveListener(ofEvents().mouseMoved,listener,&ListenerClass::mouseMoved,prio);
    ofRemoveListener(ofEvents().mousePressed,listener,&ListenerClass::mousePressed,prio);
    ofRemoveListener(ofEvents().mouseReleased,listener,&ListenerClass::mouseReleased,prio);
    ofRemoveListener(ofEvents().mouseScrolled,listener,&ListenerClass::mouseScrolled,prio);
    ofRemoveListener(ofEvents().mouseEntered,listener,&ListenerClass::mouseEntered,prio);
    ofRemoveListener(ofEvents().mouseExited,listener,&ListenerClass::mouseExited,prio);
}

template<class ListenerClass>
void ofUnregisterKeyEvents(ListenerClass * listener, int prio=OF_EVENT_ORDER_AFTER_APP){
    ofRemoveListener(ofEvents().keyPressed, listener, &ListenerClass::keyPressed,prio);
    ofRemoveListener(ofEvents().keyReleased, listener, &ListenerClass::keyReleased,prio);
}

template<class ListenerClass>
void ofUnregisterTouchEvents(ListenerClass * listener, int prio=OF_EVENT_ORDER_AFTER_APP){
    ofRemoveListener(ofEvents().touchDoubleTap, listener, &ListenerClass::touchDoubleTap,prio);
    ofRemoveListener(ofEvents().touchDown, listener, &ListenerClass::touchDown,prio);
    ofRemoveListener(ofEvents().touchMoved, listener, &ListenerClass::touchMoved,prio);
    ofRemoveListener(ofEvents().touchUp, listener, &ListenerClass::touchUp,prio);
    ofRemoveListener(ofEvents().touchCancelled, listener, &ListenerClass::touchCancelled,prio);
}

template<class ListenerClass>
void ofUnregisterGetMessages(ListenerClass * listener, int prio=OF_EVENT_ORDER_AFTER_APP){
    ofRemoveListener(ofEvents().messageEvent, listener, &ListenerClass::gotMessage,prio);
}

//template<class ListenerClass>
//void ofUnregisterDragEvents(ListenerClass * listener, int prio=OF_EVENT_ORDER_AFTER_APP){
//    ofRemoveListener(ofEvents().fileDragEvent, listener, &ListenerClass::dragEvent,prio);
//}

#endif

#endif // endif #ifdef _OF_EVENTS_

#endif /* ofEvents_h */
