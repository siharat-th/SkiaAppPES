#include "AppLib.h"
#include "../src/AppCreator.h"

sk_ui::SkiaAppLib *LayersTestLib::createApp() {
    SkDebugf( "TemplateAppLib::TemplateAppLib\n");
    return CreateLayersTestAppSkiaAppLib();
}
