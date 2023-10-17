#include "BlankLayer.h"

using namespace sk_ui;

BlankLayer::BlankLayer()
    : fShowImGuiDemoWindow( true )
    , fPause( false )
{   
    SKIAAPP()->setAppTitle( "PES Test" );
}

BlankLayer::~BlankLayer() {

}

void BlankLayer::onResize( int w, int h, float s ) {
    width = w;
    height = h;
    scale = s;
}

void BlankLayer::onPaint( SkCanvas& canvas ) {
    
    canvas.clear( SK_ColorWHITE );
    canvas.save();
    canvas.scale( scale, scale );


    ImGui::ShowDemoWindow(&fShowImGuiDemoWindow);
    
    canvas.restore();
    
}

bool BlankLayer::onChar(SkUnichar c, skui::ModifierKey mod) {
    return false;
}

// Before use, check key available in function skui::Key get_key(unsigned short vk)
bool BlankLayer::onKey(skui::Key key, skui::InputState state, skui::ModifierKey mod) {
    if ( key == skui::Key::k0 ) {
        if( state == skui::InputState::kUp ) {
            fPause = false;
        }
        else if( state == skui::InputState::kDown ) {
            fPause = true;
        }
        return true;
    }
    return false;
}

bool BlankLayer::onMouse(int x, int y, skui::InputState state, skui::ModifierKey mod) {
    return false;
}

bool BlankLayer::onMouseWheel(float delta, skui::ModifierKey) {
    return false;
}

bool BlankLayer::onTouch(intptr_t owner, skui::InputState state, float x, float y) {
    return false;
}

