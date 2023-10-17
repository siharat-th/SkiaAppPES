#ifndef __SVGDOMEDITOR_LOADER_H__
#define __SVGDOMEDITOR_LOADER_H__

#include "include/docs/SkPDFDocument.h"
#include "include/core/SkStream.h"
#include "DeclarativeUI.h"

//where createSVGEditor ?
//extern DecWidget &createSVGEditor(DecSVGDom **pSVGDom);

#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"

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

    virtual void calcSVGScale() override;
    virtual void onLoad() override;

    void selectNode( SkSVGNode *pNode );
    SkSVGNode* selectNodeByPosition(float x, float y, float scale, SkSVGNode* pSelectedNode = NULL);
    void translateNode(float dx, float dy, float scale, SkSVGNode* pNode);

    DecSVGEditor *pSVGEditor;

    DecImage *pImageLogo;

    float _svgViewScale;
    bool _svgViewMoving;
    int _svgViewLastMouseX;
    int _svgViewLastMouseY;
    int _svgViewPosnX;
    int _svgViewPosnY;

    bool _svgNodeMoving;

    bool _bMoveContentToCenter;

    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) override;
    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey mod) override;
    virtual bool onMouseWheel(int x, int y, float deltax, float delta, skui::ModifierKey mod) override;    

    void saveSVG(const char* filename);
    void exportSVG(const char* filename);
    void exportPNG( const char *filename, float dpi, std::function<void(SkCanvas &canvas)>hook=nullptr  );
    void startExportPDF( const char *filename );
    void endExportPDF();

    void renderPDF( const char *patternName, bool tiled, std::function<void(SkCanvas &canvas, int w, int h)>hook=nullptr );
    void renderTiledPDF(const char *patternName, std::function<void(SkCanvas &canvas, int w, int h)>hook=nullptr);

    //SkFILEWStream * _pdfStream;

    std::string _pdfFilename;
    SkDynamicMemoryWStream * _pdfStream;
    sk_sp<SkDocument> _pdfDoc;

    std::function<void(DecSVGDomEditor& svgDomEditor)> _onSelectChanged;
    DecWidget &onSelectChanged(std::function<void(DecSVGDomEditor &svgDomEditor)> callback);

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

    SkSVGNode* pSelectedNode;
    SkSVGNode* pHoverNode;
    SkSVGNode* pPressedNode;
    SkSVGNode* pDragNode;
    // SkPath *pSelectedNodePath; 

    SkScalar _dpi;
    bool _bEnableMouseHoverMode;

protected:  
    virtual void _draw_content(SkCanvas &canvas) override;   

    void setDrawParametersFit();
    void setDrawParametersPaper();

    std::function<void(DecSVGDomEditor *,SkCanvas &canvas)> _draw_hook;
    std::function<void(DecSVGDomEditor *,SkCanvas &canvas)> _predraw_hook;

    SkSVGSVG *root;  
};

DecSVGDomEditor &SVGDomEditor( const char *uri=NULL );

// never usage, mock SkSVGNode like as source code is maked by #defind ..., just review svgnode properties.
//class svgnode : SkRefCnt {
//public:
//    SkSVGTag fTag;
//
//    // FIXME: this should be sparse
//    SkSVGPresentationAttributes fPresentationAttributes;
//
//    using INHERITED = SkRefCnt;
//
//private:
//    bool setClipRule(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGFillRule, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setClipRule(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGFillRule, true>& getClipRule() const {
//        return fPresentationAttributes.fClipRule;
//    }
//    void setClipRule(const SkSVGProperty<SkSVGFillRule, true>& v) {
//        auto* dest = &fPresentationAttributes.fClipRule;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setClipRule(SkSVGProperty<SkSVGFillRule, true>&& v) {
//        auto* dest = &fPresentationAttributes.fClipRule;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setColor(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGColorType, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setColor(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGColorType, true>& getColor() const {
//        return fPresentationAttributes.fColor;
//    }
//    void setColor(const SkSVGProperty<SkSVGColorType, true>& v) {
//        auto* dest = &fPresentationAttributes.fColor;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setColor(SkSVGProperty<SkSVGColorType, true>&& v) {
//        auto* dest = &fPresentationAttributes.fColor;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setColorInterpolation(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGColorspace, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setColorInterpolation(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGColorspace, true>& getColorInterpolation() const {
//        return fPresentationAttributes.fColorInterpolation;
//    }
//    void setColorInterpolation(const SkSVGProperty<SkSVGColorspace, true>& v) {
//        auto* dest = &fPresentationAttributes.fColorInterpolation;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setColorInterpolation(SkSVGProperty<SkSVGColorspace, true>&& v) {
//        auto* dest = &fPresentationAttributes.fColorInterpolation;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setColorInterpolationFilters(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGColorspace, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setColorInterpolationFilters(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGColorspace, true>& getColorInterpolationFilters() const {
//        return fPresentationAttributes.fColorInterpolationFilters;
//    }
//    void setColorInterpolationFilters(const SkSVGProperty<SkSVGColorspace, true>& v) {
//        auto* dest = &fPresentationAttributes.fColorInterpolationFilters;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setColorInterpolationFilters(SkSVGProperty<SkSVGColorspace, true>&& v) {
//        auto* dest = &fPresentationAttributes.fColorInterpolationFilters;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setFillRule(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGFillRule, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setFillRule(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGFillRule, true>& getFillRule() const {
//        return fPresentationAttributes.fFillRule;
//    }
//    void setFillRule(const SkSVGProperty<SkSVGFillRule, true>& v) {
//        auto* dest = &fPresentationAttributes.fFillRule;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setFillRule(SkSVGProperty<SkSVGFillRule, true>&& v) {
//        auto* dest = &fPresentationAttributes.fFillRule;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setFill(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGPaint, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setFill(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGPaint, true>& getFill() const { return fPresentationAttributes.fFill; }
//    void setFill(const SkSVGProperty<SkSVGPaint, true>& v) {
//        auto* dest = &fPresentationAttributes.fFill;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setFill(SkSVGProperty<SkSVGPaint, true>&& v) {
//        auto* dest = &fPresentationAttributes.fFill;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setFillOpacity(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGNumberType, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setFillOpacity(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGNumberType, true>& getFillOpacity() const {
//        return fPresentationAttributes.fFillOpacity;
//    }
//    void setFillOpacity(const SkSVGProperty<SkSVGNumberType, true>& v) {
//        auto* dest = &fPresentationAttributes.fFillOpacity;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setFillOpacity(SkSVGProperty<SkSVGNumberType, true>&& v) {
//        auto* dest = &fPresentationAttributes.fFillOpacity;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setFontFamily(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGFontFamily, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setFontFamily(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGFontFamily, true>& getFontFamily() const {
//        return fPresentationAttributes.fFontFamily;
//    }
//    void setFontFamily(const SkSVGProperty<SkSVGFontFamily, true>& v) {
//        auto* dest = &fPresentationAttributes.fFontFamily;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setFontFamily(SkSVGProperty<SkSVGFontFamily, true>&& v) {
//        auto* dest = &fPresentationAttributes.fFontFamily;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setFontSize(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGFontSize, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setFontSize(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGFontSize, true>& getFontSize() const {
//        return fPresentationAttributes.fFontSize;
//    }
//    void setFontSize(const SkSVGProperty<SkSVGFontSize, true>& v) {
//        auto* dest = &fPresentationAttributes.fFontSize;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setFontSize(SkSVGProperty<SkSVGFontSize, true>&& v) {
//        auto* dest = &fPresentationAttributes.fFontSize;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setFontStyle(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGFontStyle, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setFontStyle(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGFontStyle, true>& getFontStyle() const {
//        return fPresentationAttributes.fFontStyle;
//    }
//    void setFontStyle(const SkSVGProperty<SkSVGFontStyle, true>& v) {
//        auto* dest = &fPresentationAttributes.fFontStyle;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setFontStyle(SkSVGProperty<SkSVGFontStyle, true>&& v) {
//        auto* dest = &fPresentationAttributes.fFontStyle;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setFontWeight(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGFontWeight, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setFontWeight(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGFontWeight, true>& getFontWeight() const {
//        return fPresentationAttributes.fFontWeight;
//    }
//    void setFontWeight(const SkSVGProperty<SkSVGFontWeight, true>& v) {
//        auto* dest = &fPresentationAttributes.fFontWeight;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setFontWeight(SkSVGProperty<SkSVGFontWeight, true>&& v) {
//        auto* dest = &fPresentationAttributes.fFontWeight;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setStroke(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGPaint, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setStroke(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGPaint, true>& getStroke() const {
//        return fPresentationAttributes.fStroke;
//    }
//    void setStroke(const SkSVGProperty<SkSVGPaint, true>& v) {
//        auto* dest = &fPresentationAttributes.fStroke;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setStroke(SkSVGProperty<SkSVGPaint, true>&& v) {
//        auto* dest = &fPresentationAttributes.fStroke;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setStrokeDashArray(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGDashArray, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setStrokeDashArray(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGDashArray, true>& getStrokeDashArray() const {
//        return fPresentationAttributes.fStrokeDashArray;
//    }
//    void setStrokeDashArray(const SkSVGProperty<SkSVGDashArray, true>& v) {
//        auto* dest = &fPresentationAttributes.fStrokeDashArray;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setStrokeDashArray(SkSVGProperty<SkSVGDashArray, true>&& v) {
//        auto* dest = &fPresentationAttributes.fStrokeDashArray;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setStrokeDashOffset(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGLength, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setStrokeDashOffset(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGLength, true>& getStrokeDashOffset() const {
//        return fPresentationAttributes.fStrokeDashOffset;
//    }
//    void setStrokeDashOffset(const SkSVGProperty<SkSVGLength, true>& v) {
//        auto* dest = &fPresentationAttributes.fStrokeDashOffset;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setStrokeDashOffset(SkSVGProperty<SkSVGLength, true>&& v) {
//        auto* dest = &fPresentationAttributes.fStrokeDashOffset;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setStrokeLineCap(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGLineCap, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setStrokeLineCap(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGLineCap, true>& getStrokeLineCap() const {
//        return fPresentationAttributes.fStrokeLineCap;
//    }
//    void setStrokeLineCap(const SkSVGProperty<SkSVGLineCap, true>& v) {
//        auto* dest = &fPresentationAttributes.fStrokeLineCap;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setStrokeLineCap(SkSVGProperty<SkSVGLineCap, true>&& v) {
//        auto* dest = &fPresentationAttributes.fStrokeLineCap;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setStrokeLineJoin(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGLineJoin, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setStrokeLineJoin(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGLineJoin, true>& getStrokeLineJoin() const {
//        return fPresentationAttributes.fStrokeLineJoin;
//    }
//    void setStrokeLineJoin(const SkSVGProperty<SkSVGLineJoin, true>& v) {
//        auto* dest = &fPresentationAttributes.fStrokeLineJoin;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setStrokeLineJoin(SkSVGProperty<SkSVGLineJoin, true>&& v) {
//        auto* dest = &fPresentationAttributes.fStrokeLineJoin;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setStrokeMiterLimit(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGNumberType, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setStrokeMiterLimit(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGNumberType, true>& getStrokeMiterLimit() const {
//        return fPresentationAttributes.fStrokeMiterLimit;
//    }
//    void setStrokeMiterLimit(const SkSVGProperty<SkSVGNumberType, true>& v) {
//        auto* dest = &fPresentationAttributes.fStrokeMiterLimit;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setStrokeMiterLimit(SkSVGProperty<SkSVGNumberType, true>&& v) {
//        auto* dest = &fPresentationAttributes.fStrokeMiterLimit;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setStrokeOpacity(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGNumberType, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setStrokeOpacity(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGNumberType, true>& getStrokeOpacity() const {
//        return fPresentationAttributes.fStrokeOpacity;
//    }
//    void setStrokeOpacity(const SkSVGProperty<SkSVGNumberType, true>& v) {
//        auto* dest = &fPresentationAttributes.fStrokeOpacity;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setStrokeOpacity(SkSVGProperty<SkSVGNumberType, true>&& v) {
//        auto* dest = &fPresentationAttributes.fStrokeOpacity;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setStrokeWidth(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGLength, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setStrokeWidth(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGLength, true>& getStrokeWidth() const {
//        return fPresentationAttributes.fStrokeWidth;
//    }
//    void setStrokeWidth(const SkSVGProperty<SkSVGLength, true>& v) {
//        auto* dest = &fPresentationAttributes.fStrokeWidth;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setStrokeWidth(SkSVGProperty<SkSVGLength, true>&& v) {
//        auto* dest = &fPresentationAttributes.fStrokeWidth;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setTextAnchor(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGTextAnchor, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setTextAnchor(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGTextAnchor, true>& getTextAnchor() const {
//        return fPresentationAttributes.fTextAnchor;
//    }
//    void setTextAnchor(const SkSVGProperty<SkSVGTextAnchor, true>& v) {
//        auto* dest = &fPresentationAttributes.fTextAnchor;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setTextAnchor(SkSVGProperty<SkSVGTextAnchor, true>&& v) {
//        auto* dest = &fPresentationAttributes.fTextAnchor;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setVisibility(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGVisibility, true>>&& pr) {
//        if (pr.isValid()) {
//            this->setVisibility(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGVisibility, true>& getVisibility() const {
//        return fPresentationAttributes.fVisibility;
//    }
//    void setVisibility(const SkSVGProperty<SkSVGVisibility, true>& v) {
//        auto* dest = &fPresentationAttributes.fVisibility;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setVisibility(SkSVGProperty<SkSVGVisibility, true>&& v) {
//        auto* dest = &fPresentationAttributes.fVisibility;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setClipPath(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGFuncIRI, false>>&& pr) {
//        if (pr.isValid()) {
//            this->setClipPath(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGFuncIRI, false>& getClipPath() const {
//        return fPresentationAttributes.fClipPath;
//    }
//    void setClipPath(const SkSVGProperty<SkSVGFuncIRI, false>& v) {
//        auto* dest = &fPresentationAttributes.fClipPath;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setClipPath(SkSVGProperty<SkSVGFuncIRI, false>&& v) {
//        auto* dest = &fPresentationAttributes.fClipPath;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setDisplay(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGDisplay, false>>&& pr) {
//        if (pr.isValid()) {
//            this->setDisplay(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGDisplay, false>& getDisplay() const {
//        return fPresentationAttributes.fDisplay;
//    }
//    void setDisplay(const SkSVGProperty<SkSVGDisplay, false>& v) {
//        auto* dest = &fPresentationAttributes.fDisplay;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setDisplay(SkSVGProperty<SkSVGDisplay, false>&& v) {
//        auto* dest = &fPresentationAttributes.fDisplay;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setMask(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGFuncIRI, false>>&& pr) {
//        if (pr.isValid()) {
//            this->setMask(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGFuncIRI, false>& getMask() const {
//        return fPresentationAttributes.fMask;
//    }
//    void setMask(const SkSVGProperty<SkSVGFuncIRI, false>& v) {
//        auto* dest = &fPresentationAttributes.fMask;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setMask(SkSVGProperty<SkSVGFuncIRI, false>&& v) {
//        auto* dest = &fPresentationAttributes.fMask;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setFilter(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGFuncIRI, false>>&& pr) {
//        if (pr.isValid()) {
//            this->setFilter(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGFuncIRI, false>& getFilter() const {
//        return fPresentationAttributes.fFilter;
//    }
//    void setFilter(const SkSVGProperty<SkSVGFuncIRI, false>& v) {
//        auto* dest = &fPresentationAttributes.fFilter;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setFilter(SkSVGProperty<SkSVGFuncIRI, false>&& v) {
//        auto* dest = &fPresentationAttributes.fFilter;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setOpacity(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGNumberType, false>>&& pr) {
//        if (pr.isValid()) {
//            this->setOpacity(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGNumberType, false>& getOpacity() const {
//        return fPresentationAttributes.fOpacity;
//    }
//    void setOpacity(const SkSVGProperty<SkSVGNumberType, false>& v) {
//        auto* dest = &fPresentationAttributes.fOpacity;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setOpacity(SkSVGProperty<SkSVGNumberType, false>&& v) {
//        auto* dest = &fPresentationAttributes.fOpacity;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setStopColor(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGColor, false>>&& pr) {
//        if (pr.isValid()) {
//            this->setStopColor(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGColor, false>& getStopColor() const {
//        return fPresentationAttributes.fStopColor;
//    }
//    void setStopColor(const SkSVGProperty<SkSVGColor, false>& v) {
//        auto* dest = &fPresentationAttributes.fStopColor;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setStopColor(SkSVGProperty<SkSVGColor, false>&& v) {
//        auto* dest = &fPresentationAttributes.fStopColor;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setStopOpacity(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGNumberType, false>>&& pr) {
//        if (pr.isValid()) {
//            this->setStopOpacity(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGNumberType, false>& getStopOpacity() const {
//        return fPresentationAttributes.fStopOpacity;
//    }
//    void setStopOpacity(const SkSVGProperty<SkSVGNumberType, false>& v) {
//        auto* dest = &fPresentationAttributes.fStopOpacity;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setStopOpacity(SkSVGProperty<SkSVGNumberType, false>&& v) {
//        auto* dest = &fPresentationAttributes.fStopOpacity;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setFloodColor(SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGColor, false>>&& pr) {
//        if (pr.isValid()) {
//            this->setFloodColor(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGColor, false>& getFloodColor() const {
//        return fPresentationAttributes.fFloodColor;
//    }
//    void setFloodColor(const SkSVGProperty<SkSVGColor, false>& v) {
//        auto* dest = &fPresentationAttributes.fFloodColor;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setFloodColor(SkSVGProperty<SkSVGColor, false>&& v) {
//        auto* dest = &fPresentationAttributes.fFloodColor;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setFloodOpacity(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGNumberType, false>>&& pr) {
//        if (pr.isValid()) {
//            this->setFloodOpacity(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGNumberType, false>& getFloodOpacity() const {
//        return fPresentationAttributes.fFloodOpacity;
//    }
//    void setFloodOpacity(const SkSVGProperty<SkSVGNumberType, false>& v) {
//        auto* dest = &fPresentationAttributes.fFloodOpacity;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setFloodOpacity(SkSVGProperty<SkSVGNumberType, false>&& v) {
//        auto* dest = &fPresentationAttributes.fFloodOpacity;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setLightingColor(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGColor, false>>&& pr) {
//        if (pr.isValid()) {
//            this->setLightingColor(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGColor, false>& getLightingColor() const {
//        return fPresentationAttributes.fLightingColor;
//    }
//    void setLightingColor(const SkSVGProperty<SkSVGColor, false>& v) {
//        auto* dest = &fPresentationAttributes.fLightingColor;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setLightingColor(SkSVGProperty<SkSVGColor, false>&& v) {
//        auto* dest = &fPresentationAttributes.fLightingColor;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//
//private:
//    bool setDataName(
//            SkSVGAttributeParser::ParseResult<SkSVGProperty<SkSVGStringType, false>>&& pr) {
//        if (pr.isValid()) {
//            this->setDataName(std::move(*pr));
//        }
//        return pr.isValid();
//    }
//
//public:
//    const SkSVGProperty<SkSVGStringType, false>& getDataName() const {
//        return fPresentationAttributes.fDataName;
//    }
//    void setDataName(const SkSVGProperty<SkSVGStringType, false>& v) {
//        auto* dest = &fPresentationAttributes.fDataName;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = v;
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//    void setDataName(SkSVGProperty<SkSVGStringType, false>&& v) {
//        auto* dest = &fPresentationAttributes.fDataName;
//        if (!dest->isInheritable() || v.isValue()) {
//            *dest = std::move(v);
//        } else {
//            dest->set(SkSVGPropertyState::kInherit);
//        }
//    }
//};

#endif //__SVGDOMEDITOR_LOADER_H__