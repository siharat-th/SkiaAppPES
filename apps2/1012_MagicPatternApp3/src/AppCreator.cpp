#include "AppCreator.h"
#include "tools/Resources.h"
#include "DecTemplateLayer.h"

using namespace sk_ui;


sk_ui::SkiaAppLib* CreateSkiaAppLib(std::string option) {
    // force to use local source Resources.cpp
    SkString str = GetResourcePath();

    SkDebugf( "MagicPatternApp - DecTemplateLayer\n" );

    sk_ui::SkiaAppLib * appLib = new SkiaAppLib();
    appLib->setAppTitle( "MagicPatternApp" );
    //appLib->pushLayer( new LayersTestAppLayer("json/layers_test.json") );
    appLib->pushLayer(new DecMagicPattern3Layer(option));
    appLib->pushLayer( new FPSLayer(SK_ColorBLACK, false) );

    return appLib;
}

#if !defined(__EMSCRIPTEN__) || defined(__NO_CANVASKIT__)
sk_ui::SkiaAppLib * CreateSkiaAppLib() {
    return CreateSkiaAppLib("");
}
#endif
