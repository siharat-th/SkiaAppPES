#include "DeclarativeUI.h"
#include "SVGEditorLayer.h"
#include "SVGEditor.h"

namespace sk_ui {

SVGEditorLayer::SVGEditorLayer()
{
    UI().font(Font().Regular("fonts/Kanit-Regular.ttf")
                      .Bold("fonts/Kanit-Bold.ttf")
                      .Italic("fonts/Kanit-Italic.ttf")
                      .BoldItalic("fonts/Kanit-BoldItalic.ttf")
                      .Icon("fonts/Font Awesome 6 Pro-Regular-400.otf")
                      .Emoji("fonts/NotoColorEmoji.ttf"))
        .root(
            Column()
                ._( Row()
                    .size( -1,40 )
                    .bg_color( 0xFFF0F0F0 )
                    .border_color( 0xFF808080 )
                    ._( Spacer().size( -1, -1 ) )
                    ._( Text( "SVG Editor Test" ).fontSize(18).align( ALIGN_CENTER ) )
                    ._( Spacer().size( -1, -1 ) )
                )
                ._( SVGEditor().ref( (DecWidget **)&pSVGEditor ) )
        );      
}

//bool SVGEditorLayer::onChar(SkUnichar c, skui::ModifierKey mod) {
//    //SkDebugf("SVGEditorLayer::onChar: 0x%04x\n", c);
//    if (_ui) {
//        if (_ui->onChar(c, mod)) {
//            return true;
//        }
//    }
//    return false;
//}

}
