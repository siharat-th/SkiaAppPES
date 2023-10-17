#ifndef __DEC_UI_LAYER_H__
#define __DEC_UI_LAYER_H__

#include "tools/skui/ModifierKey.h"
#include "tools/skui/InputState.h"
#include "tools/skui/Key.h"
#include "SkiaAppLayer.h"
#include "DecUI.h"
#include "DecWidget.h"

namespace sk_ui {

class DecUILayer : public sk_ui::SkiaAppLayer {
public:
    DecUILayer( );
    virtual ~DecUILayer() override;

    virtual void focus(bool f) override {
        //SkDebugf("DecUILayer::setActive:%d\n", f);
        if (_ui) {
            _ui->focus(f);
        }
    }

    virtual void onInit() override {}
    virtual void onResize(int w, int h, float scale) override;
    virtual void onUpdate() override;
    virtual void onPrePaint(SkCanvas& canvas) override;
    virtual void onPaint(SkCanvas& canvas) override;
    virtual void onPostPaint(SkCanvas& canvas) override;
    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    virtual bool onMouseWheel(float deltax, float delta, skui::ModifierKey) override;
    virtual bool onTouch(intptr_t owner, skui::InputState, float x, float y) override;
    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) override;
    virtual bool onChar(SkUnichar c, skui::ModifierKey mod) override;
    virtual bool onDropFile(const char* dropped_file,
                            uintptr_t /* uint8_t*  */ iptr = 0,
                            size_t length = 0) override;

    DecUI &UI() {
        return *_ui;
    }    

protected:
    DecUI *_ui;  

    int _width;
    int _height;
    float _scale;    

    void _drawUI(SkCanvas &canvas);

private:
};

}

#endif //__DEC_UI_LAYER_H__