#include "AppLib.h"
#include "../src/AppCreatorAvatarEmb.h"

sk_ui::SkiaAppLib *AvatarEmbLib::createApp() {
    return CreateAvatarEmbAppSkiaAppLib();
}
