#ifndef __SKIA_APP_LAYER_H__
#define __SKIA_APP_LAYER_H__

#include "SkiaApp.h"

namespace sk_ui {

class SkiaAppLayer {
public:
    SkiaAppLayer() {
        fActive = true;
        fVisible = true;
        fScaleFactor = 1.0;
        fCurrentPage = -1;
    }
    virtual ~SkiaAppLayer() = default;

    bool getActive() { return fActive; }
    void setActive(bool active) { fActive = active; }
    virtual void focus(bool f) {}

    // return value of 'true' means 'I have handled this event'
    virtual void onInit() {}
    virtual void onResize( int w, int h, float scale=1.0 ) {fScaleFactor=scale;}
    virtual void onUpdate() {}
    virtual void onPrePaint(SkCanvas &canvas) {}
    virtual void onPaint(SkCanvas &canvas) {}
    virtual void onPostPaint(SkCanvas &canvas) {}
    virtual bool onChar(SkUnichar c, skui::ModifierKey) { return false; }
    virtual bool onKey(skui::Key, skui::InputState, skui::ModifierKey) { return false; }
    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) { return false; }
    virtual bool onMouseWheel(float deltax, float delta, skui::ModifierKey) { return false; }
    virtual bool onTouch(intptr_t owner, skui::InputState, float x, float y) { return false; }
    virtual bool onDropFile(const char* dropped_file, uintptr_t /* uint8_t*  */ iptr = 0, size_t length = 0) { return false; }
    virtual void onFontChange() {}

    // Platform-detected gesture events
    virtual bool onFling(skui::InputState state) { return false; }
    virtual bool onPinch(skui::InputState state, float scale, float x, float y) { return false; }
    virtual void onUIStateChanged(const SkString& stateName, const SkString& stateValue) {}

    bool fActive;
    float fScaleFactor;
    bool fVisible;

    int fCurrentPage;

private:
};

}

#endif