#ifndef __DEC_WIDGET_H__
#define __DEC_WIDGET_H__

#include "DecFont.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "tools/skui/InputState.h"
#include "tools/skui/Key.h"
#include "tools/skui/ModifierKey.h"
#include <string>
#include <vector>
#include <functional>
#include <chrono>
//using namespace std::chrono_literals;

class DecUI;
class DecJob;

class DecStackContainer;

class DecWidget {
public:
    DecWidget(const char* _name = NULL);
    virtual ~DecWidget();

    virtual std::string type() { return "Widget"; }

    DecWidget& x(int ix);
    DecWidget& y(int iy);
    DecWidget& width(int w);
    DecWidget& height(int h);
    DecWidget& size(int w, int h);
    DecWidget& bounds(int ix, int iy, int w, int h);

    DecWidget& align(int a);

    DecWidget& font(DecFont& font);
    DecFont* font() { return _font; }
    virtual void setFont(DecFont* f);

    void adjustX(int ix);
    void adjustY(int iy);
    virtual void adjustWidth(int w);
    virtual void adjustHeight(int h);

    void moveX(int ix);
    void moveY(int iy);
    void move(int ix, int iy);

    DecWidget& onDraw(void (*draw_func)(DecWidget& widget, SkCanvas& canvas));

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey);
    virtual bool onMouseWheel(int x, int y, float deltax, float delta, skui::ModifierKey);
    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod);
    virtual bool onChar(SkUnichar c, skui::ModifierKey mod);
    virtual bool onDropFile(int ix, int iy, const char* dropped_file, uintptr_t /* uint8_t*  */ iptr = 0, size_t length = 0);
    virtual void onResized(int w, int h);

    virtual void fireOnResized();

    virtual void bubbleAction(int actionID, DecWidget* sender, void *data=NULL);
    virtual bool cascadeAction(int actionID, DecWidget* sender, void *data=NULL);

    virtual bool hitTest(int x, int y);
    virtual bool hitTestAbs(int x, int y);

    virtual DecWidget& _(DecWidget& c, bool front = false) { return child(c, front); }
    virtual DecWidget& child(DecWidget& c, bool front = false);
    void removeChild( DecWidget *c );

    inline DecWidget& ref(DecWidget** ptr) {
        *ptr = this;
        return *this;
    }

    virtual void init();
    inline int x() { return _x; }
    inline int y() { return _y; }
    inline int width() { return _width; }
    inline int height() { return _height; }

    inline int tag() { return _tag; }
    inline DecWidget& tag(int t) {
        _tag = t;
        return *this;
    }

    inline virtual float value() { return _value; }
    inline virtual DecWidget& value(float v) {
        _value = v;
        return *this;
    }

    inline virtual const char *text() { return _text.c_str(); }
    inline virtual DecWidget& text( const char *t) {
        _text = std::string( t );
        dirty( true, true );
        return *this;
    }
    virtual DecWidget &fontSize( const int s );
    inline virtual int fontSize() { return _textSize; }

    inline bool isSelected() { return _isSelected; }
    inline virtual DecWidget& select(bool i) {
        _isSelected = i;
        return *this;
    }

    virtual void setCursor(const std::string cursorStyle = "");
    inline virtual DecWidget& cursor(const std::string cursorStyle = "") {
        this->setCursor(cursorStyle);
        return *this;
    }

    DecWidget& max_width(int mw);
    DecWidget& max_height(int mw);

    DecWidget& margin(int t, int r, int b, int l);
    DecWidget& margin_top(int t);
    DecWidget& margin_right(int r);
    DecWidget& margin_bottom(int b);
    DecWidget& margin_left(int l);

    DecWidget& padding(int t, int r, int b, int l);
    DecWidget& padding_top(int t);
    DecWidget& padding_right(int r);
    DecWidget& padding_bottom(int b);
    DecWidget& padding_left(int l);

    DecWidget& gap(int g);
    DecWidget& round(int r);

    virtual int contentX();
    virtual int contentY();
    virtual int contentWidth();
    virtual int contentHeight();

    virtual void update();
    virtual bool updateUI( int level );
    virtual void updateAbsPosn();

    virtual void pre_draw(SkCanvas& canvas);
    virtual void draw(SkCanvas& canvas, int level=0);
    virtual void render(SkCanvas& canvas);
    virtual void post_draw(SkCanvas& canvas);

    virtual DecWidget& active(bool a);
    virtual DecWidget& visible(bool v);
    virtual void focus(bool f);
    virtual void mouseCapture(bool c);
    virtual void mouseMoveWatchHorizontal(bool c);
    virtual void mouseMoveWatchVertical(bool c);
    virtual DecWidget& subWidget(bool s);
    DecWidget& draggableX(bool d);
    DecWidget& draggableY(bool d);
    DecWidget& draggable(bool d);
    DecWidget& show() { visible(true); return *this; }
    DecWidget& hide() { visible(false); return *this; }
    virtual DecWidget& hideOnWeb();
    DecWidget& hideOnFlag( bool flag );
    DecWidget& focusable(bool b) { _isFocusable = b; return *this; }
    DecWidget& enableKeyboard( bool b ) { _isEnableKeyboard = b; return *this; }

    virtual DecWidget& cache(bool b);
    inline bool cache() { return _isCache; }

    virtual void dirty(bool b = true, bool updateLayout=false );
    virtual void invalidate();

    //virtual void dirtyChildren(bool b = true, bool updateLayout=true );

    virtual inline bool visible() { return _isVisible; }

    inline virtual DecWidget& background(bool b) {
        _background = b;
        dirty( true, false );
        return *this;
    }

    inline virtual DecWidget& text_color(uint32_t c) {
        _text_color = c;
        dirty( true, false );
        return *this;
    }

    inline virtual DecWidget& bg_color(uint32_t c) {
        _bg_color = c;
        dirty( true, false );
        return *this;
    }

    inline virtual DecWidget& border(bool b) {
        _border = b;
        dirty( true, false );
        return *this;
    }
    inline virtual DecWidget& border_color(uint32_t c) {
        _border_color = c;
        dirty( true, false );
        return *this;
    }
    inline DecWidget& borderWidth(int w) {
        _borderWidth = w;
        dirty( true, false );
        return *this;
    }

    inline DecWidget& externalData( void *data ) {
        _externalData = data;
        return *this;
    }

    inline DecWidget& externalWidget( DecWidget *widget ) {
        _externalWidget = widget;
        return *this;
    }    

    void setUI(DecUI* ui);
    void setParent(DecWidget* p);
    void moveModal(DecStackContainer *modalParent);

    DecWidget& onMousePressed(std::function<bool(DecWidget& widget)> callback);
    DecWidget& onMouseReleased(std::function<bool(DecWidget& widget)> callback);
    DecWidget& onValueChanged(std::function<void(DecWidget& widget)> callback);

    void pushJob( DecJob *job, std::function<void(DecWidget& widget)>onFinishCallback=nullptr );
    std::vector<DecJob *>_jobStack;
    bool _isJobActive;
    std::function<void(DecWidget& widget)>_jobOnFinishCallback;

    void setJobFrameSkip( int n ) { _jobFrameSkip = n; }
    int _jobFrameSkip;
    int _jobFrameCount;

    uint32_t id;
    std::string name;

    void setTimeOut( double milliseconds, std::function<void(DecWidget &widget)>callback );

    bool _isTimeoutActive;
    double _timeoutPeriod;
    std::chrono::high_resolution_clock::time_point _beginTimeout;
    std::function<void(DecWidget &widget)>_timeoutCallback;

    std::string ID() { return _id; }
    DecWidget& ID( std::string new_id ) { _id = new_id; return *this; }

    std::string _id;

    int _ox;
    int _oy;
    int _ow;
    int _oh;

    int _ax;
    int _ay;

    int __w;
    int __h;

    int _index;
    int _flag;

    int _maxWidth;
    int _maxHeight;

    int _textSize;

    int _margin[4];
    int _padding[4];

    int _drawLevel;

    bool _isActive;
    bool _isFocus;
    bool _isFocusable;
    bool _isVisible;
    bool _isMouseCapture;
    bool _isMouseMoveWatcherHorizontal;
    bool _isMouseMoveWatcherVertical;
    bool _isDraggableX;
    bool _isDraggableY;
    bool _isDragging;
    bool _isLimitPosn;
    bool _isSubWidget;
    bool _isPressed;
    bool _isSelected;
    bool _isCache;
    bool _isDirty;
    bool _isEnableKeyboard;
    int _isDirtyLayout;
    bool _isModal;

    int _minX;
    int _maxX;
    int _minY;
    int _maxY;

    static DecWidget* _activeWidget;
    static DecWidget* _focusWidget;
    static DecWidget* _mouseCaptureWidget;
    static DecWidget* _mouseMoveWatcherHorizontal;
    static DecWidget* _mouseMoveWatcherVertical;

    virtual void _updateAlignmentX();
    virtual void _updateAlignmentY();

    virtual void _endDrag() {}
    DecWidget* _parent;
    DecWidget* _logicalParent;
    std::vector<DecWidget*> _children;

    virtual void capture(const char* filename, bool border = true, bool children = true);

    void* _externalData;
    DecWidget *_externalWidget;

    sk_sp<SkSurface> _cacheSurface;
    sk_sp<SkImage> _cacheImage;

    std::function<bool(DecWidget& widget)> _onMousePressed;
    std::function<bool(DecWidget& widget)> _onMouseReleased;
    std::function<void(DecWidget& widget)> _onValueChanged;

    bool _border;
    int _borderWidth;
    uint32_t _border_color;
    int _focus_borderWidth;
    uint32_t _focus_border_color;

    std::string _text;
    inline DecUI *UI() { return _ui; }

    virtual void js_callback( std::string message ) {}
protected:
    DecUI* _ui;
    int _tag;

    float _value;

    bool _captureNextFrame;
    bool _captureBorder;
    bool _captureChildren;
    std::string _captureFilename;

    int _x;
    int _y;
    int _width;
    int _height;

    int _minWidth;
    int _minHeight;

    int _testDraw;

    DecFont* _font;
    DecFont* _ofont;

    int _lastMouseX;
    int _lastMouseY;


    bool _background;
    uint32_t _bg_color;
    uint32_t _bg_color_pressed;

    uint32_t _text_color;

    int _gap;

    int _align;

    int _round;

    void (*_draw_func)(DecWidget& widget, SkCanvas& canvas);

    virtual void _setGap(int g);

    virtual void _draw_bg(SkCanvas& canvas);
    virtual void _draw_border(SkCanvas& canvas);
    virtual void _draw_content(SkCanvas& canvas);
    virtual void _draw_children(SkCanvas& canvas);

    virtual void _preUpdateUI();
    virtual void _updateChildrenUI( int level );
    virtual void _postUpdateUI();

    virtual void _updateChildrenAlignment();
};

DecWidget& Widget(const char* _name = NULL);

#endif  //__DEC_WIDGET_H__