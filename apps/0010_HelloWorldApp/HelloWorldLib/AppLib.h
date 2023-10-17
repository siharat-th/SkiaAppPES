#ifndef __APPLIB_H__
#define __APPLIB_H__

#include "sk_ui.h"
#include "../src/AppCreator.h"

class HelloAppLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __APPLIB_H__
