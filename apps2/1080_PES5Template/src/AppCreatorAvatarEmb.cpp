#include "AppCreatorAvatarEmb.h"
#include "tools/Resources.h"
#include "GUI/PES5Layer.hpp"

using namespace sk_ui;


sk_ui::SkiaAppLib * CreateAvatarEmbAppSkiaAppLib() {
    // force to use local source Resources.cpp
    SkString str = GetResourcePath();

    sk_ui::SkiaAppLib * appLib = new SkiaAppLib();
    appLib->setAppTitle( "Avatar Embroidery" );
    appLib->pushLayer( new Pes5Layer("Avatar Embroidery") );
    appLib->pushLayer( new FPSLayer(SK_ColorBLACK, false) );

    return appLib;
}

#if !defined(__EMSCRIPTEN__) || defined(__NO_CANVASKIT__)
sk_ui::SkiaAppLib * CreateSkiaAppLib() {
    return CreateAvatarEmbAppSkiaAppLib();
}
#endif
