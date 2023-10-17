#include "DeclarativeUI.h"
#include "UComposerLayer.h"
#include "DecUComposer.h"
#include "DecUTestPanel.h"

namespace sk_ui {

UComposerLayer::UComposerLayer()
{
    _workspace = NULL;
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
            Row()
                .ref( (DecWidget **)&_workspace )
                .background( false )
                .border( false )
                ._( UComposer() )
        );
}

void UComposerLayer::onInit() {
    
    // native only
    //SkDebugf( "UComposerLayer::Init %li \n", SKIAAPP()->AppFlag );
    if( (SKIAAPP()->AppFlag & 1) == 0 ) {
        _workspace->_( UTestPanel() );
    }

}

}
