#include "DecUI.h"
#include "DecFlowContainer.h"

DecFlowContainer::DecFlowContainer() 
{
    //SkDebugf( "DecFlowContainer\n" );
    _setGap( 0 );    
}

DecFlowContainer::~DecFlowContainer() {

}

DecFlowContainer &Flow() {
    DecFlowContainer *w = new DecFlowContainer();
    return *w;    
}

void DecFlowContainer::_setGap( int g ) {
    _gap = g;
    _minWidth = _padding[1] + _padding[3];
    _minHeight = _padding[0] + _padding[2];    
}

void DecFlowContainer::_updateChildrenUI( int level ) {
    int cx = _padding[3];
    int cy = _padding[0];
    int maxH = 0;
    for( int i=0; i<_children.size(); i++ ) {
        DecWidget *w = _children[i];
        //w->align( ALIGN_TOPLEFT );
        //w->x(0);
        //w->y(0);
        //w->align( ALIGN_ABSOLUTE );
        w->updateUI( level ); // get children width and height
        if( (maxH != 0) && 
            ((w->_ow != -1) || (cx + _minWidth + _gap > width()) ) && 
            cx + w->width() + _padding[1] > width() ) {
            cx = _padding[3];
            cy += maxH + _gap;
            maxH = 0;
        }

        if( w->height() > maxH ) {
            maxH = w->height();
        }
        w->x(cx);
        w->y(cy);
        w->updateUI( level );
        cx += w->width() + _gap;
    }
}

