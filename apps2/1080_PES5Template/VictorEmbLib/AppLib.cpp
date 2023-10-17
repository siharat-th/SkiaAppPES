#include "AppLib.h"
#include "../src/AppCreatorVictorEmb.h"

sk_ui::SkiaAppLib *VictorEmbLib::createApp() {
    SkDebugf( "VictorEmbLib::VictorEmbLib ****\n");
    return CreateVictorEmbAppSkiaAppLib();
}
