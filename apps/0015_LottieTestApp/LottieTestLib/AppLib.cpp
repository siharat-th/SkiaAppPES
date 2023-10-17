#include "AppLib.h"
#include "../src/LottieTestLayer.h"

sk_ui::SkiaAppLib *LottieTestLib::createApp() {
    printf( "LottieTestLib::createApp\n" );
    return CreateLottieTestSkiaAppLib();
}

