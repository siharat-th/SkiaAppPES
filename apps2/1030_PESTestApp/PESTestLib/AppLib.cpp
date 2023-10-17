#include "AppLib.h"
#include "../src/AppCreator.h"

sk_ui::SkiaAppLib *PESTestLib::createApp() {
    SkDebugf( "MagicPatternLib::MagicPatternLib ****\n");
    return CreatePESTestAppSkiaAppLib();
}
