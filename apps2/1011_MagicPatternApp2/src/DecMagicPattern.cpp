
#define MAGIGPATTERN_VER_1_0 "1.0"

#define MAGIGPATTERN_VER MAGIGPATTERN_VER_1_0



#include <chrono>
#include <fstream>
#include <string>
#include <iostream>
#include <time.h>
#include "DecStackContainer.h"
#include "DecMagicPattern.h"
#include "DeclarativeUI.h"
#include "DecMPPanel.h"
#include "DecWidget.h"

uint32_t _randomColors[] = {
    0xE8CBC9CC,
    0xE82D435B,
    0xE89CB2BF,
    0xE8638181,
    0xE8919F88,
    0xE8202022,
    0xE88B2135,
    0xE8612C40,
    0xE8755D75,
    0xE8DBB9C4,
    0xE8E4B366,
    0xE8DB976A,
    0xE8B5927E,
    0xE8F7E1CC,
    0xE8FBFBF9
};
const int _numRandomColors = sizeof( _randomColors ) / sizeof( _randomColors[0] );

DecMagicPattern::DecMagicPattern( std::string option ) {
    SkDebugf( "MagicPattern %s\n", option.c_str() );
    _queryProductID = 0;
    if( option.length() ) {
        int i = atoi( option.c_str() );
        if( i >= 1 && i <= 2457 ) {
            _queryProductID = i;
        }
    }

    background( false );
    border( false );
    _testDraw = false;

    _body = 0;
    _sleeve = 0;
    _hem = 0;
    _collar = 0;
    _hair = 0;

    _pBody = NULL;
    _pSleeve = NULL;
    _pHemType = NULL;
    _pHem0 = NULL;
    _pHem1 = NULL;
    _pMPPanel = NULL;
    _pCollarType = NULL;
    _pCollar0 = NULL;
    _pCollar1 = NULL;
    _pHuman = NULL;
    _pHand = NULL;
    _pBackground = NULL;
    _pHair = NULL;
    _pRandomButton = NULL;

    _sku[0] = 0;

    _generateProduct = false;

    container = new DecColumnContainer();
    //container->font(Font()
    //    .Regular("fonts/Kanit-Regular.ttf")
    //    .Icon("fonts/Font Awesome 6 Pro-Regular-400.otf")
    //);    
    container->size( -1, -1 );
    container->border( false );
    container->background( false );

    container->_( Row() 
        .size( -1, 0 )
        .border( false )
        .background( false )
        .margin( 5, 0, 0, 0 )
        .padding( 0, 5, 0, 5 )
        .gap( 10 )
        ._( Icon( 0xf002 )
            .align( ALIGN_CENTER )
            .fontSize( 16 )
            .onMousePressed([=]( DecWidget &w ) -> bool {
                pKeyPad->visible( !pKeyPad->visible() );
                searchString = "";
                _pTextSearch->text( searchString.c_str() );
                return true;
            }) 
        )        
        ._( Text( "" ).ref( (DecWidget **)&_pProductCode ).size( 80, 0 ).align( ALIGN_CENTER ) )
        ._( Text( "" ).ref( (DecWidget **)&_pSKU ).align( ALIGN_CENTER ) )
        ._( Spacer().size( -1, 0 ) )
        ._( Icon( 0xf07a )
            .align( ALIGN_CENTER )
            .fontSize( 16 )
            .onMousePressed([=]( DecWidget &w ) -> bool {
                getPattern();
                return true;
            }) 
        )
    );
    
    container->_( Stack() 
        .ref( (DecWidget **)&_pMainImage )
        .size( -1, -1 )
        .margin( 5, 5, 5, 5 )
        .round( 10 )
        .max_height( 420 )
        .border_color( 0xFFA0A0A0 )
        .bg_color( 0xFF808080 )
        ._( Pages() 
            .size( -1, -1 )
            .align( ALIGN_CENTER )
            .background( false )
            .border( false )
            .ref( (DecWidget **)&_pBackground )     
            ._( Image( "_magic_pattern2//Background01-01.png" ).align( ALIGN_CENTER ).border( false ).size( -1, -1) )
            ._( Image( "_magic_pattern2//Background02-01.png" ).align( ALIGN_CENTER ).border( false ).size( -1, -1) )
            ._( Image( "_magic_pattern2//Background03-01.png" ).align( ALIGN_CENTER ).border( false ).size( -1, -1) )
            ._( Image( "_magic_pattern2//Background04-01.png" ).align( ALIGN_CENTER ).border( false ).size( -1, -1) )
        )   
        ._( Image( "_magic_pattern2//Body-01.png" ).ref( (DecWidget **)&_pHuman ).align( ALIGN_CENTER ).background( false ).border( false ).size( -1, -1) )
        ._( Image( "_magic_pattern2//Pant-01.png" ).align( ALIGN_CENTER ).border( false ).size( -1, -1).background(false) )
        ._( Stack().border(false).background(false)
            ._( Pages() 
                .size( -1, -1 )
                .align( ALIGN_CENTER )
                .background( false )
                .border( false )
                .ref( (DecWidget **)&_pSleeve )
                ._( Image( "_magic_pattern2//Sleeve00-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve01-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve02-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve03-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve04-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve05-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve06-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve07-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve08-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve09-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve10-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve11-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Sleeve12-01.png" ).border( false ).background( false ).size( -1, -1 ) )
            )
            ._( Pages() 
                .size( -1, -1 )
                .align( ALIGN_CENTER )
                .background( false )
                .border( false )
                .subWidget( true )
                .ref( (DecWidget **)&_pBody )
                ._( Image( "_magic_pattern2//Shirt000-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt001-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt002-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt003-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt004-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt005-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt006-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt007-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt010-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt011-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt012-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt013-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt014-01.png" ).border( false ).background( false ).size( -1, -1 ) )                    
                ._( Image( "_magic_pattern2//Shirt015-01.png" ).border( false ).background( false ).size( -1, -1 ) )                    
                ._( Image( "_magic_pattern2//Shirt016-01.png" ).border( false ).background( false ).size( -1, -1 ) )                    
                ._( Image( "_magic_pattern2//Shirt100-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt101-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt102-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt103-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt104-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt105-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt106-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt107-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt110-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt111-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt112-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt113-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt114-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt115-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt116-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt200-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt201-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt202-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt203-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt204-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt205-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt206-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt207-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt210-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt211-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt212-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt213-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt214-01.png" ).border( false ).background( false ).size( -1, -1 ) )                                                   
                ._( Image( "_magic_pattern2//Shirt215-01.png" ).border( false ).background( false ).size( -1, -1 ) )                                                   
                ._( Image( "_magic_pattern2//Shirt216-01.png" ).border( false ).background( false ).size( -1, -1 ) )                                                   
                ._( Image( "_magic_pattern2//Shirt300-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt301-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt302-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt303-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt304-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt305-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt306-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt307-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt310-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt311-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt312-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt313-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt314-01.png" ).border( false ).background( false ).size( -1, -1 ) )                                                   
                ._( Image( "_magic_pattern2//Shirt315-01.png" ).border( false ).background( false ).size( -1, -1 ) )                                                   
                ._( Image( "_magic_pattern2//Shirt316-01.png" ).border( false ).background( false ).size( -1, -1 ) )                                                   
                ._( Image( "_magic_pattern2//Shirt400-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt401-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt402-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt403-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt404-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt405-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt406-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt407-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt410-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt411-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt412-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt413-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ._( Image( "_magic_pattern2//Shirt414-01.png" ).border( false ).background( false ).size( -1, -1 ) )                                                   
                ._( Image( "_magic_pattern2//Shirt415-01.png" ).border( false ).background( false ).size( -1, -1 ) )                                                   
                ._( Image( "_magic_pattern2//Shirt416-01.png" ).border( false ).background( false ).size( -1, -1 ) )                                                   
            ) 
            ._( Pages() 
                .size( -1, -1 )
                .align( ALIGN_CENTER )
                .background( false )
                .border( false )
                .ref( (DecWidget **)&_pHemType )  
                ._( Pages() 
                    .size( -1, -1 )
                    .align( ALIGN_CENTER )
                    .background( false )
                    .border( false )  
                    .ref( (DecWidget **)&_pHem0 )   
                    ._( Image( "_magic_pattern2//Base000-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base001-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base002-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base100-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base101-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base102-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base200-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base201-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base202-01.png" ).border( false ).background( false ).size( -1, -1 ) )

                )    
                ._( Pages() 
                    .size( -1, -1 )
                    .align( ALIGN_CENTER )
                    .background( false )
                    .border( false )  
                    .ref( (DecWidget **)&_pHem1 )  
                    ._( Image( "_magic_pattern2//Base010-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base011-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base012-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base110-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base111-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base112-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base210-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base211-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Base212-01.png" ).border( false ).background( false ).size( -1, -1 ) )

                )                          
            )  
            ._( Pages() 
                .size( -1, -1 )
                .align( ALIGN_CENTER )
                .background( false )
                .border( false )
                .ref( (DecWidget **)&_pCollarType )                    
                ._( Pages() 
                    .size( -1, -1 )
                    .align( ALIGN_CENTER )
                    .background( false )
                    .border( false )
                    .ref( (DecWidget **)&_pCollar0 )
                    ._( Image( "_magic_pattern2//Collar00-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar01-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar02-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar03-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar04-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar05-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar06-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                ) 
                ._( Pages() 
                    .size( -1, -1 )
                    .align( ALIGN_CENTER )
                    .background( false )
                    .border( false )
                    .ref( (DecWidget **)&_pCollar1 )
                    ._( Image( "_magic_pattern2//Collar00-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar01-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar02-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar03-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar04-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar05-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                    ._( Image( "_magic_pattern2//Collar06-01.png" ).border( false ).background( false ).size( -1, -1 ) )
                )                 
            )
            ._( Pages() 
                .size( -1, -1 )
                .align( ALIGN_CENTER )
                .background( false )
                .border( false )
                .ref( (DecWidget **)&_pHair )     
                ._( Image( "_magic_pattern2//Hair01-01.png" ).background(false).align( ALIGN_CENTER ).border( false ).size( -1, -1) )
                ._( Image( "_magic_pattern2//Hair02-01.png" ).background(false).align( ALIGN_CENTER ).border( false ).size( -1, -1) )
                ._( Image( "_magic_pattern2//Hair03-01.png" ).background(false).align( ALIGN_CENTER ).border( false ).size( -1, -1) )
                ._( Image( "_magic_pattern2//Hair04-01.png" ).background(false).align( ALIGN_CENTER ).border( false ).size( -1, -1) )
                ._( Image( "_magic_pattern2//Hair05-01.png" ).background(false).align( ALIGN_CENTER ).border( false ).size( -1, -1) )
            )              
            ._( Image( "_magic_pattern2//Hand-01.png" ).ref( (DecWidget **)&_pHand ).align( ALIGN_CENTER ).border( false ).size( -1, -1).background(false) )
            ._( Button( "", 0xe2ca, [=](DecButton &btn) {
                random();
                updateMPPanel();
            })
                .ref( (DecWidget **)&_pRandomButton )
                .align( ALIGN_BOTTOMLEFT ) 
                .gap( 0 )
                .size( 40, 40 )
                .round( 20 )
                .margin( 10, 10, 10, 10 )
                .border_color( 0xFFA0A0A0 )
                .bg_color( 0xFFFFFFFF )
            )                    
        )
        ._( Image("images/MP-LOGO-s.png").background( false ).border( false )
            .align( ALIGN_TOPRIGHT ).margin( 5, 5, 5, 5 )
        )
        ._( Text( "#0000" )
            .ref( (DecWidget **)&_pTextProductID )
            .align( ALIGN_BOTTOMRIGHT ).margin( 5, 5, 5, 5 )
            .text_color( 0x50000000 ).fontSize( 12 )
        )
        ._( Column()
            .ref( (DecWidget **)&pKeyPad )
            .size( 0, 0 )
            .align( ALIGN_TOPLEFT )
            .round( 10 )
            .margin( 10, 10, 10, 10 )
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
                    return true;
                }).size( 32, 32 ).border_color( 0x80808080 ) )
            )        
        )
    );
    container->_( 
        MagicPatternPanel().ref( (DecWidget **)&_pMPPanel ).size( -1, 0 ).margin( 0, 10, 0, 10 )
    );

#ifndef __EMSCRIPTEN__    
    container->_( 
        Column()
            .size( -1, 0 )
            .border( false )
            .background( 0 )
            .margin_top( 20 )
            .gap(10)
            ._( Text( "Developer only" ))
            ._( Text( "(Images saved in ./images/)" ).margin( 0, 0, 10, 0 ) )
            ._( Row()
                .gap(10)
                .border( false )
                .size( -1, 0 )
                ._( Button( "Hide Random", 0, [=](DecButton &btn) {
                    _pRandomButton->visible( !_pRandomButton->visible() );
                } ))
                ._( Button( "Create PNG", 0, [=](DecButton &btn) {
                    char sz[128];
                    sprintf( sz, "_images/%s.png", _sku );
                    _pMainImage->capture( sz, false, true );
                } ))
                ._( Button( "Random PNG", 0, [=](DecButton &btn) {
                    random();
                } ))
            )
            ._( Row()
                .gap(10)
                .border( false )
                .size( -1, 0 )
                ._( Button( "Generate Product Images", 0, [=](DecButton &btn) {
                    _generateFrame = 1;
                    _generateType = 0;
                    _generateLength = 0;
                    _generateNeck = 0;
                    _generateHem = 0;
                    _generateSleeve = 0;
                    _generateCollar = 0;
                    _generateCount = 0;

                    _generateProduct = true;
                } ))                
                ._( Button( "Generate Product List", 0, [=](DecButton &btn) {
                    _generateType = 0;
                    _generateLength = 0;
                    _generateNeck = 0;
                    _generateHem = 0;
                    _generateSleeve = 0;
                    _generateCollar = 0;
                    _generateCount = 0;

                    std::ofstream f;
                
                    int png_count = 0;
                    while( true ) {
                        static char szx[8129];
                        if( _generateCount % 1000 == 0 ) {
                            if( _generateCount != 0 ) {
                                f.close();
                            }
                            char sz[128];
                            sprintf( sz, "_files/magicpattern_product2-%04i.csv", _generateCount );
                            f.open( sz );

                            sprintf( szx, "ID,Type,SKU,Name,Published,Is featured?,Visibility in catalog,Short description,Description,Date sale price starts,Date sale price ends,Tax status,Tax class,In stock?,Stock,Low stock amount,Backorders allowed?,Sold individually?,Weight (kg),Length (cm),Width (cm),Height (cm),Allow customer reviews?,Purchase note,Sale price,Regular price,Categories,Tags,Shipping class,Images,Download limit,Download expiry days,Parent,Grouped products,Upsells,Cross-sells,External URL,Button text,Position,Download 1 ID,Download 1 name,Download 1 URL,Download 2 ID,Download 2 name,Download 2 URL\n");
                            if( f ) {
                                f.write( (const char *)szx, strlen(szx) );
                            }
                        }
                        _generateCount++;      

                        int body = _generateLength * 15 + _generateType * 8 + _generateNeck;
                        setCollar( _generateCollar );          
                        setSleeve( _generateSleeve );
                        setHem( _generateHem );
                        setBody( body );
                        updateSKU(); 


                        static const char *desc_nect[] = { "คอกลม", "คอวี", "คอปาด", "คอสี่เหลี่ยม", "คอคว้าน", "คอสี่เหลี่ยมแคบ", "คอคว้านแคบ", "คอหยดน้ำมีโบว์" };
                        static const char *desc_collar[] = { "ไม่มีปก", "ปกบัว", "ปกบัวแหลม", "ปกคอจีน", "ปกระบาย", "ปกผูกโบว์", "ปกปลายแหลม" };
                        static const char *desc_type[] = { "สวมศรีษะ", "ผ่าหน้า" };
                        static const char *desc_length[] = { "เอว", "สะโพกสูง", "สะโพกล่าง", "เหนือเข่า", "คลุมเข่า" };
                        static const char *desc_sleeve[] = { "แขนกุด", "แขนสั้น", "แขนสามส่วน", "แขนยาว", "แขนทิวลิป", 
                            "แขน cap", "แขนย้วย", "แขน lantern", "แขน Flounce", "แขนมารี", "แขนผ่า", "แขนตุ๊กตา", "แขนระบาย" };
                        static const char *desc_hem[] = { "", "แต่งระบายชายเสื้อแบบสั้น ", "แต่งระบายชายเสื้อแบบยาว " };

                        static char szy[1024];
                        sprintf( szy, "แพทเทิร์นเสื้อ%s %s %s ความยาวระดับ%s %s %s<br><br>แพทเทิร์นมีเส้นแนวเย็บและเผื่อตะเข็บเย็บแล้ว<br><br>ท่านจะได้รับแพทเทิร์นทั้ง 5 ไซส์ (S, M, L, XL, XXL)\\n", 
                            desc_nect[_generateNeck],
                            desc_collar[_generateCollar],
                            desc_type[_generateType],
                            desc_length[_generateLength],
                            desc_sleeve[_generateSleeve],
                            desc_hem[_generateHem]
                        ); 

                        static char szShort[2048];
                        sprintf( szShort, "%s%s", "<span style='color: #ff0000;'>ราคาปกติ 400 บาท ซื้อ 2 แบบขึ้นไปลดเหลือแบบละ 250 บาท<br>ซื้อ 10 แบบขึ้นไป แถมฟรี 1 แบบ</span>\\n\\n", szy );

                        static char szBodyLength[512];
                            sprintf( szBodyLength, "<img class=""aligncenter wp-image-5034"" src=""https://magicpattern.co/wp-content/uploads/2022/07/size%c.png"" alt=""ขนาดตัวเสื้อสำเร็จ"" width=""640"" height=""221"" /></p>\\n", _sku[0] );


                        static char szSleeveLength[512];
                        if( _sku[6] != 'H' ) 
                            sprintf( szSleeveLength, "<img class=""aligncenter wp-image-5045"" src=""https://magicpattern.co/wp-content/uploads/2022/07/sleeve%c.png"" alt=""ขนาดแขนเสื้อสำเร็จ"" width=""640"" height=""105"" /></p>\\n", _sku[6] );
                        else 
                            szSleeveLength[0] = 0;

                        std::string strYouGot = std::string("<p>สิ่งที่ท่านจะได้รับ :</p><ol>") +
                            "<li>ไฟล์เอกสารหน้าปกและรายละเอียดแพทเทิร์น ขนาด A4</li>" + 
                            "<li>ไฟล์แพทเทิร์น ขนาดกระดาษ A4 แบบขาวดำ (สำหรับพิมพ์เพื่อทาบตัดผ้า) รวม 5 ไซส์ (5 ไฟล์)</li>" + 
                            "<li>ไฟล์แพทเทิร์น ขนาดกระดาษ A1 แบบขาวดำ (สำหรับพิมพ์เพื่อทาบตัดผ้า) รวม 5 ไซส์ (5 ไฟล์)</li>" +
                            "<li>ไฟล์แพทเทิร์น ขนาดกระดาษ A1 พิมพ์ 4 สีรวม 5 ไซส์ในไฟล์เดียว</li>" +
                            "<li>ลิงค์ดูวิดีโอวิธีการตัดเย็บ</li></ol>";

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
                        if( _sku[5] == 'A'  ) {
                            sprintf( szVideo1, "<br><h3 style='text-align: center;'>(ชุดนี้ไม่มีปก)</h3>\\n" );
                        }
                        else {
                            if( _sku[5] == 'E'  ) {
                                sprintf( szVideo1, "<br><h3 style='text-align: center;'>ปกเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/WojljAiy4tU"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
ขั้นตอนการตัดเย็บปกระบาย<br>\
00:00 Intro<br>\
00:19 เตรียมแพทเทิร์นก่อนเย็บ,วางผ้า,ตัดผ้า<br>\
01:02 การเย็บปกระบาย<br>\
04:16 การวางปกเสื้อ เพื่อเย็บติดกับคอเสื้อ<br>\
05:50 ปกระบายชิ้นสำเร็จ<br>\
<br>" );
                            }
                            else if( _sku[5] == 'F'  ) {
                                sprintf( szVideo1, "<br><h3 style='text-align: center;'>ปกเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/13j5tFoxw9E"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
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
                            else if( _sku[5] == 'G'  ) {
                                sprintf( szVideo1, "<br><h3 style='text-align: center;'>ปกเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/gVrsTAX-v5I"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
ขั้นตอนการตัดเย็บปกปลายแหลม<br>\
00:00 Intro<br>\
00:20 เตรียมแพทเทิร์นก่อนเย็บ,วางผ้า,ตัดผ้า<br>\
01:26 รีดผ้ากาวปกเสื้อ1ชิ้น<br>\
01:45 เย็บประกอบปกปลายแหลม<br>\
03:43 การวางปกเสื้อ เพื่อเย็บติดกับคอเสื้อ<br>\
05:14 ปกปลายแหลมชิ้นสำเร็จ<br>\
<br>" );
                            }
                            else {
                                sprintf( szVideo1, "<br><h3 style='text-align: center;'>ปกเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/vl5sL2W-Zco"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
ขั้นตอนการตัดเย็บปกบัว, ปกบัวแหลม, ปกคอจีน<br>\
00:00 Intro<br>\
00:26 เตรียมแพทเทิร์นก่อนเย็บ,วางผ้า,ตัดผ้า<br>\
02:25 ปกเสื้อสำหรับเสื้อผ่าหน้า<br>\
06:16 การวางปกเสื้อ เพื่อเย็บติดกับคอเสื้อแบบผ่าหน้า<br>\
07:36 ปกเสื้อสำหรับเสื้อสวมหัว<br>\
11:11 การวางปกเสื้อ เพื่อเย็บติดกับคอเสื้อแบบสวมหัว<br>\
<br>" );
                            }
                        }

                        static char szVideo2[20480];
                        if( _sku[1] == 'P'  ) {
                            sprintf( szVideo2, "<br><h3 style='width: 640px; max-width: 100%%;'>ตัวเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/EPw8niKu43o"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
- คลิปนี้สอนตัดเย็บตัวเสื้อแบบสวมหัว สำหรับคอทุกรูปแบบ ซึ่งเป็นการเก็บคอเสื้อด้วยการเย็บสาบ แบบมีปก หรือไม่มีปก ก็สามารถดูคลิปวิดีโอนี้ได้เลยค่ะ เสื้อคอกลมจะเป็นสาบแบบมีหยดน้ำด้านหลัง และสำหรับเสื้อคอวี คอปาด หรือคออื่นๆ เป็นสาบแบบปกติ(ไม่มีหยดน้ำ)<br>\
- สำหรับเสื้อที่มีปก เมื่อเย็บต่อไหล่แล้ว(ขั้นตอนที่ 3) ให้เย็บปกติดกับตัวเสื้อตามขั้นตอนที่ 4 และเย็บสาบทับลงไป ตามขั้นตอนที่ 5 จากนั้นทำต่อไปจนขั้นตอนที่ 8 ได้เลยค่ะ<br>\
- สำหรับเสื้อที่ไม่มีปก เมื่อเย็บต่อไหล่แล้ว(ขั้นตอนที่ 3) ให้ข้ามไปที่ขั้นตอนที่ 5 จากนั้นทำต่อไปจนขั้นตอนที่ 8 ได้เลยค่ะ<br>\
ขั้นตอนการตัดเย็บ<br>\
00:00 Intro<br>\
00:16 อธิบายรูปแบบการตัดเย็บของเสื้อสวมหัว<br>\
01:17 เตรียมแพทเทิร์นก่อนเย็บ,การวางผ้า,ตัดผ้า<br>\
03:09 ขั้นตอนที่ 1 รีดผ้ากาวที่สาบคอเสื้อ<br>\
03:40 ขั้นตอนที่ 2 เย็บต่อไหล่ตัวเสื้อ<br>\
05:01 ขั้นตอนที่ 3 เย็บต่อไหล่สาบคอเสื้อ<br>\
06:36 ขั้นตอนที่ 4 เย็บติดปกเสื้อ(สำหรับคอที่มีปก)<br>\
09:32 ขั้นตอนที่ 5 เย็บเก็บคอเสื้อด้วยสาบคอเสื้อ<br>\
09:43 5.1 คอเสื้อแบบมีหยดน้ำด้านหลัง<br>\
19:40 5.2 คอเสื้อที่ไม่มีหยดน้ำ<br>\
23:15 ขั้นตอนที่ 6 พับรีดชายเสื้อ<br>\
23:54 ขั้นตอนที่ 7 เย็บตะเข็บข้างตัวเสื้อ<br>\
24:50 ขั้นตอนที่ 8 พับเย็บชายเสื้อ<br>\
25:36 ตัวเสื้อสวมหัว<br>\
<br>" );
                        }
                        else if( _sku[1] == 'B'  ) {
                            sprintf( szVideo2, "<br><h3 style='text-align: center;'>ตัวเสื้อ</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/h3x1ReKjzfI"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
- คลิปวิดีโอสอนการตัดเย็บตัวเสื้อแบบผ่าหน้า สำหรับคอเสื้อทุกรูปแบบ ซึ่งเป็นการเย็บเก็บคอด้วยการกุ๊นหลบ เสื้อแบบมีปกหรือไม่มีปก ก็สามารถดูคลิปวิดีโอนี้ได้เลยค่ะ<br>\
- สำหรับเสื้อที่มีปก เมื่อเย็บต่อไหล่แล้ว(ขั้นตอนที่ 3) ให้เย็บปกติดกับตัวเสื้อตามขั้นตอนที่ 4 และเย็บผ้ากุ๊นตามขั้นตอนที่ 5 จากนั้นทำต่อไปจนขั้นตอนที่ 7 ได้เลยค่ะ<br>\
- สำหรับเสื้อที่ไม่มีปก เมื่อเย็บต่อไหล่แล้ว(ขั้นตอนที่ 3) ให้ข้ามไปที่การเย็บผ้ากุ๊นตามขั้นตอนที่ 5 จากนั้นทำต่อไปจนขั้นตอนที่ 7 ได้เลยค่ะ<br>\
<br>\
ขั้นตอนการตัดเย็บ<br>\
00:00 Intro<br>\
00:17 เตรียมแพทเทิร์นก่อนเย็บ,การวางผ้า,ตัดผ้า<br>\
01:50 ขั้นตอนที่ 1 พับเย็บสาบเสื้อชิ้นหน้า<br>\
03:58 ขั้นตอนที่ 2 เจาะรังดุม ติดกระดุม<br>\
05:31 ขั้นตอนที่ 3 เย็บต่อไหล่ตัวเสื้อ<br>\
06:55 ขั้นตอนที่ 4 เย็บติดปกเสื้อ (สำหรับคอที่มีปก)<br>\
09:29 ขั้นตอนที่ 5 เย็บเก็บคอเสื้อ ด้วยวิธีกุ๊นหลบ<br>\
15:25 เย็บเก็บคอเสื้อสี่เหลี่ยม<br>\
21:33 ขั้นตอนที่ 6 เย็บตะเข็บข้างตัวเสื้อ<br>\
22:49 ขั้นตอนที่ 7 พับเย็บชายเสื้อ<br>\
24:58 ตัวเสื้อผ่าหน้า<br>\
<br>" );
                        }                        

                        static char szVideo3[1024];
                        if( _sku[7] == 'U'  ) {
                            sprintf( szVideo3, "<br><h3 style='text-align: center;'>(ชุดนี้ไม่มีชายระบาย)</h3>\\n" );
                        }
                        else  {
                            sprintf( szVideo3, "<br><h3 style='text-align: center;'>ชายระบาย</h3>\\n<div class=""videoWrapper""><iframe title=""YouTube video player"" src=""https://www.youtube.com/embed/8F4x3qcNG_o"" width=""640"" height=""360"" frameborder=""0"" allowfullscreen=""allowfullscreen""></iframe></div><br>\
ขั้นตอนการตัดเย็บชายระบาย<br>\
00:00 Intro<br>\
00:26 เตรียมแพทเทิร์นก่อนเย็บ,วางผ้า,ตัดผ้า<br>\
01:21 ชายระบายสำหรับเสื้อผ่าหน้า<br>\
09:36 ชายระบายสำหรับเสื้อสวมหัว<br>\
<br>" );
                        }

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

                        static char szPNG[2560];
                        char szPNGFN[256];
                        szPNG[0] = 0;
                        strcat( szPNG, "<br><h2 style=\"\"text-align: center;\"\">ชิ้นส่วนแพทเทิร์น</h2><p style=\"\"text-align: center;\"\">\\n" );

                        for( int i=0; i<10; i++ ) {
                            sprintf( szPNGFN, "resources/_export_png2/%04i-%04i.png", _generateCount, i );
                            if( std::filesystem::exists( szPNGFN ) ) {
                                sprintf( szPNGFN, "<img class=\"\"alignnone size-full wp-image-6621\"\" src=\"\"https://magicpattern.co/pattern-img/%04i-%04i.png\"\" alt=\"\"\"\" />", _generateCount, i );
                                strcat( szPNG, szPNGFN );
                                png_count++;
                            }
                            else {
                                break;
                            }
                        }  
                        strcat( szPNG, "</p>" );                    

                        static char szLong[24096]; 
                        sprintf( szLong, "%s\\%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n%s\\n", 
                            szy, 
                            strYouGot.c_str(),
                            "<h3 style='text-align: center;'>การวัดตัว</h3>",
                            "<img class=""aligncenter wp-image-5016"" src=""https://magicpattern.co/wp-content/uploads/2022/07/bodysize01-1024x467.jpg"" alt=""การวัดตัว"" width=""640"" height=""292"" />\\n",
                            "<img class=""aligncenter wp-image-5031"" src=""https://magicpattern.co/wp-content/uploads/2022/07/bodysize.png"" alt=""ตารางวัดตัว"" width=""640"" height=""220"" />\\n",
                            szBodyLength, szSleeveLength, szMaterial,
                            "<h2 style='text-align: center;'>วิดีโอสอนการตัดเย็บ</h2>",
                            "<h3 style='text-align: center;'>ลำดับการตัดเย็บ</h3>",
                            "<p style='text-align: center;'>1.ปก → 2.ตัวเสื้อ → 3.ระบาย → 4.แขน</p>",
                            szVideo1, szVideo2, szVideo3, szVideo4, szPNG
                        );                                              

                        //sprintf( szx, ",\"simple, downloadable\",%s,Magic Pattern #%04i,1,0,visible,\"%s\",\"%s\",,,taxable,,1,,,0,0,,,,,0,,,400,Magic Pattern,MagicPattern,,https://magicpattern.co/_images2/%04i-%s.png,-1,-1,,,,,,,0,MagicPattern-%04i,MagicPattern-%04i.zip,https://magicpattern.co/F1n=411e/%04i.zip,,,\n",
                        //    _sku, _productCode, szShort, szLong, _productCode, _sku, _productCode, _productCode, _productCode
                        //); 
                        sprintf( szx, ",\"simple, downloadable\",%s,Magic Pattern #%04i,1,0,visible,\"%s\",\"%s\",,,taxable,,1,,,0,0,,,,,0,,,400,Magic Pattern,MagicPattern,,https://v2.magicpattern.co/_images2/%04i-%s.png,-1,-1,,,,,,,0,,,,,,\n",
                            _sku, _productCode, szShort, szLong, _productCode, _sku
                        ); 

                        if( f ) {
                            f.write( (const char *)szx, strlen(szx) );
                        }


                        //SkDebugf( "Generate Product List: %i: %i %i %i %i %i %i %s\n", 
                        //    _generateCount,
                        //    _generateType,
                        //    _generateLength,
                        //    _generateHem,
                        //    _generateSleeve,
                        //    _generateNeck,
                        //    _generateCollar,
                        //    _sku );  

                        //assert( _generateCount == _productCode ); 

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
                            if( _generateNeck >= 8 || (_generateNeck >= 7 && _generateType == 1)) {
                                _generateNeck = 0;
                                _generateSleeve++;
                                if( _generateSleeve >= 13 ) {
                                    _generateSleeve = 0;
                                    _generateHem++;
                                    if( _generateHem >= 3 || (_generateHem >= 1 && _generateLength > 2)) {
                                        _generateHem = 0;
                                        _generateLength++;
                                        if( _generateLength >= 5 ) {
                                            _generateLength = 0;
                                            _generateType++;
                                            if( _generateType >= 2 ) {
                                                _generateProduct = false;
                                                return;
                                            }
                                        }
                                    }                
                                }
                            }
                        }
                    } 
                    if( f ) {
                        f.close();
                    }
                    SkDebugf( "PNG count: %i\n", png_count );

                } ))
            )
            
    );
#endif
    
    this->_( *container );

    _pMPPanel->setMagicPatternWidget( this );

    generateProductCodeList();
    srand(time(NULL));
    if( _queryProductID == 0 ) {
        random();
    }
    else {
        gotoPattern( _queryProductID );
    }
    updateMPPanel();
}

bool DecMagicPattern::checkSKUMaterial( std::string sz1, std::string sz2, std::string sz5, std::string sz6 ) {
    if (sz1.find(_sku[1]) == std::string::npos) return false;
    if (sz2.find(_sku[2]) == std::string::npos) return false;
    if (sz5.find(_sku[5]) == std::string::npos) return false;
    if (sz6.find(_sku[6]) == std::string::npos) return false;
    return true;
}

void DecMagicPattern::random() {

    int sleeve = rand() % 13;
    setSleeve( sleeve );

    int hem = (rand() % 3);// + (3 * (_body / 10));    
    int body = rand() % (5 * 15);
    if( body / 15 > 2 ) {
        hem = 0;
    }
    setBody( body );
    setHem( hem );    

    int hair = rand() % 5;
    setHair( hair );    

    int background = rand() % 4;
    setBackground( background );      
    
    int collar = 0;
    if( ((body % 15) % 8) == 0 ) {
        collar = rand() % 6;
        if( collar == 4 && _type == 1 ) {
            collar = 5;
        }
    }
    else if( ((body % 15) % 8) == 1 ) {
        collar = (rand() % 2) * 6;
    }
    setCollar( collar );

    uint32_t bodyColor = _randomColors[rand() % _numRandomColors];
    uint32_t collarColor = _randomColors[rand() % _numRandomColors];
    uint32_t sleeveColor = _randomColors[rand() % _numRandomColors];
    uint32_t hemColor = _randomColors[rand() % _numRandomColors];
    setColor( bodyColor, bodyColor, bodyColor, bodyColor );

    updateSKU();    
}

DecMagicPattern::~DecMagicPattern() {

};

DecMagicPattern &MagicPattern( std::string option ) {
    DecMagicPattern *w = new DecMagicPattern( option );
    return *w;    
}

void DecMagicPattern::gotoPattern( int pid ) {
    if( pid > 0 && pid <= 3718 ) {
        pKeyPad->visible( false );
        std::string sku = _sku_list[pid-1].c_str();
        SkDebugf( "Goto: %i %s", pid, sku.c_str() );

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

        int body = _generateLength * 15 + _generateType * 8 + _generateNeck;
        setHem( _generateHem );
        setSleeve( _generateSleeve );
        setCollar( _generateCollar );        
        setBody( body );  

        uint32_t bodyColor = _randomColors[rand() % _numRandomColors];
        setColor( bodyColor, bodyColor, bodyColor, bodyColor );

        int background = rand() % 4;
        setBackground( background );              

        updateSKU();              
    }
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

void DecMagicPattern::generateProductCodeList() {
    int _generateType = 0;
    int _generateLength = 0;
    int _generateNeck = 0;
    int _generateHem = 0;
    int _generateSleeve = 0;
    int _generateCollar = 0;
    int _generateCount = 0;

    while( true ) {
        _generateCount++;      

        _body = _generateLength * 15 + _generateType * 8 + _generateNeck;
        _length = (_body / 15);
        _type = (_body % 15) / 8;
        _neck = (_body % 15) % 8;        

        _hem = _generateHem;
        _sleeve = _generateSleeve;
        _collar = _generateCollar;

        std::string _sku = getSKU( _length, _type, _neck, _hem, _sleeve, _collar ); 
        updateProductCode();
        _sku_list[_productCode-1] = _sku;

        //if( _generateCount > 10 )
        //    break;

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
            if( _generateNeck >= 8 || (_generateNeck >= 7 && _generateType == 1)) {
                _generateNeck = 0;
                _generateSleeve++;
                if( _generateSleeve >= 13 ) {
                    _generateSleeve = 0;
                    _generateHem++;
                    if( _generateHem >= 3 || (_generateHem >= 1 && _generateLength > 2)) {
                        _generateHem = 0;
                        _generateLength++;
                        if( _generateLength >= 5 ) {
                            _generateLength = 0;
                            _generateType++;
                            if( _generateType >= 2 ) {
                                _generateProduct = false;
                                assert( _generateCount == 3718 );
                                SkDebugf( "Generated SKU table OK\n" );
                                return;
                            }
                        }
                    }                
                }
            }
        }           

    }
}

std::string DecMagicPattern::getSKU( int _length, int _type, int _neck, int _hem, int _sleeve, int _collar) {
    char _sku[64];

    _sku[0] = 0;

    const char *_str_length[] = { "W", "H", "L", "K", "C" };
    strcat( _sku, _str_length[_length] );

    const char *_str_type[] = { "P", "B" };
    strcat( _sku, _str_type[_type] );

    const char *_str_neck[] = { "A", "B", "C", "D", "E", "F", "G", "H" };
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

std::string DecMagicPattern::getDescription() {
    static const char *desc_nect[] = { "คอกลม", "คอวี", "คอปาด", "คอสี่เหลี่ยม", "คอคว้าน" };
    static const char *desc_collar[] = { "ไม่มีปก", "ปกบัว", "ปกบัวแหลม", "ปกคอจีน", "ปกระบาย", "ปกผูกโบว์", "ปกปลายแหลม" };
    static const char *desc_type[] = { "สวมศีรษะ", "ผ่าหน้า" };
    static const char *desc_length[] = { "เอว", "สะโพกสูง", "สะโพกล่าง" };
    static const char *desc_sleeve[] = { "แขนกุด", "แขนสั้น", "แขนสามส่วน", "แขนยาว", "แขนทิวลิป", "แขน cap", "แขนย้วย", "แขน lantern", "แขน Flounce", "แขนมารี", "แขนผ่า", "แขนตุ๊กตา", "แขนระบาย" };
    static const char *desc_hem[] = { "", "แต่งระบายชายเสื้อแบบสั้น ", "แต่งระบายชายเสื้อแบบยาว " };

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

void DecMagicPattern::getPattern() {
    SkDebugf( "Product ID: %04i\n", _productCode );
    SkDebugf( "SKU: %s\n", _sku );
    SkDebugf( "Description: %s\n", getDescription().c_str() );

    char sz[1024];
    sprintf( sz, "getPattern( '%04i', '%s', '%s' )", _productCode, _sku, getDescription().c_str() );
    
#ifdef __EMSCRIPTEN__
    emscripten_run_script(sz);
#endif
}

void DecMagicPattern::update() {
    DecWidget::update();

    if( _generateProduct ) {
        if( _generateFrame % 10 == 0 ) {
            _generateCount++;
            //SkDebugf( "Generate Product: %i: %i %i %i %i %i %i %s\n", 
            //    _generateCount,
            //    _generateType,
            //    _generateLength,
            //    _generateHem,
            //    _generateSleeve,
            //    _generateNeck,
            //    _generateCollar,
            //    _sku );

            char sz[128];
            sprintf( sz, "_images2/%04i-%s.png", _productCode, _sku );
            //SkDebugf( "%i - %i %i %i %i %i %i - %s\n", _generateCount, _generateType, _generateLength, _generateHem, 
            //    _generateSleeve, _generateNeck, _generateCollar, sz );
            _pMainImage->capture( sz, false, true );

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
            else if( _generateNeck == 7 ) {
                if( _generateType == 1 ) {
                    _generateNeck++;
                }
            }
            if( !bSkip ) {
                _generateNeck++;
                if( _generateNeck >= 8 || (_generateNeck >= 7 && _generateType == 1)) {
                    _generateNeck = 0;
                    _generateSleeve++;
                    if( _generateSleeve >= 13 ) {
                        _generateSleeve = 0;
                        _generateHem++;
                        if( _generateHem >= 3 || (_generateHem >= 1 && _generateLength > 2)) {
                            _generateHem = 0;
                            _generateLength++;
                            if( _generateLength >= 5 ) {
                                _generateLength = 0;
                                _generateType++;
                                if( _generateType >= 2 ) {
                                    _generateProduct = false;
                                    return;
                                }
                            }
                        }                
                    }
                }
            }
        }
        if( _generateFrame % 10 == 5 ) {
            int body = _generateLength * 15 + _generateType * 8 + _generateNeck;
            setCollar( _generateCollar );
            setSleeve( _generateSleeve );
            setHem( _generateHem );
            
            int hair = rand() % 5;
            setHair( hair );  
            //SkDebugf( ">> %i %i, %i %i, %i %i\n", _generateLength, _length,
            //    _generateType, _type, _generateNeck, _neck );
            setBody( body );
            //SkDebugf( "%i %i, %i %i, %i %i\n", _generateLength, _length,
            //    _generateType, _type, _generateNeck, _neck );
            assert( _generateLength == _length );
            assert( _generateType == _type );
            assert( _generateNeck == _neck );

            uint32_t bodyColor = _randomColors[rand() % _numRandomColors];
            setColor( bodyColor, bodyColor, bodyColor, bodyColor );

            int background = rand() % 4;
            setBackground( background );              

            updateSKU(); 
        }
        _generateFrame++;
    }
}

void DecMagicPattern::updateSKU() {
    _sku[0] = 0;

    const char *_str_length[] = { "W", "H", "L", "K", "C" };
    strcat( _sku, _str_length[_length] );

    const char *_str_type[] = { "P", "B" };
    strcat( _sku, _str_type[_type] );

    const char *_str_neck[] = { "A", "B", "C", "D", "E", "F", "G", "H" };
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

    char szk[48];
    sprintf( szk, "(SKU: %s)", _sku );
    _pSKU->text( szk );
    _pSKU->dirty();
    _pProductCode->dirty();

    updateProductCode();
}

void DecMagicPattern::updateProductCode() {
    //SkDebugf( "%i %i %i %i %i %i\n", _type, _length, _hem, _sleeve, _neck, _collar );
    
    int _pcode = 0;
    
    if( _length > 2 ) {
        if( _type == 0 ) {
            _pcode += (_type * 2 * 1 * 13 * 14) + ((_length-3) * 1 * 13 * 14) + (_hem * 13 * 14) + 
                (_sleeve * 14) + 2457 + 351 + 234;
            if( _neck == 0 ) {
                _pcode += _collar;
            }
            else if( _neck == 1 ) {
                _pcode += 6 + (_collar / 6);
            }
            else if( _neck >= 2 ) {
                _pcode += 6 + 2 + (_neck - 2);
            }
            assert( _pcode >= 3042 && _pcode < 3042+364 );
        }
        else {
            _pcode += (_type * 2 * 1 * 13 * 14) + ((_length-3) * 1 * 13 * 12) + (_hem * 13 * 12) + 
                (_sleeve * 12) + 2457 + 351 + 234;
            if( _neck == 0 ) {
                _pcode += _collar;
                if( _collar == 5 ) {
                    _pcode--;
                }
            }
            else if( _neck == 1 ) {
                _pcode += 5 + (_collar / 6);
            }
            else if( _neck >= 2 ) {
                _pcode += 5 + 2 + (_neck - 2); 
            }
            assert( _pcode >= (3042+364) && _pcode < 3042+364+312 );
        }
        //SkDebugf( "_pcode3: %i\n", _pcode );

    }
    else {
        if( _neck > 4 ) {
            if( _type == 0 ) {
                _pcode += (_type * 3 * 3 * 13 * 3) + (_length * 3 * 13 * 3) + (_hem * 13 * 3) + 
                    (_sleeve * 3) + (_neck - 5) + 2457;
                assert( _pcode >= 2457 && _pcode < (2457+351) );
            }
            else {
                _pcode += (_type * 3 * 3 * 13 * 3) + (_length * 3 * 13 * 2) + (_hem * 13 * 2) + 
                    (_sleeve * 2) + (_neck - 5) + 2457;
                assert( _pcode >= (2457+351) && _pcode < (2457+351+234) );
            }
            //SkDebugf( "_pcode4: %i\n", _pcode );

        }
        else {
            // Magic Pattern 1
            if( _type == 0 ) {
                _pcode += (_type * 3 * 3 * 13 * 11) + (_length * 3 * 13 * 11) + (_hem * 13 * 11) + (_sleeve * 11);
                //SkDebugf( "_pcode0: %i\n", _pcode );
                if( _neck == 0 ) {
                    _pcode += _collar;
                }
                else if( _neck == 1 ) {
                    _pcode += 6 + (_collar / 6);
                }
                else if( _neck >= 2 ) {
                    _pcode += 6 + 2 + (_neck - 2);
                }
                assert( _pcode < 1287 );
            }
            else {
                _pcode += (_type * 3 * 3 * 13 * 11) + (_length * 3 * 13 * 10) + (_hem * 13 * 10) + (_sleeve * 10);
                //SkDebugf( "_pcode1: %i\n", _pcode );
                if( _neck == 0 ) {
                    _pcode += _collar;
                    if( _collar == 5 ) {
                        _pcode--;
                    }
                }
                else if( _neck == 1 ) {
                    _pcode += 5 + (_collar / 6);
                }
                else if( _neck >= 2 ) {
                    _pcode += 5 + 2 + (_neck - 2); 
                }
                assert( _pcode >= 1287 && _pcode < 2457 );
            }
        }
    }
    assert( _pcode < 3718 );

    _productCode = _pcode + 1;

    //SkDebugf( "PCODE: %i\n", _pcode );
    char szp[32];
    sprintf( szp, "รหัส: %04i", _productCode );
    //SkDebugf( "%s\n", szp );

    _pProductCode->text( szp );
    _pProductCode->dirty();

    //sprintf( szp, "#%04i", _productCode );
    _pTextProductID->text( szp );
    _pTextProductID->dirty();

    _pSKU->dirty();

}

void DecMagicPattern::setBody( int i ) {
    _body = i;
    _length = (_body / 15);
    _type = (_body % 15) / 8;
    _neck = (_body % 15) % 8;
    SkDebugf( "Body: %i - %i %i %i\n", i, _length, _type, _neck );
    if( _pBody ) {
        _pBody->activate( i );
    }
    if( _pHemType ) {
        _pHemType->activate( _type );
    }
    if( _pCollarType ) {
        _pCollarType->activate( _type );
    }
    //updateSKU();
}

void DecMagicPattern::setSleeve( int i ) {
    _sleeve = i;
    if( _pSleeve ) {
        _pSleeve->activate( i );
    }
    //updateSKU();
}

void DecMagicPattern::setBackground( int i ) {
    _background = i;
    if( _pBackground ) {
        _pBackground->activate( i );
    }
}

void DecMagicPattern::setHair( int i ) {
    _hair = i;
    if( _pHair ) {
        _pHair->activate( i );
    }
}

void DecMagicPattern::setHem( int i ) {
    _hem = i;
    if( _pHem0 ) {
        _pHem0->activate( (_length * 3) + _hem );
    }
    if( _pHem1 ) {
        _pHem1->activate( (_length * 3) + _hem );
    }
    //updateSKU();
}

void DecMagicPattern::setCollar( int i ) {
    _collar = i;
    if( _pCollar0 ) {
        _pCollar0->activate( i );
    }
    if( _pCollar1 ) {
        _pCollar1->activate( i );
    }
    //updateSKU();
}

void DecMagicPattern::setColor( int bodyColor, int collarColor, int sleevColor, int hemColor ) {
    // FBDDCF
    if( _pHuman ) {
        _pHuman->enableFilter( true );
        _pHuman->setFilterColor( 0xFFFBDDCF );
    }
    if( _pHand ) {
        _pHand->enableFilter( true );
        _pHand->setFilterColor( 0xFFFBDDCF );
    }  
    if( _pHair ) {
        for( int i=0; i<_pHair->_children.size(); i++ ) {
            DecImage *img = (DecImage *)_pHair->_children[i];
            img->enableFilter( true );
            img->setFilterColor( 0xFFFBDDCF );
        }
    }       
    if( _pBody ) {
        for( int i=0; i<_pBody->_children.size(); i++ ) {
            DecImage *img = (DecImage *)_pBody->_children[i];
            img->enableFilter( true );
            img->setFilterColor( bodyColor );
        }
    }
    if( _pHem0 ) {
        for( int i=0; i<_pHem0->_children.size(); i++ ) {
            DecImage *img = (DecImage *)_pHem0->_children[i];
            img->enableFilter( true );
            img->setFilterColor( hemColor );
        }
    }    
    if( _pHem1 ) {
        for( int i=0; i<_pHem1->_children.size(); i++ ) {
            DecImage *img = (DecImage *)_pHem1->_children[i];
            img->enableFilter( true );
            img->setFilterColor( hemColor );
        }
    }    
    if( _pSleeve ) {
        for( int i=0; i<_pSleeve->_children.size(); i++ ) {
            DecImage *img = (DecImage *)_pSleeve->_children[i];
            img->enableFilter( true );
            img->setFilterColor( sleevColor );
        }
    }   
    if( _pCollar0 ) {
        for( int i=0; i<_pCollar0->_children.size(); i++ ) {
            DecImage *img = (DecImage *)_pCollar0->_children[i];
            img->enableFilter( true );
            img->setFilterColor( collarColor );
        }
    }       
    if( _pCollar1 ) {
        for( int i=0; i<_pCollar1->_children.size(); i++ ) {
            DecImage *img = (DecImage *)_pCollar1->_children[i];
            img->enableFilter( true );
            img->setFilterColor( collarColor );
        }
    }       
}


void DecMagicPattern::updateMPPanel() {
    if( _pMPPanel ) {
        //SkDebugf( "UpdateMPPanel\n" );
        //SkDebugf( "Body: %i\n", _body );
        //SkDebugf( "Collar: %i\n", _collar );
        //SkDebugf( "Sleeve: %i\n", _sleeve );
        //SkDebugf( "Hem: %i\n", _hem );
        //SkDebugf( "Length: %i\n", _length );
        //SkDebugf( "Type: %i\n", _type );
        //SkDebugf( "Neck: %i\n", _neck );

        _pMPPanel->setShirtParams( _type, _length, _neck, _collar, _sleeve, _hem );

        //_pMPPanel->_shirt_type = _type;
        //_pMPPanel->_shirt_length = _length;
        //_pMPPanel->_shirt_neck = _neck;

        //_pMPPanel->updateShirtBody();

        //_pMPPanel->_shirt_type = type;
        //_pMPPanel->_shirt_length = length;
        //_pMPPanel->_shirt_neck = neck;
        //_pMPPanel->_shirt_hem = hem;
        //_pMPPanel->_shirt_sleeve = sleeve;

        //_pMPPanel->_shirt_collar = collar;
        // = hem - (3 * length);
        // = sleeve;
        //
        //
    }    
}

