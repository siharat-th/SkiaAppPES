#include "AppLib.h"
#include "../src/AppCreatorDrawEmb.h"

sk_ui::SkiaAppLib *DrawEmbLib::createApp() {
    SkDebugf( "DrawEmbLib::DrawEmbLib ****\n");
    return CreateDrawEmbAppSkiaAppLib();
}
