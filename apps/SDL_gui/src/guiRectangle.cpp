//
//  guiRectangle.cpp
//  HBFT
//
//  Created by Siharat on 7/4/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "guiRectangle.hpp"

// MARK: Rectangle(float)

//----------------------------------------------------------
guiRectangle::guiRectangle(){
    set(0,0,0,0);
}

//----------------------------------------------------------
guiRectangle::~guiRectangle(){}

//----------------------------------------------------------
guiRectangle::guiRectangle(float px, float py, float w, float h){
    set(px,py,w,h);
}

guiRectangle::guiRectangle(const SDL_Rect & iRect){
    set(iRect);
}

void guiRectangle::set(float px, float py, float w, float h) {
    x		= px;
    y		= py;
    width	= w;
    height	= h;
}

void guiRectangle::set(const ofPoint& p, float w, float h) {
    set(p.x, p.y, w, h);
}

void guiRectangle::set(const SDL_Rect & iRect){
    set((float)iRect.x, (float)iRect.y, (float)iRect.w, (float)iRect.h);
}

void guiRectangle::set(SDL_Rect* iRect){
    set((float)iRect->x, (float)iRect->y, (float)iRect->w, (float)iRect->h);
}

void guiRectangle::set(const guiRectangle & rect){
    set(rect.x, rect.y, rect.width, rect.height);
}

SDL_Rect guiRectangle::toSDL() const{
    return SDL_Rect{
        static_cast<int>(round(x)),
        static_cast<int>(round(y)),
        static_cast<int>(round(width)),
        static_cast<int>(round(height))};
}

//----------------------------------------------------------
void guiRectangle::setFromCenter(float px, float py, float w, float h) {
    set(px - w*0.5f, py - h*0.5f, w, h);
}

//----------------------------------------------------------
void guiRectangle::setFromCenter(const ofPoint& p, float w, float h) {
    setFromCenter(p.x, p.y, w, h);
}

//----------------------------------------------------------
void guiRectangle::growToInclude(float px, float py) {
    growToInclude(ofPoint(px,py));
}

//----------------------------------------------------------
void guiRectangle::growToInclude(const ofPoint& p) {
    float x0 = MIN(getMinX(),p.x);
    float x1 = MAX(getMaxX(),p.x);
    float y0 = MIN(getMinY(),p.y);
    float y1 = MAX(getMaxY(),p.y);
    float w = x1 - x0;
    float h = y1 - y0;
    set(x0,y0,w,h);
}

void guiRectangle::growToInclude(const guiRectangle& rect){
    float x0 = MIN(getMinX(),rect.getMinX());
    float x1 = MAX(getMaxX(),rect.getMaxX());
    float y0 = MIN(getMinY(),rect.getMinY());
    float y1 = MAX(getMaxY(),rect.getMaxY());
    float w = x1 - x0;
    float h = y1 - y0;
    set(x0,y0,w,h);
}

float guiRectangle::getMinX() const {
    return MIN(x, x + width);  // - width
}

//----------------------------------------------------------
float guiRectangle::getMaxX() const {
    return MAX(x, x + width);  // - width
}

//----------------------------------------------------------
float guiRectangle::getMinY() const{
    return MIN(y, y + height);  // - height
}

//----------------------------------------------------------
float guiRectangle::getMaxY() const {
    return MAX(y, y + height);  // - height
}

ofPoint guiRectangle::getPosition() {
    position.x = x;
    position .y = y;
    return position;
}

//----------------------------------------------------------
ofPoint guiRectangle::getCenter() const {
    return ofPoint(x + width * 0.5f, y + height * 0.5f, 0);
}

bool guiRectangle::intersects(const guiRectangle& rect) const{
    return (getMinX() < rect.getMaxX() && getMaxX() > rect.getMinX() &&
            getMinY() < rect.getMaxY() && getMaxY() > rect.getMinY());
}
bool guiRectangle::inside(float px, float py) const{
        return px > getMinX() && py > getMinY() &&
        px < getMaxX() && py < getMaxY();
    
//    if(px<position.x) return false;
//    if(py<position.y) return false;
//    if(px>position.x+width) return false;
//    if(py>position.y+height) return false;
//    return true;
}

void guiRectangle::translate(float dx, float dy) {
    x += dx;
    y += dy;
}

void guiRectangle::scale(float sX, float sY){
    width  *= sX;
    height *= sY;
}

void guiRectangle::scaleFromCenter(float sX, float sY){
    if(sX == 1.0f && sY == 1.0f) return; // nothing to do
    
    float newWidth  = width  * sX;
    float newHeight = height * sY;
    
    ofPoint center = getCenter();
    
    x = center.x - newWidth  / 2.0f;
    y = center.y - newHeight / 2.0f;
    
    width  = newWidth;
    height = newHeight;
}

guiRectangle guiRectangle::getIntersection(const guiRectangle& rect) const{
    float x0 = MAX(getMinX(),rect.getMinX());
    float x1 = MIN(getMaxX(),rect.getMaxX());
    
    float w = x1 - x0;
    if(w < 0.0f) return guiRectangle(0,0,0,0); // short circuit if needed
    
    float y0 = MAX(getMinY(),rect.getMinY());
    float y1 = MIN(getMaxY(),rect.getMaxY());
    
    float h = y1 - y0;
    if(h < 0.0f) return guiRectangle(0,0,0,0);  // short circuit if needed
    
    return guiRectangle(x0,y0,w,h);
}

guiRectangle guiRectangle::getUnion(const guiRectangle& rect) const{
    guiRectangle united = *this;
    united.growToInclude(rect);
    return united;
}

void guiRectangle::standardize(){
    if(width < 0.0f) {
        x += width;
        width = -1.0f * width;
    }
    
    if(height < 0.0f) {
        y += height;
        height = -1.0f * height;
    }
}
guiRectangle guiRectangle::getStandardized() const{
    if(isStandardized()) {
        return *this;
    } else {
        guiRectangle canRect(*this); // copy it
        canRect.standardize();
        return canRect;
    }
}

bool guiRectangle::isStandardized() const{
    return width >= 0.0f && height >= 0.0f;
}

float guiRectangle::getArea() const{
    return fabs(width) * fabs(height);
}

float guiRectangle::getPerimeter() const{
    return 2.0f * fabs(width) + 2.0f * fabs(height);
}

float guiRectangle::getAspectRatio() const{
    return fabs(width) / fabs(height);
}

void guiRectangle::scaleInTo(const guiRectangle& targetRect){
    float tw = targetRect.width; // target width
    float th = targetRect.height;  // target height
    float sw = width;   // subject width
    float sh = height;  // subject height
    if(fabs(sw) >= FLT_EPSILON || fabs(sh) >= FLT_EPSILON) {
        float wRatio = fabs(tw) / fabs(sw);
        float hRatio = fabs(th) / fabs(sh);
        float min = (std::min(wRatio,hRatio));
        scaleFromCenter(min, min);
    }
}

bool guiRectangle::isEmpty() const {
    return fabs(width)<=FLT_EPSILON && fabs(height)<=FLT_EPSILON;
}

bool guiRectangle::intersects(const guiVec2f& p0, const guiVec2f& p1) const {
    // check for a line intersection
    guiVec2f p;
    
    guiVec2f topLeft(getMinX(), getMinY());
    guiVec2f topRight(getMaxX(), getMinY());
    guiVec2f bottomRight(getMaxX(), getMaxY());
    guiVec2f bottomLeft(getMinX(), getMaxY());
    
    return inside(p0.x, p0.y) || // check end inside
        inside(p1.x, p1.y) || // check end inside
        guiLineSegmentIntersection(p0, p1, topLeft,     topRight,    p) || // cross top
        guiLineSegmentIntersection(p0, p1, topRight,    bottomRight, p) || // cross right
        guiLineSegmentIntersection(p0, p1, bottomRight, bottomLeft,  p) || // cross bottom
        guiLineSegmentIntersection(p0, p1, bottomLeft,  topLeft,     p);   // cross left
}

ostream& operator<<(ostream& os, const guiRectangle& rect){
    os << rect.x << ", " << rect.y << ", " << rect.width << ", " << rect.height;
    return os;
}
