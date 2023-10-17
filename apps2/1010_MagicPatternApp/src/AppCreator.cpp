#include "AppCreator.h"
#include "tools/Resources.h"
//#include "LayersTestAppLayer.h"
#include "MagicPatternLayer.h"

using namespace sk_ui;


sk_ui::SkiaAppLib * CreateMagicPatternAppSkiaAppLib( std::string option ) {
    SkDebugf( "CreateMagicPatternAppSkiaAppLib %s\n", option.c_str() );
    // force to use local source Resources.cpp
    SkString str = GetResourcePath();

    SkDebugf("CreateSkiaAppLib - MagicPatternLib (Alpha2)\n");

    sk_ui::SkiaAppLib * appLib = new SkiaAppLib();
    appLib->setAppTitle( "MagicPatternApp" );
    //appLib->pushLayer( new LayersTestAppLayer("json/layers_test.json") );
    appLib->pushLayer( new MagicPatternLayer( option ) );
    appLib->pushLayer( new FPSLayer(SK_ColorBLACK, false) );

    return appLib;
}

#if !defined(__EMSCRIPTEN__) || defined(__NO_CANVASKIT__)
sk_ui::SkiaAppLib * CreateSkiaAppLib() {
    return CreateMagicPatternAppSkiaAppLib( "" );
}
#endif
