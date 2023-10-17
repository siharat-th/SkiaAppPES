#include "DecUILayer.h"
#include "imgui.h"

namespace sk_ui {

DecUILayer::DecUILayer() {
    fScaleFactor = 1.0;

    _ui = &createUI( );    

    _width = 0;
    _height = 0;
    _scale = 1.0;
}

DecUILayer::~DecUILayer() {}

void DecUILayer::onResize( int w, int h, float scale ) {
    fScaleFactor = scale;    

    _width = w;
    _height = h;
    _scale = scale;    

    if( _ui ) {
        _ui->onResize( _width, _height, _scale );
    }    
}

void DecUILayer::onUpdate() {
    if (_ui) {
        _ui->onUpdate();
    }
}

void DecUILayer::onPrePaint( SkCanvas& canvas ) {
}

void DecUILayer::onPaint( SkCanvas& canvas ) {
    canvas.clear( SK_ColorTRANSPARENT );
    _drawUI( canvas );
}

void DecUILayer::onPostPaint( SkCanvas& canvas ) {
}

void DecUILayer::_drawUI(SkCanvas &canvas) {
    if( _ui ) {
        _ui->draw(canvas);
    }
}

bool DecUILayer::onMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers ) { 
    if( _ui ) {
        if( _ui->onMouse( x, y, state, modifiers ) ) {
            return true;
        }
    }
    return false; 
}  

bool DecUILayer::onMouseWheel(float deltax, float delta, skui::ModifierKey modifiers) {
    if( _ui ) {
        if( _ui->onMouseWheel( deltax, delta, modifiers ) ) {
            return true;
        }
    }    
    return false;
}

bool DecUILayer::onTouch(intptr_t owner, skui::InputState, float x, float y) {
    return false;
}

bool DecUILayer::onKey(skui::Key k, skui::InputState state, skui::ModifierKey modifiers) {
    //SkDebugf("DecUILayer::onKey: 0x%04x\n", k);
    if( _ui ) {
        if( _ui->onKey( k, state, modifiers ) ) {
            return true;
        }
    }    
    return false;
}

bool DecUILayer::onChar(SkUnichar c, skui::ModifierKey modifiers) {
    //SkDebugf("DecUILayer::onChar: 0x%04x\n", c);
    if (_ui) {
        if (_ui->onChar(c, modifiers)) {
            return true;
        }
    }
    return false; 
}

bool DecUILayer::onDropFile(const char* dropped_file, uintptr_t /* uint8_t*  */ iptr, size_t length) { 
    if (_ui) {
        if (_ui->onDropFile(dropped_file, iptr, length)) {
            return true;
        }
    }
    return false;
}

}