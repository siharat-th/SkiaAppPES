#include "AppLib.h"
#include "../src/AppCreatorUComposer.h"

sk_ui::SkiaAppLib *UComposerLib::createApp() {
    //SkDebugf( "UComposerLib::UComposerLib **\n");
    return CreateUComposerSkiaAppLib();
}
