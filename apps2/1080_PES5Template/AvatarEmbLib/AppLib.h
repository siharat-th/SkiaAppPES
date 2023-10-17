#ifndef __AVATAREMB_APPLIB_H__
#define __AVATAREMB_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreatorAvatarEmb.h"

class AvatarEmbLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __AVATAREMB_APPLIB_H__
