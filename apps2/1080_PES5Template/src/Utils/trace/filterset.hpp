//
//  filterset.hpp
//  all
//
//  Created by SIHARAT THAMMAYA on 27/9/2565 BE.
//

#ifndef filterset_hpp
#define filterset_hpp

#include "imagemap.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    
    /**
     *  Apply gaussian blur to an GrayMap
     */
    GrayMap *grayMapGaussian(GrayMap *gmap);
    
    /**
     *  Apply gaussian bluf to an RgbMap
     */
    RgbMap *rgbMapGaussian(RgbMap *rgbmap);
    
    /**
     *
     */
    GrayMap *grayMapCanny(GrayMap *gmap,
                          double lowThreshold, double highThreshold);
    
    /**
     *
     */
    GrayMap *quantizeBand(RgbMap *rgbmap, int nrColors);
    
    
    
#ifdef __cplusplus
}
#endif

#endif /* filterset_hpp */
