//
//  pesColor.cpp
//  pesEngine
//
//  Created by SIHARAT THAMMAYA on 2/7/2562 BE.
//  Copyright © 2562 SIHARAT THAMMAYA. All rights reserved.
//

#include "pesColor.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <algorithm>
#include <iostream>
#include <cmath>

pesColor::pesColor(){
    set(0xff, 0xff, 0xff, 0xff);
}

pesColor::pesColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    set(r, g, b, a);
}

pesColor::pesColor(const pesColor& c){
    set(c.r, c.g, c.b, c.a);
}

void pesColor::set(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha){
    r = red;
    g = green;
    b = blue;
    a = alpha;
}

void pesColor::setHex(int hexColor, float alpha){
    r = (hexColor >> 16) & 0xff;
    g = (hexColor >> 8) & 0xff;
    b = (hexColor >> 0) & 0xff;
    a = alpha;
}

int pesColor::getHex() const {
    return
    ((0xff & (unsigned char) r) << 16) |
    ((0xff & (unsigned char) g) << 8) |
    ((0xff & (unsigned char) b));
}

int pesGetNearestBrotherColorIndex( const pesColor & srcColor ){
    double currentClosestValue = 9999999;
    int closestIndex = -1;
    int red = srcColor.r;
    int green = srcColor.g;
    int blue = srcColor.b;
    for(int i=1; i<=64; i++)
    {
        int deltaRed;
        int deltaBlue;
        int deltaGreen;
        double dist;
        pesColor c = pesColor( pesGetBrotherColor(i) );
        
        deltaRed = red - c.r;
        deltaBlue = green - c.g;
        deltaGreen = blue - c.b;
        
        dist = sqrt((double)(deltaRed * deltaRed) + (deltaBlue * deltaBlue) + (deltaGreen * deltaGreen));
        if(dist <= currentClosestValue)
        {
            currentClosestValue = dist;
            closestIndex = i;
        }
    }
    
    return(closestIndex);
}
// MARK: - pesBrotherThread

const pesBrotherThread brother_thread_table[] = {
    {{168, 168, 168}, "Default",         "005"}, /* Index  0 */
    {{ 14,  31, 124}, "Prussian Blue",   "007"}, /* Index  1 */
    {{ 10,  85, 163}, "Blue",            "405"}, /* Index  2 */
    {{ 48, 135, 119}, "Teal Green",      "534"}, /* Index  3 */ /* TODO: Verify RGB value is correct */
    {{ 75, 107, 175}, "Cornflower Blue", "070"}, /* Index  4 */
    {{237,  23,  31}, "Red",             "800"}, /* Index  5 */
    {{209,  92,   0}, "Reddish Brown",   "337"}, /* Index  6 */
    {{145,  54, 151}, "Magenta",         "620"}, /* Index  7 */
    {{228, 154, 203}, "Light Lilac",     "810"}, /* Index  8 */
    {{145,  95, 172}, "Lilac",           "612"}, /* Index  9 */
    {{157, 214, 125}, "Mint Green",      "515"}, /* Index 10 */ /* TODO: Verify RGB value is correct */
    {{232, 169,   0}, "Deep Gold",       "214"}, /* Index 11 */
    {{254, 186,  53}, "Orange",          "208"}, /* Index 12 */
    {{255, 255,   0}, "Yellow",          "205"}, /* Index 13 */
    {{112, 188,  31}, "Lime Green",      "513"}, /* Index 14 */
    {{186, 152,   0}, "Brass",           "328"}, /* Index 15 */
    {{168, 168, 168}, "Silver",          "005"}, /* Index 16 */
    {{123, 111,   0}, "Russet Brown",    "337"}, /* Index 17 */ /* TODO: Verify RGB value is correct */
    {{255, 255, 179}, "Cream Brown",     "010"}, /* Index 18 */
    {{ 95, 101, 121}, "Pewter",          "704"}, /* Index 19 */
    {{  0,   0,   0}, "Black",           "900"}, /* Index 20 */
    {{ 11,  61, 145}, "Ultramarine",     "406"}, /* Index 21 */
    {{119,   1, 118}, "Royal Purple",    "869"}, /* Index 22 */
    {{ 41,  49,  51}, "Dark Gray",       "707"}, /* Index 23 */
    {{ 42,  19,   1}, "Dark Brown",      "058"}, /* Index 24 */
    {{246,  74, 138}, "Deep Rose",       "086"}, /* Index 25 */
    {{178, 118,  36}, "Light Brown",     "323"}, /* Index 26 */
    {{252, 187, 196}, "Salmon Pink",     "079"}, /* Index 27 */ /* TODO: Verify RGB value is correct */
    {{254,  55,  15}, "Vermillion",      "030"}, /* Index 28 */
    {{240, 240, 240}, "White",           "001"}, /* Index 29 */
    {{106,  28, 138}, "Violet",          "613"}, /* Index 30 */
    {{168, 221, 196}, "Seacrest",        "542"}, /* Index 31 */
    {{ 37, 132, 187}, "Sky Blue",        "019"}, /* Index 32 */
    {{254, 179,  67}, "Pumpkin",         "126"}, /* Index 33 */
    {{240, 231, 101}, "Cream Yellow",    "812"}, /* Index 34 */
    {{208, 166,  96}, "Khaki",           "348"}, /* Index 35 */
    {{209,  84,   0}, "Clay Brown",      "339"}, /* Index 36 */
    {{102, 186,  73}, "Leaf Green",      "509"}, /* Index 37 */
    {{ 19,  74,  70}, "Peacock Blue",    "415"}, /* Index 38 */
    {{110, 123, 119}, "Gray",            "817"}, /* Index 39 */
    {{216, 202, 198}, "Warm Gray",       "399"}, /* Index 40 */ /* TODO: Verify RGB value is correct */
    {{ 67,  86,   7}, "Dark Olive",      "517"}, /* Index 41 */
    {{240, 225, 198}, "Linen",           "307"}, /* Index 42 */
    {{249, 147, 188}, "Pink",            "085"}, /* Index 43 */
    {{  0,  56,  34}, "Deep Green",      "808"}, /* Index 44 */
    {{178, 175, 212}, "Lavender",        "804"}, /* Index 45 */
    {{104, 106, 176}, "Wisteria Violet", "607"}, /* Index 46 */
    {{239, 227, 185}, "Beige",           "843"}, /* Index 47 */
    {{247,  56, 102}, "Carmine",         "807"}, /* Index 48 */
    {{181,  76, 100}, "Amber Red",       "333"}, /* Index 49 */ /* TODO: Verify RGB value is correct */
    {{ 19,  43,  26}, "Olive Green",     "519"}, /* Index 50 */
    {{199,   1,  85}, "Dark Fuschia",    "107"}, /* Index 51 */ /* TODO: Verify RGB value is correct */
    {{254, 158,  50}, "Tangerine",       "209"}, /* Index 52 */
    {{168, 222, 235}, "Light Blue",      "017"}, /* Index 53 */
    {{  0, 103,  26}, "Emerald Green",   "507"}, /* Index 54 */ /* TODO: Verify RGB value is correct */
    {{ 78,  41, 144}, "Purple",          "614"}, /* Index 55 */
    {{ 47, 126,  32}, "Moss Green",      "515"}, /* Index 56 */
    {{254, 227, 197}, "Flesh Pink",      "124"}, /* Index 57 */ /* TODO: Verify RGB value is correct */
    {{255, 217,  17}, "Harvest Gold",    "206"}, /* Index 58 */
    {{  9,  91, 166}, "Electric Blue",   "420"}, /* Index 59 */
    {{240, 249, 112}, "Lemon Yellow",    "202"}, /* Index 60 */
    {{227, 243,  91}, "Fresh Green",     "027"}, /* Index 61 */
    {{160, 160, 160}, "Applique material","x9"}, /* Index 62 */ /* TODO: Verify RGB value is correct */
    {{160, 160, 160}, "Applique position","x8"}, /* Index 63 */ /* TODO: Verify RGB value is correct */
    {{160, 160, 160}, "Applique",         "x7"}  /* Index 64 */
};


pesColor pesGetBrotherColor(int index){
    if(index<1 || index>64)
        index = 0;
    pesBrotherThread pesThread = pesGetBrotherThread(index);
    return pesThread.color;
}

const char* pesGetBrotherColorName(int index){
    if(index<1 || index>64)
        index = 0;
    pesBrotherThread pesThread = pesGetBrotherThread(index);
    return pesThread.description;
}

const char* pesGetBrotherCatalogNumber(int index){
    if(index<1 || index>64)
        index = 0;
    pesBrotherThread pesThread = pesGetBrotherThread(index);
    return pesThread.catalogNumber;
}

static char __brother_thread_name[65][32];
const char* pesGetBrotherColorNameWithCatalogNumber(int index){
    pesBrotherThread pesThread = pesGetBrotherThread(index);
    if (index < 1 || index>64)
        index = 0;
    if(pesThread.catalogNumber[0]!='x'){
        sprintf(__brother_thread_name[index], "%s (%s)", pesThread.description, pesThread.catalogNumber);
        return __brother_thread_name[index];
    }
    return pesThread.description;
}

pesBrotherThread pesGetBrotherThread(int index){
    if(index<1 || index>64)
        index = 0;
    return brother_thread_table[index];
}

