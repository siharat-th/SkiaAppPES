#ifndef __MAGICPATTERN_SVG_H__
#define __MAGICPATTERN_SVG_H__

#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "DeclarativeUI.h"

class DecMagicPatternSVG : public DecSVGDom {
public:    
    DecMagicPatternSVG( const char *uri=NULL, std::function<void(DecSVGDom &dom)> onload=nullptr );
    virtual ~DecMagicPatternSVG() override;

    void _draw_content(SkCanvas &canvas) override;
    void exportPNG( int pid, const char *filename, float dpi, float scale, std::function<void(SkCanvas &canvas)>hook=nullptr );
    void exportSVG(const char* filename);

    SkSVGSVG *root; 

    DecImage *imgLogo;

    float fScale;
};

DecMagicPatternSVG &MagicPatternSVG( const char *uri=NULL, std::function<void(DecSVGDom &dom)> onload=nullptr );

#endif //__MAGICPATTERN_SVG_H__