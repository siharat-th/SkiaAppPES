#include "AppCreator.h"
#include "MyWidgetLayer.h"

using namespace sk_ui;

sk_ui::SkiaAppLib * CreateSkiaAppLib() {
    sk_ui::SkiaAppLib * appLib = new SkiaAppLib();
    appLib->pushLayer( new MyWidgetLayer("json/app_template.json") );
    appLib->pushLayer( new FPSLayer() );

    return appLib;
}

