#include "DeclarativeUI.h"
#include "SVGEditorLayer.h"
#include "SVGEditor.h"

namespace sk_ui {

SVGEditorLayer::SVGEditorLayer()
{
    UI()
        .font(Font()
            .Regular("fonts/Kanit-Regular.ttf")
            .Bold("fonts/Kanit-Bold.ttf")
            .Italic("fonts/Kanit-Italic.ttf")
            .BoldItalic("fonts/Kanit-BoldItalic.ttf")
            .Icon("fonts/Font Awesome 6 Pro-Regular-400.otf")
        )
        .root(
            Column()
                ._( Row()
                    .size( -1,40 )
                    .bg_color( 0xFFF0F0F0 )
                    .border_color( 0xFF808080 )
                    ._( Spacer().size( -1, -1 ) )
                    ._( Text( "MagicPattern Pattern Editor 2" ).fontSize(18).align( ALIGN_CENTER ) )
                    ._( Spacer().size( -1, -1 ) )
                )
                ._( SVGEditor().ref( (DecWidget **)&pSVGEditor ) )
        );      
}

}
