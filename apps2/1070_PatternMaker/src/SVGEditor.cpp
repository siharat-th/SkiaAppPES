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
#include "include/pathops/SkPathOps.h"
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



DecSVGEditor::DecSVGEditor() {
    nTargetGenerate = 0; // "กระโปรงชิ้นหน้าชิ้นหลังเท่ากัน" 
    bTargetGenerated = false;

    // Skirt Measure
    fWaistRoundLength = 0;        // รอบเอว
    fWaistSideDepth = 2.5;        // ลึกเอว (2.5)
    fBackDepth = 0;               // ลึกหลัง (1.5)
    fLowHipLength = 20.0;         // สะโพกล่าง (หญิง 20 ชาย 22)
    fTightLength = 0;             // เอวถึงหน้าขา
    fKneeLength = 0;              // เอวถึงเข่า
    fAnkleLength = 0;             // เอวถึงข้อเท้า
    fFloorLength = 0;             // เอวด้านข้างถึงพื้น
    fBackFloorLength = 0;         // เอวด้านหลังถึงพื้น
    fFrontFloorLength = 0;        // เอวด้านหน้าถึงพื้น
    fSkirtLength = 60;            // เอวถึงชายกระโปรง ตามลูกค้าต้องการ (60)
    fHipRoundLength = 0;          // รอบสะโพกล่าง
    fHipRoundWithBellyLength = 0; // รอบสะโพกล่างรวมพุง
    fHipRoundWithTightLength = 0; // รอบสะโพกล่างรวมหน้าขา
    fTightRoundLength = 0;        // รอบหน้าขา

    // Skirt Defined
    fSkirtLooseValue = 3;         // เผื่อหลวมกระโปรง (3)

    // Skirt Calculate
    // fSkirtWidth;

    nTargetPiece = 0;

    fNeckLineY = 17.0f;
    fWaistLineY = 0.0f;
    fCenterFrontLineX = 39.8f;

    fBustLineY = 0;
    fRealBustLineY = 0;
    fChestLineY = 0;
    fLowHipLineY = 0;
    fHighHipLineY = 0;
    fTopShoulderLineY = 0;
    fLowShoulderLineY = 0;

    fBustX = 0;
    fWaistX = 0;
    fHipX = 0;
    fShoulderX = 0;
    fChestX = 0;
    fBackX = 0;

    fWaistLength = 0;
    fRealBustLength = 0;
    fShoulderSeamLength = 0;

    fNeckRoundLength = 0;
    fNeckWidth = 0;
    fNeckDepth = 0;

    fBustRoundLength = 0;
    fWaistRoundLength = 0;

    fDom = NULL;
    root = NULL;
    _pTextDocumentPath = NULL;
    _pTextDocumentSize = NULL;
    _pPathNodeList = NULL;
    _pFileList = NULL;

    _is_S = false;
    _is_M = false;
    _is_L = false;
    _is_XL = false;
    _is_XXL = false;

    _generateFrame = 1;
    _generateCount = 0;
    _generatePattern = false;

    _forceBlack = true;
    _isTiledExport = true;

    //for( int i=0; i<5; i++ ) {
    //    _sewingLinePath[i] = NULL;
    //}
    //_editingSewingLine = NULL;
    //_SelectedSewingLinePath = NULL;
    //_selectedSewingLineIndex = -1;

    //contentBounds.setEmpty();

    size( -1, -1 );
    border( false );
    background( false );
    _( Column()
        .size( 360, -1 )
        .border(false)
        .background(false)
        .padding( 10, 10, 10, 10 )
        .gap( 10 )
        ._( Column()
            .border( false )
            .gap( 5 )
            .size( -1, 280 )
            .visible( false )
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
                        //loadSewingLine();

                        //SkDebugf( "Path: %s\n", _filepath.c_str() );
                        //SkDebugf( "Filename: %s\n", _filename.c_str() );
                        //SkDebugf( "Basename: %s\n", _filebasename.c_str() );
                    })
                )
            )
        )
        ._( Row()
            .border( false )
            .size( -1, 0 )
            ._( Stack()
                .size( -1, 0 )
                .border( false )
                .onMousePressed( [=]( DecWidget &w) -> bool {
                    _pPageMesurement->activate( 0 );
                    _pTextMesurement[0]->text_color( 0xFF0000FF );
                    _pTextMesurement[1]->text_color( 0xFF000000 );
                    return true;
                })
                ._(Text( "กระโปรง" )
                    .ref( (DecWidget **)&_pTextMesurement[0])
                    .text_color( 0xFF0000FF )
                    .align( ALIGN_CENTER )
                )
            )
            ._( Stack()
                .size( -1, 0 )
                .border( false )
                .onMousePressed( [=]( DecWidget &w) -> bool {
                    _pPageMesurement->activate( 1 );
                    _pTextMesurement[0]->text_color( 0xFF000000 );
                    _pTextMesurement[1]->text_color( 0xFF0000FF );
                    return true;
                })
                ._(Text( "เสื้อ")
                    .ref( (DecWidget **)&_pTextMesurement[1])
                    .align( ALIGN_CENTER )
                )
            )
        )
        ._( Pages()
            .ref( (DecWidget **)&_pPageMesurement )
            .border( false )
            .gap( 2 )
            .size( -1, -1 )      
            ._( Scroller()
                .size( -1, -1 )  
                ._( Column()
                    .border( false )
                    .gap( 2 )
                    .size( -1, 0 )     
                    ._(Text( "รอบเอว (ระดับคอดสุดด้านข้าง 1-2 ซ.ม.เหนือสะดือ)").margin( 10, 0, 0, 0 ) )
                    // รอบเอว คนผอม 1 ซ.ม.เหนือสะดือ, คนอ้วน 2 ซ.ม.เหนือสะดือ
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("90", 
                            [=](DecTextEdit& textedit) {
                                fWaistRoundLength = atoi(_pTEWaistRoundLength->text());
                                fWaistX = fCenterFrontLineX - ((fWaistRoundLength + 8) / 4);
                            }
                        ).ref( (DecWidget **)&_pTEWaistRoundLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEWaistRoundLength->_callback( *_pTEWaistRoundLength );
                            })
                            .size(30, 30)
                        )
                    )                        
                    ._(Text( "ลึกเอว (ระดับที่เอวด้านข้าคอด ปกติ 2.5 ซ.ม.)") )
                    // รอบเอว คนผอม 1 ซ.ม.เหนือสะดือ, คนอ้วน 2 ซ.ม.เหนือสะดือ
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("2.5", 
                            [=](DecTextEdit& textedit) {
                                fWaistSideDepth = atoi(_pTEWaistSideDepth->text());
                            }
                        ).ref( (DecWidget **)&_pTEWaistSideDepth).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEWaistSideDepth->_callback( *_pTEWaistSideDepth );
                            })
                            .size(30, 30)
                        )
                    )   
                    ._(Text( "ลึกหลัง (ปกติ 1.5-2.5 ซ.ม.)") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("1.5", 
                            [=](DecTextEdit& textedit) {
                                fBackDepth = atoi(_pTEBackDepth->text());
                            }
                        ).ref( (DecWidget **)&_pTEBackDepth).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEBackDepth->_callback( *_pTEBackDepth );
                            })
                            .size(30, 30)
                        )
                    )                                                                                    
                    ._(Text( "สะโพกล่าง (เอวถึงระดับยื่นสุดของก้น ไซส์ M 22 ซ.ม.)") ) // ปกติ 20 ซ.ม.
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("22", 
                            [=](DecTextEdit& textedit) {
                                fLowHipLength  = atoi(_pTELowHipLength->text());
                                fLowHipLineY = fWaistLineY + fLowHipLength;
                                fHighHipLineY = fWaistLineY + (fLowHipLength / 2 );
                            }
                        ).ref( (DecWidget **)&_pTELowHipLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTELowHipLength->_callback( *_pTELowHipLength );
                            })
                            .size(30, 30)
                        )
                    )    
                    ._(Text( "เอวถึงหน้าขา ") ) 
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("24", 
                            [=](DecTextEdit& textedit) {
                                fTightLength  = atoi(_pTEThighLength->text());
                            }
                        ).ref( (DecWidget **)&_pTEThighLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEThighLength->_callback( *_pTEThighLength );
                            })
                            .size(30, 30)
                        )
                    )    
                    ._(Text( "เอวถึงเข่า (ระดับข้อพับ)") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("51", 
                            [=](DecTextEdit& textedit) {
                                fKneeLength  = atoi(_pTEKneeLength->text());
                            }
                        ).ref( (DecWidget **)&_pTEKneeLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEKneeLength->_callback( *_pTEKneeLength );
                            })
                            .size(30, 30)
                        )
                    )                  
                    ._(Text( "เอวถึงข้อเท้า (ต่ำกว่าตาตุ่ม 2 ซ.ม.)") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("89", 
                            [=](DecTextEdit& textedit) {
                                fAnkleLength  = atoi(_pTEAnkleLength->text());
                            }
                        ).ref( (DecWidget **)&_pTEAnkleLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEAnkleLength->_callback( *_pTEAnkleLength );
                            })
                            .size(30, 30)
                        )
                    )  
                    ._(Text( "เอวด้านข้างถึงพื้น") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("92", 
                            [=](DecTextEdit& textedit) {
                                fFloorLength  = atoi(_pTEFloorLength->text());
                            }
                        ).ref( (DecWidget **)&_pTEFloorLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEFloorLength->_callback( *_pTEFloorLength );
                            })
                            .size(30, 30)
                        )
                    )
                    ._(Text( "ความยาวชายกระโปรง (ตามต้องการ)") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("60", // 56 
                            [=](DecTextEdit& textedit) {
                                fSkirtLength  = atoi(_pTESkirtLength->text());
                            }
                        ).ref( (DecWidget **)&_pTESkirtLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTESkirtLength->_callback( *_pTESkirtLength );
                            })
                            .size(30, 30)
                        )
                    )  
                    ._(Text( "เอวด้านหลังถึงพื้น") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("93", 
                            [=](DecTextEdit& textedit) {
                                fBackFloorLength  = atoi(_pTEBackFloorLength->text());
                            }
                        ).ref( (DecWidget **)&_pTEBackFloorLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEBackFloorLength->_callback( *_pTEBackFloorLength );
                            })
                            .size(30, 30)
                        )
                    )  
                    ._(Text( "เอวด้านหน้าถึงพื้น") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("88", 
                            [=](DecTextEdit& textedit) {
                                fFrontFloorLength  = atoi(_pTEFrontFloorLength->text());
                            }
                        ).ref( (DecWidget **)&_pTEFrontFloorLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEFrontFloorLength->_callback( *_pTEFrontFloorLength );
                            })
                            .size(30, 30)
                        )
                    )  
                    ._(Text( "รอบสะโพกล่าง (ระดับยื่นสุดของก้น)") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("105", 
                            [=](DecTextEdit& textedit) {
                                fHipRoundLength = atoi(_pTEHipRoundLength->text());
                                fHipX = fCenterFrontLineX - ((fHipRoundLength + 8) / 4);
                            }
                        ).ref( (DecWidget **)&_pTEHipRoundLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEHipRoundLength->_callback( *_pTEHipRoundLength );
                            })
                            .size(30, 30)
                        )
                    )   
                    ._(Text( "รอบสะโพกล่างรวมพุง") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("108", 
                            [=](DecTextEdit& textedit) {
                                fHipRoundWithBellyLength = atoi(_pTEHipRoundWithBellyLength->text());
                            }
                        ).ref( (DecWidget **)&_pTEHipRoundWithBellyLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEHipRoundWithBellyLength->_callback( *_pTEHipRoundWithBellyLength );
                            })
                            .size(30, 30)
                        )
                    )   
                    ._(Text( "รอบสะโพกล่างรวมหน้าขา") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("107", 
                            [=](DecTextEdit& textedit) {
                                fHipRoundWithTightLength = atoi(_pTEHipRoundWithTightLength->text());
                            }
                        ).ref( (DecWidget **)&_pTEHipRoundWithTightLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTEHipRoundWithTightLength->_callback( *_pTEHipRoundWithTightLength );
                            })
                            .size(30, 30)
                        )
                    )       
                    ._(Text( "รอบหน้าขา") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("95", 
                            [=](DecTextEdit& textedit) {
                                fTightRoundLength = atoi(_pTETightRoundLength->text());
                            }
                        ).ref( (DecWidget **)&_pTETightRoundLength).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTETightRoundLength->_callback( *_pTETightRoundLength );
                            })
                            .size(30, 30)
                        )
                    )   
                    ._(Text( "เผื่อหลวม") )
                    ._( Row()
                        .size(-1, 30)
                        .gap(4)
                        .border(false)
                        .background(false)
                        ._(TextEdit("3", 
                            [=](DecTextEdit& textedit) {
                                fSkirtLooseValue = atoi(_pTESkirtLooseValue->text());
                            }
                        ).ref( (DecWidget **)&_pTESkirtLooseValue).size(-1, 30))
                        ._( Button("", 0xe2e1,
                            [=](DecButton& btn) {
                                _pTESkirtLooseValue->_callback( *_pTESkirtLooseValue );
                            })
                            .size(30, 30)
                        )
                    )                       
                )                    
            )

            ._( Column()
                .border( false )
                .gap( 2 )
                .size( -1, -1 )        
                ._(Text( "ยาวหน้า หรือ เอว (ปุ่มคอหน้า ถึง สะดือ -1 ถีง 3 cm)").margin( 10, 0, 0, 0 ) )
                ._( Row()
                    .size(-1, 30)
                    .gap(4)
                    .border(false)
                    .background(false)
                    ._(TextEdit("", 
                        [=](DecTextEdit& textedit) {
                            SkDebugf("TextEdit ID:%d, %s\n", textedit.id, textedit.text());
                            //_pTextEditXX->setText(textedit.text());

                            fWaistLength = atoi(textedit.text());
                            SkDebugf( "Waist: %0.2f\n", fWaistLength );
                            fWaistLineY = fNeckLineY + fWaistLength;
                            fBustLineY = fNeckLineY + (fWaistLength / 2);
                            fChestLineY = fNeckLineY + (fWaistLength / 4);

                            if( fRealBustLength == 0 ) {
                                fRealBustLength = fWaistLength / 2;
                                fRealBustLineY = fNeckLineY + fRealBustLength;

                                char sz[32];
                                sprintf( sz, "%0.2f", fRealBustLength );
                                
                                while( sz[strlen(sz)-1] == '0' ) {
                                    sz[strlen(sz)-1] = 0;
                                }
                                if( sz[strlen(sz)-1] == '.' ) {
                                    sz[strlen(sz)-1] = 0;
                                } 
                                                        
                                _pTEBustLength->setText(sz);    
                                SkDebugf( "Real Bust Line %0.2f %0.2f %0.2f %s\n", fRealBustLineY, fBustLineY, fRealBustLength, sz );
                            }
                            //_pTEBustLength->focus( true );
                        }
                    ).ref( (DecWidget **)&_pTEWaistLength).size(-1, 30))
                    ._( Button("", 0xe2e1,
                        [=](DecButton& btn) {
                            _pTEWaistLength->_callback( *_pTEWaistLength );
                        })
                        .size(30, 30)
                    )
                )            
                ._(Text( "อกจริง (ปุ่มคอหน้า ถึง ระดับยอดอก)") )
                ._( Row()
                    .size(-1, 30)
                    .gap(4)
                    .border(false)
                    .background(false)
                    ._(TextEdit("",
                        [=](DecTextEdit& textedit) {
                            fRealBustLength  = atoi(_pTEBustLength->text());
                            fRealBustLineY = fNeckLineY + fRealBustLength;
                            SkDebugf( "Real Bust Line %0.2f %0.2f\n", fRealBustLineY, fBustLineY );
                        }
                    ).ref( (DecWidget **)&_pTEBustLength).size(-1, 30))
                    ._( Button("", 0xe2e1,
                        [=](DecButton& btn) {
                            _pTEBustLength->_callback( *_pTEBustLength );
                        })
                        .size(30, 30)
                    )
                )   
                ._(Text( "สะโพกล่าง (เอวถึงสะโพกล่าง ปกติ 20 ซ.ม.)") ) // ปกติ 20 ซ.ม.
                ._( Row()
                    .size(-1, 30)
                    .gap(4)
                    .border(false)
                    .background(false)
                    ._(TextEdit("", 
                        [=](DecTextEdit& textedit) {
                            fLowHipLength  = atoi(_pTELowHipLength->text());
                            fLowHipLineY = fWaistLineY + fLowHipLength;
                            fHighHipLineY = fWaistLineY + (fLowHipLength / 2 );
                        }
                    ).ref( (DecWidget **)&_pTELowHipLength).size(-1, 30))
                    ._( Button("", 0xe2e1,
                        [=](DecButton& btn) {
                            _pTELowHipLength->_callback( *_pTELowHipLength );
                        })
                        .size(30, 30)
                    )
                )    
                ._(Text( "รอบคอ (ระดับบุ๋มคอหน้า)") )
                ._( Row()
                    .size(-1, 30)
                    .gap(4)
                    .border(false)
                    .background(false)
                    ._(TextEdit("", 
                        [=](DecTextEdit& textedit) {
                            fNeckRoundLength  = atoi(_pTENeckRoundLength->text());
                            fNeckWidth = fNeckRoundLength / 6.0 + 0.5;
                            fNeckDepth = fNeckWidth + 0.5;

                            fTopShoulderLineY = fNeckLineY - fNeckDepth;
                            fLowShoulderLineY = fNeckLineY - (fNeckDepth/2);
                        }
                    ).ref( (DecWidget **)&_pTENeckRoundLength).size(-1, 30))
                    ._( Button("", 0xe2e1,
                        [=](DecButton& btn) {
                            _pTENeckRoundLength->_callback( *_pTENeckRoundLength );
                        })
                        .size(30, 30)
                    )
                )  
            ._(Text( "ตะเข็บไหล่ (ข้างคอ ถึง ปลายไหล่)") )
                ._( Row()
                    .size(-1, 30)
                    .gap(4)
                    .border(false)
                    .background(false)
                    ._(TextEdit("", 
                        [=](DecTextEdit& textedit) {
                            fShoulderSeamLength = atoi(_pTEShoulderSeamLength->text());
                            float y = (fNeckDepth / 2) * (fNeckDepth / 2);
                            float a = fShoulderSeamLength * fShoulderSeamLength;
                            float x = sqrt( a + y );
                            fShoulderX = fCenterFrontLineX - fNeckWidth - x;
                        }
                    ).ref( (DecWidget **)&_pTEShoulderSeamLength).size(-1, 30))
                    ._( Button("", 0xe2e1,
                        [=](DecButton& btn) {
                            _pTEShoulderSeamLength->_callback( *_pTEShoulderSeamLength );
                        })
                        .size(30, 30)
                    )
                )   
                ._(Text( "บ่าหน้า (ระหว่างจุดสิ้นสุดร่องรักแร้สองข้าง)") )
                ._( Row()
                    .size(-1, 30)
                    .gap(4)
                    .border(false)
                    .background(false)
                    ._(TextEdit("", 
                        [=](DecTextEdit& textedit) {
                            fChestWidthLength = atoi(_pTEChestWidthLength->text());
                            fChestX = fCenterFrontLineX - (fChestWidthLength / 2);
                        }
                    ).ref( (DecWidget **)&_pTEChestWidthLength).size(-1, 30))
                    ._( Button("", 0xe2e1,
                        [=](DecButton& btn) {
                            _pTEChestWidthLength->_callback( *_pTEChestWidthLength );
                        })
                        .size(30, 30)
                    )
                )    
                ._(Text( "บ่าหลัง (ระหว่างจุดสิ้นสุดร่องรักแร้สองข้าง)") )
                ._( Row()
                    .size(-1, 30)
                    .gap(4)
                    .border(false)
                    .background(false)
                    ._(TextEdit("", 
                        [=](DecTextEdit& textedit) {
                            fBackWidthLength = atoi(_pTEBackWidthLength->text());
                            fBackX = fCenterFrontLineX - (fBackWidthLength / 2);
                        }
                    ).ref( (DecWidget **)&_pTEBackWidthLength).size(-1, 30))
                    ._( Button("", 0xe2e1,
                        [=](DecButton& btn) {
                            _pTEBackWidthLength->_callback( *_pTEBackWidthLength );
                        })
                        .size(30, 30)
                    )
                )                              
                ._(Text( "รอบอก (ระดับยอดอก)") )
                ._( Row()
                    .size(-1, 30)
                    .gap(4)
                    .border(false)
                    .background(false)
                    ._(TextEdit("", 
                        [=](DecTextEdit& textedit) {
                            fBustRoundLength = atoi(_pTEBustRoundLength->text());
                            fBustX = fCenterFrontLineX - ((fBustRoundLength + 8) / 4);
                        }
                    ).ref( (DecWidget **)&_pTEBustRoundLength).size(-1, 30))
                    ._( Button("", 0xe2e1,
                        [=](DecButton& btn) {
                            _pTEBustRoundLength->_callback( *_pTEBustRoundLength );
                        })
                        .size(30, 30)
                    )
                )    
                ._(Text( "รอบเอว (ระดับคอดสุดด้านข้าง 1-2 ซ.ม.เหนือสะดือ)") )
                // รอบเอว คนผอม 1 ซ.ม.เหนือสะดือ, คนอ้วน 2 ซ.ม.เหนือสะดือ
                ._( Row()
                    .size(-1, 30)
                    .gap(4)
                    .border(false)
                    .background(false)
                    ._(TextEdit("", 
                        [=](DecTextEdit& textedit) {
                            fWaistRoundLength = atoi(_pTEWaistRoundLength->text());
                            fWaistX = fCenterFrontLineX - ((fWaistRoundLength + 8) / 4);
                        }
                    ).ref( (DecWidget **)&_pTEWaistRoundLength).size(-1, 30))
                    ._( Button("", 0xe2e1,
                        [=](DecButton& btn) {
                            _pTEWaistRoundLength->_callback( *_pTEWaistRoundLength );
                        })
                        .size(30, 30)
                    )
                )     
                ._(Text( "รอบสะโพก (ระดับสะโพกล่าง)") )
                ._( Row()
                    .size(-1, 30)
                    .gap(4)
                    .border(false)
                    .background(false)
                    ._(TextEdit("", 
                        [=](DecTextEdit& textedit) {
                            fHipRoundLength = atoi(_pTEHipRoundLength->text());
                            fHipX = fCenterFrontLineX - ((fHipRoundLength + 8) / 4);
                        }
                    ).ref( (DecWidget **)&_pTEHipRoundLength).size(-1, 30))
                    ._( Button("", 0xe2e1,
                        [=](DecButton& btn) {
                            _pTEHipRoundLength->_callback( *_pTEHipRoundLength );
                        })
                        .size(30, 30)
                    )
                )                                                                  
            )               
        )

        /*
        ._( Pages() 
            .border( false )
            
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
                
                ._(Row()
                    .size( -1, 0 )
                    .background( false )
                    .border( false )
                    .gap( 5 )
                    ._( Button("0/1", 0, [=]( DecWidget &w )  {
                        toggle01();
                    }) )
                    ._( Button("1/4", 0, [=]( DecWidget &w )  {
                        toggle14();
                    }) )
                    ._( Button("X", 0, [=]( DecWidget &w )  {
                        toggleX();
                    }) )
                    ._( Button("Append", 0, [=]( DecWidget &w )  {
                        appendSewingLine();
                    }) )
                )
                ._(Row()
                    .size( -1, 0 )
                    .background( false )
                    .border( false )
                    .gap( 5 )
                    ._( Button("Gen", 0, [=]( DecWidget &w )  {
                        createSewingLine();
                    }).size( -1, 0 ) )
                    ._( Button("Load", 0, [=]( DecWidget &w )  {
                        loadSewingLine();
                    }).size( -1, 0 ) )
                    ._( Button("Save", 0, [=]( DecWidget &w )  {
                        saveSewingLine();
                    }).size( -1, 0 ) )
                )
                
            )     
                
        )
         */  
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
            .visible( false )
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
                //drawSewingLine( dom, canvas );
                drawPattern( dom, canvas );
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
                .visible( false )
            )
            ._( Container()
                .size( -1, 360 )
                .border_color( 0xFF808080 )
                .visible( false )
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
                /*
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
                */
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
                /*
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
                */
                ._( Row()
                    .size( -1, 0 )
                    .border( false )
                    ._( Container().size( -1, 0 ).border(false)._(CheckBox( "S" ).check(false).ref( (DecWidget**)&_pCB_S).onValueChanged([=](DecWidget &w){
                        //SkDebugf( "S %i\n", (int)w.value() );
                        _is_S = w.value();
                        if( _is_S ) {
                            //_pCB_M->check( false );
                            //_pCB_L->check( false );
                            //_pCB_XL->check( false );
                            //_pCB_XXL->check( false );
                        }
                        //clearEditingSewingLine();
                        updateAllCheckBox();
                        updateNodeVisibility();
                    })) )
                    ._( Container().size( -1, 0 ).border(false)._(CheckBox( "M" ).check(false).ref( (DecWidget**)&_pCB_M).onValueChanged([=](DecWidget &w){
                        //SkDebugf( "M\n" );
                        _is_M = w.value();
                        if( _is_M ) {
                            //_pCB_S->check( false );
                            //_pCB_L->check( false );
                            //_pCB_XL->check( false );
                            //_pCB_XXL->check( false );
                        }
                        //clearEditingSewingLine();
                        updateAllCheckBox();
                        updateNodeVisibility();
                    })) )
                    ._( Container().size( -1, 0 ).border(false)._(CheckBox( "L" ).check(false).ref( (DecWidget**)&_pCB_L).onValueChanged([=](DecWidget &w){
                        //SkDebugf( "L\n" );
                        _is_L = w.value();
                        if( _is_L ) {
                            //_pCB_S->check( false );
                            //_pCB_M->check( false );
                            //_pCB_XL->check( false );
                            //_pCB_XXL->check( false );
                        }
                        //clearEditingSewingLine();
                        updateAllCheckBox();
                        updateNodeVisibility();
                    })) )
                    ._( Container().size( -1, 0 ).border(false)._(CheckBox( "XL" ).check(false).ref( (DecWidget**)&_pCB_XL).onValueChanged([=](DecWidget &w){
                        //SkDebugf( "XL\n" );
                        _is_XL = w.value();
                        if( _is_XL ) {
                            //_pCB_S->check( false );
                            //_pCB_M->check( false );
                            //_pCB_L->check( false );
                            //_pCB_XXL->check( false );
                        }
                        //clearEditingSewingLine();
                        updateAllCheckBox();
                        updateNodeVisibility();
                    }) ) )
                    ._( Container().size( -1, 0 ).border(false)._(CheckBox( "XXL" ).check(false).ref( (DecWidget**)&_pCB_XXL).onValueChanged([=](DecWidget &w){
                        //SkDebugf( "XXL\n" );
                        _is_XXL = w.value();
                        if( _is_XXL ) {
                            //_pCB_S->check( false );
                            //_pCB_M->check( false );
                            //_pCB_L->check( false );
                            //_pCB_XL->check( false );
                        }
                        //clearEditingSewingLine();
                        updateAllCheckBox();
                        updateNodeVisibility();
                    }) ) )
                )
                ._( Text( "Generate: " ).margin_top( 5 ) )
                ._( ComboBox()
                    .ref( (DecWidget **)&_pComboBoxGenerate )
                    .size( -1, 32 )
                    .border( true )
                    ._( ComboBoxItem( "กระโปรงชิ้นหน้าชิ้นหลังเท่ากัน" ) )
                    .onValueChanged( [=](DecWidget &widget) {
                        changeTargetGenerate( widget.text() );
                    })
                )                
                ._( Text( "Export: " ).margin_top( 5 ) )
                ._( ComboBox()
                    .ref( (DecWidget **)&_pComboBoxPiece )
                    .size( -1, 32 )
                    .border( true )
                    ._( ComboBoxItem( "ชิ้นตัวหน้า" ) )
                    ._( ComboBoxItem( "ชิ้นตัวหลัง" ) )
                    .onValueChanged( [=](DecWidget &widget) {
                        changeTargetPiece( widget.text() );
                    })
                )                
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    .margin( 10, 0, 0, 0 )
                    ._( Button("PDF Export", 0, [=]( DecButton &btn ) {
                        char sz[128];
                        if( nTargetPiece == 1 ) {
                            sprintf( sz, "Test-Back.pdf" );
                        }
                        else {
                            sprintf( sz, "Test-Front.pdf" );
                        }
                        startExportPDF( sz, false );
                        renderPDF( "Test.pdf", false );
                        pSVGDomEditor->endExportPDF();
                    }))
                )
                /*
                ._( Text( "Bulk Export: " ).margin_top( 5 ) )
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("PDF all sizes", 0, [=]( DecButton &btn ) {
                        //SkDebugf( "Export PDF all sizes\n" );
                        exportPDFs();
                    }))
                    ._( Button("PNG all sizes", 0, [=]( DecButton &btn ) {
                        //SkDebugf( "Export PNG all sizes\n" );
                        exportPNGs();
                    }))
                )
                
                ._( Row()
                    .size( -1, 0 )
                    .gap( 10 )
                    .border( false )
                    ._( Button("Test", 0, [=]( DecButton &btn ) {
                    }))
                )
                */
            )
        )
    );
}

DecSVGEditor::~DecSVGEditor() {
}

/*
void DecSVGEditor::toggle01() {
    SkDebugf( "Toggle01 %i\n", _selectedSewingLineIndex );
    if( _selectedSewingLineIndex >= 1 && _selectedSewingLineIndex < _pPathNodeList->count() ) {
        DecListItem *pItem = _pPathNodeList->getItem(_selectedSewingLineIndex);
        char sz[128];
        strcpy( sz, pItem->text() );
        if( sz[0] == 'M' ) {
            sz[0] = 'L';
        }
        else if( sz[0] == 'L' ) {
            sz[0] = 'M';
        }
        pItem->text( sz );
        _vEditingSewingLineInfo[_selectedSewingLineIndex] = sz;
        updateEditingSewingLine();
    }
}

void DecSVGEditor::toggle14() {
    if( _selectedSewingLineIndex >= 1 && _selectedSewingLineIndex < _pPathNodeList->count() ) {
        DecListItem *pItem = _pPathNodeList->getItem(_selectedSewingLineIndex);
        char sz[128];
        strcpy( sz, pItem->text() );
        if( sz[2] == '1' ) {
            sz[2] = '4';
        }
        else if( sz[2] == '4' ) {
            sz[2] = '1';
        }
        pItem->text( sz );
        _vEditingSewingLineInfo[_selectedSewingLineIndex] = sz;
        updateEditingSewingLine();
    }
}

void DecSVGEditor::toggleX() {
    if( _selectedSewingLineIndex >= 1 && _selectedSewingLineIndex < _pPathNodeList->count() ) {
        DecListItem *pItem = _pPathNodeList->getItem(_selectedSewingLineIndex);
        char sz[128];
        strcpy( sz, pItem->text() );

        if( sz[0] == 'X' ) {
            DecListItem *pItem0 = _pPathNodeList->getItem(0);
            strcpy( sz, pItem0->text() );

            if( sz[0] != 'M' ) {
                return;
            }    
            sz[0] = 'L';

            pItem->text( sz );
            _vEditingSewingLineInfo[_selectedSewingLineIndex] = sz;
            updateEditingSewingLine();
        }
        else {
            return;
        }
    }
}

void DecSVGEditor::updateSelectedSewingLine() {
    if( _selectedSewingLineIndex >= 1 && _selectedSewingLineIndex < _pPathNodeList->count() ) {
        DecListItem *pItem0 = _pPathNodeList->getItem(_selectedSewingLineIndex-1);
        char sz[128];
        strcpy( sz, pItem0->text() );

        char *verb = strtok( sz, " ");
        char *w = strtok (NULL, " ");
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
            return;
        }
        SkPath path;
        path.moveTo( atof(x), atof(y) );

        DecListItem *pItem1 = _pPathNodeList->getItem(_selectedSewingLineIndex);
        strcpy( sz, pItem1->text() );

        verb = strtok( sz, " ");
        w = strtok (NULL, " ");
        x = strtok (NULL, " ");
        y = strtok (NULL, " ");
        if( !strcmp( verb, "M" ) ) {
            path.lineTo( atof(x), atof(y) );
        }
        else if( !strcmp( verb, "L" ) ) {
            path.lineTo( atof(x), atof(y) );
        }
        else if( !strcmp( verb, "C" ) ) {
            char *x2 = strtok (NULL, " ");
            char *y2 = strtok (NULL, " ");
            char *x3 = strtok (NULL, " ");
            char *y3 = strtok (NULL, " ");
            path.cubicTo( atof(x), atof(y), atof(x2), atof(y2), atof(x3), atof(y3) );
        }
        else if( !strcmp( verb, "Q" ) ) {
            char *x2 = strtok (NULL, " ");
            char *y2 = strtok (NULL, " ");
            path.quadTo( atof(x), atof(y), atof(x2), atof(y2) );
        }
        else if( !strcmp( verb, "N" ) ) {
            char *x2 = strtok (NULL, " ");
            char *y2 = strtok (NULL, " ");
            path.quadTo( atof(x), atof(y), atof(x2), atof(y2));
        }
        else if( !strcmp( verb, "X" ) ) {
            //path->close();
            return;
        }        
        _SelectedSewingLinePath = new SkPath( path );
    }
}
*/

int DecSVGEditor::getSizeIndex() {
    if( _is_S ) return 0;
    else if( _is_M ) return 1;
    else if( _is_L ) return 2;
    else if( _is_XL ) return 3;
    else if( _is_XXL ) return 4;
    return -1;
}

/*
void DecSVGEditor::appendSewingLine() {
    int size_index = getSizeIndex();
    if( size_index == -1 ) {
        return;
    }
    if( _sewingLinePath[size_index] && _editingSewingLine ) {
        _sewingLinePath[size_index]->addPath( *_editingSewingLine );
    }
    clearEditingSewingLine();
    pNodeTree->select(NULL);
    pNodeTree->collapseAll(1);
}

void DecSVGEditor::updateEditingSewingLine( ) {
    int size_index = getSizeIndex();
    if( size_index == -1 ) {
        return;
    }

    if( _editingSewingLine ) {
        delete( _editingSewingLine );
        _editingSewingLine = NULL;
    }

    SkPath innerPath( *pSVGDomEditor->pSelectedNodePath );
    float fWTX[2] = { 1.0, 4.0 };

    for( int lx=0; lx<2; lx++ ) {
        float fWT = fWTX[lx];

        SkPath *path = new SkPath();

        for( int i=0; i<_pPathNodeList->count(); i++ ) {
            DecListItem*pItem = _pPathNodeList->getItem(i);
            char sz[128];
            strcpy( sz, pItem->text() );

            char *verb = strtok( sz, " ");
            char *w = strtok (NULL, " ");
            char *x = strtok (NULL, " ");
            char *y = strtok (NULL, " ");

            float fW = atof( w );

            if( !strcmp( verb, "M" ) ) {
                path->moveTo( atof(x), atof(y) );
            }
            else if( !strcmp( verb, "L" ) ) {
                if( fW == fWT ) {
                    path->lineTo( atof(x), atof(y) );
                }
                else {
                    path->moveTo( atof(x), atof(y) );
                }
            }
            else if( !strcmp( verb, "C" ) ) {
                char *x2 = strtok (NULL, " ");
                char *y2 = strtok (NULL, " ");
                char *x3 = strtok (NULL, " ");
                char *y3 = strtok (NULL, " ");
                if( fW == fWT ) {
                    path->cubicTo( atof(x), atof(y), atof(x2), atof(y2), atof(x3), atof(y3) );
                }
                else {
                    path->moveTo( atof(x3), atof(y3) );
                }                    
            }
            else if( !strcmp( verb, "Q" ) ) {
                char *x2 = strtok (NULL, " ");
                char *y2 = strtok (NULL, " ");
                if( fW == fWT ) {
                    path->quadTo( atof(x), atof(y), atof(x2), atof(y2) );
                }
                else {
                    path->moveTo( atof(x2), atof(y2) );
                }                    
            }
            else if( !strcmp( verb, "N" ) ) {
                char *x2 = strtok (NULL, " ");
                char *y2 = strtok (NULL, " ");
                if( fW == fWT ) {
                    path->quadTo( atof(x), atof(y), atof(x2), atof(y2));
                }
                else {
                    path->moveTo( atof(x2), atof(y2));
                }                    
            }
            else if( !strcmp( verb, "X" ) ) {
                //path->close();
            }
        }

        #if defined(SK_BUILD_FOR_WIN)
        ApplyStroke( *path, {
            //.width = 
            72.0f / 2.54f * fWT * 2.0f,
            //.miter_limit = 
            72 / 2.54 * 2.0,
            //.join = 
            SkPaint::Join::kBevel_Join,
            //.cap = 
            SkPaint::Cap::kButt_Cap,
            //.precision = 
            17,
        } );
        #else
        ApplyStroke(*path, {
            .width = 72.0f / 2.54f * fWT * 2.0f,
            .join = SkPaint::Join::kMiter_Join,
            .cap = SkPaint::Cap::kRound_Cap,
            .miter_limit = 72 / 2.54 * 2.0,
            .precision = 17
        } );
        #endif

        Op(*path, innerPath, SkPathOp::kReverseDifference_SkPathOp, &innerPath);

        delete( path );
    }
    _editingSewingLine = new SkPath( innerPath );

    pSVGDomEditor->dirty( true );
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
    if( _SelectedSewingLinePath ) {
        delete( _SelectedSewingLinePath );
        _SelectedSewingLinePath = NULL;
    }
    _selectedSewingLineIndex = -1;
    _vSewingLineInfo[size_index].clear();
}

void DecSVGEditor::clearEditingSewingLine() {
    if( _editingSewingLine ) {
        delete(_editingSewingLine );
        _editingSewingLine = NULL;
    }
    if( _SelectedSewingLinePath ) {
        delete( _SelectedSewingLinePath );
        _SelectedSewingLinePath = NULL;
    }
    _selectedSewingLineIndex = -1;
    _pPathNodeList->clear();
    int size_index = getSizeIndex();
    if( size_index == -1 ) {
        return;
    }    
    _vEditingSewingLineInfo.clear();
}

void DecSVGEditor::loadSewingLine() {
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

void DecSVGEditor::saveSewingLine() {
    if( _editingSewingLine ) {
        int size_index = getSizeIndex();
        if( size_index != -1 ) {
            _sewingLinePath[size_index] = new SkPath( *_editingSewingLine );
        }
    }
    for( int i=0; i<5; i++ ) {
        if( _sewingLinePath[i] ) {
            std::string fn = std::string("_sewingline/") + _filebasename;
            if( i == 0 ) fn += "-S";
            else if( i == 1 ) fn += "-M";
            else if( i == 2 ) fn += "-L";
            else if( i == 3 ) fn += "-XL";
            else if( i == 4 ) fn += "-XXL";

            SkDebugf( "Save sewing line %s\n", fn.c_str() );
            std::string ss = "";

            SkPoint pt[1000];
            uint8_t vb[1000];

            int nPoints = _sewingLinePath[i]->getPoints( pt, 1000 );
            int nVerbs = _sewingLinePath[i]->getVerbs( vb, 1000 );

            int p = 0;
            SkPoint lastPt;
            for( int v=0; v<nVerbs; v++ ) {
                char sz[64];
                if( vb[v] == SkPath::Verb::kMove_Verb ) { // move to
                    sprintf( sz, "M %0.2f %0.2f\n", pt[p].fX, pt[p].fY );
                    ss += sz;
                    lastPt = pt[p];
                    p++;
                }
                else if( vb[v] == SkPath::Verb::kLine_Verb ) { // line to
                    sprintf( sz, "L %0.2f %0.2f\n", pt[p].fX, pt[p].fY );
                    ss += sz;
                    lastPt = pt[p];
                    p++;
                }
                else if( vb[v] == SkPath::Verb::kQuad_Verb ) { // quad to
                    sprintf( sz, "Q %0.2f %0.2f %0.2f %0.2f\n", pt[p].fX, pt[p].fY, pt[p+1].fX, pt[p+1].fY );
                    ss += sz;
                    p++;
                    lastPt = pt[p];
                    p++;
                }
                else if( vb[v] == SkPath::Verb::kConic_Verb ) { // Conic to
                    sprintf( sz, "N %0.2f %0.2f %0.2f %0.2f\n", pt[p].fX, pt[p].fY, pt[p+1].fX, pt[p+1].fY );
                    ss += sz;
                    p++;
                    lastPt = pt[p];
                    p++;
                }
                else if( vb[v] == SkPath::Verb::kCubic_Verb ) { // Cubic to
                    sprintf( sz, "C %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f\n",
                        pt[p].fX, pt[p].fY, pt[p+1].fX, pt[p+1].fY, pt[p+2].fX, pt[p+2].fY );
                    ss += sz;
                    p++;
                    p++;
                    lastPt = pt[p];
                    p++;
                }
                else if( vb[v] == SkPath::Verb::kClose_Verb ) { // Cubic to
                    sprintf( sz, "X (Close)\n" );
                    ss += sz;
                }
            }

            std::ofstream f( fn + ".txt" );
            if( f ) {
                f.write( (const char *)ss.c_str(), ss.length() );
                f.close();
            }
        }
    }
    pNodeTree->select(NULL);
    pNodeTree->collapseAll(1);

}

void DecSVGEditor::createSewingLine() {
    clearEditingSewingLine();

    int size_index = getSizeIndex();
    SkDebugf( "Create Sewing Line: %i\n", size_index );
    std::string fn = pSVGDomEditor->name;
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
    //SkDebugf( "Edit: %s\n", fn.c_str() );

    if( pSVGDomEditor->pSelectedNodePath ) {
        SkPath *path = new SkPath(*pSVGDomEditor->pSelectedNodePath);

        SkPoint pt[1000];
        uint8_t vb[1000];
        int nPoints = path->getPoints( pt, 1000 );
        int nVerbs = path->getVerbs( vb, 1000 );
 
        int p = 0;
        SkPoint lastPt;
        _pPathNodeList->clear();
        bool bClose = false;
        for( int v=0; v<nVerbs; v++ ) {
            char sz[64];
            if( vb[v] == SkPath::Verb::kMove_Verb ) { // move to
                sprintf( sz, "M 1.00 %0.2f %0.2f", pt[p].fX, pt[p].fY );
                _vEditingSewingLineInfo.push_back( sz );
                _pPathNodeList->_( ListItem( sz ) );

                lastPt = pt[p];
                p++;
            }
            else if( vb[v] == SkPath::Verb::kLine_Verb ) { // line to
                sprintf( sz, "L 1.00 %0.2f %0.2f", pt[p].fX, pt[p].fY );
                _vEditingSewingLineInfo.push_back( sz );
                _pPathNodeList->_( ListItem( sz ) );
                lastPt = pt[p];
                p++;
            }
            else if( vb[v] == SkPath::Verb::kQuad_Verb ) { // quad to
                sprintf( sz, "Q 1.00 %0.2f %0.2f %0.2f %0.2f", pt[p].fX, pt[p].fY, pt[p+1].fX, pt[p+1].fY );
                _vEditingSewingLineInfo.push_back( sz );
                _pPathNodeList->_( ListItem( sz ) );
                p++;
                lastPt = pt[p];
                p++;
            }
            else if( vb[v] == SkPath::Verb::kConic_Verb ) { // Conic to
                sprintf( sz, "N 1.00 %0.2f %0.2f %0.2f %0.2f", pt[p].fX, pt[p].fY, pt[p+1].fX, pt[p+1].fY );
                _vEditingSewingLineInfo.push_back( sz );
                _pPathNodeList->_( ListItem( sz ) );
                p++;
                lastPt = pt[p];
                p++;
            }
            else if( vb[v] == SkPath::Verb::kCubic_Verb ) { // Cubic to
                sprintf( sz, "C 1.00 %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f",
                    pt[p].fX, pt[p].fY, pt[p+1].fX, pt[p+1].fY, pt[p+2].fX, pt[p+2].fY );
                _vEditingSewingLineInfo.push_back( sz );
                _pPathNodeList->_( ListItem( sz ) );
                p++;
                p++;
                lastPt = pt[p];
                p++;
            }
            else if( vb[v] == SkPath::Verb::kClose_Verb ) { // Close to
                sprintf( sz, "X 1.00 (Close)" );
                bClose = true;
                _vEditingSewingLineInfo.push_back( sz );
                _pPathNodeList->_( ListItem( sz ) );
            }
        }
        if( !bClose ) {
            //_vEditingSewingLineInfo.push_back( "X 1.00 (Close)" );
            //_pPathNodeList->_( ListItem( "X 1.00 (Close)" ) );
        }

        delete( path );
        updateEditingSewingLine();
    }
}

void DecSVGEditor::drawSewingLine( DecSVGDomEditor *, SkCanvas &canvas ) {
    int size_index = getSizeIndex();
    if( size_index == -1 ) {
        return;
    }
    if( _sewingLinePath[size_index] || _editingSewingLine) {
        canvas.save();
        canvas.scale( pSVGDomEditor->_svgScaleX, pSVGDomEditor->_svgScaleY );
        canvas.translate( 0+pSVGDomEditor->contentCenterOffsetX_72, 0+pSVGDomEditor->contentCenterOffsetY_72 );

        SkPaint paint;
        paint.setAntiAlias( true );
        paint.setStyle( SkPaint::Style::kStroke_Style );

        paint.setStrokeWidth( 3.0/pSVGDomEditor->drawScale_DPI );

        if( _SelectedSewingLinePath ) {
            paint.setColor( 0xFFFF0000 );
            canvas.drawPath( *_SelectedSewingLinePath, paint );
        }

        paint.setColor( 0xA00000FF );
        paint.setStrokeWidth( 0.5/pSVGDomEditor->drawScale_DPI );

        float intervals[] = { 6, 4 };
        paint.setPathEffect(SkDashPathEffect::Make(intervals, 2, 0.0f));

        if( _sewingLinePath[size_index]) {
            canvas.drawPath( *_sewingLinePath[size_index], paint );
        }


        if( _editingSewingLine ) {
            paint.setColor( 0xFFFF0000 );
            canvas.drawPath( *_editingSewingLine, paint );
        }

        canvas.restore();
    }
}
*/

void DecSVGEditor::drawText( SkCanvas &canvas, int x, int y, std::string str ) {
    SkPaint paint;
    paint.setColor( 0xFF000000 );
    paint.setStyle( SkPaint::Style::kFill_Style );
    SkFont font;
    font.setTypeface(_font->typeface(FONT_REGULAR));
    font.setSubpixel(true);
    font.setSize(40); 

    canvas.drawSimpleText( str.c_str(), str.length(), SkTextEncoding::kUTF8, x, y-10, font, paint );

}

void DecSVGEditor::drawNeckLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render ) {
    SkPaint paint;
    paint.setColor( 0xFF00FFFF );
    paint.setStyle( SkPaint::Style::kStroke_Style );

    float y = fNeckLineY * dom->_svgDPIY / 2.54;
    float x = fCenterFrontLineX * dom->_svgDPIY / 2.54 + 20;

    if( !render ) {
        canvas.drawLine( 0, y, pSVGDomEditor->drawPaperW_DPI, y, paint );

        y = y - 10;

        std::string str( "1. เส้นคอ" );
        drawText( canvas, x + (3.5 * dom->_svgDPIY / 2.54), y, str );
    }

    if( fNeckWidth && fNeckDepth ) {
        if( !render ) {
            x = (fCenterFrontLineX - fNeckWidth) * dom->_svgDPIY / 2.54;
            y = fNeckLineY * dom->_svgDPIY / 2.54;
            float y2 = y - fNeckDepth * dom->_svgDPIY / 2.54;
            canvas.drawLine( x, y, x, y2, paint );

            y = fTopShoulderLineY * dom->_svgDPIY / 2.54;
            canvas.drawLine( 0, y, pSVGDomEditor->drawPaperW_DPI, y, paint );
            y = fLowShoulderLineY * dom->_svgDPIY / 2.54;
            canvas.drawLine( 0, y, pSVGDomEditor->drawPaperW_DPI, y, paint );
        }

        if( fShoulderX ) {
            x = (fCenterFrontLineX - fNeckWidth) * dom->_svgDPIY / 2.54;
            y = fTopShoulderLineY * dom->_svgDPIY / 2.54;
            float x2 = fShoulderX * dom->_svgDPIY / 2.54;
            float y2 = fLowShoulderLineY * dom->_svgDPIY / 2.54;

            if( nTargetPiece == 1 ) {
                y2 -= 1 * dom->_svgDPIY / 2.54;
            }

            paint.setColor( 0xFF000000 );
            paint.setStrokeWidth( 4 );
            paint.setAntiAlias( true );
            canvas.drawLine( x, y, x2, y2, paint );
        }
        SkPath path;

        paint.setColor( 0xFF000000 );
        paint.setStrokeWidth( 4 );
        paint.setAntiAlias( true );

        if( nTargetPiece == 0 ) {
            float x1 = (fCenterFrontLineX - fNeckWidth) * dom->_svgDPIY / 2.54;
            float y1 = fTopShoulderLineY * dom->_svgDPIY / 2.54;
            float x4 = (fCenterFrontLineX) * dom->_svgDPIY / 2.54;
            float y4 = fNeckLineY * dom->_svgDPIY / 2.54;
            float x2 = x1;
            float y2 = (y1+y4) / 2;
            float x3 = (x1+x4) / 2;
            float y3 = y4;
            path.moveTo( x1, y1 );
            path.cubicTo( x2, y2, x3, y3, x4, y4 );
            canvas.drawPath( path, paint );
        }
        else if( nTargetPiece == 1 ) {
            float x1 = (fCenterFrontLineX - fNeckWidth) * dom->_svgDPIY / 2.54;
            float y1 = fTopShoulderLineY * dom->_svgDPIY / 2.54;
            float x4 = (fCenterFrontLineX) * dom->_svgDPIY / 2.54;
            float y4 = (fTopShoulderLineY+2) * dom->_svgDPIY / 2.54;
            float x2 = x1;
            float y2 = (y1 + y4) / 2;
            float x3 = (x1 + x4) / 2;
            float y3 = y4;
            path.moveTo( x1, y1 );
            path.cubicTo( x2, y2, x3, y3, x4, y4 );
            path.lineTo( x4, fNeckLineY * dom->_svgDPIY / 2.54 );
            canvas.drawPath( path, paint );            
        }
    }
}

void DecSVGEditor::drawWaistLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render ) {
    if( fWaistLength == 0 ) 
        return;

    SkPaint paint;
    paint.setColor( 0xFF00FFFF );
    paint.setStyle( SkPaint::Style::kStroke_Style );

    float y = fWaistLineY * dom->_svgDPIY / 2.54;
    canvas.drawLine( 0, y, pSVGDomEditor->drawPaperW_DPI, y, paint );

    float x = fCenterFrontLineX * dom->_svgDPIY / 2.54 + 20;
    y = y - 10;

    std::string str( "2. เส้นเอว" );
    drawText( canvas, x + (3.5 * dom->_svgDPIY / 2.54), y, str );    
}

void DecSVGEditor::drawBustLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render ) {
    if( fWaistLength == 0 ) 
        return;
    if( fBustLineY == 0 ) 
        return;

    SkPaint paint;
    paint.setColor( 0xFF00FFFF );
    paint.setStyle( SkPaint::Style::kStroke_Style );

    float y = fBustLineY * dom->_svgDPIY / 2.54;
    canvas.drawLine( 0, y, pSVGDomEditor->drawPaperW_DPI, y, paint );

    std::string str;
    if( fBustLineY != fRealBustLineY ) {
        paint.setColor( 0xFF00C0C0 );
        y = fRealBustLineY * dom->_svgDPIY / 2.54;
        canvas.drawLine( 0, y, pSVGDomEditor->drawPaperW_DPI, y, paint );
        str = "3.1 เส้นอกจริง";
    }
    else {
        str = "3. เส้นอกมาตรฐาน";
    }

    float x = fCenterFrontLineX * dom->_svgDPIY / 2.54 + 20;
    y = y - 10;

    drawText( canvas, x + (3.5 * dom->_svgDPIY / 2.54), y, str );    
}


void DecSVGEditor::drawHipLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render ) {
    if( fLowHipLength == 0 ) 
        return;

    SkPaint paint;
    paint.setColor( 0xFF00FFFF );
    paint.setStyle( SkPaint::Style::kStroke_Style );

    float y = fLowHipLineY * dom->_svgDPIY / 2.54;
    canvas.drawLine( 0, y, pSVGDomEditor->drawPaperW_DPI, y, paint );

    float x = fCenterFrontLineX * dom->_svgDPIY / 2.54 + 20;
    y = y - 10;

    std::string str( "5. สะโพกล่าง" );
    drawText( canvas, x + (3.5 * dom->_svgDPIY / 2.54), y, str );    

    y = fHighHipLineY * dom->_svgDPIY / 2.54;
    canvas.drawLine( 0, y, pSVGDomEditor->drawPaperW_DPI, y, paint );

    x = fCenterFrontLineX * dom->_svgDPIY / 2.54 + 20;
    y = y - 10;

    str = "6. สะโพกบน";
    drawText( canvas, x + (3.5 * dom->_svgDPIY / 2.54), y, str );    

}

void DecSVGEditor::drawChestLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render ) {
    if( fWaistLength == 0 ) 
        return;
    if( fChestLineY == 0 ) 
        return;

    SkPaint paint;
    paint.setColor( 0xFF00FFFF );
    paint.setStyle( SkPaint::Style::kStroke_Style );

    float y = fChestLineY * dom->_svgDPIY / 2.54;
    canvas.drawLine( 0, y, pSVGDomEditor->drawPaperW_DPI, y, paint );

    float x = fCenterFrontLineX * dom->_svgDPIY / 2.54 + 20;
    y = y - 10;

    std::string str( "4. เส้นบ่า" );
    drawText( canvas, x + (3.5 * dom->_svgDPIY / 2.54), y, str );    
}

void DecSVGEditor::drawArmHole( DecSVGDomEditor *dom, SkCanvas &canvas, bool render ) {
    if( fChestLineY == 0 ) 
        return;

    if( fChestX == 0 ) 
        return;

    SkPaint paint2;  
    paint2.setColor( 0xFF000000 );
    paint2.setStrokeWidth( 4 );
    paint2.setAntiAlias( true );

    //if( !fBustX ) 
    if( !render )
    {
        float x = fChestX * dom->_svgDPIY / 2.54;
        float y = fChestLineY * dom->_svgDPIY / 2.54;
        canvas.drawLine( x, y-10, x, y+10, paint2 ); 
    }

    if( !fBustX )
        return;

    float x1 = fShoulderX * dom->_svgDPIY / 2.54;
    float y1 = fLowShoulderLineY * dom->_svgDPIY / 2.54;
    float x2 = fChestX * dom->_svgDPIY / 2.54;
    float y2 = fChestLineY * dom->_svgDPIY / 2.54;
    float x3 = fBustX * dom->_svgDPIY / 2.54;
    float y3 = fRealBustLineY * dom->_svgDPIY / 2.54;
    if( nTargetPiece == 1 ) {
        y1 -= 1 * dom->_svgDPIY / 2.54;
        x2 = fBackX * dom->_svgDPIY / 2.54;
    }
    if( fBustLineY < fRealBustLineY ) {
        y3 = fBustLineY * dom->_svgDPIY / 2.54;
    }    
    float y4 = y2;
    float x4 = x3 - ( (x3 - x1) * (y3-y2)/(y3-y1) );  
    //canvas.drawLine( x4, y4-10, x4, y4+10, paint2 );   

    float slope = ( (x3 - x1) / (y3-y1) );
    float y5 = y2 - ((y2-y1) / 3);
    float x5 = x2 + (y5-y2) * slope;
    //canvas.drawLine( x5, y5-10, x5, y5+10, paint2 );   

    float y6 = y2 + ((y3-y2) /1);
    float x6 = x2 + (y6 - y2) * slope;
    //canvas.drawLine( x6, y6-10, x6, y6+10, paint2 ); 

    SkPaint paint;
    paint.setColor( 0xFF00FFFF );
    paint.setStyle( SkPaint::Style::kStroke_Style );  
    paint.setAntiAlias( true );

    //canvas.drawLine( x1, y1, x3, y3, paint ); 

    paint.setColor( 0xFF000000 );
    paint.setStrokeWidth( 4 );

    SkPath path;
    path.moveTo( x3, y3 );
    path.quadTo( x6, y6, x2, y2 );
    path.quadTo( x5, y5, x1, y1 );
    canvas.drawPath( path, paint );
}

void DecSVGEditor::drawCenterFrontLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render ) {
    SkPaint paint;

    float x = fCenterFrontLineX * dom->_svgDPIY / 2.54;
    if( !render ) {
        paint.setColor( 0xFF00FFFF );
        paint.setStyle( SkPaint::Style::kStroke_Style );

        canvas.drawLine( x, 0, x, pSVGDomEditor->drawPaperH_DPI, paint );   
        float x2 = (fCenterFrontLineX + 3) * dom->_svgDPIY / 2.54;
        canvas.drawLine( x2, 0, x2, pSVGDomEditor->drawPaperH_DPI, paint );   
    }

    if( fWaistLength == 0 ) 
        return;

    float y1 = fNeckLineY * dom->_svgDPIY / 2.54;     
    float y2 = fWaistLineY * dom->_svgDPIY / 2.54;
    paint.setColor( 0xFF000000 );
    paint.setStrokeWidth( 4 );
    canvas.drawLine( x, y1, x, y2, paint );   

    if( fLowHipLength == 0 )
        return;

    y1 = fLowHipLineY * dom->_svgDPIY / 2.54;  
    canvas.drawLine( x, y2, x, y1, paint );    
}

void DecSVGEditor::drawSideSeam( DecSVGDomEditor *dom, SkCanvas &canvas, bool render ) {
    //SkDebugf( "%0.2f %0.2f, %0.2f\n", fBustX, fWaistX, fHipX );
    if( !fBustX ) {
        return;
    }
    SkPaint paint;
    paint.setColor( 0xFF000000 );
    paint.setStrokeWidth( 4 );
    paint.setAntiAlias( true );

    float y1 = fRealBustLineY * dom->_svgDPIY / 2.54;
    if( fBustLineY < fRealBustLineY ) {
        y1 = fBustLineY * dom->_svgDPIY / 2.54;
    }
    if( !render ) {
        if( !fHipX ) {
            float x = fBustX * dom->_svgDPIY / 2.54;
            float y = y1;
            canvas.drawLine( x, y-10, x, y+10, paint ); 
        }

        if( fWaistX ) {
            float x = fWaistX * dom->_svgDPIY / 2.54;
            float y = fWaistLineY * dom->_svgDPIY / 2.54;
            canvas.drawLine( x, y-10, x, y+10, paint ); 
        }
        if( !fHipX ) {
            return;
        }
    }


    float x1 = fBustX * dom->_svgDPIY / 2.54;     
    float x2 = fWaistX * dom->_svgDPIY / 2.54;
    float x3 = fHipX * dom->_svgDPIY / 2.54;
    float x4 = fCenterFrontLineX * dom->_svgDPIY / 2.54;

    x2 = (x1 + x3) / 2;


    float y2 = fWaistLineY * dom->_svgDPIY / 2.54;
    float y3 = fLowHipLineY * dom->_svgDPIY / 2.54;

    canvas.drawLine( x1, y1, x2, y2, paint );   
    canvas.drawLine( x2, y2, x3, y3, paint );   
    canvas.drawLine( x3, y3, x4, y3, paint );
}

void DecSVGEditor::drawPattern( DecSVGDomEditor *dom, SkCanvas &canvas ) {
    canvas.save();
    
    SkPaint paint;
    paint.setColor( 0xFF00FFFF );
    paint.setStyle( SkPaint::Style::kStroke_Style );
    //canvas.drawLine( 0, 0, dom->drawPaperW_DPI, dom->drawPaperH_DPI, paint );
    //canvas.drawLine( dom->drawPaperW_DPI, 0, 0, dom->drawPaperH_DPI, paint );
    //SkDebugf( "%0.2f %0.2f %0.2f %0.2f\n", dom->drawPaperW_DPI, dom->drawPaperH_DPI, dom->_svgDPIX, dom->_svgDPIY );

    drawNeckLine( dom, canvas );
    drawCenterFrontLine( dom, canvas );
    drawWaistLine( dom, canvas );
    drawBustLine( dom, canvas );
    drawChestLine( dom, canvas );
    drawArmHole( dom, canvas );
    drawHipLine( dom, canvas );
    drawSideSeam( dom, canvas );
    canvas.restore();
}

DecSVGEditor &SVGEditor() {
    DecSVGEditor *w = new DecSVGEditor();
    return *w;
}

void DecSVGEditor::update() {
    DecWidget::update();
}

void DecSVGEditor::updateAllCheckBox() {
    /*
    if( _is_S && _is_M && _is_L && _is_XL && _is_XXL ) {
        _pCB_All->check( true, false );
    }
    else {
        _pCB_All->check( false, false );
    }
    */
}

void DecSVGEditor::changeTargetPaper( const char *paper ) {
    pSVGDomEditor->setPaperSize( paper );
    updateContentBounds();
}

void DecSVGEditor::changeTargetPiece( const char *piece ) {
    if( !strcmp( piece, "ชิ้นตัวหลัง" ) ) {
        nTargetPiece = 1;
    }
    else {
        nTargetPiece = 0;
    }
}

void DecSVGEditor::changeTargetGenerate( const char *gen ) {
    if( !strcmp( gen, "กระโปรงชิ้นหน้าชิ้นหลังเท่ากัน" )) {
        nTargetGenerate = 0;
    }
    bTargetGenerated = false;
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
        //overlaySewingLine( canvas );
    });
}

std::string DecSVGEditor::startExportPDF( const char *filname, bool a4) {
    std::string fn = std::string("_export_pdf/") + filname;
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

void DecSVGEditor::overlayPattern( SkCanvas &canvas ) {
    DecSVGDomEditor *dom = pSVGDomEditor;
    drawCenterFrontLine( dom, canvas, true );
    drawNeckLine( dom, canvas, true );
    drawArmHole( dom, canvas, true );
    drawSideSeam( dom, canvas, true );
}


/*
void DecSVGEditor::overlaySewingLine( SkCanvas &canvas ) {
    int size_index = getSizeIndex();
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
*/

void DecSVGEditor::renderPDF( const char *patternName, bool tiled ) {
    SkDebugf( "renderPDF: %s\n", patternName );
    pSVGDomEditor->renderPDF( patternName, tiled, [=]( SkCanvas &canvas, int w, int h ) {
        //overlaySewingLine( canvas );
        overlayPattern( canvas );
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

    //loadSewingLine();
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

void DecSVGEditor::onPathVerbSelect( DecList *list ) {
    SkDebugf( "Path verb: %i\n", (int)list->value() );

    //_selectedSewingLineIndex = (int)list->value();
    //if( _SelectedSewingLinePath ) {
    //    delete( _SelectedSewingLinePath );
    //    _SelectedSewingLinePath = NULL;
    //}
    //updateSelectedSewingLine();
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
    //loadSewingLine();
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

        startExportPDF(_filebasename.c_str(), _isTiledExport);
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

        startExportPDF(_filebasename.c_str(),_isTiledExport);
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

        startExportPDF(_filebasename.c_str(),_isTiledExport);
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

        startExportPDF(_filebasename.c_str(),_isTiledExport);
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

        startExportPDF(_filebasename.c_str(),_isTiledExport);
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

    _is_S = 0;

    updateSVGInfo();
    updateNodeTree();
    updateContentBounds();
    updateAllCheckBox();
    updateNodeVisibility();
}

void DecSVGEditor::init() {
    _pComboBoxGenerate->select( 0 ); //"กระโปรงชิ้นหน้าชิ้นหลังเท่ากัน" 

    if( pSVGDomEditor ) {
        pSVGDomEditor->pSVGEditor = this;
        pSVGDomEditor->_bMoveContentToCenter = false;
    }
    //changeTargetPaper( "A1" );
    generateProductCodeList();
    _pComboBoxTargetPaper->select( 1 );
    _pComboBoxPiece->select( 0 );


    fDom = pSVGDomEditor->fDom;

    std::filesystem::path fullpath( "_magicpattern/SVG/LPAS-A1=2-01.svg" );

    _filepath = fullpath.string();
    _filename = fullpath.filename().string();
    _filebasename = fullpath.stem().string();

    //loadSewingLine();
    pNodeTree->clear();
    bool ret = pSVGDomEditor->load(_filepath.c_str());

    pSVGDomEditor->dirty();
    updateAllCheckBox();
    updateNodeVisibility();
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
    //clearEditingSewingLine();
}

void DecSVGEditor::setNodeVisibility( DecTreeItem *treeItem, bool bVisible, bool includeChildren ) {
    SkSVGNode *node = (SkSVGNode *)treeItem->_externalData;
    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kHidden;
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

    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kHidden;

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
                auto sstroke = SkSVGPaint( SkSVGColor( 0x800000FF ));
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
            auto visible = SkSVGVisibility(SkSVGVisibility::Type::kHidden);
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

            /*
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
            */
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
