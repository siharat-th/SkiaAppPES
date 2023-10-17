#include "AppCreatorUComposer.h"
#include "tools/Resources.h"
#include "UComposerLayer.h"

using namespace sk_ui;


sk_ui::SkiaAppLib * CreateUComposerSkiaAppLib() {
    // force to use local source Resources.cpp
    SkString str = GetResourcePath();

    //SkDebugf( "CreateSkiaAppLib - UComposerLayer **\n" );
    sk_ui::SkiaAppLib * appLib = NULL;
    if( SKIAAPP() ) {
        //SkDebugf( "Use existed SkiaApp\n" );
    }
    else {
        //SkDebugf( "Create new SkiaAppLib\n" );
        appLib = new SkiaAppLib();
    }

    // Jimmy ** see it later ** applib and SKIAAPP() are not same.
    if( SKIAAPP() ) {
        //SkDebugf( "created SkiaApp\n" );
    }
    SKIAAPP()->setAppTitle( "SVGEditorApp" );
    //appLib->pushLayer( new LayersTestAppLayer("json/layers_test.json") );
    SKIAAPP()->pushLayer( new UComposerLayer() );
    SKIAAPP()->pushLayer( new FPSLayer(SK_ColorBLACK, false) );
    //SkDebugf( "return SKIAAPP()\n" );
    return SKIAAPP();
}

#if !defined(__EMSCRIPTEN__) || defined(__NO_CANVASKIT__)
sk_ui::SkiaAppLib * CreateSkiaAppLib() {
    return CreateUComposerSkiaAppLib();
}
#endif
