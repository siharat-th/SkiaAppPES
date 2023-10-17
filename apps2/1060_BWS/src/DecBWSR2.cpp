#include "DeclarativeUI.h"
#include "DecBWSR2.h"

DecBWSR2::DecBWSR2() {
    size( 80, 20 );
    border( true );
    round( 40 );
    bg_color( 0xFFE0E0E0 );
    background( false );
    _( Stack() 
        .background( false )
        ._( Text( "Shift" ).align( ALIGN_CENTER ).text_color(0xFFB0B0B0) )
    );

    bR2 = false;    
}

DecBWSR2::~DecBWSR2(){

} 

DecBWSR2 &BWSR2() {
    DecBWSR2 *w = new DecBWSR2();
    return *w;
}

void DecBWSR2::update() {
    if( bR2 ) {
        background( true );
    }
    else {
        background( false );
    }
}