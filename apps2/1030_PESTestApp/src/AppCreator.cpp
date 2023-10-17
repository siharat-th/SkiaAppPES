#include "AppCreator.h"
#include "tools/Resources.h"
#include "PESTestLayer.h"

using namespace sk_ui;


sk_ui::SkiaAppLib * CreatePESTestAppSkiaAppLib() {
    // force to use local source Resources.cpp
    SkString str = GetResourcePath();

    SkDebugf("CreatePESTestAppLib - PESTestLib\n");

    sk_ui::SkiaAppLib * appLib = new SkiaAppLib();
    appLib->setAppTitle( "PESTestApp" );
    appLib->pushLayer( new DecPesLayer() );
    appLib->pushLayer( new FPSLayer(SK_ColorBLACK, false) );

    return appLib;
}

#if !defined(__EMSCRIPTEN__) || defined(__NO_CANVASKIT__)
sk_ui::SkiaAppLib * CreateSkiaAppLib() {
    return CreatePESTestAppSkiaAppLib();
}
#endif
