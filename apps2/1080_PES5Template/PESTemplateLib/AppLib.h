#ifndef __PESTEMPLATE_APPLIB_H__
#define __PESTEMPLATE_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreatorPES5.h"

class PESTemplateLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __PESTEMPLATE_APPLIB_H__
