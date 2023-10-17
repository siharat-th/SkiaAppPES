#ifndef __TEMPLATE_LAYER_H__
#define __TEMPLATE_LAYER_H__

#include "SkiaAppLayer.h"

namespace sk_ui {

class TemplateLayer : public sk_ui::SkiaAppLayer {
public:
    TemplateLayer( );
    virtual ~TemplateLayer() override;

    void onResize( int w, int h, float scale ) override;
    void onPaint( SkCanvas& canvas ) override;
    bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    bool onMouseWheel(float deltax, float delta, skui::ModifierKey) override;
    bool onTouch(intptr_t owner, skui::InputState, float x, float y) override;

private:
};

}

#endif //__TEMPLATE_LAYER_H__