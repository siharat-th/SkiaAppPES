#ifndef __LAYERTEST_APPLIB_H__
#define __LAYERTEST_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreator.h"

class LayersTestLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __LAYERTEST_APPLIB_H__
