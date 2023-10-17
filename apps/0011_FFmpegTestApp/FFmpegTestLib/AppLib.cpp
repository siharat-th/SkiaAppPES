#include "AppLib.h"
#include "../src/FFmpegTestLayer.h"

sk_ui::SkiaAppLib *HelloAppLib::createApp() {
    return CreateSkiaAppLib();
}

