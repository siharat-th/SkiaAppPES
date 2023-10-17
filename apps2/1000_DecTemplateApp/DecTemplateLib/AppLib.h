#ifndef __DECTEMPLATE_APPLIB_H__
#define __DECTEMPLATE_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreator.h"

class DecTemplateLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __DECTEMPLATE_APPLIB_H__
