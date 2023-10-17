#ifndef __SVGDOMEDITOR_LOADER_H__
#define __SVGDOMEDITOR_LOADER_H__

#include "include/docs/SkPDFDocument.h"
#include "include/core/SkStream.h"
#include "DeclarativeUI.h"

//where createSVGEditor ?
//extern DecWidget &createSVGEditor(DecSVGDom **pSVGDom);

class SkSVGSVG;
class DecSVGEditor;

class DecSVGDomEditor : public DecSVGDom {
public:    
    DecSVGDomEditor( const char *uri=NULL );
    virtual ~DecSVGDomEditor() override;

    virtual std::string type() override { return "DecSVGDomEditor"; }

    SkSize fWigetSize = SkSize::MakeEmpty();
    //virtual void onResized(int w, int h) override;
    virtual void update() override;
    virtual void adjustWidth(int w) override;
    virtual void adjustHeight(int h) override;

    virtual void onLoad() override;

    void selectNode( SkSVGNode *pNode );

    DecSVGEditor *pSVGEditor;

    DecImage *pImageLogo;

    float _svgViewScale;
    bool _svgViewMoving;
    int _svgViewLastMouseX;
    int _svgViewLastMouseY;
    int _svgViewPosnX;
    int _svgViewPosnY;

    bool _bMoveContentToCenter;

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    virtual bool onMouseWheel(int x, int y, float deltax, float delta, skui::ModifierKey) override;    

    void exportSVG( const char *filename, std::function<void(SkCanvas &canvas)>hook=nullptr );
    void exportPNG( const char *filename, float dpi, std::function<void(SkCanvas &canvas)>hook=nullptr  );
    void startExportPDF( const char *filename );
    void endExportPDF();

    void renderPDF( const char *patternName, bool tiled, std::function<void(SkCanvas &canvas, int w, int h)>hook=nullptr );
    void renderTiledPDF(const char *patternName, std::function<void(SkCanvas &canvas, int w, int h)>hook=nullptr);

    //SkFILEWStream * _pdfStream;

    std::string _pdfFilename;
    SkDynamicMemoryWStream * _pdfStream;
    sk_sp<SkDocument> _pdfDoc;

    bool drawFit;

    float drawScale_DPI;

    float drawCenterOffsetX_DPI;
    float drawCenterOffsetY_DPI;

    float drawPaperW_DPI;
    float drawPaperH_DPI;

    float contentCenterOffsetX_72;
    float contentCenterOffsetY_72;

    std::string paperSizeName;
    float paperWidth_mm;
    float paperHeight_mm;

    std::string minPaperSizeName;
    float minPaperWidth_mm;
    float minPaperHeight_mm;

    void setDrawParameters();
    void setPaperSize( const char *paper );    
    void setMinPaperSize( );
    void setVisibleContentBounds();

    bool isNodeVisible( SkSVGNode *node );

    DecSVGDomEditor &drawHook( std::function<void(DecSVGDomEditor *, SkCanvas &canvas)>hook );
    DecSVGDomEditor &preDrawHook( std::function<void(DecSVGDomEditor *, SkCanvas &canvas)>hook );

    SkRect _visibleContentBounds;
    bool _showMinPaperBounds;
    bool _showVisibleContentBounds;
    bool _showTileGrid;

    float _tilePaperW;
    float _tilePaperH;
    
    float _tileX;
    float _tileY;
    float _tileW;
    float _tileH;

    float _tileTop;
    float _tileRight;
    float _tileBottom;
    float _tileLeft;

    int _numTileX;
    int _numTileY;

    void setTile( const char *paper, float top=10, float right=15, float bottom=15, float left=10 ); // border in mm
    void calculateTile();

    SkSVGNode *pSelectedNode;   
    SkPath *pSelectedNodePath; 

protected:  
    virtual void _draw_content(SkCanvas &canvas) override;   

    void setDrawParametersFit();
    void setDrawParametersPaper();

    std::function<void(DecSVGDomEditor *,SkCanvas &canvas)> _draw_hook;
    std::function<void(DecSVGDomEditor *,SkCanvas &canvas)> _predraw_hook;

    SkSVGSVG *root;  
};

DecSVGDomEditor &SVGDomEditor( const char *uri=NULL );

#endif //__SVGDOMEDITOR_LOADER_H__