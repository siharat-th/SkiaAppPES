#include "MyWidgetLayer.h"
    
namespace sk_ui {

MyWidgetLayer::MyWidgetLayer( const char *src ) 
//    : WidgetLayer( src )
{
}

MyWidgetLayer::~MyWidgetLayer() {
    
}

void MyWidgetLayer::onUpdate() {
    WidgetLayer::onUpdate();
}

void MyWidgetLayer::onPaint(SkCanvas &canvas){
    WidgetLayer::onPaint(canvas);
    pCanvas.draw(canvas);
}

bool MyWidgetLayer::onMouse(int x, int y, skui::InputState state, skui::ModifierKey key){
    pCanvas.onMouse(x, y, state);
    return WidgetLayer::onMouse(x, y, state, key);
}

bool MyWidgetLayer::onMouseWheel(float delta, skui::ModifierKey key){
    return WidgetLayer::onMouseWheel(delta, key);
}

bool MyWidgetLayer::onChar(SkUnichar c, skui::ModifierKey modifiers){
//    std::cout << c << std::endl;
    return true;
}

void MyWidgetLayer::onResize( int w, int h, float s ){
    WidgetLayer::onResize(w, h, s);
    width = w;
    height = h;
    scale = s;
    pCanvas.set(w, h, s);
}

} 
