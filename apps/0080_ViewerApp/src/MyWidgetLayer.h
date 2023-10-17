#ifndef __MY_WIDGET_LAYER_H__
#define __MY_WIDGET_LAYER_H__

#include "sk_ui.h"
#include "pesCanvas.hpp"

namespace sk_ui {

class MyWidgetLayer : public WidgetLayer {
public:
    MyWidgetLayer( const char *src=NULL );
    virtual ~MyWidgetLayer() override;
    virtual void onUpdate() override;
    virtual void onPaint(SkCanvas &canvas) override;
    void onResize( int w, int h, float scale ) override;
    
    bool onMouse(int x, int y, skui::InputState input, skui::ModifierKey key) override;
    bool onMouseWheel(float delta, skui::ModifierKey key) override;
    bool onChar(SkUnichar c, skui::ModifierKey modifiers) override;
    
    int width, height;
    float scale;
    pesCanvas pCanvas;
};

}

#endif
