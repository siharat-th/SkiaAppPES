#include "AppLib.h"
#include "../src/AppCreator.h"

sk_ui::SkiaAppLib* MagicPatternLib::createApp(std::string option) {
    SkDebugf( "MagicPatternLib::createApp **\n");
    return CreateSkiaAppLib(option);
}
