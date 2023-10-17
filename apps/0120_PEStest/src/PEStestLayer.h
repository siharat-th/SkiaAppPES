#ifndef __HELLO_WORLD_LAYER_H__
#define __HELLO_WORLD_LAYER_H__

#include "sk_ui.h"

class PEStestLayer : public sk_ui::SkiaAppLayer {
public:
    PEStestLayer( );
    virtual ~PEStestLayer() override;

    void onResize( int w, int h, float scale ) override;
    void onPaint( SkCanvas& canvas ) override;
    bool onChar(SkUnichar c, skui::ModifierKey mod) override;
    bool onKey(skui::Key, skui::InputState, skui::ModifierKey) override;
    bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    bool onMouseWheel(float delta, skui::ModifierKey) override;
    bool onTouch(intptr_t owner, skui::InputState, float x, float y) override;

    void addText();
    void saveDoc();

private:
    
    bool        fShowImGuiDock;
    bool        fPause;

    int         width;
    int         height;
    float       scale;

    SkScalar fRotationAngle;

    ImGuiID dock_main_id;
    ImGuiID dock_id_prop;
    ImGuiID dock_id_bottom;

    void ShowAppDockSpace(bool* p_open);
    
    void ShowDocBuilder();

    void ShowLayerPane(bool *p_open);
    void ShowToolPane(bool *p_open);

    void newDocument();

    void updateInfo();
};



#endif // __HELLO_WORLD_LAYER_H__
