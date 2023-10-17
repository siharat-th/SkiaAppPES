#include "DecTab.h"

DecTabBar::DecTabBar(std::function<void(DecTabBar &tabBar, int activeTab)> callback) {
    _callback = callback;
    nActiveTab = -1;
}

DecTabBar::~DecTabBar() {

}

void DecTabBar::setActiveTab( int n ) {
    nActiveTab = n;
    for( int i=0; i<tabButtons.size(); i++ ) {
        if( n == tabButtons[i]->tag() ) {
            tabButtons[i]->bg_color( 0xFFFFFFFF );
            tabButtons[i]->select(true);
        }
        else {
            tabButtons[i]->bg_color( 0xFFF0F0F0 );
            tabButtons[i]->select(false);
        }
    }    
    if( _callback ) {
        _callback( *this, nActiveTab );
    }
}

DecWidget &DecTabBar::_( DecWidget &c, bool front ) {
    DecWidget::_( c, front );
    if( !strcmp( c.type().c_str(), "TabButton" ) ) {
        SkDebugf( "Add TabButton OK\n" );
        DecTabButton *btn = (DecTabButton *)&c;
        tabButtons.push_back( btn );
        btn->onMousePressed( [=](DecWidget &btn) -> bool {
            //SkDebugf( "On TabButton Click %i\n", btn.tag());
            DecTabButton *tbtn = (DecTabButton *)&btn;
            setActiveTab( tbtn->tag() );
            return true;
        });
    }
    return *this;
}

DecTabBar &TabBar(std::function<void(DecTabBar &tabBar, int activeTab)> callback) {
    DecTabBar *w = new DecTabBar(callback );
    return *w; 
}

DecTabContent::DecTabContent() {

}

DecTabContent::~DecTabContent() {

}

DecTabContent &TabContent() {
    DecTabContent *w = new DecTabContent( );
    return *w;     
}

void DecTabContent::_draw_border(SkCanvas& canvas) {
    if (!_border) {
        return;
    }
    SkPaint paint;

    if (_isFocus) {
        paint.setStrokeWidth(_focus_borderWidth);
        paint.setColor(_focus_border_color);
    } else {
        paint.setStrokeWidth(_borderWidth);
        paint.setColor(_border_color);
    }
    paint.setStyle(SkPaint::Style::kStroke_Style);

    // if( _round ) {
    //    paint.setAntiAlias( true );
    //    float b2 = _borderWidth / 2.0;
    //    canvas.drawRoundRect( SkRect::MakeXYWH( b2, b2, _width-_borderWidth, _height-_borderWidth
    //    ), _round, _round, paint );
    //}
    // else {
    //    canvas.drawRect( SkRect::MakeXYWH( 0, 0, _width, _height ), paint) ;
    //}

    const SkScalar stroke_halfw = paint.getStrokeWidth() / 2;
    SkRect border_rc = SkRect::MakeWH(_width, _height);
    border_rc.inset(stroke_halfw, stroke_halfw);

    if (_round ) {
        if( !_captureNextFrame || _captureBorder ) {
            paint.setAntiAlias(true);
            canvas.drawRoundRect(border_rc, _round, _round, paint);
        }
    } else {
        //canvas.drawRect(border_rc, paint);
        canvas.drawLine( 0, 0, 0, _height, paint );
        canvas.drawLine( 0, _height, _width, _height, paint );
        canvas.drawLine( _width, _height, _width, 0, paint );
    }
}