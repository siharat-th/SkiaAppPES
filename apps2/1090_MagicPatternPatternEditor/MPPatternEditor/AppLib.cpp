#include "AppLib.h"
#include "../src/AppCreator.h"

sk_ui::SkiaAppLib *SVGEditorLib::createApp() {
    SkDebugf( "SVGEditorLib::SVGEditorLib **\n");
    return CreateSVGEditorAppSkiaAppLib();
}
