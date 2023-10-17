#ifndef __DECBWS_APPLIB_H__
#define __DECBWS_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreator.h"

class BWSLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __DECBWS_APPLIB_H__
