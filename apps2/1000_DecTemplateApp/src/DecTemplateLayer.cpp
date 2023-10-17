#include "DeclarativeUI.h"
#include "DecTemplateLayer.h"
#include "DecTemplate.h"

namespace sk_ui {

DecTemplateLayer::DecTemplateLayer()
{
    UI()
        .font(Font()
            .Regular("fonts/Kanit-Regular.ttf")
            .Bold("fonts/Kanit-Bold.ttf")
            .Italic("fonts/Kanit-Italic.ttf")
            .BoldItalic("fonts/Kanit-BoldItalic.ttf")
            .Icon("fonts/Font Awesome 6 Pro-Regular-400.otf")
            .Emoji("fonts/NotoColorEmoji.ttf")
        )    
        .root(
            Container()
                ._( Template() )
        );
}

}
