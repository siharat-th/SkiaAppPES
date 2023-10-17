#include "TestMagicPattern.h"
#include "DecMagicPattern.h"

DecWidget &createMagicPattern( std::string option ) {
    return (Column()
        ._( Row()
            .size( -1,32 )
            .bg_color( 0xFFF0F0F0 )
            .border_color( 0xFF808080 )
            ._( Spacer().size( -1, -1 ) )
            ._( Text( "Pinn MagicPattern2 v1.80" ).fontSize(18).align( ALIGN_CENTER ) )
            ._( Spacer().size( -1, -1 ) )
        )
        ._( Container()
            .size( -1, -1 )
            .border( false )
            .background( false )
            .max_width( 360 )
            .align( ALIGN_TOPCENTER )
            ._(
                MagicPattern( option )
            )
        )
    );
}
