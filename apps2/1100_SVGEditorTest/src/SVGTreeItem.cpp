#include "SVGTreeItem.h"


DecSVGTreeItem::DecSVGTreeItem( const char *text ) :
    DecTreeItem(text) {
}

DecSVGTreeItem::~DecSVGTreeItem() {
}

DecSVGTreeItem &SVGTreeItem( const char *text ) {
    DecSVGTreeItem *w = new DecSVGTreeItem(text);
    return *w;      
}