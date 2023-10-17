#include "DeclarativeUI.h"
#include "PESTestLayer.h"
#include "TestMagicPattern.h"
#include "DecMagicPattern.h"

namespace sk_ui {

PESTestLayer::PESTestLayer()
{
    SkDebugf( "MagicPatternLayer ****\n");
    UI()
        .font(Font()
            .Regular("fonts/Kanit-Regular.ttf")
            .Bold("fonts/Kanit-Bold.ttf")
            .Italic("fonts/Kanit-Italic.ttf")
            .BoldItalic("fonts/Kanit-BoldItalic.ttf")
            .Icon("fonts/Font Awesome 6 Pro-Regular-400.otf")
        )    
        .root(
            Container()
                ._( createMagicPattern()
                    .ref( (DecWidget **)&pMagicPattern)
                )
        );
}

}
