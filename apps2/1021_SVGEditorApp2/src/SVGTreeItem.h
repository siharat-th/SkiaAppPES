#ifndef __SVG_TREEITEM_H__
#define __SVG_TREEITEM_H__

#include "DeclarativeUI.h"

class DecSVGTreeItem : public DecTreeItem {
public:    
    DecSVGTreeItem( const char *text ); 
    virtual ~DecSVGTreeItem();
};

DecSVGTreeItem &SVGTreeItem( const char *text );

#endif //__SVG_TREEITEM_H__