#ifndef __LOTTIE_APPLIB_H__
#define __LOTTIE_APPLIB_H__

#include "sk_ui.h"
#include "../src/AppCreator.h"

class LottieTestLib : public sk_ui::SkiaAppLib, sk_ui::SkiaAppLayer {
public:
    static sk_ui::SkiaAppLib *createApp();
};

#endif // __LOTTIE_APPLIB_H__
