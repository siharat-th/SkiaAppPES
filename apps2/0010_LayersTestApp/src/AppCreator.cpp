#include "AppCreator.h"
#include "tools/Resources.h"
//#include "LayersTestAppLayer.h"
#include "UITestLayer.h"

using namespace sk_ui;


sk_ui::SkiaAppLib * CreateLayersTestAppSkiaAppLib() {
    // force to use local source Resources.cpp
    SkString str = GetResourcePath();

    sk_ui::SkiaAppLib * appLib = new SkiaAppLib();
    appLib->setAppTitle( "LayersTestApp" );
    //appLib->pushLayer( new LayersTestAppLayer("json/layers_test.json") );
    appLib->pushLayer( new UITestLayer() );
    appLib->pushLayer( new FPSLayer(SK_ColorBLACK, false) );

    return appLib;
}

#if !defined(__EMSCRIPTEN__) || defined(__NO_CANVASKIT__)
sk_ui::SkiaAppLib * CreateSkiaAppLib() {
    return CreateLayersTestAppSkiaAppLib();
}
#endif
