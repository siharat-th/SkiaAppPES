#include "AppCreator.h"
#include "PEStestLayer.h"
#include "tools/Resources.h"
#include "include/core/SkTime.h"
#include "include/core/SkSwizzle.h"

using namespace sk_ui;

sk_ui::SkiaAppLib * CreateSkiaAppLib() {
    // force to use local source Resources.cpp
    SkString str = GetResourcePath();

    printf( "CreateSkiaAppLib - Hello\n" );

    sk_ui::SkiaAppLib * appLib = new SkiaAppLib();
    appLib->pushLayer( new PEStestLayer() );
    appLib->pushLayer( new WidgetLayer("json/pes_test.json") );
    appLib->pushLayer( new ImGuiLayer() );
    //appLib->pushLayer( new StatsLayer() );
    //appLib->pushLayer( new FPSLayer(SK_ColorBLACK, true) );

    return appLib;
}

