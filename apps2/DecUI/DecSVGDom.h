#ifndef __DEC_SVG_DOM_H__
#define __DEC_SVG_DOM_H__

#include "DecWidget.h"
#include <functional>

class SkSVGDOM;

class DecSVGDom : public DecWidget {
public:    
    DecSVGDom( const char *uri, std::function<void(DecSVGDom &dom)> onload=nullptr  );
    virtual ~DecSVGDom() override;

    virtual std::string type() override { return "SVGDom"; }

    bool loadFromResources(const char* filename);
    bool load(const char* filename, uintptr_t /* uint8_t*  */ iptr = 0, size_t length = 0);
    virtual void onResized( int w, int h ) override;    

    virtual void onLoad() { dirty(true); }

    virtual bool onDropFile(int ix, int iy, const char* dropped_file, uintptr_t /* uint8_t*  */ iptr = 0, size_t length = 0) override;

    SkString  fPath;
    sk_sp<SkSVGDOM> fDom;
    SkSize fWinSize = SkSize::MakeEmpty();

    virtual void calcSVGScale();

    bool _svgContentSize;

    float _svgScaleX;
    float _svgScaleY;     

    float _svgDPIX;
    float _svgDPIY;        

    float _svgWidth;
    float _svgHeight;

    float _svgWidth72;
    float _svgHeight72; 

    std::function<void(DecSVGDom &dom)> _onLoad;

    SkPixmap getPixmap( float dpi, std::function<void(SkCanvas &canvas)>hook=nullptr );
    sk_sp<SkData> getPNGData( float dpi );
    sk_sp<SkData> getSVGData();
    sk_sp<SkData> getSVGRenderData();


    void saveSVG( const char *filename );
    void exportSVG( const char *filename );
    void exportPNG( const char *filename, float dpi, std::function<void(SkCanvas &canvas)>hook=nullptr  );

protected:

    virtual void _draw_content(SkCanvas &canvas) override;   
};

DecSVGDom &SVGDom( const char *uri, std::function<void(DecSVGDom &dom)> onload=nullptr );

#endif //__DEC_SVG_DOM_H__