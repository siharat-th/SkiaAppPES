#include "DeclarativeUI.h"
#include "DecTestWidget.h"

DecTestWidget::DecTestWidget() : DecWidget() {

}

DecTestWidget::~DecTestWidget() {

}

DecTestWidget &TestWidget() {
    DecTestWidget *w = new DecTestWidget();
    return *w;
}

void DecTestWidget::update() {
    DecWidget::update();
    //SkDebugf( "Update\n" );
    static int w = 0;
    if( w != width() ) {
        SkDebugf( "Update %s %i\n", type().c_str(), width() );
        w = width();
    }    
}

bool DecTestWidget::updateUI( int level ) {
    bool b = DecWidget::updateUI( level );
    static int w = 0;
    if( w != width() ) {
        SkDebugf( "UpdateUI %s %i %i %i %i\n", type().c_str(), width(), _isDirtyLayout, __w, _width );
        w = width();
    }
    return b;
}

void DecTestWidget::onResized(int _w, int _h) {
    DecWidget::onResized( _w, _h );
    static int w = 0;
    if( w != _w ) {
        SkDebugf( "onResized %s %i %i\n", type().c_str(), _w, width() );
        w = _w;
    }    
}

void DecTestWidget::fireOnResized() {
    DecWidget::fireOnResized();
    static int w = 0;
    if( w != width() ) {
        SkDebugf( "fireOnResized %s %i\n", type().c_str(), width() );
        w = width();
    }
}

void DecTestWidget::dirty(bool b, bool updateLayout ) {
    DecWidget::dirty( b, updateLayout );
    SkDebugf( "dirty %i %i\n", b, _isDirtyLayout );
}