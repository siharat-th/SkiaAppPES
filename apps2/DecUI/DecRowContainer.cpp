#include "DecUI.h"
#include "DecRowContainer.h"

DecRowContainer::DecRowContainer() 
{
    //SkDebugf( "DecRowContainer\n" );
    _setGap( 0 );
}

DecRowContainer::~DecRowContainer() {

}

DecRowContainer &Row() {
    DecRowContainer *w = new DecRowContainer();
    return *w;    
}

void DecRowContainer::_setGap( int g ) {
    _gap = g;
    _minWidth = _padding[1] + _padding[3];
}

int DecRowContainer::contentWidth() { 
    if( _ow == 0 ) {
        if( _children.size() > 0 ) {
            for( int i=_children.size()-1; i>=0; i-- ) {
                DecWidget *w = _children[i];
                if( w->visible() == false ) {
                    continue;
                }
                return w->x() + w->width();
            }            
        }
        return 0;
    }
    return _width - (_padding[1] + _padding[3]); 
}

void DecRowContainer::_updateChildrenUI( int level ) {
    int cx = _padding[3];
    int maxW = 0;
    int nFill = 0;
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        if( w->visible() == false )
            continue;        

        cx += w->_margin[3];
        w->x(cx);
        w->updateUI( level );

        if( w->_ow != -1 ) {
            maxW = cx + w->width();
            cx += w->width() + _gap;
        }
        else {
            maxW = cx;
            cx += _gap;
        }
        cx += w->_margin[1];

        if( w->_ow == -1 ) {
            nFill++;
        }
    }
    
    if( (nFill > 0) ) {
        int ex = ((contentX() + contentWidth()) - maxW);
        DecWidget *lw = _children[_children.size()-1];
        ex -= lw->_margin[1];        
        int nAdd = ex / nFill;
        int nRem = ex - (nAdd * nFill);

        if( (maxW >= (contentX() + contentWidth())) ) {
            nAdd = 0;
            nRem = 0;
        }

        int ax = 0;
        for( int i=0; i<_children.size(); i++ ) {
            DecWidget *w = _children[i];
            w->adjustX( w->x() + ax );
            if( w->_ow == -1 ) {
                int aw = nAdd;
                if( nRem > 0 ) {
                    aw++;
                    nRem--;
                }
                w->adjustWidth( aw );
                ax += w->width();

            }
        }  

    }
    _isDirtyLayout = true;
}

void DecRowContainer::_updateChildrenAlignment() {
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        w->_updateAlignmentY();
    }
}