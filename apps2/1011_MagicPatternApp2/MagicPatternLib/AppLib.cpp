#include "AppLib.h"
#include "../src/AppCreator.h"

sk_ui::SkiaAppLib *MagicPatternLib::createApp( std::string option ) {
    SkDebugf( "MagicPatternLib::MagicPatternLib ****\n");
    return CreateMagicPatternAppSkiaAppLib( option );
}
