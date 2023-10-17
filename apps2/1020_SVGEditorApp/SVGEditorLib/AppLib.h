#ifndef __SVGEDITOR_APPLIB_H__
#define __SVGEDITOR_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreator.h"

class SVGEditorLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __SVGEDITOR_APPLIB_H__
