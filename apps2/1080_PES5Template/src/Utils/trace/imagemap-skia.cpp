//
//  imagemap-skia.cpp
//  all
//
//  Created by SIHARAT THAMMAYA on 29/9/2565 BE.
//

#include "imagemap-skia.hpp"

/*#########################################################################
 ## G R A Y M A P
 #########################################################################*/


GrayMap *SkBitmapToGrayMap(const SkBitmap & bmp){
    if (bmp.drawsNothing())
        return NULL;
    
    int width               = bmp.width();
    int height              = bmp.height();
    uint8_t *pixdata        = (uint8_t*)bmp.getPixels();
    int rowstride           = bmp.rowBytes();
    int n_channels          = bmp.bytesPerPixel();
    
    GrayMap *grayMap = GrayMapCreate(width, height);
    if (!grayMap)
        return NULL;
    
    
    //### Fill in the odd cells with RGB values
    int x,y;
    int row  = 0;
    for (y=0 ; y<height ; y++)
    {
        uint8_t *p = pixdata + row;
        for (x=0 ; x<width ; x++)
        {
            int alpha = (int)p[3];
            int white = 3 * (255-alpha);
            unsigned long sample = (int)p[0] + (int)p[1] +(int)p[2];
            unsigned long bright = sample * alpha / 256 + white;
            grayMap->setPixel(grayMap, x, y, bright);
            p += n_channels;
        }
        row += rowstride;
    }
    
    return grayMap;
}

SkBitmap *GrayMapToSkBitmap(GrayMap *grayMap){
    if (!grayMap)
        return NULL;
    
    SkBitmap * bitmap = new SkBitmap();
    // rowBytes of zero is treated as imageInfo.minRowBytes()
    int rowBytes = 0;
    bitmap->setInfo(SkImageInfo::Make(grayMap->width, grayMap->height,
                                      kRGBA_8888_SkColorType, kOpaque_SkAlphaType), rowBytes);
    int n_channels = bitmap->bytesPerPixel();
    int rowstride  = bitmap->rowBytes();
    int x,y;
    int row  = 0;
    uint8_t* pixdata = (uint8_t*)bitmap->getPixels();
    for (y=0 ; y<grayMap->height ; y++)
    {
        uint8_t *p = pixdata + row;
        for (x=0 ; x<grayMap->width ; x++)
        {
            unsigned long pix = grayMap->getPixel(grayMap, x, y) / 3;
            p[0] = p[1] = p[2] = (uint8_t)(pix & 0xff);
            p[3] = 0xff;
            p += n_channels;
        }
        row += rowstride;
    }
    
    return bitmap;
}





/*#########################################################################
 ## P A C K E D    P I X E L    M A P
 #########################################################################*/

PackedPixelMap *SkBitmapToPackedPixelMap(const SkBitmap & bmp){
    if (bmp.drawsNothing())
        return NULL;
    
    int width               = bmp.width();
    int height              = bmp.height();
    uint8_t *pixdata        = (uint8_t*)bmp.getPixels();
    int rowstride           = bmp.rowBytes();
    int n_channels          = bmp.bytesPerPixel();
    
    PackedPixelMap *ppMap = PackedPixelMapCreate(width, height);
    if (!ppMap)
        return NULL;
    
    int x,y;
    int row  = 0;
    for (y=0 ; y<height ; y++)
    {
        uint8_t *p = pixdata + row;
        for (x=0 ; x<width ; x++)
        {
            int alpha = (int)p[3];
            int white = 255 - alpha;
            int r     = (int)p[0];  r = r * alpha / 256 + white;
            int g     = (int)p[1];  g = g * alpha / 256 + white;
            int b     = (int)p[2];  b = b * alpha / 256 + white;
            
            ppMap->setPixel(ppMap, x, y, r, g, b);
            p += n_channels;
        }
        row += rowstride;
    }
    
    return ppMap;
}





/*#########################################################################
 ## R G B   M A P
 #########################################################################*/

RgbMap *SkBitmapToRgbMap(const SkBitmap & bmp){
    if (bmp.drawsNothing())
        return NULL;
    
    int width               = bmp.width();
    int height              = bmp.height();
    uint8_t *pixdata        = (uint8_t*)bmp.getPixels();
    int rowstride           = bmp.rowBytes();
    int n_channels          = bmp.bytesPerPixel();
    
    RgbMap *rgbMap = RgbMapCreate(width, height);
    if (!rgbMap)
        return NULL;
    
    int x,y;
    int row  = 0;
    for (y=0 ; y<height ; y++)
    {
        uint8_t *p = pixdata + row;
        for (x=0 ; x<width ; x++)
        {
            int alpha = (int)p[3];
            int white = 255 - alpha;
            int r     = (int)p[0];  r = r * alpha / 256 + white;
            int g     = (int)p[1];  g = g * alpha / 256 + white;
            int b     = (int)p[2];  b = b * alpha / 256 + white;
            
            rgbMap->setPixel(rgbMap, x, y, r, g, b);
            p += n_channels;
        }
        row += rowstride;
    }
    
    return rgbMap;
}




/*#########################################################################
 ## I N D E X E D   M A P
 #########################################################################*/

SkBitmap *IndexedMapToSkBitmap(IndexedMap *iMap){
    if (!iMap)
        return NULL;
    
    SkBitmap * bitmap = new SkBitmap();
    // rowBytes of zero is treated as imageInfo.minRowBytes()
    int rowBytes = 0;
    bitmap->setInfo(SkImageInfo::Make(iMap->width, iMap->height,
                                      kRGBA_8888_SkColorType, kOpaque_SkAlphaType), rowBytes);
    int n_channels = bitmap->bytesPerPixel();
    int rowstride  = bitmap->rowBytes();
    
    int x,y;
    int row  = 0;
    uint8_t* pixdata = (uint8_t*)bitmap->getPixels();
    int nrColors =  iMap->nrColors;
    RGB lastColor = iMap->clut[nrColors-1];
    for (y=0 ; y<iMap->height ; y++)
    {
        uint8_t *p = pixdata + row;
        for (x=0 ; x<iMap->width ; x++)
        {
            RGB rgb = iMap->getPixelValue(iMap, x, y);
            if(rgb.r==lastColor.r && rgb.g==lastColor.g && rgb.b==lastColor.b){
                p[0] = 0;
                p[1] = 0;
                p[2] = 0;
                p[3] = 0xff;
            }
            else{
#if defined(SK_BUILD_FOR_MAC)
                p[0] = rgb.b & 0xff;
                p[1] = rgb.g & 0xff;
                p[2] = rgb.r & 0xff;
#else
                p[0] = rgb.r & 0xff;
                p[1] = rgb.g & 0xff;
                p[2] = rgb.b & 0xff;
#endif
                p[3] = 0xff;
            }
            p += n_channels;
        }
        row += rowstride;
    }
    
    return bitmap;
}
