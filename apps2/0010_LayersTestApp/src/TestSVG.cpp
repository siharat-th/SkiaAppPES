#include "TestSVG.h"
#include "DecWidget.h"
#include "DecFlowContainer.h"
#include "DecSVGDom.h"

// msvc compile error
// TestSVG.cpp(7): fatal error C1060: compiler is out of heap space

/*
DecWidget &createSVGS() {
    return (Flow()
        .padding( 10, 10, 10, 10 )
        .gap( 10 )
        ._( SVGDom( "svg/MagicPattern00.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern01.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern02.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern03.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern04.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern05.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern06.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern07.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern08.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern09.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern10.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern11.svg").size( 200, 150 ) ) 
        ._( SVGDom( "svg/MagicPattern12.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern13.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern14.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern15.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern16.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern17.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern18.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern19.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern20.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern21.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern22.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern23.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern24.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern25.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern26.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern27.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern28.svg").size( 200, 150 ) )
        ._( SVGDom( "svg/MagicPattern29.svg").size( 200, 150 ) )
    );
}
*/

DecWidget &createSVGS() {
    DecFlowContainer *w = &Flow();
      w->padding( 10, 10, 10, 10 )
        .gap( 10 )
        ._( SVGDom( "resources/svg/MagicPattern00.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern01.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern02.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern03.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern04.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern05.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern06.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern07.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern08.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern09.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern10.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern11.svg").size( 200, 150 ).cache(true) ) 
        ._( SVGDom( "resources/svg/MagicPattern12.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern13.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern14.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern15.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern16.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern17.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern18.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern19.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern20.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern21.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern22.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern23.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern24.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern25.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern26.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern27.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern28.svg").size( 200, 150 ).cache(true) )
        ._( SVGDom( "resources/svg/MagicPattern29.svg").size( 200, 150 ).cache(true) )		
		;
		return (*w);
}



