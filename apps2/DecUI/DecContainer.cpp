#include "DecContainer.h"
#include "DecUI.h"

DecContainer::DecContainer() {
    //SkDebugf( "DecContainer\n" );

    x(0);
    y(0);
    width(-1);
    height(-1);
    //align( ALIGN_ABSOLUTE );
}

DecContainer::~DecContainer() {

}

DecContainer &Container() {
    DecContainer *w = new DecContainer();
    return *w;    
}

void DecContainer::_draw_content(SkCanvas &canvas) {
}

