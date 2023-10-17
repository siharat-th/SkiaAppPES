#include "DecList.h"
#include "DecScroller.h"
#include "DecListItem.h"

DecList::DecList() {
    _selectedListItem = NULL;

    child( Scroller()
        .ref( (DecWidget **)&_pScroller )
        .size( -1, -1 )
        .background( true )
        .bg_color( 0xFFF0F0F0 )
        ._(Column()
            .ref( (DecWidget **)&_pColumn )
            .size( -1, 0 )
            .background( false )
            .border( false )
        )
        .cache( false )
    );
}

DecList::~DecList() {

}

DecList &List() {
    DecList *w = new DecList();
    return *w;  
}

DecListItem *DecList::getItem( int index ) {
    if( index >= 0 && index < _pColumn->_children.size() ) {
        return (DecListItem *)_pColumn->_children[index];
    }
    return NULL;
}

DecWidget &DecList::_( DecWidget &c, bool front ) {
    if( _pColumn ) {
        c._index = _pColumn->_children.size();
        c.dirty( true, true );
        _pColumn->_(c, front);
        _pColumn->dirty( true, true );
    }
    return *this;
}

void DecList::clear() { 
    _pColumn->_children.clear(); 
    _pColumn->dirty( true ); 
    
    _pColumn->x(0);
    _pColumn->y(0);  
}

DecWidget &DecList::select_listItem( int index ) {
    _value = index;
    if( index == -1 ) {
        if( _selectedListItem ) {
            _selectedListItem->select( false );
        }
        _selectedListItem = NULL;
        if( _parent ) {
            _parent->bubbleAction( ACTION_LIST_SELECT_CHANGED, this, NULL );
        }        
    }
    else {
        if( _pColumn ) {
            if( _selectedListItem ) {
                _selectedListItem->select( false );
            }
            _selectedListItem = (DecListItem *)_pColumn->_children[index];
            _selectedListItem->select( true );
            if( _parent ) {
                _parent->bubbleAction( ACTION_LIST_SELECT_CHANGED, this, _selectedListItem );
            }     
        }
    }
    if (_selectedListItem) {
        text(_selectedListItem->text());
    } else {
        text("");
    }
    if( _onValueChanged ) {
        _onValueChanged( *this );
    }
    return *this;
}

bool DecList::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers) { 
    if (_pScroller && _pColumn && 
        _pScroller->height() < _pColumn->height()  &&
        state == skui::InputState::kMove)
        return true;
    return DecWidget::onMouse(ix, iy, state, modifiers);
}

void DecList::bubbleAction( int actionID, DecWidget *sender, void *data ) {
    if( actionID == ACTION_LISTITEM_SELECTED ) {
        if( sender ) {
            _value = sender->_index;
            text( sender->text() );
            if( sender->type() == "DecListItem" ) {
                //dirty(true);
                if( _selectedListItem ) {
                    _selectedListItem->select( false );
                }
                _selectedListItem = (DecListItem *)sender;
                sender->select( true );
                if( _parent ) {
                    _parent->bubbleAction( ACTION_LIST_SELECT_CHANGED, this, _selectedListItem );
                }
            }
        }
        else {
            _value = -1;
            if( _selectedListItem ) {
                _selectedListItem->select( false );
            }
            _selectedListItem = NULL;
            if( _parent ) {
                _parent->bubbleAction( ACTION_LIST_SELECT_CHANGED, this, NULL );
            }
        }       
        if( _onValueChanged ) {
            _onValueChanged( *this );
        }
    }
    else {
        DecWidget::bubbleAction( actionID, sender, data );
    }
}