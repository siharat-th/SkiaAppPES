#ifndef __UCOMPOSER_APPLIB_H__
#define __UCOMPOSER_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreatorUComposer.h"

class UComposerLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __UCOMPOSER_APPLIB_H__
