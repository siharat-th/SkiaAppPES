#include "DecButton.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"

DecButton::DecButton( const char *title, const int icon, std::function<void(DecButton &button)> callback )  //void (*callback)(DecButton &button) ) 
    : DecLabel( title, icon ) {
    _callback = callback;

    padding( 5, 12, 5, 12 );
    border( true );
    bg_color( 0xFFF0F0F0 );
    _bg_color_pressed = 0x80808080;
    round( 10 );

    _isPressed = false;

    cache( true );
}

DecButton::~DecButton() {

}

DecTabButton::DecTabButton( const char *title, const int icon,std::function<void(DecButton &button)> callback ) 
    : DecButton( title, icon, callback ) {
    selected = false;
}

DecTabButton::~DecTabButton() {

}

DecButton &Button( const char *title, const int icon, std::function<void(DecButton &button)> callback ) {
    DecButton *w = new DecButton( title, icon, callback );
    return *w;  
}

DecTabButton &TabButton( const char *title, const int icon, std::function<void(DecButton &button)> callback ) {
    DecTabButton *w = new DecTabButton( title, icon, callback );
    return *w;  
}

bool DecButton::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers ) {
    if( state == skui::InputState::kDown ) {
        //SkDebugf( "Button MouseDown\n" );
        _isPressed = true;
        //invalidate();
        dirty( true, false );
        mouseCapture( true );
        if( _onMousePressed ) {
            if( _onMousePressed( *this ) )
                return true;
        }        
        //active( true );
    }
    else if( state == skui::InputState::kUp ) {
        //SkDebugf( "Button up\n" );

        if( _isPressed ) {
            if( hitTest( ix+x(), iy+y() ) ) {
                if( _callback ) {
                    (_callback)(*this);
                }
            }
            _isPressed = false;
            //invalidate();
            dirty( true, false );
            mouseCapture( false );
            if( _onMouseReleased ) {
                if( _onMouseReleased( *this ) )
                    return true;
            }               
        }
    }

    return false;
}

void DecButton::_draw_bg(SkCanvas &canvas) {
    if( !_background ) {
        return;
    }
    SkPaint paint;

    paint.setColor( _bg_color );
    paint.setStyle( SkPaint::Style::kFill_Style );

    if( _round ) {
        paint.setAntiAlias( true );
        canvas.drawRoundRect( SkRect::MakeXYWH( 0, 0, _width, _height ), _round, _round, paint );
    }
    else {
        canvas.drawRect( SkRect::MakeXYWH( 0, 0, _width, _height ), paint) ;
    }

    if( _isPressed ) {
        paint.setColor( _bg_color_pressed );
        if( _round ) {
            paint.setAntiAlias( true );
            canvas.drawRoundRect( SkRect::MakeXYWH( 0, 0, _width, _height ), _round, _round, paint );
        }
        else {
            canvas.drawRect( SkRect::MakeXYWH( 0, 0, _width, _height ), paint) ;
        }
    }

}

DecWidget& DecTabButton::select(bool a) {
    DecWidget::select(a);
    selected = a;
    if( a ) {
        bg_color( 0xFFFFFFFF );
    }
    else {
        bg_color( 0xFFF0F0F0 );
    }
    dirty();

    return *this;
}

void DecTabButton::_draw_bg(SkCanvas &canvas) {
    if( !_background ) {
        return;
    }
    SkPaint paint;

    paint.setColor( _bg_color );
    paint.setStyle( SkPaint::Style::kFill_Style );

    paint.setAntiAlias( true );
    SkPath path;

    path.moveTo( 0, _height );
    path.lineTo( 0, 0 + _round );
    path.arcTo( 0, 0, _round, 0, _round );
    path.lineTo( _width-_round, 0 );
    path.arcTo( _width, 0, _width, _round, _round );
    path.lineTo( _width, _height );        
    canvas.drawPath( path, paint );

    if( _isPressed ) {
        paint.setColor( _bg_color_pressed );
        paint.setAntiAlias( true );
        canvas.drawPath( path, paint );
    }
}

void DecTabButton::_draw_border(SkCanvas& canvas) {
    if (!_border) {
        return;
    }
    SkPaint paint;

    paint.setStrokeWidth(_borderWidth);
    paint.setColor(_border_color);
    paint.setStyle(SkPaint::Style::kStroke_Style);

    const SkScalar stroke_halfw = paint.getStrokeWidth() / 2;
    SkRect border_rc = SkRect::MakeWH(_width, _height);
    border_rc.inset(stroke_halfw, stroke_halfw);

    if (_round ) {
        if( !_captureNextFrame || _captureBorder ) {
            paint.setAntiAlias(true);
            SkPath path;

            path.moveTo( 0, _height );
            path.lineTo( 0, 0 + _round );
            path.arcTo( 0, 0, _round, 0, _round );
            path.lineTo( _width-_round, 0 );
            path.arcTo( _width, 0, _width, _round, _round );
            path.lineTo( _width, _height );
            canvas.drawPath( path, paint );
        }
    } else {
        canvas.drawRect(border_rc, paint);
    }
    if( !selected ) {
        canvas.drawLine( 0, _height, _width, _height, paint );        
    }
}