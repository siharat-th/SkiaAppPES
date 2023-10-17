//
//  quantize.hpp
//  all
//
//  Created by SIHARAT THAMMAYA on 27/9/2565 BE.
//

#ifndef quantize_hpp
#define quantize_hpp

#include "imagemap.hpp"

/**
 * Quantize an RGB image to a reduced number of colors.
 */
IndexedMap *rgbMapQuantize(RgbMap *rgbmap, int nrColors);

#endif /* quantize_hpp */
