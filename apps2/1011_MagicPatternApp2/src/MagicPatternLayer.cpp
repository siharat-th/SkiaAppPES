#include "DeclarativeUI.h"
#include "MagicPatternLayer.h"
#include "TestMagicPattern.h"
#include "DecMagicPattern.h"

namespace sk_ui {

MagicPatternLayer::MagicPatternLayer( std::string option )
{
    SkDebugf( "MagicPatternLayer **** %s\n", option.c_str() );
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
                ._( createMagicPattern( option )
                    .ref( (DecWidget **)&pMagicPattern)
                )
        );
}

}
