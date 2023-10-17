#include "DecCheckBox.h"

DecCheckBox::DecCheckBox( const char *title, std::function<void(DecCheckBox &cb)>callback ) 
    : DecLabel( title, 0xf0c8 ) {
    _callback = callback;

    _isPressed = false;
    _isChecked = false;

    cache( true );
}

DecCheckBox::~DecCheckBox() {

}

DecCheckBox &CheckBox( const char *title, std::function<void(DecCheckBox &cb)>callback ) {
    DecCheckBox *w = new DecCheckBox( title, callback );
    return *w;  
}

DecCheckBox &DecCheckBox::check( bool b, bool update ) { 
    _isChecked = b; 
    value( _isChecked );
    if( _isChecked ) {
        _icon->setIcon( 0xf2d3 );
    }
    else {
        _icon->setIcon( 0xf0c8 );
    }
    if( update ) {
        if( _callback ) {
            _callback(*this);
        }
        if( _onValueChanged ) {
            _onValueChanged( *this );
        }
    }
    dirty(); 
    return *this;
}

bool DecCheckBox::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers ) {
    if( state == skui::InputState::kDown ) {
        //SkDebugf( "(ChcekBox) %s\n", type().c_str() );
        //active( true );
        _isPressed = true;
        mouseCapture( true );
    }
    else if( state == skui::InputState::kUp ) {
        if( _isPressed && hitTest( ix+x(), iy+y() ) ) {
            check( !_isChecked );
        }
        _isPressed = false;
        mouseCapture( false );
    }

    return false;
}
