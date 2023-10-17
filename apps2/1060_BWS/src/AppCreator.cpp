#include "AppCreator.h"
#include "tools/Resources.h"
#include "DecBWSLayer.h"

using namespace sk_ui;


sk_ui::SkiaAppLib * CreateBWSSkiaAppLib() {
    // force to use local source Resources.cpp
    SkString str = GetResourcePath();

    printf( "CreateSkiaAppLib - BWSLayer\n" );

    sk_ui::SkiaAppLib * appLib = new SkiaAppLib();
    appLib->setAppTitle( "BWSApp" );
    //appLib->pushLayer( new LayersTestAppLayer("json/layers_test.json") );
    appLib->pushLayer( new DecBWSLayer() );
    appLib->pushLayer( new FPSLayer(SK_ColorBLACK, false) );

    return appLib;
}

#if !defined(__EMSCRIPTEN__) || defined(__NO_CANVASKIT__)
sk_ui::SkiaAppLib * CreateSkiaAppLib() {
    return CreateBWSSkiaAppLib();
}
#endif
