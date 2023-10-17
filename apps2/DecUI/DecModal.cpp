#include "DecModal.h"
#include "DecUI.h"
    
DecModal::DecModal() {
    _isModal = true;

    visible( false );
    size( 320, 240 );
    align( ALIGN_CENTER );
    border( true );
    border_color( 0xFF000000 );
    background( true );
    bg_color( 0xFFFFFFFF );
}

DecModal::~DecModal() {

}

DecWidget& DecModal::visible( bool v ) {
    _isVisible = v;
    if( _ui && _ui->modalWidget ) {
        _ui->modalWidget->visible( v );
        if (v) {
            _ui->modalWidget->dirty(true, true);
        }
    }
    return *this;
}

DecModal &Modal() {
    DecModal *w = new DecModal();
    return *w;     
}

void DecModal::_draw_content(SkCanvas &canvas) {
}