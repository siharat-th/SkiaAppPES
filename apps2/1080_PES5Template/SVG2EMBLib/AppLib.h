#ifndef __SVG2EMB_APPLIB_H__
#define __SVG2EMB_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreatorSVG2EMB.h"

class SVG2EMBLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __SVG2EMB_APPLIB_H__
