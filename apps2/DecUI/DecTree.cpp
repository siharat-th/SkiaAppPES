#include "DecTree.h"
#include "DecScroller.h"
#include "DecTreeItem.h"

DecTree::DecTree() {
    _selectedTreeItem = NULL;
}

DecTree::~DecTree() {

}

DecTree &Tree() {
    DecTree *w = new DecTree();
    return *w;  
}

void DecTree::select_treeItem( DecTreeItem *item, bool bUpdateVisible ) {
    if( item ) {
        if (bUpdateVisible) {
            _pScroller->pScroll2Widget = item->getRootItem();
        }

        if( _selectedTreeItem ) {
            _selectedTreeItem->select( false );
        }
        _selectedTreeItem = item;
        item->select( true );

        if( _parent ) {
            _parent->bubbleAction( ACTION_TREE_SELECT_CHANGED, this, item );
        }

        //bUpdateVisible = false;
        //if( bUpdateVisible ) {
        //    DecTreeItem *pitem = (DecTreeItem *)item->_pParentTreeItem;
        //    while( pitem ) {
        //        pitem->expand();
        //        pitem = (DecTreeItem *)pitem->_pParentTreeItem;
        //    }
        //    this->updateUI(0);
        //    SkDebugf("item: %0.2f, %0.2f %0.2f, %0.2f, %0.2f, %0.2f\n", 
        //        (float)item->_ax,
        //        (float)item->_ay,                
        //        (float)item->x(),
        //        (float)item->y(),
        //        (float)item->width(),
        //        (float)item->height()
        //    );
        //    SkDebugf("_pScroller: %0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f\n", 
        //        (float)_pScroller->_ax,
        //        (float)_pScroller->_ay,
        //        (float)_pScroller->x(),
        //        (float)_pScroller->y(),
        //        (float)_pScroller->width(),
        //        (float)_pScroller->height()
        //    );
        //
        //    //int w=0, h=0;
        //    if (_pScroller->_children.size() && _pScroller->_children[0]) {
        //        auto c = _pScroller->_children[0];
        //        //w = c->width();
        //        //h = c->height();
        //        SkDebugf("c: %0.2f, %0.2f, %0.2f, %0.2f, %0.2f, %0.2f\n", 
        //            (float)c->_ax,
        //            (float)c->_ay,
        //            (float)c->x(),
        //            (float)c->y(),
        //            (float)c->width(),
        //            (float)c->height()
        //        );
        //    }
        //
        //    //SkDebugf("item: %0.2f, %0.2f, %0.2f, %0.2f\n", 
        //    //    item->_ax, item->_ay, item->width(), item->height(), 
        //    //);
        //    if( item->_ay > this->_pScroller->_ay + this->_pScroller->height() ) {
        //        float _sy = ((this->_pScroller->_ay + this->_pScroller->height())-item->_ay);
        //        SkDebugf( "Need Scroll %0.2f\n", _sy );
        //        this->_pScroller->moveXY( 0, _sy );
        //        this->dirty();
        //    }
        //    else if( item->_ay < this->_pScroller->_ay ) {
        //        float _sy = this->_pScroller->_ay - item->_ay;
        //        SkDebugf( "Need Scroll %0.2f\n", _sy );
        //        this->_pScroller->moveXY( 0, _sy );
        //        this->dirty();                
        //    }
        //}
    }
    else {
        if( _selectedTreeItem ) {
            _selectedTreeItem->select( false );
        }
        _selectedTreeItem = NULL;
        if( _parent ) {
            _parent->bubbleAction( ACTION_TREE_SELECT_CHANGED, this, NULL );
        }
    }
    if( _onValueChanged ) {
        _onValueChanged( *this );
    }
}

void DecTree::collapseAll( int level ) {
    for( int i=0; i<_pColumn->_children.size(); i++ ) {
        DecTreeItem *item = (DecTreeItem *)_pColumn->_children[i];
        item->collapseAll( level );
    }
}

void DecTree::bubbleAction( int actionID, DecWidget *sender, void *data ) {
    if( actionID == ACTION_TREEITEM_SELECTED ) {
        if( sender ) {
            if( sender->type() == "DecTreeItem" ) {
                //dirty(true);
                select_treeItem( (DecTreeItem *)sender );
                return;
            }
        }
        select_treeItem( NULL );
        return;
    }
    
    DecWidget::bubbleAction( actionID, sender );
}

void DecTree::clear() {
    dirty();
    bubbleAction(ACTION_TREEITEM_SELECTED, NULL);
    if (_pColumn) {
        _pColumn->dirty();
        _pColumn->_children.clear();
    }
}