#ifndef __MOUSE_TEST_H__
#define __MOUSE_TEST_H__

#include "SkiaAppLayer.h"

namespace sk_ui {

class MouseTestLayer : public sk_ui::SkiaAppLayer {
public:
    MouseTestLayer( );
    virtual ~MouseTestLayer() override;

    void onResize( int w, int h, float scale ) override;
    void onPaint( SkCanvas& canvas ) override;
    bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    bool onMouseWheel(float deltax, float delta, skui::ModifierKey) override;
    bool onTouch(intptr_t owner, skui::InputState, float x, float y) override;

private:
    float touchX;
    float touchY;
    int mouseX;
    int mouseY;
};

}

#endif //__MOUSE_TEST_H__