#include "AppLib.h"
#include "../src/AppCreator.h"

sk_ui::SkiaAppLib *BWSLib::createApp() {
    SkDebugf( "BWSLib::DecTemplateLib **\n");
    return CreateBWSSkiaAppLib();
}
