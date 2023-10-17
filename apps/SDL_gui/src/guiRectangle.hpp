//
//  guiRectangle.hpp
//  HBFT
//
//  Created by Siharat on 7/4/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef guiRectangle_hpp
#define guiRectangle_hpp

#include <stdio.h>
#include "SDL_gui.h"
#include "ofVec3f.h"

/// 2D rectangle. Since the `width` and `height` variables can be negative, the
/// x/y-position is not guaranteed to represent the upper right hand corner.
///
/// To avoid this ambiguity, users should should prefer "standardized"
/// rectangles. "Standardized" rectangles are rectangles whose width >= 0 and
/// height >= 0. The standardize() method can be used to ensure
/// that the is in it the "standard" form.

typedef ofVec3f ofPoint;

namespace of{
    namespace Point{
        const ofPoint None(0, 0, 0);
    }
}

enum RectMode{
    RECTMODE_CORNER=0,
    RECTMODE_CENTER=1
};

class guiRectangle{
public:
    guiRectangle();
    guiRectangle(float px, float py, float w, float h);
    guiRectangle(const SDL_Rect & iRect);   
    
    virtual ~guiRectangle();

    //guiRectangle(const guiRectangle&) = default;
    //guiRectangle(guiRectangle&&) = default;
    //guiRectangle& operator=(const guiRectangle&) = default;
    //guiRectangle& operator=(guiRectangle&&) = default;    
    
    void set(float px, float py, float w, float h);
    void set(const ofPoint& p, float w, float h);
    void set(const SDL_Rect & iRect);
    void set(SDL_Rect* iRect);
    void set(const guiRectangle & rect);
    
    void setFromCenter(float px, float py, float w, float h);
    void setFromCenter(const ofPoint& p, float w, float h);
    
    void growToInclude(float px, float py);
    void growToInclude(const ofPoint& p);
    void growToInclude(const guiRectangle& rect);
    
    ofPoint getPosition();
    ofPoint getCenter() const;
    
    float getMinX() const;
    float getMaxX() const;
    float getMinY() const;
    float getMaxY() const;
    
    bool intersects(const guiRectangle& rect) const;
    bool inside(float px, float py) const;
    
    void translate(float dx, float dy);
    void scale(float sX, float sY);
    void scaleFromCenter(float sX, float sY);
    
    guiRectangle getIntersection(const guiRectangle& rect) const;
    guiRectangle getUnion(const guiRectangle& rect) const;
    
    void standardize();
    guiRectangle getStandardized() const;
    bool isStandardized() const;
    
    float getArea() const;
    float getPerimeter() const;
    float getAspectRatio() const;
    
    bool isEmpty() const;
    
    /// \brief The x-position of the ofRectangle.
    float x;
    
    /// \brief The y-position of the ofRectangle.
    float y;
    
    /// \brief The width of the ofRectangle.
    float width;
    
    /// \brief The height of the ofRectangle.
    float height;
    
    SDL_Rect toSDL() const;
    
    void scaleInTo(const guiRectangle& targetRect);
    
    bool intersects(const guiVec2f& p0, const guiVec2f& p1) const;
    
private:
    ofPoint position;
};

ostream& operator<<(ostream& os, const guiRectangle& rect);

#endif /* guiRectangle_hpp */
