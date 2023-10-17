#ifndef __DEC_UI_H__
#define __DEC_UI_H__

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "tools/skui/ModifierKey.h"
#include "tools/skui/InputState.h"
#include "modules/skparagraph/include/FontCollection.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/src/ParagraphBuilderImpl.h"
#include "modules/skparagraph/include/TypefaceFontProvider.h"
#include "tools/skui/Key.h"
#include "DecWidget.h"
#include "DecStackContainer.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

const int ALIGN_ABSOLUTE        = 0;
const int _ALIGN_TOP            = 1;
const int _ALIGN_RIGHT          = 2;
const int _ALIGN_BOTTOM         = 4;
const int _ALIGN_LEFT           = 8;
const int _ALIGN_CENTER         = 16;
const int _ALIGN_VCENTER        = 32;
const int ALIGN_TOPLEFT         = _ALIGN_TOP + _ALIGN_LEFT;
const int ALIGN_TOPCENTER       = _ALIGN_TOP + _ALIGN_CENTER;
const int ALIGN_TOPRIGHT        = _ALIGN_TOP + _ALIGN_RIGHT;
const int ALIGN_VCENTERLEFT     = _ALIGN_VCENTER + _ALIGN_LEFT;
const int ALIGN_CENTER          = _ALIGN_VCENTER + _ALIGN_CENTER;
const int ALIGN_VCENTERRIGHT    = _ALIGN_VCENTER + _ALIGN_RIGHT;
const int ALIGN_BOTTOMLEFT      = _ALIGN_BOTTOM + _ALIGN_LEFT;
const int ALIGN_BOTTOMCENTER    = _ALIGN_BOTTOM + _ALIGN_CENTER;
const int ALIGN_BOTTOMRIGHT     = _ALIGN_BOTTOM + _ALIGN_RIGHT;

const int SCROLL_NONE           = 0;
const int SCROLL_HORIZONTAL     = 1;
const int SCROLL_VERTICAL       = 2;
const int SCROLL_BOTH           = 3;

const int ACTION_LISTITEM_SELECTED          = 1;
const int ACTION_LIST_SELECT_CHANGED        = 2;
const int ACTION_TREEITEM_SELECTED          = 3;
const int ACTION_TREE_SELECT_CHANGED        = 4;
const int ACTION_COMBOBOXITEM_SELECTED      = 5;
const int ACTION_COMBOBOX_SELECT_CHANGED    = 6;
const int ACTION_FILELIST_SELECT_CHANGED    = 7;

extern std::vector<std::string>__DecArguments;

class DecUI {
public:
    DecUI();
    virtual ~DecUI();

    virtual void focus(bool f);

    DecUI &setRoot( DecWidget *_root );
    void onResize( int w, int h, float scale );
    void onUpdate();

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey);
    virtual bool onMouseWheel(float deltax, float delta, skui::ModifierKey);

    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod);
    virtual bool onChar(SkUnichar c, skui::ModifierKey mod);
    virtual bool onDropFile(const char* dropped_file, uintptr_t /* uint8_t*  */ iptr = 0, size_t length = 0);

    inline int width() { return _width / _scale; }
    inline int height() { return _height / _scale; }
    //inline float scale() { return _scale; }  
    
    DecUI &font( DecFont &font );
    DecFont *font() { return _font; }
    virtual void setFont( DecFont *f );  

    DecUI &root( DecWidget &w );    

    void invalidate();

    void draw(SkCanvas &canvas);    

    int _width;
    int _height;
    float _scale;     

    int _lastMouseX;
    int _lastMouseY;  

    DecWidget *userRootWidget;   
    DecStackContainer *rootWidget;   
    DecStackContainer *modalWidget;

    sk_sp<skia::textlayout::FontCollection>fontCollection;
private:

    DecFont *_font;
};

DecUI &createUI();

sk_sp<SkSurface> DecMakeRasterSurface(int w, int h);

#ifdef __EMSCRIPTEN__
void emscriptenDownloadAsset( void *widget, 
    std::string url, 
    void (*onsuccess)(struct emscripten_fetch_t *fetch),
    void (*onerror)(struct emscripten_fetch_t *fetch) );
#endif  

#endif //__DEC_UI_H__