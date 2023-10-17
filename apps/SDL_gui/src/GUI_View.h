//
// Created by Jimmy on 2/10/2016 AD.
//

#ifndef SHAPES_GUI_VIEW_H
#define SHAPES_GUI_VIEW_H
#include <SDL.h>
#include <SDL_ttf.h>
#include "GUI_Utils.h"
#include "GUI_Text.h"
#include <string>
#include <functional>
#include "LocalizedString.hpp"

#define GUI_ALIGN_LEFT        0
#define GUI_ALIGN_CENTER      2
#define GUI_ALIGN_RIGHT       4
#define GUI_ALIGN_TOP         0
#define GUI_ALIGN_VCENTER     16
#define GUI_ALIGN_BOTTOM      32
#define GUI_ALIGN_ABSOLUTE    64

#define GUI_LAYOUT_ABSOLUTE   0
#define GUI_LAYOUT_HORIZONTAL 2
#define GUI_LAYOUT_VERTICAL   4

class GUI_View {
protected:
    int ox, oy, ow, oh;

    int min_width;
    int min_height;

    int layout;

    bool    _hidden;
    bool    _disable;
    bool    _uiready;
    int     _border;

    
public:
    static GUI_View *createView( GUI_View *parent, const char *title, int x, int y, int width, int height );
    
    // pom
#if __USE_SKIA__
    sk_sp<SkImage> imageIcon, imageTitle;
#endif
    
    SDL_Texture *textureIcon;
    GUI_Rect icon_area;
    int icon_gap;
    int align;
    SDL_Color borderColor;

    std::string title;
    GUI_Rect tw_area;   // relative to top window
    GUI_Point topleft;  // relative to parent window

    SDL_Color bgcol;
    int corner;

    GUI_View **children;
    int lst_child, end_child;
    GUI_Rect clip_area; // clip rec

    bool    click_through;
    bool    dragable;
    bool    clicktotop;
    bool    bClose;

    int     tag;
    GUI_View *parent;

    bool    isFocus;
    bool    dragging;
    GUI_Point lastMousePoint;

    SDL_Texture *titleTexture;
    GUI_Rect title_area;
    SDL_Color titleColor;
    SDL_Color iconColor;
    int title_align;

    int padding[4];
    int margin[4];

    bool __handleInputEvent;

    static GUI_View *create(GUI_View *parent, const char *title, int x, int y, int width, int height);

    GUI_View(GUI_View *parent, const char *title, int x, int y, int width, int height, SDL_Color bgcol = cWhite,
             void(*disp_cmd)(GUI_View *) = NULL, bool(*event_cmd)(GUI_View *v, SDL_Event *ev) = NULL);

    virtual ~GUI_View();

    virtual void close();

    virtual void add_child(GUI_View* child);
    virtual void remove_child(GUI_View* child);
    virtual void remove_all_children();

    virtual bool handleEvents(SDL_Event *ev);
    virtual bool handleUserEvents(SDL_Event *ev);

    virtual void predraw();
    virtual void draw();
    virtual void postdraw();

    virtual void toTop();

    virtual void move(int dx, int dy);  // move win
    virtual void move_twarea(int dx, int dy);  // move win

    inline int getPhysicalWidth() {
        return tw_area.w;
    }

    inline int getPhysicalHeight() {
        return tw_area.h;
    }
    
    inline int getViewWidth() {
        return tw_area.w/GUI_scale;
    }
    
    inline int getViewHeight() {
        return tw_area.h/GUI_scale;
    }

    inline void setBorder(int b) {
        _border = b;
    }
    
    inline void setBackgroundColor(SDL_Color color) {
        bgcol = color;
    }


    virtual void clear(GUI_Rect *rect = 0);

    virtual GUI_View *hitTest(int x, int y, bool bRecursive = true);
    
#if __USE_SKIA__
    void createTitleImage(int fontSize = GUI_FONT_TITLE);
    void setIcon(sk_sp<SkImage> image);
#endif

    void createTitleTexture(int fontSize = GUI_FONT_TITLE);

    void setIcon(SDL_Texture *tex, bool bshared_ptr = false);
    void centerToParent();
    void setPosition(int x, int y);
    SDL_Point getPosition();
    SDL_Rect getRect();

    void setLayout(int l);
    void setAlignment(int a);

    virtual void setTitle(std::string t, int fontSize = GUI_FONT_TITLE);

    void updateLayout();
    virtual void updateSubLayout();

    virtual void setPadding(int p0, int p1, int p2, int p3);
    virtual void setMargin(int p0, int p1, int p2, int p3);

    void setEventHandler(bool(*event_cmd)(GUI_View *v, SDL_Event *ev));

    virtual void show() {
        _hidden = false; _uiready = false;
    }

    virtual void hide() {
        _hidden = true; _uiready = false;
        isFocus = false;
    }

    virtual void enable() {
        _disable = false;
    }

    virtual void disable() {
        _disable = true; textSelectionCancel(); isFocus = false;
    }

    virtual bool isEnable() {
        return !_disable;
    }

    virtual bool isVisible() {
        return !_hidden;
    }

    virtual bool isUIReady() {
        return _uiready;
    }

    virtual void resize(int w, int h) {
        tw_area.w = w * GUI_scale; tw_area.h = h * GUI_scale;
    }

    // Tooh - text selection
    virtual bool isActive() {
        return (!_disable && !_hidden && isFocus);
    }

    // pom
    std::function<void(GUI_View*)> display_cmd;
    std::function<bool(GUI_View*, SDL_Event*)> handle_event_cmd;
    std::function<void(GUI_View*)> onClose;

    // pom
    GUI_Rect getAbsoluteArea(); // local area(absolute non-scale)
    void pushState();
    void popState();
    void disableInputEvent();
    void enableInputEvent();
    bool __shared_ptr_textureIcon;

    // Tooh - text selection
    int textSelectionScrollIndex;
    int textSelectionStartIndex;
    int textSelectionEndIndex;
    int textSelectionFontSize;
    bool textSelectionIsSelected();
    void textSelectionGetSelectedIndex(int* startIndex, int* endIndex);
    virtual void textSelectionCancel();
    
    // pom
    inline void setToolTip(const std::string & tt){
        tooltip = LSTR(tt);
    }
    
    inline std::string getToolTip() const{
        return tooltip;
    }
    
    inline bool hasToolTip(){
        return (tooltip.empty()==false);
    }

private:
    bool ___hidden, __uiready, __disable;
    std::string tooltip;
};

extern GUI_View * GUI_mouseCapturedWindow;
extern GUI_View * GUI_focusWindow;
#endif //SHAPES_GUI_VIEW_H
