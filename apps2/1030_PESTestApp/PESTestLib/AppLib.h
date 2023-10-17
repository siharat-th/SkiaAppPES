#ifndef __PESTEST_APPLIB_H__
#define __PESTEST_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreator.h"

class PESTestLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __PESTEST_APPLIB_H__
