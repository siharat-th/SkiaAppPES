#include "DecListItem.h"
#include "DecUI.h"
#include "DecList.h"

DecListItem::DecListItem( const char *txt ) {

    border( false );
    padding( 2, 5, 2, 5 );
    size( -1, 0 );
    _( Text( txt )
        .ref( (DecWidget **)&_pText )
        .size( -1, 0 )
        .background(false)
        .align( ALIGN_CENTER ) 
    );
    text( txt );
    cache( false );
}

DecListItem::~DecListItem() {

}

DecWidget& DecListItem::text( const char *t) {
    DecWidget::text( t );
    if( _pText ) {
        _pText->text( t );
    }
    return *this;
}

DecListItem &ListItem( const char *text ) {
    DecListItem *w = new DecListItem(text);
    return *w;      
}

DecWidget &DecListItem::select( bool i ) {
    DecWidget::select( i );

    if( i ) {
        background( true );
        bg_color( 0xFFD0D0D0 );
    }
    else {
        background( true );
        bg_color( 0xFFFFFFFF );
    }
    return *this;
}

bool DecListItem::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifier ) {
    //if( state == skui::InputState::kDown ) {
    //    bubbleAction( ACTION_LISTITEM_SELECTED, this );
    //}
    if( state == skui::InputState::kDown ) {
        _isSelected = !_isSelected;
        if( _isSelected ) {
            bg_color(0xFFFF0000);
            bubbleAction( ACTION_LISTITEM_SELECTED, this );
        }
        else {
            bg_color(0xFFFFFFFF);
            bubbleAction( ACTION_LISTITEM_SELECTED, NULL );
        }  
    }    

    return false;
}
