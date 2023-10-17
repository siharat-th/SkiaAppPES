#include "DecSwitch.h"
#include "DecUI.h"

DecSwitch::DecSwitch( const char *title, void (*callback)(DecSwitch &button) ) 
    : DecLabel( title, 0xf204) {
    _icon->fontSize( 32 );
    _callback = callback;

    _isPressed = false;
    _isChecked = false;

    _textWidget->align( ALIGN_CENTER );
    _textWidget->align( ALIGN_CENTER );
}

DecSwitch::~DecSwitch() {

}

DecSwitch &Switch( const char *title, void (*callback)(DecSwitch &button) ) {
    DecSwitch *w = new DecSwitch( title, callback );
    return *w;  
}

bool DecSwitch::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers ) {
    if( state == skui::InputState::kDown ) {
        //active( true );
        _isPressed = true;
        mouseCapture( true );
    }
    else if( state == skui::InputState::kUp ) {
        if( _isPressed && hitTest( ix+x(), iy+y() ) ) {
            _isChecked = !_isChecked;
            if( _isChecked ) {
                _icon->setIcon( 0xf205 );
            }
            else {
                _icon->setIcon( 0xf204 );
            }

            if( _callback ) {
                (*_callback)(*this);
            }
        }
        _isPressed = false;
        mouseCapture( false );
    }

    return false;
}
