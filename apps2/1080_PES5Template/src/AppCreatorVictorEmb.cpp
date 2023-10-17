#include "AppCreatorVictorEmb.h"
#include "tools/Resources.h"
#include "GUI/PES5Layer.hpp"

using namespace sk_ui;


sk_ui::SkiaAppLib * CreateVictorEmbAppSkiaAppLib() {
    SkDebugf( "VictorEmb\n" );
    // force to use local source Resources.cpp
    SkString str = GetResourcePath();

    sk_ui::SkiaAppLib * appLib = new SkiaAppLib();
    appLib->setAppTitle( "Victor Embroider" );
    appLib->pushLayer( new Pes5Layer("Victor Embroidery") );
    appLib->pushLayer( new FPSLayer(SK_ColorBLACK, false) );

    return appLib;
}

#if !defined(__EMSCRIPTEN__) || defined(__NO_CANVASKIT__)
sk_ui::SkiaAppLib * CreateSkiaAppLib() {
    return CreateVictorEmbAppSkiaAppLib();
}
#endif
