#include "DecUI.h"
#include "DecColumnContainer.h"

DecColumnContainer::DecColumnContainer() 
{
    //SkDebugf( "DecColumnContainer\n" );
    _setGap(0);    
}

DecColumnContainer::~DecColumnContainer() {

}

DecColumnContainer &Column() {
    DecColumnContainer *w = new DecColumnContainer();
    return *w;    
}

int DecColumnContainer::contentHeight() { 
    if( _oh == 0 ) {
        if( _children.size() > 0 ) {
            for( int i=_children.size()-1; i>=0; i-- ) {
                DecWidget *w = _children[i];
                if( w->visible() == false ) {
                    continue;
                }
                return w->y() + w->height();
            }
        }
        return 0;
    }    
    return _height - (_padding[0] + _padding[2]); 
} 

void DecColumnContainer::_setGap( int g ) {
    _gap = g;
    _minHeight = _padding[0] + _padding[2];        
}

void DecColumnContainer::_updateChildrenUI( int level ) {
    int cy = _padding[0];
    int maxH = 0;
    int nFill = 0;
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        if( w->visible() == false )
            continue;
        //w->y(0);
        //w->align( ALIGN_ABSOLUTE );
        //w->updateUI(); // get children width and height
        //w->align( ALIGN_TOPLEFT );
        cy += w->_margin[0];
        w->y(cy);
        w->updateUI( level );

        if( w->_oh != -1 ) {
            maxH = cy + w->height();
            cy += w->height() + _gap;
        }
        else {
            maxH = cy;
            cy += _gap;
        }
        cy += w->_margin[2];

        if( w->_oh == -1 ) {
            nFill++;
        }
    }
    
    if( (nFill > 0) ) {
        int ey = ((contentY() + contentHeight()) - maxH);
        DecWidget *lw = _children[_children.size()-1];
        ey -= lw->_margin[2];
        int nAdd = ey / nFill;
        int nRem = ey - (nAdd * nFill);

        if( (maxH >= (contentY() + contentHeight())) ) {
            nAdd = 0;
            nRem = 0;
        }

        int ay = 0;
        for( int i=0; i<_children.size(); i++ ) {
            DecWidget *w = _children[i];
            w->adjustY( w->y() + ay );
            if( w->_oh == -1 ) {
                int ah = nAdd;
                if( nRem > 0 ) {
                    ah++;
                    nRem--;
                }
                w->adjustHeight( ah );
                ay += w->height();
            }
        }        
    }
}

void DecColumnContainer::_updateChildrenAlignment() {
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->_updateAlignmentX();
    }
}