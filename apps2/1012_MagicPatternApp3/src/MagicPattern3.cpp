#include <time.h>
#include <fstream>
#include "DeclarativeUI.h"
#include "MagicPattern3.h"
#include "MagicPatternSVG.h"
#include "DecSVGDom.h"
#include "modules/skresources/include/SkResources.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "modules/svg/include/SkSVGText.h"
#include "modules/svg/include/SkSVGG.h"
#include "modules/svg/include/SkSVGRenderContext.h"
#include "MPButton.h"
#include "MPPanel.h"

uint32_t _colorThemes[][3] {
    { 0xFFea8b94, 0xFFeba4a9, 0xFFffcacf },
    { 0xFFe54e85, 0xFFf26996, 0xFFfcacc9 },
    { 0xFF8471af, 0xFF9a87bb, 0xFFbaabdd },
    { 0xFFcacc6a, 0xFFd3d980, 0xFFf0f2a0 },
    { 0xFFd35854, 0xFFe37973, 0xFFf99795 },
    { 0xFF9dc6b6, 0xFFc1dbd2, 0xFFd5f7ea },
    { 0xFF094760, 0xFF0f5a7e, 0xFF1a84ad },
    { 0xFFa3a9c9, 0xFFc9cdeb, 0xFFebeeff },
    { 0xFF81bec6, 0xFFa2dce8, 0xFFc7f3f9 },
    { 0xFFd1c5ba, 0xFFf7ede3, 0xFFfcf4ed }
};
const int _numColorThemes = sizeof( _colorThemes ) / sizeof( _colorThemes[0] );

uint32_t _randomColors[][8] = {
    // เสื้อ        ปก          แขน         ระบาย       กระเป๋าบน    กระเป๋าล่าง   ฝากระเป๋าบน   ฝากระเป๋าล่าง
    { 0xFF8b2135, 0xFF612c40, 0xFF612c40, 0xFFdbb9c4, 0xFF612c40, 0xFF612c40, 0xFF612c40, 0xFF612c40 },
    { 0xFF2d435b, 0xFFcbc9cc, 0xFFcbc9cc, 0xFFcbc9cc, 0xFFcbc9cc, 0xFFcbc9cc, 0xFFcbc9cc, 0xFFcbc9cc },
    { 0xFFf26996, 0xFF9a87bb, 0xFF9a87bb, 0xFF9a87bb, 0xFF9a87bb, 0xFF9a87bb, 0xFF9a87bb, 0xFF9a87bb },
    { 0xFF0f5a7e, 0xFFa2dce8, 0xFFa2dce8, 0xFFa2dce8, 0xFFa2dce8, 0xFFa2dce8, 0xFFa2dce8, 0xFFa2dce8 },
    { 0xFF638181, 0xFFc1dbd2, 0xFFc1dbd2, 0xFFc1dbd2, 0xFFc1dbd2, 0xFFc1dbd2, 0xFFc1dbd2, 0xFFc1dbd2 },
    { 0xFFfbfbf9, 0xFF755d75, 0xFFfbfbf9, 0xFF755d75, 0xFFfbfbf9, 0xFFfbfbf9, 0xFFfbfbf9, 0xFFfbfbf9 },
    { 0xFFe37973, 0xFFeba4a9, 0xFFeba4a9, 0xFFeba4a9, 0xFFeba4a9, 0xFFeba4a9, 0xFFeba4a9, 0xFFeba4a9 },
    { 0xFF612c40, 0xFF202022, 0xFF755d75, 0xFF202022, 0xFF202022, 0xFF202022, 0xFF202022, 0xFF202022 },
    { 0xFF9a87bb, 0xFF755d75, 0xFF755d75, 0xFFc9cdeb, 0xFF755d75, 0xFF755d75, 0xFF755d75, 0xFF755d75 },
    { 0xFFd3d980, 0xFF638181, 0xFF638181, 0xFFc1dbd2, 0xFF638181, 0xFF638181, 0xFF638181, 0xFF638181 },
    { 0xFFf26996, 0xFFf26996, 0xFFfbfbf9, 0xFFfbfbf9, 0xFFf26996, 0xFFf26996, 0xFFf26996, 0xFFf26996 },
    { 0xFFb5927e, 0xFFdb976a, 0xFFf7e1cc, 0xFFf7e1cc, 0xFFdb976a, 0xFFdb976a, 0xFFdb976a, 0xFFdb976a },
    { 0xFF638181, 0xFF202022, 0xFF202022, 0xFF202022, 0xFF638181, 0xFF638181, 0xFF638181, 0xFF638181 },
    { 0xFF0f5a7e, 0xFF9cb2bf, 0xFF9cb2bf, 0xFF0f5a7e, 0xFF9cb2bf, 0xFF9cb2bf, 0xFF9cb2bf, 0xFF9cb2bf },
    { 0xFFd3d980, 0xFFd3d980, 0xFFc1dbd2, 0xFFc1dbd2, 0xFFd3d980, 0xFFd3d980, 0xFFd3d980, 0xFFd3d980 },
    { 0xFF755d75, 0xFF202022, 0xFF202022, 0xFFc9cdeb, 0xFF202022, 0xFF202022, 0xFF202022, 0xFF202022 },
    { 0xFFdbb9c4, 0xFFcbc9cc, 0xFFcbc9cc, 0xFF755d75, 0xFFcbc9cc, 0xFFcbc9cc, 0xFFcbc9cc, 0xFFcbc9cc },
    { 0xFF2d435b, 0xFF202022, 0xFF202022, 0xFF9cb2bf, 0xFF2D435b, 0xFF2D435b, 0xFF2D435b, 0xFF2D435b },
    { 0xFF202022, 0xFF9a87bb, 0xFF9a87bb, 0xFFfbfbf9, 0xFF202022, 0xFF202022, 0xFF202022, 0xFF202022 },
    { 0xFF202022, 0xFFcbc9cc, 0xFFcbc9cc, 0xFFfbfbf9, 0xFFcbc9cc, 0xFFcbc9cc, 0xFFcbc9cc, 0xFFcbc9cc },
    { 0xFFf7e1cc, 0xFF202022, 0xFF202022, 0xFFFbFbF9, 0xFFfbfbf9, 0xFFfbfbf9, 0xFFfbfbf9, 0xFFfbfbf9 }
};
const int _numRandomColors = sizeof( _randomColors ) / sizeof( _randomColors[0] );

int PocketRandom[][2][2] = {
    { { 1, 0 }, { 0, 0 } },
    { { 0, 1 }, { 0, 0 } },
    { { 1, 1 }, { 0, 0 } },
    { { 0, 0 }, { 1, 0 } },
    { { 0, 0 }, { 0, 1 } },
    { { 0, 0 }, { 1, 1 } },
    { { 1, 0 }, { 1, 1 } },
    { { 0, 1 }, { 1, 1 } },
    { { 1, 1 }, { 1, 1 } }
};
const int _numRandomPockets = sizeof( PocketRandom ) / sizeof( PocketRandom[0] );

const char *svgTagName[] = {
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

void DecMagicPattern::initNodes() {
    for( int i=0; i<NUM_BG; i++ ) {
        _vBGNode[i] = NULL;
    }
    for( int i=0; i<NUM_HAND; i++ ) {
        _vHandNode[i] = NULL;
    }
    for( int i=0; i<2; i++ ) {
        _vBodyNode[i] = NULL;
        _vShoeNode[i] = NULL;
    }
    for( int i=0; i<NUM_PANT; i++ ) {
        _vPantNode[i] = NULL;
    }
    for( int i=0; i<NUM_LENGTH; i++ ) {
        _vLengthNode[i] = NULL;
    }
    for( int i=0; i<NUM_HAIR; i++ ) {
        _vHairNode[i] = NULL;
    }
    for( int i=0; i<NUM_TYPE; i++ ) {
        for( int j=0; j<NUM_NECK; j++ ) {
            _vNeckNode[i][j] = NULL;
        }
    }
    for( int i=0; i<NUM_SLEEVE; i++ ) {
        _vSleeveNode[i] = NULL;
    }
    for( int i=0; i<NUM_COLLAR; i++ ) {
        _vCollarNode[i] = NULL;
    } 

    for( int i=0; i<NUM_HEM_BODY_LENGTH; i++ ) {
        for( int j=0; j<NUM_HEM_BODY_TYPE; j++ ) {
            for( int k=0; k<NUM_HEM_LENGTH; k++ ) {
                _vHemNode[i][j][k] = NULL;
            }
        }
    }

    for( int i=0; i<3; i++ ) {
        for( int j=0; j<2; j++ ) {
            for( int k=0; k<3; k++ ) {
                _vHemNode[i][j][k] = NULL;
            }
        }
    }
    for( int l=0; l<NUM_POCKET_LEVEL; l++ ) {
        for( int i=0; i<NUM_POCKET_SIDE; i++ ) {
            for( int p=0; p<NUM_POCKET_TYPE; p++ ) {
                _vPocketNode[l][i][p] = NULL;
            }
            for( int c=0; c<NUM_POCKETCOVER_TYPE; c++ ) {
                _vPocketCoverNode[l][i][c] = NULL;
            }
        }
    }      
}

DecMagicPattern::DecMagicPattern() {
    pSVG = NULL;
    svgStartLoad = false;
    svgLoaded = false;
    svgScanned = false;
    svgReady = false;
    pControllerContainer = NULL;

    for( int i=0; i<15; i++ ) {
        svgLayerCount[i] = 0;
    }

    pMPPanel = NULL;
    pButtonCollar = NULL;
    pButtonHem = NULL;
    pButtonPanel = NULL;

    bRandomPocket = false;

    currentSVGTag = 0;
    nCurrentPatternID = 0;
    currentColor = _randomColors[0][0];
    currentColorCollar = _randomColors[0][1];
    currentColorSleeve = _randomColors[0][2];
    currentColorHem = _randomColors[0][3];
    currentColorPocket0 = _randomColors[0][4];
    currentColorPocket1 = _randomColors[0][4];
    currentColorShoe = _randomColors[0][0];

    nSVGHeight = 0;
    fSVGScale = 1.0;
    fSVGZoomChange = 0;

    initNodes();

    nBackground = 0;
    nType = 0;
    nLength = 0;
    nNeck = 0;
    nCollar = 0;
    nSleeve = 0;
    nHem = 0;

    nLastNeck = -1;
    nLastType = -1;
    nLastCollar = -1;

    size( -1, -1 );
    border( false );
    background( false );
    cache( false );
    ID( "MP3 Main" );
    _( Stack()
        .ID( "MP3 Container" )
        .ref( (DecWidget **)&pContainer )
        .max_width( 428 )
        .max_height( 746 )
        .align( ALIGN_TOPCENTER )
        .cache( false )
        .background( true )
        .bg_color( 0xFFB0B0B0 )
        ._( Column() 
            .size( -1, -1 )
            .background( false )
            .border( false )
            // Tool bar
            ._(Row()
                .size( -1, 32 )
                .background( true )
                .bg_color( 0xFFFFFFFF )
                .border( false )
                .padding( 0, 20, 0, 20 )
                // Search Icon
                ._( Icon( 0xf002 )
                    .align( ALIGN_CENTER )
                    .fontSize( 16 )
                    .onMousePressed([=]( DecWidget &w ) -> bool {
                        pKeyPad->visible( !pKeyPad->visible() );
                        if( pKeyPad->visible() ) {
                            pKeyPad->dirty( true, true );
                        }
                        searchString = "";
                        _pTextSearch->text( searchString.c_str() );
                        return true;
                    }) 
                )         
                // Product ID        
                ._( Stack()
                    .size( 75, 0 )
                    .align( ALIGN_CENTER )
                    .border( false )
                    .background( false )
                    .margin( 0, 0, 0, 15 )
                    ._( Text("รหัส: " ) 
                        .ref( (DecWidget **)&pTextPID )
                        .align( ALIGN_VCENTERLEFT )
                    )
                )
                // SKU
                ._( Stack()
                    .size( 120, 0 )
                    .align( ALIGN_CENTER )
                    .border( false )
                    .background( false )
                    ._( Text("SKU: " ) 
                        .ref( (DecWidget **)&pTextSKU )
                        .align( ALIGN_VCENTERLEFT )
                    )
                )
                // Version
                ._( Stack()
                    .size( 50, 0 )
                    .align( ALIGN_CENTER )
                    .border( false )
                    .background( false )
                    ._( Text("v2.4 " ) 
                        .align( ALIGN_VCENTERLEFT )
                        .text_color( 0xFF808080 )
                    )
                )                
                ._( Spacer().size( -1, 0 ) )
                // Go to WooCommerce Product Page
                ._( Icon( 0xf07a )
                    .align( ALIGN_CENTER )
                    .fontSize( 20 )
                    .onMousePressed([=]( DecWidget &w ) -> bool {
                        getPattern();
                        return true;
                    }) 
                )                
            )
            // SVG
            ._( MagicPatternSVG() 
                .ref( (DecWidget **)&pSVG )
                .ID( "MP3 SVG" )
                .size( -1, -1 )
                .cache( true )
                .background( true )
                .bg_color( 0xFFFFFFFF )            
            )
        )
        // Logo
        ._( Image("images/MP-LOGO-s.png").background( false ).border( false )
            .align( ALIGN_TOPRIGHT ).margin( 40, 10, 5, 5 )
        )

        // Panel
        ._( Column()
            .size( -1, 0 )   
            .align( ALIGN_BOTTOMLEFT )     
            .border( false )
            .background( false ) 
            // Tools (Button)
            ._( Column()
                .size( 0, 0 )
                .margin( 10, 10, 10, 10 )
                .border( false )
                .background( false )
                .gap( 10 )
                ._( Button( "", 0xf2fb) 
                    .onMousePressed( [=](DecWidget &widget) -> bool {
                        randomPocket();
                        return true;
                    })
                    .size( 40, 40 )
                    .round( 20 )  
                    .border_color( 0x80000000 )
                    .text_color( 0xFF404040 )
                    .padding( 0, 0, 0, 0 )                    
                )                 
                // Random
                ._( Button( "", 0xe2ca) 
                    .onMousePressed( [=](DecWidget &widget) -> bool {
                        randomPattern();
                        return true;
                    })
                    .size( 40, 40 )
                    .round( 20 )  
                    .border_color( 0x80000000 )
                    .text_color( 0xFF404040 )
                    .padding( 0, 0, 0, 0 )                    
                )   
                /*
                ._( CheckBox( "ตัวอย่างกระเป๋า", [=](DecCheckBox &cb) {
                    bRandomPocket = cb.value();
                }) )             
                */
            )
            // Sub Menu Panel
            ._( MPPanel( this, 80 ) 
                .ref( (DecWidget **)&pMPPanel )
            )
            // Main Menu
            ._( Scroller( SCROLL_HORIZONTAL ) 
                .ref( (DecWidget **)&pButtonPanel )
                .size( -1, 0 )   
                //.padding( 8, 0, 8, 0 )   
                .background( true )
                .bg_color( 0x80FFFFFF )
                ._(Row()
                    .size( 0, 0 ).gap( 5 ).border(false).background(false)
                    .padding( 8, 8, 8, 8 )  
                    ._(MPButton( "รูปแบบเสื้อ", 0, [=](DecButton &btn) { pMPPanel->setMode( 0 ); 
                        nLastNeck = -1; nLastCollar = -1; } ))
                    ._(MPButton( "ความยาวเสื้อ", 0, [=](DecButton &btn) { pMPPanel->setMode( 1 ); } ))
                    ._(MPButton( "คอเสื้อ", 0, [=](DecButton &btn) { pMPPanel->setMode( 2 ); nLastType = -1; nLastCollar = -1; } ))
                    ._(MPButton( "ปกเสื้อ", 0, [=](DecButton &btn) { 
                            if( nNeck == 0 ) {
                                pMPPanel->setMode( 3 ); 
                            }
                            else if( nNeck == 1 ) {
                                pMPPanel->setMode( 6 );
                            }
                        } )
                        .ref( (DecWidget **)&pButtonCollar )
                    )
                    ._(MPButton( "แขนเสื้อ", 0, [=](DecButton &btn) { pMPPanel->setMode( 4 ); } ))
                    ._(MPButton( "ระบายชายเสื้อ", 0, [=](DecButton &btn) { pMPPanel->setMode( 5 ); } )
                        .ref( (DecWidget **)&pButtonHem )
                    )
                    ._(MPButton( "กระเป๋า", 0, [=](DecButton &btn) { pMPPanel->setMode( 7 ); } ))
                )  
            )  
            // Color
            ._( Scroller( SCROLL_HORIZONTAL ) 
                .size( -1, 0 )  
                ._( Row()
                    .padding( 0, 10, 10, 10 )
                    .size( 0, 0 ).gap( 2 ).border(false).background(false)
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFea8b94 ); } ).gap(0).bg_color( 0xFFea8b94 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFeba4a9 ); } ).gap(0).bg_color( 0xFFeba4a9 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFffcacf ); } ).gap(0).bg_color( 0xFFffcacf ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFe54e85 ); } ).gap(0).bg_color( 0xFFe54e85 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFf26996 ); } ).gap(0).bg_color( 0xFFf26996 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFfcacc9 ); } ).gap(0).bg_color( 0xFFfcacc9 ).size( 30, 30 ).round(15) ) 

                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFF8471af ); } ).gap(0).bg_color( 0xFF8471af ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFF9a87bb ); } ).gap(0).bg_color( 0xFF9a87bb ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFbaabdd ); } ).gap(0).bg_color( 0xFFbaabdd ).size( 30, 30 ).round(15) ) 

                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFcacc6a ); } ).gap(0).bg_color( 0xFFcacc6a ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFd3d980 ); } ).gap(0).bg_color( 0xFFd3d980 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFf0f2a0 ); } ).gap(0).bg_color( 0xFFf0f2a0 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFd35854 ); } ).gap(0).bg_color( 0xFFd35854 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFe37973 ); } ).gap(0).bg_color( 0xFFe37973 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFf99795 ); } ).gap(0).bg_color( 0xFFf99795 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFF9dc6b6 ); } ).gap(0).bg_color( 0xFF9dc6b6 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFc1dbd2 ); } ).gap(0).bg_color( 0xFFc1dbd2 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFd5f7ea ); } ).gap(0).bg_color( 0xFFd5f7ea ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFF094760 ); } ).gap(0).bg_color( 0xFF094760 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFF0f5a7e ); } ).gap(0).bg_color( 0xFF0f5a7e ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFF1a84ad ); } ).gap(0).bg_color( 0xFF1a84ad ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFa3a9c9 ); } ).gap(0).bg_color( 0xFFa3a9c9 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFc9cdeb ); } ).gap(0).bg_color( 0xFFc9cdeb ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFebeeff ); } ).gap(0).bg_color( 0xFFebeeff ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFF81bec6 ); } ).gap(0).bg_color( 0xFF81bec6 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFa2dce8 ); } ).gap(0).bg_color( 0xFFa2dce8 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFc7f3f9 ); } ).gap(0).bg_color( 0xFFc7f3f9 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFd1c5ba ); } ).gap(0).bg_color( 0xFFd1c5ba ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFf7ede3 ); } ).gap(0).bg_color( 0xFFf7ede3 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xFFfcf4ed ); } ).gap(0).bg_color( 0xFFfcf4ed ).size( 30, 30 ).round(15) ) 

                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8CBC9CC ); } ).gap(0).bg_color( 0xE8CBC9CC ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE82D435B ); } ).gap(0).bg_color( 0xE82D435B ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE89CB2BF ); } ).gap(0).bg_color( 0xE89CB2BF ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8638181 ); } ).gap(0).bg_color( 0xE8638181 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8919F88 ); } ).gap(0).bg_color( 0xE8919F88 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8202022 ); } ).gap(0).bg_color( 0xE8202022 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE88B2135 ); } ).gap(0).bg_color( 0xE88B2135 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8612C40 ); } ).gap(0).bg_color( 0xE8612C40 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8755D75 ); } ).gap(0).bg_color( 0xE8755D75 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8DBB9C4 ); } ).gap(0).bg_color( 0xE8DBB9C4 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8E4B366 ); } ).gap(0).bg_color( 0xE8E4B366 ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8DB976A ); } ).gap(0).bg_color( 0xE8DB976A ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8B5927E ); } ).gap(0).bg_color( 0xE8B5927E ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8F7E1CC ); } ).gap(0).bg_color( 0xE8F7E1CC ).size( 30, 30 ).round(15) ) 
                    ._(Button( "", 0, [=](DecButton &btn) { this->setColor( 0xE8FBFBF9 ); } ).gap(0).bg_color( 0xE8FBFBF9 ).size( 30, 30 ).round(15) )                 
                )
            )            
        )
        // Zoom Panel
        ._( Column()
            .size( 0, 0 )
            .border( false )
            .background( false )
            .align( ALIGN_TOPLEFT )
            .margin( 52, 10, 10, 10 )
            .gap( 10 )
            ._( Button( "", 0xf010) 
                // Zoom out
                .onMousePressed( [=](DecWidget &widget) -> bool {
                    fSVGZoomChange = 0 - (fSVGScale * 0.1);
                    return true;
                })
                .onMouseReleased( [=](DecWidget &widget) -> bool {
                    fSVGZoomChange = 0;
                    return true;
                })                
                .size( 40, 40 )
                .round( 20 )  
                .border_color( 0x80000000 )
                .text_color( 0xFF404040 )
                .padding( 0, 0, 0, 0 )
            )
            ._( Button( "", 0xf00e) 
                // Zoom in
                .onMousePressed( [=](DecWidget &widget) -> bool {
                    fSVGZoomChange = fSVGScale * 0.1;
                    return true;
                })
                .onMouseReleased( [=](DecWidget &widget) -> bool {
                    fSVGZoomChange = 0;
                    return true;
                })                
                .size( 40, 40 )
                .round( 20 )                
                .border_color( 0x80000000 )
                .text_color( 0xFF404040 )
                .padding( 0, 0, 0, 0 )
            )            
        )
        // Key Pad
        ._( Column()
            .ref( (DecWidget **)&pKeyPad )
            .size( 0, 0 )
            .align( ALIGN_TOPLEFT )
            .round( 10 )
            .margin( 40, 10, 10, 10 )
            .padding( 10, 10, 10, 10 )
            .border_color( 0x80808080 )
            .gap( 10 )
            .visible( false )
            ._( Row()
                .size( 32*3+20, 0 )
                ._( Spacer().size( -1, 0 ) )
                ._( Text("")
                    .ref( (DecWidget **)&_pTextSearch )
                    .align( ALIGN_TOPCENTER )
                    .fontSize(20) 
                    .size( 0, 0 )
                )
                ._( Spacer().size( -1, 0) )
            )
            ._( Row()
                .gap( 10 )
                .size( 0, 0 )
                .border( false )
                ._( Button( "1", 0, [=]( DecWidget &w) -> bool {
                    addSearchChar( "1" );
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
                ._( Button( "2", 0, [=]( DecWidget &w) -> bool {
                    addSearchChar( "2" );
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
                ._( Button( "3", 0, [=]( DecWidget &w) -> bool {
                    addSearchChar( "3" );
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
            )
            ._( Row()
                .gap( 10 )
                .size( 0, 0 )
                .border( false )
                ._( Button( "4", 0, [=]( DecWidget &w) -> bool {
                    addSearchChar( "4" );
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
                ._( Button( "5", 0, [=]( DecWidget &w) -> bool {
                    addSearchChar( "5" );
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
                ._( Button( "6", 0, [=]( DecWidget &w) -> bool {
                    addSearchChar( "6" );
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
            )            
            ._( Row()
                .size( 0, 0 )
                .gap( 10 )
                .border( false )
                ._( Button( "7", 0, [=]( DecWidget &w) -> bool {
                    addSearchChar( "7" );
                    return true;
                } ).size( 32, 32 ).border_color( 0x80808080 ) )
                ._( Button( "8", 0, [=]( DecWidget &w) -> bool {
                    addSearchChar( "8" );
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
                ._( Button( "9", 0, [=]( DecWidget &w) -> bool {
                    addSearchChar( "9" );
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
            )        
            ._( Row()
                .size( 0, 0 )
                .gap( 10 )
                .border( false )
                ._( Button( "", 0xf55a, [=]( DecWidget &w) -> bool {
                    addSearchChar( "X" );
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
                ._( Button( "0", 0, [=]( DecWidget &w) -> bool {
                    addSearchChar( "0" );
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
                ._( Button( "OK", 0, [=]( DecWidget &w) -> bool {
                    if( searchString.length() == 0 ) {
                        return true;
                    }
                    gotoPattern( atoi( searchString.c_str() ));
                    pKeyPad->visible( false );
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
            )        
        )

    );
#ifndef __EMSCRIPTEN__  
    _( Column() 
        .ref( (DecWidget **)&pControllerContainer )
        .ID( "MP3 Controller" )
        .margin( 10, 10, 10, 10 )
        .border( false )
        .max_width( 280 )
        .visible( true )
        .gap( 10 )
        ._( Text("Developer's Tools" ) )
        ._( ComboBox() 
            .ref( (DecWidget **)&pComboDevices )
            .ID( "MP3 Device Combo" )
            ._( ComboBoxItem( "iPhone 8" ) )
            ._( ComboBoxItem( "iPhone 8 Plus" ) )
            ._( ComboBoxItem( "iPhone 11" ) )
            ._( ComboBoxItem( "iPhone 11 Pro" ) )
            ._( ComboBoxItem( "iPhone 11 Pro Max" ) )
            ._( ComboBoxItem( "iPhone SE 2nd gen" ) )
            ._( ComboBoxItem( "iPhone 12 Mini" ) )
            ._( ComboBoxItem( "iPhone 12" ) )
            ._( ComboBoxItem( "iPhone 12 Pro" ) )
            ._( ComboBoxItem( "iPhone 12 Pro Max" ) )
            ._( ComboBoxItem( "iPod Touch 7th gen" ) )
            ._( ComboBoxItem( "iPad Pro 9.7 inches" ) )
            ._( ComboBoxItem( "iPad Pro 11 inches" ) )
            ._( ComboBoxItem( "iPad Pro 12.9 inches" ) )
            ._( ComboBoxItem( "iPad 8th gen" ) )
            ._( ComboBoxItem( "iPad Air 4th gen" ) )
            ._( ComboBoxItem( "Custom" ) )
            .onValueChanged( [=](DecWidget &widget) {
                SkDebugf( "Device Changed %i\n", (int)widget.value() );
                onDeviceChanged((int)widget.value());
            })          
        )
        ._( Row()
            .size( -1, 0 )
            .border( false )
            .gap( 10 )
            ._( Button( "Export PNG", 0, [=](DecButton &btn) {
                exportPNG();
            }))
            ._( Button( "Export All PNGs", 0, [=](DecButton &btn) {
                exportPNGs();
            }))        
        )
        ._( Button( "Export SVG", 0, [=](DecButton &btn) {
            exportSVG();
        }))
        ._( Button( "Export WooCommerce Product List", 0, [=](DecButton &btn) {
            exportProductList();
        }))
    );
#endif
}

DecMagicPattern::~DecMagicPattern(){

} 

DecMagicPattern &MagicPattern() {
    SkDebugf("MagicPattern\n");
    DecMagicPattern *w = new DecMagicPattern();
    return *w;
}

std::string DecMagicPattern::getDescription() {
    static const char *desc_nect[] = { "คอกลม", "คอวี", "คอปาด", "คอสี่เหลี่ยม", "คอคว้าน", "คอสี่เหลี่ยมแคบ", "คอคว้านแคบ", "คอหยดน้ำผูกโบว" };
    static const char *desc_collar[] = { "ไม่มีปก", "ปกบัว", "ปกบัวแหลม", "ปกคอจีน", "ปกระบาย", "ปกผูกโบว์", "ปกปลายแหลม" };
    static const char *desc_type[] = { "", "สวมศีรษะ", "ผ่าหน้า" };
    static const char *desc_length[] = { "", "เอว", "สะโพกสูง", "สะโพกล่าง", "ระดับเข่า", "คลุมเข่า" };
    static const char *desc_sleeve[] = { "", "แขนกุด", "แขนสั้น", "แขนสามส่วน", "แขนยาว", "แขนทิวลิป", "แขน cap", "แขนย้วย", "แขน lantern", "แขน Flounce", "แขนมารี", "แขนผ่า", "แขนตุ๊กตา", "แขนระบาย" };
    static const char *desc_hem[] = { "", "แต่งระบายชายเสื้อแบบสั้น ", "แต่งระบายชายเสื้อแบบยาว " };

    std::string blouse_dress = "เสื้อ";
    if( nLength > 3 ) {
        blouse_dress = "เดรส";
    }
    //SkDebugf( "Type: %i %s\n", nLength, desc_length[nLength] );
    static char szy[512];
    sprintf( szy, "แพทเทิร์น%s%s %s %s ความยาวระดับ%s %s %s", 
        blouse_dress.c_str(),
        desc_nect[nNeck],
        desc_collar[nCollar],
        desc_type[nType],
        desc_length[nLength],
        desc_sleeve[nSleeve],
        desc_hem[nHem]
    ); 
    return std::string( szy );
}

void DecMagicPattern::getPattern() {
    SkDebugf( "Product ID: %04i\n", nCurrentPatternID );
    SkDebugf( "SKU: %s\n", sku.c_str() );
    //SkDebugf( "Description: %s\n", getDescription().c_str() );

    char sz[1024];
    sprintf( sz, "getPattern( '%04i', '%s', '%s' )", nCurrentPatternID, sku.c_str(), getDescription().c_str() );
    
#ifdef __EMSCRIPTEN__
    emscripten_run_script(sz);
#endif
}

void DecMagicPattern::addSearchChar( std::string s ) {
    if( s == "X" ) {
        if( searchString.length() == 0 ) {
            return;
        }
        searchString = searchString.substr( 0, searchString.length()-1);
    }
    else {
        if( searchString.length() == 4 ) {
            return;
        }
        searchString.append( s );
    }
    _pTextSearch->text( searchString.c_str() );
}

 bool DecMagicPattern::checkSKUMaterial( std::string sz0, std::string sz1, std::string sz2, std::string sz3, 
        std::string sz5, std::string sz6, std::string sz7 )
 {
    if (sz0.find(sku[0]) == std::string::npos) return false;
    if (sz1.find(sku[1]) == std::string::npos) return false;
    if (sz2.find(sku[2]) == std::string::npos) return false;
    if (sz3.find(sku[3]) == std::string::npos) return false;
    if (sz5.find(sku[5]) == std::string::npos) return false;
    if (sz6.find(sku[6]) == std::string::npos) return false;
    if (sz7.find(sku[7]) == std::string::npos) return false;
    return true;
}

bool DecMagicPattern::checkSKUVideo( std::string sz0, std::string sz1, std::string sz2, std::string sz3 ) {
    if (sz0.find(sku[0]) == std::string::npos) return false;
    if (sz1.find(sku[1]) == std::string::npos) return false;
    if (sz2.find(sku[2]) == std::string::npos) return false;
    if (sz3.find(sku[3]) == std::string::npos) return false;
    return true;
}

bool DecMagicPattern::checkSKUVideo2( std::string sz0, std::string sz1, std::string sz2, std::string sz3, std::string sz5 ) {
    if (sz0.find(sku[0]) == std::string::npos) return false;
    if (sz1.find(sku[1]) == std::string::npos) return false;
    if (sz2.find(sku[2]) == std::string::npos) return false;
    if (sz3.find(sku[3]) == std::string::npos) return false;
    if (sz5.find(sku[5]) == std::string::npos) return false;
    return true;
}

void DecMagicPattern::exportProductList() {
    std::ofstream f;
    static char szx[8129];

    for( int i=0; i<3718; i++ ) {
        // divide CSV every 1,000 items
        if( i % 1000 == 0 ) {
            if( i != 0 ) {
                f.close();
            }
            char sz[128];
            sprintf( sz, "_files/magicpattern_product2-%04i.csv", i );
            f.open( sz );

            sprintf( szx, "ID,Type,SKU,Name,Published,Is featured?,Visibility in catalog,Short description,Description,Date sale price starts,Date sale price ends,Tax status,Tax class,In stock?,Stock,Low stock amount,Backorders allowed?,Sold individually?,Weight (kg),Length (cm),Width (cm),Height (cm),Allow customer reviews?,Purchase note,Sale price,Regular price,Categories,Tags,Shipping class,Images,Download limit,Download expiry days,Parent,Grouped products,Upsells,Cross-sells,External URL,Button text,Position,Download 1 ID,Download 1 name,Download 1 URL,Download 2 ID,Download 2 name,Download 2 URL\n");
            if( f ) {
                f.write( (const char *)szx, strlen(szx) );
            }
        }

        gotoPattern( i+1 );

        // Description
        std::string desc = getDescription() + "<br><br>";
        desc = desc + "แพทเทิร์นมีเส้นแนวเย็บและเผื่อตะเข็บเย็บแล้ว<br><br>";
        desc = desc + "ท่านจะได้รับแพทเทิร์นทั้ง 5 ไซส์ (S, M, L, XL, XXL)<br><br>";
        desc = desc + "<p style='color: red;'>(ในแพทเทิร์นมีเส้นตำแหน่งสำหรับกระเป๋าปะให้ แต่แพทเทิร์นเสื้อไม่รวมแพทเทิร์นกระเป๋า" +
                "ซึ่งต้องซื้อแยก 1 ชุดประกอบด้วยแพทเทิร์นกระเป๋าปะ 10 แบบ และฝากระเป๋า 4 แบบ " +
                "ท่านสามารถซื้อแพทเทิร์นกระเป๋าปะได้ที่ลิงค์นี้ค่ะ " + 
                "<a style='color:blue;' href='https://magicpattern.co/product/แพทเทิร์นกระเป๋าปะ-10-แบบ'>แพทเทิร์นกระเป๋าปะ</a>)</p><br>";

        // Short Description
       
        std::string price ="<span style='color: #ff0000;'>ราคาปกติ 400 บาท ซื้อ 2 แบบขึ้นไปลดเหลือแบบละ 250 บาท<br>ซื้อ 10 แบบขึ้นไป แถมฟรี 1 แบบ</span><br><br>";
        static char szShort[2048];
        sprintf( szShort, "%s%s", price.c_str(), desc.c_str() );
       
        // 9/9
         /*
        std::string price ="<span style='color: #ff0000;'>ราคาปกติ 400 บาท ลดเหลือ 199 บาท ซื้อชุดต่อไปในตะกร้าเดียวกัน เหลือชุดละ 99.- ซื้อกี่ชุดก็ได้ใน 1 ตะกร้า</span><br><br>";
        static char szShort[2048];
        std::string promotion = "<div style='background-color: #ff0000; color: #ffffff; font-size:40px; text-align:center; margin-bottom:10px;'>ลดราคาพิเศษ 9/9</div>";
        sprintf( szShort, "%s%s%s", promotion.c_str(), price.c_str(), desc.c_str() );
         */
 
        // Size (Body Length)
        static char szBodyLength[512];
        sprintf( szBodyLength, "<img class=""aligncenter wp-image-5034"" src=""https://magicpattern.co/resources/_magicpattern/Sizes/size%c.png"" alt=""ขนาดตัวเสื้อสำเร็จ"" width=""640"" /></p>\\n", sku[0] );

        // Size (Sleeve)
        static char szSleeveLength[512];
        if( sku[6] != 'H' ) 
            sprintf( szSleeveLength, "<img class=""aligncenter wp-image-5045"" src=""https://magicpattern.co/resources/_magicpattern/Sizes/sleeve%c.png"" alt=""ขนาดแขนเสื้อสำเร็จ"" width=""640"" height=""105"" /></p>\\n", sku[6] );
        else 
            szSleeveLength[0] = 0;

        // What you will get
        std::string strYouGot = std::string("<p>สิ่งที่ท่านจะได้รับ :</p><ol>") +
            "<li>ไฟล์เอกสารหน้าปกและรายละเอียดแพทเทิร์น ขนาด A4</li>" + 
            "<li>ไฟล์แพทเทิร์น ขนาดกระดาษ A4 แบบขาวดำ (สำหรับพิมพ์เพื่อทาบตัดผ้า) รวม 5 ไซส์ (5 ไฟล์)</li>" + 
            "<li>ไฟล์แพทเทิร์น ขนาดกระดาษ A1 แบบขาวดำ (สำหรับพิมพ์เพื่อทาบตัดผ้า) รวม 5 ไซส์ (5 ไฟล์)</li>" +
            "<li>ไฟล์แพทเทิร์น ขนาดกระดาษ A1 พิมพ์ 4 สีรวม 5 ไซส์ในไฟล์เดียว</li>" +
            "<li>ลิงค์ดูวิดีโอวิธีการตัดเย็บ</li></ol>";

        // Parts
        static char szPNG[2560];
        char szPNGFN[256];
        szPNG[0] = 0;
        strcat( szPNG, "<br>&nbsp;<br>&nbsp;<br><h3 style=\"\"text-align: center;\"\">ชิ้นส่วนแพทเทิร์น</h3><p style=\"\"text-align: center;\"\">\\n" );

        for( int i=0; i<10; i++ ) {
            sprintf( szPNGFN, "resources/_export_png_web/%04i-%04i.png", nCurrentPatternID, i );
            if( std::filesystem::exists( szPNGFN ) ) {
                sprintf( szPNGFN, "<img class=\"\"alignnone size-full wp-image-6621\"\" src=\"\"https://magicpattern.co/resources/_export_png_web/%04i-%04i.png\"\" alt=\"\"\"\" />", nCurrentPatternID, i );
                strcat( szPNG, szPNGFN );
            }
            else {
                break;
            }
        }  
        strcat( szPNG, "</p>" );  
        //SkDebugf( "%s\n", szPNG ); 

        static char szMaterial[512] = "";
        static char szVideo1[2048] = "";
        static char szVideo2[2048] = "";

        // Video
        // 1. ตัดผ้าตัวเสื้อ/ชุดเดรส
        int n = -1;
        if( checkSKUVideo( "WHL", "P", "A",       "SN" ) ) n = 0;
        if( checkSKUVideo( "WHL", "P", "BCDEFG",  "SN" ) ) n = 1;
        if( checkSKUVideo( "WHL", "P", "H",       "SN" ) ) n = 2;
        if( checkSKUVideo( "WHL", "B", "ABCDEFG", "SN" ) ) n = 3;
        if( checkSKUVideo( "KC",  "P", "A",       "SN" ) ) n = 4;
        if( checkSKUVideo( "KC",  "P", "BCDEFG",  "SN" ) ) n = 5;
        if( checkSKUVideo( "KC",  "P", "H",       "SN" ) ) n = 6;
        if( checkSKUVideo( "KC",  "B", "ABCDEFG", "SN" ) ) n = 7;
        if( n != -1 ) {
            const char *szv1[] = {
                "nAY-8OM2gC0", "8tUFiOVzydY", "x3Cwx42BZ-k", "d8-caun9EL4",
                "uAvnogAnpCU", "YnAPeHzAmUE", "tROpdVBc_84", "YTnRPV6Ce7M"
            };
            sprintf( szVideo1, "<br>&nbsp;<br><h3 style='text-align: center;'>1. ตัดผ้าตัวเสื้อ/ชุดเดรส</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/%s"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>", szv1[n]);        
        }
        else {
            szVideo1[0] = 0;
            SkDebugf( "No Intro Video: %s\n", sku.c_str() );
            exit( 1 );
        }

        // 2 ปกเสื้อ
        if( sku[5] == 'A'  ) {
            sprintf( szVideo2, "<br>&nbsp;<br><h3 style='text-align: center;'>2. ปกเสื้อ</h3><p style='text-align: center;'>(ชุดนี้ไม่มีปก)</p>\\n" );
        }
        else {
            if( sku[5] == 'E'  ) {
                sprintf( szVideo2, "<br>&nbsp;<br><h3 style='text-align: center;'>2. ปกเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/WojljAiy4tU"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
ขั้นตอนการตัดเย็บปกระบาย<br>\
00:00 Intro<br>\
00:19 เตรียมแพทเทิร์นก่อนเย็บ,วางผ้า,ตัดผ้า<br>\
01:02 การเย็บปกระบาย<br>\
04:16 การวางปกเสื้อ เพื่อเย็บติดกับคอเสื้อ<br>\
05:50 ปกระบายชิ้นสำเร็จ<br>\
<br>" );
            }
            else if( sku[5] == 'F'  ) {
                sprintf( szVideo2, "<br>&nbsp;<br><h3 style='text-align: center;'>2. ปกเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/13j5tFoxw9E"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
ขั้นตอนการตัดเย็บปกผูกโบว์<br>\
00:00 Intro<br>\
00:19 เตรียมแพทเทิร์นก่อนเย็บ,วางผ้า,ตัดผ้า<br>\
01:23 อธิบายปกผูกโบว์<br>\
01:40 ปกผูกโบว์สำหรับเสื้อสวมหัว<br>\
01:57 ปกผูกโบว์สำหรับเสื้อผ่าหน้า <br>\
02:31 การเย็บประกอบปกผูกโบว์(สวมหัว,ผ่าหน้า)<br>\
04:23 การวางปกเสื้อ เพื่อเย็บติดกับคอเสื้อแบบสวมหัว<br>\
05:33 การวางปกเสื้อ เพื่อเย็บติดกับคอเสื้อแบบผ่าหน้า<br>\
05:58 ปกผูกโบว์ชิ้นสำเร็จ<br>\
<br>" );
            }
            else if( sku[5] == 'G'  ) {
                sprintf( szVideo2, "<br>&nbsp;<br><h3 style='text-align: center;'>2. ปกเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/gVrsTAX-v5I"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
ขั้นตอนการตัดเย็บปกปลายแหลม<br>\
00:00 Intro<br>\
00:20 เตรียมแพทเทิร์นก่อนเย็บ,วางผ้า,ตัดผ้า<br>\
01:26 รีดผ้ากาวปกเสื้อ1ชิ้น<br>\
01:45 เย็บประกอบปกปลายแหลม<br>\
03:43 การวางปกเสื้อ เพื่อเย็บติดกับคอเสื้อ<br>\
05:14 ปกปลายแหลมชิ้นสำเร็จ<br>\
<br>" );
            }
            else if( sku[5] == 'B' || sku[5] == 'C' || sku[5] == 'D' ){
                sprintf( szVideo2, "<br>&nbsp;<br><h3 style='text-align: center;'>2. ปกเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/vl5sL2W-Zco"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
ขั้นตอนการตัดเย็บปกบัว, ปกบัวแหลม, ปกคอจีน<br>\
00:00 Intro<br>\
00:26 เตรียมแพทเทิร์นก่อนเย็บ,วางผ้า,ตัดผ้า<br>\
02:25 ปกเสื้อสำหรับเสื้อผ่าหน้า<br>\
06:16 การวางปกเสื้อ เพื่อเย็บติดกับคอเสื้อแบบผ่าหน้า<br>\
07:36 ปกเสื้อสำหรับเสื้อสวมหัว<br>\
11:11 การวางปกเสื้อ เพื่อเย็บติดกับคอเสื้อแบบสวมหัว<br>\
<br>" );
            }
            else {
                szVideo2[0] = 0;
                SkDebugf( "No Collar Video: %s\n", sku.c_str() );
                exit( 1 );
            }
        }

        // 3 กระเป๋าปะ
        std::string szVideo3 = std::string("<br>&nbsp;<br><h3 style='text-align: center;'>3. กระเป๋าปะ</h3>") + 
                "<p style='color: red;'>(ในแพทเทิร์นมีเส้นตำแหน่งสำหรับกระเป๋าปะให้ แต่แพทเทิร์นเสื้อไม่รวมแพทเทิร์นกระเป๋า" +
                "ซึ่งต้องซื้อแยก 1 ชุดประกอบด้วยแพทเทิร์นกระเป๋าปะ 10 แบบ และฝากระเป๋า 4 แบบ " +
                "ท่านสามารถซื้อแพทเทิร์นกระเป๋าปะได้ที่ลิงค์นี้ค่ะ " + 
                "<a style='color:blue;' href='https://magicpattern.co/product/แพทเทิร์นกระเป๋าปะ-10-แบบ'>แพทเทิร์นกระเป๋าปะ</a>)</p><br>" +
            "<br><h4 style='text-align: center;'>กระเป๋าปะแบบเรียบ</h4>" +
            "<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/6gccal9r_nY"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>" +
            "<br>&nbsp;<br><h4 style='text-align: center;'>กระเป๋าปะแบบจีบ box / จีบคว่ำ</h4>" +
            "<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/pC-M-LqF_Ik"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>";

        // 4 ฝากระเป๋าปะ
        std::string szVideo4 = std::string("<br>&nbsp;<br><h3 style='text-align: center;'>4. ฝากระเป๋าปะ</h3>") + 
            "<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/avC5JCYKSYo"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>";

        // 5 เย็บกระเป๋าและฝาบนชิ้นงาน
        std::string szVideo5 = std::string("<br>&nbsp;<br><h3 style='text-align: center;'>5. เย็บกระเป๋าและฝาบนชิ้นงาน</h3>") + 
            "<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/Tb2vSvFmHQ4"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>";

        // 6. คอเสื้อ
        std::string szv6 = "";
        std::string szv6a = "";
        if( checkSKUVideo2( "WHLKC", "P", "A",       "SN", "A"    ) ) { szv6 = "ZEVoIec2K4s"; szv6a = "คอกลม สวมหัว ไม่มีปก"; };
        if( checkSKUVideo2( "WHLKC", "P", "A",       "SN", "B"    ) ) { szv6 = "izF0a-rRhcE"; szv6a = "คอกลม สวมหัว ปกบัว"; };
        if( checkSKUVideo2( "WHLKC", "P", "A",       "SN", "C"    ) ) { szv6 = "1CUM28dUNms"; szv6a = "คอกลม สวมหัว ปกบัวแหลม"; };
        if( checkSKUVideo2( "WHLKC", "P", "A",       "SN", "D"    ) ) { szv6 = "J3MqRiWYVv4"; szv6a = "คอกลม สวมหัว ปกจีน"; };
        if( checkSKUVideo2( "WHLKC", "P", "A",       "SN", "E"    ) ) { szv6 = "M4J23uOKfww"; szv6a = "คอกลม สวมหัว ปกระบาย"; };
        if( checkSKUVideo2( "WHLKC", "P", "A",       "SN", "F"    ) ) { szv6 = "PVVvF7KiYfM"; szv6a = "คอกลม สวมหัว ปกโบว์"; };

        if( checkSKUVideo2( "WHLKC", "P", "BCDEFG",  "SN", "A"     ) ) { szv6 = "4OlfHnNYjyQ"; szv6a = "คอวี/ปาด/เหลี่ยม/คว้าน สวมหัว ไม่มีปก";};
        if( checkSKUVideo2( "WHLKC", "P", "B",       "SN", "G"     ) ) { szv6 = "QwAg8ZdJj0Q"; szv6a = "คอวี สวมหัว มีปก";};
        if( checkSKUVideo2( "WHLKC", "P", "H",       "SN", "A"     ) ) { szv6 = "KkFge3g82ks"; szv6a = "คอหยดน้ำด้านหน้า";};

        if( checkSKUVideo2( "WHLKC", "B", "ABCDEFG", "SN", "ABCDF" ) ) { szv6 = "WhSBV1q42ho"; szv6a = "คอกลม ผ่าหน้า ทุกปก และไม่มีปก";};
        if( checkSKUVideo2( "WHLKC", "B", "C",       "SN", "A"     ) ) { szv6 = "C8QZsA0p7vA"; szv6a = "คอปาด ผ่าหน้า";};
        if( checkSKUVideo2( "WHLKC", "B", "DF",      "SN", "A"     ) ) { szv6 = "cB8TRqvcJUY"; szv6a = "คอเหลี่ยม ผ่าหน้า";};
        if( checkSKUVideo2( "WHLKC", "B", "EG",      "SN", "A"     ) ) { szv6 = "dTJSHWNbOXc"; szv6a = "คอคว้าน ผ่าหน้า";};
        if( checkSKUVideo2( "WHLKC", "B", "B",       "SN", "AG"    ) ) { szv6 = "NHHjjH_OV98"; szv6a = "คอวี ผ่าหน้า มีปกและไม่มีปก";};

        if( szv6 == "" ) {
            SkDebugf( "No Neck Video: %s\n", sku.c_str() );
            exit(1);
        }
        std::string szVideo6 = std::string("<br>&nbsp;<br><h3 style='text-align: center;'>6. คอเสื้อ</h3>") + 
            "<br><h4 style='text-align: center;'>"+ szv6a + "</h4>" +
            "<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/" + szv6 + """ width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>";

        // 7. ตะเข็บข้าง
        std::string szv7 = "";
        std::string szv7a = "";
        if( checkSKUVideo( "WHL", "P", "ABCDEFGH", "SN"    ) ) { szv7 = "U3MqZQ1PobU"; szv7a = "เสื้อสวมหัว"; };
        if( checkSKUVideo( "WHL", "B", "ABCDEFG",  "SN"    ) ) { szv7 = "qhmFA7VjgRE"; szv7a = "เสื้อผ่าหน้า"; };
        if( checkSKUVideo( "KC",  "P", "ABCDEFGH", "SN"    ) ) { szv7 = "KO9Hr_s3Gt8"; szv7a = "เดรสสวมหัว"; };
        if( checkSKUVideo( "KC",  "B", "ABCDEFG",  "SN"    ) ) { szv7 = "_d-0eSothLQ"; szv7a = "เดรสผ่าหน้า"; };

        if( szv7 == "" ) {
            SkDebugf( "No Side Seam Video: %s\n", sku.c_str() );
            exit(1);
        }
        std::string szVideo7 = std::string("<br>&nbsp;<br><h3 style='text-align: center;'>7. ตะเข็บข้าง</h3>") + 
            "<br><h4 style='text-align: center;'>"+ szv7a + "</h4>" +
            "<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/" + szv7 + """ width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>";

        // 8. ชายระบาย
        static char szVideo8[1024];
        if( sku[7] == 'U'  ) {
            sprintf( szVideo8, "<br>&nbsp;<br><h3 style='text-align: center;'>8. ชายระบาย</h3><p style='text-align: center;'>(ชุดนี้ไม่มีชายระบาย)</p>\\n" );
        }
        else  {
            sprintf( szVideo8, "<br>&nbsp;<br><h3 style='text-align: center;'>8. ชายระบาย</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/8F4x3qcNG_o"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
ขั้นตอนการตัดเย็บชายระบาย<br>\
00:00 Intro<br>\
00:26 เตรียมแพทเทิร์นก่อนเย็บ,วางผ้า,ตัดผ้า<br>\
01:21 ชายระบายสำหรับเสื้อผ่าหน้า<br>\
09:36 ชายระบายสำหรับเสื้อสวมหัว<br>\
<br>" );
        }

        // 9. แขนเสื้อ
        static char szVideo9[512];
        if( sku[6] == 'H'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/Ay7Fq2kZxYE"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'I'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/Sbihs091Kxw"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'J'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/Sbihs091Kxw"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'K'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/Sbihs091Kxw"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'L'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/pGxV4XtdfiE"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'M'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/rRoydvoHBRY"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'N'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/bjg5-f_tpfE"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'O'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/t093KRHlHMA"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'P'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/lStDIEPc1m4"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'Q'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/Yf6rcIJjqZ4"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'R'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/l_AZDvv4XYg"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'S'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/OwYuaSyAfvc"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( sku[6] == 'T'  ) {
            sprintf( szVideo9, "<br>&nbsp;<br><h3 style='text-align: center;'>9. แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/TlbwV9Ku3Mc"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }       

        n = -1;
        if( checkSKUMaterial( "WHL", "P", "A",       "N", "ABCDEF", "H",     "UVW" ) ) n = 1;
        if( checkSKUMaterial( "WHL", "P", "A",       "S", "ABCDEF", "ILMST", "UVW" ) ) n = 2;
        if( checkSKUMaterial( "WHL", "P", "A",       "S", "ABCDEF", "JKNOR", "UVW" ) ) n = 3;
        if( checkSKUMaterial( "WHL", "P", "A",       "S", "ABCDEF", "P",     "UVW" ) ) n = 4;
        if( checkSKUMaterial( "WHL", "P", "A",       "S", "ABCDEF", "Q",     "UVW" ) ) n = 5;
        if( checkSKUMaterial( "WHL", "B", "ABCDEFG", "N", "A",      "H",     "UVW" ) ) n = 6;
        if( checkSKUMaterial( "WHL", "B", "AB",      "N", "BCDEFG", "H",     "UVW" ) ) n = 7;
        if( checkSKUMaterial( "WHL", "B", "AB",      "S", "BCDEFG", "ILMST", "UVW" ) ) n = 8;
        if( checkSKUMaterial( "WHL", "B", "AB",      "S", "BCDEFG", "JKNOR", "UVW" ) ) n = 9;
        if( checkSKUMaterial( "WHL", "B", "AB",      "S", "BCDEFG", "P",     "UVW" ) ) n = 10;
        if( checkSKUMaterial( "WHL", "B", "AB",      "S", "BCDEFG", "Q",     "UVW" ) ) n = 11;
        if( checkSKUMaterial( "WHL", "P", "BCDEFG",  "N", "AG",     "H",     "UVW" ) ) n = 12;
        if( checkSKUMaterial( "WHL", "P", "BCDEFG",  "S", "AG",     "ILMST", "UVW" ) ) n = 13;
        if( checkSKUMaterial( "WHL", "P", "BCDEFG",  "S", "AG",     "JKNOR", "UVW" ) ) n = 14;
        if( checkSKUMaterial( "WHL", "P", "BCDEFG",  "S", "AG",     "P",     "UVW" ) ) n = 15;
        if( checkSKUMaterial( "WHL", "P", "BCDEFG",  "S", "AG",     "Q",     "UVW" ) ) n = 16;
        if( checkSKUMaterial( "WHL", "B", "ABCDEFG", "S", "A",      "ILMST", "UVW" ) ) n = 17;
        if( checkSKUMaterial( "WHL", "B", "ABCDEFG", "S", "A",      "JKNOR", "UVW" ) ) n = 18;
        if( checkSKUMaterial( "WHL", "B", "ABCDEFG", "S", "A",      "P",     "UVW" ) ) n = 19;
        if( checkSKUMaterial( "WHL", "B", "ABCDEFG", "S", "A",      "Q",     "UVW" ) ) n = 20;
        if( checkSKUMaterial( "KC",  "P", "A",       "N", "ABCDEF", "H",     "U"   ) ) n = 21;
        if( checkSKUMaterial( "KC",  "P", "A",       "S", "ABCDEF", "ILMST", "U"   ) ) n = 22;
        if( checkSKUMaterial( "KC",  "P", "A",       "S", "ABCDEF", "JKNOR", "U"   ) ) n = 23;
        if( checkSKUMaterial( "KC",  "P", "A",       "S", "ABCDEF", "P",     "U"   ) ) n = 24;
        if( checkSKUMaterial( "KC",  "P", "A",       "S", "ABCDEF", "Q",     "U"   ) ) n = 25;
        if( checkSKUMaterial( "KC",  "B", "ABCDEFG", "N", "A",      "H",     "U"   ) ) n = 26;
        if( checkSKUMaterial( "KC",  "B", "AB",      "N", "BCDEFG", "H",     "U"   ) ) n = 27;
        if( checkSKUMaterial( "KC",  "B", "AB",      "S", "BCDEFG", "ILMST", "U"   ) ) n = 28;
        if( checkSKUMaterial( "KC",  "B", "AB",      "S", "BCDEFG", "JKNOR", "U"   ) ) n = 29;
        if( checkSKUMaterial( "KC",  "B", "AB",      "S", "BCDEFG", "P",     "U"   ) ) n = 30;
        if( checkSKUMaterial( "KC",  "B", "AB",      "S", "BCDEFG", "Q",     "U"   ) ) n = 31;
        if( checkSKUMaterial( "KC",  "P", "BCDEFG",  "N", "AG",     "H",     "U"   ) ) n = 32;
        if( checkSKUMaterial( "KC",  "P", "BCDEFG",  "S", "AG",     "ILMST", "U"   ) ) n = 33;
        if( checkSKUMaterial( "KC",  "P", "BCDEFG",  "S", "AG",     "JKNOR",  "U"  ) ) n = 34;
        if( checkSKUMaterial( "KC",  "P", "BCDEFG",  "S", "AG",     "P",     "U"   ) ) n = 35;
        if( checkSKUMaterial( "KC",  "P", "BCDEFG",  "S", "AG",     "Q",     "U"   ) ) n = 36;
        if( checkSKUMaterial( "KC",  "B", "ABCDEFG", "S", "A",      "ILMST", "U"   ) ) n = 37;
        if( checkSKUMaterial( "KC",  "B", "ABCDEFG", "S", "A",      "JKNOR", "U"   ) ) n = 38;
        if( checkSKUMaterial( "KC",  "B", "ABCDEFG", "S", "A",      "P",     "U"   ) ) n = 39;
        if( checkSKUMaterial( "KC",  "B", "ABCDEFG", "S", "A",      "Q",     "U"   ) ) n = 40;
        if( checkSKUMaterial( "WHL", "P", "H",       "N", "A",      "H",     "UVW" ) ) n = 41;
        if( checkSKUMaterial( "WHL", "P", "H",       "S", "A",      "ILMST", "UVW" ) ) n = 42;
        if( checkSKUMaterial( "WHL", "P", "H",       "S", "A",      "JKNOR", "UVW" ) ) n = 43;
        if( checkSKUMaterial( "WHL", "P", "H",       "S", "A",      "P",     "UVW" ) ) n = 44;
        if( checkSKUMaterial( "WHL", "P", "H",       "S", "A",      "Q",     "UVW" ) ) n = 45;
        if( checkSKUMaterial( "KC",  "P", "H",       "N", "A",      "H",     "U"   ) ) n = 46;
        if( checkSKUMaterial( "KC",  "P", "H",       "S", "A",      "ILMST", "U"   ) ) n = 47;
        if( checkSKUMaterial( "KC",  "P", "H",       "S", "A",      "JKNOR", "U"   ) ) n = 48;
        if( checkSKUMaterial( "KC",  "P", "H",       "S", "A",      "P",     "U"   ) ) n = 49;
        if( checkSKUMaterial( "KC",  "P", "H",       "S", "A",      "Q",     "U"   ) ) n = 50;

        if( n != -1 ) {
            sprintf( szMaterial, "<br>&nbsp;<br><img class=""aligncenter wp-image-5070"" src=""https://magicpattern.co/resources/_magicpattern/Material/material%02i.png"" alt=""วัสดุที่ใช้"" width=""360"" height=""360"" />\\n", n ); 
       }
        else {
            szMaterial[0] = 0;
            SkDebugf( "No Material: %s\n", sku.c_str() );
            exit( 1 );
        }        
        
        // การวัดตัว
        std::string measurement = "<br>&nbsp;<br><h3 style='text-align: center;'>การวัดตัว</h3><img class=""aligncenter wp-image-5016"" src=""https://magicpattern.co/resources/_magicpattern/Sizes/bodysize01.png"" alt=""การวัดตัว"" width=""640"" height=""292"" />\\n";

        // body size;
        std::string body_size = "<img class=""aligncenter wp-image-5031"" src=""https://magicpattern.co/resources/_magicpattern/Sizes/bodysize.png"" alt=""ตารางวัดตัว"" width=""640"" height=""220"" />\\n";

        // Video Description
        std::string video_desc = std::string("<br>&nbsp;<br><h2 style='text-align: center;'>วิดีโอสอนการตัดเย็บ</h2>") +
            "<h4 style='text-align: center;'>ลำดับการดูวิดีโอเมจิกแพทเทิร์น</h4>" +
            "<p style='text-align: center;'>1.แพทเทิร์นและวิธีตัดผ้าตัวเสื้อ/ชุดเดรส ➡ <br>" +
            "2. ปกเสื้อ ➡ 3. กระเป๋าปะ ➡ 4. ฝากระเป๋าปะ ➡ <br>" +
            "5.เย็บกระเป๋าและฝาบนชิ้นงาน ➡ 6.คอเสื้อ ➡ <br>" +
            "7.ตะเข็บข้าง ➡ 8.ระบายชายเสื้อ ➡ 9.แขนเสื้อ</p><br>";

        // Layout
        n = 0;
        if( checkSKUMaterial( "WHL", "P", "A",       "S",  "A",   "IJKLMNOPQRS",  "UVW" ) ) n = 1;
        if( checkSKUMaterial( "WHL", "P", "A",       "S",  "BCD", "IJKLMNOPQRS",  "UVW" ) ) n = 2;
        if( checkSKUMaterial( "WHL", "P", "A",       "S",  "E",   "IJKLMNOPQRS",  "UVW" ) ) n = 3;
        if( checkSKUMaterial( "WHL", "P", "A",       "S",  "F",   "IJKLMNOPQRS",  "UVW" ) ) n = 4;
        if( checkSKUMaterial( "WHL", "P", "B",       "S",  "G",   "IJKLMNOPQRS",  "UVW" ) ) n = 5;
        if( checkSKUMaterial( "WHL", "P", "A",       "N",  "A",   "H",            "UVW" ) ) n = 6;
        if( checkSKUMaterial( "WHL", "P", "A",       "N",  "BCD", "H",            "UVW" ) ) n = 7;
        if( checkSKUMaterial( "WHL", "P", "A",       "N",  "E",   "H",            "UVW" ) ) n = 8;
        if( checkSKUMaterial( "WHL", "P", "A",       "N",  "F",   "H",            "UVW" ) ) n = 9;
        if( checkSKUMaterial( "WHL", "P", "B",       "N",  "G",   "H",            "UVW" ) ) n = 10;
        if( checkSKUMaterial( "WHL", "B", "ABCDEFG", "NS", "A",   "HIJKLMNOPQRS", "UVW" ) ) n = 11;
        if( checkSKUMaterial( "WHL", "B", "A",       "NS", "BCD", "HIJKLMNOPQRS", "UVW" ) ) n = 12;
        if( checkSKUMaterial( "WHL", "B", "A",       "NS", "F",   "HIJKLMNOPQRS", "UVW" ) ) n = 14;
        if( checkSKUMaterial( "WHL", "B", "B",       "NS", "G",   "HIJKLMNOPQRS", "UVW" ) ) n = 15;
        if( checkSKUMaterial( "WHL", "P", "A",       "S",  "A",   "T",            "UVW" ) ) n = 28;
        if( checkSKUMaterial( "WHL", "P", "A",       "S",  "BCD", "T",            "UVW" ) ) n = 29;
        if( checkSKUMaterial( "WHL", "P", "A",       "S",  "E",   "T",            "UVW" ) ) n = 30;
        if( checkSKUMaterial( "WHL", "P", "A",       "S",  "F",   "T",            "UVW" ) ) n = 31;
        if( checkSKUMaterial( "WHL", "P", "B",       "S",  "G",   "T",            "UVW" ) ) n = 32;
        if( checkSKUMaterial( "WHL", "B", "ABCDEFG", "S",  "A",   "T",            "UVW" ) ) n = 33;
        if( checkSKUMaterial( "WHL", "B", "A",       "S",  "BCD", "T",            "UVW" ) ) n = 34;
        if( checkSKUMaterial( "WHL", "B", "A",       "S",  "F",   "T",            "UVW" ) ) n = 36;
        if( checkSKUMaterial( "WHL", "B", "B",       "S",  "G",   "T",            "UVW" ) ) n = 37;
        if( checkSKUMaterial( "WHL", "P", "BCDEFG",  "S",  "A",   "IJKLMNOPQRS",  "UVW" ) ) n = 38;
        if( checkSKUMaterial( "WHL", "P", "BCDEFG",  "N",  "A",   "H",            "UVW" ) ) n = 39;
        if( checkSKUMaterial( "WHL", "P", "BCDEFG",  "S",  "A",   "T",            "UVW" ) ) n = 40;
        if( checkSKUMaterial( "KC",  "P", "A",       "S",  "A",   "IJKLMNOPQRS",  "U"   ) ) n = 41;
        if( checkSKUMaterial( "KC",  "P", "A",       "S",  "BCD", "IJKLMNOPQRS",  "U"   ) ) n = 42;
        if( checkSKUMaterial( "KC",  "P", "A",       "S",  "E",   "IJKLMNOPQRS",  "U"   ) ) n = 43;
        if( checkSKUMaterial( "KC",  "P", "A",       "S",  "F",   "IJKLMNOPQRS",  "U"   ) ) n = 44;
        if( checkSKUMaterial( "KC",  "P", "B",       "S",  "G",   "IJKLMNOPQRS",  "U"   ) ) n = 45;
        if( checkSKUMaterial( "KC",  "P", "A",       "N",  "A",   "H",            "U"   ) ) n = 46;
        if( checkSKUMaterial( "KC",  "P", "A",       "N",  "BCD", "H",            "U"   ) ) n = 47;
        if( checkSKUMaterial( "KC",  "P", "A",       "N",  "E",   "H",            "U"   ) ) n = 48;
        if( checkSKUMaterial( "KC",  "P", "A",       "N",  "F",   "H",            "U"   ) ) n = 49;
        if( checkSKUMaterial( "KC",  "P", "B",       "N",  "G",   "H",            "U"   ) ) n = 50;
        if( checkSKUMaterial( "KC",  "B", "ABCDEFG", "NS", "A",   "HIJKLMNOPQRS", "U"   ) ) n = 51;
        if( checkSKUMaterial( "KC",  "B", "A",       "NS", "BCD", "HIJKLMNOPQRS", "U"   ) ) n = 52;
        if( checkSKUMaterial( "KC",  "B", "A",       "NS", "F",   "HIJKLMNOPQRS", "U"   ) ) n = 53;
        if( checkSKUMaterial( "KC",  "B", "B",       "NS", "G",   "HIJKLMNOPQRS", "U"   ) ) n = 54;
        if( checkSKUMaterial( "KC",  "P", "A",       "S",  "A",   "T",            "U"   ) ) n = 55;
        if( checkSKUMaterial( "KC",  "P", "A",       "S",  "BCD", "T",            "U"   ) ) n = 56;
        if( checkSKUMaterial( "KC",  "P", "A",       "S",  "E",   "T",            "U"   ) ) n = 57;
        if( checkSKUMaterial( "KC",  "P", "A",       "S",  "F",   "T",            "U"   ) ) n = 58;
        if( checkSKUMaterial( "KC",  "P", "B",       "S",  "G",   "T",            "U"   ) ) n = 59;
        if( checkSKUMaterial( "KC",  "B", "ABCDEFG", "S",  "A",   "T",            "U"   ) ) n = 60;
        if( checkSKUMaterial( "KC",  "B", "A",       "S",  "BCD", "T",            "U"   ) ) n = 61;
        if( checkSKUMaterial( "KC",  "B", "A",       "S",  "F",   "T",            "U"   ) ) n = 62;
        if( checkSKUMaterial( "KC",  "B", "B",       "S",  "G",   "T",            "U"   ) ) n = 63;
        if( checkSKUMaterial( "KC",  "P", "BCDEFG",  "S",  "A",   "IJKLMNOPQRS",  "U"   ) ) n = 64;
        if( checkSKUMaterial( "KC",  "P", "BCDEFG",  "N",  "A",   "H",            "U"   ) ) n = 65;
        if( checkSKUMaterial( "KC",  "P", "BCDEFG",  "S",  "A",   "T",            "U"   ) ) n = 66;
        if( checkSKUMaterial( "WHL", "P", "H",       "S",  "A",   "IJKLMNOPQRS",  "UVW" ) ) n = 67;
        if( checkSKUMaterial( "WHL", "P", "H",       "N",  "A",   "H",            "UVW" ) ) n = 68;
        if( checkSKUMaterial( "WHL", "P", "H",       "S",  "A",   "T",            "UVW" ) ) n = 69;
        if( checkSKUMaterial( "KC",  "P", "H",       "S",  "A",   "IJKLMNOPQRS",  "UVW" ) ) n = 70;
        if( checkSKUMaterial( "KC",  "P", "H",       "N",  "A",   "H",            "U"   ) ) n = 71;
        if( checkSKUMaterial( "KC",  "P", "H",       "S",  "A",   "T",            "U"   ) ) n = 72;
        if( n == 0 ) {
            SkDebugf( "No Layout: %s\n", sku.c_str() );
            exit( 1 );
        }

        // Long Description
        static char szLong[24096]; 
        sprintf( szLong, "%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n<br>&nbsp;<br><br>&nbsp;<br>", 
            desc.c_str(), 
            strYouGot.c_str(),
            measurement.c_str(),
            body_size.c_str(),
            szBodyLength, szSleeveLength, szMaterial,
            video_desc.c_str(),
            szVideo1, szVideo2, szVideo3.c_str(), szVideo4.c_str(), szVideo5.c_str(), 
            szVideo6.c_str(), szVideo7.c_str(), szVideo8, szVideo9, szPNG
        );   

        // Product Images
        char product_images[1024];
        sprintf( product_images, "https://magicpattern.co/wp-content/uploads/2022/08/%04i-0.png", nCurrentPatternID );
        //, https://magicpattern.co/wp-content/uploads/2022/08/%04i-1.png, https://magicpattern.co/wp-content/uploads/2022/08/%04i-2.png",
        // nCurrentPatternID, nCurrentPatternID, nCurrentPatternID );

        // Download
        char downloads[512];
        sprintf( downloads, "MagicPattern-%04i,MagicPattern-%04i.zip,https://magicpattern.co/F1n=411e/%04i.zip", 
            nCurrentPatternID, nCurrentPatternID, nCurrentPatternID );

        // WooCommerce Product List
        
        sprintf( szx, ",\"simple, downloadable\",%s,Magic Pattern #%04i,1,0,visible,\"%s\",\"%s\",,,taxable,,1,,,0,0,,,,,0,,,400,Magic Pattern,MagicPattern,,\"%s\",-1,-1,,,,,,,0,%s,,,\n",
            sku.c_str(), nCurrentPatternID, szShort, szLong, product_images, downloads
        );
            
        // 9/9
        /*
        sprintf( szx, ",\"simple, downloadable\",%s,Magic Pattern #%04i,1,0,visible,\"%s\",\"%s\",,,taxable,,1,,,0,0,,,,,0,,199,400,Magic Pattern (D),MagicPattern,,\"%s\",-1,-1,,,,,,,0,%s,,,\n",
            sku.c_str(), nCurrentPatternID, szShort, szLong, product_images, downloads
        );
            */          

        if( f ) {
            f.write( (const char *)szx, strlen(szx) );
        }
    }
    if( f ) {
        f.close();
    }
    /*
    while( true ) {
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

        static char szMaterial[512];
        sprintf( szMaterial, "<img class=""aligncenter wp-image-5070"" src=""https://magicpattern.co/wp-content/uploads/2022/07/material%02i.png"" alt=""วัสดุที่ใช้"" width=""360"" height=""360"" />\\n", n ); 

        static char szVideo1[2048];

                            



        static char szVideo4[512];
        if( _sku[6] == 'H'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/Ay7Fq2kZxYE"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'I'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/Sbihs091Kxw"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'J'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/Sbihs091Kxw"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'K'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/Sbihs091Kxw"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'L'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/pGxV4XtdfiE"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'M'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/rRoydvoHBRY"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'N'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/bjg5-f_tpfE"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'O'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/t093KRHlHMA"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'P'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/lStDIEPc1m4"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'Q'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/Yf6rcIJjqZ4"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'R'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/l_AZDvv4XYg"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'S'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/OwYuaSyAfvc"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        }
        else if( _sku[6] == 'T'  ) {
            sprintf( szVideo4, "<br><h3 style='text-align: center;'>แขนเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/TlbwV9Ku3Mc"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div>\\n" );
        } 
    } 
    if( f ) {
        f.close();
    }
    SkDebugf( "PNG count: %i\n", png_count );
    */
}

void DecMagicPattern::exportPNGs() {
    setJobFrameSkip( 1 );
    __jobIndex = 1;

    pushJob( new DecJob([=](DecWidget &w) -> bool {
        return __jobExportPNG( w );
    }) );
}

bool DecMagicPattern::__jobExportPNG(DecWidget &widget) {
    gotoPattern( __jobIndex );
    exportPNG();

    __jobIndex += 1;
    if( __jobIndex > 3718) {
        return true;
    }
    return false;
}

void DecMagicPattern::exportPNG() {
    char sz[256];
    sprintf( sz, "_export_png2/%04i-0.png", nCurrentPatternID );
    randomStyle( nCurrentPatternID, false );
    pSVG->exportPNG( nCurrentPatternID, sz, 150, 1.0 );

    /*
    sprintf( sz, "_export_png2/%04i-1.png", nCurrentPatternID );
    randomStyle( nCurrentPatternID );
    pSVG->exportPNG( nCurrentPatternID, sz, 150, 1.0 );

    randomStyle( nCurrentPatternID );
    sprintf( sz, "_export_png2/%04i-2.png", nCurrentPatternID );
    pSVG->exportPNG( nCurrentPatternID, sz, 150, 1.9 );
    */
}

void DecMagicPattern::exportSVG() {
    char sz[256];
    sprintf( sz, "_export_png2/%04i.svg", nCurrentPatternID );
    pSVG->exportSVG( sz );
}

void DecMagicPattern::gotoPattern( int _pid ) {
    randomStyle(_pid);

    SkDebugf( "Product ID: %04i\n", nCurrentPatternID );
    SkDebugf( "SKU: %s\n", sku.c_str() );
    //SkDebugf( "Description: %s\n", getDescription().c_str() );    
}

void DecMagicPattern::randomPocket() {
    int poc = rand() % _numRandomPockets;
    if( nLength == 1 ) {
        poc = rand() % (_numRandomPockets - 6);
    }

    int ptype = (rand() % (NUM_POCKET_TYPE-1)) + 1;
    int pctype = (rand() % (NUM_POCKETCOVER_TYPE));

    //SkDebugf( "Random: %i %i\n", pid, num_pid2sku );
    setPocket( 0, 0, PocketRandom[poc][0][0] * ptype );
    setPocket( 0, 1, PocketRandom[poc][0][1] * ptype );
    setPocket( 1, 0, PocketRandom[poc][1][0] * ptype );
    setPocket( 1, 1, PocketRandom[poc][1][1] * ptype );  

    setPocketCover( 0, 0, PocketRandom[poc][0][0] * pctype );
    setPocketCover( 0, 1, PocketRandom[poc][0][1] * pctype );
    setPocketCover( 1, 0, PocketRandom[poc][1][0] * pctype );
    setPocketCover( 1, 1, PocketRandom[poc][1][1] * pctype );  
}

void DecMagicPattern::randomPattern() {
    int c = rand() % 2;

    if( c == 0 ) {
        int _pid = rand() % 3042;
        randomStyle(_pid+1, bRandomPocket);
    }
    else {
        int _pid = rand() % (3718-3042);
        randomStyle(_pid+3042+1, bRandomPocket);
    }
    SkDebugf( "Product ID: %04i\n", nCurrentPatternID );
    SkDebugf( "SKU: %s\n", sku.c_str() );
    SkDebugf( "Description: %s\n", getDescription().c_str() );       
}

void DecMagicPattern::randomStyle( int _pid, bool bPocket ) {
    /*
    //int col = rand() % _numRandomColors;

    //currentColor = _randomColors[col][0];
    //currentColorCollar = _randomColors[col][1];
    //currentColorSleeve = _randomColors[col][2];
    //currentColorHem = _randomColors[col][3];
    //currentColorPocket0 = _randomColors[col][4];
    //currentColorPocket1 = _randomColors[col][4];
    //currentColorShoe = _randomColors[col][0];
    */

    int col = rand() % _numColorThemes; 

    int mainCol = rand() % 3;
    currentColor = _colorThemes[col][mainCol];  
    currentColorCollar = _colorThemes[col][rand() % 3];
    currentColorSleeve = _colorThemes[col][rand() % 3];
    currentColorHem = _colorThemes[col][rand() % 3];
    currentColorPocket0 = _colorThemes[col][rand() % 3];
    currentColorPocket1 = _colorThemes[col][rand() % 3];
    currentColorShoe = _colorThemes[col][mainCol];    

    loadPattern( _pid );

    int bg = (rand() % (NUM_BG-1-4))+4;
    setBackground( bg+1 );

    int hair = rand() % (NUM_HAIR-1);
    setHair( hair + 1 );

    int poc = rand() % _numRandomPockets;
    if( nLength == 1 ) {
        poc = rand() % (_numRandomPockets - 6);
    }

    //SkDebugf( "Random: %i %i %i\n", nLength, poc, _numRandomPockets );
    int ptype = (rand() % (NUM_POCKET_TYPE-1)) + 1;
    int pctype = (rand() % (NUM_POCKETCOVER_TYPE));

    if( !bPocket ) {
        ptype = 0;
        pctype = 0;
    }

    //SkDebugf( "Random: %i %i\n", pid, num_pid2sku );
    setPocket( 0, 0, PocketRandom[poc][0][0] * ptype );
    setPocket( 0, 1, PocketRandom[poc][0][1] * ptype );
    setPocket( 1, 0, PocketRandom[poc][1][0] * ptype );
    setPocket( 1, 1, PocketRandom[poc][1][1] * ptype );  

    setPocketCover( 0, 0, PocketRandom[poc][0][0] * pctype );
    setPocketCover( 0, 1, PocketRandom[poc][0][1] * pctype );
    setPocketCover( 1, 0, PocketRandom[poc][1][0] * pctype );
    setPocketCover( 1, 1, PocketRandom[poc][1][1] * pctype );  

    SkDebugf( "SVG Style OK\n" );  

    //exportPNG();
}

void DecMagicPattern::loadPattern( int pid ) {
    nCurrentPatternID = pid;

    char sz[64];
    sprintf( sz, "รหัส: %04i", nCurrentPatternID );
    pTextPID->text( sz );

    sku = PID2SKU[nCurrentPatternID-1];
    sprintf( sz, "SKU: %s", sku.c_str() );
    pTextSKU->text( sz );

    const char *_str_length = "WHLKC";
    const char *_str_type = "PB";
    const char *_str_neck = "ABCDEFGH";
    const char *_str_collar = "ABCDEFG";
    const char *_str_sleeve = "HIJKLMNOPQRST";
    const char *_str_hem = "UVW";        

    int _generateLength = strchr( _str_length, sku[0] ) - _str_length;    
    int _generateType = strchr( _str_type, sku[1] ) - _str_type;
    int _generateNeck = strchr( _str_neck, sku[2] ) - _str_neck;

    int _generateCollar = strchr( _str_collar, sku[5] ) - _str_collar;
    int _generateSleeve = strchr( _str_sleeve, sku[6] ) - _str_sleeve;
    int _generateHem = strchr( _str_hem, sku[7] ) - _str_hem;


    if( _generateLength <= 2 ) {
        //pSVG->fScale = fSVGScaleMax;
        //pSVG->fScale = fSVGScaleFit;
    }
    else {
        //pSVG->fScale = fSVGScaleFit;
    }
    setLength( _generateLength + 1 );
    setType( _generateType + 1 );
    setNeck( _generateNeck );
    setCollar( _generateCollar );
    setSleeve( _generateSleeve + 1 );
    setHem( _generateHem );
}

void DecMagicPattern::setColor( uint32_t color ) {
    int n = pMPPanel->nMode;
    if( n == 0 || n == 1 || n == 2 ) {
        currentColor = color;  
        currentColorShoe = color;
    }
    else if( n == 3 || n == 6 ) {
        currentColorCollar = color;
    }
    else if( n == 4 ) {
        currentColorSleeve = color;
    }
    else if( n == 5 ) {
        currentColorHem = color;
    }
    else if( n == 7 ) {
        currentColorPocket0 = color;
        currentColorPocket1 = color;
    }
    else {
        currentColor = color;  
        currentColorShoe = color;
    }
   
    loadPattern( nCurrentPatternID );
}

void DecMagicPattern::setNodeVisibility( SkSVGNode* node, bool bVisible ) {
    //SkDebugf( "SetNodeVisibility\n )" );
    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kHidden;
    if( bVisible ) {
        vType = SkSVGVisibility::Type::kVisible;
    }

    auto visible = SkSVGVisibility(vType);
    SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
    node->setVisibility(visibilityVisible);
}

void DecMagicPattern::setBackground( int n ) {
    nBackground = n;
    for( int i=0; i<NUM_BG; i++ ) {
        if( _vBGNode[i] != NULL ) {
            if( i == n ) {
                setNodeVisibility( _vBGNode[i], true );
            }
            else {
                setNodeVisibility( _vBGNode[i], false );
            }
        }
    }
    if( pSVG ) {
        pSVG->invalidate();
        pSVG->dirty( true, false );
    }
}

void DecMagicPattern::setHand( int n ) {
    for( int i=0; i<NUM_HAND; i++ ) {
        if( _vHandNode[i] != NULL ) {
            if( i == n ) {
                setNodeVisibility( _vHandNode[i], true );
            }
            else {
                setNodeVisibility( _vHandNode[i], false );
            }
        }
    }
}

void DecMagicPattern::setPant( int n ) {
    nPant = n;
    for( int i=0; i<NUM_PANT; i++ ) {
        if( _vPantNode[i] != NULL ) {
            if( i == n ) {
                setNodeVisibility( _vPantNode[i], true );
            }
            else {
                setNodeVisibility( _vPantNode[i], false );
            }
        }
    }
    if( pSVG ) {
        pSVG->invalidate();
        pSVG->dirty( true, false );
    }
}

void DecMagicPattern::setHair( int n ) {
    nHair = n;
    for( int i=0; i<NUM_HAIR; i++ ) {
        if( _vHairNode[i] != NULL ) {
            if( i == n ) {
                setNodeVisibility( _vHairNode[i], true );
            }
            else {
                setNodeVisibility( _vHairNode[i], false );
            }
        }
    }
    if( pSVG ) {
        pSVG->invalidate();
        pSVG->dirty( true, false );
    }    
}

void DecMagicPattern::setSleeve( int n ) {
    nSleeve = n;
    for( int i=0; i<NUM_SLEEVE; i++ ) {
        if( _vSleeveNode[i] != NULL ) {
            if( i == n ) {
                setNodeVisibility( _vSleeveNode[i], true );
                setNodeColor( _vSleeveNode[i], currentColorSleeve );
            }
            else {
                setNodeVisibility( _vSleeveNode[i], false );
            }
        }
    }
    //SkDebugf( "Sleeve: %i\n", nSleeve );
    updateProductID();       
    if( pSVG ) {
        pSVG->invalidate();
        pSVG->dirty( true, false );
    }
}

void DecMagicPattern::setType( int n ) {
    nType = n;
    //SkDebugf( "SetType: %i %i %i\n", nType, nCollar, nLastNeck );
    if( nType == 2 ) { // ผ่าหน้า
        if( nNeck == 7 ) { // คอหยดน้ำมีโบว์
            nLastNeck = 7;
            nNeck = 0; // คอกลม
        }
        if( nCollar == 4 ) { // ปกคอระบาย
            nLastCollar = 4;
            setCollar( 0 );
        }
        if( pMPPanel ) {
            pMPPanel->ShowNeck7( false );
            pMPPanel->ShowCollar4( false );
        }
    }
    else {
        if( nLastNeck != -1 ) {
            nNeck = nLastNeck;
        }
        if( nLastCollar != -1 ) {
            setCollar( nLastCollar );
        }
        if( pMPPanel ) {
            pMPPanel->ShowNeck7( true );
            pMPPanel->ShowCollar4( true );
        }
    }
    //SkDebugf( "SetType OK\n" );
    setHem( nHem );
    showBody();
    //SkDebugf( "Type: %i\n", nType );
    updateProductID();    
}

void DecMagicPattern::setNodeColor( SkSVGNode *node, SkColor color ) {
    //SkDebugf( "SetNodeColor\n" );
    SkSVGTag tag = node->tag();
    const SkSVGProperty<SkSVGPaint, true> &svg_fill = node->getFill();
    const SkSVGProperty<SkSVGPaint, true> &svg_stroke = node->getStroke();

    /*
    if( svg_stroke.isValue() ) {
        if( svg_stroke->type() == SkSVGPaint::Type::kColor ) {
            SkSVGColor col = svg_stroke->color();

            // convert colors to all black
            auto sstroke = SkSVGPaint( SkSVGColor( SK_ColorBLACK));
            SkSVGProperty<SkSVGPaint, true> sstrokestroke( sstroke );
            node->setStroke( sstrokestroke );
        }
    }
    */
    if( svg_fill.isValue() ) {
        if( svg_fill->type() == SkSVGPaint::Type::kColor ) {
            //SkSVGColor col = svg_fill->color();

            //SkDebugf( "Color: %X\n", col.color() );
            auto sfill1 = SkSVGPaint( SkSVGColor( 0xF8000000 | (0xFFFFFF & color) ));
            SkSVGProperty<SkSVGPaint, true> sfillfill1( sfill1 );
            auto sfill2 = SkSVGPaint( SkSVGColor( 0x20000000 ));
            SkSVGProperty<SkSVGPaint, true> sfillfill2( sfill2 );
            auto sfill3 = SkSVGPaint( SkSVGColor( 0x30000000 ));
            SkSVGProperty<SkSVGPaint, true> sfillfill3( sfill3 );
            auto sfill4 = SkSVGPaint( SkSVGColor( 0xFFFBDDCF ));
            SkSVGProperty<SkSVGPaint, true> sfillfill4( sfill4 );
            auto sfill99 = SkSVGPaint( SkSVGColor( 0xFFFF0000 ));
            SkSVGProperty<SkSVGPaint, true> sfillfill99( sfill99 );

            if( node->_customTagID == 1 ) {
                node->setFill( sfillfill1 ); 
            }
            else if( node->_customTagID == 2 ) {
                node->setFill( sfillfill2 ); 
            }
            else if( node->_customTagID == 3 ) {
                node->setFill( sfillfill3 ); 
            }
            else if( node->_customTagID == 4 ) {
                // เนื้อ
                node->setFill( sfillfill4 ); 
            }
            else if( node->_customTagID == 5 ) {
                // ตัดเส้นเนื้อ
            }
            else {
                //node->setFill( sfillfill99 ); 
            }

            //auto scolortype = SkSVGColorType(color);
            //SkSVGProperty<SkSVGColorType, true> scolor( scolortype );
            //node->setColor( scolor );
        }
    }    
    if( node->hasChildren() ) {
        if( tag == SkSVGTag::kG ) {
            SkSVGG *pG = (SkSVGG *)node;
            int numChildren = (int)pG->fChildren.size();
            for( int i=0; i<numChildren; i++ ) {
                sk_sp<SkSVGNode> child_node = pG->fChildren.at( i );

                //rootItem._( TreeItem( sz ) );
                setNodeColor( child_node.get(), color );
            }
        }
    }    
}

void DecMagicPattern::setLength( int n ) {
    nLength = n;
    if( nLength > 3 ) {
        if( nHem > 0 ) {
            setHem( 0 );
        }
    }
    int p[2] = { 0, 0 };
    int pc[2] = { 0, 0 };
    for( int i=1; i<5; i++ ) {
        if( nPocket[i][0] ) {
            p[0] = nPocket[i][0];
            nPocket[i][0] = 0;
        }
        if( nPocket[i][1] ) {
            p[1] = nPocket[i][1];
            nPocket[i][1] = 0;
        }
        if( nPocketCover[i][0] ) {
            pc[0] = nPocketCover[i][0];
            nPocketCover[i][0] = 0;
        }
        if( nPocketCover[i][1] ) {
            pc[1] = nPocketCover[i][1];
            nPocketCover[i][1] = 0;
        }
    }    
    //SkDebugf( "LengthPocket: %i %i\n", p[0], p[1] );
    setPocket( 1, 0, p[0] );
    setPocket( 1, 1, p[1] );
    setPocketCover( 1, 0, pc[0] );
    setPocketCover( 1, 1, pc[1] );

    for( int i=0; i<NUM_LENGTH; i++ ) {
        if( _vLengthNode[i] != NULL ) {
            if( i == n ) {
                setNodeVisibility( _vLengthNode[i], true );
                setNodeColor( _vLengthNode[i], currentColor );
            }
            else {
                setNodeVisibility( _vLengthNode[i], false );
            }
        }
    }
    if( pButtonHem ) {
        if( nLength > 3 ) {
            pButtonHem->hide();
        }
        else {
            pButtonHem->show();
        }
        if( pButtonPanel ) {
            pButtonPanel->invalidate();
            pButtonPanel->dirty( true, true );
        }
    }    
    if( nLength <= 3 ) {
        setPant( 1 );
        setHand( 1 );
        setNodeColor( _vHandNode[1], currentColor );
    }
    else {
        setPant( 0 );
        setHand( 2 );
        setNodeColor( _vHandNode[2], currentColor );
    }
    setHem( nHem );
    showBody();
    //SkDebugf( "Length: %i\n", nLength );
    updateProductID();
}

void DecMagicPattern::updateProductID() {
    static const char *lengthSKU = " WHLKC";
    static const char *typeSKU = " PB";
    static const char *neckSKU = "ABCDEFGH";
    static const char *collarSKU = "ABCDEFG";
    static const char *hasSleeveSKU = " NSSSSSSSSSSSS";
    static const char *sleeveSKU = " HIJKLMNOPQRST";
    static const char *hemSKU = "UVW";

    std::string newSKU = "";
    newSKU += lengthSKU[nLength];
    newSKU += typeSKU[nType];
    newSKU += neckSKU[nNeck];
    newSKU += hasSleeveSKU[nSleeve];
    newSKU += "-";
    newSKU += collarSKU[nCollar];
    newSKU += sleeveSKU[nSleeve];
    newSKU += hemSKU[nHem];
    //SkDebugf("New SKU: %s\n", newSKU.c_str() );

    for( int i=0; i<num_pid2sku; i++ ) {
        if( strcmp( newSKU.c_str(), PID2SKU[i] ) == 0 ) {
            nCurrentPatternID = i+1;
            sku = newSKU;
            char sz[64];
            sprintf( sz, "รหัส: %04i", nCurrentPatternID );
            pTextPID->text( sz );
            sprintf( sz, "SKU: %s", sku.c_str() );
            pTextSKU->text( sz );            
            break;
        }
    }
}

void DecMagicPattern::setCollar( int n ) {
    nCollar = n;
    for( int i=0; i<NUM_COLLAR; i++ ) {
        if( _vCollarNode[i] != NULL ) {
            if( i == n ) {
                setNodeVisibility( _vCollarNode[i], true );
                setNodeColor( _vCollarNode[i], currentColorCollar );
            }
            else {
                setNodeVisibility( _vCollarNode[i], false );
            }
        }
    }
    //SkDebugf( "Collar: %i\n", nCollar );
    updateProductID();     
    if( pSVG ) {
        pSVG->invalidate();
        pSVG->dirty( true, false );
    }
}

void DecMagicPattern::setNeck( int n ) {
    nNeck = n;
    if( nNeck == 0 ) { // คอกลม
        if( nLastCollar >= 1 && nLastCollar <= 5 ) {
            setCollar( nLastCollar );
        }
        else {
            if( nCollar > 5 ) {
                nLastCollar = nCollar;
                setCollar( 0 );
            }
        }
    }
    else if( nNeck == 1 ) {
        if( nLastCollar == 6 ) {
            setCollar( nLastCollar );
        }
        else {
            if( nCollar > 0 && nCollar != 6 ) {
                nLastCollar = nCollar;
                setCollar( 0 );
            }
        }
    }
    else {
        if( nCollar > 0 ) {
            nLastCollar = nCollar;
            setCollar( 0 );
        }
    }
    if( nNeck == 7 ) { // คอหยดน้ำมีโบว์
        if( nType == 2 ) { // ผ่าหน้า
            nLastType = 2;
            nType = 1; // สวมศีรษะ
        }
    }
    else {
        if( nLastType != -1 ) {
            nType = nLastType;
        }
    }
    if( pButtonCollar ) {
        if( nNeck >= 2 ) {
            pButtonCollar->hide();
        }
        else {
            pButtonCollar->show();
        }
        if( pButtonPanel ) {
            pButtonPanel->invalidate();
            pButtonPanel->dirty( true, true );
        }
    }
    showBody();
    //SkDebugf( "Neck: %i\n", nNeck );
    updateProductID();    
}

void DecMagicPattern::showBody() {
    //SkDebugf( "ShowBody %i %i %i\n", nType, nLength, nNeck );
    int nTypeLength = nType;
    if( nType == 2 ) {
        nTypeLength += (nLength - 1);
    }
    for( int i=0; i<NUM_TYPE; i++ ) {
        for( int j=0; j<NUM_NECK; j++ ) {
            if( _vNeckNode[i][j] != NULL ) {
                if( i == nTypeLength && j == nNeck ) {
                    //SkDebugf( "Show Body %i %i\n", i, j );
                    setNodeVisibility( _vNeckNode[i][j], true );
                    setNodeColor( _vNeckNode[i][j], currentColor );
                    setNodeColor( _vShoeNode[1], currentColor );

                }
                else {
                    //SkDebugf( "Hide Body %i %i\n", i, j );
                    setNodeVisibility( _vNeckNode[i][j], false );
                }                
            }
            else {
                //SkDebugf( "Invalid Body %i %i\n", i, j );
            }
        }
    }
    if( pSVG ) {
        pSVG->invalidate();
        pSVG->dirty( true, false );
    }
}

void DecMagicPattern::setHem( int n ) {
    if( nLength > 3 )
        nHem = 0;
    else
        nHem = n;

    for( int i=0; i<NUM_HEM_BODY_LENGTH; i++ ) {
        for( int j=0; j<NUM_HEM_BODY_TYPE; j++ ) {
            for( int k=0; k<NUM_HEM_LENGTH; k++ ) {
                if( _vHemNode[i][j][k] != NULL ) {
                    if( i == nLength-1 && j == nType-1 && k == nHem ) {
                        setNodeVisibility( _vHemNode[i][j][k], true );
                        setNodeColor( _vHemNode[i][j][k], currentColorHem );
                    }
                    else {
                        setNodeVisibility( _vHemNode[i][j][k], false );
                    }
                }
            }
        }
    }    
    //SkDebugf( "Hem: %i\n", nHem );
    updateProductID();   
    if( pSVG ) {
        pSVG->invalidate();
        pSVG->dirty( true, false );
    }    
}

void DecMagicPattern::init() {
#ifndef __EMSCRIPTEN__  
    pComboDevices->select( 9 );
#endif

    int h = height();
    SkDebugf( "init - Height: %i\n", h );


}

void DecMagicPattern::onResized(int w, int h) {
    DecWidget::onResized( w, h );

#ifndef __EMSCRIPTEN__ 
    if( pControllerContainer ) {

        if( w > 1100 ) {
            pControllerContainer->visible( true );
        }
        else {
            pControllerContainer->visible( false );
        }
    }
#endif
    if( pSVG ) {
        pSVG->invalidate();
        pSVG->dirty( true, true );
    }
}

void DecMagicPattern::onLoad(DecSVGDom &svg) {
    SkDebugf( "OnLoad\n" );
    fDom = svg.fDom;
    if( fDom ) {
        root = fDom->getRoot();
        
        SkSVGTag rtag = root->tag();
        SkDebugf( "Root: %s\n", svgTagName[(int)rtag] );   

        if( root->hasChildren() ) {
            int numChildren = (int)root->fChildren.size();

            for( int i=0; i<numChildren; i++ ) {
                sk_sp<SkSVGNode> node = root->fChildren.at( i );
                scanSubNode( node.get(), 1, "root" );
            }
            int n = 0;
            for( int i=0; i<15; i++ ) {
                //SkDebugf( "SVG Layer %i: %i\n", i, svgLayerCount[i] );
                n += svgLayerCount[i];
            }
            //SkDebugf( "SVG total layers: %i\n", n );
        }        
    }
    SkDebugf( "Loaded and Scanned\n" );

    //pSVG->fScale = fSVGScale;

    //setBackground( 1 );
    //setHair( 2 );
    //setLength( 1 );
    //setType( 1 );
    //setNeck( 0 );
    //setSleeve( 2 );
    //setCollar( 0 );
    //setHem( 0 );

    setNodeColor( _vBodyNode[1], 0xFFFBDDCF );
    setNodeColor( _vHandNode[1], 0xFFFBDDCF );
    setNodeColor( _vHairNode[1], 0xFFFBDDCF );

    //SkSize sz = fDom->containerSize();
    //int h = height();
    //SkDebugf( "onLoad - Height: %i\n", h );
    SkDebugf( "OnLoad done\n" );
    srand (time(NULL));
    randomPattern();

    //SkDebugf( "SVG %i %i\n", pSVG->width(), pSVG->height() );

    SkDebugf( "UI Invalidate\n" );
    svgReady = true;
    UI()->invalidate();
}

void DecMagicPattern::scanSubNode(SkSVGNode* node, int deep, std::string parentNodeName ) {
    svgLayerCount[deep]++;

    SkSVGTag tag = node->tag();

    if( tag == SkSVGTag::kTextLiteral ) {
        return;
    }    

    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;

    const SkSVGProperty<SkSVGVisibility, true> &visibility = node->getVisibility();
    const SkSVGProperty<SkSVGStringType, false> &propData = node->getDataName();
    const SkSVGStringType* data = propData.getMaybeNull();
    const char* cpchData = data ? data->c_str() : "";
    std::string nodeName = std::string( cpchData );

    if( visibility.isValue() ) {
        vType = visibility->type();
    } else {
        if (tag == SkSVGTag::kG) {
            auto visible = SkSVGVisibility(SkSVGVisibility::Type::kVisible);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);
        }
    }    

    char chDeep[] = "--------------------------------------------------";
    chDeep[deep] = 0;   
    if( deep <= 3 ) {
        /*
        SkDebugf("%i: %s %s %s \n",
                    deep,
                    parentNodeName.c_str(),
                    svgTagName[(int)tag], cpchData );  
        */  
    }  

    if( deep == 2 ) {
        currentSVGTag = 0;
        if( nodeName == "หุ่น" ) {
            currentSVGTag = 3;
            vType = SkSVGVisibility::Type::kVisible;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            _vBodyNode[1] = node;
        }
        else if( nodeName == "มือ" ) {
            currentSVGTag = 3;
            vType = SkSVGVisibility::Type::kVisible;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            _vHandNode[1] = node;
        }     
        else if( nodeName == "มือในกระเป๋า" ) {
            currentSVGTag = 11;
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            _vHandNode[2] = node;
        }          
        else if( nodeName == "รองเท้า" ) {
            currentSVGTag = 9;
            vType = SkSVGVisibility::Type::kVisible;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            _vShoeNode[1] = node;
        }         
        else if( nodeName == "กางเกง" ) {
            vType = SkSVGVisibility::Type::kVisible;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            _vPantNode[1] = node;
        }             
        else if( nodeName == "BG" || nodeName == "ผม" || nodeName == "คอ 8 แบบ" ||
            nodeName == "คอเสื้อผ่าหน้า 7 แบบ" || nodeName == "คอเสื้อเดรสผ่าหน้า 7 แบบ" ||
            nodeName == "แขนเสื้อ" || nodeName == "ความยาว (เสื้อ)" || nodeName == "ความยาว (เดรส)" ||
            nodeName == "ปกเสื้อ (คอกลม)" || nodeName == "ปกเสื้อ (คอวี)" ||
            nodeName == "ระบายชายเสื้อ 10 ซม." || nodeName == "ระบายชายเสื้อ 20 ซม." ||
            starts_with( nodeName, "กระเป๋า" ) || starts_with( nodeName, "ฝากระเป๋า" ) 
            ) {

            vType = SkSVGVisibility::Type::kVisible;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);
        }
        else {
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);
        }
    }

    if( deep == 3 ) {     
        if( starts_with( parentNodeName, "กระเป๋า" ) ) {
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            int pocket_level = -1;
            int pocket_side = -1;
            if( ends_with( parentNodeName, "(ขวาบน)" ) ) {
                currentSVGTag = 7;
                pocket_level = 0;
                pocket_side = 0;
            }
            else if( ends_with( parentNodeName, "(ซ้ายบน)" ) ) {
                currentSVGTag = 7;
                pocket_level = 0;
                pocket_side = 1;
            }
            else if( ends_with( parentNodeName, "(เอวขวาล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 1;
                pocket_side = 0;
            }
            else if( ends_with( parentNodeName, "(เอวซ้ายล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 1;
                pocket_side = 1;
            }
            else if( ends_with( parentNodeName, "(สะโพกบนขวาล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 2;
                pocket_side = 0;
            }
            else if( ends_with( parentNodeName, "(สะโพกบนซ้ายล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 2;
                pocket_side = 1;
            }
            else if( ends_with( parentNodeName, "(สะโพกล่างขวาล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 3;
                pocket_side = 0;
            }
            else if( ends_with( parentNodeName, "(สะโพกล่างซ้ายล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 3;
                pocket_side = 1;
            }
            else if( ends_with( parentNodeName, "(เดรสขวาล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 4;
                pocket_side = 0;
            }
            else if( ends_with( parentNodeName, "(เดรสซ้ายล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 4;
                pocket_side = 1;
            }

            int pocket_type = -1;
            if( starts_with( nodeName, "กป ทรงเหลี่ยม") ) {
                pocket_type = 1;
            }
            else if( starts_with( nodeName, "กป ทรงโค้ง") ) {
                pocket_type = 2;
            }
            else if( starts_with( nodeName, "กป ทรงหกเหลี่ยม") ) {
                pocket_type = 3;
            }
            else if( starts_with( nodeName, "กป ทรงแหลม") ) {
                pocket_type = 4;
            }
            else if( starts_with( nodeName, "กป จีบboxสี่เหลี่ยม") ) {
                pocket_type = 5;
            }
            else if( starts_with( nodeName, "กป จีบboxทรงโค้ง") ) {
                pocket_type = 6;
            }
            else if( starts_with( nodeName, "กป จีบboxหกเหลี่ยม") ) {
                pocket_type = 7;
            }
            else if( starts_with( nodeName, "กป จีบคว่ำสี่เหลี่ยม") ) {
                pocket_type = 8;
            }
            else if( starts_with( nodeName, "กป จีบคว่ำทรงโค้ง") ) {
                pocket_type = 9;
            }
            else if( starts_with( nodeName, "กป จีบคว่ำหกเหลี่ยม") ) {
                pocket_type = 10;
            }

            if( pocket_level != -1 && pocket_side != -1 && pocket_type != -1 ) {
                //SkDebugf( "Set Pocket %i %i %i %s\n", pocket_level, pocket_side, pocket_type, nodeName.c_str() );
                _vPocketNode[pocket_level][pocket_side][pocket_type] = node;
            }
        }
        else if( starts_with( parentNodeName, "ฝากระเป๋า" ) ) {
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            int pocket_level = -1;
            int pocket_side = -1;
            if( ends_with( parentNodeName, "(ขวาบน)" ) ) {
                currentSVGTag = 7;
                pocket_level = 0;
                pocket_side = 0;
            }
            else if( ends_with( parentNodeName, "(ซ้ายบน)" ) ) {
                currentSVGTag = 7;
                pocket_level = 0;
                pocket_side = 1;
            }
            else if( ends_with( parentNodeName, "(เอวขวาล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 1;
                pocket_side = 0;
            }
            else if( ends_with( parentNodeName, "(เอวซ้ายล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 1;
                pocket_side = 1;
            }
            else if( ends_with( parentNodeName, "(สะโพกบนขวาล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 2;
                pocket_side = 0;
            }
            else if( ends_with( parentNodeName, "(สะโพกบนซ้ายล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 2;
                pocket_side = 1;
            }
            else if( ends_with( parentNodeName, "(สะโพกล่างขวาล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 3;
                pocket_side = 0;
            }
            else if( ends_with( parentNodeName, "(สะโพกล่างซ้ายล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 3;
                pocket_side = 1;
            }
            else if( ends_with( parentNodeName, "(เดรสขวาล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 4;
                pocket_side = 0;
            }
            else if( ends_with( parentNodeName, "(เดรสซ้ายล่าง)" ) ) {
                currentSVGTag = 8;
                pocket_level = 4;
                pocket_side = 1;
            }    

            int pocketcover_type = -1;
            if( starts_with( nodeName, "ฝากระเป๋าเหลี่ยม") ) {
                pocketcover_type = 1;
            }
            else if( starts_with( nodeName, "ฝากระเป๋ามน") ) {
                pocketcover_type = 2;
            }                    
            else if( starts_with( nodeName, "ฝากระเป๋าแหลม") ) {
                pocketcover_type = 3;
            }                    
            else if( starts_with( nodeName, "ฝากระเป๋าตัดมุม") || starts_with( nodeName, "ฝากระเป๋าหกเหลี่ยม") ) {
                pocketcover_type = 4;
            }       

            if( pocket_level != -1 && pocket_side != -1 && pocketcover_type != -1 ) {
                //SkDebugf( "Set PocketCover %i %i %i %s\n", pocket_level, pocket_side, pocketcover_type, nodeName.c_str() );
                _vPocketCoverNode[pocket_level][pocket_side][pocketcover_type] = node;
            }                         
        }
        else if( parentNodeName == "BG" ) {
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            if( nodeName == "2" ) {
                _vBGNode[1] = node;
            }
            else if( nodeName == "3" ) {
                _vBGNode[2] = node;
            }
            else if( nodeName == "4" ) {
                _vBGNode[3] = node;
            }
            else if( nodeName == "5" ) {
                _vBGNode[4] = node;
            }
            else if( nodeName == "6" ) {
                _vBGNode[5] = node;
            }
            else if( nodeName == "7" ) {
                _vBGNode[6] = node;
            }
            else if( nodeName == "8" ) {
                _vBGNode[7] = node;
            }
            else if( nodeName == "9" ) {
                _vBGNode[8] = node;
            }
        } else if( parentNodeName == "ผม" ) {
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            if( nodeName == "1" ) {
                currentSVGTag = 10;
                _vHairNode[1] = node;
            }
            else if( nodeName == "2" ) {
                _vHairNode[2] = node;
            }
            else if( nodeName == "3" ) {
                _vHairNode[3] = node;
            }
            else if( nodeName == "4" ) {
                _vHairNode[4] = node;
            }
            else if( nodeName == "5" ) {
                _vHairNode[5] = node;
            }
        } else if( parentNodeName == "แขนเสื้อ" ) {
            currentSVGTag = 4;
            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            if( nodeName == "แขนกุด" ) {
                _vSleeveNode[1] = node ; 
            }
            else {
                int n = nodeName[0] - 'H';
                _vSleeveNode[n + 1] = node;
            }   
        } else if( parentNodeName == "ความยาว (เสื้อ)" ) {
            currentSVGTag = 1;

            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            if( nodeName == "สะโพกเอว" ) {
                _vLengthNode[1] = node;
            }
            else if( nodeName == "สะโพกบน" ) {
                _vLengthNode[2] = node;
            }
            else if( nodeName == "สะโพกล่าง" ) {
                _vLengthNode[3] = node;
            }
        } else if( parentNodeName == "ความยาว (เดรส)" ) {
            currentSVGTag = 1;

            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            if( nodeName == "เหนือเข่า" ) {
                _vLengthNode[4] = node;
            }
            else if( nodeName == "ความยาวคลุมเข่า" ) {
                _vLengthNode[5] = node;
            }
        } else if( parentNodeName == "คอ 8 แบบ" ) {
            currentSVGTag = 2;

            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            int n = 1;  

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
            else if( starts_with( nodeName, "คอสี่เหลี่ยมแคบ" ) ) {
                c = 5;
            }    
            else if( starts_with( nodeName, "คอคว้านแคบ" ) ) {
                c = 6;
            }                 
            else if( starts_with( nodeName, "คอสี่เหลี่ยม" ) ) {
                c = 3;
            }    
            else if( starts_with( nodeName, "คอคว้าน" ) ) {
                c = 4;
            }                 
            else if( starts_with( nodeName, "คอหยดน้ำมีโบว์" ) ) {
                c = 7;
            }      
            else {
                SkDebugf( "คอไม่เข้าพวก %s\n", nodeName.c_str() );
            }           
            
            _vNeckNode[n][c] = node;
        } else if( parentNodeName == "คอเสื้อผ่าหน้า 7 แบบ" ) {
            currentSVGTag = 2;

            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            int n = -1;
            if( ends_with( nodeName, "เอว" ) ) {
                n = 2;
            }          
            else if( ends_with( nodeName, "บน" ) ) {
                n = 3;
            }          
            else if( ends_with( nodeName, "ล่าง" ) ) {
                n = 4;
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
            else if( starts_with( nodeName, "สี่เหลี่ยมแคบ" ) ) {
                c = 5;
            }    
            else if( starts_with( nodeName, "คอคว้านแคบ" ) ) {
                c = 6;
            }                 
            else if( starts_with( nodeName, "คอสี่เหลี่ยม" ) ) {
                c = 3;
            }    
            else if( starts_with( nodeName, "คอคว้าน" ) ) {
                c = 4;
            }                 
            else if( starts_with( nodeName, "คอหยดน้ำมีโบว์" ) ) {
                c = 7;
            }      
            else {
                SkDebugf( "คอไม่เข้าพวก %s\n", nodeName.c_str() );
            }           
            
            if( n != -1 && c != -1 ) {
                _vNeckNode[n][c] = node;
            }
        } else if( parentNodeName == "คอเสื้อเดรสผ่าหน้า 7 แบบ" ) {
            currentSVGTag = 2;

            vType = SkSVGVisibility::Type::kHidden;

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            int n = -1;
            if( ends_with( nodeName, "(เหนือเข่า)" ) ) {
                n = 5;
            }          
            else if( ends_with( nodeName, "(คลุมเข่า)" ) ) {
                n = 6;
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
            else if( starts_with( nodeName, "แคบ" ) ) {
                c = 5;
            }    
            else if( starts_with( nodeName, "คว้านแคบ" ) ) {
                c = 6;
            }                 
            else if( starts_with( nodeName, "คอสี่เหลี่ยม" ) ) {
                c = 3;
            }    
            else if( starts_with( nodeName, "คอคว้าน" ) ) {
                c = 4;
            }                 
            else if( starts_with( nodeName, "คอหยดน้ำมีโบว์" ) ) {
                c = 7;
            }      
            else {
                SkDebugf( "คอไม่เข้าพวก %s\n", nodeName.c_str() );
            }           
            
            if( n != -1 && c != -1 ) {
                _vNeckNode[n][c] = node;
            }
        } else if( parentNodeName == "ระบายชายเสื้อ 10 ซม." || parentNodeName == "ระบายชายเสื้อ 20 ซม.") {
            currentSVGTag = 6;
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

            if( parentNodeName == "ระบายชายเสื้อ 20 ซม." ) {
                _vHemNode[l][t][2] = node;     
            }
            else {
                _vHemNode[l][t][1] = node;     
            }
        } else if( parentNodeName == "ปกเสื้อ (คอกลม)" || parentNodeName == "ปกเสื้อ (คอวี)" ) {
            currentSVGTag = 5;
            vType = SkSVGVisibility::Type::kHidden;  

            auto visible = SkSVGVisibility(vType);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);      

            if( nodeName == "ปกบัว") {
                _vCollarNode[1] = node;
            }
            else if( nodeName == "ปกบัวแหลม") {
                _vCollarNode[2] = node;
            }
            else if( nodeName == "ปกคอจีน") {
                _vCollarNode[3] = node;
            }
            else if( nodeName == "ปกระบาย") {
                _vCollarNode[4] = node;
            }
            else if( nodeName == "ปกผูกโบว์") {
                _vCollarNode[5] = node;
            }  
            else if( nodeName == "" ) {
                _vCollarNode[6] = node;
            }                                     
        }     
    }
    if( deep >= 3 ) {  
        if( currentSVGTag != 0 ) {
            const SkSVGProperty<SkSVGPaint, true> &svg_fill = node->getFill();
            if( svg_fill.isValue() ) {
                if( svg_fill->type() == SkSVGPaint::Type::kColor ) {
                    SkSVGColor col = svg_fill->color();
                    if( currentSVGTag == 11 ) {
                        //SkDebugf( "SVGTag: %i xColor: %X\n", currentSVGTag, col.color() );
                    }
                    if( col.color() == 0xFFFFFFFF || col.color() == 0xFFFFF8F3 || 
                        col.color() == 0xFFFFCA9F || col.color() == 0xFFFBFBF9) {
                        if( currentSVGTag == 3 ) { // body
                            node->_customTagID = 4; // skin
                        }
                        else {
                            node->_customTagID = 1; // length
                        }
                    }
                    else if( col.color() == 0xFFD0D2D3 || col.color() == 0xFFE2DAD5 ) {
                        node->_customTagID = 2;
                    }
                    else if( col.color() == 0xFF6D6F71 || col.color() == 0xFF6D6E70 || 
                        col.color() == 0xFF636363 || col.color() == 0xFF58595B || col.color() == 0xFFE6E7E8 ) {
                        node->_customTagID = 3;
                    }
                    else if( col.color() == 0xFF231F20 || col.color() == 0xFF1D1D1B ) {
                        node->_customTagID = 5; // ตัดเส้นเนื้อ
                    }
                    else if( col.color() == 0xFF3B2314 ) {
                        // ผม
                    }
                    else 
                    {
                        node->_customTagID = 99;
                        //SkDebugf( "SVGTag: %i xColor: %X\n", currentSVGTag, col.color() );
                    }
                }
            }
            
            /*
            if( col.color() != 0xFF231F20 && col.color() != 0xFFD0D2D3 ) {
                node->setFill( sfillfill ); 
            }
            */
        }
    }

    if( node->hasChildren() ) {
        if( tag == SkSVGTag::kG ) {
            SkSVGG *pG = (SkSVGG *)node;
            int numChildren = (int)pG->fChildren.size();
            for( int i=0; i<numChildren; i++ ) {
                sk_sp<SkSVGNode> child_node = pG->fChildren.at( i );
                scanSubNode( child_node.get(), deep+1, nodeName );
            }
        }
    }    
}

void DecMagicPattern::setPocket( int level, int side, int type ) {
    //SkDebugf( "SetPocket %i %i %i %i\n", level, side, type, nLength );
    int _l = level;
    if( level == 1 ) {
        if( nLength == 5 ) {
            level += nLength - 2;
        }
        else {
            level += nLength-1;
        }
    }
    //SkDebugf( "SetPocket %i %i %i\n", level, side, type );
    nPocket[level][side] = type;

    for( int l=0; l<NUM_POCKET_LEVEL; l++ ) {
        for( int i=0; i<NUM_POCKET_SIDE; i++ ) {
            for( int p=0; p<NUM_POCKET_TYPE; p++ ) {
                if( _vPocketNode[l][i][p] != NULL ) {
                    if( nPocket[l][i] == p ) {
                        setNodeVisibility( _vPocketNode[l][i][p], true );
                        setNodeColor( _vPocketNode[l][i][p],  currentColorPocket0);
                    }
                    else {
                        setNodeVisibility( _vPocketNode[l][i][p] , false );
                    }
                }
            }
        }
    }     
    if( pSVG ) {
        pSVG->invalidate();
        pSVG->dirty( true, false );
    }   
}

void DecMagicPattern::setPocketCover( int level, int side, int type ) {
    //SkDebugf( "SetPocketCover %i %i %i\n", level, side, type );
    int _l = level;
    if( level == 1 ) {
        if( nLength == 5 ) {
            level += nLength - 2;
        }
        else {
            level += nLength-1;
        }
    }    
    nPocketCover[level][side] = type;

    for( int l=0; l<NUM_POCKET_LEVEL; l++ ) {
        for( int i=0; i<NUM_POCKET_SIDE; i++ ) {
            for( int p=0; p<NUM_POCKETCOVER_TYPE; p++ ) {
                if( _vPocketCoverNode[l][i][p] != NULL ) {
                    if( nPocketCover[l][i] == p ) {
                        setNodeVisibility( _vPocketCoverNode[l][i][p], true );
                        setNodeColor( _vPocketCoverNode[l][i][p], currentColorPocket0 );
                    }
                    else {
                        setNodeVisibility( _vPocketCoverNode[l][i][p] , false );
                    }
                }
            }
        }
    }    
    //SkDebugf( "Pocket Cover: %i %i %i\n", level, side, type ); 
    if( pSVG ) {
        pSVG->invalidate();
        pSVG->dirty( true, false );
    }   
}

void DecMagicPattern::update() {
    DecWidget::update();
    //SkDebugf( "Update\n" );
    static int nCount = 0;
    if( !svgStartLoad ) {
        SkDebugf( "Load SVG\n" );
        svgStartLoad = true;
        pSVG->_onLoad = [=]( DecSVGDom &dom ) {
            svgLoaded = true;
            //onLoad( dom );
        };        
#ifdef __EMSCRIPTEN__    
        pSVG->load( "_magicpattern/mp2/dress-01.svg" );
#else
        pSVG->load( "_magicpattern/dress-01.svg" );
#endif        
        return;
    }
    if( !svgLoaded ) {
        return;
    }
    if( !svgScanned ) {
        SkDebugf( "Scan SVG\n" );
        svgScanned = true;
        onLoad( *pSVG );
        return;
    }
    if( !svgReady ) {
        return;
    }

    int ww = pSVG->width();
    int hh = pSVG->height();
    int hp = pButtonPanel->height();
    if( hh != -1 && hp != -1 && hh - hp != nSVGHeight ) {
        SkSize sz = fDom->containerSize();
        SkDebugf( "onResized - Height: %i %i %i\n", ww, hh, hp );
        SkDebugf( "onResized - SVG: %i %i\n", (int)sz.fWidth, (int)sz.fHeight );
        nSVGHeight = hh - hp;

        fSVGScaleFit = fSVGScale = nSVGHeight / sz.fHeight;
        fSVGScaleMin = fSVGScaleFit;
        fSVGScaleMax = fSVGScaleMin * 2.4;
        pSVG->fScale = fSVGScale;        
        pSVG->invalidate();
        pSVG->dirty( true, false );
        //pButtonPanel->invalidate();
    }

    nCount++;
    if( fSVGZoomChange ) {
        fSVGScale += fSVGZoomChange;
        if( fSVGScale > fSVGScaleMax ) {
            fSVGScale = fSVGScaleMax;
        }
        if( fSVGScale < fSVGScaleMin ) {
            fSVGScale = fSVGScaleMin;
        }
        pSVG->fScale = fSVGScale;
        pSVG->invalidate();
        pSVG->dirty( true, false );
        //pButtonPanel->invalidate();
    }
}

void DecMagicPattern::onDeviceChanged( int index ) {
    switch( index ) {
        case 0:
            //iPhone 8 - 375, 553
            pContainer->max_width( 375 );
            pContainer->max_height( 553 );
            UI()->invalidate();
            break;
        case 1:
            //iPhone 8 Plus - 414, 622
            pContainer->max_width( 414 );
            pContainer->max_height( 622 );
            UI()->invalidate();
            break;
        case 2:
            //iPhone 11 - 414, 715
            pContainer->max_width( 414 );
            pContainer->max_height( 715 );
            UI()->invalidate();
            break;
        case 3:
            //iPhone 11 Pro - 375, 635
            pContainer->max_width( 375 );
            pContainer->max_height( 635 );
            UI()->invalidate();
            break;
        case 4:
            //iPhone 11 Pro Max - 414, 719
            pContainer->max_width( 414 );
            pContainer->max_height( 719 );
            UI()->invalidate();
            break;
        case 5:
            //iPhone SE - 375, 553
            pContainer->max_width( 375 );
            pContainer->max_height( 553 );
            UI()->invalidate();
            break;
        case 6:
            //iPhone 12 Mini - 375, 629
            pContainer->max_width( 375 );
            pContainer->max_height( 629 );
            UI()->invalidate();
            break;
        case 7:
            //iPhone 12 - 390, 664
            pContainer->max_width( 390 );
            pContainer->max_height( 664 );
            UI()->invalidate();
            break;
        case 8:
            //iPhone 12 Pro - 390, 664
            pContainer->max_width( 390 );
            pContainer->max_height( 664 );
            UI()->invalidate();
            break;
        case 9:
            //iPhone 12 Pro Max - 428, 746
            pContainer->max_width( 428 );
            pContainer->max_height( 746 );
            UI()->invalidate();
            break;
        case 10:
            //iPod Touch - 320, 454
            pContainer->max_width( 320 );
            pContainer->max_height( 454 );
            UI()->invalidate();
            break;
        case 11:
            //iPad Pro 9.7 in - 768, 954
            pContainer->max_width( 768 );
            pContainer->max_height( 954 );
            UI()->invalidate();
            break;
        case 12:
            //iPad Pro 11 in - 834, 1120
            pContainer->max_width( 834 );
            pContainer->max_height( 1120 );
            UI()->invalidate();
            break;
        case 13:
            //iPad Pro 12.9 in - 1024, 1292
            pContainer->max_width( 1024 );
            pContainer->max_height( 1292 );
            UI()->invalidate();
            break;
        case 14:
            //iPad 8 Gen - 810, 1010
            pContainer->max_width( 810 );
            pContainer->max_height( 1010 );
            UI()->invalidate();
            break;
        case 15:
            //iPad Air 4 Gen - 820, 1106
            pContainer->max_width( 820 );
            pContainer->max_height( 1106 );
            UI()->invalidate();
            break;
        case 16:
            //Custom - 370, 442
            pContainer->max_width( 370 );
            pContainer->max_height( 442 );
            pSVG->fScale = 1.0;
            pSVG->invalidate();
            pSVG->dirty( true, false );
            UI()->invalidate();
            break;
    }
}

const char *PID2SKU[] = {
"WPAN-AHU","WPAN-BHU","WPAN-CHU","WPAN-DHU","WPAN-EHU","WPAN-FHU","WPBN-AHU","WPBN-GHU","WPCN-AHU","WPDN-AHU","WPEN-AHU","WPAS-AIU","WPAS-BIU","WPAS-CIU","WPAS-DIU","WPAS-EIU","WPAS-FIU","WPBS-AIU","WPBS-GIU","WPCS-AIU","WPDS-AIU",
"WPES-AIU","WPAS-AJU","WPAS-BJU","WPAS-CJU","WPAS-DJU","WPAS-EJU","WPAS-FJU","WPBS-AJU","WPBS-GJU","WPCS-AJU","WPDS-AJU","WPES-AJU","WPAS-AKU","WPAS-BKU","WPAS-CKU","WPAS-DKU","WPAS-EKU","WPAS-FKU","WPBS-AKU","WPBS-GKU",
"WPCS-AKU","WPDS-AKU","WPES-AKU","WPAS-ALU","WPAS-BLU","WPAS-CLU","WPAS-DLU","WPAS-ELU","WPAS-FLU","WPBS-ALU","WPBS-GLU","WPCS-ALU","WPDS-ALU","WPES-ALU","WPAS-AMU","WPAS-BMU","WPAS-CMU","WPAS-DMU","WPAS-EMU","WPAS-FMU",
"WPBS-AMU","WPBS-GMU","WPCS-AMU","WPDS-AMU","WPES-AMU","WPAS-ANU","WPAS-BNU","WPAS-CNU","WPAS-DNU","WPAS-ENU","WPAS-FNU","WPBS-ANU","WPBS-GNU","WPCS-ANU","WPDS-ANU","WPES-ANU","WPAS-AOU","WPAS-BOU","WPAS-COU","WPAS-DOU",
"WPAS-EOU","WPAS-FOU","WPBS-AOU","WPBS-GOU","WPCS-AOU","WPDS-AOU","WPES-AOU","WPAS-APU","WPAS-BPU","WPAS-CPU","WPAS-DPU","WPAS-EPU","WPAS-FPU","WPBS-APU","WPBS-GPU","WPCS-APU","WPDS-APU","WPES-APU","WPAS-AQU","WPAS-BQU",
"WPAS-CQU","WPAS-DQU","WPAS-EQU","WPAS-FQU","WPBS-AQU","WPBS-GQU","WPCS-AQU","WPDS-AQU","WPES-AQU","WPAS-ARU","WPAS-BRU","WPAS-CRU","WPAS-DRU","WPAS-ERU","WPAS-FRU","WPBS-ARU","WPBS-GRU","WPCS-ARU","WPDS-ARU","WPES-ARU",
"WPAS-ASU","WPAS-BSU","WPAS-CSU","WPAS-DSU","WPAS-ESU","WPAS-FSU","WPBS-ASU","WPBS-GSU","WPCS-ASU","WPDS-ASU","WPES-ASU","WPAS-ATU","WPAS-BTU","WPAS-CTU","WPAS-DTU","WPAS-ETU","WPAS-FTU","WPBS-ATU","WPBS-GTU","WPCS-ATU",
"WPDS-ATU","WPES-ATU","WPAN-AHV","WPAN-BHV","WPAN-CHV","WPAN-DHV","WPAN-EHV","WPAN-FHV","WPBN-AHV","WPBN-GHV","WPCN-AHV","WPDN-AHV","WPEN-AHV","WPAS-AIV","WPAS-BIV","WPAS-CIV","WPAS-DIV","WPAS-EIV","WPAS-FIV","WPBS-AIV",
"WPBS-GIV","WPCS-AIV","WPDS-AIV","WPES-AIV","WPAS-AJV","WPAS-BJV","WPAS-CJV","WPAS-DJV","WPAS-EJV","WPAS-FJV","WPBS-AJV","WPBS-GJV","WPCS-AJV","WPDS-AJV","WPES-AJV","WPAS-AKV","WPAS-BKV","WPAS-CKV","WPAS-DKV","WPAS-EKV",
"WPAS-FKV","WPBS-AKV","WPBS-GKV","WPCS-AKV","WPDS-AKV","WPES-AKV","WPAS-ALV","WPAS-BLV","WPAS-CLV","WPAS-DLV","WPAS-ELV","WPAS-FLV","WPBS-ALV","WPBS-GLV","WPCS-ALV","WPDS-ALV","WPES-ALV","WPAS-AMV","WPAS-BMV","WPAS-CMV",
"WPAS-DMV","WPAS-EMV","WPAS-FMV","WPBS-AMV","WPBS-GMV","WPCS-AMV","WPDS-AMV","WPES-AMV","WPAS-ANV","WPAS-BNV","WPAS-CNV","WPAS-DNV","WPAS-ENV","WPAS-FNV","WPBS-ANV","WPBS-GNV","WPCS-ANV","WPDS-ANV","WPES-ANV","WPAS-AOV",
"WPAS-BOV","WPAS-COV","WPAS-DOV","WPAS-EOV","WPAS-FOV","WPBS-AOV","WPBS-GOV","WPCS-AOV","WPDS-AOV","WPES-AOV","WPAS-APV","WPAS-BPV","WPAS-CPV","WPAS-DPV","WPAS-EPV","WPAS-FPV","WPBS-APV","WPBS-GPV","WPCS-APV","WPDS-APV",
"WPES-APV","WPAS-AQV","WPAS-BQV","WPAS-CQV","WPAS-DQV","WPAS-EQV","WPAS-FQV","WPBS-AQV","WPBS-GQV","WPCS-AQV","WPDS-AQV","WPES-AQV","WPAS-ARV","WPAS-BRV","WPAS-CRV","WPAS-DRV","WPAS-ERV","WPAS-FRV","WPBS-ARV","WPBS-GRV",
"WPCS-ARV","WPDS-ARV","WPES-ARV","WPAS-ASV","WPAS-BSV","WPAS-CSV","WPAS-DSV","WPAS-ESV","WPAS-FSV","WPBS-ASV","WPBS-GSV","WPCS-ASV","WPDS-ASV","WPES-ASV","WPAS-ATV","WPAS-BTV","WPAS-CTV","WPAS-DTV","WPAS-ETV","WPAS-FTV",
"WPBS-ATV","WPBS-GTV","WPCS-ATV","WPDS-ATV","WPES-ATV","WPAN-AHW","WPAN-BHW","WPAN-CHW","WPAN-DHW","WPAN-EHW","WPAN-FHW","WPBN-AHW","WPBN-GHW","WPCN-AHW","WPDN-AHW","WPEN-AHW","WPAS-AIW","WPAS-BIW","WPAS-CIW","WPAS-DIW",
"WPAS-EIW","WPAS-FIW","WPBS-AIW","WPBS-GIW","WPCS-AIW","WPDS-AIW","WPES-AIW","WPAS-AJW","WPAS-BJW","WPAS-CJW","WPAS-DJW","WPAS-EJW","WPAS-FJW","WPBS-AJW","WPBS-GJW","WPCS-AJW","WPDS-AJW","WPES-AJW","WPAS-AKW","WPAS-BKW",
"WPAS-CKW","WPAS-DKW","WPAS-EKW","WPAS-FKW","WPBS-AKW","WPBS-GKW","WPCS-AKW","WPDS-AKW","WPES-AKW","WPAS-ALW","WPAS-BLW","WPAS-CLW","WPAS-DLW","WPAS-ELW","WPAS-FLW","WPBS-ALW","WPBS-GLW","WPCS-ALW","WPDS-ALW","WPES-ALW",
"WPAS-AMW","WPAS-BMW","WPAS-CMW","WPAS-DMW","WPAS-EMW","WPAS-FMW","WPBS-AMW","WPBS-GMW","WPCS-AMW","WPDS-AMW","WPES-AMW","WPAS-ANW","WPAS-BNW","WPAS-CNW","WPAS-DNW","WPAS-ENW","WPAS-FNW","WPBS-ANW","WPBS-GNW","WPCS-ANW",
"WPDS-ANW","WPES-ANW","WPAS-AOW","WPAS-BOW","WPAS-COW","WPAS-DOW","WPAS-EOW","WPAS-FOW","WPBS-AOW","WPBS-GOW","WPCS-AOW","WPDS-AOW","WPES-AOW","WPAS-APW","WPAS-BPW","WPAS-CPW","WPAS-DPW","WPAS-EPW","WPAS-FPW","WPBS-APW",
"WPBS-GPW","WPCS-APW","WPDS-APW","WPES-APW","WPAS-AQW","WPAS-BQW","WPAS-CQW","WPAS-DQW","WPAS-EQW","WPAS-FQW","WPBS-AQW","WPBS-GQW","WPCS-AQW","WPDS-AQW","WPES-AQW","WPAS-ARW","WPAS-BRW","WPAS-CRW","WPAS-DRW","WPAS-ERW",
"WPAS-FRW","WPBS-ARW","WPBS-GRW","WPCS-ARW","WPDS-ARW","WPES-ARW","WPAS-ASW","WPAS-BSW","WPAS-CSW","WPAS-DSW","WPAS-ESW","WPAS-FSW","WPBS-ASW","WPBS-GSW","WPCS-ASW","WPDS-ASW","WPES-ASW","WPAS-ATW","WPAS-BTW","WPAS-CTW",
"WPAS-DTW","WPAS-ETW","WPAS-FTW","WPBS-ATW","WPBS-GTW","WPCS-ATW","WPDS-ATW","WPES-ATW","HPAN-AHU","HPAN-BHU","HPAN-CHU","HPAN-DHU","HPAN-EHU","HPAN-FHU","HPBN-AHU","HPBN-GHU","HPCN-AHU","HPDN-AHU","HPEN-AHU","HPAS-AIU",
"HPAS-BIU","HPAS-CIU","HPAS-DIU","HPAS-EIU","HPAS-FIU","HPBS-AIU","HPBS-GIU","HPCS-AIU","HPDS-AIU","HPES-AIU","HPAS-AJU","HPAS-BJU","HPAS-CJU","HPAS-DJU","HPAS-EJU","HPAS-FJU","HPBS-AJU","HPBS-GJU","HPCS-AJU","HPDS-AJU",
"HPES-AJU","HPAS-AKU","HPAS-BKU","HPAS-CKU","HPAS-DKU","HPAS-EKU","HPAS-FKU","HPBS-AKU","HPBS-GKU","HPCS-AKU","HPDS-AKU","HPES-AKU","HPAS-ALU","HPAS-BLU","HPAS-CLU","HPAS-DLU","HPAS-ELU","HPAS-FLU","HPBS-ALU","HPBS-GLU",
"HPCS-ALU","HPDS-ALU","HPES-ALU","HPAS-AMU","HPAS-BMU","HPAS-CMU","HPAS-DMU","HPAS-EMU","HPAS-FMU","HPBS-AMU","HPBS-GMU","HPCS-AMU","HPDS-AMU","HPES-AMU","HPAS-ANU","HPAS-BNU","HPAS-CNU","HPAS-DNU","HPAS-ENU","HPAS-FNU",
"HPBS-ANU","HPBS-GNU","HPCS-ANU","HPDS-ANU","HPES-ANU","HPAS-AOU","HPAS-BOU","HPAS-COU","HPAS-DOU","HPAS-EOU","HPAS-FOU","HPBS-AOU","HPBS-GOU","HPCS-AOU","HPDS-AOU","HPES-AOU","HPAS-APU","HPAS-BPU","HPAS-CPU","HPAS-DPU",
"HPAS-EPU","HPAS-FPU","HPBS-APU","HPBS-GPU","HPCS-APU","HPDS-APU","HPES-APU","HPAS-AQU","HPAS-BQU","HPAS-CQU","HPAS-DQU","HPAS-EQU","HPAS-FQU","HPBS-AQU","HPBS-GQU","HPCS-AQU","HPDS-AQU","HPES-AQU","HPAS-ARU","HPAS-BRU",
"HPAS-CRU","HPAS-DRU","HPAS-ERU","HPAS-FRU","HPBS-ARU","HPBS-GRU","HPCS-ARU","HPDS-ARU","HPES-ARU","HPAS-ASU","HPAS-BSU","HPAS-CSU","HPAS-DSU","HPAS-ESU","HPAS-FSU","HPBS-ASU","HPBS-GSU","HPCS-ASU","HPDS-ASU","HPES-ASU",
"HPAS-ATU","HPAS-BTU","HPAS-CTU","HPAS-DTU","HPAS-ETU","HPAS-FTU","HPBS-ATU","HPBS-GTU","HPCS-ATU","HPDS-ATU","HPES-ATU","HPAN-AHV","HPAN-BHV","HPAN-CHV","HPAN-DHV","HPAN-EHV","HPAN-FHV","HPBN-AHV","HPBN-GHV","HPCN-AHV",
"HPDN-AHV","HPEN-AHV","HPAS-AIV","HPAS-BIV","HPAS-CIV","HPAS-DIV","HPAS-EIV","HPAS-FIV","HPBS-AIV","HPBS-GIV","HPCS-AIV","HPDS-AIV","HPES-AIV","HPAS-AJV","HPAS-BJV","HPAS-CJV","HPAS-DJV","HPAS-EJV","HPAS-FJV","HPBS-AJV",
"HPBS-GJV","HPCS-AJV","HPDS-AJV","HPES-AJV","HPAS-AKV","HPAS-BKV","HPAS-CKV","HPAS-DKV","HPAS-EKV","HPAS-FKV","HPBS-AKV","HPBS-GKV","HPCS-AKV","HPDS-AKV","HPES-AKV","HPAS-ALV","HPAS-BLV","HPAS-CLV","HPAS-DLV","HPAS-ELV",
"HPAS-FLV","HPBS-ALV","HPBS-GLV","HPCS-ALV","HPDS-ALV","HPES-ALV","HPAS-AMV","HPAS-BMV","HPAS-CMV","HPAS-DMV","HPAS-EMV","HPAS-FMV","HPBS-AMV","HPBS-GMV","HPCS-AMV","HPDS-AMV","HPES-AMV","HPAS-ANV","HPAS-BNV","HPAS-CNV",
"HPAS-DNV","HPAS-ENV","HPAS-FNV","HPBS-ANV","HPBS-GNV","HPCS-ANV","HPDS-ANV","HPES-ANV","HPAS-AOV","HPAS-BOV","HPAS-COV","HPAS-DOV","HPAS-EOV","HPAS-FOV","HPBS-AOV","HPBS-GOV","HPCS-AOV","HPDS-AOV","HPES-AOV","HPAS-APV",
"HPAS-BPV","HPAS-CPV","HPAS-DPV","HPAS-EPV","HPAS-FPV","HPBS-APV","HPBS-GPV","HPCS-APV","HPDS-APV","HPES-APV","HPAS-AQV","HPAS-BQV","HPAS-CQV","HPAS-DQV","HPAS-EQV","HPAS-FQV","HPBS-AQV","HPBS-GQV","HPCS-AQV","HPDS-AQV",
"HPES-AQV","HPAS-ARV","HPAS-BRV","HPAS-CRV","HPAS-DRV","HPAS-ERV","HPAS-FRV","HPBS-ARV","HPBS-GRV","HPCS-ARV","HPDS-ARV","HPES-ARV","HPAS-ASV","HPAS-BSV","HPAS-CSV","HPAS-DSV","HPAS-ESV","HPAS-FSV","HPBS-ASV","HPBS-GSV",
"HPCS-ASV","HPDS-ASV","HPES-ASV","HPAS-ATV","HPAS-BTV","HPAS-CTV","HPAS-DTV","HPAS-ETV","HPAS-FTV","HPBS-ATV","HPBS-GTV","HPCS-ATV","HPDS-ATV","HPES-ATV","HPAN-AHW","HPAN-BHW","HPAN-CHW","HPAN-DHW","HPAN-EHW","HPAN-FHW",
"HPBN-AHW","HPBN-GHW","HPCN-AHW","HPDN-AHW","HPEN-AHW","HPAS-AIW","HPAS-BIW","HPAS-CIW","HPAS-DIW","HPAS-EIW","HPAS-FIW","HPBS-AIW","HPBS-GIW","HPCS-AIW","HPDS-AIW","HPES-AIW","HPAS-AJW","HPAS-BJW","HPAS-CJW","HPAS-DJW",
"HPAS-EJW","HPAS-FJW","HPBS-AJW","HPBS-GJW","HPCS-AJW","HPDS-AJW","HPES-AJW","HPAS-AKW","HPAS-BKW","HPAS-CKW","HPAS-DKW","HPAS-EKW","HPAS-FKW","HPBS-AKW","HPBS-GKW","HPCS-AKW","HPDS-AKW","HPES-AKW","HPAS-ALW","HPAS-BLW",
"HPAS-CLW","HPAS-DLW","HPAS-ELW","HPAS-FLW","HPBS-ALW","HPBS-GLW","HPCS-ALW","HPDS-ALW","HPES-ALW","HPAS-AMW","HPAS-BMW","HPAS-CMW","HPAS-DMW","HPAS-EMW","HPAS-FMW","HPBS-AMW","HPBS-GMW","HPCS-AMW","HPDS-AMW","HPES-AMW",
"HPAS-ANW","HPAS-BNW","HPAS-CNW","HPAS-DNW","HPAS-ENW","HPAS-FNW","HPBS-ANW","HPBS-GNW","HPCS-ANW","HPDS-ANW","HPES-ANW","HPAS-AOW","HPAS-BOW","HPAS-COW","HPAS-DOW","HPAS-EOW","HPAS-FOW","HPBS-AOW","HPBS-GOW","HPCS-AOW",
"HPDS-AOW","HPES-AOW","HPAS-APW","HPAS-BPW","HPAS-CPW","HPAS-DPW","HPAS-EPW","HPAS-FPW","HPBS-APW","HPBS-GPW","HPCS-APW","HPDS-APW","HPES-APW","HPAS-AQW","HPAS-BQW","HPAS-CQW","HPAS-DQW","HPAS-EQW","HPAS-FQW","HPBS-AQW",
"HPBS-GQW","HPCS-AQW","HPDS-AQW","HPES-AQW","HPAS-ARW","HPAS-BRW","HPAS-CRW","HPAS-DRW","HPAS-ERW","HPAS-FRW","HPBS-ARW","HPBS-GRW","HPCS-ARW","HPDS-ARW","HPES-ARW","HPAS-ASW","HPAS-BSW","HPAS-CSW","HPAS-DSW","HPAS-ESW",
"HPAS-FSW","HPBS-ASW","HPBS-GSW","HPCS-ASW","HPDS-ASW","HPES-ASW","HPAS-ATW","HPAS-BTW","HPAS-CTW","HPAS-DTW","HPAS-ETW","HPAS-FTW","HPBS-ATW","HPBS-GTW","HPCS-ATW","HPDS-ATW","HPES-ATW","LPAN-AHU","LPAN-BHU","LPAN-CHU",
"LPAN-DHU","LPAN-EHU","LPAN-FHU","LPBN-AHU","LPBN-GHU","LPCN-AHU","LPDN-AHU","LPEN-AHU","LPAS-AIU","LPAS-BIU","LPAS-CIU","LPAS-DIU","LPAS-EIU","LPAS-FIU","LPBS-AIU","LPBS-GIU","LPCS-AIU","LPDS-AIU","LPES-AIU","LPAS-AJU",
"LPAS-BJU","LPAS-CJU","LPAS-DJU","LPAS-EJU","LPAS-FJU","LPBS-AJU","LPBS-GJU","LPCS-AJU","LPDS-AJU","LPES-AJU","LPAS-AKU","LPAS-BKU","LPAS-CKU","LPAS-DKU","LPAS-EKU","LPAS-FKU","LPBS-AKU","LPBS-GKU","LPCS-AKU","LPDS-AKU",
"LPES-AKU","LPAS-ALU","LPAS-BLU","LPAS-CLU","LPAS-DLU","LPAS-ELU","LPAS-FLU","LPBS-ALU","LPBS-GLU","LPCS-ALU","LPDS-ALU","LPES-ALU","LPAS-AMU","LPAS-BMU","LPAS-CMU","LPAS-DMU","LPAS-EMU","LPAS-FMU","LPBS-AMU","LPBS-GMU",
"LPCS-AMU","LPDS-AMU","LPES-AMU","LPAS-ANU","LPAS-BNU","LPAS-CNU","LPAS-DNU","LPAS-ENU","LPAS-FNU","LPBS-ANU","LPBS-GNU","LPCS-ANU","LPDS-ANU","LPES-ANU","LPAS-AOU","LPAS-BOU","LPAS-COU","LPAS-DOU","LPAS-EOU","LPAS-FOU",
"LPBS-AOU","LPBS-GOU","LPCS-AOU","LPDS-AOU","LPES-AOU","LPAS-APU","LPAS-BPU","LPAS-CPU","LPAS-DPU","LPAS-EPU","LPAS-FPU","LPBS-APU","LPBS-GPU","LPCS-APU","LPDS-APU","LPES-APU","LPAS-AQU","LPAS-BQU","LPAS-CQU","LPAS-DQU",
"LPAS-EQU","LPAS-FQU","LPBS-AQU","LPBS-GQU","LPCS-AQU","LPDS-AQU","LPES-AQU","LPAS-ARU","LPAS-BRU","LPAS-CRU","LPAS-DRU","LPAS-ERU","LPAS-FRU","LPBS-ARU","LPBS-GRU","LPCS-ARU","LPDS-ARU","LPES-ARU","LPAS-ASU","LPAS-BSU",
"LPAS-CSU","LPAS-DSU","LPAS-ESU","LPAS-FSU","LPBS-ASU","LPBS-GSU","LPCS-ASU","LPDS-ASU","LPES-ASU","LPAS-ATU","LPAS-BTU","LPAS-CTU","LPAS-DTU","LPAS-ETU","LPAS-FTU","LPBS-ATU","LPBS-GTU","LPCS-ATU","LPDS-ATU","LPES-ATU",
"LPAN-AHV","LPAN-BHV","LPAN-CHV","LPAN-DHV","LPAN-EHV","LPAN-FHV","LPBN-AHV","LPBN-GHV","LPCN-AHV","LPDN-AHV","LPEN-AHV","LPAS-AIV","LPAS-BIV","LPAS-CIV","LPAS-DIV","LPAS-EIV","LPAS-FIV","LPBS-AIV","LPBS-GIV","LPCS-AIV",
"LPDS-AIV","LPES-AIV","LPAS-AJV","LPAS-BJV","LPAS-CJV","LPAS-DJV","LPAS-EJV","LPAS-FJV","LPBS-AJV","LPBS-GJV","LPCS-AJV","LPDS-AJV","LPES-AJV","LPAS-AKV","LPAS-BKV","LPAS-CKV","LPAS-DKV","LPAS-EKV","LPAS-FKV","LPBS-AKV",
"LPBS-GKV","LPCS-AKV","LPDS-AKV","LPES-AKV","LPAS-ALV","LPAS-BLV","LPAS-CLV","LPAS-DLV","LPAS-ELV","LPAS-FLV","LPBS-ALV","LPBS-GLV","LPCS-ALV","LPDS-ALV","LPES-ALV","LPAS-AMV","LPAS-BMV","LPAS-CMV","LPAS-DMV","LPAS-EMV",
"LPAS-FMV","LPBS-AMV","LPBS-GMV","LPCS-AMV","LPDS-AMV","LPES-AMV","LPAS-ANV","LPAS-BNV","LPAS-CNV","LPAS-DNV","LPAS-ENV","LPAS-FNV","LPBS-ANV","LPBS-GNV","LPCS-ANV","LPDS-ANV","LPES-ANV","LPAS-AOV","LPAS-BOV","LPAS-COV",
"LPAS-DOV","LPAS-EOV","LPAS-FOV","LPBS-AOV","LPBS-GOV","LPCS-AOV","LPDS-AOV","LPES-AOV","LPAS-APV","LPAS-BPV","LPAS-CPV","LPAS-DPV","LPAS-EPV","LPAS-FPV","LPBS-APV","LPBS-GPV","LPCS-APV","LPDS-APV","LPES-APV","LPAS-AQV",
"LPAS-BQV","LPAS-CQV","LPAS-DQV","LPAS-EQV","LPAS-FQV","LPBS-AQV","LPBS-GQV","LPCS-AQV","LPDS-AQV","LPES-AQV","LPAS-ARV","LPAS-BRV","LPAS-CRV","LPAS-DRV","LPAS-ERV","LPAS-FRV","LPBS-ARV","LPBS-GRV","LPCS-ARV","LPDS-ARV",
"LPES-ARV","LPAS-ASV","LPAS-BSV","LPAS-CSV","LPAS-DSV","LPAS-ESV","LPAS-FSV","LPBS-ASV","LPBS-GSV","LPCS-ASV","LPDS-ASV","LPES-ASV","LPAS-ATV","LPAS-BTV","LPAS-CTV","LPAS-DTV","LPAS-ETV","LPAS-FTV","LPBS-ATV","LPBS-GTV",
"LPCS-ATV","LPDS-ATV","LPES-ATV","LPAN-AHW","LPAN-BHW","LPAN-CHW","LPAN-DHW","LPAN-EHW","LPAN-FHW","LPBN-AHW","LPBN-GHW","LPCN-AHW","LPDN-AHW","LPEN-AHW","LPAS-AIW","LPAS-BIW","LPAS-CIW","LPAS-DIW","LPAS-EIW","LPAS-FIW",
"LPBS-AIW","LPBS-GIW","LPCS-AIW","LPDS-AIW","LPES-AIW","LPAS-AJW","LPAS-BJW","LPAS-CJW","LPAS-DJW","LPAS-EJW","LPAS-FJW","LPBS-AJW","LPBS-GJW","LPCS-AJW","LPDS-AJW","LPES-AJW","LPAS-AKW","LPAS-BKW","LPAS-CKW","LPAS-DKW",
"LPAS-EKW","LPAS-FKW","LPBS-AKW","LPBS-GKW","LPCS-AKW","LPDS-AKW","LPES-AKW","LPAS-ALW","LPAS-BLW","LPAS-CLW","LPAS-DLW","LPAS-ELW","LPAS-FLW","LPBS-ALW","LPBS-GLW","LPCS-ALW","LPDS-ALW","LPES-ALW","LPAS-AMW","LPAS-BMW",
"LPAS-CMW","LPAS-DMW","LPAS-EMW","LPAS-FMW","LPBS-AMW","LPBS-GMW","LPCS-AMW","LPDS-AMW","LPES-AMW","LPAS-ANW","LPAS-BNW","LPAS-CNW","LPAS-DNW","LPAS-ENW","LPAS-FNW","LPBS-ANW","LPBS-GNW","LPCS-ANW","LPDS-ANW","LPES-ANW",
"LPAS-AOW","LPAS-BOW","LPAS-COW","LPAS-DOW","LPAS-EOW","LPAS-FOW","LPBS-AOW","LPBS-GOW","LPCS-AOW","LPDS-AOW","LPES-AOW","LPAS-APW","LPAS-BPW","LPAS-CPW","LPAS-DPW","LPAS-EPW","LPAS-FPW","LPBS-APW","LPBS-GPW","LPCS-APW",
"LPDS-APW","LPES-APW","LPAS-AQW","LPAS-BQW","LPAS-CQW","LPAS-DQW","LPAS-EQW","LPAS-FQW","LPBS-AQW","LPBS-GQW","LPCS-AQW","LPDS-AQW","LPES-AQW","LPAS-ARW","LPAS-BRW","LPAS-CRW","LPAS-DRW","LPAS-ERW","LPAS-FRW","LPBS-ARW",
"LPBS-GRW","LPCS-ARW","LPDS-ARW","LPES-ARW","LPAS-ASW","LPAS-BSW","LPAS-CSW","LPAS-DSW","LPAS-ESW","LPAS-FSW","LPBS-ASW","LPBS-GSW","LPCS-ASW","LPDS-ASW","LPES-ASW","LPAS-ATW","LPAS-BTW","LPAS-CTW","LPAS-DTW","LPAS-ETW",
"LPAS-FTW","LPBS-ATW","LPBS-GTW","LPCS-ATW","LPDS-ATW","LPES-ATW","WBAN-AHU","WBAN-BHU","WBAN-CHU","WBAN-DHU","WBAN-FHU","WBBN-AHU","WBBN-GHU","WBCN-AHU","WBDN-AHU","WBEN-AHU","WBAS-AIU","WBAS-BIU","WBAS-CIU","WBAS-DIU",
"WBAS-FIU","WBBS-AIU","WBBS-GIU","WBCS-AIU","WBDS-AIU","WBES-AIU","WBAS-AJU","WBAS-BJU","WBAS-CJU","WBAS-DJU","WBAS-FJU","WBBS-AJU","WBBS-GJU","WBCS-AJU","WBDS-AJU","WBES-AJU","WBAS-AKU","WBAS-BKU","WBAS-CKU","WBAS-DKU",
"WBAS-FKU","WBBS-AKU","WBBS-GKU","WBCS-AKU","WBDS-AKU","WBES-AKU","WBAS-ALU","WBAS-BLU","WBAS-CLU","WBAS-DLU","WBAS-FLU","WBBS-ALU","WBBS-GLU","WBCS-ALU","WBDS-ALU","WBES-ALU","WBAS-AMU","WBAS-BMU","WBAS-CMU","WBAS-DMU",
"WBAS-FMU","WBBS-AMU","WBBS-GMU","WBCS-AMU","WBDS-AMU","WBES-AMU","WBAS-ANU","WBAS-BNU","WBAS-CNU","WBAS-DNU","WBAS-FNU","WBBS-ANU","WBBS-GNU","WBCS-ANU","WBDS-ANU","WBES-ANU","WBAS-AOU","WBAS-BOU","WBAS-COU","WBAS-DOU",
"WBAS-FOU","WBBS-AOU","WBBS-GOU","WBCS-AOU","WBDS-AOU","WBES-AOU","WBAS-APU","WBAS-BPU","WBAS-CPU","WBAS-DPU","WBAS-FPU","WBBS-APU","WBBS-GPU","WBCS-APU","WBDS-APU","WBES-APU","WBAS-AQU","WBAS-BQU","WBAS-CQU","WBAS-DQU",
"WBAS-FQU","WBBS-AQU","WBBS-GQU","WBCS-AQU","WBDS-AQU","WBES-AQU","WBAS-ARU","WBAS-BRU","WBAS-CRU","WBAS-DRU","WBAS-FRU","WBBS-ARU","WBBS-GRU","WBCS-ARU","WBDS-ARU","WBES-ARU","WBAS-ASU","WBAS-BSU","WBAS-CSU","WBAS-DSU",
"WBAS-FSU","WBBS-ASU","WBBS-GSU","WBCS-ASU","WBDS-ASU","WBES-ASU","WBAS-ATU","WBAS-BTU","WBAS-CTU","WBAS-DTU","WBAS-FTU","WBBS-ATU","WBBS-GTU","WBCS-ATU","WBDS-ATU","WBES-ATU","WBAN-AHV","WBAN-BHV","WBAN-CHV","WBAN-DHV",
"WBAN-FHV","WBBN-AHV","WBBN-GHV","WBCN-AHV","WBDN-AHV","WBEN-AHV","WBAS-AIV","WBAS-BIV","WBAS-CIV","WBAS-DIV","WBAS-FIV","WBBS-AIV","WBBS-GIV","WBCS-AIV","WBDS-AIV","WBES-AIV","WBAS-AJV","WBAS-BJV","WBAS-CJV","WBAS-DJV",
"WBAS-FJV","WBBS-AJV","WBBS-GJV","WBCS-AJV","WBDS-AJV","WBES-AJV","WBAS-AKV","WBAS-BKV","WBAS-CKV","WBAS-DKV","WBAS-FKV","WBBS-AKV","WBBS-GKV","WBCS-AKV","WBDS-AKV","WBES-AKV","WBAS-ALV","WBAS-BLV","WBAS-CLV","WBAS-DLV",
"WBAS-FLV","WBBS-ALV","WBBS-GLV","WBCS-ALV","WBDS-ALV","WBES-ALV","WBAS-AMV","WBAS-BMV","WBAS-CMV","WBAS-DMV","WBAS-FMV","WBBS-AMV","WBBS-GMV","WBCS-AMV","WBDS-AMV","WBES-AMV","WBAS-ANV","WBAS-BNV","WBAS-CNV","WBAS-DNV",
"WBAS-FNV","WBBS-ANV","WBBS-GNV","WBCS-ANV","WBDS-ANV","WBES-ANV","WBAS-AOV","WBAS-BOV","WBAS-COV","WBAS-DOV","WBAS-FOV","WBBS-AOV","WBBS-GOV","WBCS-AOV","WBDS-AOV","WBES-AOV","WBAS-APV","WBAS-BPV","WBAS-CPV","WBAS-DPV",
"WBAS-FPV","WBBS-APV","WBBS-GPV","WBCS-APV","WBDS-APV","WBES-APV","WBAS-AQV","WBAS-BQV","WBAS-CQV","WBAS-DQV","WBAS-FQV","WBBS-AQV","WBBS-GQV","WBCS-AQV","WBDS-AQV","WBES-AQV","WBAS-ARV","WBAS-BRV","WBAS-CRV","WBAS-DRV",
"WBAS-FRV","WBBS-ARV","WBBS-GRV","WBCS-ARV","WBDS-ARV","WBES-ARV","WBAS-ASV","WBAS-BSV","WBAS-CSV","WBAS-DSV","WBAS-FSV","WBBS-ASV","WBBS-GSV","WBCS-ASV","WBDS-ASV","WBES-ASV","WBAS-ATV","WBAS-BTV","WBAS-CTV","WBAS-DTV",
"WBAS-FTV","WBBS-ATV","WBBS-GTV","WBCS-ATV","WBDS-ATV","WBES-ATV","WBAN-AHW","WBAN-BHW","WBAN-CHW","WBAN-DHW","WBAN-FHW","WBBN-AHW","WBBN-GHW","WBCN-AHW","WBDN-AHW","WBEN-AHW","WBAS-AIW","WBAS-BIW","WBAS-CIW","WBAS-DIW",
"WBAS-FIW","WBBS-AIW","WBBS-GIW","WBCS-AIW","WBDS-AIW","WBES-AIW","WBAS-AJW","WBAS-BJW","WBAS-CJW","WBAS-DJW","WBAS-FJW","WBBS-AJW","WBBS-GJW","WBCS-AJW","WBDS-AJW","WBES-AJW","WBAS-AKW","WBAS-BKW","WBAS-CKW","WBAS-DKW",
"WBAS-FKW","WBBS-AKW","WBBS-GKW","WBCS-AKW","WBDS-AKW","WBES-AKW","WBAS-ALW","WBAS-BLW","WBAS-CLW","WBAS-DLW","WBAS-FLW","WBBS-ALW","WBBS-GLW","WBCS-ALW","WBDS-ALW","WBES-ALW","WBAS-AMW","WBAS-BMW","WBAS-CMW","WBAS-DMW",
"WBAS-FMW","WBBS-AMW","WBBS-GMW","WBCS-AMW","WBDS-AMW","WBES-AMW","WBAS-ANW","WBAS-BNW","WBAS-CNW","WBAS-DNW","WBAS-FNW","WBBS-ANW","WBBS-GNW","WBCS-ANW","WBDS-ANW","WBES-ANW","WBAS-AOW","WBAS-BOW","WBAS-COW","WBAS-DOW",
"WBAS-FOW","WBBS-AOW","WBBS-GOW","WBCS-AOW","WBDS-AOW","WBES-AOW","WBAS-APW","WBAS-BPW","WBAS-CPW","WBAS-DPW","WBAS-FPW","WBBS-APW","WBBS-GPW","WBCS-APW","WBDS-APW","WBES-APW","WBAS-AQW","WBAS-BQW","WBAS-CQW","WBAS-DQW",
"WBAS-FQW","WBBS-AQW","WBBS-GQW","WBCS-AQW","WBDS-AQW","WBES-AQW","WBAS-ARW","WBAS-BRW","WBAS-CRW","WBAS-DRW","WBAS-FRW","WBBS-ARW","WBBS-GRW","WBCS-ARW","WBDS-ARW","WBES-ARW","WBAS-ASW","WBAS-BSW","WBAS-CSW","WBAS-DSW",
"WBAS-FSW","WBBS-ASW","WBBS-GSW","WBCS-ASW","WBDS-ASW","WBES-ASW","WBAS-ATW","WBAS-BTW","WBAS-CTW","WBAS-DTW","WBAS-FTW","WBBS-ATW","WBBS-GTW","WBCS-ATW","WBDS-ATW","WBES-ATW","HBAN-AHU","HBAN-BHU","HBAN-CHU","HBAN-DHU",
"HBAN-FHU","HBBN-AHU","HBBN-GHU","HBCN-AHU","HBDN-AHU","HBEN-AHU","HBAS-AIU","HBAS-BIU","HBAS-CIU","HBAS-DIU","HBAS-FIU","HBBS-AIU","HBBS-GIU","HBCS-AIU","HBDS-AIU","HBES-AIU","HBAS-AJU","HBAS-BJU","HBAS-CJU","HBAS-DJU",
"HBAS-FJU","HBBS-AJU","HBBS-GJU","HBCS-AJU","HBDS-AJU","HBES-AJU","HBAS-AKU","HBAS-BKU","HBAS-CKU","HBAS-DKU","HBAS-FKU","HBBS-AKU","HBBS-GKU","HBCS-AKU","HBDS-AKU","HBES-AKU","HBAS-ALU","HBAS-BLU","HBAS-CLU","HBAS-DLU",
"HBAS-FLU","HBBS-ALU","HBBS-GLU","HBCS-ALU","HBDS-ALU","HBES-ALU","HBAS-AMU","HBAS-BMU","HBAS-CMU","HBAS-DMU","HBAS-FMU","HBBS-AMU","HBBS-GMU","HBCS-AMU","HBDS-AMU","HBES-AMU","HBAS-ANU","HBAS-BNU","HBAS-CNU","HBAS-DNU",
"HBAS-FNU","HBBS-ANU","HBBS-GNU","HBCS-ANU","HBDS-ANU","HBES-ANU","HBAS-AOU","HBAS-BOU","HBAS-COU","HBAS-DOU","HBAS-FOU","HBBS-AOU","HBBS-GOU","HBCS-AOU","HBDS-AOU","HBES-AOU","HBAS-APU","HBAS-BPU","HBAS-CPU","HBAS-DPU",
"HBAS-FPU","HBBS-APU","HBBS-GPU","HBCS-APU","HBDS-APU","HBES-APU","HBAS-AQU","HBAS-BQU","HBAS-CQU","HBAS-DQU","HBAS-FQU","HBBS-AQU","HBBS-GQU","HBCS-AQU","HBDS-AQU","HBES-AQU","HBAS-ARU","HBAS-BRU","HBAS-CRU","HBAS-DRU",
"HBAS-FRU","HBBS-ARU","HBBS-GRU","HBCS-ARU","HBDS-ARU","HBES-ARU","HBAS-ASU","HBAS-BSU","HBAS-CSU","HBAS-DSU","HBAS-FSU","HBBS-ASU","HBBS-GSU","HBCS-ASU","HBDS-ASU","HBES-ASU","HBAS-ATU","HBAS-BTU","HBAS-CTU","HBAS-DTU",
"HBAS-FTU","HBBS-ATU","HBBS-GTU","HBCS-ATU","HBDS-ATU","HBES-ATU","HBAN-AHV","HBAN-BHV","HBAN-CHV","HBAN-DHV","HBAN-FHV","HBBN-AHV","HBBN-GHV","HBCN-AHV","HBDN-AHV","HBEN-AHV","HBAS-AIV","HBAS-BIV","HBAS-CIV","HBAS-DIV",
"HBAS-FIV","HBBS-AIV","HBBS-GIV","HBCS-AIV","HBDS-AIV","HBES-AIV","HBAS-AJV","HBAS-BJV","HBAS-CJV","HBAS-DJV","HBAS-FJV","HBBS-AJV","HBBS-GJV","HBCS-AJV","HBDS-AJV","HBES-AJV","HBAS-AKV","HBAS-BKV","HBAS-CKV","HBAS-DKV",
"HBAS-FKV","HBBS-AKV","HBBS-GKV","HBCS-AKV","HBDS-AKV","HBES-AKV","HBAS-ALV","HBAS-BLV","HBAS-CLV","HBAS-DLV","HBAS-FLV","HBBS-ALV","HBBS-GLV","HBCS-ALV","HBDS-ALV","HBES-ALV","HBAS-AMV","HBAS-BMV","HBAS-CMV","HBAS-DMV",
"HBAS-FMV","HBBS-AMV","HBBS-GMV","HBCS-AMV","HBDS-AMV","HBES-AMV","HBAS-ANV","HBAS-BNV","HBAS-CNV","HBAS-DNV","HBAS-FNV","HBBS-ANV","HBBS-GNV","HBCS-ANV","HBDS-ANV","HBES-ANV","HBAS-AOV","HBAS-BOV","HBAS-COV","HBAS-DOV",
"HBAS-FOV","HBBS-AOV","HBBS-GOV","HBCS-AOV","HBDS-AOV","HBES-AOV","HBAS-APV","HBAS-BPV","HBAS-CPV","HBAS-DPV","HBAS-FPV","HBBS-APV","HBBS-GPV","HBCS-APV","HBDS-APV","HBES-APV","HBAS-AQV","HBAS-BQV","HBAS-CQV","HBAS-DQV",
"HBAS-FQV","HBBS-AQV","HBBS-GQV","HBCS-AQV","HBDS-AQV","HBES-AQV","HBAS-ARV","HBAS-BRV","HBAS-CRV","HBAS-DRV","HBAS-FRV","HBBS-ARV","HBBS-GRV","HBCS-ARV","HBDS-ARV","HBES-ARV","HBAS-ASV","HBAS-BSV","HBAS-CSV","HBAS-DSV",
"HBAS-FSV","HBBS-ASV","HBBS-GSV","HBCS-ASV","HBDS-ASV","HBES-ASV","HBAS-ATV","HBAS-BTV","HBAS-CTV","HBAS-DTV","HBAS-FTV","HBBS-ATV","HBBS-GTV","HBCS-ATV","HBDS-ATV","HBES-ATV","HBAN-AHW","HBAN-BHW","HBAN-CHW","HBAN-DHW",
"HBAN-FHW","HBBN-AHW","HBBN-GHW","HBCN-AHW","HBDN-AHW","HBEN-AHW","HBAS-AIW","HBAS-BIW","HBAS-CIW","HBAS-DIW","HBAS-FIW","HBBS-AIW","HBBS-GIW","HBCS-AIW","HBDS-AIW","HBES-AIW","HBAS-AJW","HBAS-BJW","HBAS-CJW","HBAS-DJW",
"HBAS-FJW","HBBS-AJW","HBBS-GJW","HBCS-AJW","HBDS-AJW","HBES-AJW","HBAS-AKW","HBAS-BKW","HBAS-CKW","HBAS-DKW","HBAS-FKW","HBBS-AKW","HBBS-GKW","HBCS-AKW","HBDS-AKW","HBES-AKW","HBAS-ALW","HBAS-BLW","HBAS-CLW","HBAS-DLW",
"HBAS-FLW","HBBS-ALW","HBBS-GLW","HBCS-ALW","HBDS-ALW","HBES-ALW","HBAS-AMW","HBAS-BMW","HBAS-CMW","HBAS-DMW","HBAS-FMW","HBBS-AMW","HBBS-GMW","HBCS-AMW","HBDS-AMW","HBES-AMW","HBAS-ANW","HBAS-BNW","HBAS-CNW","HBAS-DNW",
"HBAS-FNW","HBBS-ANW","HBBS-GNW","HBCS-ANW","HBDS-ANW","HBES-ANW","HBAS-AOW","HBAS-BOW","HBAS-COW","HBAS-DOW","HBAS-FOW","HBBS-AOW","HBBS-GOW","HBCS-AOW","HBDS-AOW","HBES-AOW","HBAS-APW","HBAS-BPW","HBAS-CPW","HBAS-DPW",
"HBAS-FPW","HBBS-APW","HBBS-GPW","HBCS-APW","HBDS-APW","HBES-APW","HBAS-AQW","HBAS-BQW","HBAS-CQW","HBAS-DQW","HBAS-FQW","HBBS-AQW","HBBS-GQW","HBCS-AQW","HBDS-AQW","HBES-AQW","HBAS-ARW","HBAS-BRW","HBAS-CRW","HBAS-DRW",
"HBAS-FRW","HBBS-ARW","HBBS-GRW","HBCS-ARW","HBDS-ARW","HBES-ARW","HBAS-ASW","HBAS-BSW","HBAS-CSW","HBAS-DSW","HBAS-FSW","HBBS-ASW","HBBS-GSW","HBCS-ASW","HBDS-ASW","HBES-ASW","HBAS-ATW","HBAS-BTW","HBAS-CTW","HBAS-DTW",
"HBAS-FTW","HBBS-ATW","HBBS-GTW","HBCS-ATW","HBDS-ATW","HBES-ATW","LBAN-AHU","LBAN-BHU","LBAN-CHU","LBAN-DHU","LBAN-FHU","LBBN-AHU","LBBN-GHU","LBCN-AHU","LBDN-AHU","LBEN-AHU","LBAS-AIU","LBAS-BIU","LBAS-CIU","LBAS-DIU",
"LBAS-FIU","LBBS-AIU","LBBS-GIU","LBCS-AIU","LBDS-AIU","LBES-AIU","LBAS-AJU","LBAS-BJU","LBAS-CJU","LBAS-DJU","LBAS-FJU","LBBS-AJU","LBBS-GJU","LBCS-AJU","LBDS-AJU","LBES-AJU","LBAS-AKU","LBAS-BKU","LBAS-CKU","LBAS-DKU",
"LBAS-FKU","LBBS-AKU","LBBS-GKU","LBCS-AKU","LBDS-AKU","LBES-AKU","LBAS-ALU","LBAS-BLU","LBAS-CLU","LBAS-DLU","LBAS-FLU","LBBS-ALU","LBBS-GLU","LBCS-ALU","LBDS-ALU","LBES-ALU","LBAS-AMU","LBAS-BMU","LBAS-CMU","LBAS-DMU",
"LBAS-FMU","LBBS-AMU","LBBS-GMU","LBCS-AMU","LBDS-AMU","LBES-AMU","LBAS-ANU","LBAS-BNU","LBAS-CNU","LBAS-DNU","LBAS-FNU","LBBS-ANU","LBBS-GNU","LBCS-ANU","LBDS-ANU","LBES-ANU","LBAS-AOU","LBAS-BOU","LBAS-COU","LBAS-DOU",
"LBAS-FOU","LBBS-AOU","LBBS-GOU","LBCS-AOU","LBDS-AOU","LBES-AOU","LBAS-APU","LBAS-BPU","LBAS-CPU","LBAS-DPU","LBAS-FPU","LBBS-APU","LBBS-GPU","LBCS-APU","LBDS-APU","LBES-APU","LBAS-AQU","LBAS-BQU","LBAS-CQU","LBAS-DQU",
"LBAS-FQU","LBBS-AQU","LBBS-GQU","LBCS-AQU","LBDS-AQU","LBES-AQU","LBAS-ARU","LBAS-BRU","LBAS-CRU","LBAS-DRU","LBAS-FRU","LBBS-ARU","LBBS-GRU","LBCS-ARU","LBDS-ARU","LBES-ARU","LBAS-ASU","LBAS-BSU","LBAS-CSU","LBAS-DSU",
"LBAS-FSU","LBBS-ASU","LBBS-GSU","LBCS-ASU","LBDS-ASU","LBES-ASU","LBAS-ATU","LBAS-BTU","LBAS-CTU","LBAS-DTU","LBAS-FTU","LBBS-ATU","LBBS-GTU","LBCS-ATU","LBDS-ATU","LBES-ATU","LBAN-AHV","LBAN-BHV","LBAN-CHV","LBAN-DHV",
"LBAN-FHV","LBBN-AHV","LBBN-GHV","LBCN-AHV","LBDN-AHV","LBEN-AHV","LBAS-AIV","LBAS-BIV","LBAS-CIV","LBAS-DIV","LBAS-FIV","LBBS-AIV","LBBS-GIV","LBCS-AIV","LBDS-AIV","LBES-AIV","LBAS-AJV","LBAS-BJV","LBAS-CJV","LBAS-DJV",
"LBAS-FJV","LBBS-AJV","LBBS-GJV","LBCS-AJV","LBDS-AJV","LBES-AJV","LBAS-AKV","LBAS-BKV","LBAS-CKV","LBAS-DKV","LBAS-FKV","LBBS-AKV","LBBS-GKV","LBCS-AKV","LBDS-AKV","LBES-AKV","LBAS-ALV","LBAS-BLV","LBAS-CLV","LBAS-DLV",
"LBAS-FLV","LBBS-ALV","LBBS-GLV","LBCS-ALV","LBDS-ALV","LBES-ALV","LBAS-AMV","LBAS-BMV","LBAS-CMV","LBAS-DMV","LBAS-FMV","LBBS-AMV","LBBS-GMV","LBCS-AMV","LBDS-AMV","LBES-AMV","LBAS-ANV","LBAS-BNV","LBAS-CNV","LBAS-DNV",
"LBAS-FNV","LBBS-ANV","LBBS-GNV","LBCS-ANV","LBDS-ANV","LBES-ANV","LBAS-AOV","LBAS-BOV","LBAS-COV","LBAS-DOV","LBAS-FOV","LBBS-AOV","LBBS-GOV","LBCS-AOV","LBDS-AOV","LBES-AOV","LBAS-APV","LBAS-BPV","LBAS-CPV","LBAS-DPV",
"LBAS-FPV","LBBS-APV","LBBS-GPV","LBCS-APV","LBDS-APV","LBES-APV","LBAS-AQV","LBAS-BQV","LBAS-CQV","LBAS-DQV","LBAS-FQV","LBBS-AQV","LBBS-GQV","LBCS-AQV","LBDS-AQV","LBES-AQV","LBAS-ARV","LBAS-BRV","LBAS-CRV","LBAS-DRV",
"LBAS-FRV","LBBS-ARV","LBBS-GRV","LBCS-ARV","LBDS-ARV","LBES-ARV","LBAS-ASV","LBAS-BSV","LBAS-CSV","LBAS-DSV","LBAS-FSV","LBBS-ASV","LBBS-GSV","LBCS-ASV","LBDS-ASV","LBES-ASV","LBAS-ATV","LBAS-BTV","LBAS-CTV","LBAS-DTV",
"LBAS-FTV","LBBS-ATV","LBBS-GTV","LBCS-ATV","LBDS-ATV","LBES-ATV","LBAN-AHW","LBAN-BHW","LBAN-CHW","LBAN-DHW","LBAN-FHW","LBBN-AHW","LBBN-GHW","LBCN-AHW","LBDN-AHW","LBEN-AHW","LBAS-AIW","LBAS-BIW","LBAS-CIW","LBAS-DIW",
"LBAS-FIW","LBBS-AIW","LBBS-GIW","LBCS-AIW","LBDS-AIW","LBES-AIW","LBAS-AJW","LBAS-BJW","LBAS-CJW","LBAS-DJW","LBAS-FJW","LBBS-AJW","LBBS-GJW","LBCS-AJW","LBDS-AJW","LBES-AJW","LBAS-AKW","LBAS-BKW","LBAS-CKW","LBAS-DKW",
"LBAS-FKW","LBBS-AKW","LBBS-GKW","LBCS-AKW","LBDS-AKW","LBES-AKW","LBAS-ALW","LBAS-BLW","LBAS-CLW","LBAS-DLW","LBAS-FLW","LBBS-ALW","LBBS-GLW","LBCS-ALW","LBDS-ALW","LBES-ALW","LBAS-AMW","LBAS-BMW","LBAS-CMW","LBAS-DMW",
"LBAS-FMW","LBBS-AMW","LBBS-GMW","LBCS-AMW","LBDS-AMW","LBES-AMW","LBAS-ANW","LBAS-BNW","LBAS-CNW","LBAS-DNW","LBAS-FNW","LBBS-ANW","LBBS-GNW","LBCS-ANW","LBDS-ANW","LBES-ANW","LBAS-AOW","LBAS-BOW","LBAS-COW","LBAS-DOW",
"LBAS-FOW","LBBS-AOW","LBBS-GOW","LBCS-AOW","LBDS-AOW","LBES-AOW","LBAS-APW","LBAS-BPW","LBAS-CPW","LBAS-DPW","LBAS-FPW","LBBS-APW","LBBS-GPW","LBCS-APW","LBDS-APW","LBES-APW","LBAS-AQW","LBAS-BQW","LBAS-CQW","LBAS-DQW",
"LBAS-FQW","LBBS-AQW","LBBS-GQW","LBCS-AQW","LBDS-AQW","LBES-AQW","LBAS-ARW","LBAS-BRW","LBAS-CRW","LBAS-DRW","LBAS-FRW","LBBS-ARW","LBBS-GRW","LBCS-ARW","LBDS-ARW","LBES-ARW","LBAS-ASW","LBAS-BSW","LBAS-CSW","LBAS-DSW",
"LBAS-FSW","LBBS-ASW","LBBS-GSW","LBCS-ASW","LBDS-ASW","LBES-ASW","LBAS-ATW","LBAS-BTW","LBAS-CTW","LBAS-DTW","LBAS-FTW","LBBS-ATW","LBBS-GTW","LBCS-ATW","LBDS-ATW","LBES-ATW","WPFN-AHU","WPGN-AHU","WPHN-AHU","WPFS-AIU",
"WPGS-AIU","WPHS-AIU","WPFS-AJU","WPGS-AJU","WPHS-AJU","WPFS-AKU","WPGS-AKU","WPHS-AKU","WPFS-ALU","WPGS-ALU","WPHS-ALU","WPFS-AMU","WPGS-AMU","WPHS-AMU","WPFS-ANU","WPGS-ANU","WPHS-ANU","WPFS-AOU","WPGS-AOU","WPHS-AOU",
"WPFS-APU","WPGS-APU","WPHS-APU","WPFS-AQU","WPGS-AQU","WPHS-AQU","WPFS-ARU","WPGS-ARU","WPHS-ARU","WPFS-ASU","WPGS-ASU","WPHS-ASU","WPFS-ATU","WPGS-ATU","WPHS-ATU","WPFN-AHV","WPGN-AHV","WPHN-AHV","WPFS-AIV","WPGS-AIV",
"WPHS-AIV","WPFS-AJV","WPGS-AJV","WPHS-AJV","WPFS-AKV","WPGS-AKV","WPHS-AKV","WPFS-ALV","WPGS-ALV","WPHS-ALV","WPFS-AMV","WPGS-AMV","WPHS-AMV","WPFS-ANV","WPGS-ANV","WPHS-ANV","WPFS-AOV","WPGS-AOV","WPHS-AOV","WPFS-APV",
"WPGS-APV","WPHS-APV","WPFS-AQV","WPGS-AQV","WPHS-AQV","WPFS-ARV","WPGS-ARV","WPHS-ARV","WPFS-ASV","WPGS-ASV","WPHS-ASV","WPFS-ATV","WPGS-ATV","WPHS-ATV","WPFN-AHW","WPGN-AHW","WPHN-AHW","WPFS-AIW","WPGS-AIW","WPHS-AIW",
"WPFS-AJW","WPGS-AJW","WPHS-AJW","WPFS-AKW","WPGS-AKW","WPHS-AKW","WPFS-ALW","WPGS-ALW","WPHS-ALW","WPFS-AMW","WPGS-AMW","WPHS-AMW","WPFS-ANW","WPGS-ANW","WPHS-ANW","WPFS-AOW","WPGS-AOW","WPHS-AOW","WPFS-APW","WPGS-APW",
"WPHS-APW","WPFS-AQW","WPGS-AQW","WPHS-AQW","WPFS-ARW","WPGS-ARW","WPHS-ARW","WPFS-ASW","WPGS-ASW","WPHS-ASW","WPFS-ATW","WPGS-ATW","WPHS-ATW","HPFN-AHU","HPGN-AHU","HPHN-AHU","HPFS-AIU","HPGS-AIU","HPHS-AIU","HPFS-AJU",
"HPGS-AJU","HPHS-AJU","HPFS-AKU","HPGS-AKU","HPHS-AKU","HPFS-ALU","HPGS-ALU","HPHS-ALU","HPFS-AMU","HPGS-AMU","HPHS-AMU","HPFS-ANU","HPGS-ANU","HPHS-ANU","HPFS-AOU","HPGS-AOU","HPHS-AOU","HPFS-APU","HPGS-APU","HPHS-APU",
"HPFS-AQU","HPGS-AQU","HPHS-AQU","HPFS-ARU","HPGS-ARU","HPHS-ARU","HPFS-ASU","HPGS-ASU","HPHS-ASU","HPFS-ATU","HPGS-ATU","HPHS-ATU","HPFN-AHV","HPGN-AHV","HPHN-AHV","HPFS-AIV","HPGS-AIV","HPHS-AIV","HPFS-AJV","HPGS-AJV",
"HPHS-AJV","HPFS-AKV","HPGS-AKV","HPHS-AKV","HPFS-ALV","HPGS-ALV","HPHS-ALV","HPFS-AMV","HPGS-AMV","HPHS-AMV","HPFS-ANV","HPGS-ANV","HPHS-ANV","HPFS-AOV","HPGS-AOV","HPHS-AOV","HPFS-APV","HPGS-APV","HPHS-APV","HPFS-AQV",
"HPGS-AQV","HPHS-AQV","HPFS-ARV","HPGS-ARV","HPHS-ARV","HPFS-ASV","HPGS-ASV","HPHS-ASV","HPFS-ATV","HPGS-ATV","HPHS-ATV","HPFN-AHW","HPGN-AHW","HPHN-AHW","HPFS-AIW","HPGS-AIW","HPHS-AIW","HPFS-AJW","HPGS-AJW","HPHS-AJW",
"HPFS-AKW","HPGS-AKW","HPHS-AKW","HPFS-ALW","HPGS-ALW","HPHS-ALW","HPFS-AMW","HPGS-AMW","HPHS-AMW","HPFS-ANW","HPGS-ANW","HPHS-ANW","HPFS-AOW","HPGS-AOW","HPHS-AOW","HPFS-APW","HPGS-APW","HPHS-APW","HPFS-AQW","HPGS-AQW",
"HPHS-AQW","HPFS-ARW","HPGS-ARW","HPHS-ARW","HPFS-ASW","HPGS-ASW","HPHS-ASW","HPFS-ATW","HPGS-ATW","HPHS-ATW","LPFN-AHU","LPGN-AHU","LPHN-AHU","LPFS-AIU","LPGS-AIU","LPHS-AIU","LPFS-AJU","LPGS-AJU","LPHS-AJU","LPFS-AKU",
"LPGS-AKU","LPHS-AKU","LPFS-ALU","LPGS-ALU","LPHS-ALU","LPFS-AMU","LPGS-AMU","LPHS-AMU","LPFS-ANU","LPGS-ANU","LPHS-ANU","LPFS-AOU","LPGS-AOU","LPHS-AOU","LPFS-APU","LPGS-APU","LPHS-APU","LPFS-AQU","LPGS-AQU","LPHS-AQU",
"LPFS-ARU","LPGS-ARU","LPHS-ARU","LPFS-ASU","LPGS-ASU","LPHS-ASU","LPFS-ATU","LPGS-ATU","LPHS-ATU","LPFN-AHV","LPGN-AHV","LPHN-AHV","LPFS-AIV","LPGS-AIV","LPHS-AIV","LPFS-AJV","LPGS-AJV","LPHS-AJV","LPFS-AKV","LPGS-AKV",
"LPHS-AKV","LPFS-ALV","LPGS-ALV","LPHS-ALV","LPFS-AMV","LPGS-AMV","LPHS-AMV","LPFS-ANV","LPGS-ANV","LPHS-ANV","LPFS-AOV","LPGS-AOV","LPHS-AOV","LPFS-APV","LPGS-APV","LPHS-APV","LPFS-AQV","LPGS-AQV","LPHS-AQV","LPFS-ARV",
"LPGS-ARV","LPHS-ARV","LPFS-ASV","LPGS-ASV","LPHS-ASV","LPFS-ATV","LPGS-ATV","LPHS-ATV","LPFN-AHW","LPGN-AHW","LPHN-AHW","LPFS-AIW","LPGS-AIW","LPHS-AIW","LPFS-AJW","LPGS-AJW","LPHS-AJW","LPFS-AKW","LPGS-AKW","LPHS-AKW",
"LPFS-ALW","LPGS-ALW","LPHS-ALW","LPFS-AMW","LPGS-AMW","LPHS-AMW","LPFS-ANW","LPGS-ANW","LPHS-ANW","LPFS-AOW","LPGS-AOW","LPHS-AOW","LPFS-APW","LPGS-APW","LPHS-APW","LPFS-AQW","LPGS-AQW","LPHS-AQW","LPFS-ARW","LPGS-ARW",
"LPHS-ARW","LPFS-ASW","LPGS-ASW","LPHS-ASW","LPFS-ATW","LPGS-ATW","LPHS-ATW","WBFN-AHU","WBGN-AHU","WBFS-AIU","WBGS-AIU","WBFS-AJU","WBGS-AJU","WBFS-AKU","WBGS-AKU","WBFS-ALU","WBGS-ALU","WBFS-AMU","WBGS-AMU","WBFS-ANU",
"WBGS-ANU","WBFS-AOU","WBGS-AOU","WBFS-APU","WBGS-APU","WBFS-AQU","WBGS-AQU","WBFS-ARU","WBGS-ARU","WBFS-ASU","WBGS-ASU","WBFS-ATU","WBGS-ATU","WBFN-AHV","WBGN-AHV","WBFS-AIV","WBGS-AIV","WBFS-AJV","WBGS-AJV","WBFS-AKV",
"WBGS-AKV","WBFS-ALV","WBGS-ALV","WBFS-AMV","WBGS-AMV","WBFS-ANV","WBGS-ANV","WBFS-AOV","WBGS-AOV","WBFS-APV","WBGS-APV","WBFS-AQV","WBGS-AQV","WBFS-ARV","WBGS-ARV","WBFS-ASV","WBGS-ASV","WBFS-ATV","WBGS-ATV","WBFN-AHW",
"WBGN-AHW","WBFS-AIW","WBGS-AIW","WBFS-AJW","WBGS-AJW","WBFS-AKW","WBGS-AKW","WBFS-ALW","WBGS-ALW","WBFS-AMW","WBGS-AMW","WBFS-ANW","WBGS-ANW","WBFS-AOW","WBGS-AOW","WBFS-APW","WBGS-APW","WBFS-AQW","WBGS-AQW","WBFS-ARW",
"WBGS-ARW","WBFS-ASW","WBGS-ASW","WBFS-ATW","WBGS-ATW","HBFN-AHU","HBGN-AHU","HBFS-AIU","HBGS-AIU","HBFS-AJU","HBGS-AJU","HBFS-AKU","HBGS-AKU","HBFS-ALU","HBGS-ALU","HBFS-AMU","HBGS-AMU","HBFS-ANU","HBGS-ANU","HBFS-AOU",
"HBGS-AOU","HBFS-APU","HBGS-APU","HBFS-AQU","HBGS-AQU","HBFS-ARU","HBGS-ARU","HBFS-ASU","HBGS-ASU","HBFS-ATU","HBGS-ATU","HBFN-AHV","HBGN-AHV","HBFS-AIV","HBGS-AIV","HBFS-AJV","HBGS-AJV","HBFS-AKV","HBGS-AKV","HBFS-ALV",
"HBGS-ALV","HBFS-AMV","HBGS-AMV","HBFS-ANV","HBGS-ANV","HBFS-AOV","HBGS-AOV","HBFS-APV","HBGS-APV","HBFS-AQV","HBGS-AQV","HBFS-ARV","HBGS-ARV","HBFS-ASV","HBGS-ASV","HBFS-ATV","HBGS-ATV","HBFN-AHW","HBGN-AHW","HBFS-AIW",
"HBGS-AIW","HBFS-AJW","HBGS-AJW","HBFS-AKW","HBGS-AKW","HBFS-ALW","HBGS-ALW","HBFS-AMW","HBGS-AMW","HBFS-ANW","HBGS-ANW","HBFS-AOW","HBGS-AOW","HBFS-APW","HBGS-APW","HBFS-AQW","HBGS-AQW","HBFS-ARW","HBGS-ARW","HBFS-ASW",
"HBGS-ASW","HBFS-ATW","HBGS-ATW","LBFN-AHU","LBGN-AHU","LBFS-AIU","LBGS-AIU","LBFS-AJU","LBGS-AJU","LBFS-AKU","LBGS-AKU","LBFS-ALU","LBGS-ALU","LBFS-AMU","LBGS-AMU","LBFS-ANU","LBGS-ANU","LBFS-AOU","LBGS-AOU","LBFS-APU",
"LBGS-APU","LBFS-AQU","LBGS-AQU","LBFS-ARU","LBGS-ARU","LBFS-ASU","LBGS-ASU","LBFS-ATU","LBGS-ATU","LBFN-AHV","LBGN-AHV","LBFS-AIV","LBGS-AIV","LBFS-AJV","LBGS-AJV","LBFS-AKV","LBGS-AKV","LBFS-ALV","LBGS-ALV","LBFS-AMV",
"LBGS-AMV","LBFS-ANV","LBGS-ANV","LBFS-AOV","LBGS-AOV","LBFS-APV","LBGS-APV","LBFS-AQV","LBGS-AQV","LBFS-ARV","LBGS-ARV","LBFS-ASV","LBGS-ASV","LBFS-ATV","LBGS-ATV","LBFN-AHW","LBGN-AHW","LBFS-AIW","LBGS-AIW","LBFS-AJW",
"LBGS-AJW","LBFS-AKW","LBGS-AKW","LBFS-ALW","LBGS-ALW","LBFS-AMW","LBGS-AMW","LBFS-ANW","LBGS-ANW","LBFS-AOW","LBGS-AOW","LBFS-APW","LBGS-APW","LBFS-AQW","LBGS-AQW","LBFS-ARW","LBGS-ARW","LBFS-ASW","LBGS-ASW","LBFS-ATW",
"LBGS-ATW","KPAN-AHU","KPAN-BHU","KPAN-CHU","KPAN-DHU","KPAN-EHU","KPAN-FHU","KPBN-AHU","KPBN-GHU","KPCN-AHU","KPDN-AHU","KPEN-AHU","KPFN-AHU","KPGN-AHU","KPHN-AHU","KPAS-AIU","KPAS-BIU","KPAS-CIU","KPAS-DIU","KPAS-EIU",
"KPAS-FIU","KPBS-AIU","KPBS-GIU","KPCS-AIU","KPDS-AIU","KPES-AIU","KPFS-AIU","KPGS-AIU","KPHS-AIU","KPAS-AJU","KPAS-BJU","KPAS-CJU","KPAS-DJU","KPAS-EJU","KPAS-FJU","KPBS-AJU","KPBS-GJU","KPCS-AJU","KPDS-AJU","KPES-AJU",
"KPFS-AJU","KPGS-AJU","KPHS-AJU","KPAS-AKU","KPAS-BKU","KPAS-CKU","KPAS-DKU","KPAS-EKU","KPAS-FKU","KPBS-AKU","KPBS-GKU","KPCS-AKU","KPDS-AKU","KPES-AKU","KPFS-AKU","KPGS-AKU","KPHS-AKU","KPAS-ALU","KPAS-BLU","KPAS-CLU",
"KPAS-DLU","KPAS-ELU","KPAS-FLU","KPBS-ALU","KPBS-GLU","KPCS-ALU","KPDS-ALU","KPES-ALU","KPFS-ALU","KPGS-ALU","KPHS-ALU","KPAS-AMU","KPAS-BMU","KPAS-CMU","KPAS-DMU","KPAS-EMU","KPAS-FMU","KPBS-AMU","KPBS-GMU","KPCS-AMU",
"KPDS-AMU","KPES-AMU","KPFS-AMU","KPGS-AMU","KPHS-AMU","KPAS-ANU","KPAS-BNU","KPAS-CNU","KPAS-DNU","KPAS-ENU","KPAS-FNU","KPBS-ANU","KPBS-GNU","KPCS-ANU","KPDS-ANU","KPES-ANU","KPFS-ANU","KPGS-ANU","KPHS-ANU","KPAS-AOU",
"KPAS-BOU","KPAS-COU","KPAS-DOU","KPAS-EOU","KPAS-FOU","KPBS-AOU","KPBS-GOU","KPCS-AOU","KPDS-AOU","KPES-AOU","KPFS-AOU","KPGS-AOU","KPHS-AOU","KPAS-APU","KPAS-BPU","KPAS-CPU","KPAS-DPU","KPAS-EPU","KPAS-FPU","KPBS-APU",
"KPBS-GPU","KPCS-APU","KPDS-APU","KPES-APU","KPFS-APU","KPGS-APU","KPHS-APU","KPAS-AQU","KPAS-BQU","KPAS-CQU","KPAS-DQU","KPAS-EQU","KPAS-FQU","KPBS-AQU","KPBS-GQU","KPCS-AQU","KPDS-AQU","KPES-AQU","KPFS-AQU","KPGS-AQU",
"KPHS-AQU","KPAS-ARU","KPAS-BRU","KPAS-CRU","KPAS-DRU","KPAS-ERU","KPAS-FRU","KPBS-ARU","KPBS-GRU","KPCS-ARU","KPDS-ARU","KPES-ARU","KPFS-ARU","KPGS-ARU","KPHS-ARU","KPAS-ASU","KPAS-BSU","KPAS-CSU","KPAS-DSU","KPAS-ESU",
"KPAS-FSU","KPBS-ASU","KPBS-GSU","KPCS-ASU","KPDS-ASU","KPES-ASU","KPFS-ASU","KPGS-ASU","KPHS-ASU","KPAS-ATU","KPAS-BTU","KPAS-CTU","KPAS-DTU","KPAS-ETU","KPAS-FTU","KPBS-ATU","KPBS-GTU","KPCS-ATU","KPDS-ATU","KPES-ATU",
"KPFS-ATU","KPGS-ATU","KPHS-ATU","CPAN-AHU","CPAN-BHU","CPAN-CHU","CPAN-DHU","CPAN-EHU","CPAN-FHU","CPBN-AHU","CPBN-GHU","CPCN-AHU","CPDN-AHU","CPEN-AHU","CPFN-AHU","CPGN-AHU","CPHN-AHU","CPAS-AIU","CPAS-BIU","CPAS-CIU",
"CPAS-DIU","CPAS-EIU","CPAS-FIU","CPBS-AIU","CPBS-GIU","CPCS-AIU","CPDS-AIU","CPES-AIU","CPFS-AIU","CPGS-AIU","CPHS-AIU","CPAS-AJU","CPAS-BJU","CPAS-CJU","CPAS-DJU","CPAS-EJU","CPAS-FJU","CPBS-AJU","CPBS-GJU","CPCS-AJU",
"CPDS-AJU","CPES-AJU","CPFS-AJU","CPGS-AJU","CPHS-AJU","CPAS-AKU","CPAS-BKU","CPAS-CKU","CPAS-DKU","CPAS-EKU","CPAS-FKU","CPBS-AKU","CPBS-GKU","CPCS-AKU","CPDS-AKU","CPES-AKU","CPFS-AKU","CPGS-AKU","CPHS-AKU","CPAS-ALU",
"CPAS-BLU","CPAS-CLU","CPAS-DLU","CPAS-ELU","CPAS-FLU","CPBS-ALU","CPBS-GLU","CPCS-ALU","CPDS-ALU","CPES-ALU","CPFS-ALU","CPGS-ALU","CPHS-ALU","CPAS-AMU","CPAS-BMU","CPAS-CMU","CPAS-DMU","CPAS-EMU","CPAS-FMU","CPBS-AMU",
"CPBS-GMU","CPCS-AMU","CPDS-AMU","CPES-AMU","CPFS-AMU","CPGS-AMU","CPHS-AMU","CPAS-ANU","CPAS-BNU","CPAS-CNU","CPAS-DNU","CPAS-ENU","CPAS-FNU","CPBS-ANU","CPBS-GNU","CPCS-ANU","CPDS-ANU","CPES-ANU","CPFS-ANU","CPGS-ANU",
"CPHS-ANU","CPAS-AOU","CPAS-BOU","CPAS-COU","CPAS-DOU","CPAS-EOU","CPAS-FOU","CPBS-AOU","CPBS-GOU","CPCS-AOU","CPDS-AOU","CPES-AOU","CPFS-AOU","CPGS-AOU","CPHS-AOU","CPAS-APU","CPAS-BPU","CPAS-CPU","CPAS-DPU","CPAS-EPU",
"CPAS-FPU","CPBS-APU","CPBS-GPU","CPCS-APU","CPDS-APU","CPES-APU","CPFS-APU","CPGS-APU","CPHS-APU","CPAS-AQU","CPAS-BQU","CPAS-CQU","CPAS-DQU","CPAS-EQU","CPAS-FQU","CPBS-AQU","CPBS-GQU","CPCS-AQU","CPDS-AQU","CPES-AQU",
"CPFS-AQU","CPGS-AQU","CPHS-AQU","CPAS-ARU","CPAS-BRU","CPAS-CRU","CPAS-DRU","CPAS-ERU","CPAS-FRU","CPBS-ARU","CPBS-GRU","CPCS-ARU","CPDS-ARU","CPES-ARU","CPFS-ARU","CPGS-ARU","CPHS-ARU","CPAS-ASU","CPAS-BSU","CPAS-CSU",
"CPAS-DSU","CPAS-ESU","CPAS-FSU","CPBS-ASU","CPBS-GSU","CPCS-ASU","CPDS-ASU","CPES-ASU","CPFS-ASU","CPGS-ASU","CPHS-ASU","CPAS-ATU","CPAS-BTU","CPAS-CTU","CPAS-DTU","CPAS-ETU","CPAS-FTU","CPBS-ATU","CPBS-GTU","CPCS-ATU",
"CPDS-ATU","CPES-ATU","CPFS-ATU","CPGS-ATU","CPHS-ATU","KBAN-AHU","KBAN-BHU","KBAN-CHU","KBAN-DHU","KBAN-FHU","KBBN-AHU","KBBN-GHU","KBCN-AHU","KBDN-AHU","KBEN-AHU","KBFN-AHU","KBGN-AHU","KBAS-AIU","KBAS-BIU","KBAS-CIU",
"KBAS-DIU","KBAS-FIU","KBBS-AIU","KBBS-GIU","KBCS-AIU","KBDS-AIU","KBES-AIU","KBFS-AIU","KBGS-AIU","KBAS-AJU","KBAS-BJU","KBAS-CJU","KBAS-DJU","KBAS-FJU","KBBS-AJU","KBBS-GJU","KBCS-AJU","KBDS-AJU","KBES-AJU","KBFS-AJU",
"KBGS-AJU","KBAS-AKU","KBAS-BKU","KBAS-CKU","KBAS-DKU","KBAS-FKU","KBBS-AKU","KBBS-GKU","KBCS-AKU","KBDS-AKU","KBES-AKU","KBFS-AKU","KBGS-AKU","KBAS-ALU","KBAS-BLU","KBAS-CLU","KBAS-DLU","KBAS-FLU","KBBS-ALU","KBBS-GLU",
"KBCS-ALU","KBDS-ALU","KBES-ALU","KBFS-ALU","KBGS-ALU","KBAS-AMU","KBAS-BMU","KBAS-CMU","KBAS-DMU","KBAS-FMU","KBBS-AMU","KBBS-GMU","KBCS-AMU","KBDS-AMU","KBES-AMU","KBFS-AMU","KBGS-AMU","KBAS-ANU","KBAS-BNU","KBAS-CNU",
"KBAS-DNU","KBAS-FNU","KBBS-ANU","KBBS-GNU","KBCS-ANU","KBDS-ANU","KBES-ANU","KBFS-ANU","KBGS-ANU","KBAS-AOU","KBAS-BOU","KBAS-COU","KBAS-DOU","KBAS-FOU","KBBS-AOU","KBBS-GOU","KBCS-AOU","KBDS-AOU","KBES-AOU","KBFS-AOU",
"KBGS-AOU","KBAS-APU","KBAS-BPU","KBAS-CPU","KBAS-DPU","KBAS-FPU","KBBS-APU","KBBS-GPU","KBCS-APU","KBDS-APU","KBES-APU","KBFS-APU","KBGS-APU","KBAS-AQU","KBAS-BQU","KBAS-CQU","KBAS-DQU","KBAS-FQU","KBBS-AQU","KBBS-GQU",
"KBCS-AQU","KBDS-AQU","KBES-AQU","KBFS-AQU","KBGS-AQU","KBAS-ARU","KBAS-BRU","KBAS-CRU","KBAS-DRU","KBAS-FRU","KBBS-ARU","KBBS-GRU","KBCS-ARU","KBDS-ARU","KBES-ARU","KBFS-ARU","KBGS-ARU","KBAS-ASU","KBAS-BSU","KBAS-CSU",
"KBAS-DSU","KBAS-FSU","KBBS-ASU","KBBS-GSU","KBCS-ASU","KBDS-ASU","KBES-ASU","KBFS-ASU","KBGS-ASU","KBAS-ATU","KBAS-BTU","KBAS-CTU","KBAS-DTU","KBAS-FTU","KBBS-ATU","KBBS-GTU","KBCS-ATU","KBDS-ATU","KBES-ATU","KBFS-ATU",
"KBGS-ATU","CBAN-AHU","CBAN-BHU","CBAN-CHU","CBAN-DHU","CBAN-FHU","CBBN-AHU","CBBN-GHU","CBCN-AHU","CBDN-AHU","CBEN-AHU","CBFN-AHU","CBGN-AHU","CBAS-AIU","CBAS-BIU","CBAS-CIU","CBAS-DIU","CBAS-FIU","CBBS-AIU","CBBS-GIU",
"CBCS-AIU","CBDS-AIU","CBES-AIU","CBFS-AIU","CBGS-AIU","CBAS-AJU","CBAS-BJU","CBAS-CJU","CBAS-DJU","CBAS-FJU","CBBS-AJU","CBBS-GJU","CBCS-AJU","CBDS-AJU","CBES-AJU","CBFS-AJU","CBGS-AJU","CBAS-AKU","CBAS-BKU","CBAS-CKU",
"CBAS-DKU","CBAS-FKU","CBBS-AKU","CBBS-GKU","CBCS-AKU","CBDS-AKU","CBES-AKU","CBFS-AKU","CBGS-AKU","CBAS-ALU","CBAS-BLU","CBAS-CLU","CBAS-DLU","CBAS-FLU","CBBS-ALU","CBBS-GLU","CBCS-ALU","CBDS-ALU","CBES-ALU","CBFS-ALU",
"CBGS-ALU","CBAS-AMU","CBAS-BMU","CBAS-CMU","CBAS-DMU","CBAS-FMU","CBBS-AMU","CBBS-GMU","CBCS-AMU","CBDS-AMU","CBES-AMU","CBFS-AMU","CBGS-AMU","CBAS-ANU","CBAS-BNU","CBAS-CNU","CBAS-DNU","CBAS-FNU","CBBS-ANU","CBBS-GNU",
"CBCS-ANU","CBDS-ANU","CBES-ANU","CBFS-ANU","CBGS-ANU","CBAS-AOU","CBAS-BOU","CBAS-COU","CBAS-DOU","CBAS-FOU","CBBS-AOU","CBBS-GOU","CBCS-AOU","CBDS-AOU","CBES-AOU","CBFS-AOU","CBGS-AOU","CBAS-APU","CBAS-BPU","CBAS-CPU",
"CBAS-DPU","CBAS-FPU","CBBS-APU","CBBS-GPU","CBCS-APU","CBDS-APU","CBES-APU","CBFS-APU","CBGS-APU","CBAS-AQU","CBAS-BQU","CBAS-CQU","CBAS-DQU","CBAS-FQU","CBBS-AQU","CBBS-GQU","CBCS-AQU","CBDS-AQU","CBES-AQU","CBFS-AQU",
"CBGS-AQU","CBAS-ARU","CBAS-BRU","CBAS-CRU","CBAS-DRU","CBAS-FRU","CBBS-ARU","CBBS-GRU","CBCS-ARU","CBDS-ARU","CBES-ARU","CBFS-ARU","CBGS-ARU","CBAS-ASU","CBAS-BSU","CBAS-CSU","CBAS-DSU","CBAS-FSU","CBBS-ASU","CBBS-GSU",
"CBCS-ASU","CBDS-ASU","CBES-ASU","CBFS-ASU","CBGS-ASU","CBAS-ATU","CBAS-BTU","CBAS-CTU","CBAS-DTU","CBAS-FTU","CBBS-ATU","CBBS-GTU","CBCS-ATU","CBDS-ATU","CBES-ATU","CBFS-ATU","CBGS-ATU"    
};

int num_pid2sku = (int)(sizeof( PID2SKU ) / sizeof( PID2SKU[0] ));