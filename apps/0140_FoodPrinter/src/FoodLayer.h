#ifndef __FOOD_PRINTER_LAYER_H__
#define __FOOD_PRINTER_LAYER_H__

#include "sk_ui.h"

class FoodLayer : public sk_ui::SkiaAppLayer {
public:
    FoodLayer( );
    virtual ~FoodLayer() override;

    void onResize( int w, int h, float scale ) override;
    void onPaint( SkCanvas& canvas ) override;
    bool onChar(SkUnichar c, skui::ModifierKey mod) override;
    bool onKey(skui::Key, skui::InputState, skui::ModifierKey) override;
    bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    bool onMouseWheel(float delta, skui::ModifierKey) override;
    bool onTouch(intptr_t owner, skui::InputState, float x, float y) override;

private:
    bool        fShowImGuiDemoWindow;
    bool        fPause;

    int         width;
    int         height;
    float       scale;

};



#endif // __FOOD_PRINTER_LAYER_H__
