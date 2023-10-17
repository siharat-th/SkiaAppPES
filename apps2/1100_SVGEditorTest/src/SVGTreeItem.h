#ifndef __SVG_TREEITEM_H__
#define __SVG_TREEITEM_H__

#include "DeclarativeUI.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "modules/svg/include/SkSVGText.h"
#include "modules/svg/include/SkSVGG.h"
#include "modules/svg/include/SkSVGRenderContext.h"

class DecSVGTreeItem : public DecTreeItem {
public:    
    DecSVGTreeItem( const char *text ); 
    virtual ~DecSVGTreeItem();

    SkSVGTag svgTag;
};

DecSVGTreeItem &SVGTreeItem( const char *text );

#endif //__SVG_TREEITEM_H__