#include "TemplateLayer.h"
#include "imgui.h"

namespace sk_ui {

TemplateLayer::TemplateLayer() {}

TemplateLayer::~TemplateLayer() {}

void TemplateLayer::onResize( int w, int h, float scale ) {
    
}
void TemplateLayer::onPaint( SkCanvas& canvas ) {

}

bool TemplateLayer::onMouse(int x, int y, skui::InputState, skui::ModifierKey) {
    return false;    
}

bool TemplateLayer::onMouseWheel(float deltax, float delta, skui::ModifierKey) {
    return false;
}

bool TemplateLayer::onTouch(intptr_t owner, skui::InputState, float x, float y) {
    return false;
}

}