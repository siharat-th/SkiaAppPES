#include "DeclarativeUI.h"
#include "DecComboBoxItem.h"
    
DecComboBoxItem::DecComboBoxItem( const char *text ) :
    DecListItem( text ) {

}

DecComboBoxItem::~DecComboBoxItem() {

}

bool DecComboBoxItem::onMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers ) {
    if( state == skui::InputState::kDown ) {
        //SkDebugf( "ComboItem pressed\n" );
        if( _logicalParent ) {
            _logicalParent->bubbleAction( ACTION_COMBOBOXITEM_SELECTED, this );
        }
    }

    return false;
}

DecComboBoxItem &ComboBoxItem( const char *text ) {
    DecComboBoxItem *w = new DecComboBoxItem( text );
    return *w;     
}