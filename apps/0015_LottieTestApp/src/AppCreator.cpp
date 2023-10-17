#include "AppCreator.h"
#include "LottieTestLayer.h"
#include "tools/Resources.h"
#include "include/core/SkTime.h"
#include "include/core/SkSwizzle.h"

using namespace sk_ui;

sk_ui::SkiaAppLib * CreateLottieTestSkiaAppLib() {    
    // force to use local source Resources.cpp
    SkString str = GetResourcePath();

    printf( "CreateSkiaAppLib - Lottie\n" );
    sk_ui::SkiaAppLib * appLib = new SkiaAppLib();
    appLib->pushLayer( new LottieTestLayer() );
    //appLib->pushLayer( new WidgetLayer("json/test_widget2.json") );
    //appLib->pushLayer( new ImGuiLayer() );
    //appLib->pushLayer( new StatsLayer() );
    appLib->pushLayer( new FPSLayer(SK_ColorBLACK, true) );

    return appLib; 
}

#if !defined(__EMSCRIPTEN__) || defined(__NO_CANVASKIT__)
sk_ui::SkiaAppLib * CreateSkiaAppLib() {
    return CreateLottieTestSkiaAppLib();
}
#endif

