#include "DeclarativeUI.h"
#include "DecTemplateLayer.h"
#include "MagicPattern3.h"

namespace sk_ui {

DecMagicPattern3Layer::DecMagicPattern3Layer(std::string option) {
    UI()
        .font(Font()
            .Regular("fonts/Kanit-Light.ttf")
            //.Bold("fonts/Kanit-Bold.ttf")
            //.Italic("fonts/Kanit-Italic.ttf")
            //.BoldItalic("fonts/Kanit-BoldItalic.ttf")
            .Icon("fonts/Font Awesome 6 Pro-Light-300.otf")
            //.Emoji("fonts/NotoColorEmoji.ttf")
        )    
        .root(
            Container()
                .ID( "MP3 Layer Container" )
                ._( MagicPattern() )
        );
}

}
