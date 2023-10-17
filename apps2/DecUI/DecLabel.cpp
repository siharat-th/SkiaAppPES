#include "DecLabel.h"
#include "DecUI.h"
#include "DecSpacer.h"

DecLabel::DecLabel( const char *title, const int icon ) {
    //if( _ow != 0 )
    //    _( Spacer().size(-1,0) );
    _( Row()
        .size( 0, 0 )
        .ref( (DecWidget**)&_container)
        .align( ALIGN_CENTER )
        .border(false)
        .background(false)
        .gap( 0 )
    );
    _icon = NULL;
    if( icon != 0 ) {
        _icon = new DecIcon( icon );
        _icon->fontSize( 14 );
        _icon->align( ALIGN_CENTER );
        _container->_( *_icon );
    }

    _textWidget = new DecText();
    _textWidget->text( title );
    _textWidget->fontSize( 14 );
    _textWidget->align( ALIGN_CENTER );
    _container->_( *_textWidget );
    //if( _ow != 0 )
    //    _( Spacer().size(-1,0) );

    if( strlen(title) ) {
        _container->gap(6);
    }
    else {
        _container->padding_left(1);
    }
    border( false );
    DecWidget::size( 0, 0 );
}

DecLabel::~DecLabel() {

}

DecLabel &Label( const char *title, const int icon ) {
    DecLabel *w = new DecLabel( title, icon );
    return *w;    
}

DecWidget &DecLabel::fontSize( const int s ) {
    _textWidget->fontSize( s );
    if( _icon ) {
        _icon->fontSize( s );
    }
    return *this;
}

DecWidget& DecLabel::text_color(uint32_t c) {
    _textWidget->text_color( c );
    if( _icon ) {
        _icon->text_color( c );
    }
    return *this;
}