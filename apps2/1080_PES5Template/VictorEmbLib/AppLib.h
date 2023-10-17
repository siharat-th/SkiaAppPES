#ifndef __VICTOREMB_APPLIB_H__
#define __VICTOREMB_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreatorVictorEmb.h"

class VictorEmbLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __VICTOREMB_APPLIB_H__
