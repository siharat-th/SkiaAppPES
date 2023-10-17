#ifndef __HELLO_WORLD_LAYER_H__
#define __HELLO_WORLD_LAYER_H__

#include "sk_ui.h"

class HelloWorldLayer : public sk_ui::SkiaAppLayer {
public:
    HelloWorldLayer( );
    virtual ~HelloWorldLayer() override;

    void onResize( int w, int h, float scale ) override;
    void onPaint( SkCanvas& canvas ) override;
    bool onChar(SkUnichar c, skui::ModifierKey mod) override;
    bool onKey(skui::Key, skui::InputState, skui::ModifierKey) override;
    bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    bool onMouseWheel(float deltax, float delta, skui::ModifierKey) override;
    bool onTouch(intptr_t owner, skui::InputState, float x, float y) override;

private:
    bool        fShowImGuiDemoWindow;
    bool        fPause;

    int         width;
    int         height;
    float       scale;

    SkScalar fRotationAngle;
};



#endif // __HELLO_WORLD_LAYER_H__
