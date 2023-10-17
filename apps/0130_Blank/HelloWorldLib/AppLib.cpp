#include "AppLib.h"
#include "../src/HelloWorldLayer.h"

sk_ui::SkiaAppLib *HelloAppLib::createApp() {
    return CreateSkiaAppLib();
}

