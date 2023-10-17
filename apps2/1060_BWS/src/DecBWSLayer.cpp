#include "DeclarativeUI.h"
#include "DecBWSLayer.h"
#include "DecBWS.h"

namespace sk_ui {

DecBWSLayer::DecBWSLayer()
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
            Container()
                .padding( 0, 0, 0, 0 )
                ._( BWS() )
        );
}

}
