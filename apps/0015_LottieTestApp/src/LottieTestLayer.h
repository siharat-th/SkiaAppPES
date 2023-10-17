#ifndef __LOTTEI_TEST_LAYER_H__
#define __LOTTEI_TEST_LAYER_H__

#include "sk_ui.h"
#include "tools/viewer/AnimTimer.h"
#include "modules/skottie/include/Skottie.h"
#include "modules/skottie/utils/SkottieUtils.h"
#include "include/pathops/SkPathOps.h"

class LottieTestLayer : public sk_ui::SkiaAppLayer {
public:
    LottieTestLayer( );
    virtual ~LottieTestLayer() override;

    SkOpBuilder builder;

    void onResize( int w, int h, float scale ) override;
    void onPaint( SkCanvas& canvas ) override;
    bool onChar(SkUnichar c, skui::ModifierKey mod) override;
    bool onKey(skui::Key, skui::InputState, skui::ModifierKey) override;
    bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    bool onMouseWheel(float deltax, float delta, skui::ModifierKey) override;
    bool onTouch(intptr_t owner, skui::InputState, float x, float y) override;

private:
    bool        fPause;

    int         width;
    int         height;
    float       scale;

    //std::unique_ptr<skottie_utils::CustomPropertyManager>      fPropManager;
    sk_sp<skottie::Animation> fAnimation;
    AnimTimer              fAnimTimer;    
};



#endif // __HELLO_WORLD_LAYER_H__
