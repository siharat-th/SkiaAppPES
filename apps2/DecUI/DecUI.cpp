#include "include/core/SkSurface.h"
#include "tools/Resources.h"
#include "modules/skparagraph/include/FontCollection.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/src/ParagraphBuilderImpl.h"
#include "modules/skparagraph/include/TypefaceFontProvider.h"
#include "DecUI.h"

static std::vector<DecUI *>ui_storage;


DecUI &createUI() {
    DecUI *_ui = new DecUI();
    ui_storage.push_back( _ui );
    return *_ui; 
} 

DecUI::DecUI() {
    userRootWidget = NULL;
    rootWidget = NULL;
    modalWidget = NULL;

    _lastMouseX = -1;
    _lastMouseY = -1;

    rootWidget = new DecStackContainer();
    rootWidget->ID( "ROOT" );
    rootWidget->size( -1, -1 );
    rootWidget->font( *_font );
    rootWidget->setUI(this);    
    rootWidget->border( false );

    modalWidget = new DecStackContainer();
    modalWidget->size( -1, -1 );
    modalWidget->border( false );
    modalWidget->bg_color( 0x80000000 );
    modalWidget->visible( false );
    rootWidget->_( *modalWidget );
    
    /*
    std::string fonts = GetResourcePath("fonts/").c_str();
    std::set<std::pair<std::string, std::string>> font_paths = {
            //{"Roboto", "Roboto-Regular.ttf"},
            //{"Roboto", "Roboto-Bold.ttf"},
            {"Kanit", "Kanit-Regular.ttf"},
            {"Kanit", "Kanit-Bold.ttf"},
            {"Emoji", "NotoColorEmoji.ttf"},
    };

    sk_sp<skia::textlayout::TypefaceFontProvider> font_provider =
            sk_make_sp<skia::textlayout::TypefaceFontProvider>();

    for (auto& pair : font_paths) {
        SkString family_name = SkString(pair.first.c_str());
        std::string path = fonts;
        path += pair.second;

        auto data = SkData::MakeFromFileName(path.c_str());
        font_provider->registerTypeface(SkTypeface::MakeFromData(std::move(data)), family_name);
    }

    fontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    fontCollection->setAssetFontManager(std::move(font_provider));
    fontCollection->setDefaultFontManager(SkFontMgr::RefDefault());
    */
}

DecUI::~DecUI() {

}

void DecUI::focus(bool f) {
    //SkDebugf("DecUI::focus:%d\n", f);
    if (DecWidget::_focusWidget) {
        DecWidget::_focusWidget->focus(f);
    }
}

DecUI &DecUI::root( DecWidget &w ) {
    setRoot(&w);
    w.border(false);
    onResize( _width, _height, _scale );   

    if( rootWidget ) {
        rootWidget->init();
    }

    return *this; 
}

void DecUI::onResize( int w, int h, float scale ) {
    _width = w;
    _height = h;
    _scale = scale;
    invalidate();
    if( rootWidget ) {
        rootWidget->update();   
        rootWidget->updateUI( 0 );
        rootWidget->updateAbsPosn();
        rootWidget->fireOnResized();
#ifdef __EMSCRIPTEN__
        if (rootWidget->_isDirty || rootWidget->_isDirtyLayout) {
            EM_ASM({ globalThis['isDirty'] = true; });
        }
#endif
    }    
}

void DecUI::onUpdate() {
    if (rootWidget) {
        rootWidget->update();
        rootWidget->updateUI(0);
        rootWidget->updateAbsPosn();
        //SkDebugf("onUpdate _isDirty: %i\n", rootWidget->_isDirty);
#ifdef __EMSCRIPTEN__
        if (rootWidget->_isDirty || rootWidget->_isDirtyLayout) {
            EM_ASM({ globalThis['isDirty'] = true; });
        }
#endif
    }
}

bool DecUI::onMouseWheel(float deltax, float delta, skui::ModifierKey modifiers) {
    if( rootWidget && rootWidget->hitTest( _lastMouseX, _lastMouseY ) ) {
        deltax /= 8.0f;
        delta /= 8.0f;
        rootWidget->onMouseWheel( _lastMouseX - rootWidget->x(), _lastMouseY - rootWidget->y(), 
            deltax, delta, modifiers );
    }
    return false;    
}

bool DecUI::onMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers ) {
    _lastMouseX = x;
    _lastMouseY = y;
    //SkDebugf( "UI Mouse: %i %i\n", x, y );
    /*
    if( captureMouseWidget ) {
        captureMouseWidget->onMouse( x - captureMouseWidget->ax(), y - captureMouseWidget->ay(), state, modifiers );
    }
    else 
    */
    if( state == skui::InputState::kDown ) {
        if( DecWidget::_focusWidget ) {
            if( !DecWidget::_focusWidget->hitTestAbs( x, y ) ) {
                DecWidget::_focusWidget->focus( false );
            }
            else {
                //SkDebugf( "Mouse Abs\n" );
            }
        }
    }
    if( DecWidget::_mouseMoveWatcherHorizontal ) {
        DecWidget::_mouseMoveWatcherHorizontal->onMouse(
            x - DecWidget::_mouseMoveWatcherHorizontal->_ax, 
            y - DecWidget::_mouseMoveWatcherHorizontal->_ay, 
            state, modifiers 
        );
    }
    if( DecWidget::_mouseMoveWatcherVertical ) {
        DecWidget::_mouseMoveWatcherVertical->onMouse(
            x - DecWidget::_mouseMoveWatcherVertical->_ax, 
            y - DecWidget::_mouseMoveWatcherVertical->_ay, 
            state, modifiers 
        );
    }    

    if( DecWidget::_mouseCaptureWidget ) {
        DecWidget::_mouseCaptureWidget->onMouse( 
            x - DecWidget::_mouseCaptureWidget->_ax, 
            y - DecWidget::_mouseCaptureWidget->_ay, 
            state, modifiers );
        return true;
    }

    if( rootWidget && rootWidget->hitTest( x, y ) ) {
        rootWidget->onMouse( x - rootWidget->x(), y - rootWidget->y(), state, modifiers );
    }
    return false;
}

void DecUI::invalidate() {
    if( rootWidget ) {
        rootWidget->invalidate();
    }
}

void DecUI::draw(SkCanvas &canvas) {
    canvas.save();
    canvas.scale( _scale, _scale );    
    if( rootWidget ) {
//        rootWidget->update();
//        
//        rootWidget->updateUI( 0 );
//        rootWidget->updateAbsPosn();

        rootWidget->pre_draw( canvas );
        rootWidget->draw( canvas );
        rootWidget->post_draw( canvas );
    }    
    canvas.restore();
    //SkDebugf("draw _isDirty: %i\n", rootWidget->_isDirty);
}

DecUI &DecUI::setRoot( DecWidget *_root ) {


    userRootWidget = _root;
    userRootWidget->background(false);

    rootWidget->_( *userRootWidget, true );

    userRootWidget->moveModal( modalWidget );
    
    return *this;
}

bool DecUI::onKey(skui::Key k, skui::InputState state, skui::ModifierKey modifiers) {
    if( DecWidget::_focusWidget ) {
        DecWidget::_focusWidget->onKey( k, state, modifiers );
    }

    return false;
}

bool DecUI::onChar(SkUnichar c, skui::ModifierKey mod) {
    if (DecWidget::_focusWidget) {
        DecWidget::_focusWidget->onChar(c, mod);
    }

    return false;
}

bool DecUI::onDropFile(const char* dropped_file, uintptr_t /* uint8_t*  */ iptr, size_t length) { 
    if (rootWidget && rootWidget->hitTest(_lastMouseX, _lastMouseY)) {
        bool ret = rootWidget->onDropFile(_lastMouseX - rootWidget->x(), _lastMouseY - rootWidget->y(), dropped_file, iptr, length);
        SkDebugf("hitTest=true, onDropFile:%s, lastMouseXY:[%d,%d], ret: %d\n", dropped_file, _lastMouseX, _lastMouseY, ret);
        return ret;
    }
    SkDebugf("hitTest=false, onDropFile:%s, lastMouseXY:[%d,%d]\n", dropped_file, _lastMouseX, _lastMouseY);
    return false;
}

DecUI &DecUI::font( DecFont &font ) {
    _font = &font;

    sk_sp<skia::textlayout::TypefaceFontProvider> font_provider =
            sk_make_sp<skia::textlayout::TypefaceFontProvider>();

    font_provider->registerTypeface(font._typeface[FONT_REGULAR],SkString("Kanit"));
    font_provider->registerTypeface(font._typeface[FONT_BOLD],SkString("Kanit"));
    font_provider->registerTypeface(font._typeface[FONT_BOLDITALIC],SkString("Kanit"));
    font_provider->registerTypeface(font._typeface[FONT_ITALIC],SkString("Kanit"));
    font_provider->registerTypeface(font._typeface[FONT_ICON],SkString("Icon"));
    font_provider->registerTypeface(font._typeface[FONT_EMOJI],SkString("Emoji"));

    fontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    fontCollection->setAssetFontManager(std::move(font_provider));
    fontCollection->setDefaultFontManager(SkFontMgr::RefDefault());    

    if( rootWidget ) {
        rootWidget->font( *_font );
    }    
    return *this;
}

void DecUI::setFont( DecFont *f ) {
    _font = f;
}

void dec_release_direct_surface_storage(void* pixels, void* context){
    if (pixels == context) {
        //SkDebugf("expected release context\n");
    }
    sk_free(pixels);
}

sk_sp<SkSurface> DecMakeRasterSurface(int w, int h){
    if (w > 0 && h > 0) {
        SkImageInfo imageInfo = SkImageInfo::Make(
                w, h, SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kUnpremul_SkAlphaType);
        size_t pixelLen = w * h * 4;                 // // it's 8888, so 4 bytes per pixel
        void* pixelPtr = sk_malloc_throw(pixelLen);  // sk_malloc_canfail(pixelLen);
        if (pixelPtr) {
            sk_sp<SkSurface> surface(SkSurface::MakeRasterDirectReleaseProc(imageInfo, pixelPtr, w * 4, dec_release_direct_surface_storage, pixelPtr));
            return surface;
        }
    } else {
        // just-test-remove-later
        //SkDebugf("!! Who want to MakeRasterSurface size: [%d, %d]\n", w, h);
    }
    return nullptr;
}

#ifdef __EMSCRIPTEN__
void emscriptenDownloadAsset( void *widget, 
    std::string url, 
    void (*onsuccess)(struct emscripten_fetch_t *fetch),
    void (*onerror)(struct emscripten_fetch_t *fetch) ) {
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);

    strcpy(attr.requestMethod, "GET");

    //std::string full_url = std::string( "/resources/") + url;
    std::string full_url;
    if (url[0] == '/') {
        full_url = std::string(url);
    } else if (url.find("resources/") == 0) {
        //full_url = std::string("/") + url;
        full_url = std::string("") + url;
    } else {
        //full_url = std::string("/resources/") + url;
        full_url = GetResourcePath(url.c_str()).c_str();
    }
    //const SkString res_path = GetResourcePath(url.c_str());
    //SkDebugf("emscriptenDownloadAsset res_path:'%s'\n", res_path.c_str());
    //SkDebugf("emscriptenDownloadAsset url:'%s'\n", url.c_str());
    //SkDebugf("emscriptenDownloadAsset full_url:'%s'\n", full_url.c_str());
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.userData = (void *)widget;
    attr.onsuccess = onsuccess;
    attr.onerror = onerror;
    emscripten_fetch(&attr, full_url.c_str());
}
#endif