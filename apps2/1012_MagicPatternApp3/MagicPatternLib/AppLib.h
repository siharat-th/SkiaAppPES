#ifndef __MAGICPATTERN_APPLIB_H__
#define __MAGICPATTERN_APPLIB_H__

#include "DeclarativeUI.h"
#include "../src/AppCreator.h"

class MagicPatternLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib* createApp(std::string option);
};

#endif  // __MAGICPATTERN_APPLIB_H__
