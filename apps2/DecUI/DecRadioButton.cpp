#include "DecRadioButton.h"

DecRadioButton::DecRadioButton( const char *title, void (*callback)(DecRadioButton &button) ) 
    : DecLabel( title, 0xf111) {
    _callback = callback;

    _isPressed = false;
    _isChecked = false;
}

DecRadioButton::~DecRadioButton() {

}

DecRadioButton &RadioButton( const char *title, void (*callback)(DecRadioButton &button) ) {
    DecRadioButton *w = new DecRadioButton( title, callback );
    return *w;  
}

bool DecRadioButton::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers ) {
    if( state == skui::InputState::kDown ) {
        //active( true );
        _isPressed = true;
        mouseCapture( true );
    }
    else if( state == skui::InputState::kUp ) {
        if( _isPressed && hitTest( ix+x(), iy+y() ) ) {
            _isChecked = !_isChecked;
            if( _isChecked ) {
                _icon->setIcon( 0xf192 );
            }
            else {
                _icon->setIcon( 0xf111 );
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
