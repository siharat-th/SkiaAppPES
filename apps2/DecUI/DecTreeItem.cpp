#include "DecTreeItem.h"
#include "DecUI.h"
#include "DecList.h"

DecTreeItem::DecTreeItem( const char *txt ) {
    _pParentTreeItem = NULL;
    _pTree = NULL;
    _pExpandIcon = NULL;
    _pContentRow = NULL;
    _pRightIconContainer = NULL;
    _isSelected = false;
    _isExpand = true;
    _pText = NULL;

    //text( txt ).cache( false );
    size( -1, 0 );
    border( false );
    child( Row()
        .ref( (DecWidget **)&_pContentRow )
        .padding( 2, 5, 2, 5 )
        .size( -1, 0 )
        .border( false )
        .background( false )
        ._( Icon( 0xe122 ).size( 0, 0 ).cache( false )
            .background(false).align( ALIGN_CENTER ).padding_right( 5 )
            .ref( (DecWidget **)&_pExpandIcon )
            .onMousePressed( [=](DecWidget &widget) -> bool {
                toggleExpand();
                return true;
            })
        )
        ._( Text( txt )
            .ref( (DecWidget **)&_pText )
            .size( -1, 0 )
            .background(false)
            .align( ALIGN_CENTER ) 
        )
        ._( Row().size( 0, 0).ref( (DecWidget **)&_pRightIconContainer ).border(false).background(false))
    );
    child( Column()
        .padding( 0, 0, 0, 16 )
        .size( -1, 0 )
        .border( false )
        .ref( (DecWidget **)&_pChildContainer )
    );
}

DecWidget& DecTreeItem::text( const char *t) {
    DecWidget::text( t );
    if( _pText ) {
        _pText->text( t );
    }

    return *this;
}

DecTreeItem::~DecTreeItem() {

}

DecTreeItem &TreeItem( const char *text ) {
    DecTreeItem *w = new DecTreeItem(text);
    return *w;      
}

DecTreeItem &DecTreeItem::addWidget( DecWidget &widget ) {
    _pRightIconContainer->_( widget );
    
    return *this;
}

DecTreeItem& DecTreeItem::expand() {
    _pChildContainer->visible(true);
    _pExpandIcon->setIcon( 0xf0d7 );
    _isExpand = true;
    dirty(true, true);
    return *this;
}

DecTreeItem& DecTreeItem::collapse() {
    _pChildContainer->visible(false);
    _pExpandIcon->setIcon( 0xf0da );
    _isExpand = false;
    dirty(true, true);
    return *this;
}

DecTreeItem& DecTreeItem::collapseAll( int level, int cur_level ) {
    if( cur_level >= level ) {
        collapse();
    }
    for( int i=0; i<_pChildContainer->_children.size(); i++ ) {
        DecTreeItem *item = (DecTreeItem *)_pChildContainer->_children[i];
        item->collapseAll( level, cur_level+1 );
    }
    return *this;
}

void DecTreeItem::toggleExpand() {
    if( _pChildContainer->_children.size() == 0 ) 
        return;
    if( _pExpandIcon ) {
        if( isExpand() ) {
            collapse();
        }
        else {
            expand();
        }
    }
}

DecWidget &DecTreeItem::select( bool i ) {
    DecWidget::select( i );

    if( i ) {
        background( true );
        bg_color( 0xFFD0D0D0 );
    }
    else {
        background( false );
        bg_color( 0xFFFFFFFF );
    }
    return *this;
}

DecWidget &DecTreeItem::_( DecWidget &c, bool front ) {
    if( _pChildContainer ) {
        //c.tag( _pChildContainer->_children.size() );
        if( c.type() == "DecTreeItem" ) {
            DecTreeItem *p = (DecTreeItem *)&c;
            p->_pParentTreeItem = this;
        }
        _pChildContainer->_(c, front);
    }
    return *this;
}

void DecTreeItem::bubbleAction( int actionID, DecWidget *sender, void *data ) {
    if( actionID == ACTION_TREEITEM_SELECTED ) {
        //SkDebugf( "Got bubble\n" );
    }
    if( _parent ) {
        _parent->bubbleAction( actionID, sender );
    }
}

bool DecTreeItem::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers ) {
    if( _pChildContainer->hitTest( ix, iy ) ) {
        if( _pChildContainer->onMouse( ix - _pChildContainer->x(), iy - _pChildContainer->y(), 
            state, modifiers ) ) {
            return true;
        }
        return false;
    }
    for( int i=_children.size()-1; i>=0; i-- ) {
        DecWidget *w = _children[i];
        if( w->hitTest( ix, iy ) ) {
            if( w->onMouse( ix - w->x(), iy - w->y(), state, modifiers ) ) {
                _isPressed = false;
                mouseCapture( false );            
                return true;
            }
        }
    }     
    if( state == skui::InputState::kDown ) {
        //_isPressed = true;
        //mouseCapture( true );
        _isSelected = !_isSelected;
        if( _isSelected ) {
            bg_color(0xFFFF0000);
            bubbleAction( ACTION_TREEITEM_SELECTED, this );
        }
        else {
            bg_color(0xFFFFFFFF);
            bubbleAction( ACTION_TREEITEM_SELECTED, NULL );
        }        
    }
    else if( state == skui::InputState::kUp ) {
        if( _isPressed ) {
            //if( hitTest( ix+x(), iy+y() ) ) {
            //    _isSelected = !_isSelected;
            //    if( _isSelected ) {
            //        bg_color(0xFFFF0000);
            //        bubbleAction( ACTION_TREEITEM_SELECTED, this );
            //    }
            //    else {
            //        bg_color(0xFFFFFFFF);
            //        bubbleAction( ACTION_TREEITEM_SELECTED, NULL );
            //    }
            //}
            //_isPressed = false;
            //mouseCapture( false );
        }
    }
    

    return false;
}