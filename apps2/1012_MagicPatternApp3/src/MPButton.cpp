#include "MPButton.h"

DecMPButton::DecMPButton(const char *title, const int icon, std::function<void(DecButton &button)> callback) 
    : DecButton( title, icon, callback ) {
    padding( 7, 16, 7, 16 );
    border( true );
    border_color( 0x80000000 );
    bg_color( 0xFFFFFFFF );
    _bg_color_pressed = 0x80A0A0A0;
    round( 12 );

}

DecMPButton::~DecMPButton() {
    
}

DecMPButton &MPButton(const char *title, const int icon, std::function<void(DecButton &button)> callback) {
    DecMPButton *w = new DecMPButton( title, icon, callback );
    return *w;      
}
