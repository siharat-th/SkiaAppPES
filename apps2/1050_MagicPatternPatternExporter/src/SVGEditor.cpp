#include <fstream>
#include <string>
#include <iostream>  
#include <cstring> 
#include "DeclarativeUI.h"
#include "DecSVGDom.h"
#include "DecUI.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "include/core/SkSamplingOptions.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkFontMetrics.h"
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

DecSVGEditor::DecSVGEditor() {
    fDom = NULL;
    root = NULL;
    _pTextDocumentPath = NULL;
    _pTextDocumentSize = NULL;
    _pSKUList = NULL;
    _pFileList = NULL;

    _is_S = true;
    _is_M = true;
    _is_L = true;
    _is_XL = true;
    _is_XXL = true;

    _generateFrame = 1;
    _generateCount = 0;                     
    _generatePattern = false;  

    _is_png_forweb = false;

    _forceBlack = true;
    _isTiledExport = true;

    //contentBounds.setEmpty();  

    for( int i=0; i<5; i++ ) {
        _sewingLinePath[i] = NULL;
    }    

    size( -1, -1 );
    border( false );
    background( false );
    _( Column()
        .size( 180, -1 )
        .border(false)
        .background(false)
        .padding( 10, 10, 10, 10 )
        .gap( 10 )
        ._( Column()
            .border( false )
            .gap( 5 )
            ._(Text( "Product SKU:") )
            ._(List()
                .ref( (DecWidget **)&_pSKUList )
                .onValueChanged( [=]( DecWidget &widget ) {
                    if( widget.value() == -1 ) {
                        _pFilePages->activate( 0 );
                        _pImagePages->activate( 0 );
                    }
                    else {
                        _pFilePages->activate( 1 );
                        _pImagePages->activate( 1 );
                        setProduct( (int)widget.value()+1, _sku_list[(int)widget.value()].c_str() );
                    }
                })
            )
        )
        ._( Column()
            .border( false )
            .gap( 5 )
            ._( Row().size( -1, 0 ).border(false)
                ._(Text( "Image Files:").size( -1, 0 ).align( ALIGN_CENTER ) )
                ._(Icon( 0xf021 ).align( ALIGN_CENTER ).onMousePressed( [=]( DecWidget &w) -> bool {
                    _pImageFileList->refresh();
                    return false;
                }) )
            )
            ._( Pages() 
                .border( false )
                .ref( (DecWidget **)&_pImagePages )
                ._(FileList( "resources/_export_png" )
                    .ref( (DecWidget **)&_pImageFileList )
                    .size( -1, -1 )
                )
                ._(List()
                    .size( -1, -1 )
                    .ref( (DecWidget **)&_pProductImageList )
                )       
            )     
        )
        ._( Column()
            .border( false )
            .gap( 5 )
            ._(Text( "Pattern Files:") )
            ._( Pages() 
                .border( false )
                .ref( (DecWidget **)&_pFilePages )
                ._(FileList( "_magicpattern/SVG" )
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
                        loadSewingLine( _filebasename.c_str() );              

                        //SkDebugf( "Path: %s\n", _filepath.c_str() );
                        //SkDebugf( "Filename: %s\n", _filename.c_str() );
                        //SkDebugf( "Basename: %s\n", _filebasename.c_str() );
                    })
                )
                ._(List()
                    .size( -1, -1 )
                    .ref( (DecWidget **)&_pProductFileList )
                    .onValueChanged( [=]( DecWidget &widget ) {
                        if( widget.value() == -1 ) {
                            return;
                        }
                        char sz[256];
                        strcpy( sz, "_magicpattern/SVG/" );
                        strcat( sz, widget.text() );
                        std::filesystem::path fullpath = std::filesystem::path( sz );

                        _filepath = fullpath.string();
                        _filename = fullpath.filename().string();
                        _filebasename = fullpath.stem().string();
                        pNodeTree->clear();
                        pSVGDomEditor->load(_filepath.c_str());
                        pSVGDomEditor->dirty();                

                        //SkDebugf( "Path: %s\n", _filepath.c_str() );
                        //SkDebugf( "Filename: %s\n", _filename.c_str() );
                        //SkDebugf( "Basename: %s\n", _filebasename.c_str() );                        
                    })
                )        
            )

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
            ._( CheckBox( "Show Min Paper Size", [=]( DecCheckBox &cb ) {
                pSVGDomEditor->_showMinPaperBounds = cb.value();
                pSVGDomEditor->dirty();                
            }).check( false, false ) )
            ._( CheckBox( "Show Content Area", [=]( DecCheckBox &cb ) {
                pSVGDomEditor->_showVisibleContentBounds = cb.value();
                pSVGDomEditor->dirty();                
            }).check( true, false ) )
            ._( CheckBox( "Show Tile Grid", [=]( DecCheckBox &cb ) {
                pSVGDomEditor->_showTileGrid = cb.value();
                pSVGDomEditor->dirty();                
            }).check( false, false ) )
        )
        ._( SVGDomEditor()
            .drawHook( [=](DecSVGDomEditor *dom, SkCanvas &canvas) {
                //SkDebugf( "DrawHook\n" );
                //drawContentBounds( dom, canvas );
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
        .size( 320, -1 )
        ._( Column()
            .size( -1, 0 )
            .border(false)
            .background(false)
            .gap( 10 )
            .padding( 10, 10, 10, 10 )
            ._( Text("")
                .ref((DecWidget**)&_pTextDocumentPath)
            )
            ._( Container()
                .size( -1, 360 )
                .border_color( 0xFF808080 )
                ._( Tree()
                    .border( false )
                    .ref( (DecWidget **)&pNodeTree )
                )
            )
            ._( Column()
                .size( -1, 0 )
                .border( false )
                .gap( 5 )
                .cache( false )
                ._( Text( "Document size: " )
                    .ref( (DecWidget **)&_pTextDocumentSize )
                )
                ._( Text( "Document DPI: " )
                    .ref( (DecWidget **)&_pTextDocumentDPI )
                )        
                ._( Text( "Minimal paper size: " )
                    .ref( (DecWidget **)&_pTextMinimalPaperSize )
                )
                ._( Text( "Target paper size:" )
                    .ref( (DecWidget **)&_pTextTargetPaperSize )
                )
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
                ._( Text( "Size: " ).margin_top( 5 ) )
                ._( CheckBox( "All" ).check(true).ref( (DecWidget**)&_pCB_All).onValueChanged([=](DecWidget &w){
                    //SkDebugf( "All\n" );
                    _pCB_S->check( w.value(), false );
                    _pCB_M->check( w.value(), false );              
                    _pCB_L->check( w.value(), false );              
                    _pCB_XL->check( w.value(), false );              
                    _pCB_XXL->check( w.value(), false );              
                    _is_S = w.value();
                    _is_M = w.value();
                    _is_L = w.value();
                    _is_XL = w.value();
                    _is_XXL = w.value();
                    updateNodeVisibility();
                }) )
                ._( Row() 
                    .size( -1, 0 )
                    .border( false )
                    ._( Container().size( -1, 0 ).border(false)._(CheckBox( "S" ).check(true).ref( (DecWidget**)&_pCB_S).onValueChanged([=](DecWidget &w){
                        //SkDebugf( "S %i\n", (int)w.value() );
                        _is_S = w.value();
                        updateAllCheckBox();
                        updateNodeVisibility();
                    })) )
                    ._( Container().size( -1, 0 ).border(false)._(CheckBox( "M" ).check(true).ref( (DecWidget**)&_pCB_M).onValueChanged([=](DecWidget &w){
                        //SkDebugf( "M\n" );
                        _is_M = w.value();
                        updateAllCheckBox();
                        updateNodeVisibility();                    
                    })) )
                    ._( Container().size( -1, 0 ).border(false)._(CheckBox( "L" ).check(true).ref( (DecWidget**)&_pCB_L).onValueChanged([=](DecWidget &w){
                        //SkDebugf( "L\n" );
                        _is_L = w.value();
                        updateAllCheckBox();
                        updateNodeVisibility();                    
                    })) )
                    ._( Container().size( -1, 0 ).border(false)._(CheckBox( "XL" ).check(true).ref( (DecWidget**)&_pCB_XL).onValueChanged([=](DecWidget &w){
                        //SkDebugf( "XL\n" );
                        _is_XL = w.value();
                        updateAllCheckBox();
                        updateNodeVisibility();                    
                    }) ) )
                    ._( Container().size( -1, 0 ).border(false)._(CheckBox( "XXL" ).check(true).ref( (DecWidget**)&_pCB_XXL).onValueChanged([=](DecWidget &w){
                        //SkDebugf( "XXL\n" );
                        _is_XXL = w.value();
                        updateAllCheckBox();
                        updateNodeVisibility();                    
                    }) ) ) 
                ) 
                ._( Text( "Export: " ).margin_top( 5 ) ) 
                ._( Row()
                    .size( -1, 0 )
                    .border( false )
                    .gap( 10 )
                    ._( CheckBox( "Tiled Export", [=]( DecCheckBox &cb ) {
                        _isTiledExport = cb.value();
                    }).check( true, false ) )
                    ._( CheckBox( "PNG for Web", [=]( DecCheckBox &cb ) {
                        _is_png_forweb = cb.value();
                    }).check( false, false ) )                
                )
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("Start", 0, [=]( DecButton &btn ) {
                        //SkDebugf( "Start Export PDF\n" );
                        std::string fn = exportPDF();
                    }))
                    ._( Button("PDF Export", 0, [=]( DecButton &btn ) {
                        //SkDebugf( "Render PDF\n" );
                        pSVGDomEditor->renderPDF( _filebasename.c_str(), _isTiledExport, [=]( SkCanvas &canvas, int w, int h ) {
                            overlaySewingLine( canvas );
                        } );
                    }))                               
                    ._( Button("End PDF", 0, [=]( DecButton &btn ) {
                        //SkDebugf( "End Export PDF\n" );
                        pSVGDomEditor->endExportPDF();
                    }))            
                )
                ._( Text( "Product Export: " ).margin_top( 5 ) ) 
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("Product PDF Export", 0, [=]( DecButton &btn ) {
                        exportProductPDF( false, true );
                    }))                               
                )
                ._( Text( "Bulk Export: " ).margin_top( 5 ) )
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("PDF all sizes", 0, [=]( DecButton &btn ) {
                        //SkDebugf( "Export PDF all sizes\n" );
                        exportPDFs();
                    }))
                    ._( Button("PDF all patterns, all sizes", 0, [=]( DecButton &btn ) {
                        //SkDebugf( "Export PDF all patterns, all sizes\n" );
                        exportProducts();
                    }))
                )
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("PNG all sizes", 0, [=]( DecButton &btn ) {
                        //SkDebugf( "Export PNG all sizes\n" );
                        exportPNGs();
                    }))   
                    ._( Button("PNG all patterns, all sizes", 0, [=]( DecButton &btn ) {
                        //SkDebugf( "Export PNG all patterns, all sizes\n" );
                        exportParts();
                    }))                                     
                )            
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("PDF by product_list.txt", 0, [=]( DecButton &btn ) {
                        //SkDebugf( "Export PDF all patterns, all sizes\n" );
                        exportProductsByList("product_list.txt");
                    }))
                    ._( Button("Test", 0, [=]( DecButton &btn ) {
                        SkDebugf( "ProductList h: %i %i\n", _pSKUList->height(), _pSKUList->_pScroller->height() );
                    }))
                )            
            )
        )
    );
}

DecSVGEditor::~DecSVGEditor() {

}

void DecSVGEditor::renderPDF( const char *patternName, bool tiled ) {
    pSVGDomEditor->renderPDF( _filebasename.c_str(), _isTiledExport, [=]( SkCanvas &canvas, int w, int h ) {
        overlaySewingLine( canvas );
    });
}

void DecSVGEditor::updateProductImages() {
    _vProductImageList.clear();
    _pProductImageList->clear();

    char szImageFNPrefix[8];
    sprintf( szImageFNPrefix, "%04i", _product_id );
    std::string fn = std::string(szImageFNPrefix);
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
    SkDebugf( "Get Image: %s\n", fn.c_str() );
    for( int i=0; i<_pImageFileList->_filelist.size(); i++ ) {
        char sz[128];
        strcpy( sz, _pImageFileList->_filelist[i].c_str() );
        if( !strncmp( sz, fn.c_str(), fn.length() ) ) {

            _pProductImageList->_( ListItem( sz ) );
            sprintf( sz, "_export_png/%s", _pImageFileList->_filelist[i].c_str() );
            _vProductImageList.push_back( sz );
        }
    }
}

void DecSVGEditor::setProduct( int id, const char *sku ) {
    _product_id = id;

    _sku = std::string( sku );
    //SkDebugf( "SKU: %s\n", sku );
    _vProductFileList.clear();
    _pProductFileList->clear();

    char szBody[8];
    strncpy( szBody, sku, 4 );
    szBody[4] = 0;
    strcat( szBody, "-" );
    //SkDebugf( "Body: %s\n", szBody );

    for( int i=0; i<_pFileList->_filelist.size(); i++ ) {
        char sz[128];
        strcpy( sz, _pFileList->_filelist[i].c_str() );
        if( !strncmp( sz, szBody, strlen(szBody) ) ) {
            _pProductFileList->_( ListItem( sz ) );
            _vProductFileList.push_back( sz );
        }
    }


    char szCollar[8];
    strncpy( szCollar, &_sku[5], 1 );
    szCollar[1] = 0;
    strcat( szCollar, "-" );
    //SkDebugf( "Collar: %s\n", szCollar );

    for( int i=0; i<_pFileList->_filelist.size(); i++ ) {
        char sz[128];
        strcpy( sz, _pFileList->_filelist[i].c_str() );
        if( !strncmp( sz, szCollar, strlen(szCollar) ) ) {
            _pProductFileList->_( ListItem( sz ) );
            _vProductFileList.push_back( sz );
        }
    }    

    char szSleeve[8];
    strncpy( szSleeve, &_sku[6], 1 );
    szSleeve[1] = 0;
    strcat( szSleeve, "-" );
    //SkDebugf( "Sleeve: %s\n", szSleeve );

    for( int i=0; i<_pFileList->_filelist.size(); i++ ) {
        char sz[128];
        strcpy( sz, _pFileList->_filelist[i].c_str() );
        if( !strncmp( sz, szSleeve, strlen(szSleeve) ) ) {
            _pProductFileList->_( ListItem( sz ) );
            _vProductFileList.push_back( sz );
        }
    }  

    char szHem[8];
    strncpy( szHem, &_sku[7], 1 );
    szHem[1] = 0;
    strcat( szHem, "-" );
    //SkDebugf( "Hem: %s\n", szHem );

    for( int i=0; i<_pFileList->_filelist.size(); i++ ) {
        char sz[128];
        strcpy( sz, _pFileList->_filelist[i].c_str() );
        if( !strncmp( sz, szHem, strlen(szHem) ) ) {
            _pProductFileList->_( ListItem( sz ) );
            _vProductFileList.push_back( sz );
        }
    }        

    updateProductImages();
}

DecSVGEditor &SVGEditor() {
    DecSVGEditor *w = new DecSVGEditor();
    return *w;
}

void DecSVGEditor::update() {
    DecWidget::update();

    /*
    if( _generatePattern ) {
        if( _generateFrame % 10 == 0 ) {
            SkDebugf( "Generate Pattern: %i:\n", 
                _generateCount
            );
            _pFileList->select( _generateCount );
        }
        else if( _generateFrame % 15 == 0 ) {
            exportPDFs();
            _generateCount++;
            if( _generateCount >= _pFileList->count() ) {
                _generatePattern = false;
                _pMessageBox->message( "All patterns were exported." ).show();
            }
        }
        _generateFrame++;
    }
    */
}

void DecSVGEditor::updateAllCheckBox() {
    if( _is_S && _is_M && _is_L && _is_XL && _is_XXL ) {
        _pCB_All->check( true, false );
    }
    else {
        _pCB_All->check( false, false );
    }
}

void DecSVGEditor::changeTargetPaper( const char *paper ) {
    pSVGDomEditor->setPaperSize( paper );
    updateContentBounds();
}

std::string DecSVGEditor::exportPDF(bool a4) {
    std::string fn = std::string("_export/") + _filebasename;
    if( a4 ) {
        fn = fn + "-A4";
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

void DecSVGEditor::exportParts() {
    setJobFrameSkip( 1 );
    __jobProductIndex = 0;

    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobPartExport( w );
    }) );
}

bool DecSVGEditor::__jobPartExport(DecWidget &widget) {
    _pSKUList->select( __jobProductIndex );
    exportPNGs();

    __jobProductIndex += 1;
    if( __jobProductIndex >= _pSKUList->count() ) {
        return true;
    }
    return false;
}

void DecSVGEditor::exportProducts() {
    setJobFrameSkip( 1 );
    __jobProductIndex = 0;

    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobProductExport( w );
    }) );
}

void DecSVGEditor::exportProductsByList(const char *fn) {
    std::ifstream file(fn);
    std::string str; 
    __jobProductList.clear();
    while (std::getline(file, str))
    {
        //SkDebugf( "Command: %s\n", str.c_str() );
        __jobProductList.push_back( std::string( str ) );
    }
    //SkDebugf( "Done.\n" );
    __jobProductIndex = 0;

    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobProductExportByList( w );
    }), [=]( DecWidget &w ) {
        _pMessageBox->message( "Export by list was done." ).show();
        SkDebugf( "DecArguments: %i\n", (int)__DecArguments.size() );
        if( __DecArguments.size() ) {
            exit( 0 );
            /*
            SkDebugf( "Start timer\n" );
            setTimeOut( 3000, [=](DecWidget &w) {
                SkDebugf( "Exit\n" );
                exit( 0 );
            });
            */
        } 
    });    
}

bool DecSVGEditor::__jobProductExportByList(DecWidget &widget) {
    char sz[128];
    strcpy( sz, __jobProductList[__jobProductIndex].c_str() );
    char *ptr = strtok( sz, ",");  
    char *ptr2 = strtok (NULL, ",");
    SkDebugf( "Product: %s, Type: %s\n", ptr, ptr2 ); 

    std::string size_id = std::string( ptr2 );   

    _is_S = false;
    _is_M = false;
    _is_L = false;
    _is_XL = false;
    _is_XXL = false;      

    if( size_id == "all" ) {
        _is_S = true;
        _is_M = true;
        _is_L = true;
        _is_XL = true;
        _is_XXL = true;      
        _forceBlack = false;
        _pSKUList->select( atoi( ptr )-1 );
        _isTiledExport = false;
        updateProductImages();
        updateNodeVisibility();
        exportProductPDF();        
    }
    else if( size_id == "a4" ) {
        _isTiledExport = true;
        _forceBlack = true;
        _pSKUList->select( atoi( ptr )-1 );
        exportPDFs();
    }
    else if( size_id == "a1" ) {
        _isTiledExport = false;
        _forceBlack = true;
        _pSKUList->select( atoi( ptr )-1 );
        exportPDFs();
    }
    else if( size_id == "cover" ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;  
        _is_M = true;
        _forceBlack = true;
        _pSKUList->select( atoi( ptr )-1 );
        updateProductImages();
        updateNodeVisibility();
        exportProductPDF( true );      
    }    
    else {
        if( size_id == "s" ) _is_S = true;
        else if( size_id == "m" ) _is_M = true;
        else if( size_id == "l" ) _is_L = true;
        else if( size_id == "xl" ) _is_XL = true;
        else if( size_id == "xxl" ) _is_XXL = true;
        
        _pSKUList->select( atoi( ptr )-1 );
        updateNodeVisibility();
        exportProductPDF();
    }

    __jobProductIndex += 1;
    if( __jobProductIndex >= __jobProductList.size() ) {
        return true;
    }
    return false;    
}

bool DecSVGEditor::__jobProductExport(DecWidget &widget) {
    //setProduct( __jobProductIndex+1, _sku_list[__jobProductIndex].c_str() );
    _pSKUList->select( __jobProductIndex );
    exportPDFs();


    __jobProductIndex += 1;
    if( __jobProductIndex >= _pSKUList->count() ) {
        return true;
    }
    return false;
}

void DecSVGEditor::exportProductPDF( bool cover, bool force ) {
    _pProductFileList->select( -1 );
    
    if( cover ) {
        size_name = "COVER";
    }
    else {
        if( _is_S && _is_M && _is_L && _is_XL && _is_XXL ) {
            size_name = "ALL";
        }
        else {
            if( _is_S ) {
                size_name = "S";
            }
            if( _is_M ) {
                size_name = "M";
            }
            if( _is_L ) {
                size_name = "L";
            }
            if( _is_XL ) {
                size_name = "XL";
            }
            if( _is_XXL ) {
                size_name =  "XXL";
            }
        }
    }
    
    char szPID[12];
    sprintf( szPID, "%04i", _product_id );

    std::string fn;
    if( _isTiledExport ) {
        if( size_name == "COVER" ) {
            fn = std::string("_export/") + std::string(szPID) + "-" + size_name + "-" + _sku + "-" + ".pdf";
        }
        else {
            fn = std::string("_export/") + std::string(szPID) + "-A4-" + _sku + "-" + size_name + ".pdf";
        }
    }
    else {
        fn = std::string("_export/") + std::string(szPID) + "-A1-" + _sku + "-"   + size_name + ".pdf";
    }

    if( force || !std::filesystem::exists( std::filesystem::path( fn ) ) ) {
        //SkDebugf( "Export PDF: %s\n", fn.c_str() );
        pSVGDomEditor->startExportPDF(fn.c_str()); 

        if( cover ) {
            __jobFileIndex = -2;
        }
        else {
            __jobFileIndex = 0;
        }

        pushJob( new DecJob([=](DecWidget &w) -> bool {
            return __jobFileExport( w );
        }) );
    }

    return;   
}

void DecSVGEditor::overlayImage( SkCanvas &canvas ) {
    DecImage *img[10];
    int imgs_h = 0;
    int max_w = 0;
    for( int i=0; i<10; i++ ) {
        img[i] = NULL;
    }
    for( int i=0; i<_vProductImageList.size(); i++ ) {
        img[i] = new DecImage();
        img[i]->load( _vProductImageList[i].c_str() );   
        imgs_h += img[i]->image->height();   
        if( img[i]->image->width() > max_w ) {
            max_w = img[i]->image->width();
        }      
    }

    float scy = (6.86*72) / imgs_h;
    float scx = (3.5*72) / max_w;

    float sc = std::min( scx, scy );

    int y = 4.53 * 72 + ((6.86 * 72) - (imgs_h * sc)) / 2;

    for( int i=0; i<_vProductImageList.size(); i++ ) {
        SkRect rect = SkRect::MakeXYWH( 0, 0, img[i]->image->width(), img[i]->image->height() );
        canvas.save();
        canvas.translate( 6.05 * 72.0 - (img[i]->image->width() * sc) / 2, y );
        canvas.scale( sc, sc );

        SkPaint paint;
        canvas.drawImageRect( img[i]->image, rect, SkSamplingOptions(), &paint );
        canvas.restore();
        y += img[i]->image->height() * sc;
        delete( img[i] );
    }

}

void DecSVGEditor::overlayLayout( SkCanvas &canvas ) {
    DecImage *img[3];
    int imgs_h = 0;

    img[0] = new DecImage();
    std::string fn;
    if( _sku[1] == 'P' ) {
        if( _sku[6] == 'H' ) {
            if( _sku[5] == 'A' ) {
                fn = "_magicpattern/Layout/Lay06.png";
            }
            else if( _sku[5] == 'E' ) {
                fn = "_magicpattern/Layout/Lay30.png";
            }
            else if( _sku[5] == 'F' ) {
                fn = "_magicpattern/Layout/Lay31.png";
            }
            else if( _sku[5] == 'G' ) {
                fn = "_magicpattern/Layout/Lay32.png";
            }
            else {
                fn = "_magicpattern/Layout/Lay29.png";
            }
        }
        else if( _sku[6] == 'T' ) {
            if( _sku[5] == 'A' ) {
                fn = "_magicpattern/Layout/Lay28.png";
            }
            else if( _sku[5] == 'E' ) {
                fn = "_magicpattern/Layout/Lay03.png";
            }
            else if( _sku[5] == 'F' ) {
                fn = "_magicpattern/Layout/Lay04.png";
            }
            else if( _sku[5] == 'G' ) {
                fn = "_magicpattern/Layout/Lay05.png";
            }
            else {
                fn = "_magicpattern/Layout/Lay02.png";
            }
        }
        else {
            if( _sku[5] == 'A' ) {
                fn = "_magicpattern/Layout/Lay01.png";
            }
            else if( _sku[5] == 'E' ) {
                fn = "_magicpattern/Layout/Lay03.png";
            }
            else if( _sku[5] == 'F' ) {
                fn = "_magicpattern/Layout/Lay04.png";
            }
            else if( _sku[5] == 'G' ) {
                fn = "_magicpattern/Layout/Lay05.png";
            }
            else {
                fn = "_magicpattern/Layout/Lay02.png";
            }
        }
    }
    else if( _sku[1] == 'B' ) {
        if( _sku[6] == 'T' ) {
            if( _sku[5] == 'A' ) {
                fn = "_magicpattern/Layout/Lay33.png";
            }
            else if( _sku[5] == 'F' ) {
                fn = "_magicpattern/Layout/Lay36.png";
            }
            else if( _sku[5] == 'G' ) {
                fn = "_magicpattern/Layout/Lay37.png";
            }
            else {
                fn = "_magicpattern/Layout/Lay34.png";
            }
        }
        else {
            if( _sku[5] == 'A' ) {
                fn = "_magicpattern/Layout/Lay11.png";
            }
            else if( _sku[5] == 'F' ) {
                fn = "_magicpattern/Layout/Lay14.png";
            }
            else if( _sku[5] == 'G' ) {
                fn = "_magicpattern/Layout/Lay15.png";
            }
            else {
                fn = "_magicpattern/Layout/Lay12.png";
            }
        }
    }
    if (fn == "") return;
    img[0]->load( fn.c_str() );   
    imgs_h += img[0]->image->height(); 

    if( _sku[6] == 'H' || _sku[6] == 'T' ) {    
        img[1] = NULL;
    }
    else {
        img[1] = new DecImage();
        if( _sku[6] == 'I' ) {    
            fn = "_magicpattern/Layout/Lay16.png";
        }        
        else if( _sku[6] == 'J' ) {    
            fn = "_magicpattern/Layout/Lay17.png";
        }        
        else if( _sku[6] == 'K' ) {    
            fn = "_magicpattern/Layout/Lay18.png";
        }        
        else if( _sku[6] == 'L' ) {    
            fn = "_magicpattern/Layout/Lay19.png";
        }        
        else if( _sku[6] == 'M' ) {    
            fn = "_magicpattern/Layout/Lay20.png";
        }        
        else if( _sku[6] == 'N' ) {    
            fn = "_magicpattern/Layout/Lay21.png";
        }        
        else if( _sku[6] == 'O' ) {    
            fn = "_magicpattern/Layout/Lay22.png";
        }        
        else if( _sku[6] == 'P' ) {    
            fn = "_magicpattern/Layout/Lay23.png";
        }        
        else if( _sku[6] == 'Q' ) {    
            fn = "_magicpattern/Layout/Lay24.png";
        }        
        else if( _sku[6] == 'R' ) {    
            fn = "_magicpattern/Layout/Lay25.png";
        }        
        else if( _sku[6] == 'S' ) {    
            fn = "_magicpattern/Layout/Lay26.png";
        } 
        img[1]->load( fn.c_str() );
        imgs_h += img[1]->image->height(); 
    }

    if( _sku[7] == 'U' ) {    
        img[2] = NULL;
    }
    else {
        img[2] = new DecImage();
        fn = "_magicpattern/Layout/Lay27.png";
        img[2]->load( fn.c_str() );
        imgs_h += img[2]->image->height(); 
    }    

    float sc = (6.86*72) / imgs_h;

    int y = 4.53 * 72;

    SkRect rect = SkRect::MakeXYWH( 0, 0, img[0]->image->width(), img[0]->image->height() );
    canvas.save();
    canvas.translate( 2.05 * 72.0 - (img[0]->image->width() * sc) / 2, y );
    canvas.scale( sc, sc );
    SkPaint paint;
    canvas.drawImageRect( img[0]->image, rect, SkSamplingOptions(), &paint );
    canvas.restore();
    y += img[0]->image->height() * sc;
    delete( img[0] );

    if( img[1] ) {
        rect = SkRect::MakeXYWH( 0, 0, img[1]->image->width(), img[1]->image->height() );
        canvas.save();
        canvas.translate( 2 * 72.0 - (img[1]->image->width() * sc) / 2, y );
        canvas.scale( sc, sc );
        canvas.drawImageRect( img[1]->image, rect, SkSamplingOptions(), &paint );
        canvas.restore();  
        y += img[1]->image->height() * sc;
        delete( img[1] );   
    }
            

    if( img[2] ) {
        rect = SkRect::MakeXYWH( 0, 0, img[2]->image->width(), img[2]->image->height() );
        canvas.save();
        canvas.translate( 2 * 72.0 - (img[2]->image->width() * sc) / 2, y );
        canvas.scale( sc, sc );
        canvas.drawImageRect( img[2]->image, rect, SkSamplingOptions(), &paint );
        canvas.restore();  
        delete( img[2] );   
    }
    
}

void DecSVGEditor::overlayCoverImage( SkCanvas &canvas ) {
    DecImage *img = new DecImage();

    char sz[128];
    sprintf( sz, "_magicpattern/Images/%04i-%s.png", _product_id, _sku.c_str() );

    img->load( sz );

    float w = img->width();
    float h = img->height();

    SkPaint paint;
    SkRect rect = SkRect::MakeXYWH( 0.2 * 72, 1.77 * 72, 4.7 * 72, (4.7 * 72) * h / w );
    canvas.drawImageRect( img->image, rect, SkSamplingOptions(), &paint );

    SkFont font;
    font.setTypeface(_font->typeface(FONT_REGULAR));
    font.setSubpixel(true);
    font.setSize(0.3*72);       

    paint.setAntiAlias(true);
    paint.setColor(SK_ColorWHITE);
    paint.setStyle( SkPaint::Style::kFill_Style );

    SkFontMetrics metrics;
    font.getMetrics(&metrics);

    sprintf( sz, "MagicPattern #%04i", _product_id );
    auto blob = SkTextBlob::MakeFromText(sz, strlen(sz), font);
    int ww = font.measureText( sz, strlen(sz), SkTextEncoding::kUTF8, &rect ); 
    canvas.drawTextBlob(blob, 
        6.5*72 - ww/2, 2.3*72, paint);  

    sprintf( sz, "(SKU: %s)", _sku.c_str() );
    blob = SkTextBlob::MakeFromText(sz, strlen(sz), font);
    ww = font.measureText( sz, strlen(sz), SkTextEncoding::kUTF8, &rect ); 
    canvas.drawTextBlob(blob, 
        6.5*72 - ww/2, 2.3*72 + 0.5*72, paint);  

    if( size_name == "COVER" ) {
        sprintf( sz, "Size: S,M,L,XL,XXL" );
    }
    else {
        sprintf( sz, "Size: %s", size_name.c_str() );
    }
    blob = SkTextBlob::MakeFromText(sz, strlen(sz), font);
    ww = font.measureText( sz, strlen(sz), SkTextEncoding::kUTF8, &rect ); 
    paint.setColor(SK_ColorBLACK);
    canvas.drawTextBlob(blob, 
        6.5 * 72 - ww/2, 5.2 * 72, paint);   

    DecParagraph * paragraph = new DecParagraph();
    paragraph->setUI( this->_ui );
    paragraph->bg_color( 0x00000000 );
    paragraph->fontSize( 14 );
    paragraph->text( getDescription().c_str() );
    paragraph->size( 2.85 * 72.0, 10.0 * 72.0 );
    paragraph->reFormat();  

    paragraph->paragraph->paint( &canvas, 5.1 * 72.0, 3.3 * 72.0 );

    sprintf( sz, "https://magicpattern.co/product/magic-pattern-%04i/", _product_id );
    DecQRCode qr( sz );
    qr.border( false );
    qr.background( false );
    qr.size( 1.25 * 72, 1.25 * 72 );
    canvas.save();
    canvas.translate( 5.07 * 72.0, 6.02 * 72.0 );
    qr.render( canvas );
    canvas.restore();

    delete( img );
}

std::string DecSVGEditor::getDescription() {
    static const char *desc_nect[] = { "คอกลม", "คอวี", "คอปาด", "คอสี่เหลี่ยม", "คอคว้าน" };
    static const char *desc_collar[] = { "ไม่มีปก", "ปกบัว", "ปกบัวแหลม", "ปกคอจีน", "ปกระบาย", "ปกผูกโบว์", "ปกปลายแหลม" };
    static const char *desc_type[] = { "สวมศีรษะ", "ผ่าหน้า" };
    static const char *desc_length[] = { "เอว", "สะโพกสูง", "สะโพกล่าง" };
    static const char *desc_sleeve[] = { "แขนกุด", "แขนสั้น", "แขนสามส่วน", "แขนยาว", "แขนทิวลิป", "แขน cap", "แขนย้วย", "แขน lantern", "แขน Flounce", "แขนมารี", "แขนผ่า", "แขนตุ๊กตา", "แขนระบาย" };
    static const char *desc_hem[] = { "", "แต่งระบายชายเสื้อแบบสั้น ", "แต่งระบายชายเสื้อแบบยาว " };

    const char *_str_length = "WHL";
    const char *l = strchr( _str_length, _sku[0] );
    int _length = l - _str_length;

    const char *_str_type = "PB";
    const char *t = strchr( _str_type, _sku[1] );
    int _type = t - _str_type;

    int _neck = _sku[2] - 'A';
    int _collar = _sku[5] - 'A';
    int _sleeve = _sku[6] - 'H';
    int _hem = _sku[7] - 'U';

    static char szy[512];
    sprintf( szy, "แพทเทิร์นเสื้อ%s %s %s ความยาวระดับ%s %s %s", 
        desc_nect[_neck],
        desc_collar[_collar],
        desc_type[_type],
        desc_length[_length],
        desc_sleeve[_sleeve],
        desc_hem[_hem]
    ); 
    return std::string( szy );
}


bool DecSVGEditor::checkSKUMaterial( std::string sz1, std::string sz2, std::string sz5, std::string sz6 ) {
    if (sz1.find(_sku[1]) == std::string::npos) return false;
    if (sz2.find(_sku[2]) == std::string::npos) return false;
    if (sz5.find(_sku[5]) == std::string::npos) return false;
    if (sz6.find(_sku[6]) == std::string::npos) return false;
    return true;
}

void DecSVGEditor::overlayLogo( SkCanvas &canvas, int exportW, int exportH ) {
    // Test Box
    canvas.save();
    canvas.scale( pSVGDomEditor->_svgScaleX, pSVGDomEditor->_svgScaleY );

    SkPaint paint;

    int tbx = exportW - 72 - (72 / 2);
    int tby = 0 + (72 / 2);
    int cm = 72.0 / 2.54;

    paint.setStyle( SkPaint::Style::kStroke_Style );

    SkRect rect = SkRect::MakeXYWH( tbx, tby, 72, 72 );
    canvas.drawRect(rect, paint);
    rect = SkRect::MakeXYWH( tbx + (72.0 - cm)/2, tby + (72.0 - cm)/2, cm, cm );
    canvas.drawRect(rect, paint);

    SkFont font;
    font.setTypeface(_font->typeface(FONT_REGULAR));
    font.setSubpixel(true);
    font.setSize( 7 );

    SkFontMetrics metrics;
    font.getMetrics(&metrics);

    paint.setStyle( SkPaint::Style::kFill_Style );

    std::string text = "ตรวจสอบขนาด";
    sk_sp<SkTextBlob> blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);

    int ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 

    canvas.drawTextBlob(blob, tbx + (72-ww)/2, tby+72-metrics.fDescent, paint);  

    text = "1 นิ้ว";
    blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
    ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 

    canvas.drawTextBlob(blob, tbx + (72-ww)/2, tby-metrics.fAscent, paint);  

    text = "1 ซ.ม.";
    blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
    ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 

    canvas.drawTextBlob(blob, tbx + (72-ww)/2, tby+36-(metrics.fDescent+metrics.fAscent)/2, paint); 
    canvas.restore();

    // Watermark
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorBLACK);
    paint.setStyle( SkPaint::Style::kFill_Style );    
    rect = SkRect::MakeXYWH( 0, 0, pSVGDomEditor->pImageLogo->image->width(), pSVGDomEditor->pImageLogo->image->height() );
    canvas.save();
    float sc = std::min( exportW, exportH) * 0.6 / (float)pSVGDomEditor->pImageLogo->image->width();
    canvas.translate( exportW / 2, exportH / 2 );
    canvas.scale( sc, sc );
    canvas.rotate( -45 );
    canvas.translate( 0-pSVGDomEditor->pImageLogo->image->width()/2, 0-pSVGDomEditor->pImageLogo->image->height()/2);   
    paint.setColorFilter(SkColorFilters::Blend(0x30FFFFFF, SkBlendMode::kModulate));
    canvas.drawImageRect( pSVGDomEditor->pImageLogo->image, rect, SkSamplingOptions(), &paint );
    canvas.restore();     
}

void DecSVGEditor::overlayMaterial( SkCanvas &canvas ) {
    int n = 0;
    if( checkSKUMaterial( "P", "A",     "ABCDEF", "H"       ) ) n = 1;
    if( checkSKUMaterial( "P", "A",     "ABCDEF", "ILMST"   ) ) n = 2;
    if( checkSKUMaterial( "P", "A",     "ABCDEF", "JKNOR"   ) ) n = 3;
    if( checkSKUMaterial( "P", "A",     "ABCDEF", "P"       ) ) n = 4;
    if( checkSKUMaterial( "P", "A",     "ABCDEF", "Q"       ) ) n = 5;

    if( checkSKUMaterial( "B", "ABCDE", "A",      "H"       ) ) n = 6;

    if( checkSKUMaterial( "B", "AB",    "BCDEFG", "H"       ) ) n = 7;
    if( checkSKUMaterial( "B", "AB",    "BCDEFG", "ILMST"   ) ) n = 8;
    if( checkSKUMaterial( "B", "AB",    "BCDEFG", "JKNOR"   ) ) n = 9;
    if( checkSKUMaterial( "B", "AB",    "BCDEFG", "P"       ) ) n = 10;
    if( checkSKUMaterial( "B", "AB",    "BCDEFG", "Q"       ) ) n = 11;

    if( checkSKUMaterial( "P", "BCDE",  "AG",     "H"       ) ) n = 12;
    if( checkSKUMaterial( "P", "BCDE",  "AG",     "ILMST"   ) ) n = 13;
    if( checkSKUMaterial( "P", "BCDE",  "AG",     "JKNOR"   ) ) n = 14;
    if( checkSKUMaterial( "P", "BCDE",  "AG",     "P"       ) ) n = 15;
    if( checkSKUMaterial( "P", "BCDE",  "AG",     "Q"       ) ) n = 16;

    if( checkSKUMaterial( "B", "ABCDE",  "A",      "ILMST"   ) ) n = 17;
    if( checkSKUMaterial( "B", "ABCDE",  "A",      "JKNOR"   ) ) n = 18;
    if( checkSKUMaterial( "B", "ABCDE",  "A",      "P"       ) ) n = 19;
    if( checkSKUMaterial( "B", "ABCDE",  "A",      "Q"       ) ) n = 20;

    DecImage *img = new DecImage();

    char sz[128];
    sprintf( sz, "_magicpattern/Material/material%02i.png", n );

    img->load( sz );

    float w = img->width();
    float h = img->height();

    SkPaint paint;
    SkRect rect = SkRect::MakeXYWH( 4.25 * 72, 7.5 * 72, 3.79 * 72, (3.79 * 72) * h / w );
    canvas.drawImageRect( img->image, rect, SkSamplingOptions(), &paint );   

    sprintf( sz, "_magicpattern/Sizes/size%c.png", _sku[0] );
    img->load( sz );

    w = img->width();
    h = img->height();

    rect = SkRect::MakeXYWH( 0.2 * 72, 9.2 * 72, 3.9 * 72, (3.9 * 72) * h / w );
    canvas.drawImageRect( img->image, rect, SkSamplingOptions(), &paint ); 

    if( _sku[6] != 'H' ) {
        sprintf( sz, "_magicpattern/Sizes/sleeve%c.png", _sku[6] );
        img->load( sz );

        w = img->width();
        h = img->height();

        rect = SkRect::MakeXYWH( 0.2 * 72, (9.2 + 1.45) * 72, 3.9 * 72, (3.9 * 72) * h / w );
        canvas.drawImageRect( img->image, rect, SkSamplingOptions(), &paint ); 
    }

    sprintf( sz, "images/MP-Copyright.png" );
    img->load( sz );
    SkDebugf( "Image: %i %i\n", img->width(), img->height() );
    rect = SkRect::MakeXYWH( (8.25 * 72 - 6.0 * 72)/2, 11.3 * 72, 6.0 * 72, img->height() * (6.0 * 72) / img->width() );
    canvas.drawImageRect( img->image, rect, SkSamplingOptions(), &paint ); 

    delete( img ); 

}

bool DecSVGEditor::__jobFileExport( DecWidget &widget ) {
    int i = __jobFileIndex;

    clearSewingLine();

    std::string filepath;
    if( i == -2 ) {
        filepath = std::string("_magicpattern/Cover/Cover1-01.svg");

        pNodeTree->clear();
        bool ret = pSVGDomEditor->load(filepath.c_str());
        pSVGDomEditor->dirty();   

        pSVGDomEditor->renderPDF( "", false, [=]( SkCanvas &canvas, int w, int h ) {
            overlayCoverImage( canvas );
            overlayMaterial( canvas );
        });       
    }
    else if( i == -1 ) {
        filepath = std::string("_magicpattern/Cover/Cover2-01.svg");

        pNodeTree->clear();
        bool ret = pSVGDomEditor->load(filepath.c_str());
        pSVGDomEditor->dirty();   

        pSVGDomEditor->renderPDF( "", false, [=]( SkCanvas &canvas, int w, int h ) {
            overlayLayout( canvas );
            overlayImage( canvas );
        } );       

    }
    else {
        filepath = std::string("_magicpattern/SVG/") + _vProductFileList[i];

        std::filesystem::path fpath( filepath );
        std::string filebasename = fpath.stem().string();

        loadSewingLine( filebasename.c_str() );        

        pNodeTree->clear();
        bool ret = pSVGDomEditor->load(filepath.c_str());
        pSVGDomEditor->dirty();  

        char szProductID[32];
        sprintf( szProductID, "#%04i (%s) ", _product_id, size_name.c_str() );

        std::string patternName = "";
        if( (_vProductFileList[i][0] == 'W' || _vProductFileList[i][0] == 'H' || _vProductFileList[i][0] == 'L') &&
            (_vProductFileList[i][1] == 'P' || _vProductFileList[i][1] == 'B') &&
            (_vProductFileList[i][3] == 'N' || _vProductFileList[i][3] == 'S') &&
            (_vProductFileList[i][4] == '-')) {
            patternName = std::string(szProductID) + "แพทเทิร์นตัวเสื้อ ";
        }
        else if( (_vProductFileList[i][0] >= 'A' && _vProductFileList[i][0] <= 'G') &&
            (_vProductFileList[i][1] == '-')) {
            patternName = std::string(szProductID) + "แพทเทิร์นปก ";
        }
        else if( (_vProductFileList[i][0] >= 'H' && _vProductFileList[i][0] <= 'T') &&
            (_vProductFileList[i][1] == '-')) {
            patternName = std::string(szProductID) + "แพทเทิร์นแขน ";
        }
        else if( (_vProductFileList[i][0] >= 'U' && _vProductFileList[i][0] <= 'W') &&
            (_vProductFileList[i][1] == '-')) {
            patternName = std::string(szProductID) + "แพทเทิร์นระบาย ";
        }        

        pSVGDomEditor->renderPDF( patternName.c_str(), _isTiledExport, [=]( SkCanvas &canvas, int w, int h ) {
            if( !_isTiledExport ) {
                overlayLogo( canvas, w, h );
            }
            overlaySewingLine( canvas );
        } ); 
    }

    if( __jobFileIndex == -1 ) {
        pSVGDomEditor->endExportPDF();  
        return true;
    }
    __jobFileIndex++;
    if( (int)__jobFileIndex >= (int)_vProductFileList.size() ) {
        pSVGDomEditor->endExportPDF();    
        return true;
    }
    return false;
}



void DecSVGEditor::exportPartPNG() {
    __jobPNGFileIndex = 0;

    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobPNGFileExport( w );
    }) );

    return;      
}


void DecSVGEditor::clearSewingLine() {
    int size_index = getSizeIndex();
    if( size_index == -1 ) {
        return;
    }
    if( _sewingLinePath[size_index] ) {
        delete( _sewingLinePath[size_index] );
        _sewingLinePath[size_index] = NULL;
    }
    _vSewingLineInfo[size_index].clear();
}

bool DecSVGEditor::__jobPNGFileExport( DecWidget &widget ) {
    char szPID[12];
    sprintf( szPID, "%04i", _product_id );

    if (!_vProductFileList.size()) return false;

    int i = __jobPNGFileIndex;
    std::string filepath;
    //while( true ) 
    {
        i = __jobPNGFileIndex;
        filepath = std::string("_magicpattern/SVG/") + (_vProductFileList.size() > i ? _vProductFileList[i]:"");

        std::filesystem::path fullpath(filepath);
        _filepath = fullpath.string();
        _filename = fullpath.filename().string();
        _filebasename = fullpath.stem().string();   

        /*
        char szFI[32];
        sprintf( szFI, "%04i", __jobPNGFileIndex );
        std::string fn = std::string("resources/_export_png/") + std::string(szPID) + "-" + size_name + "-" + std::string(szFI) + "-" + _filebasename  + ".png";

        if( std::filesystem::exists( std::filesystem::path( fn ) ) ) {
            SkDebugf( "Skip: %s\n", fn.c_str() );
            __jobPNGFileIndex++;
            if( __jobPNGFileIndex >= _vProductFileList.size() ) {   
                return true;
            }
        }
        else {
            break;
        }
        */
    }




    loadSewingLine( _filebasename.c_str() );       

    pNodeTree->clear();
    bool ret = pSVGDomEditor->load(filepath.c_str());
    pSVGDomEditor->dirty();  

    char szProductID[32];
    sprintf( szProductID, "#%04i (%s) ", _product_id, size_name.c_str() );


    if( _is_S && _is_M && _is_L && _is_XL && _is_XXL ) {
        size_name = "ALL";
    }
    else {
        if( _is_S ) {
            size_name = "S";
        }
        if( _is_M ) {
            size_name = "M";
        }
        if( _is_L ) {
            size_name = "L";
        }
        if( _is_XL ) {
            size_name = "XL";
        }
        if( _is_XXL ) {
            size_name =  "XXL";
        }
    }  

    char szFI[32];
    sprintf( szFI, "%04i", __jobPNGFileIndex );
    std::string fn;
    if( _is_png_forweb ) {
        fn = std::string("resources/_export_png2/") + std::string(szPID) + "-" + std::string(szFI) + ".png";
    }
    else {
        fn = std::string("resources/_export_png/") + std::string(szPID) + "-" + size_name + "-" + std::string(szFI) + "-" + _filebasename  + ".png";
    }

    if( !std::filesystem::exists( std::filesystem::path( fn ) ) ) {
        SkDebugf( "PNG: %s\n", fn.c_str() );
        int dpi = 72;
        if( _is_png_forweb ) {
            dpi = 10;
        }            
        pSVGDomEditor->exportPNG( fn.c_str(), dpi, [=]( SkCanvas &canvas ) {
            overlaySewingLine( canvas );
        });                
    }
    else {
        SkDebugf( "Skip2: %s\n", fn.c_str() );
    }

    __jobPNGFileIndex++;
    if( __jobPNGFileIndex >= _vProductFileList.size() ) {   
        return true;
    }
    return false;    
}

void DecSVGEditor::exportPNGs() {
    setJobFrameSkip( 1 );

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

    if( _is_png_forweb ) {
        __jobSizeIndex = 1;
    }
    else {
        __jobSizeIndex = 0;
    }
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
        exportPartPNG();
    }
    else if( __jobSizeIndex == 1 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;        

        _is_M = true;
        updateNodeVisibility();
        exportPartPNG();
    }
    else if( __jobSizeIndex == 2 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;        

        _is_L = true;
        updateNodeVisibility();
        exportPartPNG();
    }
    else if( __jobSizeIndex == 3 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;        

        _is_XL = true;
        updateNodeVisibility();
        exportPartPNG();
    }
    else if( __jobSizeIndex == 4 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;        

        _is_XXL = true;
        updateNodeVisibility();
        exportPartPNG();
    }

    __jobSizeIndex += 1;
    if( _is_png_forweb ) {
        if( __jobSizeIndex > 1 ) {
            return true;
        }
    }
    else {
        if( __jobSizeIndex > 4 ) {
            return true;
        }        
    }
    return false;
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
    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobSizeExport( w );
    }) );
}

bool DecSVGEditor::__jobSizeExport( DecWidget &widget ) {
    //SkDebugf( "Export Size: %i\n", __jobSizeIndex );
    if( __jobSizeIndex == 0 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;        

        _is_S = true;
        updateProductImages();
        updateNodeVisibility();
        exportProductPDF();
    }
    else if( __jobSizeIndex == 1 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;        

        _is_M = true;
        updateProductImages();
        updateNodeVisibility();
        exportProductPDF();
    }
    else if( __jobSizeIndex == 2 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;        

        _is_L = true;
        updateProductImages();
        updateNodeVisibility();
        exportProductPDF();
    }
    else if( __jobSizeIndex == 3 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;        

        _is_XL = true;
        updateProductImages();
        updateNodeVisibility();
        exportProductPDF();
    }
    else if( __jobSizeIndex == 4 ) {
        _is_S = false;
        _is_M = false;
        _is_L = false;
        _is_XL = false;
        _is_XXL = false;        

        _is_XXL = true;
        updateProductImages();
        updateNodeVisibility();
        exportProductPDF();
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
    if (_pTextDocumentPath) {
        _pTextDocumentPath->setText(pSVGDomEditor->fPath.c_str());
        _pTextDocumentPath->dirty();
        pSVGDomEditor->dirty();
    }
    //if( pSVGDomEditor ) {
    //    pSVGDomEditor->pSVGEditor = this;
    //}    
    fDom = pSVGDomEditor->fDom;

    updateSVGInfo();
    updateNodeTree();
    updateContentBounds();
}

void DecSVGEditor::init() {
    if( pSVGDomEditor ) {
        pSVGDomEditor->pSVGEditor = this;
    }    
    generateProductCodeList();
    _pComboBoxTargetPaper->select( 1 );

    

    fDom = pSVGDomEditor->fDom;

    std::filesystem::path fullpath( "_magicpattern/SVG/HBAN-A1=2-01.svg" );

    _filepath = fullpath.string();
    _filename = fullpath.filename().string();
    _filebasename = fullpath.stem().string();

    loadSewingLine(_filebasename.c_str());

    pNodeTree->clear();
    bool ret = pSVGDomEditor->load(_filepath.c_str());

    pSVGDomEditor->dirty(); 

    if( __DecArguments.size() ) {
        setTimeOut( 3000, [=](DecWidget &widget) {
            if( __DecArguments[0] == "product_list.txt" ||
                __DecArguments[0] == "product_list1.txt" ||
                __DecArguments[0] == "product_list2.txt" ||
                __DecArguments[0] == "product_list3.txt" ||
                __DecArguments[0] == "product_list4.txt" ||
                __DecArguments[0] == "../SkiaApps/product_list.txt" ||
                __DecArguments[0] == "../SkiaApps/product_list1.txt" ||
                __DecArguments[0] == "../SkiaApps/product_list2.txt" ||
                __DecArguments[0] == "../SkiaApps/product_list3.txt" ||
                __DecArguments[0] == "../SkiaApps/product_list4.txt" ) 
            {
                exportProductsByList(__DecArguments[0].c_str());
            }
            else {
                _pMessageBox->message( __DecArguments[0].c_str() ).show();
            }
        });
    }
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

void DecSVGEditor::generateProductCodeList() {
    int _generateType = 0;
    int _generateLength = 0;
    int _generateNeck = 0;
    int _generateHem = 0;
    int _generateSleeve = 0;
    int _generateCollar = 0;
    int _generateCount = 0;

    while( true ) {
        _generateCount++;      
        //if( _generateCount > 10 )
        //    break;

        int _body = _generateLength * 10 + _generateType * 5 + _generateNeck;
        int _length = (_body / 10);
        int _type = (_body % 10) / 5;
        int _neck = (_body % 5);

        int _hem = _generateHem;
        int _sleeve = _generateSleeve;
        int _collar = _generateCollar;

        std::string _sku = getSKU( _length, _type, _neck, _hem, _sleeve, _collar ); 
        //SkDebugf( "%04i, SKU: %s Type:%i Length:%i Neck:%i Collar:%i Sleeve:%i hem:%i\n", _generateCount, _sku.c_str(), _type, _length, _neck, _collar, _sleeve, _hem );
        _sku_list.push_back( _sku );

        char sz[32];
        sprintf( sz, "%04i %s", _generateCount, _sku.c_str() );
        //SkDebugf( "%s\n", sz );
        _pSKUList->_( ListItem( sz ) );

        bool bSkip = false;
        if( _generateNeck == 0 ) {
            _generateCollar++;
            if( _generateCollar == 4 && _generateType == 1 ) {
                _generateCollar++;
            }
            if( _generateCollar >= 6 ) {
                _generateCollar = 0;
            }
            else {
                bSkip = true;
            }
        }
        else if( _generateNeck == 1 ) {
            _generateCollar += 6;
            if( _generateCollar >= 12 ) {
                _generateCollar = 0;
            }
            else {
                bSkip = true;
            }
        }
        if( !bSkip ) {
            _generateNeck++;
            if( _generateNeck >= 5 ) {
                _generateNeck = 0;
                _generateSleeve++;
                if( _generateSleeve >= 13 ) {
                    _generateSleeve = 0;
                    _generateHem++;
                    if( _generateHem >= 3 ) {
                        _generateHem = 0;
                        _generateLength++;
                        if( _generateLength >= 3 ) {
                            _generateLength = 0;
                            _generateType++;
                            if( _generateType >= 2 ) {
                                break;
                            }
                        }
                    }                
                }
            }
        }

    }
}

/*
void DecSVGEditor::drawContentBounds( DecSVGDomEditor *dom, SkCanvas &canvas ) {
    return;
    canvas.save();
    canvas.scale( dom->_svgScaleX, dom->_svgScaleY );

    SkPaint paint;
    paint.setColor( 0x40FF0000 );
    paint.setAntiAlias( true );
    paint.setStrokeWidth( 1 / dom->drawScale );
    paint.setStyle( SkPaint::Style::kStroke_Style );    

    //for( int i=0; i<_vNodeBounds.size(); i++ ) {
    //    SkRect rect = _vNodeBounds[i];
    //    canvas.drawRect( rect, paint );
    //}

    SkRect rect = SkRect::MakeXYWH( 
        (dom->drawPaperW / dom->_svgScaleX - contentBounds.width()) / 2, 
        (dom->drawPaperH / dom->_svgScaleY - contentBounds.height()) / 2, 
        contentBounds.width(), contentBounds.height() );

    canvas.drawRect( rect, paint );
    //canvas.drawRect( contentBounds, paint );

    canvas.restore();
}
*/

void DecSVGEditor::bubbleAction( int actionID, DecWidget *sender, void *data ) {
    if( actionID == ACTION_TREE_SELECT_CHANGED ) {
        selectNode( (DecTreeItem *)data );
    }
}

void DecSVGEditor::selectNode( DecTreeItem *pItem ) {
    if( pSVGDomEditor ) {
        if( pItem ) {
            pSVGDomEditor->selectNode( (SkSVGNode *)pItem->_externalData );
        }
        else {
            pSVGDomEditor->selectNode( NULL );
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

    /*
    if( !fDom ) 
        return;
    SkRect rect;
    rect.setEmpty();

    for( int i=0; i<_vTreeItems.size(); i++ ) {
        DecTreeItem *item = _vTreeItems[i];
        if( item->_flag ) {
            rect.join( _vNodeBounds[i] );
        }
    }

    //SkDebugf( "Update Content Bounds\n" );
    
    root = fDom->getRoot();

    SkSize szC = fDom->containerSize();
    SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
    if( szR.fWidth == 0 || szR.fHeight == 0 ) {
        szR.fWidth = szC.fWidth;
        szR.fHeight = szC.fHeight;
    }
    //SkDebugf( "Container size: %0.2f %0.2f\n", szC.fWidth, szC.fHeight );
    //SkDebugf( "SVG Size: %0.2f %0.2f\n", szR.fWidth, szR.fHeight );

    if( rect.width() > szR.fWidth || 
        rect.height() > szR.fHeight ) {
        //SkDebugf( "Over size\n" );
        contentBounds = SkRect::MakeXYWH( 0, 0, szR.fWidth, szR.fHeight );
    }        
    else {
        contentBounds = rect.makeOutset( pSVGDomEditor->_svgDPIX/10, pSVGDomEditor->_svgDPIY/10 );
    }
    //SkDebugf( "Content size: %0.2f %0.2f\n", contentBounds.width(), contentBounds.height() );

    DecSVGDomEditor *dom = pSVGDomEditor;

    float centerContentX = (dom->drawPaperW / dom->_svgScaleX - contentBounds.width()) / 2;
    float centerContentY = (dom->drawPaperH / dom->_svgScaleY - contentBounds.height()) / 2; 
    dom->drawOffsetY = centerContentY - contentBounds.fTop;
    dom->drawOffsetX = centerContentX - contentBounds.fLeft;
    */
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
            auto visible = SkSVGVisibility(SkSVGVisibility::Type::kHidden);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);        
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

void DecSVGEditor::loadSewingLine( const char *filebasename ) {
    //SkDebugf( "Load sewing line\n" );
    for( int i=0; i<5; i++ ) {
        std::string fn = std::string("_sewingline/") + filebasename;
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
            //SkDebugf( "Load: %s\n", fn.c_str() );
            std::ifstream file(fn);
            std::string str; 
            _vSewingLineInfo[i].clear();
            //_pSewingLineNodeList->clear();
            while (std::getline(file, str))
            {
                //SkDebugf( "Path el: %s\n", str.c_str() );
                _vSewingLineInfo[i].push_back( std::string( str ) );
                //_pPathNodeList->_( ListItem( str.c_str() ));
                //_pSewingLineNodeList->_( ListItem( str.c_str() ));
            } 
            file.close();
            SkPath *path = new SkPath();
            for( int j=0; j<_vSewingLineInfo[i].size(); j++ ) {
                std::string str = _vSewingLineInfo[i][j];
                //SkDebugf( "el: %s\n", str.c_str() );

                char sz[2048];
                strcpy( sz, str.c_str() );
                //SkDebugf( "Path: %s\n", sz );

                char *verb = strtok( sz, " ");
                char *x = strtok (NULL, " ");
                char *y = strtok (NULL, " ");

                if( !strcmp( verb, "M" ) ) {
                    path->moveTo( atof(x), atof(y) );
                }
                else if( !strcmp( verb, "L" ) ) {
                    path->lineTo( atof(x), atof(y) );
                }
                else if( !strcmp( verb, "C" ) ) {
                    char *x2 = strtok (NULL, " ");
                    char *y2 = strtok (NULL, " ");
                    char *x3 = strtok (NULL, " ");
                    char *y3 = strtok (NULL, " ");
                    path->cubicTo( atof(x), atof(y), atof(x2), atof(y2), atof(x3), atof(y3) );
                }
                else if( !strcmp( verb, "Q" ) ) {
                    char *x2 = strtok (NULL, " ");
                    char *y2 = strtok (NULL, " ");
                    path->quadTo( atof(x), atof(y), atof(x2), atof(y2) );
                }
                else if( !strcmp( verb, "N" ) ) {
                    char *x2 = strtok (NULL, " ");
                    char *y2 = strtok (NULL, " ");
                    path->quadTo( atof(x), atof(y), atof(x2), atof(y2));
                }
                else if( !strcmp( verb, "X" ) ) {
                    path->close();
                }

            }
            _sewingLinePath[i] = path;
        }
    }
}

int DecSVGEditor::getSizeIndex() {
    if( _is_S ) return 0;
    else if( _is_M ) return 1;
    else if( _is_L ) return 2;
    else if( _is_XL ) return 3;
    else if( _is_XXL ) return 4;
    return -1;
}

void DecSVGEditor::drawSewingLine( DecSVGDomEditor *, SkCanvas &canvas ) {
    int size_index = getSizeIndex();
    if( size_index == -1 ) {
        return;
    }
    if( _sewingLinePath[size_index]) {
        canvas.save();
        canvas.scale( pSVGDomEditor->_svgScaleX, pSVGDomEditor->_svgScaleY );
        canvas.translate( 0+pSVGDomEditor->contentCenterOffsetX_72, 0+pSVGDomEditor->contentCenterOffsetY_72 );

        SkPaint paint;
        paint.setAntiAlias( true );
        paint.setStyle( SkPaint::Style::kStroke_Style );

        paint.setStrokeWidth( 3.0/pSVGDomEditor->drawScale_DPI );

        /*
        if( _SelectedSewingLinePath ) {
            paint.setColor( 0xFFFF0000 );
            canvas.drawPath( *_SelectedSewingLinePath, paint );
        }
        */

        paint.setColor( 0xA00000FF );
        paint.setStrokeWidth( 0.5/pSVGDomEditor->drawScale_DPI );

        float intervals[] = { 6, 4 };
        paint.setPathEffect(SkDashPathEffect::Make(intervals, 2, 0.0f));

        if( _sewingLinePath[size_index]) {
            canvas.drawPath( *_sewingLinePath[size_index], paint );
        }

        /*
        if( _editingSewingLine ) {
            paint.setColor( 0xFFFF0000 );
            canvas.drawPath( *_editingSewingLine, paint );
        }
        */

        canvas.restore();
    }
}

void DecSVGEditor::overlaySewingLine( SkCanvas &canvas ) {
    int size_index = getSizeIndex();
    if( size_index == -1 )
        return;
    if( _sewingLinePath[size_index]) {
        canvas.save();
        canvas.scale( pSVGDomEditor->_svgScaleX, pSVGDomEditor->_svgScaleY );

        SkPaint paint;
        paint.setAntiAlias( true );
        paint.setStyle( SkPaint::Style::kStroke_Style );

        paint.setColor( 0x600000FF );
        paint.setStrokeWidth( 0.5/pSVGDomEditor->drawScale_DPI );

        float intervals[] = { 6, 4 };
        paint.setPathEffect(SkDashPathEffect::Make(intervals, 2, 0.0f));

        canvas.drawPath( *_sewingLinePath[size_index], paint );


        canvas.restore();
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

            char sz[256];
            sprintf( sz, "Document size: %0.2f x %0.2f inches", szR.fWidth/72.0, szR.fHeight/72.0 );
            if( _pTextDocumentSize ) {
                _pTextDocumentSize->setText( sz );
            }
            sprintf( sz, "Document DPI: %0.2f x %0.2f DPI", pSVGDomEditor->_svgDPIX, pSVGDomEditor->_svgDPIY );
            if( _pTextDocumentDPI ) {
                _pTextDocumentDPI->setText( sz );
            }            
            sprintf( sz, "Minimal paper size: %0.2f x %0.2f inches (%s)", 
                pSVGDomEditor->minPaperWidth_mm/25.4, pSVGDomEditor->minPaperHeight_mm/25.4,
                pSVGDomEditor->minPaperSizeName.c_str() );
            if( _pTextMinimalPaperSize ) {
                _pTextMinimalPaperSize->setText( sz );
            }
            sprintf( sz, "Target paper size: %0.2f x %0.2f inches (%s)", 
                pSVGDomEditor->paperWidth_mm/25.4, pSVGDomEditor->paperHeight_mm/25.4,
                pSVGDomEditor->paperSizeName.c_str() );
            if( _pTextTargetPaperSize ) {
                _pTextTargetPaperSize->setText( sz );
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
