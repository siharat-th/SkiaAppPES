#include "AppLib.h"
#include "../src/AppCreatorSVG2EMB.h"

sk_ui::SkiaAppLib *SVG2EMBLib::createApp() {
    SkDebugf( "SVG2EMBLib::SVG2EMBLib ****\n");
    return CreateSVG2EMBAppSkiaAppLib();
}
