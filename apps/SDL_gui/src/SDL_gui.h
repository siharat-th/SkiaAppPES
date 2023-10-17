//
// Created by Jimmy on 2/9/2016 AD.
//

#ifndef SHAPES_SDL_GUI_H
#define SHAPES_SDL_GUI_H


#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#if __USE_SKIA__
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkPath.h"
#include "include/core/SkSurface.h"
#include "include/core/SkSwizzle.h"
#include "include/core/SkVertices.h"
#include "include/utils/SkRandom.h"
#include "include/core/SkColor.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#include "src/gpu/gl/GrGLDefines.h"
#include "skutil.h"
#endif


#ifndef __EMSCRIPTEN__
//#include <SDL_net.h>
#endif

#ifdef __WINDOWS__
#ifdef NULL
#undef NULL
#endif
#define NULL nullptr 
#endif

#if defined(WIN32) || defined(_MSC_VER)  || defined(__MACOSX__) || defined(__linux__) || defined(__ANDROID__)
#ifndef NDEBUG
#define DEBUG 1
#endif
#endif


#include "GUI_Utils.h"
#include "GUI_View.h"
#include "GUI_Dialog.hpp"
#include "GUI_Label.h"
#include "GUI_Text.h"
#include "GUI_Button.h"
#include "GUI_Image.h"
#include "GUI_ToolBar.h"
#include "GUI_Popup.h"
#include "GUI_ScrollView.h"
#include "GUI_Network.h"
#include "GUI_List.h"
#include "GUI_Switch.h"
#include "GUI_Dropdown.h"
#include "GUI_Slider.h"
#include "GUI_Math.h"
#include "GUI_Color.h"
#include "GUI_Preference.h"
#include "GUI_Downloader.h"


#if defined(WIN32)
#include <SDL_syswm.h>
#endif

// pom
// Uncomment to use Software Renderer
//#define SW_RENDER_SURFACE 1
#define SW_RENDER_FONTAWESOME 1

// pom

#define MSA_START \
Uint32 __tick__ = SDL_GetTicks();

#define MSA_REPORT \
Uint32 __elapsed__ = SDL_GetTicks() - __tick__; \
GUI_LogNotice( __FILE__ , "%s Elapsed: %i ms" , __func__ , __elapsed__ );


typedef struct __GUI_USER_MESSAGE {
    Uint32 message_id;
    Uint32 parameter1;
    Sint32 parameter2;
    void *extra_parameter1;
    void *extra_parameter2;
} GUI_UserMessage;

int GUI_Init(void);
void GUI_Quit(void);
//int GUI_Skia_Init(int & dw, int & dh, GrGLFramebufferInfo & info);
//void GUI_Skia_Quit(void);

SDL_Window *GUI_CreatePortraitWindow(std::string title = "", int w = 0, int h = 0, int x = -1, int y = -1, bool maximized = false);
SDL_Window *GUI_CreateLandscapeWindow(std::string title = "", int w = 0, int h = 0, int x = -1, int y = -1, bool maximized = false,
                                      bool HARDWARE_ACCELERATED = true);

SDL_Window* GUI_CreateWindow(const char *title = "GUI", int sx = 0, int sy = 0, bool HARDWARE_ACCELERATED = false, int x = -1, int y = -1, bool maximized = false);
void GUI_Run(std::function<bool(SDL_Event *ev)>handle_ev = NULL, bool bFPS = false);

// pom - Anti-alias version
void GUI_FillRoundRect(int x, int y, int xc, int yc, int radius, SDL_Color col);
void GUI_DrawCircle(int xc, int yc, int radius, SDL_Color col);

//extern const bool USE_D3D;
extern SDL_Window *GUI_window;
extern SDL_Renderer *GUI_renderer;

extern int GUI_physicalWindowWidth;
extern int GUI_physicalWindowHeight;

extern int GUI_windowWidth;
extern int GUI_windowHeight;

extern int GUI_ExpectedWindowWidth;
extern int GUI_ExpectedWindowHeight;

extern GUI_View *GUI_modalWindow;

extern GUI_View *GUI_TopView;

extern SDL_GLContext GUI_glContext;
#if __USE_SKIA__
extern SkCanvas *GUI_canvas;
#endif

class GUI_View;
GUI_View *GUI_CreateTopView(const char* wm_title, int x = 0, int y = 0, int w = -1, int h = -1, void(*disp_cmd)(GUI_View *) = NULL);

#if defined(WIN32)
void GUI_SetWindowIcon(int icon_id);
#endif

extern bool GUI_TakeScreenShot;
extern std::string GUI_ScreenShotPath;

extern const Uint32 GUI_EventID;
extern const Uint32 GUI_EventPaint;
extern const Uint32 GUI_EventListSelectd;
extern const Uint32 GUI_EventUser;

int SDL_GUI_Init(void);
void SDL_GUI_Exit(void);

void GUI_SetModal(GUI_View *view, bool bAutoHide = true);
bool GUI_IsModal(GUI_View *view);
void GUI_SetModalAutoClose(bool bAuto);
void GUI_SetModalAutoHide(bool bAuto);
void GUI_SetScaleParameters(void);

bool GUI_PostMessage(Uint32 msg, int param1, int param2, void *extra1, void *extra2);

void GUI_AddHeartbeat(GUI_Downloader * fn);
void GUI_RemoveHeartbeat(GUI_Downloader * fn);

//pom
void doGuiLoop();
bool isGUIRunning();
Uint32 getFrameTime();
int  GUI_InitFontAwesomeTexture();
void GUI_DeleteFontAwesomeTexture();

#if defined(WIN32)
extern HWND hwnd;
#endif

// pom
namespace sdl {
    namespace framerate {

#define FPS_UPPER_LIMIT	200
#define FPS_LOWER_LIMIT	1
#define FPS_DEFAULT     30

        typedef struct {
            Uint32 framecount;
            float rateticks;
            Uint32 baseticks;
            Uint32 lastticks;
            Uint32 rate;
        } FPSmanager;

        extern int SDL_setFramerate(FPSmanager * manager, Uint32 rate);
        extern int SDL_getFramerate(FPSmanager * manager);
        extern int SDL_getFramecount(FPSmanager * manager);
        extern Uint32 SDL_framerateDelay(FPSmanager * manager);

        void GUI_RunWithManager();
    }
}

#ifdef _OF_EVENTS_
#include <set>
#include "ofEvent.h"

#define  ofVec2f  guiVec2f

bool ofGetMousePressed(int button = -1); //by default any button
bool ofGetKeyPressed(int key = -1); //by default any key

int	ofGetMouseX();
int	ofGetMouseY();

int	ofGetPreviousMouseX();
int	ofGetPreviousMouseY();

// MARK: Base
class ofEventArgs {
};

// MARK: KeyEvent
class ofKeyEventArgs : public ofEventArgs {
public:
    enum Type {
        Pressed,
        Released,
    };

    ofKeyEventArgs()
        :type(Pressed)
        , key(0)
        , keycode(0)
        , scancode(0)
        , codepoint(0) {
    }

    ofKeyEventArgs(Type type, int key, int keycode, int scancode, unsigned int codepoint)
        :type(type)
        , key(key)
        , keycode(keycode)
        , scancode(scancode)
        , codepoint(codepoint) {

    }

    ofKeyEventArgs(Type type, int key)
        :type(type)
        , key(key)
        , keycode(0)
        , scancode(0)
        , codepoint(0) {

    }

    Type type;
    /// \brief For special keys, one of OF_KEY_* (@see ofConstants.h). For all other keys, the Unicode code point you'd expect if this key combo (including modifier keys that may be down) was pressed in a text editor (same as codepoint).
    int key;
    /// \brief The keycode returned by the windowing system, independent of any modifier keys or keyboard layout settings. For ofAppGLFWWindow this value is one of GLFW_KEY_* (@see glfw3.h) - typically, ASCII representation of the symbol on the physical key, so A key always returns 0x41 even if shift, alt, ctrl are down.
    int keycode;
    /// \brief The raw scan code returned by the keyboard, OS and hardware specific.
    int scancode;
    /// \brief The Unicode code point you'd expect if this key combo (including modifier keys) was pressed in a text editor, or -1 for non-printable characters.
    unsigned int codepoint;
};

// MARK: MouseEvent
class ofMouseEventArgs : public ofEventArgs, public ofVec2f {
public:
    enum Type {
        Pressed,
        Moved,
        Released,
        Dragged,
        Scrolled,
        Entered,
        Exited
    };

    ofMouseEventArgs()
        :type(Pressed)
        , button(OF_MOUSE_BUTTON_LEFT)
        , scrollX(0.f)
        , scrollY(0.f) {
    }

    ofMouseEventArgs(Type type, float x, float y, int button)
        :ofVec2f(x, y)
        , type(type)
        , button(button)
        , scrollX(0.f)
        , scrollY(0.f) {
    }

    ofMouseEventArgs(Type type, float x, float y)
        :ofVec2f(x, y)
        , type(type)
        , button(0)
        , scrollX(0.f)
        , scrollY(0.f) {
    }

    Type type;
    int button;
    float scrollX;
    float scrollY;
};

class ofTouchEventArgs : public ofEventArgs, public ofVec2f {
public:
    enum Type {
        down,
        up,
        move,
        doubleTap,
        cancel
    };

    ofTouchEventArgs()
        :type(down)
        , id(0)
        , time(0)
        , numTouches(0)
        , width(0)
        , height(0)
        , angle(0)
        , minoraxis(0)
        , majoraxis(0)
        , pressure(0)
        , xspeed(0)
        , yspeed(0)
        , xaccel(0)
        , yaccel(0) {

    }

    ofTouchEventArgs(Type type, float x, float y, int id)
        :ofVec2f(x, y)
        , type(type)
        , id(id)
        , time(0)
        , numTouches(0)
        , width(0)
        , height(0)
        , angle(0)
        , minoraxis(0)
        , majoraxis(0)
        , pressure(0)
        , xspeed(0)
        , yspeed(0)
        , xaccel(0)
        , yaccel(0) {
    }

    Type type;
    int id;
    int time;
    int numTouches;
    float width, height;
    float angle;
    float minoraxis, majoraxis;
    float pressure;
    float xspeed, yspeed;
    float xaccel, yaccel;
};

class ofResizeEventArgs : public ofEventArgs {
public:
    ofResizeEventArgs()
        :width(0)
        , height(0) {
    }

    ofResizeEventArgs(int width, int height)
        :width(width)
        , height(height) {
    }

    int width;
    int height;
};

class ofMessage : public ofEventArgs {
public:
    ofMessage(string msg) {
        message = msg;
    }
    string message;
};

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

    void disable();
    void enable();

    void setFrameRate(int _targetRate);
    float getFrameRate() const;
    float getTargetFrameRate() const;
    double getLastFrameTime() const;
    uint64_t getFrameNum() const;

    bool getMousePressed(int button = -1) const;
    bool getKeyPressed(int key = -1) const;
    int getMouseX() const;
    int getMouseY() const;
    int getPreviousMouseX() const;
    int getPreviousMouseY() const;

    //  event notification only for internal OF use
    void notifySetup();
    void notifyUpdate();
    void notifyDraw();

    void notifyKeyPressed(int key, int keycode = -1, int scancode = -1, int codepoint = -1);
    void notifyKeyReleased(int key, int keycode = -1, int scancode = -1, int codepoint = -1);
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

private:
    float targetRate;
    bool bFrameRateSet;

    int	currentMouseX, currentMouseY;
    int	previousMouseX, previousMouseY;
    bool bPreMouseNotSet;
    set<int> pressedMouseButtons;
    set<int> pressedKeys;
};

void ofSendMessage(ofMessage msg);
void ofSendMessage(string messageString);

ofCoreEvents & ofEvents();

template<class ListenerClass>
void ofRegisterMouseEvents(ListenerClass * listener, int prio = OF_EVENT_ORDER_AFTER_APP) {
    ofAddListener(ofEvents().mouseDragged, listener, &ListenerClass::mouseDragged, prio);
    ofAddListener(ofEvents().mouseMoved, listener, &ListenerClass::mouseMoved, prio);
    ofAddListener(ofEvents().mousePressed, listener, &ListenerClass::mousePressed, prio);
    ofAddListener(ofEvents().mouseReleased, listener, &ListenerClass::mouseReleased, prio);
    ofAddListener(ofEvents().mouseScrolled, listener, &ListenerClass::mouseScrolled, prio);
}

template<class ListenerClass>
void ofRegisterKeyEvents(ListenerClass * listener, int prio = OF_EVENT_ORDER_AFTER_APP) {
    ofAddListener(ofEvents().keyPressed, listener, &ListenerClass::keyPressed, prio);
    ofAddListener(ofEvents().keyReleased, listener, &ListenerClass::keyReleased, prio);
}

template<class ListenerClass>
void ofRegisterTouchEvents(ListenerClass * listener, int prio = OF_EVENT_ORDER_AFTER_APP) {
    ofAddListener(ofEvents().touchDoubleTap, listener, &ListenerClass::touchDoubleTap, prio);
    ofAddListener(ofEvents().touchDown, listener, &ListenerClass::touchDown, prio);
    ofAddListener(ofEvents().touchMoved, listener, &ListenerClass::touchMoved, prio);
    ofAddListener(ofEvents().touchUp, listener, &ListenerClass::touchUp, prio);
    ofAddListener(ofEvents().touchCancelled, listener, &ListenerClass::touchCancelled, prio);
}

template<class ListenerClass>
void ofRegisterGetMessages(ListenerClass * listener, int prio = OF_EVENT_ORDER_AFTER_APP) {
}

template<class ListenerClass>
void ofUnregisterMouseEvents(ListenerClass * listener, int prio = OF_EVENT_ORDER_AFTER_APP) {
    ofRemoveListener(ofEvents().mouseDragged, listener, &ListenerClass::mouseDragged, prio);
    ofRemoveListener(ofEvents().mouseMoved, listener, &ListenerClass::mouseMoved, prio);
    ofRemoveListener(ofEvents().mousePressed, listener, &ListenerClass::mousePressed, prio);
    ofRemoveListener(ofEvents().mouseReleased, listener, &ListenerClass::mouseReleased, prio);
    ofRemoveListener(ofEvents().mouseScrolled, listener, &ListenerClass::mouseScrolled, prio);
}

template<class ListenerClass>
void ofUnregisterKeyEvents(ListenerClass * listener, int prio = OF_EVENT_ORDER_AFTER_APP) {
    ofRemoveListener(ofEvents().keyPressed, listener, &ListenerClass::keyPressed, prio);
    ofRemoveListener(ofEvents().keyReleased, listener, &ListenerClass::keyReleased, prio);
}

template<class ListenerClass>
void ofUnregisterTouchEvents(ListenerClass * listener, int prio = OF_EVENT_ORDER_AFTER_APP) {
    ofRemoveListener(ofEvents().touchDoubleTap, listener, &ListenerClass::touchDoubleTap, prio);
    ofRemoveListener(ofEvents().touchDown, listener, &ListenerClass::touchDown, prio);
    ofRemoveListener(ofEvents().touchMoved, listener, &ListenerClass::touchMoved, prio);
    ofRemoveListener(ofEvents().touchUp, listener, &ListenerClass::touchUp, prio);
    ofRemoveListener(ofEvents().touchCancelled, listener, &ListenerClass::touchCancelled, prio);
}

template<class ListenerClass>
void ofUnregisterGetMessages(ListenerClass * listener, int prio = OF_EVENT_ORDER_AFTER_APP) {
}

#endif
#endif // endif _OF_EVENTS_
#endif //SHAPES_SDL_GUI_H
