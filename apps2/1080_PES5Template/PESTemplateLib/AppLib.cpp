#include "AppLib.h"
#include "../src/AppCreatorPES5.h"

sk_ui::SkiaAppLib *PESTemplateLib::createApp() {
    return CreatePESTemplateAppSkiaAppLib();
}
