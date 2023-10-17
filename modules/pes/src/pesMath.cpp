//
//  pesMath.cpp
//  pesEngine
//
//  Created by SIHARAT THAMMAYA on 2/7/2562 BE.
//  Copyright © 2562 SIHARAT THAMMAYA. All rights reserved.
//

#include "pesMath.hpp"
#include <float.h>
#include <algorithm>

using namespace std;

float pesMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp) {
    if (fabs(inputMin - inputMax) < FLT_EPSILON) {
        return outputMin;
    } else {
        float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
        
        if (clamp) {
            if (outputMax < outputMin) {
                if (outVal < outputMax)outVal = outputMax;
                else if (outVal > outputMin)outVal = outputMin;
            } else {
                if (outVal > outputMax)outVal = outputMax;
                else if (outVal < outputMin)outVal = outputMin;
            }
        }
        
        return outVal;
    }
}

bool pesInRange(int t, int min, int max) {
    return t >= min && t <= max;
}

bool pesInRange(float t, float min, float max) {
    return t >= min && t <= max;
}

float pesDist(float x1, float y1, float x2, float y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

float pesDistSquared(float x1, float y1, float x2, float y2) {
    return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

float pesWrap(float value, float from, float to) {
    // from http://stackoverflow.com/a/5852628/599884
    if (from > to) {
        std::swap(from, to);
    }
    
    float cycle = to - from;
    
    if (cycle == 0) {
        return to;
    }
    
    return value - cycle * floor((value - from) / cycle);
}

bool pesLineCircleIntersec(pesVec2f v1, pesVec2f v2, pesVec2f center, float radius){
    pesVec2f d = v2 - v1;
    pesVec2f f = v1 - center;
    
    float a = d.dot(d);
    float b = 2 * f.dot(d);
    float c = f.dot(f) - radius*radius;
    
    float discriminant = b*b - 4 * a*c;
    
    if (discriminant < 0) {
        // no intersection
        return false;
    } else {
        discriminant = sqrt(discriminant);
        float t1 = (-b - discriminant) / (2 * a);
        float t2 = (-b + discriminant) / (2 * a);
        
        if (t1 >= 0 && t1 <= 1) {
            // t1 is the intersection, and it's closer than t2
            // (since t1 uses -b - discriminant)
            return true;
        }
        
        // here t1 didn't intersect so we are either started
        // inside the sphere or completely past it
        if (t2 >= 0 && t2 <= 1) {
            return true;
        }
        
        return false;
    }
}

float pesClamp(float value, float min, float max) {
    return value < min ? min : value > max ? max : value;
}

float pesLerp(float start, float stop, float amt) {
    return start + (stop - start) * amt;
}

bool pesLineSegmentIntersection(const pesVec2f& line1Start, const pesVec2f& line1End, const pesVec2f& line2Start, const pesVec2f& line2End, pesVec2f& intersection){
    pesVec2f diffLA, diffLB;
    float compareA, compareB;
    diffLA = line1End - line1Start;
    diffLB = line2End - line2Start;
    compareA = diffLA.x*line1Start.y - diffLA.y*line1Start.x;
    compareB = diffLB.x*line2Start.y - diffLB.y*line2Start.x;
    if (
        (
         ((diffLA.x*line2Start.y - diffLA.y*line2Start.x) < compareA) ^
         ((diffLA.x*line2End.y - diffLA.y*line2End.x) < compareA)
         )
        &&
        (
         ((diffLB.x*line1Start.y - diffLB.y*line1Start.x) < compareB) ^
         ((diffLB.x*line1End.y - diffLB.y*line1End.x) < compareB)
         )
        ) {
        float lDetDivInv = 1 / ((diffLA.x*diffLB.y) - (diffLA.y*diffLB.x));
        intersection.x = -((diffLA.x*compareB) - (compareA*diffLB.x)) * lDetDivInv;
        intersection.y = -((diffLA.y*compareB) - (compareA*diffLB.y)) * lDetDivInv;
        
        return true;
    }
    
    return false;
}
