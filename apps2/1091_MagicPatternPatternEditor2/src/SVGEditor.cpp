#include <fstream>
#include <string>
#include <iostream>
#include <cstring>
#include <math.h>  
#include "DeclarativeUI.h"
#include "DecSVGDom.h"
#include "DecUI.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "include/core/SkSamplingOptions.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkFontMetrics.h"
#include "include/pathops/SkPathOps.h"
#include "include/core/SkPathEffect.h"
#include "include/core/SkPathUtils.h"
#include "src/utils/SkOSPath.h"
#include "modules/skresources/include/SkResources.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "modules/svg/include/SkSVGText.h"
#include "modules/svg/include/SkSVGG.h"
#include "modules/svg/include/SkSVGTypes.h"
#include "modules/svg/include/SkSVGRenderContext.h"
#include "SVGEditor.h"
#include "../../1020_SVGEditorApp/src/SVGDomEditor.h"

// for SK_ARRAY_COUNT
#include "include/core/SkTypes.h"

const float MP_BUST_CIRCUM[5] = { 90.5, 96.0, 107.0, 117.0, 122.5 };
const float MP_WAIST_CIRCUM[5] = { 92.5, 99.0, 109.0, 119.0, 124.5 };
const float MP_FULL_LENGTH[5] = { 40, 41, 42, 43, 44 };

const float MP_BODY_BUST[5] = { 80.0, 85.5, 97.0, 107.0, 112.0 };
const float MP_BODY_WAIST[5] = { 61.0, 67.5, 76.0, 85.0, 94.0 };
const float MP_BODY_HIP[5] = { 85.0, 91.5, 102.0, 112.0, 121.0 };

                                       //xs   s     m     l     xl     2xl    3xl    4xl
const float SP_BODY_BUST[8] =        { 80.0,  84.0, 88.0, 92.0, 98.0,  102.0, 107.0, 112.0 };
const float SP_BODY_WAIST[8] =       { 60.0,  62.0, 69.0, 74.0, 80.0,  84.0,  88.0,  92.0 };
const float SP_BODY_HIP[8] =         { 85.0,  88.0, 94.0, 97.0, 102.0, 107.0, 112.0, 117.0 };
const float SP_BODY_NECK_CIRCUM[8] = { 32.0,  34.0, 36.0, 38.0, 40.0,  42.0,  44.0,  44.0 };
const float SP_BODY_CF_LENGTH[8] =   { 32.0,  33.0, 36.0, 38.0, 40.0,  42.0,  42.0,  42.0 };

const char *tagName[] = {
    "Circle",
    "ClipPath",
    "Defs",
    "Ellipse",
    "FeBlend",
    "FeColorMatrix",
    "FeComposite",
    "FeDiffuseLighting",
    "FeDisplacementMap",
    "FeDistantLight",
    "FeFlood",
    "FeGaussianBlur",
    "FeImage",
    "FeMorphology",
    "FeOffset",
    "FePointLight",
    "FeSpecularLighting",
    "FeSpotLight",
    "FeTurbulence",
    "Filter",
    "G",
    "Image",
    "Line",
    "LinearGradient",
    "Mask",
    "Path",
    "Pattern",
    "Polygon",
    "Polyline",
    "RadialGradient",
    "Rect",
    "Stop",
    "Svg",
    "Text",
    "TextLiteral",
    "TextPath",
    "TSpan",
    "Use"
};

const char *visibilityType[] = {
    "Visible",
    "Hidden",
    "Collapse",
    "Inherit"
};

const char *svgPaintType[] = {
    "None",
    "Color",
    "IRI",
};

const char *svgColorType[] = {
    "CurrentColor",
    "Color",
    "ICCColor",
};

struct StrokeOpts {
    // Default values are set in interface.js which allows clients
    // to set any number of them. Otherwise, the binding code complains if
    // any are omitted.
    SkScalar width;
    SkScalar miter_limit;
    SkPaint::Join join;
    SkPaint::Cap cap;
    float precision;
};

bool ApplyStroke(SkPath& path, StrokeOpts opts) {
    SkPaint p;
    p.setStyle(SkPaint::kStroke_Style);
    p.setStrokeCap(opts.cap);
    p.setStrokeJoin(opts.join);
    p.setStrokeWidth(opts.width);
    p.setStrokeMiter(opts.miter_limit);

    //return p.getFillPath(path, &path, nullptr, opts.precision);
    return skpathutils::FillPathWithPaint(path, p, &path);
}

void DecSVGEditor::clearPatternPoints() {
    for( int si=0; si<5; si++ ) {
        for( int i=0; i<PT_MAX; i++ ) {
            _bPatternPointsSet[si][i] = false;
            _patternPoints[si][i].fX = 0;
            _patternPoints[si][i].fY = 0;
        }
        for( int i=0; i<SZ_MAX; i++ ) {
            _patternSizes[si][i] = 0;
        }
    }
}

DecSVGEditor::DecSVGEditor() {
    clearPatternPoints();

    fDom = NULL;
    root = NULL;
    _pPathNodeList = NULL;
    _pFileList = NULL;

    _is_S = true;
    _is_M = false;
    _is_L = false;
    _is_XL = false;
    _is_XXL = false;

    _generateFrame = 1;
    _generateCount = 0;
    _generatePattern = false;

    _forceBlack = true;
    _excludeText = false;
    _excludePocket = false;
    _isTiledExport = true;

    for( int i=0; i<5; i++ ) {
        _sewingLinePath[i] = NULL;
    }
    _SelectedSewingLinePath = NULL;
    _selectedSewingLineIndex = -1;

    size( -1, -1 );
    border( false );
    background( false );
    _( Column()
        .size( 300, -1 )
        .border(false)
        .background(false)
        .padding( 10, 10, 10, 10 )
        .gap( 10 )
        ._( Column()
            .border( false )
            .gap( 5 )
            .size( -1, 200 )
            ._(Text( "Pattern Files:") )
            ._(FileList( "_magicpattern/V2-Blouse-Dress-SVG" )
                .size( -1, -1 )
                .ref( (DecWidget **)&_pFileList )
                .onValueChanged( [=]( DecWidget &widget ) {
                    DecFileList *filelist = (DecFileList *)&widget;
                    _filepath = filelist->fullpath.string();
                    _filename = filelist->fullpath.filename().string();
                    _filebasename = filelist->fullpath.stem().string();

                    pNodeTree->clear();
                    pSVGDomEditor->load(_filepath.c_str());
                    pSVGDomEditor->dirty();
                    loadSewingLine();
                })    
            )
        )
        ._( Container()
            .size( -1, 200 )
            .border_color( 0xFF808080 )
            ._( Tree()
                .border( false )
                .tag( 7000 )
                .ref( (DecWidget **)&pNodeTree )
            )
        )   
        ._( Column()
            .size( -1, 0 )
            .border( false )
            .gap( 5 )
            .cache( false )
            ._( ComboBox()
                .ref( (DecWidget **)&_pComboBoxTargetPaper )
                .size( -1, 32 )
                .border( true )
                ._( ComboBoxItem( "A0" ) )
                ._( ComboBoxItem( "A1" ) )
                ._( ComboBoxItem( "A2" ) )
                ._( ComboBoxItem( "A3" ) )
                ._( ComboBoxItem( "A4" ) )
                ._( ComboBoxItem( "A5" ) )
                ._( ComboBoxItem( "A6" ) )
                ._( ComboBoxItem( "A7" ) )
                ._( ComboBoxItem( "A8" ) )
                .onValueChanged( [=](DecWidget &widget) {
                    changeTargetPaper( widget.text() );
                })
            )
            ._( Row()
                .size( -1, 0 )
                .border( false )
                ._( Container().size( -1, 0 ).border(false)._(CheckBox( "S" ).check(true).ref( (DecWidget**)&_pCB_S).onValueChanged([=](DecWidget &w){
                    _is_S = w.value();
                    if( _is_S ) {
                        _pCB_M->check( false );
                        _pCB_L->check( false );
                        _pCB_XL->check( false );
                        _pCB_XXL->check( false );
                    }
                    updateSewingLineList();
                    updateNodeVisibility();
                })) )
                ._( Container().size( -1, 0 ).border(false)._(CheckBox( "M" ).check(false).ref( (DecWidget**)&_pCB_M).onValueChanged([=](DecWidget &w){
                    _is_M = w.value();
                    if( _is_M ) {
                        _pCB_S->check( false );
                        _pCB_L->check( false );
                        _pCB_XL->check( false );
                        _pCB_XXL->check( false );
                    }
                    updateSewingLineList();
                    updateNodeVisibility();
                })) )
                ._( Container().size( -1, 0 ).border(false)._(CheckBox( "L" ).check(false).ref( (DecWidget**)&_pCB_L).onValueChanged([=](DecWidget &w){
                    _is_L = w.value();
                    if( _is_L ) {
                        _pCB_S->check( false );
                        _pCB_M->check( false );
                        _pCB_XL->check( false );
                        _pCB_XXL->check( false );
                    }
                    updateSewingLineList();
                    updateNodeVisibility();
                })) )
                ._( Container().size( -1, 0 ).border(false)._(CheckBox( "XL" ).check(false).ref( (DecWidget**)&_pCB_XL).onValueChanged([=](DecWidget &w){
                    _is_XL = w.value();
                    if( _is_XL ) {
                        _pCB_S->check( false );
                        _pCB_M->check( false );
                        _pCB_L->check( false );
                        _pCB_XXL->check( false );
                    }
                    updateSewingLineList();
                    updateNodeVisibility();
                }) ) )
                ._( Container().size( -1, 0 ).border(false)._(CheckBox( "XXL" ).check(false).ref( (DecWidget**)&_pCB_XXL).onValueChanged([=](DecWidget &w){
                    _is_XXL = w.value();
                    if( _is_XXL ) {
                        _pCB_S->check( false );
                        _pCB_M->check( false );
                        _pCB_L->check( false );
                        _pCB_XL->check( false );
                    }
                    updateSewingLineList();
                    updateNodeVisibility();
                }) ) )
            )
        )
        ._( Column()
            .border( false )
            .gap( 5 )
            ._(Text( "Sewing Line:") )
            ._(List()
                .ref( (DecWidget **)&_pPathNodeList )
                .onValueChanged( [=]( DecWidget &w) {
                    onPathVerbSelect( (DecList *)&w );
                })
            )
        )    
        ._(Row()
            .size( -1, 0 )
            .background( false )
            .border( false )
            .gap( 5 )
            ._( Button("Load", 0, [=]( DecWidget &w )  {
                loadPatternPoints();
            }).size( -1, 0 ) )
            ._( Button("Save", 0, [=]( DecWidget &w )  {
                savePatternPoints();
            }).size( -1, 0 ) )
        )
    );
    //._( SVGEditor( "resources/svg/HBAN-01.svg") );
    //._( SVGEditor( "resources/svg/MagicPattern-01.svg") );
    //_(SVGDomEditor("resources/svg/Blouse-01.svg").ref((DecWidget**)&pSVGDomEditor).cache(true));
    _( Column()
        .border( false )
        ._( Row()
            .border( false )
            .size( -1, 0 )
            .padding( 5, 10, 5, 0 )
            .gap( 20 )
            ._( CheckBox( "All Black Color", [=]( DecCheckBox &cb ) {
                _forceBlack = cb.value();
                pNodeTree->clear();
                pNodeTree->dirty();
                pSVGDomEditor->load(_filepath.c_str());
                pSVGDomEditor->dirty();
            }).check( true, false ) )
            ._( CheckBox( "Remove Text Layers", [=]( DecCheckBox &cb ) {
                _excludeText = cb.value();
                pNodeTree->clear();
                pNodeTree->dirty();
                pSVGDomEditor->load(_filepath.c_str());
                pSVGDomEditor->dirty();                
            }).check( false, false ) )            
        )
        ._( Row()
            .border( false )
            .size( -1, 0 )
            .padding( 5, 10, 5, 0 )
            .gap( 20 )
            ._( CheckBox( "Show Min Paper Size", [=]( DecCheckBox &cb ) {
                pSVGDomEditor->_showMinPaperBounds = cb.value();
                pSVGDomEditor->dirty();
            }).check( false, false ) )
            ._( CheckBox( "Show Content Area", [=]( DecCheckBox &cb ) {
                pSVGDomEditor->_showVisibleContentBounds = cb.value();
                pSVGDomEditor->dirty();
            }).check( false, false ) )
            ._( CheckBox( "Show Tile Grid", [=]( DecCheckBox &cb ) {
                pSVGDomEditor->_showTileGrid = cb.value();
                pSVGDomEditor->dirty();
            }).check( false, false ) )
        )
        ._( SVGDomEditor()
            .preDrawHook( [=](DecSVGDomEditor *dom, SkCanvas &canvas) {
                drawSewingLine( dom, canvas );
            })
            .ref((DecWidget**)&pSVGDomEditor)
            .cache(true)
        )
    );
    _( MessageBox()
        .ref( (DecWidget **)&_pMessageBox )
    );
    _( Scroller( SCROLL_VERTICAL )
        .size( 360, -1 )
        ._( Column()
            .size( -1, 0 )
            .border(false)
            .background(false)
            .gap( 10 )
            .padding( 10, 10, 10, 10 )

            ._( Column()
                .size( -1, 0 )
                .border( false )
                .gap( 5 )
                .cache( false )
                ._( Text( "Export: " ).margin_top( 5 ) )
                ._( CheckBox( "Tiled Export", [=]( DecCheckBox &cb ) {
                    _isTiledExport = cb.value();
                }).check( true, false ) )
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("PDF Export", 0, [=]( DecButton &btn ) {
                        startExportPDF( _isTiledExport );
                        renderPDF( _filebasename.c_str(), _isTiledExport );
                        pSVGDomEditor->endExportPDF();
                    }))
                    ._( Button("PNG Export", 0, [=]( DecButton &btn ) {
                        exportPNG();
                    }))
                    ._( Button("SVG Export", 0, [=]( DecButton &btn ) {
                        exportSVG();
                    }))
                )
                ._( Text( "Bulk Export: " ).margin_top( 5 ) )
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("PDF all sizes", 0, [=]( DecButton &btn ) {
                        exportPDFs();
                    }))
                    ._( Button("PNG all sizes", 0, [=]( DecButton &btn ) {
                        exportPNGs();
                    }))
                    ._( Button("SVG all sizes", 0, [=]( DecButton &btn ) {
                        exportSVGs();
                    }))
                )
                ._( Text( "Setting: " ).margin_top( 5 ) )
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("Shoulder Tip", 0, [=]( DecButton &btn ) {
                        setPatternPoint(PT_SHOULDER_TIP);
                    }))
                    ._( Button("Shoulder/Neck", 0, [=]( DecButton &btn ) {
                        setPatternPoint(PT_SHOULDER_NECK);
                    }))
                )
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("SS Bust", 0, [=]( DecButton &btn ) {
                        setPatternPoint(PT_SS_BUST);
                    }))
                    ._( Button("CF Neck", 0, [=]( DecButton &btn ) {
                        setPatternPoint(PT_CF_NECK);
                    }))
                )                
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("SS Waist", 0, [=]( DecButton &btn ) {
                        setPatternPoint(PT_SS_WAIST);
                    }))
                    ._( Button("CF Waist", 0, [=]( DecButton &btn ) {
                        setPatternPoint(PT_CF_WAIST);
                    }))
                )
            )
            ._( Column()
                .size( -1, 0 )
                .border( false )
                .gap( 5 )
                .cache( false )
                ._( Text( "Pattern Sizes: " ).margin_top( 5 ) )
                ._( Text( "Full length: " )
                    .ref( (DecWidget **)&_pTextFullLength )
                )
                ._( Text( "Neck: " )
                    .ref( (DecWidget **)&_pTextNeck )
                )
                ._( Text( "Bust Circum: " )
                    .ref( (DecWidget **)&_pTextCircum )
                )
                ._( Text( "Seam: " )
                    .ref( (DecWidget **)&_pTextSeam )
                )
            )         
            ._( Column()
                .size( -1, 0 )
                .border( false )
                .gap( 5 )
                .cache( false )
                ._( Text( "MagicPattern Body Sizes: " ).margin_top( 5 ) )
                ._( Text( "Bust: " )
                    .ref( (DecWidget **)&_pTextMPBodySizes )
                )
            )              
            ._( Column()
                .size( -1, 0 )
                .border( false )
                .gap( 5 )
                .cache( false )
                ._( Text( "Generate Parameters: " ).margin_top( 5 ) )
                ._( TextEdit("0", 
                    [=](DecTextEdit& textedit) {
                        
                    })
                    .size(-1, 32)
                    .ref((DecWidget**)&_pTENeckCircum)
                )
            )              
        )
    );
}

DecSVGEditor::~DecSVGEditor() {
}

void DecSVGEditor::updateSewingLineList() {
    _pPathNodeList->clear();
    int size_index = getSizeIndex();
    if( size_index == -1 ) {
        return;
    }    
    std::vector<std::string>slInfo = _vSewingLineInfo[size_index];
    for( int i=0; i<slInfo.size(); i++ ) {
        _pPathNodeList->_( ListItem( slInfo[i].c_str() ));
    }
    updateSizes();
}

void DecSVGEditor::updateSelectedSewingLine() {
    clearSelectedSewingLinePath();

    if( _selectedSewingLineIndex >= 1 && _selectedSewingLineIndex < _pPathNodeList->count() ) {
        DecListItem *pItem0 = _pPathNodeList->getItem(_selectedSewingLineIndex-1);
        char sz[128];
        strcpy( sz, pItem0->text() );

        char *verb = strtok( sz, " ");
        char *x = strtok (NULL, " ");
        char *y = strtok (NULL, " ");
        if( !strcmp( verb, "C" ) ) {
            x = strtok (NULL, " ");
            y = strtok (NULL, " ");
            x = strtok (NULL, " ");
            y = strtok (NULL, " ");
        }
        else if( !strcmp( verb, "Q" ) ) {
            x = strtok (NULL, " ");
            y = strtok (NULL, " ");
        }
        else if( !strcmp( verb, "N" ) ) {
            x = strtok (NULL, " ");
            y = strtok (NULL, " ");
        }
        else if( !strcmp( verb, "X" ) ) {
            SkDebugf( "Unknown verb: %s\n", verb );
            return;
        }
        SkPath path;
        path.moveTo( atof(x), atof(y) );

        DecListItem *pItem1 = _pPathNodeList->getItem(_selectedSewingLineIndex);
        strcpy( sz, pItem1->text() );

        verb = strtok( sz, " ");
        x = strtok (NULL, " ");
        y = strtok (NULL, " ");
        if( !strcmp( verb, "M" ) ) {
            path.lineTo( atof(x), atof(y) );
            _currentPoint = SkPoint::Make( atof(x), atof(y) );
        }
        else if( !strcmp( verb, "L" ) ) {
            path.lineTo( atof(x), atof(y) );
            _currentPoint = SkPoint::Make( atof(x), atof(y) );
        }
        else if( !strcmp( verb, "C" ) ) {
            char *x2 = strtok (NULL, " ");
            char *y2 = strtok (NULL, " ");
            char *x3 = strtok (NULL, " ");
            char *y3 = strtok (NULL, " ");
            path.cubicTo( atof(x), atof(y), atof(x2), atof(y2), atof(x3), atof(y3) );
            _currentPoint = SkPoint::Make( atof(x3), atof(y3) );
        }
        else if( !strcmp( verb, "Q" ) ) {
            char *x2 = strtok (NULL, " ");
            char *y2 = strtok (NULL, " ");
            path.quadTo( atof(x), atof(y), atof(x2), atof(y2) );
            _currentPoint = SkPoint::Make( atof(x2), atof(y2) );
        }
        else if( !strcmp( verb, "N" ) ) {
            char *x2 = strtok (NULL, " ");
            char *y2 = strtok (NULL, " ");
            path.quadTo( atof(x), atof(y), atof(x2), atof(y2));
            _currentPoint = SkPoint::Make( atof(x2), atof(y2) );
        }
        else if( !strcmp( verb, "X" ) ) {
            //path->close();
            return;
        }        
        _SelectedSewingLinePath = new SkPath( path );
    }
}

bool DecSVGEditor::onChar(SkUnichar c, skui::ModifierKey mod) {
    return false;
}

int DecSVGEditor::getSizeIndex() {
    if( _is_S ) return 0;
    else if( _is_M ) return 1;
    else if( _is_L ) return 2;
    else if( _is_XL ) return 3;
    else if( _is_XXL ) return 4;
    return -1;
}

void DecSVGEditor::calculateSizes() {
    for( int si=0; si<5; si++ ){
        if( _bPatternPointsSet[si][PT_SHOULDER_NECK] && _bPatternPointsSet[si][PT_CF_WAIST] && _bPatternPointsSet[si][PT_SHOULDER_TIP]) {
            // คำนวณคอจากแพทเทิร์นเมย์ (ใหญ่ไปมากจาก size อารีย์)
            _patternSizes[si][SZ_NECK_HORZ] = (_patternPoints[si][PT_CF_WAIST].fX - _patternPoints[si][PT_SHOULDER_NECK].fX) / 72.0 * 2.54;
            _patternSizes[si][SZ_NECK_VERT] = (_patternPoints[si][PT_CF_NECK].fY - _patternPoints[si][PT_SHOULDER_NECK].fY) / 72.0 * 2.54;
            _patternSizes[si][SZ_NECK_CIRCUM] = (_patternSizes[si][SZ_NECK_HORZ] - 0.5) * 6.0;

            // Full length จากแพทเทิร์นเมย์ ตรงตามสเปค 
            _patternSizes[si][SZ_FULL_LENGTH] = (_patternPoints[si][PT_CF_WAIST].fY - _patternPoints[si][PT_SHOULDER_NECK].fY) / 72.0 * 2.54;
            _patternSizes[si][SZ_CF_LENGTH] = _patternSizes[si][SZ_FULL_LENGTH] - _patternSizes[si][SZ_NECK_VERT];

            // Circum จากแพทเทิร์นเมย์ (ใหญ่กว่าสเปคไป 0.5 ซ.ม.)
            _patternSizes[si][SZ_BUST_CIRCUM] = (_patternPoints[si][PT_CF_WAIST].fX - _patternPoints[si][PT_SS_BUST].fX) * 4.0 / 72.0 * 2.54;
            _patternSizes[si][SZ_WAIST_CIRCUM] = (_patternPoints[si][PT_CF_WAIST].fX - _patternPoints[si][PT_SS_WAIST].fX) * 4.0 / 72.0 * 2.54;

            _patternSizes[si][SZ_SHOULDER_SEAM] = sqrt(
                pow(_patternPoints[si][PT_SHOULDER_NECK].fX - _patternPoints[si][PT_SHOULDER_TIP].fX, 2) +
                pow(_patternPoints[si][PT_SHOULDER_NECK].fY - _patternPoints[si][PT_SHOULDER_TIP].fY, 2) ) / 72.0 * 2.54;


            // คำนวณกลางคอหน้าแบบครูอารีย์จาก shoulder slope * 2
            //_patternSizes[si][SZ_NECK_VERT_A] = (_patternPoints[si][PT_SHOULDER_TIP].fY - _patternPoints[si][PT_SHOULDER_NECK].fY) * 2.0 / 72.0 * 2.54;
            //_patternSizes[si][SZ_NECK_HORZ_A] = _patternSizes[si][SZ_NECK_VERT_A] - 0.5;
            //_patternSizes[si][SZ_NECK_CIRCUM_A] = (_patternSizes[si][SZ_NECK_HORZ_A] - 0.5) * 6.0;

            // กลางคอหน้าคำนวณแบบครูอารีย์ = ไหล่คอ + คอแนวตั้ง
            //_patternPoints[si][PT_CF_NECK_A].fX = _patternPoints[si][PT_CF_WAIST].fX;
            //_patternPoints[si][PT_CF_NECK_A].fY = _patternPoints[si][PT_SHOULDER_NECK].fY + (_patternSizes[si][SZ_NECK_VERT_A] / 2.54 * 72.0);
            //_bPatternPointsSet[si][PT_CF_NECK_A] = true;   

            // ไหล่คอคำนวณแบบครูอารีย์ 
            //_patternPoints[si][PT_SHOULDER_NECK_A].fX = _patternPoints[si][PT_CF_WAIST].fX - _patternSizes[si][SZ_NECK_HORZ_A] / 2.54 * 72.0;
            //_patternPoints[si][PT_SHOULDER_NECK_A].fY = _patternPoints[si][PT_SHOULDER_NECK].fY;
            //_bPatternPointsSet[si][PT_SHOULDER_NECK_A] = true;  


            //_patternSizes[si][SZ_CF_LENGTH_A] = (_patternPoints[si][PT_CF_WAIST].fY - _patternPoints[si][PT_CF_NECK_A].fY) / 72.0 * 2.54;

            // กลางคอหน้า (เมย์)
            //_patternPoints[si][PT_CF_NECK].fX = _patternPoints[si][PT_CF_WAIST].fX;
            //_patternPoints[si][PT_CF_NECK].fY = _patternPoints[si][PT_SHOULDER_NECK].fY + (_patternSizes[si][SZ_NECK_VERT] / 2.54 * 72.0);
            //_bPatternPointsSet[si][PT_CF_NECK] = true;            

            // กลางอกหน้า (เมย์)
            //_patternPoints[si][PT_CF_BUST].fX = _patternPoints[si][PT_CF_WAIST].fX;
            //_patternPoints[si][PT_CF_BUST].fY = (_patternPoints[si][PT_CF_NECK].fY - 0.75) + (_patternSizes[si][SZ_CF_LENGTH] / 2 / 2.54 * 72.0);
            //_bPatternPointsSet[si][PT_CF_BUST] = true;            

            //_patternPoints[si][PT_CF_BUST_A].fX = _patternPoints[si][PT_CF_WAIST].fX;
            //_patternPoints[si][PT_CF_BUST_A].fY = (_patternPoints[si][PT_CF_NECK_A].fY) + (_patternSizes[si][SZ_CF_LENGTH_A] / 2 / 2.54 * 72.0);
            //_bPatternPointsSet[si][PT_CF_BUST_A] = true;            
        }
    }   
    updateSizes();
}

void DecSVGEditor::updateSizes() {
    int si = getSizeIndex();
    if( si != -1 ) {
        char sz[64];

        sprintf( sz, "Full length: %0.2f (%0.2f), CF length: %0.2f", _patternSizes[si][SZ_FULL_LENGTH], MP_FULL_LENGTH[si], _patternSizes[si][SZ_CF_LENGTH] );
        _pTextFullLength->text( sz );
        
        sprintf( sz, "Neck H: %0.2f, V: %0.2f, C: %0.2f", _patternSizes[si][SZ_NECK_HORZ], _patternSizes[si][SZ_NECK_VERT],
            _patternSizes[si][SZ_NECK_CIRCUM] );
        _pTextNeck->text( sz );

        sprintf( sz, "Bust Circum: %0.2f (%0.2f), Waist Circum: %0.2f (%0.2f)", _patternSizes[si][SZ_BUST_CIRCUM], MP_BUST_CIRCUM[si], _patternSizes[si][SZ_WAIST_CIRCUM], MP_WAIST_CIRCUM[si] );
        _pTextCircum->text( sz );

        sprintf( sz, "Shoulder Seam: %0.2f", _patternSizes[si][SZ_SHOULDER_SEAM] );
        _pTextSeam->text( sz );        

        // MP Body sizes
        sprintf( sz, "Bust: %0.2f, Waist: %0.2f, Hip: %0.2f", MP_BODY_BUST[si], MP_BODY_WAIST[si], MP_BODY_HIP[si] );
        _pTextMPBodySizes->text( sz );



        //sprintf( sz, "Neck (A) H: %0.2f, V: %0.2f, C: %0.2f", _patternSizes[si][SZ_NECK_HORZ_A], _patternSizes[si][SZ_NECK_VERT_A],
        //    _patternSizes[si][SZ_NECK_CIRCUM_A] );
        //_pTextNeckA->text( sz );
    }
}


void DecSVGEditor::setPatternPoint( int index ) {
    if( _selectedSewingLineIndex == -1 ) {
        return;
    }

    int si = getSizeIndex();
    if( si != -1 ) {
        if( index >= 0 && index < PT_MAX ) {
            _patternPoints[si][index] = _currentPoint;
            _bPatternPointsSet[si][index] = true;
        }
    }
    calculateSizes();
}

void DecSVGEditor::clearPatternPoint( int index ) {
    int si = getSizeIndex();
    if( si != -1 ) {    
        if( index > 0 && index < PT_MAX ) {
            _bPatternPointsSet[si][index] = false;
        }
    }
}

void DecSVGEditor::loadSewingLine() {
    clearPatternPoints();
    clearSelectedSewingLinePath();
    _selectedSewingLineIndex = -1;

    SkDebugf( "Load sewing line\n" );
    for( int i=0; i<5; i++ ) {
        std::string fn = std::string("_sewingline/") + _filebasename;
        if( i == 0 ) fn += "-S";
        else if( i == 1 ) fn += "-M";
        else if( i == 2 ) fn += "-L";
        else if( i == 3 ) fn += "-XL";
        else if( i == 4 ) fn += "-XXL";
        fn += ".txt";

        if( _sewingLinePath[i] ) {
            delete( _sewingLinePath[i] );
            _sewingLinePath[i] = NULL;
        }        

        if( std::filesystem::exists( fn ) ) {
            std::ifstream file(fn);
            std::string str; 
            _vSewingLineInfo[i].clear();
            while (std::getline(file, str))
            {
                _vSewingLineInfo[i].push_back( std::string( str ) );
            } 
            file.close();
            SkPath *path = new SkPath();
            for( int j=0; j<_vSewingLineInfo[i].size(); j++ ) {
                std::string str = _vSewingLineInfo[i][j];

                char sz[2048];
                strcpy( sz, str.c_str() );
                float lastX = 0;
                float lastY = 0;

                char *verb = strtok( sz, " ");
                char *x = strtok (NULL, " ");
                char *y = strtok (NULL, " ");

                if( !strcmp( verb, "M" ) ) {
                    path->moveTo( atof(x), atof(y) );
                    lastX = atof(x);
                    lastY = atof(y);
                }
                else if( !strcmp( verb, "L" ) ) {
                    if( atof(x) != lastX || atof(y) != lastY ) {
                        path->lineTo( atof(x), atof(y) );
                        lastX = atof(x);
                        lastY = atof(y);
                    }
                }
                else if( !strcmp( verb, "C" ) ) {
                    char *x2 = strtok (NULL, " ");
                    char *y2 = strtok (NULL, " ");
                    char *x3 = strtok (NULL, " ");
                    char *y3 = strtok (NULL, " ");
                    path->cubicTo( atof(x), atof(y), atof(x2), atof(y2), atof(x3), atof(y3) );
                    lastX = atof(x3);
                    lastY = atof(y3);
                }
                else if( !strcmp( verb, "Q" ) ) {
                    char *x2 = strtok (NULL, " ");
                    char *y2 = strtok (NULL, " ");
                    path->quadTo( atof(x), atof(y), atof(x2), atof(y2) );
                    lastX = atof(x2);
                    lastY = atof(y2);
                }
                else if( !strcmp( verb, "N" ) ) {
                    char *x2 = strtok (NULL, " ");
                    char *y2 = strtok (NULL, " ");
                    path->quadTo( atof(x), atof(y), atof(x2), atof(y2));
                    lastX = atof(x2);
                    lastY = atof(y2);
                }
                else if( !strcmp( verb, "X" ) ) {
                    //path->close();
                }

            }
            _sewingLinePath[i] = path;
        }
    }
    loadPatternPoints();
    updateSewingLineList();
}

void DecSVGEditor::drawGuides( SkCanvas &canvas ) {
    int size_index = getSizeIndex();
    if( size_index == -1 ) {
        return;
    }
    SkPaint paint;
    paint.setAntiAlias( true );
    paint.setStyle( SkPaint::Style::kStroke_Style );
    paint.setColor( 0xFF00FFFF );
    paint.setStrokeWidth( 0.5/pSVGDomEditor->drawScale_DPI );

    if( _bPatternPointsSet[size_index][PT_CF_WAIST] ) {
        // CF
        canvas.drawLine( _patternPoints[size_index][PT_CF_WAIST].fX, 0-pSVGDomEditor->contentCenterOffsetY_72, 
            _patternPoints[size_index][PT_CF_WAIST].fX, height()/pSVGDomEditor->drawScale_DPI, paint );
        // Waist
        canvas.drawLine( 0-pSVGDomEditor->contentCenterOffsetX_72, _patternPoints[size_index][PT_CF_WAIST].fY,
            width()/pSVGDomEditor->drawScale_DPI, _patternPoints[size_index][PT_CF_WAIST].fY, paint );
    }
    // เส้นไหล่บน
    if( _bPatternPointsSet[size_index][PT_SHOULDER_NECK] ) {
        canvas.drawLine( 0-pSVGDomEditor->contentCenterOffsetX_72, _patternPoints[size_index][PT_SHOULDER_NECK].fY,
            width()/pSVGDomEditor->drawScale_DPI, _patternPoints[size_index][PT_SHOULDER_NECK].fY, paint );        
    }
    // เส้นคอ (อารีย์)
    //if( _bPatternPointsSet[size_index][PT_CF_NECK_A] ) {
    //    canvas.drawLine( 0-pSVGDomEditor->contentCenterOffsetX_72, _patternPoints[size_index][PT_CF_NECK_A].fY,
    //        width()/pSVGDomEditor->drawScale_DPI, _patternPoints[size_index][PT_CF_NECK_A].fY, paint );        
    //}
    // ปลายไหล่
    if( _bPatternPointsSet[size_index][PT_SHOULDER_TIP] ) {
        canvas.drawLine( 0-pSVGDomEditor->contentCenterOffsetX_72, _patternPoints[size_index][PT_SHOULDER_TIP].fY,
            width()/pSVGDomEditor->drawScale_DPI, _patternPoints[size_index][PT_SHOULDER_TIP].fY, paint );        
    }
}

void DecSVGEditor::drawSewingLine( DecSVGDomEditor *, SkCanvas &canvas ) {
    int size_index = getSizeIndex();
    if( size_index == -1 ) {
        return;
    }
    canvas.save();
    canvas.scale( pSVGDomEditor->_svgScaleX, pSVGDomEditor->_svgScaleY );
    canvas.translate( 0+pSVGDomEditor->contentCenterOffsetX_72, 0+pSVGDomEditor->contentCenterOffsetY_72 );

    drawGuides( canvas );
    if( _sewingLinePath[size_index] ) {
        SkPaint paint;
        paint.setAntiAlias( true );
        paint.setStyle( SkPaint::Style::kStroke_Style );

        paint.setColor( 0xA00000FF );
        paint.setStrokeWidth( 0.4/pSVGDomEditor->drawScale_DPI );

        float intervals[] = { 6, 4 };
        paint.setPathEffect(SkDashPathEffect::Make(intervals, 2, 0.0f));

        if( _sewingLinePath[size_index]) {
            canvas.drawPath( *_sewingLinePath[size_index], paint );
        }

        SkPaint paintStroke;
        paintStroke.setAntiAlias( true );
        paintStroke.setStyle( SkPaint::Style::kStroke_Style );
        paintStroke.setStrokeWidth( 1/pSVGDomEditor->drawScale_DPI );
        paintStroke.setColor( 0xFFFF0000 );

        int _w = 2/pSVGDomEditor->drawScale_DPI;
        SkRect rect = SkRect::MakeXYWH( _currentPoint.fX-_w, _currentPoint.fY-_w, _w*2, _w*2 );

        if( _SelectedSewingLinePath ) {
            canvas.drawPath( *_SelectedSewingLinePath, paintStroke );

            SkPaint paintFill;
            paintFill.setColor( 0xFFFFFFFF );
            paintFill.setStyle( SkPaint::Style::kFill_Style );

            canvas.drawRect( rect, paintFill );
        }

        int si = getSizeIndex();
        if( si != -1 ) {
            SkPaint paintFillPt;
            paintFillPt.setColor( 0xFF0000FF );
            paintFillPt.setStyle( SkPaint::Style::kFill_Style );
            for( int i=0; i<PT_MAX; i++ ) {
                if( _bPatternPointsSet[si][i] ) {
                    SkRect rect2 = SkRect::MakeXYWH( _patternPoints[si][i].fX-_w, _patternPoints[si][i].fY-_w, _w*2, _w*2 );
                    canvas.drawRect( rect2, paintFillPt );
                }
            }
        }

        if( _SelectedSewingLinePath ) {
            canvas.drawRect( rect, paintStroke );
        }
    }
    canvas.restore();
}

DecSVGEditor &SVGEditor() {
    DecSVGEditor *w = new DecSVGEditor();
    return *w;
}

void DecSVGEditor::update() {
    DecWidget::update();
    focus(true );
}

void DecSVGEditor::changeTargetPaper( const char *paper ) {
    pSVGDomEditor->setPaperSize( paper );
    updateContentBounds();
    pSVGDomEditor->dirty(true, true);
}

void DecSVGEditor::savePatternPoints() {
    for( int i=0; i<5; i++ ) {
        std::string fn = std::string("_patternPoints/") + _filebasename;
        if( i == 0 ) fn += "-S";
        else if( i == 1 ) fn += "-M";
        else if( i == 2 ) fn += "-L";
        else if( i == 3 ) fn += "-XL";
        else if( i == 4 ) fn += "-XXL";
        SkDebugf( "Save Pattern Points: %s\n", fn.c_str() );

        std::string ss = "";
        for( int p=0; p<PT_MAX; p++ ) {
            char sz[256];
            sprintf( sz, "%i, %0.2f, %0.2f\n", _bPatternPointsSet[i][p],
                _patternPoints[i][p].fX, _patternPoints[i][p].fY );
            ss += sz;
        }
        std::ofstream f( fn + ".txt" );
        if( f ) {
            f.write( (const char *)ss.c_str(), ss.length() );
            f.close();
        }
    }
}

void DecSVGEditor::loadPatternPoints() {
    for( int i=0; i<5; i++ ) {
        std::string fn = std::string("_patternPoints/") + _filebasename;
        if( i == 0 ) fn += "-S";
        else if( i == 1 ) fn += "-M";
        else if( i == 2 ) fn += "-L";
        else if( i == 3 ) fn += "-XL";
        else if( i == 4 ) fn += "-XXL";
        fn += ".txt";

        if( std::filesystem::exists( fn ) ) {
            std::ifstream file(fn);
            std::string str; 
            int l = 0;
            while (std::getline(file, str))
            {
                char sz[256];
                strcpy( sz, str.c_str() );

                char *b = strtok( sz, " ");
                char *x = strtok (NULL, " ");
                char *y = strtok (NULL, " ");  

                _bPatternPointsSet[i][l] = atoi(b) ? true : false;
                if( atoi(b) ) {
                    _patternPoints[i][l].fX = atof( x );
                    _patternPoints[i][l].fY = atof( y );
                }
                else {
                    _patternPoints[i][l].fX = 0;
                    _patternPoints[i][l].fY = 0;                    
                }
                l++;
            } 
            file.close();
        }        
    }
    calculateSizes();
}

void DecSVGEditor::exportSVGs() {
    setJobFrameSkip( 5 );

    saveS = _is_S;
    saveM = _is_M;
    saveL = _is_L;
    saveXL = _is_XL;
    saveXXL = _is_XXL;

    _is_S = false;
    _is_M = false;
    _is_L = false;
    _is_XL = false;
    _is_XXL = false;

    loadSewingLine();
    __jobSizeIndex = 0;
    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobSVGSizeExport( w );
    }) );
}

bool DecSVGEditor::__jobSVGSizeExport( DecWidget &widget ) {
    if( __jobSizeIndex == 0 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_S = true;
        updateNodeVisibility();
        exportSVG();
    }
    else if( __jobSizeIndex == 1 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_M = true;
        updateNodeVisibility();
        exportSVG();
    }
    else if( __jobSizeIndex == 2 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_L = true;
        updateNodeVisibility();
        exportSVG();
    }
    else if( __jobSizeIndex == 3 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_XL = true;
        updateNodeVisibility();
        exportSVG();
    }
    else if( __jobSizeIndex == 4 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_XXL = true;
        updateNodeVisibility();
        exportSVG();
    }

    __jobSizeIndex += 1;
    if( __jobSizeIndex > 4 ) {
        return true;
    }
    return false;
}

void DecSVGEditor::exportSVG() {
    std::string fn = std::string("_export_svg/") + _filebasename;
    if( _is_S && _is_M && _is_L && _is_XL && _is_XXL ) {
        fn = fn + "-ALL";
    }
    else {
        if( _is_S ) {
            fn = fn + "-S";
        }
        if( _is_M ) {
            fn = fn + "-M";
        }
        if( _is_L ) {
            fn = fn + "-L";
        }
        if( _is_XL ) {
            fn = fn + "-XL";
        }
        if( _is_XXL ) {
            fn = fn + "-XXL";
        }
    }
    fn = fn + ".svg";

    pSVGDomEditor->exportSVG(fn.c_str(), [=]( SkCanvas &canvas ) {
        overlaySewingLine( canvas );
    });    
}

void DecSVGEditor::exportPNG() {
    std::string fn = std::string("_export_png/") + _filebasename;
    if( _is_S && _is_M && _is_L && _is_XL && _is_XXL ) {
        fn = fn + "-ALL";
    }
    else {
        if( _is_S ) {
            fn = fn + "-S";
        }
        if( _is_M ) {
            fn = fn + "-M";
        }
        if( _is_L ) {
            fn = fn + "-L";
        }
        if( _is_XL ) {
            fn = fn + "-XL";
        }
        if( _is_XXL ) {
            fn = fn + "-XXL";
        }
    }
    fn = fn + ".png";

    pSVGDomEditor->exportPNG(fn.c_str(), 72, [=]( SkCanvas &canvas ) {
        overlaySewingLine( canvas );
    });
}

std::string DecSVGEditor::startExportPDF(bool a4) {
    std::string fn = std::string("_export_pdf/") + _filebasename;
    if( a4 ) {
        fn = fn + "-A4";
    }
    else {
        fn = fn + "-A1";
    }
    if( _is_S && _is_M && _is_L && _is_XL && _is_XXL ) {
        fn = fn + "-ALL";
    }
    else {
        if( _is_S ) {
            fn = fn + "-S";
        }
        if( _is_M ) {
            fn = fn + "-M";
        }
        if( _is_L ) {
            fn = fn + "-L";
        }
        if( _is_XL ) {
            fn = fn + "-XL";
        }
        if( _is_XXL ) {
            fn = fn + "-XXL";
        }
    }
    fn = fn + ".pdf";

    pSVGDomEditor->startExportPDF(fn.c_str());
    return fn;
}

void DecSVGEditor::overlaySewingLine( SkCanvas &canvas ) {
    int size_index = getSizeIndex();
    if( _sewingLinePath[size_index]) {
        canvas.save();
        canvas.scale( pSVGDomEditor->_svgScaleX, pSVGDomEditor->_svgScaleY );

        SkPaint paint;
        paint.setAntiAlias( true );
        paint.setStyle( SkPaint::Style::kStroke_Style );

        paint.setColor( 0x600000FF );
        paint.setStrokeWidth( 0.4/pSVGDomEditor->drawScale_DPI );

        float intervals[] = { 6, 4 };
        paint.setPathEffect(SkDashPathEffect::Make(intervals, 2, 0.0f));

        canvas.drawPath( *_sewingLinePath[size_index], paint );


        canvas.restore();
    }
}

void DecSVGEditor::renderPDF( const char *patternName, bool tiled ) {
    pSVGDomEditor->renderPDF( _filebasename.c_str(), _isTiledExport, [=]( SkCanvas &canvas, int w, int h ) {
        overlaySewingLine( canvas );
    });
}

void DecSVGEditor::exportPNGs() {
    setJobFrameSkip( 5 );

    saveS = _is_S;
    saveM = _is_M;
    saveL = _is_L;
    saveXL = _is_XL;
    saveXXL = _is_XXL;

    _is_S = false;
    _is_M = false;
    _is_L = false;
    _is_XL = false;
    _is_XXL = false;

    loadSewingLine();
    __jobSizeIndex = 0;
    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobPNGSizeExport( w );
    }) );
}

bool DecSVGEditor::__jobPNGSizeExport( DecWidget &widget ) {
    if( __jobSizeIndex == 0 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_S = true;
        updateNodeVisibility();
        exportPNG();
    }
    else if( __jobSizeIndex == 1 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_M = true;
        updateNodeVisibility();
        exportPNG();
    }
    else if( __jobSizeIndex == 2 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_L = true;
        updateNodeVisibility();
        exportPNG();
    }
    else if( __jobSizeIndex == 3 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_XL = true;
        updateNodeVisibility();
        exportPNG();
    }
    else if( __jobSizeIndex == 4 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_XXL = true;
        updateNodeVisibility();
        exportPNG();
    }

    __jobSizeIndex += 1;
    if( __jobSizeIndex > 4 ) {
        //_is_S = saveS;
        //_is_M = saveM;
        //_is_L = saveL;
        //_is_XL = saveXL;
        //_is_XXL = saveXXL;

        //updateNodeVisibility();
        //_pMessageBox->message( "All sizes were exported." ).show();

        return true;
    }
    return false;
}

void DecSVGEditor::clearSelectedSewingLinePath() {
    if( _SelectedSewingLinePath ) {
        delete( _SelectedSewingLinePath );
        _SelectedSewingLinePath = NULL;
    }
}

void DecSVGEditor::onPathVerbSelect( DecList *list ) {
    SkDebugf( "Path verb: %i\n", (int)list->value() );
    _selectedSewingLineIndex = (int)list->value();
    updateSelectedSewingLine();
    pSVGDomEditor->dirty( true, false );
}

void DecSVGEditor::exportPDFs() {
    saveS = _is_S;
    saveM = _is_M;
    saveL = _is_L;
    saveXL = _is_XL;
    saveXXL = _is_XXL;

    _is_S = false;
    _is_M = false;
    _is_L = false;
    _is_XL = false;
    _is_XXL = false;

    __jobSizeIndex = 0;
    loadSewingLine();
    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobSizeExport( w );
    }) );
}

bool DecSVGEditor::__jobSizeExport( DecWidget &widget ) {
    if( __jobSizeIndex == 0 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_S = true;
        updateNodeVisibility();

        startExportPDF(_isTiledExport);
        renderPDF( _filebasename.c_str(), _isTiledExport );
        pSVGDomEditor->endExportPDF();
    }
    else if( __jobSizeIndex == 1 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_M = true;
        updateNodeVisibility();

        startExportPDF(_isTiledExport);
        renderPDF( _filebasename.c_str(), _isTiledExport );
        pSVGDomEditor->endExportPDF();
    }
    else if( __jobSizeIndex == 2 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_L = true;
        updateNodeVisibility();

        startExportPDF(_isTiledExport);
        renderPDF( _filebasename.c_str(), _isTiledExport );
        pSVGDomEditor->endExportPDF();
    }
    else if( __jobSizeIndex == 3 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_XL = true;
        updateNodeVisibility();

        startExportPDF(_isTiledExport);
        renderPDF( _filebasename.c_str(), _isTiledExport );
        pSVGDomEditor->endExportPDF();
    }
    else if( __jobSizeIndex == 4 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;

        _is_XXL = true;
        updateNodeVisibility();

        startExportPDF(_isTiledExport);
        renderPDF( _filebasename.c_str(), _isTiledExport );
        pSVGDomEditor->endExportPDF();
    }

    __jobSizeIndex += 1;
    if( __jobSizeIndex > 4 ) {
        //_is_S = saveS;
        //_is_M = saveM;
        //_is_L = saveL;
        //_is_XL = saveXL;
        //_is_XXL = saveXXL;

        //updateNodeVisibility();
        //_pMessageBox->message( "All sizes were exported." ).show();

        return true;
    }
    return false;
}

void DecSVGEditor::updateNodeVisibility() {
    for( int i=0; i<_vTreeItems.size(); i++ ) {
        DecTreeItem *item = _vTreeItems[i];
        if( !strncmp(item->text(), "G text", 6 ) ) {
            if( _excludeText ) {
                setNodeVisibility( item, false );
            }
            else {
                setNodeVisibility( item, true );
            }
        }
        if( !strncmp(item->text(), "G S", 3 ) ) {
            setNodeVisibility( item, _is_S );
        }
        else if( !strncmp(item->text(), "G M", 3 ) ) {
            setNodeVisibility( item, _is_M );
        }
        else if( !strncmp(item->text(), "G L", 3 ) ) {
            setNodeVisibility( item, _is_L );
        }
        else if( !strncmp(item->text(), "G XL", 4 ) ) {
            setNodeVisibility( item, _is_XL );
        }
        else if( !strncmp(item->text(), "G XXL", 5 ) ) {
            setNodeVisibility( item, _is_XXL );
        }
    }
    updateContentBounds();
}

void DecSVGEditor::onLoad() {
    pSVGDomEditor->dirty();

    fDom = pSVGDomEditor->fDom;

    updateSVGInfo();
    updateNodeTree();
    updateContentBounds();
}

void DecSVGEditor::init() {
    if( pSVGDomEditor ) {
        pSVGDomEditor->pSVGEditor = this;
    }
    _pComboBoxTargetPaper->select( 0 );

    focus( true );

    fDom = pSVGDomEditor->fDom;

    std::filesystem::path fullpath( "_magicpattern/V2-Blouse-Dress-SVG/WPAN-A1=1-01.svg" );

    _filepath = fullpath.string();
    _filename = fullpath.filename().string();
    _filebasename = fullpath.stem().string();

    loadSewingLine();
    pNodeTree->clear();
    bool ret = pSVGDomEditor->load(_filepath.c_str());

    pSVGDomEditor->dirty();
}

std::string DecSVGEditor::getSKU( int _length, int _type, int _neck, int _hem, int _sleeve, int _collar) {
    char _sku[64];

    _sku[0] = 0;

    const char *_str_length[] = { "W", "H", "L" };
    strcat( _sku, _str_length[_length] );

    const char *_str_type[] = { "P", "B" };
    strcat( _sku, _str_type[_type] );

    const char *_str_neck[] = { "A", "B", "C", "D", "E" };
    strcat( _sku, _str_neck[_neck] );

    const char *_str_has_sleeve[] = { "N", "S" };
    strcat( _sku, _str_has_sleeve[_sleeve == 0 ? 0 : 1] );

    strcat( _sku, "-" );
    const char *_str_collar[] = { "A", "B", "C", "D", "E", "F", "G" };
    strcat( _sku, _str_collar[_collar] );

    const char *_str_sleeve[] = { "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T" };
    strcat( _sku, _str_sleeve[_sleeve] );

    const char *_str_hem[] = { "U", "V", "W" };
    strcat( _sku, _str_hem[_hem] );

    //SkDebugf( "SKU: %s\n", _sku );

    return std::string( _sku );
}

void DecSVGEditor::bubbleAction( int actionID, DecWidget *sender, void *data ) {
    if( actionID == ACTION_TREE_SELECT_CHANGED ) {
        //if( sender->tag() == 7000 ) { // SVG Tree
            int tag = sender->tag();
            selectNode( (DecTreeItem *)data );
        //}
    }
}

void DecSVGEditor::onTreeNodeSelected( SkSVGNode *node ) {
    if( node ) {
        SkSVGTag tag = node->tag();
        if( tag == SkSVGTag::kPath ) {

        }
        SkDebugf( "Selected node\n" );
    }
}

void DecSVGEditor::selectNode( DecTreeItem *pItem ) {
    if( pSVGDomEditor ) {
        if( pItem ) {
            pSVGDomEditor->selectNode( (SkSVGNode *)pItem->_externalData );
            onTreeNodeSelected( (SkSVGNode *)pItem->_externalData );
        }
        else {
            pSVGDomEditor->selectNode( NULL );
            onTreeNodeSelected( NULL );
        }
    }
}

void DecSVGEditor::setNodeVisibility( DecTreeItem *treeItem, bool bVisible, bool includeChildren ) {
    SkSVGNode *node = (SkSVGNode *)treeItem->_externalData;
    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;
    if( bVisible ) {
        vType = SkSVGVisibility::Type::kVisible;
    }
    else {
        vType = SkSVGVisibility::Type::kHidden;
    }
    treeItem->_flag = vType == SkSVGVisibility::Type::kVisible ? 1 : 0;

    auto visible = SkSVGVisibility(vType);
    SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
    node->setVisibility(visibilityVisible);

    DecIcon *icon = (DecIcon *)treeItem->_externalWidget;
    icon->text_color( vType == SkSVGVisibility::Type::kVisible ? 0xFF000000 : 0xFFC0C0C0 );

    if( includeChildren ) {
        std::vector<DecWidget*>children = treeItem->getChildrenNodes();
        for( int i=0; i<children.size(); i++ ) {
            DecTreeItem *w = (DecTreeItem *)children[i];
            setNodeVisibility( w, bVisible );
        }
    }
    pSVGDomEditor->dirty();

    updateContentBounds();
}

void DecSVGEditor::updateContentBounds() {
    pSVGDomEditor->setVisibleContentBounds();
    return;
}

bool DecSVGEditor::getNodeVisibility( DecTreeItem *treeItem ) {
    SkSVGNode *node = (SkSVGNode *)treeItem->_externalData;
    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;

    const SkSVGProperty<SkSVGVisibility, true> &visibility = node->getVisibility();
    bool bVisible = true;
    if( visibility.isValue() ) {
        vType = visibility->type();
    }
    if( vType == SkSVGVisibility::Type::kVisible ) {
        bVisible = true;
    }
    else {
        bVisible = false;
    }
    return bVisible;
}

void DecSVGEditor::toggleNodeVisibility( DecTreeItem *treeItem ) {
    bool bVisible = getNodeVisibility(treeItem);

    bVisible = !bVisible;
    setNodeVisibility( treeItem, bVisible );
}

void DecSVGEditor::addSubNodeToTreeItem(DecTreeItem& treeNode, SkSVGNode* node, int deep, std::string parentNodeName ) {
    DecTreeItem *new_node;

    char sz[128];

    SkSVGTag tag = node->tag();

    if( tag == SkSVGTag::kTextLiteral ) {
        return;
    }

    // maxdeep:50
    char chDeep[] = "--------------------------------------------------";
    chDeep[deep] = 0;

    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;

    const SkSVGProperty<SkSVGPaint, true> &svg_stroke = node->getStroke();
    const SkSVGProperty<SkSVGPaint, true> &svg_fill = node->getFill();
    const SkSVGProperty<SkSVGVisibility, true> &visibility = node->getVisibility();
    const SkSVGProperty<SkSVGStringType, false> &propData = node->getDataName();
    const SkSVGStringType* data = propData.getMaybeNull();
    const char* cpchData = data ? data->c_str() : "";
    std::string nodeName = std::string( cpchData );

    if( svg_stroke.isValue() ) {
        if( svg_stroke->type() == SkSVGPaint::Type::kColor ) {
            SkSVGColor col = svg_stroke->color();
            //SkDebugf("%snode: %s, has Color[%08X], data-name:'%s' \n",
            //            chDeep,
            //            tagName[(int)tag],
            //            (uint32_t)col.color(),
            //            cpchData);

            if( _forceBlack ) {
                // convert colors to all black
                auto sstroke = SkSVGPaint( SkSVGColor( SK_ColorBLACK));
                SkSVGProperty<SkSVGPaint, true> sstrokestroke( sstroke );
                node->setStroke( sstrokestroke );
            }
        }
    }
    if( svg_fill.isValue() ) {
        if( svg_fill->type() == SkSVGPaint::Type::kColor ) {
            SkSVGColor col = svg_fill->color();
            //SkDebugf("%snode: %s, has FillColor[%08X], data-name:'%s' \n",
            //            chDeep,
            //            tagName[(int)tag],
            //            (uint32_t)col.color(),
            //            cpchData);
            if( _forceBlack ) {
                auto sfill = SkSVGPaint( SkSVGColor( SK_ColorBLACK));
                SkSVGProperty<SkSVGPaint, true> sfillfill( sfill );
                node->setFill( sfillfill );
            }
        }
    }

    if( visibility.isValue() ) {
        vType = visibility->type();
    } else {
        if (tag == SkSVGTag::kG) {
            auto visible = SkSVGVisibility(SkSVGVisibility::Type::kVisible);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);
        }
    }

    // hide pocket
    if( nodeName != "" ) {
        //SkDebugf( "Node: %s\n", nodeName.c_str() );
        if( !strncmp(nodeName.c_str(), "กระเป๋า", strlen("กระเป๋า") ) ) {
            //SkDebugf( "Hide กระเป๋า\n" );
            //auto visible = SkSVGVisibility(SkSVGVisibility::Type::kHidden);
            //SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            //node->setVisibility(visibilityVisible);        
        }

    }

    if( deep == 1 ) {
        /*
        SkDebugf("%i: %s %s %s \n",
                    deep,
                    parentNodeName.c_str(),
                    tagName[(int)tag], cpchData );
        */
    }

    sprintf(sz, "%s %s ", tagName[(int)tag], cpchData);
    treeNode._(TreeItem(sz).ref((DecWidget**)&new_node), true);  // add to front to display like Adobe Illustrator

    if( deep == 1 ) {
        new_node->_index = _vTreeItems.size();                                  // node index
        new_node->_flag = vType == SkSVGVisibility::Type::kVisible ? 1 : 0;     // node visible flag
        _vTreeItems.push_back( new_node );

        /*
        SkSize szC = fDom->containerSize();
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
        SkRect rect = node->objectBoundingBox( rctx );
        _vNodeBounds.push_back( rect );                                         // node bounds
        */
    }
    if( deep == 3 ) {

    }

    new_node->_externalData = node;
    new_node->addWidget( Icon( 0xf06e)
        .ref( (DecWidget **)&new_node->_externalWidget )
        .externalData( new_node )
        .text_color( vType == SkSVGVisibility::Type::kVisible ? 0xFF000000 : 0xFFC0C0C0 )
        .onMousePressed( [=](DecWidget &widget) -> bool {
            toggleNodeVisibility( (DecTreeItem *)widget._externalData );
            return false;
        })
    );
    if( node->hasChildren() ) {
        if( tag == SkSVGTag::kG ) {
            SkSVGG *pG = (SkSVGG *)node;
            int numChildren = (int)pG->fChildren.size();
            for( int i=0; i<numChildren; i++ ) {
                sk_sp<SkSVGNode> child_node = pG->fChildren.at( i );

                //rootItem._( TreeItem( sz ) );
                addSubNodeToTreeItem( *new_node, child_node.get(), deep+1, nodeName );
            }
        }
    }
    if( deep > 0 )
    {
        treeNode.collapse();
    }
}

void DecSVGEditor::updateSVGInfo() {
    if( pSVGDomEditor ) {
        fDom = pSVGDomEditor->fDom;
        if( fDom ) {
            root = fDom->getRoot();

            SkSize szC = fDom->containerSize();
            SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
            if( szR.fWidth == 0 || szR.fHeight == 0 ) {
                szR.fWidth = szC.fWidth;
                szR.fHeight = szC.fHeight;
            }
        }
    }
}

void DecSVGEditor::updateNodeTree() {
    if( pSVGDomEditor ) {
        fDom = pSVGDomEditor->fDom;
        if( fDom ) {
            root = fDom->getRoot();

            char sz[256];
            SkSVGTag rtag = root->tag();
            sprintf( sz, "%s", tagName[(int)rtag] );

            DecTreeItem &rootItem = TreeItem( sz );
            if( pNodeTree ) {
                pNodeTree->_( rootItem );
            }
            _vTreeItems.clear();
            //_vNodeBounds.clear();

            if( root->hasChildren() ) {
                int numChildren = (int)root->fChildren.size();

                if( pNodeTree ) {
                    for( int i=0; i<numChildren; i++ ) {
                        sk_sp<SkSVGNode> node = root->fChildren.at( i );

                        addSubNodeToTreeItem( rootItem, node.get(), 1, "root" );
                    }
                }
            }
            rootItem.expand();
            _pTreeItem0 = &rootItem;

            updateNodeVisibility();
        }

    }
}
