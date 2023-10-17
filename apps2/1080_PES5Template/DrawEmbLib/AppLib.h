#ifndef __DRAWEMB_APPLIB_H__
#define __DRAWEMB_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreatorSVG2EMB.h"

class DrawEmbLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __DRAWEMB_APPLIB_H__
