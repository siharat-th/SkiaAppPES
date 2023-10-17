#include "MPPanel.h"
#include "MPButton.h"
#include "MagicPattern3.h"

DecMPPanel::DecMPPanel( DecMagicPattern *pMP, int h ) {
    ID( "MPPanel" );
    pMagicPattern = pMP;
    nMode = -1;
    
    pButtonNeck7 = NULL;
    pButtonCollar4 = NULL;

    pPanelType = NULL;
    pPanelLength = NULL;
    pPanelNeck = NULL;
    pPanelCollar = NULL;
    pPanelCollar2 = NULL;
    pPanelSleeve = NULL;
    pPanelHem = NULL;

    currentPocketLevel = -1;
    currentPocketSide = -1;

    height( 0 );
    background( false );
    border( false );
    visible( false );

    _( Row()
        .ref( (DecWidget **)&pPanelType )
        .ID( "PanelType" )
        .size( -1, 0 )
        .align( ALIGN_BOTTOMLEFT )
        .padding( 10, 0, 10, 0 )
        .gap( 10 )
        .bg_color( 0x20000000 )
        .border( false )
        .visible( false )
        ._( Spacer().size( -1, 0 ) )
        ._(MPButton( "สวมศีรษะ", 0, [=](DecButton &btn) { pMagicPattern->setType( 1 ); } ))
        ._(MPButton( "ผ่าหน้า", 0, [=](DecButton &btn) { pMagicPattern->setType( 2 ); } ))        
        ._( Spacer().size( -1, 0 ) )
    );
    _( Scroller()
        .ref( (DecWidget **)&pPanelLength )
        .ID( "PanelLength" )
        .size( -1, 0 )
        .align( ALIGN_BOTTOMLEFT )
        //.padding( 10, 0, 10, 0 )
        .background( true )
        .bg_color( 0x20000000 )
        .visible( false )
        ._( Row()
            .size( 0, 0 )
            .padding( 10, 10, 10, 10 )
            .gap( 10 )
            .background( false )
            .border( false )
            ._(MPButton( "เอว", 0, [=](DecButton &btn) { pMagicPattern->setLength( 1 ); } ))
            ._(MPButton( "สะโพกสูง", 0, [=](DecButton &btn) { pMagicPattern->setLength( 2 ); } ))        
            ._(MPButton( "สะโพกต่ำ", 0, [=](DecButton &btn) { pMagicPattern->setLength( 3 ); } ))        
            ._(MPButton( "เหนือเข่า", 0, [=](DecButton &btn) { pMagicPattern->setLength( 4 ); } ))        
            ._(MPButton( "คลุมเข่า", 0, [=](DecButton &btn) { pMagicPattern->setLength( 5 ); } ))        
        )
    );
    _( Scroller()
        .ref( (DecWidget **)&pPanelNeck )
        .ID( "PanelNeck" )
        .size( -1, 0 )
        .align( ALIGN_BOTTOMLEFT )
        .background( true )
        .bg_color( 0x20000000 )
        .visible( false )
        ._( Row()
            .size( 0, 0 )
            .padding( 10, 10, 10, 10 )
            .gap( 10 )
            .background( false )
            .border( false )
            ._(MPButton( "คอกลม", 0, [=](DecButton &btn) { pMagicPattern->setNeck( 0 ); } ))
            ._(MPButton( "คอวี", 0, [=](DecButton &btn) { pMagicPattern->setNeck( 1 ); } ))
            ._(MPButton( "คอปาด", 0, [=](DecButton &btn) { pMagicPattern->setNeck( 2 ); } ))
            ._(MPButton( "คอสี่เหลี่ยม", 0, [=](DecButton &btn) { pMagicPattern->setNeck( 3 ); } ))
            ._(MPButton( "คอคว้าน", 0, [=](DecButton &btn) { pMagicPattern->setNeck( 4 ); } ))
            ._(MPButton( "คอสี่เหลี่ยมแคบ", 0, [=](DecButton &btn) { pMagicPattern->setNeck( 5 ); } ))
            ._(MPButton( "คอคว้านแคบ", 0, [=](DecButton &btn) { pMagicPattern->setNeck( 6 ); } ))
            ._(MPButton( "คอหยดน้ำมีโบว์", 0, [=](DecButton &btn) { pMagicPattern->setNeck( 7 ); } )
                .ref( (DecWidget **)&pButtonNeck7 )
            )     
        )
    );
    _( Scroller()
        .ref( (DecWidget **)&pPanelCollar )
        .ID( "PanelCollar" )
        .size( -1, 0 )
        .align( ALIGN_BOTTOMLEFT )
        //.padding( 10, 0, 10, 0 )
        .background( true )
        .bg_color( 0x20000000 )
        .visible( false )
        ._( Row()
            .size( 0, 0 )
            .padding( 10, 10, 10, 10 )
            .gap( 10 )
            .background( false )
            .border( false )
            ._(MPButton( "ไม่มีปก", 0, [=](DecButton &btn) { pMagicPattern->setCollar( 0 ); } ))
            ._(MPButton( "ปกบัว", 0, [=](DecButton &btn) { pMagicPattern->setCollar( 1 ); } ))
            ._(MPButton( "ปกบัวแหลม", 0, [=](DecButton &btn) { pMagicPattern->setCollar( 2 ); } ))
            ._(MPButton( "ปกคอจีน", 0, [=](DecButton &btn) { pMagicPattern->setCollar( 3 ); } ))
            ._(MPButton( "ปกระบาย", 0, [=](DecButton &btn) { pMagicPattern->setCollar( 4 ); } )
                .ref( (DecWidget **)&pButtonCollar4 )
            )
            ._(MPButton( "ปกผูกโบว์", 0, [=](DecButton &btn) { pMagicPattern->setCollar( 5 ); } ))
        )
    );
    _( Scroller()
        .ref( (DecWidget **)&pPanelCollar2 )
        .ID( "PanelCollar2" )
        .size( -1, 0 )
        .align( ALIGN_BOTTOMLEFT )
        //.padding( 10, 0, 10, 0 )
        .background( true )
        .bg_color( 0x20000000 )
        .visible( false )
        ._( Row()
            .size( 0, 0 )
            .padding( 10, 10, 10, 10 )
            .gap( 10 )
            .background( false )
            .border( false )
            ._(MPButton( "ไม่มีปก", 0, [=](DecButton &btn) { pMagicPattern->setCollar( 0 ); } ))
            ._(MPButton( "ปกปลายแหลม", 0, [=](DecButton &btn) { pMagicPattern->setCollar( 6 ); } ))
        )
    );
    _( Scroller()
        .ref( (DecWidget **)&pPanelSleeve )
        .ID( "PanelSleeve" )
        .size( -1, 0 )
        .align( ALIGN_BOTTOMLEFT )
        //.padding( 10, 0, 10, 0 )
        .background( true )
        .bg_color( 0x20000000 )
        .visible( false )
        ._( Row()
            .size( 0, 0 )
            .padding( 10, 10, 10, 10 )
            .gap( 10 )
            .background( false )
            .border( false )
            ._(MPButton( "แขนสั้น", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 2 ); } ))
            ._(MPButton( "แขนสามส่วน", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 3 ); } ))
            ._(MPButton( "แขนยาว", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 4 ); } ))               
            ._(MPButton( "แขนทิวลิป", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 5 ); } ))               
            ._(MPButton( "แขน cap", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 6 ); } ))
            ._(MPButton( "แขนย้วย", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 7 ); } ))
            ._(MPButton( "แขน lantern", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 8 ); } ))                
            ._(MPButton( "แขน Flounce", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 9 ); } ))
            ._(MPButton( "แขนมารี", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 10 ); } ))
            ._(MPButton( "แขนผ่า", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 11 ); } ))               
            ._(MPButton( "แขนตุ๊กตา", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 12 ); } ))               
            ._(MPButton( "แขนระบาย", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 13 ); } ))
            ._(MPButton( "แขนกุด", 0, [=](DecButton &btn) { pMagicPattern->setSleeve( 1 ); } ))
        )
    );
    _( Scroller()
        .ref( (DecWidget **)&pPanelHem )
        .ID( "PanelHem" )
        .size( -1, 0 )
        .align( ALIGN_BOTTOMLEFT )
        //.padding( 10, 0, 10, 0 )
        .background( true )
        .bg_color( 0x20000000 )
        .visible( false )
        ._( Row()
            .size( 0, 0 )
            .padding( 10, 10, 10, 10 )
            .gap( 10 )
            .background( false )
            .border( false )
            ._(MPButton( "ไม่มีระบาย", 0, [=](DecButton &btn) { pMagicPattern->setHem( 0 ); } ))
            ._(MPButton( "ระบาย 10 ซ.ม.", 0, [=](DecButton &btn) { pMagicPattern->setHem( 1 ); } ))
            ._(MPButton( "ระบาย 20 ซ.ม.", 0, [=](DecButton &btn) { pMagicPattern->setHem( 2 ); } ))
        )
    );    
    _( Row()
        .ref( (DecWidget **)&pPanelPocket )
        .size( -1, 96 )
        .background( true )
        .bg_color( 0x20000000 )
        .border( false )
        .visible( false )
        .padding( 8, 0, 8, 8 )
        .gap( 8 )
        ._( Stack()
            .ref( (DecWidget **)&pPocketContainer )
            .size( 78, 82 )
            .background( false )
            .border( false )
            ._( Image( "_magicpattern/icons/PocketNone.png" )
                .ref( (DecWidget **)&pPocketInactive[0] )
                .align( ALIGN_TOPLEFT ) 
                .background( false )
                .border( false )
                .onMousePressed( [=](DecWidget &w) -> bool {
                    setAcitvePocket( 0, 0, true );
                    return true;
                })
            )
            ._( Image( "_magicpattern/icons/PocketSelected.png" )
                .ref( (DecWidget **)&pPocketActive[0] )
                .align( ALIGN_TOPLEFT ) 
                .background( false )
                .border( false )
                .visible( false )
                .onMousePressed( [=](DecWidget &w) -> bool {
                    setAcitvePocket( 0, 0, false );
                    return true;
                })
            )            
            ._( Image( "_magicpattern/icons/PocketNone.png" ) 
                .ref( (DecWidget **)&pPocketInactive[1] )
                .align( ALIGN_TOPRIGHT ) 
                .background( false )
                .border( false )
                .onMousePressed( [=](DecWidget &w) -> bool {
                    setAcitvePocket( 0, 1, true );
                    return true;
                })            
            )
            ._( Image( "_magicpattern/icons/PocketSelected.png" )
                .ref( (DecWidget **)&pPocketActive[1] )
                .align( ALIGN_TOPRIGHT ) 
                .background( false )
                .border( false )
                .visible( false )
                .onMousePressed( [=](DecWidget &w) -> bool {
                    setAcitvePocket( 0, 1, false );
                    return true;
                })
            )            
            ._( Image( "_magicpattern/icons/PocketNone.png" )
                .ref( (DecWidget **)&pPocketInactive[2] )
                .align( ALIGN_BOTTOMLEFT ) 
                .background( false )
                .border( false )
                .onMousePressed( [=](DecWidget &w) -> bool {
                    setAcitvePocket( 1, 0, true );
                    return true;
                })            
            )
            ._( Image( "_magicpattern/icons/PocketSelected.png" )
                .ref( (DecWidget **)&pPocketActive[2] )
                .align( ALIGN_BOTTOMLEFT ) 
                .background( false )
                .border( false )
                .visible( false )
                .onMousePressed( [=](DecWidget &w) -> bool {
                    setAcitvePocket( 1, 0, false );
                    return true;
                })
            )
            ._( Image( "_magicpattern/icons/PocketNone.png" ) 
                .ref( (DecWidget **)&pPocketInactive[3] )
                .align( ALIGN_BOTTOMRIGHT ) 
                .background( false )
                .border( false )
                .onMousePressed( [=](DecWidget &w) -> bool {
                    setAcitvePocket( 1, 1, true );
                    return true;
                })  
            )
            ._( Image( "_magicpattern/icons/PocketSelected.png" )
                .ref( (DecWidget **)&pPocketActive[3] )
                .align( ALIGN_BOTTOMRIGHT ) 
                .background( false )
                .border( false )
                .visible( false )
                .onMousePressed( [=](DecWidget &w) -> bool {
                    setAcitvePocket( 1, 1, false );
                    return true;
                })
            )
        )
        ._( Container()
            .size( 2, 60 )
            .align( ALIGN_CENTER )
            .background( true )
            .border( false )
            .bg_color( 0x40000000 )
        )
        ._( Stack() 
            .background( false )
            .border(false)
            ._(Column()
                .ref( (DecWidget **)&pPocketInfo )
                .background( true )
                .bg_color( 0xA0FFFFFF )
                .size( 0, 0 )
                .align( ALIGN_CENTER ) 
                .padding( 10, 10, 10, 10 )
                ._(Text( "การแสดงกระเป๋าเพื่อเป็นแนวทางออกแบบเท่านั้น") )
                ._(Text( "แพทเทิร์นเสื้อและเดรสไม่รวมแพทเทิร์นกระเป๋าค่ะ") )
            )
            ._(Row()
                .ref( (DecWidget **)&pPocketCatContainer )
                .background( false )
                .border( false )
                .visible( false )
                .gap( 10 )
                ._( Stack()
                    .size( 35, 82 )
                    .background( false )
                    .border( false )  
                    ._( Image( "_magicpattern/icons/PocketCover.png" ) 
                        .align( ALIGN_TOPLEFT ) 
                        .background( false )
                        .border( false )
                    )            
                    ._( Image( "_magicpattern/icons/PocketBody.png" ) 
                        .align( ALIGN_BOTTOMLEFT ) 
                        .background( false )
                        .border( false )
                    )            
                )
                ._( Column()
                    //.ref( (DecWidget **)&pPocketTypeContainer )
                    .size( -1, 0 )
                    .gap( 6 )
                    .border( false )
                    .background( false )
                    .align( ALIGN_CENTER )
                    ._( Scroller()
                        .size( -1, 0 )
                        .border( false )
                        .align( ALIGN_TOPLEFT )
                        ._( Row()
                            .size( 0, 0 )
                            .border( false )
                            .background( false )
                            .gap( 4 )
                            .padding( 0, 8, 0, 0 )
                            ._(MPButton( "ไม่มีฝากระเป๋า", 0, [=](DecButton &btn) { pMagicPattern->setPocketCover( currentPocketLevel, currentPocketSide, 0 ); } ))
                            ._(MPButton( "เหลี่ยม", 0, [=](DecButton &btn) { pMagicPattern->setPocketCover( currentPocketLevel, currentPocketSide, 1 ); } ))
                            ._(MPButton( "มน", 0, [=](DecButton &btn) { pMagicPattern->setPocketCover( currentPocketLevel, currentPocketSide, 2 ); } ))
                            ._(MPButton( "แหลม", 0, [=](DecButton &btn) { pMagicPattern->setPocketCover( currentPocketLevel, currentPocketSide, 3 ); } ))
                            ._(MPButton( "หกเหลี่ยม", 0, [=](DecButton &btn) { pMagicPattern->setPocketCover( currentPocketLevel, currentPocketSide, 4 ); } ))
                        )                 
                    )
                    ._( Scroller()
                        .size( -1, 0 )
                        .border( false )
                        .align( ALIGN_BOTTOMLEFT )
                        ._( Row()
                            .size( 0, 0 )
                            .border( false )
                            .background( false )
                            .gap( 4 )
                            .padding( 0, 8, 0, 0 )
                            ._(MPButton( "ไม่มีกระเป๋า", 0, [=](DecButton &btn) { pMagicPattern->setPocket( currentPocketLevel, currentPocketSide, 0 ); } ))
                            ._(MPButton( "เหลี่ยม", 0, [=](DecButton &btn) { pMagicPattern->setPocket( currentPocketLevel, currentPocketSide, 1 ); } ))
                            ._(MPButton( "โค้ง", 0, [=](DecButton &btn) { pMagicPattern->setPocket( currentPocketLevel, currentPocketSide, 2 ); } ))
                            ._(MPButton( "หกเหลี่ยม", 0, [=](DecButton &btn) { pMagicPattern->setPocket( currentPocketLevel, currentPocketSide, 3 ); } ))
                            ._(MPButton( "แหลม", 0, [=](DecButton &btn) { pMagicPattern->setPocket( currentPocketLevel, currentPocketSide, 4 ); } ))
                            ._(MPButton( "จีบ Box สี่เหลี่ยม", 0, [=](DecButton &btn) { pMagicPattern->setPocket( currentPocketLevel, currentPocketSide, 5 ); } ))
                            ._(MPButton( "จีบ Box โค้ง", 0, [=](DecButton &btn) { pMagicPattern->setPocket( currentPocketLevel, currentPocketSide, 6 ); } ))
                            ._(MPButton( "จีบ Box หกเหลี่ยม", 0, [=](DecButton &btn) { pMagicPattern->setPocket( currentPocketLevel, currentPocketSide, 7 ); } ))
                            ._(MPButton( "จีบคว่ำสี่เหลี่ยม", 0, [=](DecButton &btn) { pMagicPattern->setPocket( currentPocketLevel, currentPocketSide, 8 ); } ))
                            ._(MPButton( "จีบคว่ำโค้ง", 0, [=](DecButton &btn) { pMagicPattern->setPocket( currentPocketLevel, currentPocketSide, 9 ); } ))
                            ._(MPButton( "จีบคว่ำหกเหลี่ยม", 0, [=](DecButton &btn) { pMagicPattern->setPocket( currentPocketLevel, currentPocketSide, 10 ); } ))
                        ) 
                    )
                )            
            )
        )
    );
}

DecMPPanel::~DecMPPanel() {

}

DecMPPanel &MPPanel( DecMagicPattern *pMP, int h ) {
    DecMPPanel *w = new DecMPPanel( pMP, h );
    return *w;   
}

void DecMPPanel::setAcitvePocket( int level, int side, bool active ) {
    int o = (currentPocketLevel * 2) + currentPocketSide;
    int n = (level*2)+side;

    if( active ) {
        if( o >= 0 ) {
            pPocketActive[o]->hide();
            pPocketInactive[o]->show();
        }
        SkDebugf( "Active: %i\n", n );
        currentPocketLevel = level;
        currentPocketSide = side;
        pPocketActive[n]->show();
        pPocketInactive[n]->hide();

        pPocketInfo->hide();
        pPocketCatContainer->show();
        //pPocketTypeContainer->show();

    }
    else {
        SkDebugf( "Inactive: %i\n", n );
        currentPocketLevel = -1;
        currentPocketSide = -1;
        pPocketActive[n]->hide();
        pPocketInactive[n]->show();

        pPocketCatContainer->hide();
        //pPocketTypeContainer->hide();
        pPocketInfo->show();

    }
    pPanelPocket->invalidate();
    pPanelPocket->dirty( true, true );
}

void DecMPPanel::setMode( int n ) {
    //SkDebugf( "SetMode: %i %i\n", n, nMode );
    if( n == -1 ) {
        hide();
        dirty( true, true );
    }
    if( n == nMode ) {
        hide();
        dirty( true, true );
        if( n == 0 ) {
            pPanelType->hide();
        }
        else if( n == 1 ) {
            pPanelLength->hide();
        }
        else if( n == 2 ) {
            pPanelNeck->hide();
        }
        else if( n == 3 ) {
            pPanelCollar->hide();
        }
        else if( n == 4 ) {
            pPanelSleeve->hide();
        }
        else if( n == 5 ) {
            pPanelHem->hide();
        }
        else if( n == 6 ) {
            pPanelCollar2->hide();
        }
        else if( n == 7 ) {
            pPanelPocket->hide();
        }
        nMode = -1;
    }
    else {       
        if( n == 0 ) {
            SkDebugf( "Show Type\n" );
            pPanelType->show();
            pPanelType->dirty( true, true );
        }
        else if( n == 1 ) {
            SkDebugf( "Show Length\n" );
            pPanelLength->show();
            pPanelLength->dirty( true, true );
        }
        else if( n == 2 ) {
            SkDebugf( "Show Neck\n" );
            pPanelNeck->reset();
            pPanelNeck->show();
            pPanelNeck->dirty( true, true );
        }
        else if( n == 3 ) {
            SkDebugf( "Show Collar\n" );
            pPanelCollar->reset();
            pPanelCollar->show();
            pPanelCollar->dirty( true, true );
        }
        else if( n == 4 ) {
            SkDebugf( "Show Sleeve\n" );
            pPanelSleeve->show();
            pPanelSleeve->dirty( true, true );
        }
        else if( n == 5 ) {
            SkDebugf( "Show Hem\n" );
            pPanelHem->show();
            pPanelHem->dirty( true, true );
        }
        else if( n == 6 ) {
            SkDebugf( "Show Collar2\n" );
            pPanelCollar2->reset();
            pPanelCollar2->show();
            pPanelCollar2->dirty( true, true );
        }  
        else if( n == 7 ) {
            SkDebugf( "Show Pocket\n" );
            pPanelPocket->show();
            pPanelPocket->dirty( true, true );
        }  
        if( nMode == 0 ) {
            pPanelType->hide();
        }
        else if( nMode == 1 ) {
            pPanelLength->hide();
        }
        else if( nMode == 2 ) {
            pPanelNeck->hide();
        }
        else if( nMode == 3 ) {
            pPanelCollar->hide();
        }
        else if( nMode == 4 ) {
            pPanelSleeve->hide();
        }
        else if( nMode == 5 ) {
            pPanelHem->hide();
        }
        else if( nMode == 6 ) {
            pPanelCollar2->hide();
        }   
        else if( nMode == 7 ) {
            pPanelPocket->hide();
        }   
        nMode = n;      
        show();
        invalidate();
        dirty( true, true );
    }
}

void DecMPPanel::ShowNeck7( bool show ) {
    if( !pButtonNeck7 ) 
        return;

    if( show ) {
        pButtonNeck7->show();
    }
    else {
        pButtonNeck7->hide();
    }
    pButtonNeck7->dirty( true, true );
}

void DecMPPanel::ShowCollar4( bool show ) {
    if( !pButtonCollar4 )
        return;
    
    if( show ) {
        pButtonCollar4->show();
    }
    else {
        pButtonCollar4->hide();
    }
    pButtonCollar4->dirty( true, true );
}


