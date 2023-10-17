//
//  imagemap-skia.hpp
//  all
//
//  Created by SIHARAT THAMMAYA on 29/9/2565 BE.
//

#ifndef imagemap_skia_hpp
#define imagemap_skia_hpp

#include <stdio.h>
#include "imagemap.hpp"
#include "include/core/SkBitmap.h"


#ifdef __cplusplus
extern "C" {
#endif

    GrayMap *SkBitmapToGrayMap(const SkBitmap & bmp);
    SkBitmap *GrayMapToSkBitmap(GrayMap *grayMap);
    PackedPixelMap *SkBitmapToPackedPixelMap(const SkBitmap & bmp);
    RgbMap *SkBitmapToRgbMap(const SkBitmap & bmp);
    SkBitmap *IndexedMapToSkBitmap(IndexedMap *iMap);

//    Note
//    sk_sp<SkImage> SkBitmap::asImage() const


#ifdef __cplusplus
}
#endif

#endif /* imagemap_skia_hpp */
