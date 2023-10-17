#include "AppLib.h"
#include "../src/AppCreator.h"

sk_ui::SkiaAppLib *DecTemplateLib::createApp() {
    SkDebugf( "DecTemplateLib::DecTemplateLib **\n");
    return CreateDecTemplateSkiaAppLib();
}
