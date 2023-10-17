#include "DecMPPanel.h"
#include "DeclarativeUI.h"
#include "DecMagicPattern.h"
#include "DecUI.h"

DecMPPanel::DecMPPanel() {
    background( false );
    border( false );
    _testDraw = false;

    _pMagicPattern = NULL;

    pModeWidget = NULL;
    pDesignWidget = NULL;   
    pNeck = NULL;
    pCollarType = NULL;

    _shirt_mode = 0;

    _shirt_length = 0;
    _shirt_type = 0;
    _shirt_neck = 0;
    _shirt_collar = 0;
    _shirt_sleeve = 0;
    _shirt_hem = 0;

    _shirt_body_color = 0xFFFFFFFF;
    _shirt_sleeve_color = 0xAFFFFFFF;
    _shirt_hem_color = 0xAFFFFFFF;
    _shirt_collar_color = 0xFFFFFFFF;

    updateShirtBody();

    container = new DecColumnContainer();
    container->font(Font()
        .Regular("fonts/Kanit-Regular.ttf")
        .Icon("fonts/Font Awesome 6 Pro-Regular-400.otf")
    );    
    container->size( -1, 0 );
    container->border( false );
    container->background( false );
    container->gap( 5 );
    container->_(
        Row()
            .size( -1, 32 )
            .gap( 5 )
            .border( false )
            .background( false )
            ._( Button( "", 0xf060, [=](DecButton &btn) {
                this->pModeWidget->previous();
                this->pDesignWidget->previous();
                _shirt_mode = this->pModeWidget->getActivateIndex();
            } ).border_color(0xFF808080).gap(0) )
            ._( Pages().size( -1, -1 ) 
                .border( false )
                .background( true )
                .bg_color( 0xFFE0E0E0 )
                .ref( (DecWidget **)&pModeWidget )
                ._(Text( "รูปแบบการสวมใส่" ).align( ALIGN_CENTER ))
                ._(Text( "ความยาวเสื้อ" ).align( ALIGN_CENTER ))
                ._(Text( "คอเสื้อ" ).align( ALIGN_CENTER ))
                ._(Text( "ปกเสื้อ" ).align( ALIGN_CENTER ))
                ._(Text( "แขนเสื้อ" ).align( ALIGN_CENTER ))
                ._(Text( "ระบายชายเสื้อ" ).align( ALIGN_CENTER ))
            )
            ._( Button( "", 0xf061, [=](DecButton &btn) {
                this->pModeWidget->next();
                this->pDesignWidget->next();
                _shirt_mode = this->pModeWidget->getActivateIndex();
            }).border_color(0xFF808080).gap(0) )
    );
    container->_(
        Pages()
            .size( -1, 0 )
            .gap( 10 )
            .border( false )
            .background( false )
            .ref( (DecWidget **)&pDesignWidget )
            ._(Row()
                .size( -1, 0 ).gap( 10 ).border(false).background(false)
                ._(Spacer().size( -1, -1 ) )
                ._(Button( "แบบสวมหัว", 0, [=](DecButton &btn) { this->setShirtType( 0 ); } ))
                ._(Button( "แบบผ่าหน้า", 0, [=](DecButton &btn) { this->setShirtType( 1 ); }  ))
                ._(Spacer().size( -1, -1 ))
            )   
            ._( Scroller( SCROLL_HORIZONTAL ) 
                .size( -1, 0 )                 
                ._(Row()
                    .size( 0, 0 ).gap( 10 ).border(false).background(false)
                    ._(Button( "ระดับเอว", 0, [=](DecButton &btn) { this->setShirtLength( 0 ); } ))
                    ._(Button( "ระดับสะโพกสูง", 0, [=](DecButton &btn) { this->setShirtLength( 1 ); }  ))
                    ._(Button( "ระดับสะโพกล่าง", 0, [=](DecButton &btn) { this->setShirtLength( 2 ); }  ))
                ) 
            )  
            ._( Scroller( SCROLL_HORIZONTAL ) 
                .size( -1, 0 )              
                ._(Row()
                    .size( 0, 0 ).gap( 5 ).border(false).background(false)
                    ._(Button( "คอกลม", 0, [=](DecButton &btn) { this->setShirtNeck( 0 ); } ))
                    ._(Button( "คอวี", 0, [=](DecButton &btn) { this->setShirtNeck( 1 ); } ))
                    ._(Button( "คอปาด", 0, [=](DecButton &btn) { this->setShirtNeck( 2 ); } ))
                    ._(Button( "คอสี่เหลี่ยม", 0, [=](DecButton &btn) { this->setShirtNeck( 3 ); } ))
                    ._(Button( "คอคว้าน", 0, [=](DecButton &btn) { this->setShirtNeck( 4 ); } ))
                )  
            )               
            ._(Pages().size( -1, 0 ).border(false).background(false)
                .ref( (DecWidget **)&pNeck )
                ._(Pages()
                    .size( -1, 0 ).gap( 5 ).border(false).background(false)
                    .ref( (DecWidget**)&pCollarType )
                    ._( Scroller( SCROLL_HORIZONTAL ) 
                        .size( -1, 0 )
                        ._( Row()
                            .size( 0, 0 ).gap( 5 ).border(false).background(false)
                            ._(Button( "ไม่มีปก", 0, [=](DecButton &btn) { this->setShirtCollar( 0 ); } ))
                            ._(Button( "ปกบัว", 0, [=](DecButton &btn) { this->setShirtCollar( 1 ); } ))
                            ._(Button( "ปกบัวแหลม", 0, [=](DecButton &btn) { this->setShirtCollar( 2 ); } ))
                            ._(Button( "ปกคอจีน", 0, [=](DecButton &btn) { this->setShirtCollar( 3 ); } ))
                            ._(Button( "ปกระบาย", 0, [=](DecButton &btn) { this->setShirtCollar( 4 ); } ))
                            ._(Button( "ปกผูกโบว์", 0, [=](DecButton &btn) { this->setShirtCollar( 5 ); } ))
                        )
                    )
                    ._( Scroller( SCROLL_HORIZONTAL ) 
                        .size( -1, 0 )
                        ._( Row()
                            .size( 0, 0 ).gap( 5 ).border(false).background(false)
                            ._(Button( "ไม่มีปก", 0, [=](DecButton &btn) { this->setShirtCollar( 0 ); } ))
                            ._(Button( "ปกบัว", 0, [=](DecButton &btn) { this->setShirtCollar( 1 ); } ))
                            ._(Button( "ปกบัวแหลม", 0, [=](DecButton &btn) { this->setShirtCollar( 2 ); } ))
                            ._(Button( "ปกคอจีน", 0, [=](DecButton &btn) { this->setShirtCollar( 3 ); } ))
                            ._(Button( "ปกผูกโบว์", 0, [=](DecButton &btn) { this->setShirtCollar( 5 ); } ))
                        )
                    )
                )          
                ._( Row()
                    .size( -1, 0 ).gap( 5 ).border(false).background(false)
                    ._(Spacer().size( -1, -1 ) )
                    ._(Button( "ไม่มีปก", 0, [=](DecButton &btn) { this->setShirtCollar( 0 ); } ))
                    ._(Button( "ปกปลายแหลม", 0, [=](DecButton &btn) { this->setShirtCollar( 6 ); } ))
                    ._(Spacer().size( -1, -1 ) )
                )
                ._( Row()
                    .size( -1, 0 ).gap( 5 ).border(false).background(false)
                    ._(Spacer().size( -1, -1 ) )
                    ._(Button( "ไม่มีปก", 0, [=](DecButton &btn) { this->setShirtCollar( 0 ); } ))
                    ._(Spacer().size( -1, -1 ) )
                )
            )
            ._(Column()
                .size( -1, 0 ).gap( 5 ).border(false).background(false)
                ._( Scroller( SCROLL_HORIZONTAL ) 
                    .size( -1, 0 )
                    ._( Row()
                        .size( 0, 0 ).gap( 5 ).border(false).background(false)
                        ._(Button( "แขนสั้น", 0, [=](DecButton &btn) { this->setShirtSleeve( 1 ); } ))
                        ._(Button( "แขนสามส่วน", 0, [=](DecButton &btn) { this->setShirtSleeve( 2 ); } ))
                        ._(Button( "แขนยาว", 0, [=](DecButton &btn) { this->setShirtSleeve( 3 ); } ))               
                        ._(Button( "แขนทิวลิป", 0, [=](DecButton &btn) { this->setShirtSleeve( 4 ); } ))               
                        ._(Button( "แขน cap", 0, [=](DecButton &btn) { this->setShirtSleeve( 5 ); } ))
                        ._(Button( "แขนย้วย", 0, [=](DecButton &btn) { this->setShirtSleeve( 6 ); } ))
                        ._(Button( "แขน lantern", 0, [=](DecButton &btn) { this->setShirtSleeve( 7 ); } ))                
                        ._(Button( "แขน Flounce", 0, [=](DecButton &btn) { this->setShirtSleeve( 8 ); } ))
                        ._(Button( "แขนมารี", 0, [=](DecButton &btn) { this->setShirtSleeve( 9 ); } ))
                        ._(Button( "แขนผ่า", 0, [=](DecButton &btn) { this->setShirtSleeve( 10 ); } ))               
                        ._(Button( "แขนตุ๊กตา", 0, [=](DecButton &btn) { this->setShirtSleeve( 11 ); } ))               
                        ._(Button( "แขนระบาย", 0, [=](DecButton &btn) { this->setShirtSleeve( 12 ); } ))
                        ._(Button( "แขนกุด", 0, [=](DecButton &btn) { this->setShirtSleeve( 0 ); } ))
                    )                
                )
            )
            ._(Row()
                .size( -1, 0 ).gap( 5 ).border(false).background(false)
                ._(Spacer().size( -1, -1 ) )
                ._(Button( "ไม่มีระบาย", 0, [=](DecButton &btn) { this->setShirtHem( 0 ); } ))
                ._(Button( "ระบาย 10 ซ.ม.", 0, [=](DecButton &btn) { this->setShirtHem( 1 ); } ))
                ._(Button( "ระบาย 20 ซ.ม.", 0, [=](DecButton &btn) { this->setShirtHem( 2 ); } ))
                ._(Spacer().size( -1, -1 ) )
            )    
        );   
    container->_( 
        //Scroller( SCROLL_HORIZONTAL ) 
        //    .size( -1, 0 )
        //    
            Scroller( SCROLL_HORIZONTAL ) 
                .size( -1, 0 )  
                ._( Row()
                    .padding_bottom( 10 )
                    .size( 0, 0 ).gap( 2 ).border(false).background(false)
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
            );

        //);     

    this->_( *container );
}

DecMPPanel::~DecMPPanel() {

}

DecMPPanel &MagicPatternPanel() {
    DecMPPanel *w = new DecMPPanel();
    return *w;   
}

void DecMPPanel::setMagicPatternWidget( DecMagicPattern *pMagicPattern ) { 
    _pMagicPattern = pMagicPattern; 
    updateShirtBody();
}

void DecMPPanel::updateShirtBody() {
    SkDebugf( "UpdateShirtBody\n" );
    _shirt_body_index = (_shirt_length * 10) + (_shirt_type * 5) + _shirt_neck;    
    if( _pMagicPattern ) {
        _pMagicPattern->setBody( _shirt_body_index );
    }
    updateShirtHem();
}

void DecMPPanel::updateShirtSleeve() {
    if( _pMagicPattern ) {
        _pMagicPattern->setSleeve( _shirt_sleeve );
    }    
}

void DecMPPanel::updateShirtCollar() {
    if( _pMagicPattern ) {
        _pMagicPattern->setCollar( _shirt_collar );
    }    
}

void DecMPPanel::updateShirtHem() {
    SkDebugf( "updateShirtHem\n" );
    if( _pMagicPattern ) {
        _pMagicPattern->setHem( _shirt_hem );
    }    
}

void DecMPPanel::updateShirtColor() {
    SkDebugf( "updateShirtColor\n" );
    if( _pMagicPattern ) {
        _pMagicPattern->setColor( _shirt_body_color, _shirt_collar_color, _shirt_sleeve_color, _shirt_hem_color );
    }    
}

void DecMPPanel::setShirtType( int i ) {
    SkDebugf( "SetShirtType\n" );
    _shirt_type = i;
    updateShirtBody();
    if( pCollarType ) {
        pCollarType->activate( _shirt_type );
    }
}

void DecMPPanel::setShirtLength( int i ) {
    _shirt_length = i;
    updateShirtBody();
}

void DecMPPanel::setShirtNeck( int i ) {
    _shirt_neck = i;
    if( pNeck ) {
        if( i == 0 ) {
            pNeck->activate( 0 );
        }
        else if( i == 1 ) {
            pNeck->activate( 1 );
        }
        else {
            pNeck->activate( 2 );
        }
    }
    setShirtCollar(0);
    updateShirtBody();
}

void DecMPPanel::setShirtCollar( int i ) {
    _shirt_collar = i;
    updateShirtCollar();
    updateShirtBody();
}

void DecMPPanel::setShirtSleeve( int i ) {
    _shirt_sleeve = i;
    updateShirtSleeve();
    updateShirtBody();
}

void DecMPPanel::setShirtHem( int i ) {
    _shirt_hem = i;
    updateShirtHem();
    updateShirtBody();
}

void DecMPPanel::setColor( int c ) {
    SkDebugf( "SetColor: %i %i\n", c, _shirt_mode );
    if( _shirt_mode == 5 ) {
        _shirt_hem_color = c;
    }
    else if( _shirt_mode == 4 ) {
        if( c == 0xE8FBFBF9 ) {
            c = (c & 0x00FFFFFF) | 0xAFFFFFFF;
        }
        _shirt_sleeve_color = c;
    }
    else if( _shirt_mode == 3 ) {
        if( _shirt_collar > 0 ) {
            _shirt_collar_color = c | 0xFF000000;
        }
        else {
            _shirt_body_color = c;
            _shirt_collar_color = c | 0xFF000000;
        }
    }
    else {
        _shirt_collar_color = c | 0xFF000000;
        _shirt_body_color = c;
    }
    updateShirtColor();
}

void DecMPPanel::setShirtParams( int type, int length, int neck, int collar, int sleeve, int hem ) {
    SkDebugf( "setShirtParams\n" );
    //_shirt_collar = 0;

    _shirt_type = type;
    _shirt_length = length;
    _shirt_neck = neck;
    _shirt_sleeve = sleeve;
    _shirt_hem = hem;
    _shirt_collar = collar;
    _shirt_body_index = (_shirt_length * 10) + (_shirt_type * 5) + _shirt_neck;

    if( pNeck ) {
        if( _shirt_neck == 0 ) {
            pNeck->activate( 0 );
        }
        else if( _shirt_neck == 1 ) {
            pNeck->activate( 1 );
        }
        else {
            pNeck->activate( 2 );
        }
    }    
}
