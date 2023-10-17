#include "DeclarativeUI.h"
#include "UITestLayer.h"
#include "DemoWidgets.h"
#include "TestSVG.h"
#include "ParagraphTest.h"
//#include "TestMagicPattern.h"
//#include "DecMagicPattern.h"

namespace sk_ui {

UITestLayer::UITestLayer()
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
            Pages()
                .focusable( true )
                ._( createParagraphTest() )
                ._( createDemo() )
                ._( createSVGS() )
        );
}

}
