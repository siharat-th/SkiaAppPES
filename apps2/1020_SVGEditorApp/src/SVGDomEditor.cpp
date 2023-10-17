#include "DeclarativeUI.h"
#include "DecSVGDom.h"
#include "DecUI.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "include/core/SkSamplingOptions.h"
#include "include/docs/SkPDFDocument.h"
#include "include/encode/SkPngEncoder.h"
#include "include/core/SkStream.h"
#include "src/utils/SkOSPath.h"
#include "modules/skresources/include/SkResources.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "modules/svg/include/SkSVGRenderContext.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkFontMetrics.h"
#include "SVGDomEditor.h"
#include <SVGEditor.h>
#include "include/svg/SkSVGCanvas.h"
//#include <src/xml/SkXMLWriter.h>
//#include <src/svg/SkSVGDevice.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


const int _paperWidth[] = { 52, 74, 105, 148, 210, 297, 420, 594, 841, 1188 };
const int _paperHeight[] = { 74, 105, 148, 210, 297, 420, 594, 841, 1188, 841 };
const char *_paperName[] = { "A8", "A7", "A6", "A5", "A4", "A3", "A2", "A1", "A0", "A0L", "n/a" };
const int _numPaperType = 10;

DecSVGDomEditor::DecSVGDomEditor( const char *uri ) :
    DecSVGDom( NULL ) {
    _testDraw = false;
    pImageLogo = new DecImage();
    pImageLogo->load( "images/MP-LOGO-C.png" );

    pSVGEditor = NULL;

    pSelectedNode = NULL;

    drawFit = false;

    contentCenterOffsetX_72 = 0;
    contentCenterOffsetY_72 = 0;

    setPaperSize( "A1" );

    minPaperWidth_mm = 0;
    minPaperHeight_mm = 0;  

    _draw_hook = NULL;  
    _predraw_hook = NULL;

    _showMinPaperBounds = false;
    _showVisibleContentBounds = false;   
    _showTileGrid = false; 

    _tileW = 0;
    _tileH = 0;
    _numTileX = 0;
    _numTileY = 0;

    _tileTop = 0;
    _tileRight = 0;
    _tileBottom = 0;
    _tileLeft = 0;

    _pdfStream = nullptr;
    _pdfDoc = nullptr;

    _svgViewScale = 1.0;
    _svgViewMoving = false;
    _svgViewPosnX = 0;
    _svgViewPosnY = 0;

    _bMoveContentToCenter = true;
    
    size( -1, -1 );
    border(false);
    bg_color(0xFFE0E0E0);

    pSelectedNodePath = NULL;

    _visibleContentBounds.setEmpty();
    
    if( uri ) {
        load( uri );
    }
}

DecSVGDomEditor::~DecSVGDomEditor() {

}

void DecSVGDomEditor::update() {
    //SkDebugf("DecSVGDomEditor::update: parent-size:[%d, %d]\n",
    //         this->_parent->width(),
    //         this->_parent->height());
    static int snParentHeight = this->_parent->height();
    int nParentHeight = this->_parent->height();
    if (snParentHeight != nParentHeight) {
        snParentHeight = nParentHeight;
        dirty(true, true);
    }
}

void DecSVGDomEditor::adjustWidth(int w) {
    if (_ow == 0 || _ow == -1) {
        // fixed: cache & dirty
        if (fWigetSize.fWidth > 0) {
            // fixed: cache & dirty
            _width = fWigetSize.fWidth;
        } else {
            fWigetSize.fWidth = _width;
        }
        if (w != _width) {
            dirty(true, true);
            _width = w;
            // fixed: cache & dirty
            fWigetSize.fWidth = w;
        }
        if (_width > _maxWidth) {
            _width = _maxWidth;
            // fixed: cache & dirty
            fWigetSize.fWidth = _maxWidth;
        }
        _updateChildrenUI(0);
        _updateChildrenAlignment();
    }
}

void DecSVGDomEditor::adjustHeight(int h) {
    if (_oh == 0 || _oh == -1) {
        if (fWigetSize.fHeight > 0) {
            // fixed: cache & dirty
            _height = fWigetSize.fHeight;
        } else {
            fWigetSize.fHeight = _height;
        }
        if (h != _height) {
            dirty(true, true);
            _height = h;
            // fixed: cache & dirty
            fWigetSize.fHeight = h;
        }
        if (_height > _maxHeight) {
            _height = _maxHeight;
            // fixed: cache & dirty
            fWigetSize.fHeight = _maxHeight;
        }
        _updateChildrenUI(0);
        _updateChildrenAlignment();
    }
}


bool DecSVGDomEditor::onMouse(int x, int y, skui::InputState state, skui::ModifierKey modifier ) {
    if( state == skui::InputState::kDown ) {
        SkDebugf( "Mouse mod: %i\n", (int)modifier );
        if( ((int)modifier & (int)skui::ModifierKey::kCommand) == (int)skui::ModifierKey::kCommand ||
            ((int)modifier & (int)skui::ModifierKey::kControl) == (int)skui::ModifierKey::kControl ) {
            if( _svgViewScale == 1.0 ) {
                _svgViewScale = 2.0;

                int cx = width() / 2;
                int cy = height() / 2;

                _svgViewPosnX = cx - (x * _svgViewScale);
                _svgViewPosnY = cy - (y * _svgViewScale);
                dirty( true );
            }
            else {
                _svgViewScale = 1.0;
                _svgViewPosnX = 0;
                _svgViewPosnY = 0;
                dirty( true );
            }
            //SkDebugf( "svgScale: %0.2f\n", _svgViewScale );
        }
        else {
            _svgViewMoving = true;
            _svgViewLastMouseX = x;
            _svgViewLastMouseY = y;            
        }
    }
    else if( state == skui::InputState::kMove ) {
        if( _svgViewMoving ) {
            _svgViewPosnX += (x-_svgViewLastMouseX);
            _svgViewPosnY += (y-_svgViewLastMouseY);

            _svgViewLastMouseX = x;
            _svgViewLastMouseY = y;
            dirty( true );
        }

    }
    else if( state == skui::InputState::kUp ) {
        _svgViewMoving = false;
    }

    return false;
}

bool DecSVGDomEditor::onMouseWheel(int x, int y, float deltax, float delta, skui::ModifierKey) {
    if( _svgViewScale > 1.0 ) {
        _svgViewPosnX += deltax * _svgViewScale;
        _svgViewPosnY += delta * _svgViewScale;
        dirty( true );
    }
    return false;
}

bool DecSVGDomEditor::isNodeVisible( SkSVGNode *node ) {
    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;

    const SkSVGProperty<SkSVGVisibility, true> &visibility = node->getVisibility();
    if( visibility.isValue() ) {
        vType = visibility->type();
    } 
    return vType == SkSVGVisibility::Type::kVisible ? true : false;
}

void DecSVGDomEditor::setVisibleContentBounds() {
    if( !fDom ) {
        _visibleContentBounds.setEmpty();
        return;
    }
    SkRect rect;
    rect.setEmpty();

    SkSize szC = fDom->containerSize();
    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        szR.fWidth = szC.fWidth;
        szR.fHeight = szC.fHeight;
    }

    SkSVGLengthContext       lctx(szC);
    SkSVGPresentationContext pctx;
    auto surface = SkSurface::MakeRasterN32Premul(100, 100);
    SkCanvas *canvas = surface->getCanvas();
    SkSVGRenderContext       rctx = SkSVGRenderContext(
                                        canvas, 
                                        fDom->fFontMgr, 
                                        fDom->fResourceProvider, 
                                        fDom->fIDMapper, 
                                        lctx, 
                                        pctx,
                                        {nullptr, nullptr});  

    root = fDom->getRoot();
    if( root->hasChildren() ) {
        int numChildren = (int)root->fChildren.size();
        for( int i=0; i<numChildren; i++ ) {
            sk_sp<SkSVGNode> node = root->fChildren.at( i );
            SkSVGNode *pNode = node.get();
            bool visible = isNodeVisible( pNode );
            if( visible ) {
                SkRect bounds = node->objectBoundingBox( rctx ); 
                rect.join( bounds ); 
            }
        }
    }
    
    if( rect.width() > szR.fWidth || 
        rect.height() > szR.fHeight ) {
        //SkDebugf( "Over size\n" );
        _visibleContentBounds = SkRect::MakeXYWH( 0, 0, szR.fWidth, szR.fHeight );
    }        
    else {
        _visibleContentBounds = rect.makeOutset( _svgDPIX/10, _svgDPIY/10 );
    }    

    float centerContentX = (drawPaperW_DPI / _svgScaleX - _visibleContentBounds.width()) / 2;
    float centerContentY = (drawPaperH_DPI / _svgScaleY - _visibleContentBounds.height()) / 2; 
    contentCenterOffsetY_72 = centerContentY - _visibleContentBounds.fTop;
    contentCenterOffsetX_72 = centerContentX - _visibleContentBounds.fLeft;   

    calculateTile(); 
}

DecSVGDomEditor &DecSVGDomEditor::drawHook( std::function<void(DecSVGDomEditor *, SkCanvas &canvas)>hook ) {
    _draw_hook = hook;
    return *this;
}

DecSVGDomEditor &DecSVGDomEditor::preDrawHook( std::function<void(DecSVGDomEditor *, SkCanvas &canvas)>hook ) {
    _predraw_hook = hook;
    return *this;
}


void DecSVGDomEditor::setMinPaperSize() {
    if( !fDom ) {
        minPaperSizeName = std::string( "n/a" );
        minPaperWidth_mm = 0;
        minPaperHeight_mm = 0;
        return;
    }

    root = fDom->getRoot();
    SkSize szC = fDom->containerSize();
    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        szR.fWidth = szC.fWidth;
        szR.fHeight = szC.fHeight;
    }

    float w = szR.fWidth/72.0;
    float h = szR.fHeight/72.0;

    int fW = (int)(w * 25.4);
    int fH = (int)(h * 25.4);
    //if( fW > fH ) { // landscape
    //    fW = (int)(h * 25.4);
    //    fH = (int)(w * 25.4);
    //}
    for( int i=0; i<_numPaperType; i++ ) {
        if( _paperWidth[i] >= fW && _paperHeight[i] >= fH ) {
            minPaperSizeName = std::string( _paperName[i] );
            minPaperWidth_mm = _paperWidth[i];
            minPaperHeight_mm = _paperHeight[i];
            return;
        }
    }
}

void DecSVGDomEditor::setTile( const char *paper, float top, float right, float bottom, float left ) {
    for( int i=0; i<_numPaperType; i++ ) {
        if( !strcmp( _paperName[i], paper ) ) {
            _tilePaperW = _paperWidth[i];
            _tilePaperH = _paperHeight[i];

            float w = _paperWidth[i] - (right + left); // page size and border in mm
            float h = _paperHeight[i] - (top + bottom);
            _tileW = (w / 25.4 * 72.0);
            _tileH = (h / 25.4 * 72.0);

            _tileTop = top / 25.4 * 72.0;
            _tileRight = right / 25.4 * 72.0;
            _tileBottom = bottom / 25.4 * 72.0;
            _tileLeft = left / 25.4 * 72.0;

            calculateTile();
            break;
        }
    }
}

void DecSVGDomEditor::calculateTile() {
    float nX = _visibleContentBounds.width() / _tileW;
    float nY = _visibleContentBounds.height() / _tileH;

    _numTileX = (int)nX;
    _numTileY = (int)nY;

    if( _numTileX < nX )
        _numTileX++;
    if( _numTileY < nY )
        _numTileY++;

    float _totalTileW = _tileW * _numTileX;
    float _totalTileH = _tileH * _numTileY;

    _tileX = _visibleContentBounds.x() - (_totalTileW - _visibleContentBounds.width()) / 2.0;
    _tileY = _visibleContentBounds.y() - (_totalTileH - _visibleContentBounds.height()) / 2.0;
}

void DecSVGDomEditor::setPaperSize( const char *paper ) {
    for( int i=0; i<_numPaperType; i++ ) {
        if( !strcmp( _paperName[i], paper ) ) {
            paperSizeName = std::string( _paperName[i] );
            paperWidth_mm = _paperWidth[i];
            paperHeight_mm = _paperHeight[i];

            setDrawParameters();
            setMinPaperSize();
            setVisibleContentBounds();                     

            return;
        }
    }
    if( !fDom ) {
        paperWidth_mm = 0;
        paperHeight_mm = 0;
        return;
    }

    root = fDom->getRoot();
    SkSize szC = fDom->containerSize();
    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        szR.fWidth = szC.fWidth;
        szR.fHeight = szC.fHeight;
    }
    paperSizeName = std::string(paper);
    paperWidth_mm = szR.fWidth / 72 * 25.4;
    paperHeight_mm = szR.fHeight / 72 * 25.4;

    setDrawParameters();
    setMinPaperSize();
    setVisibleContentBounds();                  
}


void DecSVGDomEditor::selectNode( SkSVGNode *pNode ){
    if( pNode == pSelectedNode ) {
        return;
    }
    dirty();
    pSelectedNode = pNode;
    if( pSelectedNodePath ) {
        delete( pSelectedNodePath );
    }
    pSelectedNodePath = NULL;
    if( pNode ) {
        SkSVGTag tag = pSelectedNode->tag();
        SkDebugf( "Tag: %i\n", (int) tag );
        if( tag == SkSVGTag::kPath || tag == SkSVGTag::kRect || 
            tag == SkSVGTag::kPolygon || tag == SkSVGTag::kPolyline ) {
            auto surface = SkSurface::MakeRasterN32Premul(100, 100);
            SkCanvas *canvas = surface->getCanvas();
            SkSize szC = fDom->containerSize(); 
            SkSVGLengthContext       lctx(szC);
            SkSVGPresentationContext pctx;
            SkSVGRenderContext       rctx = SkSVGRenderContext(
                                                canvas, 
                                                fDom->fFontMgr, 
                                                fDom->fResourceProvider, 
                                                fDom->fIDMapper, 
                                                lctx, 
                                                pctx,
                                                {nullptr, nullptr});

            SkPath path = pNode->asPath(rctx);
            pSelectedNodePath = new SkPath( path );
            SkDebugf( "Set Selected Path\n" );
        }
    }
}

DecSVGDomEditor &SVGDomEditor(const char *uri ) {
    DecSVGDomEditor *w = new DecSVGDomEditor(uri);
    return *w;    
}


void DecSVGDomEditor::onLoad() {
    //SkDebugf( "DecSVGDomEditor::OnLoad\n" );
    root = fDom->getRoot();

    setDrawParameters();
    setMinPaperSize();
    setVisibleContentBounds();
    setTile( "A4", 10, 15, 15, 10 );

    if( pSVGEditor ) {
        pSVGEditor->onLoad();
    }
}

void DecSVGDomEditor::renderTiledPDF(const char *patternName, std::function<void(SkCanvas &canvas, int w, int h )>hook) {
    float _tileX_DPI = _tileX * _svgScaleX;
    float _tileY_DPI = _tileY * _svgScaleY;
    float _tileW_DPI = _tileW * _svgScaleX;
    float _tileH_DPI = _tileH * _svgScaleY;

    float scale = 1.0 / _svgScaleX;

    float a4W = _tilePaperW / 25.4 * 72.0;
    float a4H = _tilePaperH / 25.4 * 72.0;

    for( int y=0; y<_numTileY; y++ ) {
        for( int x=0; x<_numTileX; x++ ) {
            SkCanvas* canvas = _pdfDoc->beginPage(a4W, a4H);

            canvas->clear( SK_ColorWHITE );

            canvas->save();
            SkRect rect = SkRect::MakeXYWH( _tileLeft, _tileTop, _tileW, _tileH );
            canvas->clipRect( rect );

            canvas->scale( scale, scale );
            canvas->translate( 
                (_tileLeft * _svgScaleX)-(_tileX_DPI + (x * _tileW_DPI)), 
                (_tileTop * _svgScaleY)-(_tileY_DPI + (y * _tileH_DPI)) );

            if( hook ) {
                hook( *canvas, a4W, a4H );
            }
            fDom->render( canvas );
            canvas->restore();

            SkPaint paint;
            SkRect rectGlueR = SkRect::MakeXYWH( _tileLeft + _tileW, _tileTop, 5.0 / 25.4 * 72, _tileH );
            paint.setColor( 0xFFE0E0E0 );
            paint.setStyle( SkPaint::Style::kFill_Style );
            if( x < _numTileX-1 ) {
                canvas->drawRect( rectGlueR, paint );
            }

            SkRect rectGlueB = SkRect::MakeXYWH( _tileLeft, _tileTop + _tileH, _tileW, 5.0 / 25.4 * 72 );
            if( y < _numTileY-1 ) {
                canvas->drawRect( rectGlueB, paint );
            }

            SkRect rectGlueC = SkRect::MakeXYWH( _tileLeft + _tileW, _tileTop + _tileH, 
                5.0 / 25.4 * 72, 5.0 / 25.4 * 72 );
            if( x < _numTileX-1 && y < _numTileY-1 ) {
                canvas->drawRect( rectGlueC, paint );
            }

            paint.setColor( SK_ColorGRAY );
            paint.setStrokeWidth( 0.5 * scale );
            paint.setStyle( SkPaint::Style::kStroke_Style );
            canvas->drawRect(  rect, paint );

            paint.setColor( SK_ColorBLACK );
            paint.setStrokeWidth( 1.5 * scale );
            paint.setStyle( SkPaint::Style::kStroke_Style );

            // Left cut line
            if( x > 0 ) {
                if( y == 0 ) {
                    canvas->drawLine( _tileLeft, 0, _tileLeft, a4H, paint );
                }
                else {
                    canvas->drawLine( _tileLeft, _tileTop, _tileLeft, a4H, paint );
                }
            }

            // Top cut line
            if( y > 0 ) {
                if( x == 0 ) {
                    canvas->drawLine( 0, _tileTop, a4W, _tileTop, paint );
                }
                else {
                    canvas->drawLine( _tileLeft, _tileTop, a4W, _tileTop, paint );
                }
            }      

            SkFont font;
            font.setTypeface(_font->typeface(FONT_REGULAR));
            font.setSubpixel(true);
            font.setSize(9);       

            paint.setAntiAlias(true);
            paint.setColor(SK_ColorBLACK);
            paint.setStyle( SkPaint::Style::kFill_Style );

            SkFontMetrics metrics;
            font.getMetrics(&metrics);

            std::string text = "ติดกาวด้านนี้";
            auto blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);

            int ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 

            // Glue Text Right
            if( x < _numTileX-1 ) {
                canvas->save();
                canvas->translate( _tileLeft + _tileW - metrics.fAscent, _tileTop + (_tileH - ww) / 2.0 );
                canvas->rotate( -90 );
                canvas->drawTextBlob(blob, 0, 0, paint);               
                canvas->restore();
            }

            // Glue Glue Bottom
            if( y < _numTileY-1 ) {
                canvas->drawTextBlob(blob, 
                    _tileLeft + (_tileW - ww) / 2, 
                    _tileTop + _tileH - metrics.fAscent, paint);               
            }

            text = "ตัดตามเส้นนี้";
            blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);

            ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect );                    

            // Glue Text Right
            if( x > 0 ) {
                canvas->save();
                canvas->translate( _tileLeft - metrics.fDescent, _tileTop + (_tileH - ww) / 2.0 );
                canvas->rotate( -90 );
                canvas->drawTextBlob(blob, 0, 0, paint);               
                canvas->restore();
            }

            // Cut Glue Bottom
            if( y > 0 ) {
                canvas->drawTextBlob(blob, 
                    _tileLeft + (_tileW - ww) / 2, 
                    _tileTop - metrics.fDescent, paint);               
            }

            char sz[256];
            sprintf( sz, "%s แผ่นที่ %i / %i", patternName, y * _numTileX + x + 1, _numTileX * _numTileY );

            // Title (Product ID, Size, Sheet ID, Page#)
            blob = SkTextBlob::MakeFromText(sz, strlen(sz), font);
            canvas->drawTextBlob(blob, 
                _tileLeft + (72 / 20), 
                _tileTop - metrics.fAscent + (72 / 40), 
                paint); 

            // Assembly Grid
            if( _numTileX > 1 || _numTileY > 1 ) {
                if( x == 0 && y == 0 ) {
                    canvas->save();
                    float nn = 25.0;
                    float grid_scale = nn / _tileW_DPI;
                    canvas->translate( _tileLeft + (72 / 20), _tileTop - metrics.fAscent + + metrics.fDescent + (72 / 40) );
                    canvas->scale( grid_scale, grid_scale );
                    canvas->translate( 0-_tileX_DPI, 0-_tileY_DPI );
                    fDom->render( canvas );
                    canvas->restore();

                    paint.setColor( SK_ColorDKGRAY );
                    paint.setStrokeWidth( 0.5 * scale );
                    for( int yy = 0; yy<_numTileY; yy++ ) {
                        for( int xx = 0; xx<_numTileX; xx++ ) {
                            SkRect rect = SkRect::MakeXYWH( 
                                _tileLeft + (72 / 20) + (xx * nn),
                                _tileTop - metrics.fAscent + + metrics.fDescent + (72 / 40) + (yy * nn * _tileH / _tileW),
                                nn, nn * _tileH / _tileW
                            );
                            if( x == xx && y == yy ) {
                                paint.setStyle( SkPaint::Style::kStroke_Style );
                                paint.setStrokeWidth( 2 * scale );
                                canvas->drawRect( rect, paint );
                            }
                            else {
                                paint.setStrokeWidth( 0.5 * scale );
                                paint.setStyle( SkPaint::Style::kStroke_Style );
                                canvas->drawRect( rect, paint );
                            }
                        }
                    } 
                }            
                else {
                    paint.setColor( SK_ColorDKGRAY );
                    paint.setStrokeWidth( 0.5 * scale );
                    for( int yy = 0; yy<_numTileY; yy++ ) {
                        for( int xx = 0; xx<_numTileX; xx++ ) {
                            int nn = 8;
                            SkRect rect = SkRect::MakeXYWH( 
                                _tileLeft + (72 / 20) + (xx * nn),
                                _tileTop - metrics.fAscent + + metrics.fDescent + (72 / 40) + (yy * nn),
                                6, 6
                            );
                            if( x == xx && y == yy ) {
                                paint.setStyle( SkPaint::Style::kFill_Style );
                                canvas->drawRect( rect, paint );
                            }
                            paint.setStyle( SkPaint::Style::kStroke_Style );
                            canvas->drawRect( rect, paint );
                        }
                    } 
                }
            }

            // Test Box
            if( x == 0 && y == 0 ) {
                int tbx = _tileLeft + _tileW - 72 - (72 / 20);
                int tby = _tileTop + (72 / 20);
                int cm = 72.0 / 2.54;

                paint.setStyle( SkPaint::Style::kStroke_Style );

                rect = SkRect::MakeXYWH( tbx, tby, 72, 72 );
                canvas->drawRect(rect, paint);
                rect = SkRect::MakeXYWH( tbx + (72.0 - cm)/2, tby + (72.0 - cm)/2, cm, cm );
                canvas->drawRect(rect, paint);

                font.setSize( 7 );
                font.getMetrics(&metrics);

                paint.setStyle( SkPaint::Style::kFill_Style );

                text = "ตรวจสอบขนาด";
                blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
                ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 

                canvas->drawTextBlob(blob, tbx + (72-ww)/2, tby+72-metrics.fDescent, paint);  

                text = "1 นิ้ว";
                blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
                ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 

                canvas->drawTextBlob(blob, tbx + (72-ww)/2, tby-metrics.fAscent, paint);  

                text = "1 ซ.ม.";
                blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
                ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 

                canvas->drawTextBlob(blob, tbx + (72-ww)/2, tby+36-(metrics.fDescent+metrics.fAscent)/2, paint); 
            }

 

            // Watermark
            rect = SkRect::MakeXYWH( 0, 0, pImageLogo->image->width(), pImageLogo->image->height() );
            canvas->save();
            float sc = 6.0 * 72.0 / (float)pImageLogo->image->width();
            canvas->translate( _tileLeft + _tileW / 2, _tileTop + _tileH / 2 );
            canvas->scale( sc, sc );
            canvas->rotate( -45 );
            canvas->translate( 0-pImageLogo->image->width()/2, 0-pImageLogo->image->height()/2);
            
            paint.setColorFilter(SkColorFilters::Blend(0x30FFFFFF, SkBlendMode::kModulate));
            canvas->drawImageRect( pImageLogo->image, rect, SkSamplingOptions(), &paint );
            canvas->restore();

  
            _pdfDoc->endPage();
        }
    }
}

void DecSVGDomEditor::renderPDF( const char *patternName, bool tiled, std::function<void(SkCanvas &canvas, int w, int h)>hook ) {
    if (_pdfDoc.get() == nullptr) 
        return;

    if (tiled) {
        setTile( "A4", 10, 15, 15, 10 );
        return renderTiledPDF( patternName, hook );
    }

    float dpi = 72;

    // SVG size at SVG DPI 
    SkSize szC = fDom->containerSize();   

    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        szR.fWidth = szC.fWidth;
        szR.fHeight = szC.fHeight;
    }

    // PDF size at 72 DPI
    float exportW = szR.fWidth;
    float exportH = szR.fHeight;

    // paper size
    float wInch = szR.fWidth/72.0;
    float hInch = szR.fHeight/72.0;

    int wMM = (int)(wInch * 25.4);
    int hMM = (int)(wInch * 25.4);

    bool bFitA1 = false;
    if( (wMM < _paperWidth[7] && hMM < _paperHeight[7]) ||
        (wMM < _paperHeight[7] && hMM < _paperWidth[7]) ) {
        bFitA1 = true;
    }  
    else {
        setTile( "A1", 10, 15, 15, 10 );
        return renderTiledPDF( patternName, hook );
    }

    float scale = std::min(exportW/szC.fWidth, exportH/szC.fHeight );

    float dw = 0;
    float dh = 0;    

    SkCanvas* canvas = _pdfDoc->beginPage(exportW, exportH);

    if (canvas == nullptr) 
        return;

    canvas->clear( SK_ColorWHITE );
    canvas->scale( scale, scale );

    char sz[256];
    sprintf( sz, "%s", patternName );

    SkFont font;
    font.setTypeface(_font->typeface(FONT_REGULAR));
    font.setSubpixel(true);
    font.setSize(12);       

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorBLACK);
    paint.setStyle( SkPaint::Style::kFill_Style );

    SkFontMetrics metrics;
    font.getMetrics(&metrics);

    // Title (Product ID, Size, Sheet ID, Page#)
    auto blob = SkTextBlob::MakeFromText(sz, strlen(sz), font);
    canvas->drawTextBlob(blob, 
        72/2, 
        72/2 + 10, 
        paint); 

    canvas->translate( dw, dh );
    fDom->render(canvas);
    if (hook) {
        hook( *canvas, exportW, exportH );
    }

    _pdfDoc->endPage();
}

void DecSVGDomEditor::startExportPDF( const char *filename ) {
    _pdfFilename = std::string( filename );
    _pdfStream = new SkDynamicMemoryWStream();
    if (_pdfStream == nullptr) 
        return;
    _pdfDoc = SkPDF::MakeDocument(_pdfStream);
}

void DecSVGDomEditor::endExportPDF() {
    if (_pdfDoc.get() == nullptr) 
        return;

    _pdfDoc->close();
    sk_sp<SkData> data = _pdfStream->detachAsData();

    if (data.get() == nullptr) 
        return;

#ifdef __EMSCRIPTEN__
    std::filesystem::path _path = std::filesystem::path(_pdfFilename.c_str());
    std::string strFilename = _path.filename();
    if (strFilename.empty()) return;

    EM_ASM({
        var pbytes = $0;
        var byteslen = $1;
        var dataUint8Array = CanvasKit['decUtility']['getBytes'](pbytes, byteslen);
        var blob = new Blob([dataUint8Array]);
        var a = document.createElement("a");
        document.body.appendChild(a);
        a.href = window.URL.createObjectURL(blob);
        a.download = UTF8ToString($2);
        a.onclick = function() {
            setTimeout(
                    function() {
                        window.URL.revokeObjectURL(a.href);
                        document.body.removeChild(a);
                    },
                    1500);
        };
        a.click();
    }, data->bytes(), data->size(), strFilename.c_str());
#else
    std::filesystem::path _path = std::filesystem::path(_pdfFilename.c_str());
    if (_path.has_parent_path()) {
        std::string str_path_dir = _path.parent_path().string();
        if (!std::filesystem::exists(str_path_dir)) {
            if (!std::filesystem::create_directories(str_path_dir)) {
                SkDebugf("Could not open export file: error on create directories.\n");
                return;
            }
        }
    }

    SkFILEWStream file(_pdfFilename.c_str());
    file.write(data->bytes(), data->size());
    delete _pdfStream;
    _pdfStream = nullptr;
#endif
}

void DecSVGDomEditor::exportSVG(const char* filename, std::function<void(SkCanvas &canvas)>hook) {
    if (!fDom) {
        return;
    }

    // SkSize containerSize = fDom->containerSize();
    //SkSize containerSize = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 95.997395));
    SkSize containerSize = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 96));
    SkDebugf("containerSize:[%.03f,%.03f]\n",
             containerSize.width(),
             containerSize.height());
    SkRect bounds = SkRect::MakeSize(containerSize);
    SkDynamicMemoryWStream writer;
    { // block for SkXMLStreamWriter(inside SkSVGCanvas obj) flush() on destrutor
        auto pSelectedNode_Temp = pSelectedNode;
        pSelectedNode = nullptr;

        std::unique_ptr<SkCanvas> canvas = SkSVGCanvas::Make(bounds, &writer);
        SkCanvas* pcanvas = canvas.get();

        // this->_draw_content(*pcanvas);
        fDom->render(pcanvas);
        if( hook ) {
            hook( *canvas );
        }         

        pSelectedNode = pSelectedNode_Temp;
    }
    
    sk_sp<SkData> data = writer.detachAsData();
    if (data.get() == nullptr) return;
    //SkDebugf("rawOutput:%.*s\n", (int)data->size(), (const char*)data->data());

#ifdef __EMSCRIPTEN__
    //gdata = data;
    std::filesystem::path _path = std::filesystem::path(filename);
    std::string strFilename = _path.filename();
    if (strFilename.empty()) return;

    EM_ASM({
        var pbytes = $0;
        var byteslen = $1;
        var dataUint8Array = CanvasKit['decUtility']['getBytes'](pbytes, byteslen);
        var blob = new Blob([dataUint8Array]);
        var a = document.createElement("a");
        document.body.appendChild(a);
        a.href = window.URL.createObjectURL(blob);
        a.download = UTF8ToString($2);
        a.onclick = function() {
            setTimeout(
                    function() {
                        window.URL.revokeObjectURL(a.href);
                        document.body.removeChild(a);
                    },
                    1500);
        };
        a.click();
    }, data->bytes(), data->size(), strFilename.c_str());
#else
    std::filesystem::path _path = std::filesystem::path(filename);
    if (_path.has_parent_path()) {
        std::string str_path_dir = _path.parent_path().string();
        if (!std::filesystem::exists(str_path_dir)) {
            if (!std::filesystem::create_directories(str_path_dir)) {
                SkDebugf("Could not open export file: error on create directories.\n");
                return;
            }
        }
    }

    SkFILEWStream file(filename);
    if (!file.isValid()) {
        SkDebugf("Could not open export file: error on create file.\n");
        return;
    }
    file.write(data->bytes(), data->size());

#endif

}

void DecSVGDomEditor::exportPNG( const char *filename, float dpi, std::function<void(SkCanvas &canvas)>hook  ) {
    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));

    //float exportW = szR.fWidth * dpi / 72.0;
    //float exportH = szR.fHeight * dpi / 72.0;

    float exportW = _visibleContentBounds.width() * dpi / 72.0;
    float exportH = _visibleContentBounds.height() * dpi / 72.0;

    SkSize sz = fDom->containerSize();

    auto surface = SkSurface::MakeRasterN32Premul(exportW, exportH);

    //float scale = std::min(exportW/sz.fWidth, exportH/sz.fHeight );
    float scale = std::min( exportW/(_visibleContentBounds.width() * _svgScaleX), exportH/(_visibleContentBounds.height() * _svgScaleY) );

    //float dw = ((exportW - sz.fWidth * scale)/2) / scale;
    //float dh = ((exportH - sz.fHeight * scale)/2) / scale;
    float dw = 0;
    float dh = 0;

    SkCanvas *canvas = surface->getCanvas();
    canvas->scale( scale, scale );
    canvas->translate( 0-(_visibleContentBounds.left() * _svgScaleX), 0-(_visibleContentBounds.top() * _svgScaleY) );
    fDom->render( canvas );
    if( hook ) {
        hook( *canvas );
    }    

    SkPixmap pixmap;
    surface->peekPixels(&pixmap);

    std::filesystem::path _path = std::filesystem::path(filename);
    if (_path.has_parent_path()) {
        std::string str_path_dir = _path.parent_path().string();
        if (!std::filesystem::exists(str_path_dir)) {
            if (!std::filesystem::create_directories(str_path_dir)) {
                SkDebugf("Could not open export file: error on create directories.\n");
                return;
            }
        }
    }

    SkFILEWStream out(filename);
    if (!out.isValid()) {
        SkDebugf( "Could not open export file: error on create file.\n" );
        return;
    }

    // Use default encoding options.
    SkPngEncoder::Options png_options;

    if (!SkPngEncoder::Encode(&out, pixmap, png_options)) {
        SkDebugf( "PNG encoding failed.\n" );
        return;
    }    
}

void DecSVGDomEditor::setDrawParametersFit() {
    SkSize szC = fDom->containerSize();   

    drawPaperW_DPI = szC.fWidth;
    drawPaperH_DPI = szC.fHeight;      

    drawScale_DPI = std::min( width()/drawPaperW_DPI, height()/drawPaperH_DPI );
    drawCenterOffsetX_DPI = ((width() - drawPaperW_DPI * drawScale_DPI)/2) / drawScale_DPI;
    drawCenterOffsetY_DPI = ((height() - drawPaperH_DPI * drawScale_DPI)/2) / drawScale_DPI;
}

void DecSVGDomEditor::setDrawParametersPaper() {
    SkSize szC = fDom->containerSize();   

    drawPaperW_DPI = ((paperWidth_mm / 25.4)) * _svgDPIX;
    drawPaperH_DPI = ((paperHeight_mm / 25.4)) * _svgDPIY;    

    drawScale_DPI = std::min( (width()-20)/drawPaperW_DPI, (height()-20)/drawPaperH_DPI );

    drawCenterOffsetX_DPI = (((width() - drawPaperW_DPI * drawScale_DPI)/2) / drawScale_DPI);
    drawCenterOffsetY_DPI = (((height() - drawPaperH_DPI * drawScale_DPI)/2) / drawScale_DPI);
}

void DecSVGDomEditor::setDrawParameters() {
    if( !fDom ) 
        return;
    if( drawFit ) {
        setDrawParametersFit( );
    }
    else {
        setDrawParametersPaper( );
    }
}

void DecSVGDomEditor::_draw_content(SkCanvas &canvas) {
    if (!_isVisible) {
        return;
    }
    if (_testDraw) {
        DecWidget::_draw_content(canvas);
        return;
    }
    if( !fDom ) {
        return;
    }

    //DecSVGDom::_draw_content(canvas);
    //return;

    setDrawParameters();

    SkSize szC = fDom->containerSize();  

    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        szR.fWidth = szC.fWidth;
        szR.fHeight = szC.fHeight;
    }
         
    canvas.save();
    canvas.translate( _svgViewPosnX, _svgViewPosnY );
    canvas.scale( _svgViewScale, _svgViewScale );
    canvas.scale( drawScale_DPI, drawScale_DPI );
    canvas.translate( drawCenterOffsetX_DPI, drawCenterOffsetY_DPI );    

    if( _showTileGrid ) {
        canvas.save();
        canvas.scale( _svgScaleX, _svgScaleY );
        canvas.translate( 0+contentCenterOffsetX_72, 0+contentCenterOffsetY_72 );
        
        SkPaint paint;
        paint.setColor( 0xFFFFFFFF );
        paint.setAntiAlias( true );
        paint.setStrokeWidth( 1 / drawScale_DPI );
        paint.setStyle( SkPaint::Style::kFill_Style );   

        SkRect rect = SkRect::MakeXYWH( 
            _tileX, 
            _tileY, 
            _tileW * _numTileX, 
            _tileH * _numTileY );
        canvas.drawRect( rect, paint );  

        canvas.restore();
    }
    else {
        // Draw Paper
        SkRect rect = SkRect::MakeXYWH( 0, 0, drawPaperW_DPI, drawPaperH_DPI );
        SkPaint paintBG;
        paintBG.setColor( 0xFFFFFFFF );
        paintBG.setStyle( SkPaint::Style::kFill_Style );    
        canvas.drawRect( rect, paintBG );    
        paintBG.setColor( 0xFF000000 );
        paintBG.setStyle( SkPaint::Style::kStroke_Style );            
        canvas.drawRect( rect, paintBG );   
        canvas.clipRect( rect, false );
    }

    // Draw SVG at center of paper
    canvas.save();
    if( _bMoveContentToCenter ) {
        canvas.translate( 0+contentCenterOffsetX_72*_svgScaleX, 0+contentCenterOffsetY_72*_svgScaleY );
    }
    fDom->render(&canvas);
    canvas.restore();

    SkSVGLengthContext       lctx(szC);
    SkSVGPresentationContext pctx;
    SkSVGRenderContext       rctx = SkSVGRenderContext(
                                        &canvas, 
                                        fDom->fFontMgr, 
                                        fDom->fResourceProvider, 
                                        fDom->fIDMapper, 
                                        lctx, 
                                        pctx,
                                        {nullptr, nullptr});

    if( _predraw_hook ) {
        _predraw_hook( this, canvas );
    }

    // All overlay draw at 72 DPI (Document Space)
    canvas.save();
    canvas.scale( _svgScaleX, _svgScaleY );

    // Minimal paper bounds
    if( _showMinPaperBounds ) {
        float minPW = (minPaperWidth_mm / 25.4) * 72.0;
        float minPH = (minPaperHeight_mm / 25.4) * 72.0;
        float minPX = (drawPaperW_DPI / _svgScaleX - minPW) / 2;
        float minPY = (drawPaperH_DPI / _svgScaleY- minPH) / 2;
        SkRect rect = SkRect::MakeXYWH( minPX, minPY, minPW, minPH );
        SkPaint paintBG;

        paintBG.setColor( 0xFF00FF00 );
        paintBG.setStrokeWidth( 1.0 / drawScale_DPI );
        paintBG.setStyle( SkPaint::Style::kStroke_Style );    
        canvas.drawRect( rect, paintBG );     
    }

    // All object bounds ar draw at 72 DPI (center translate)
    // Selection to center space    
    canvas.translate( 0+contentCenterOffsetX_72, 0+contentCenterOffsetY_72 );
    if( pSelectedNode ) {
        SkRect rect = pSelectedNode->objectBoundingBox( rctx );
        
        SkPaint paint;
        paint.setAntiAlias( true );
        paint.setColor( 0x80FF0000 );
        paint.setStrokeWidth( 1.0/drawScale_DPI );
        paint.setStyle( SkPaint::Style::kStroke_Style );    
            
        canvas.drawRect( rect, paint );  
    }       

    if( _showVisibleContentBounds ) {
        SkPaint paint;
        paint.setColor( 0x40FF0000 );
        paint.setAntiAlias( true );
        paint.setStrokeWidth( 1 / drawScale_DPI );
        paint.setStyle( SkPaint::Style::kStroke_Style );    

        SkRect rect = SkRect::MakeXYWH( 
            _visibleContentBounds.x(), 
            _visibleContentBounds.y(), 
            _visibleContentBounds.width(), _visibleContentBounds.height() );
        canvas.drawRect( rect, paint );
    }

    if( _showTileGrid ) {
        SkPaint paint;
        paint.setColor( 0x400000FF );
        paint.setAntiAlias( true );
        paint.setStrokeWidth( 1 / drawScale_DPI );
        paint.setStyle( SkPaint::Style::kStroke_Style );   

        SkRect rect = SkRect::MakeXYWH( 
            _tileX, 
            _tileY, 
            _tileW * _numTileX, 
            _tileH * _numTileY );
        canvas.drawRect( rect, paint );
        for( int i=0; i<_numTileX; i++ ) {
            float xx = _tileX + (i * _tileW);
            canvas.drawLine( xx, _tileY, xx, _tileY + _tileH * _numTileY, paint );
        }
        for( int i=0; i<_numTileY; i++ ) {
            float yy = _tileY + (i * _tileH);
            canvas.drawLine( _tileX, yy, _tileX + _tileW * _numTileX, yy, paint );
        }
    }

    canvas.restore();

    if( _draw_hook ) {
        _draw_hook( this, canvas );
    }


    canvas.restore();
}