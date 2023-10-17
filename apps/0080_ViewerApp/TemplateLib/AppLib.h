#ifndef __TEMPLATE_APPLIB_H__
#define __TEMPLATE_APPLIB_H__

#include "sk_ui.h"

namespace sk_ui {

class TemplateAppLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    TemplateAppLib( );
    virtual ~TemplateAppLib() override;

    static TemplateAppLib *getInstance();    

    void onPaint( SkCanvas& canvas ) override;


#ifdef __EMSCRIPTEN__
    void onAppResize(int w, int h, float s) { SkiaAppLib::onAppResize( w, h, s); }
    void onAppPrePaint() { SkiaAppLib::onAppPrePaint(); }
    void onAppPaint( SkCanvas& canvas ) { SkiaAppLib::onAppPaint( canvas ); }
    bool onAppChar(SkUnichar c, skui::ModifierKey mod) { return SkiaAppLib::onAppChar( c, mod ); }
    bool onAppKey(skui::Key key, skui::InputState state, skui::ModifierKey modifiers) { return SkiaAppLib::onAppKey(key, state, modifiers); }
    bool onAppMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers) { return SkiaAppLib::onAppMouse(x, y, state, modifiers); }
    bool onAppMouseWheel(float delta, skui::ModifierKey modifiers) { return SkiaAppLib::onAppMouseWheel( delta, modifiers ); }
    void setImage(long instance, sk_sp<SkImage> img) { SkiaAppLib::setImage( instance, img ); }
#endif
private:
};

}

#endif // __TEMPLATE_APPLIB_H__
