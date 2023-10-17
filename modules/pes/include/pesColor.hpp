//
//  pesColor.hpp
//  pesEngine
//
//  Created by SIHARAT THAMMAYA on 2/7/2562 BE.
//  Copyright © 2562 SIHARAT THAMMAYA. All rights reserved.
//

#ifndef pesColor_hpp
#define pesColor_hpp

#include <stdio.h>

class pesColor{
public:
    pesColor();
    pesColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a=0xff);
    pesColor(const pesColor&);

    //pesColor(const pesColor&) = default;
    pesColor(pesColor&&) = default;
    pesColor& operator=(const pesColor&) = default;
    pesColor& operator=(pesColor&&) = default;       
    
    void set(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha=0xff);
    void setHex(int hexColor, float alpha=0xff);
    int getHex() const;
    
    unsigned char r, g, b, a;
};

typedef struct pesBrotherThread_
{
    pesColor color;
    const char* description;
    const char* catalogNumber;
} pesBrotherThread;

pesColor pesGetBrotherColor(int index);
const char* pesGetBrotherColorName(int index);
const char* pesGetBrotherCatalogNumber(int index);
const char* pesGetBrotherColorNameWithCatalogNumber(int index);
pesBrotherThread pesGetBrotherThread(int index);
int pesGetNearestBrotherColorIndex( const pesColor & srcColor );

#endif /* pesColor_hpp */
