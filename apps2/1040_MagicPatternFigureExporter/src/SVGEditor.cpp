#include "DeclarativeUI.h"
#include "DecSVGDom.h"
#include "DecUI.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "src/utils/SkOSPath.h"
#include "modules/skresources/include/SkResources.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "modules/svg/include/SkSVGText.h"
#include "modules/svg/include/SkSVGG.h"
#include "modules/svg/include/SkSVGRenderContext.h"
#include "SVGEditor.h"
#include "../../1020_SVGEditorApp/src/SVGDomEditor.h"

// for SK_ARRAY_COUNT
#include "include/core/SkTypes.h"

const int paperWidth[] = { 52, 74, 105, 148, 210, 297, 420, 594, 841 };
const int paperHeight[] = { 74, 105, 148, 210, 297, 420, 594, 841, 1188 };
const char *paperName[] = { "A8", "A7", "A6", "A5", "A4", "A3", "A2", "A1", "A0", "n/a" };
const int numPaperType = 9;

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

static inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

static inline bool starts_with(std::string const & value, std::string const & starting)
{
    if (starting.size() > value.size()) return false;
    return std::equal(starting.begin(), starting.end(), value.begin());
}

DecSVGEditor::DecSVGEditor() {
    _isFocusable = true; 
    focus( true );

    fDom = NULL;
    root = NULL;
    _pTextDocumentPath = NULL;
    _pTextDocumentSize = NULL;
    _nCurrentBG = 0;
    _nCurrentBlouseType = 0;
    _nCurrentBlouseLength = 1;
    _nCurrentBlouseNeck = 1;
    _nCurrentCollar = 0;
    _nCurrentBlouseHem = 0;
    _nCurrentSleeve = 0;

    for( int nn=0; nn<4; nn++ ) {
        for( int cc=0; cc<5; cc++ ) {
            _vBlouseNeckTreeItems[nn][cc] = NULL;
        }
    }    

    for( int ll=0; ll<3; ll++ ) {
        for( int tt=0; tt<2; tt++ ) {
            for( int hh=0; hh<3; hh++ ) {
                _vBlouseHemTreeItems[ll][tt][hh] = NULL;
            }
        }
    }

    for( int ss=0; ss<13; ss++ ) {
        _vSleeveTreeItems[ss] = NULL;
    }

    for( int ss=0; ss<7; ss++ ) {
        _vCollarTreeItems[ss] = NULL;
    }

    _keyMode = 0;

    _nBG = 0;
    _nLen = 0;
    _nNeck = 0;
    _nHem = 0;
    _nSleeve = 0;
    _nCollar = 0;
    _nHair = 0;

    size( -1, -1 );
    border( false );
    background( false );

    _( Container()
        .size( 44, -1 )
        .border(false)
        .background(false)
    );
    //._( SVGEditor( "resources/svg/HBAN-01.svg") );
    //._( SVGEditor( "resources/svg/MagicPattern-01.svg") );
    //_(SVGDomEditor("resources/svg/Blouse-01.svg").ref((DecWidget**)&pSVGDomEditor).cache(true));
    //_( SVGDomEditor("_magicpattern/Blouse-04.svg").ref((DecWidget**)&pSVGDomEditor));
    _( SVGDomEditor("_magicpattern/Blouse2-01.svg").ref((DecWidget**)&pSVGDomEditor));
    _( FileDialogBox()
        .ref( (DecWidget **)&_pFileDialog )
    );
    _( Column()
        .size( 400, -1 )
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
        ._( Text( "" )
            .ref( (DecWidget **)&_pTextDocumentSize )
        )
        ._( Column()
            .size( -1, -1 )
            .border( false )
            .gap( 5 )
            ._( Text( "Export: " ).margin_top( 5 ) )
            ._( Row()
                .size( -1, 0 )
                .gap( 10 )
                .border( false )
                ._( Button("PNG 300 DPI", 0, [=]( DecButton &btn ) {
                    //SkDebugf( "Export PNG\n" );
                    pSVGDomEditor->exportPNG( "_export/test.png", 300 );
                }))
                ._( Button("PDF Document", 0, [=]( DecButton &btn ) {
                    //SkDebugf( "Export PDF\n" );
                    //pSVGDomEditor->exportPDF( "_export/test.pdf" );
                }))
            )
            ._( Text( "Background: " ).margin_top( 5 ) )
            ._( ComboBox()
                .ref( (DecWidget **)&_pBGComboBox )
                .size( -1, 32 )
                .border( true )
                .onValueChanged( [=](DecWidget &widget) {
                    showBG( widget.value() );
                })
            )
            ._(Row().size( -1, 0).border(false).gap( 10 )
                ._(Column().size( -1, 0).border(false)
                    ._( Text( "Hair: " ).margin_top( 5 ) )
                    ._( ComboBox()
                        .ref( (DecWidget **)&_pHairComboBox )
                        .size( -1, 32 )
                        .border( true )
                        .onValueChanged( [=](DecWidget &widget) {
                            showHair( widget.value() );
                        })
                    ) 
                )
                ._(Column().size( -1, 0).border(false)
                    ._( Text( "Body: " ).margin_top( 5 ) )
                    ._( ComboBox()
                        .ref( (DecWidget **)&_pBodyComboBox )
                        .size( -1, 32 )
                        .border( true )
                        .onValueChanged( [=](DecWidget &widget) {
                            //SkDebugf( "BG: %i\n", (int)widget.value() );
                            showBody( widget.value() );
                        })
                    ) 
                )         
                ._(Column().size( -1, 0).border(false)
                    ._( Text( "Pant: " ).margin_top( 5 ) )
                    ._( ComboBox()
                        .ref( (DecWidget **)&_pPantComboBox )
                        .size( -1, 32 )
                        .border( true )
                        .onValueChanged( [=](DecWidget &widget) {
                            //SkDebugf( "BG: %i\n", (int)widget.value() );
                            showPant( widget.value() );
                        })
                    ) 
                ) 
                ._(Column().size( -1, 0).border(false)
                    ._( Text( "Hand: " ).margin_top( 5 ) )
                    ._( ComboBox()
                        .ref( (DecWidget **)&_pHandComboBox )
                        .size( -1, 32 )
                        .border( true )
                        .onValueChanged( [=](DecWidget &widget) {
                            //SkDebugf( "BG: %i\n", (int)widget.value() );
                            showHand( widget.value() );
                        })
                    ) 
                )                                       
            )
            ._(Row().size( -1, 0).border(false).gap( 10 )
                ._(Column().size( -1, 0).border(false)      
                    ._( Text( "Length: " ).margin_top( 5 ) )
                    ._( ComboBox()
                        .ref( (DecWidget **)&_pBlouseLengthComboBox )
                        .size( -1, 32 )
                        .border( true )
                        .onValueChanged( [=](DecWidget &widget) {
                            //SkDebugf( "BG: %i\n", (int)widget.value() );
                            showBlouseLength( widget.value() );
                        })
                    )                   
                )
                ._(Column().size( -1, 0).border(false)      
                    ._( Text( "Type: " ).margin_top( 5 ) )
                    ._( ComboBox()
                        .ref( (DecWidget **)&_pBlouseTypeComboBox )
                        .size( -1, 32 )
                        .border( true )
                        ._( ComboBoxItem( "สวมหัว" ) )
                        ._( ComboBoxItem( "ผ่าหน้า" ) )
                        .onValueChanged( [=](DecWidget &widget) {
                            //SkDebugf( "BG: %i\n", (int)widget.value() );
                            showBlouseType( widget.value() );
                        })
                    )                   
                )        
                ._(Column().size( -1, 0).border(false)      
                    ._( Text( "Neck: " ).margin_top( 5 ) )
                    ._( ComboBox()
                        .ref( (DecWidget **)&_pBlouseNeckComboBox )
                        .size( -1, 32 )
                        .border( true )
                        .onValueChanged( [=](DecWidget &widget) {
                            //SkDebugf( "BG: %i\n", (int)widget.value() );
                            showBlouseNeck( widget.value() );
                        })
                    )                   
                ) 
            )
            ._(Row().size( -1, 0).border(false).gap( 10 )       
                ._(Column().size( -1, 0).border(false)      
                    ._( Text( "Collar: " ).margin_top( 5 ) )
                    ._( ComboBox()
                        .ref( (DecWidget **)&_pCollarComboBox )
                        .size( -1, 32 )
                        .border( true )
                        .onValueChanged( [=](DecWidget &widget) {
                            //SkDebugf( "BG: %i\n", (int)widget.value() );
                            showCollar( widget.value() );
                        })
                    )                   
                )                              
                ._(Column().size( -1, 0).border(false)      
                    ._( Text( "Sleeve: " ).margin_top( 5 ) )
                    ._( ComboBox()
                        .ref( (DecWidget **)&_pSleeveComboBox )
                        .size( -1, 32 )
                        .border( true )
                        .onValueChanged( [=](DecWidget &widget) {
                            //SkDebugf( "BG: %i\n", (int)widget.value() );
                            showSleeve( widget.value() );
                        })
                    )                   
                )   
                ._(Column().size( -1, 0).border(false)      
                    ._( Text( "Hem: " ).margin_top( 5 ) )
                    ._( ComboBox()
                        .ref( (DecWidget **)&_pBlouseHemComboBox )
                        .size( -1, 32 )
                        .border( true )
                        .onValueChanged( [=](DecWidget &widget) {
                            //SkDebugf( "BG: %i\n", (int)widget.value() );
                            showBlouseHem( widget.value() );
                        })
                    )                   
                )                              
            )
            ._( Text( "Export: " ).margin_top( 5 ) )
            ._(Row().size( -1, 0 ).border( false ).gap( 10 )
                ._(Button( "Background", 0, [=](DecButton &btn) {
                    exportBG();
                }))
                ._(Button( "Body", 0, [=](DecButton &btn) {
                    exportBody();
                }))
                ._(Button( "Hair", 0, [=](DecButton &btn) {
                    exportHair();
                }))
                ._(Button( "Hand", 0, [=](DecButton &btn) {
                    exportHand();
                }))
                ._(Button( "Pant", 0, [=](DecButton &btn) {
                    exportPant();
                }))
            )
            ._(Row().size( -1, 0 ).border( false ).gap( 10 )
                ._(Button( "BlouseBody", 0, [=](DecButton &btn) {
                    exportBlouseBody();
                }))    
                ._(Button( "Collar", 0, [=](DecButton &btn) {
                    exportCollar();
                }))                              
                ._(Button( "Sleve", 0, [=](DecButton &btn) {
                    exportSleeve();
                }))                              
                ._(Button( "Hem", 0, [=](DecButton &btn) {
                    exportHem();
                }))    
            )
        )
    );
}

DecSVGEditor::~DecSVGEditor() {

}

DecSVGEditor &SVGEditor() {
    DecSVGEditor *w = new DecSVGEditor();
    return *w;
}



bool DecSVGEditor::onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) {
    if( state == skui::InputState::kDown ) {
        SkDebugf( "OnKey %i\n", (int)k );
        if( k == skui::Key::kB ) {
            _keyMode = 'b';
        }
        if( k == skui::Key::kC ) {
            _keyMode = 'c';
        }
        if( k == skui::Key::kL ) {
            _keyMode = 'l';
        }  
        if( k == skui::Key::kT ) {
            _keyMode = 't';
        }               
        if( k == skui::Key::kN ) {
            _keyMode = 'n';
        }      
        if( k == skui::Key::kH ) {
            _keyMode = 'h';
        }                 
        if( k == skui::Key::kS ) {
            _keyMode = 's';
        }                 
        if( _keyMode == 'b' ) {
            if( k >= skui::Key::k0 && k <= skui::Key::k4 ) {
                setBG( ((int)k - (int)skui::Key::k0) );
            }
        }
        else if( _keyMode == 'c' ) {
            if( k >= skui::Key::k0 && k <= skui::Key::k6 ) {
                setCollar( ((int)k - (int)skui::Key::k0) );
            }
        }        
        else if( _keyMode == 'l' ) {
            if( k >= skui::Key::k0 && k <= skui::Key::k2 ) {
                setLength( ((int)k - (int)skui::Key::k0) );
            }
        }
        else if( _keyMode == 't' ) {
            if( k >= skui::Key::k0 && k <= skui::Key::k1 ) {
                setType( ((int)k - (int)skui::Key::k0) );
            }
        }
        else if( _keyMode == 'n' ) {
            if( k >= skui::Key::k0 && k <= skui::Key::k4 ) {
                setNeck( ((int)k - (int)skui::Key::k0) );
            }
        }
        else if( _keyMode == 'h' ) {
            if( k >= skui::Key::k0 && k <= skui::Key::k2 ) {
                setHem( ((int)k - (int)skui::Key::k0) );
            }
        }    
        else if( _keyMode == 's' ) {
            if( k >= skui::Key::k0 && k <= skui::Key::k9 ) {
                setSleeve( ((int)k - (int)skui::Key::k0) );
            }
            if( k == skui::Key::kQ ) {
                setSleeve( 10 );
            }
            else if( k == skui::Key::kW ) {
                setSleeve( 11 );
            }
            else if( k == skui::Key::kE ) {
                setSleeve( 12 );
            }
        }             
    }
    return false;
}

void DecSVGEditor::setBG( int bg ) {
    _pBGComboBox->select( bg );
}

void DecSVGEditor::setLength( int l, bool show ) {
    if( show ) {
        _pBlouseLengthComboBox->select( l );
    }
    else {
        _nCurrentBlouseLength = l;
    }
}

void DecSVGEditor::showBG( int bg ) {
    _nCurrentBG = bg;
    SkDebugf( "BG: %i\n", _nCurrentBG );
    for( int i=0; i<_nBG; i++ ) {
        DecTreeItem *pItem = _vBGTreeItems[i];
        if( !pItem )
            continue;          
        if( i == bg ) {
            setNodeVisibility( pItem, true );
        }
        else {
            setNodeVisibility( pItem, false );
        }
    }
}

void DecSVGEditor::setHair( int bg ) {
    _pHairComboBox->select( bg );
}

void DecSVGEditor::showHair( int bg ) {
    for( int i=0; i<6; i++ ) {
        DecTreeItem *pItem = _vHairTreeItems[i];
        if( !pItem )
            continue;        
        if( i == bg ) {
            setNodeVisibility( pItem, true );
        }
        else {
            setNodeVisibility( pItem, false );
        }
    }
}

void DecSVGEditor::setType( int t ) {
    SkDebugf( "SetType: %i\n", t );
    _pBlouseTypeComboBox->select( t );
}

void DecSVGEditor::showBlouseType( int bg ) {
    _nCurrentBlouseType = bg;
    showBlouseNeck( _nCurrentBlouseNeck );
    showBlouseHem( _nCurrentBlouseHem );
}

void DecSVGEditor::setNeck( int n ) {
    _pBlouseNeckComboBox->select( n );
}

void DecSVGEditor::showBlouseNeck( int bg ) {
    _nCurrentBlouseNeck = bg;
    SkDebugf( "ShowNeck %i %i %i\n", _nCurrentBlouseType, _nCurrentBlouseLength, _nCurrentBlouseNeck );
    for( int n=0; n<4; n++ ) {
        for( int i=0; i<5; i++ ) {
            DecTreeItem *pItem = _vBlouseNeckTreeItems[n][i];
            if( !pItem )
                continue;    
            if( i == bg && n == (_nCurrentBlouseType * (_nCurrentBlouseLength+1)) ) {
                setNodeVisibility( pItem, true );
            }
            else {
                setNodeVisibility( pItem, false );
            }                
        }          
    }
}

void DecSVGEditor::setCollar( int c ) {
    _pCollarComboBox->select( c );
}

void DecSVGEditor::showCollar( int c ) {
    _nCurrentCollar = c;

    for( int i=0; i<7; i++ ) {
        DecTreeItem *pItem = _vCollarTreeItems[i];
        if( !pItem )
            continue;
        if( i == c ) {
            setNodeVisibility( pItem, true );
        }
        else {
            setNodeVisibility( pItem, false );
        }
    }    
}

void DecSVGEditor::setHem( int h ) {
    _pBlouseHemComboBox->select( h );
}

void DecSVGEditor::showBlouseHem( int h ) {
    _nCurrentBlouseHem = h;

    SkDebugf( "ShowHem %i %i %i\n", _nCurrentBlouseLength, _nCurrentBlouseType, _nCurrentBlouseHem );
    for( int ll=0; ll<3; ll++ ) {
        for( int tt=0; tt<2; tt++ ) {
            for( int hh=0; hh<3; hh++ ) {
                DecTreeItem *pItem = _vBlouseHemTreeItems[ll][tt][hh];
                if( !pItem )
                    continue;    
                if( h == hh && tt == _nCurrentBlouseType && ll == _nCurrentBlouseLength )  {
                    setNodeVisibility( pItem, true );
                }
                else {
                    setNodeVisibility( pItem, false );
                }
            }  
        }          
    }    
}

void DecSVGEditor::setBody( int bg ) {
    _pBodyComboBox->select( bg );
}

void DecSVGEditor::showBody( int bg ) {
    for( int i=0; i<2; i++ ) {
        DecTreeItem *pItem = _vBodyTreeItems[i];
        if( !pItem )
            continue;
        if( i == bg ) {
            setNodeVisibility( pItem, true );
        }
        else {
            setNodeVisibility( pItem, false );
        }
    }
}

void DecSVGEditor::setPant( int bg ) {
    _pPantComboBox->select( bg );
}

void DecSVGEditor::showPant( int bg ) {
    for( int i=0; i<2; i++ ) {
        DecTreeItem *pItem = _vPantTreeItems[i];
        if( !pItem )
            continue;        
        if( i == bg ) {
            setNodeVisibility( pItem, true );
        }
        else {
            setNodeVisibility( pItem, false );
        }
    }
}

void DecSVGEditor::setHand( int bg ) {
    _pHandComboBox->select( bg );
}

void DecSVGEditor::showHand( int bg ) {
    for( int i=0; i<2; i++ ) {
        DecTreeItem *pItem = _vHandTreeItems[i];
        if( !pItem )
            continue;        
        if( i == bg ) {
            setNodeVisibility( pItem, true );
        }
        else {
            setNodeVisibility( pItem, false );
        }
    }
}

void DecSVGEditor::setSleeve( int sl ) {
    _pSleeveComboBox->select( sl );
}

void DecSVGEditor::showSleeve( int sl ) {
    _nCurrentSleeve = sl;
    for( int i=0; i<13; i++ ) {
        DecTreeItem *pItem = _vSleeveTreeItems[i];
        if( !pItem )
            continue;         
        if( i == sl ) {
            setNodeVisibility( pItem, true );
        }
        else {
            setNodeVisibility( pItem, false );
        }
    }
}

void DecSVGEditor::showBlouseLength( int sl ) {
    _nCurrentBlouseLength = sl;
    for( int i=0; i<_nLen; i++ ) {
        DecTreeItem *pItem = _vBlouseLengthTreeItems[i];
        if( !pItem )
            continue;         
        if( i == sl ) {
            setNodeVisibility( pItem, true );
        }
        else {
            setNodeVisibility( pItem, false );
        }
    }
    showBlouseNeck( _nCurrentBlouseNeck );
    showBlouseHem( _nCurrentBlouseHem );
}

void DecSVGEditor::onLoad() {
    if (_pTextDocumentPath) {
        _pTextDocumentPath->setText(pSVGDomEditor->fPath.c_str());
        _pTextDocumentPath->dirty();
        pSVGDomEditor->dirty();
    }
    updateNodeTree();
}

void DecSVGEditor::init() {
    //SkDebugf("DecSVGEditor::init\n");
    //SkDebugf("Document path:%s\n", pSVGDomEditor ? pSVGDomEditor->fPath.c_str() : "");
    if( pSVGDomEditor ) {
        pSVGDomEditor->pSVGEditor = this;
        pSVGDomEditor->drawFit = true;
        pSVGDomEditor->setPaperSize( "Custom" );
        fDom = pSVGDomEditor->fDom;
    }       
    if (_pTextDocumentPath) {
        _pTextDocumentPath->setText(pSVGDomEditor->fPath.c_str());
        _pTextDocumentPath->dirty();
        pSVGDomEditor->dirty();
    }
    updateNodeTree();
}

void DecSVGEditor::openSVG() {
    //SkDebugf( "OpenSVG\n" );
    if( _pFileDialog ) {
        _pFileDialog->getFile( [=](DecFileDialogBox &dlg, const char *filepath) {
            SkDebugf( "Open File: %s\n", filepath );
            pNodeTree->clear();
            pSVGDomEditor->load(filepath);
            pSVGDomEditor->dirty();
            pSVGDomEditor->_showVisibleContentBounds = false;
        });
    }
}

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

void DecSVGEditor::setNodeVisibility( DecTreeItem *treeItem, bool bVisible ) {
    if( !treeItem->_externalData )
        return;
    SkSVGNode *node = (SkSVGNode *)treeItem->_externalData;
    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;
    if( bVisible ) {
        vType = SkSVGVisibility::Type::kVisible;
    }
    else {
        vType = SkSVGVisibility::Type::kHidden;
    }

    auto visible = SkSVGVisibility(vType);
    SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
    node->setVisibility(visibilityVisible);

    DecIcon *icon = (DecIcon *)treeItem->_externalWidget;
    icon->text_color( vType == SkSVGVisibility::Type::kVisible ? 0xFF000000 : 0xFFC0C0C0 );

    std::vector<DecWidget*>children = treeItem->getChildrenNodes();
    for( int i=0; i<children.size(); i++ ) {
        DecTreeItem *w = (DecTreeItem *)children[i];
        setNodeVisibility( w, bVisible );
    }
    pSVGDomEditor->dirty();
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

    const SkSVGProperty<SkSVGVisibility, true> &visibility = node->getVisibility();
    const SkSVGProperty<SkSVGStringType, false> &propData = node->getDataName();
    const SkSVGStringType* data = propData.getMaybeNull();
    const char* cpchData = data ? data->c_str() : "";
    std::string nodeName = std::string( cpchData );

    if( visibility.isValue() ) {
        vType = visibility->type();

        // can remove >>>
        /*
        int numChildren = 0;
        if (node->hasChildren()) {
            if (tag == SkSVGTag::kG) {
                SkSVGG* pG = (SkSVGG*)node;
                numChildren = (int)pG->fChildren.size();
            }
        }
        SkDebugf("%snode: %s(child:%d), has Visibility[%s], data-name:'%s' \n",
                 chDeep,
                 tagName[(int)tag],
                 numChildren,
                 visibilityType[(int)vType],
                 cpchData);
        */
        // can remove <<<
    } else {
        if (tag == SkSVGTag::kG) {
            auto visible = SkSVGVisibility(SkSVGVisibility::Type::kVisible);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            // can remove >>>
            /*
            int numChildren = 0;
            if (node->hasChildren()) {
                if (tag == SkSVGTag::kG) {
                    SkSVGG* pG = (SkSVGG*)node;
                    numChildren = (int)pG->fChildren.size();
                }
            }
            SkDebugf("%snode: %s(child:%d) set Visibility[Visible], data-name:'%s' \n",
                     chDeep,
                     tagName[(int)tag],
                     numChildren,
                     cpchData);
            */
            // can remove <<<
        }
    }
    if( deep <= 2 ) {
        SkDebugf("%i: %s %s %s \n",
                    deep,
                    parentNodeName.c_str(),
                    tagName[(int)tag], cpchData );    
    }

    sprintf(sz, "%s %s ", tagName[(int)tag], cpchData);
    treeNode._(TreeItem(sz).ref((DecWidget**)&new_node), true);  // add to front to display like Adobe Illustrator
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

    if( deep == 1 ) {
        if( nodeName == "หุ่น" ) {
            vType = SkSVGVisibility::Type::kVisible;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            _vBodyTreeItems[0] = NULL;
            _vBodyTreeItems[1] = new_node;
            _pBodyComboBox->_( ComboBoxItem( "0" ));
            _pBodyComboBox->_( ComboBoxItem( nodeName.c_str() ));  
            _pBodyComboBox->select( 1 );          
        }
        else if( nodeName == "มือ" ) {
            vType = SkSVGVisibility::Type::kVisible;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            _vHandTreeItems[0] = NULL;
            _vHandTreeItems[1] = new_node;

            _pHandComboBox->_( ComboBoxItem( "0" ));
            _pHandComboBox->_( ComboBoxItem( "มือ" ));  
        }     
        else if( nodeName == "กางเกง" ) {
            vType = SkSVGVisibility::Type::kVisible;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            _vPantTreeItems[0] = NULL;
            _vPantTreeItems[1] = new_node;
            _pPantComboBox->_( ComboBoxItem( "0" ));
            _pPantComboBox->_( ComboBoxItem( nodeName.c_str() ));  
            _pPantComboBox->select( 1 );          
        }             
        else if( nodeName == "BG" || nodeName == "ผม 2 ทรง" || nodeName == "คอ" ||
            nodeName == "แขนเสื้อ" || nodeName == "ความยาวเสื้อ" || nodeName == "shopping bags" ||
            nodeName == "ปกเสื้อ (คอกลม)" || nodeName == "ปกเสื้อ (คอวี)" ||
            nodeName == "ระบายชายเสื้อ 10 ซม." || nodeName == "ระบายชายเสื้อ 20 ซม.") {

            SkDebugf( "Level2: %s\n", nodeName.c_str() );

            vType = SkSVGVisibility::Type::kVisible;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);
        }
        else {
            SkDebugf( "Level2: %s\n", nodeName.c_str() );
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);
        }
    }
    if( deep == 2 ) {
        if( parentNodeName == "BG" ) {
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            if( _nBG == 0 ) {
                _vBGTreeItems[_nBG++] =  NULL;
                _pBGComboBox->_( ComboBoxItem( "0" ));
            }
            _vBGTreeItems[_nBG++] = new_node;
            _pBGComboBox->_( ComboBoxItem( nodeName.c_str() ));

            _pBGComboBox->select( 1 );  
        } else if( parentNodeName == "ผม 2 ทรง" ) {
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            if( _nHair == 0 ) {
                _pHairComboBox->_( ComboBoxItem( "0" )); 
                _pHairComboBox->_( ComboBoxItem( "1" )); 
                _pHairComboBox->_( ComboBoxItem( "2" )); 
                _pHairComboBox->_( ComboBoxItem( "3" )); 
                _pHairComboBox->_( ComboBoxItem( "4" )); 
                _pHairComboBox->_( ComboBoxItem( "5" )); 

                _vHairTreeItems[0] = NULL;
                _nHair++;
            }
            _vHairTreeItems[_nHair++] = new_node;
        } else if( parentNodeName == "แขนเสื้อ" ) {
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            if( _nSleeve == 0 ) {
                _pSleeveComboBox->_( ComboBoxItem( "แขนกุด" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขนสั้น" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขนสามส่วน" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขนยาว" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขนทิวลิป" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขน cap" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขนย้วย" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขน lantern" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขน flounce" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขนมารี" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขนผ่า" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขนตุ๊กตา" )); 
                _pSleeveComboBox->_( ComboBoxItem( "แขนระบาย" )); 
            }
            _nSleeve++;
            if( nodeName == "แขนกุด" ) {
                _vSleeveTreeItems[0] = new_node ; 
            }
            else {
                _vSleeveTreeItems[nodeName[0] - 'K'] = new_node;
            }

                      
        } else if( parentNodeName == "ความยาวเสื้อ" ) {
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            if( _nLen == 0 ) {
                _pBlouseLengthComboBox->_( ComboBoxItem( "เอว" ));
                _pBlouseLengthComboBox->_( ComboBoxItem( "สะโพกบน" ));
                _pBlouseLengthComboBox->_( ComboBoxItem( "สะโพกล่าง" ));
            }
            _nLen++;

            if( nodeName == "สะโพกเอว" ) {
                _vBlouseLengthTreeItems[0] = new_node;
                _pBlouseLengthComboBox->select( 0 );
            }
            else if( nodeName == "สะโพกบน" ) {
                _vBlouseLengthTreeItems[1] = new_node;
            }
            else if( nodeName == "สะโพกล่าง" ) {
                _vBlouseLengthTreeItems[2] = new_node;
            }
        } else if( parentNodeName == "คอ" ) {
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            int n = 0;
            if( ends_with( nodeName, "เอว" ) ) {
                n = 1;
            }          
            else if( ends_with( nodeName, "บน" ) ) {
                n = 2;
            }          
            else if( ends_with( nodeName, "ล่าง" ) ) {
                n = 3;
            }   

            int c = -1;
            if( starts_with( nodeName, "คอกลม" ) ) {
                c = 0;
            }       
            else if( starts_with( nodeName, "คอวี" ) ) {
                c = 1;
            }       
            else if( starts_with( nodeName, "คอปาด" ) ) {
                c = 2;
            }       
            else if( starts_with( nodeName, "คอสี่เหลี่ยม" ) ) {
                c = 3;
            }    
            else if( starts_with( nodeName, "คอคว้าน" ) ) {
                c = 4;
            }                 
            
            if( _nNeck == 0 ) {
                _pBlouseNeckComboBox->_( ComboBoxItem( "คอกลม" ));                     
                _pBlouseNeckComboBox->_( ComboBoxItem( "คอวี" ));                     
                _pBlouseNeckComboBox->_( ComboBoxItem( "คอปาด" ));                     
                _pBlouseNeckComboBox->_( ComboBoxItem( "คอสี่เหลี่ยม" ));                     
                _pBlouseNeckComboBox->_( ComboBoxItem( "คอคว้าน" ));                     
            }
            SkDebugf( "SetNeck: %i %i\n", n, c );
            _vBlouseNeckTreeItems[n][c] = new_node;

            if( nodeName == "คอกลม" ) {
                _pBlouseNeckComboBox->select( 0 );
            }            
            _nNeck++;
        } else if( parentNodeName == "ระบายชายเสื้อ 10 ซม." || parentNodeName == "ระบายชายเสื้อ 20 ซม.") {
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);   

            int l = -1;
            int t = 0;
            if( ends_with( nodeName, "เอว" ) ) {
                l = 0;
            }     
            else if( ends_with( nodeName, "เอว (ผ่าหน้า)" ) ) {
                l = 0;
                t = 1;
            }
            else if( ends_with( nodeName, "บน" ) ) {
                l = 1;
            }     
            else if( ends_with( nodeName, "บน (ผ่าหน้า)" ) ) {
                l = 1;
                t = 1;
            } 
            else if( ends_with( nodeName, "ล่าง" ) ) {
                l = 2;
            }     
            else if( ends_with( nodeName, "ล่าง (ผ่าหน้า)" ) ) {
                l = 2;
                t = 1;
            }                                 

            if( _nHem == 0 ) {
                _pBlouseHemComboBox->_( ComboBoxItem( "ไม่มีระบาย" ));                     
                _pBlouseHemComboBox->_( ComboBoxItem( "10 ซม." ));                     
                _pBlouseHemComboBox->_( ComboBoxItem( "20 ซม." ));                     
            }      
            _nHem++;    

            SkDebugf( "SetHem %s %i %i %i\n", nodeName.c_str(), l, t, 1 );
            if( parentNodeName == "ระบายชายเสื้อ 20 ซม." ) {
                _vBlouseHemTreeItems[l][t][2] = new_node;     
            }
            else {
                _vBlouseHemTreeItems[l][t][1] = new_node;     
            }

        } else if( parentNodeName == "ปกเสื้อ (คอกลม)" || parentNodeName == "ปกเสื้อ (คอวี)" ) {
            vType = SkSVGVisibility::Type::kHidden;  

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);      

            if( _nCollar == 0 ) {
                _pCollarComboBox->_( ComboBoxItem( "ไม่มีปก" ));
                _pCollarComboBox->_( ComboBoxItem( "ปกบัว" ));
                _pCollarComboBox->_( ComboBoxItem( "ปกบัวแหลม" ));
                _pCollarComboBox->_( ComboBoxItem( "ปกคอจีน" ));
                _pCollarComboBox->_( ComboBoxItem( "ปกระบาย" ));
                _pCollarComboBox->_( ComboBoxItem( "ปกผูกโบว์" ));
                _pCollarComboBox->_( ComboBoxItem( "ปกปลายแหลม" ));
            }
            _nCollar++;

            if( nodeName == "ปกบัว") {
                _vCollarTreeItems[1] = new_node;
            }
            else if( nodeName == "ปกบัวแหลม") {
                _vCollarTreeItems[2] = new_node;
            }
            else if( nodeName == "ปกคอจีน") {
                _vCollarTreeItems[3] = new_node;
            }
            else if( nodeName == "ปกระบาย") {
                _vCollarTreeItems[4] = new_node;
            }
            else if( nodeName == "ปกผูกโบว์") {
                _vCollarTreeItems[5] = new_node;
            }
            else {
                _vCollarTreeItems[6] = new_node;
            }                          
        }     
    }


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
    if( deep <= 2 ) {
        treeNode.expand();
    }
    else {
        treeNode.collapse();
    }    
}

const char *DecSVGEditor::getFitPaperSize( float w, float h ) {
    int fW = (int)(w * 25.4);
    int fH = (int)(h * 25.4);
    if( fW > fH ) { // landscape
        fW = (int)(h * 25.4);
        fH = (int)(w * 25.4);
    }
    for( int i=0; i<numPaperType; i++ ) {
        if( paperWidth[i] >= fW && paperHeight[i] >= fH ) {
            return paperName[i];
        }
    }
    return paperName[numPaperType];
}

void DecSVGEditor::updateNodeTree() {
    if( pSVGDomEditor ) {
        pSVGDomEditor->pSVGEditor = this;

        fDom = pSVGDomEditor->fDom;
        if( fDom ) {
            root = fDom->getRoot();

            //fDom->findNodeById()

            SkSize szC = fDom->containerSize();
            //SkDebugf( "Container size: %0.2f %0.2f\n", szC.fWidth, szC.fHeight );

            SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
            //SkDebugf( "SVG Root Size: %0.2f %0.2f\n", szR.fWidth, szR.fHeight );
            //SkDebugf( "SVG Root Inches: %0.2f %0.2f\n", szR.fWidth/72.0, szR.fHeight/72.0 );
            if( szR.fWidth == 0 || szR.fHeight == 0 ) {
                szR.fWidth = szC.fWidth;
                szR.fHeight = szC.fHeight;
            }
            
            char sz[256];
            sprintf( sz, "Paper Size: %0.2f x %0.2f inches (%s)", szR.fWidth/72.0, szR.fHeight/72.0,
                getFitPaperSize( szR.fWidth/72.0, szR.fHeight/72.0 ) );
            if( _pTextDocumentSize ) {
                _pTextDocumentSize->setText( sz );
            }

            SkSVGTag rtag = root->tag();
            sprintf( sz, "%s", tagName[(int)rtag] );

            DecTreeItem &rootItem = TreeItem( sz );
            if( pNodeTree ) {
                pNodeTree->_( rootItem );
            }

            if( root->hasChildren() ) {
                int numChildren = (int)root->fChildren.size();

                if( pNodeTree ) {
                    for( int i=0; i<numChildren; i++ ) {
                        sk_sp<SkSVGNode> node = root->fChildren.at( i );

                        addSubNodeToTreeItem( rootItem, node.get(), 1, "root" );
                    }
                }
            }
            _pBlouseTypeComboBox->select( 0 );
            _pBlouseHemComboBox->select( 0 );
            _pSleeveComboBox->select( 0 );
            _pCollarComboBox->select( 0 ); 
            _pBlouseNeckComboBox->select( 0 ); 
            _pHairComboBox->select( 1 );        
            _pHandComboBox->select( 1 );  

            rootItem.expand();
        }

    }
}


void DecSVGEditor::exportBG() {
    setJobFrameSkip( 5 );
    __jobExportBGIndex = 0;

    setBG( 0 );
    setBody( 0 );
    setCollar( 0 );
    setHair( 0 );
    setHand( 0 );
    setPant( 0 );
    setLength( -1 );
    setNeck( -1 );
    setSleeve( -1 );  

    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobExportBG( w );
    }) );    
}

bool DecSVGEditor::__jobExportBG( DecWidget &widget ) {
    __jobExportBGIndex++;
    setBG( __jobExportBGIndex);

    char sz[128];
    sprintf( sz, "resources/magic_pattern/Background%02i-01.png", __jobExportBGIndex );

    pSVGDomEditor->exportPNG( sz, 150 );

    if( __jobExportBGIndex >= 4 ) {
        return true;
    }
    return false;
}

void DecSVGEditor::exportHem() {
    __jobExportHemLengthIndex = 0;
    __jobExportHemTypeIndex = 0;
    __jobExportHemHemIndex = 0;

    setBG( 0 );
    setBody( 0 );
    setCollar( 0 );
    setHair( 0 );
    setHand( 0 );
    setPant( 0 );
    setLength( -1 );
    setNeck( -1 );
    setSleeve( -1 );  

    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobExportHem( w );
    }) );        
}

bool DecSVGEditor::__jobExportHem( DecWidget &widget ) {
    char sz[128];
    sprintf( sz, "resources/magic_pattern/Base%i%i%i-01.png", 
        __jobExportHemLengthIndex, __jobExportHemTypeIndex, __jobExportHemHemIndex );

    setLength( __jobExportHemLengthIndex, false );
    setType( __jobExportHemTypeIndex );
    setHem( __jobExportHemHemIndex );

    pSVGDomEditor->exportPNG( sz, 150 );

    __jobExportHemHemIndex++;
    if( __jobExportHemHemIndex > 2 ) {
        __jobExportHemHemIndex = 0;
        __jobExportHemTypeIndex++;
        if( __jobExportHemTypeIndex > 1 ) {
            __jobExportHemTypeIndex = 0;
            __jobExportHemLengthIndex++;
            if( __jobExportHemLengthIndex > 2 ) {
                return true;
            }
        }
    }


    return false;
} 

void DecSVGEditor::exportBody() {
    setBG( 0 );
    setBody( 1 );
    setCollar( 0 );
    setHair( 0 );
    setHand( 0 );
    setPant( 0 );
    setLength( -1 );
    setNeck( -1 );
    setSleeve( -1 );     

    char sz[128];
    sprintf( sz, "resources/magic_pattern/Body-01.png");   
    pSVGDomEditor->exportPNG( sz, 150 ); 
}

void DecSVGEditor::exportHand() {
    setBG( 0 );
    setBody( 0 );
    setCollar( 0 );
    setHair( 0 );
    setHand( 1 );
    setPant( 0 );
    setLength( -1 );
    setNeck( -1 );
    setSleeve( -1 );     

    char sz[128];
    sprintf( sz, "resources/magic_pattern/Hand-01.png");   
    pSVGDomEditor->exportPNG( sz, 150 ); 
}

void DecSVGEditor::exportPant() {
    setBG( 0 );
    setBody( 0 );
    setCollar( 0 );
    setHair( 0 );
    setHand( 0 );
    setPant( 1 );
    setLength( -1 );
    setNeck( -1 );
    setSleeve( -1 );     

    char sz[128];
    sprintf( sz, "resources/magic_pattern/Pant-01.png");   
    pSVGDomEditor->exportPNG( sz, 150 ); 
}

void DecSVGEditor::exportCollar() {
    __jobExportCollarIndex = 0;
    setBG( 0 );
    setBody( 0 );
    setCollar( 0 );
    setHair( 0 );
    setHand( 0 );
    setPant( 0 );
    setLength( -1 );
    setNeck( -1 );
    setSleeve( -1 );  

    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobExportCollar( w );
    }) );         
}

bool DecSVGEditor::__jobExportCollar( DecWidget &widget ) {
    char sz[128];
    sprintf( sz, "resources/magic_pattern/Collar%02i-01.png", __jobExportCollarIndex );

    setCollar( __jobExportCollarIndex );

    pSVGDomEditor->exportPNG( sz, 150 );

    __jobExportCollarIndex++;
    if( __jobExportCollarIndex > 6 ) {
        return true;
    }
    return false;
}

void DecSVGEditor::exportHair() {
    __jobExportHairIndex = 0;
    setBG( 0 );
    setBody( 0 );
    setCollar( 0 );
    setHair( 0 );
    setHair( 0 );
    setHand( 0 );
    setPant( 0 );
    setLength( -1 );
    setNeck( -1 );
    setSleeve( -1 );  

    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobExportHair( w );
    }) );         
}

bool DecSVGEditor::__jobExportHair( DecWidget &widget ) {
    char sz[128];
    sprintf( sz, "resources/magic_pattern/Hair%02i-01.png", __jobExportHairIndex );

    setHair( __jobExportHairIndex );

    pSVGDomEditor->exportPNG( sz, 150 );

    __jobExportHairIndex++;
    if( __jobExportHairIndex > 5 ) {
        return true;
    }
    return false;
}

void DecSVGEditor::exportBlouseBody() {
    __jobExportBlouseBodyLengthIndex = 0;
    __jobExportBlouseBodyTypeIndex = 0;
    __jobExportBlouseBodyNeckIndex = 0;

    setBG( 0 );
    setBody( 0 );
    setCollar( 0 );
    setHair( 0 );
    setHand( 0 );
    setPant( 0 );
    setLength( -1 );
    setNeck( -1 );
    setSleeve( -1 );  

    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobExportBlouseBody( w );
    }) );        
}

bool DecSVGEditor::__jobExportBlouseBody( DecWidget &widget ) {
    char sz[128];
    sprintf( sz, "resources/magic_pattern/Shirt%i%i%i-01.png", 
        __jobExportBlouseBodyLengthIndex, __jobExportBlouseBodyTypeIndex, __jobExportBlouseBodyNeckIndex );

    setLength( __jobExportBlouseBodyLengthIndex );
    setType( __jobExportBlouseBodyTypeIndex );
    setNeck( __jobExportBlouseBodyNeckIndex );

    pSVGDomEditor->exportPNG( sz, 150 );

    __jobExportBlouseBodyNeckIndex++;
    if( __jobExportBlouseBodyNeckIndex > 4 ) {
        __jobExportBlouseBodyNeckIndex = 0;
        __jobExportBlouseBodyTypeIndex++;
        if( __jobExportBlouseBodyTypeIndex > 1 ) {
            __jobExportBlouseBodyTypeIndex = 0;
            __jobExportBlouseBodyLengthIndex++;
            if( __jobExportBlouseBodyLengthIndex > 2 ) {
                return true;
            }
        }
    }


    return false;
} 


void DecSVGEditor::exportSleeve() {
    __jobExportSleeveIndex = 0;
    setBG( 0 );
    setBody( 0 );
    setCollar( 0 );
    setHair( 0 );
    setHair( 0 );
    setHand( 0 );
    setPant( 0 );
    setLength( -1 );
    setNeck( -1 );
    setSleeve( -1 );  

    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobExportSleeve( w );
    }) );         
}

bool DecSVGEditor::__jobExportSleeve( DecWidget &widget ) {
    char sz[128];
    sprintf( sz, "resources/magic_pattern/Sleeve%02i-01.png", __jobExportSleeveIndex );

    setSleeve( __jobExportSleeveIndex );

    pSVGDomEditor->exportPNG( sz, 150 );

    __jobExportSleeveIndex++;
    if( __jobExportSleeveIndex > 12 ) {
        return true;
    }
    return false;
}