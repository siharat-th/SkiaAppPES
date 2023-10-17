//
//  GUI_Math.cpp
//  Pukchue2
//
//  Created by SIHARAT THAMMAYA on 11/13/2559 BE.
//  Copyright © 2559 Jimmy Software Co., Ltd. All rights reserved.
//


#include <cmath>
#include "float.h"
#include "GUI_Math.h"
#include "GUI_Log.h"
#include "ofVec3f.h"

float guiNormalize(float value, float min, float max) {
    return guiClamp((value - min) / (max - min), 0, 1);
}

float guiMin(float x, float y) {
    return x < y ? x : y;
}

float guiMax(float x, float y) {
    return x > y ? x : y;
}

float guiMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp) {
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

float guiClamp(float value, float min, float max) {
    return value < min ? min : value > max ? max : value;
}

bool  guiInRange(int t, int min, int max) {
    return t >= min && t <= max;
}

bool  guiInRange(float t, float min, float max) {
    return t >= min && t <= max;

}

float guiLerp(float start, float stop, float amt) {
    return start + (stop - start) * amt;
}

float guiLerpDegrees(float currentAngle, float targetAngle, float pct) {
    return currentAngle + guiAngleDifferenceDegrees(currentAngle, targetAngle) * pct;
}

float guiLerpRadians(float currentAngle, float targetAngle, float pct) {
    return currentAngle + guiAngleDifferenceRadians(currentAngle, targetAngle) * pct;
}

float guiDist(float x1, float y1, float x2, float y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

float guiDist(float x1, float y1, float z1, float x2, float y2, float z2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));
}

float guiDistSquared(float x1, float y1, float x2, float y2) {
    return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

float guiDistSquared(float x1, float y1, float z1, float x2, float y2, float z2) {
    return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));
}

float guiRadToDeg(float radians) {
    return (float)(radians * RAD_TO_DEG);
}

float guiDegToRad(float degrees) {
    return (float)(degrees * DEG_TO_RAD);
}

float guiAngleDifferenceDegrees(float currentAngle, float targetAngle) {
    return guiWrapDegrees(targetAngle - currentAngle);
}

float guiAngleDifferenceRadians(float currentAngle, float targetAngle) {
    return  guiWrapRadians(targetAngle - currentAngle);
}

float guiWrap(float value, float from, float to) {
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

float guiWrapRadians(float angle, float from, float to) {
    return guiWrap(angle, from, to);
}

float guiWrapDegrees(float angle, float from, float to) {
    return guiWrap(angle, from, to);
}

int guiNextPow2(int a) {
    // from nehe.gamedev.net lesson 43
    int rval = 1;
    while (rval < a) rval <<= 1;
    return rval;
}

int guiSign(float n) {
    if (n > 0) return 1;
    else if (n < 0) return -1;
    else return 0;
}

#ifndef _WIN32
#pragma mark guiVec2f
#endif

//guiVec2f::guiVec2f() : x(0), y(0), reachTarget(true) {
//}
//
//guiVec2f::guiVec2f(float _x, float _y) : x(_x), y(_y), reachTarget(true) {
//}
//
//guiVec2f::guiVec2f(const ofVec3f& vec) : reachTarget(true) {
//    x = vec.x;
//    y = vec.y;
//}

guiVec2f::guiVec2f() : x(0), y(0) {
}

guiVec2f::guiVec2f(float _x, float _y) : x(_x), y(_y) {
}

guiVec2f::guiVec2f(const ofVec3f& vec){
    x = vec.x;
    y = vec.y;
}

guiVec2f::guiVec2f(const SDL_Point & p){
    set(p);
}

void guiVec2f::set(float _x, float _y) {
    x = _x;
    y = _y;
}

void guiVec2f::set(const SDL_Point & p){
    set((float)p.x, (float)p.y);
}

guiVec2f guiVec2f::operator+(const guiVec2f& vec) const {
    return guiVec2f(x + vec.x, y + vec.y);
}

guiVec2f guiVec2f::operator-(const guiVec2f& vec) const {
    return guiVec2f(x - vec.x, y - vec.y);
}

guiVec2f guiVec2f::operator*(const guiVec2f& vec) const {
    return guiVec2f(x*vec.x, y*vec.y);
}

guiVec2f guiVec2f::operator*(const float f) const {
    return guiVec2f(x*f, y*f);
}

guiVec2f guiVec2f::operator/(const float f) const {
    return guiVec2f(x / f, y / f);
}

guiVec2f& guiVec2f::operator/=(const float f) {
    if (f == 0) return *this;

    x /= f;
    y /= f;

    return *this;
}

float guiVec2f::distance(const guiVec2f& pnt) const {
    float vx = x - pnt.x;
    float vy = y - pnt.y;

    return (float)sqrt(vx*vx + vy*vy);
}

float guiVec2f::squareDistance(const guiVec2f& pnt) const {
    float vx = x - pnt.x;
    float vy = y - pnt.y;

    return vx*vx + vy*vy;
}

guiVec2f guiVec2f::getMiddle(const guiVec2f& pnt) const {
    return guiVec2f((x + pnt.x) / 2.0f, (y + pnt.y) / 2.0f);
}

guiVec2f& guiVec2f::normalize() {
    float length = (float)sqrt(x*x + y*y);

    if (length > 0) {
        x /= length;
        y /= length;
    }

    return *this;
}

float guiVec2f::length() const {
    return (float)sqrt(x*x + y*y);
}

float guiVec2f::lengthSquared() const {
    return (float)(x*x + y*y);
}

// returns The angle in degrees (-180...180)
float guiVec2f::angle(const guiVec2f& vec) const {
    return (float)(atan2(x*vec.y - y*vec.x, x*vec.x + y*vec.y)*RAD_TO_DEG);
}

// returns The angle in radians (-PI...PI)
float guiVec2f::angleRad(const guiVec2f& vec) const {
    return atan2(x*vec.y - y*vec.x, x*vec.x + y*vec.y);
}

float guiVec2f::dot(const guiVec2f& vec) const {
    return x*vec.x + y*vec.y;
}

guiVec2f& guiVec2f::rotate(float degree) {
    float a = (float)(degree * DEG_TO_RAD);
    float xrot = x*cos(a) - y*sin(a);
    y = x*sin(a) + y*cos(a);
    x = xrot;
    return *this;
}

// rotate around `pivot` rather than around the origin
guiVec2f& guiVec2f::rotate(float degree, const guiVec2f& pivot) {
    float a = (float)(degree * DEG_TO_RAD);
    float xrot = ((x - pivot.x)*cos(a) - (y - pivot.y)*sin(a)) + pivot.x;
    y = ((x - pivot.x)*sin(a) + (y - pivot.y)*cos(a)) + pivot.y;
    x = xrot;
    return *this;
}

guiVec2f& guiVec2f::rotateRad(float rad) {
    float a = rad;
    float xrot = x*cos(a) - y*sin(a);
    y = x*sin(a) + y*cos(a);
    x = xrot;
    return *this;
}

guiVec2f& guiVec2f::rotateRad(float rad, const guiVec2f& pivot) {
    float a = rad;
    float xrot = ((x - pivot.x)*cos(a) - (y - pivot.y)*sin(a)) + pivot.x;
    y = ((x - pivot.x)*sin(a) + (y - pivot.y)*cos(a)) + pivot.y;
    x = xrot;
    return *this;
}

guiVec2f& guiVec2f::scale(const float length) {
    float l = (float)sqrt(x*x + y*y);
    if (l > 0) {
        x = (x / l)*length;
        y = (y / l)*length;
    }

    return *this;
}

guiVec2f& guiVec2f::operator+=(const guiVec2f& vec) {
    x += vec.x;
    y += vec.y;

    return *this;
}

guiVec2f& guiVec2f::operator-=(const guiVec2f& vec) {
    x -= vec.x;
    y -= vec.y;

    return *this;
}

bool guiVec2f::operator==(const guiVec2f& vec) const {
    return (x == vec.x) && (y == vec.y);
}

bool guiVec2f::match(const guiVec2f& vec, float tolerance) const {
    return (fabs(x - vec.x) < tolerance)
        && (fabs(y - vec.y) < tolerance);
}

// Perpendicular normalized vector.
guiVec2f guiVec2f::perpendiculared() const {
    return getPerpendicular();
}

guiVec2f guiVec2f::getPerpendicular() const {
    float length = (float)sqrt(x*x + y*y);

    if (length > 0)
        return guiVec2f(-(y / length), x / length);
    else
        return guiVec2f();
}

guiVec2f& guiVec2f::perpendicular() {
    float length = (float)sqrt(x*x + y*y);

    if (length > 0) {
        float _x = x;
        x = -(y / length);
        y = _x / length;
    }

    return *this;
}

SDL_Point guiVec2f::toSDL() const{
    SDL_Point p;
    p.x = (int)round(x);
    p.y = (int)round(y);
    return p;
}

inline guiVec2f guiVec2f::getInterpolated(const guiVec2f& pnt, float p) const {
    return guiVec2f(x*(1 - p) + pnt.x*p, y*(1 - p) + pnt.y*p);
}

inline guiVec2f& guiVec2f::interpolate(const guiVec2f& pnt, float p) {
    x = x*(1 - p) + pnt.x*p;
    y = y*(1 - p) + pnt.y*p;

    return *this;
}

void guiVec2f::moveToTarget(float x, float y) {
//    targetX = x;
//    targetY = y;
//    reachTarget = false;
}

bool guiVec2f::update() {
//    if (reachTarget) return true;
//    guiVec2f target(targetX, targetY);
//
//    if (distance(target) < 0.01) {
//        *this = target;
//        reachTarget = true;
//        return true;
//    } else {
//        guiVec2f cur(x, y);
//        cur = cur + (target - cur)*0.5;
//
//        if (cur.distance(target) < 0.01) {
//            *this = target;
//            reachTarget = true;
//            return true;
//        }
//
//        *this = cur;
//    }

    return false;
}

bool guiVec2f::isReachTarget() {
//    return reachTarget;
    return false;
}

// catmul-rom
void guiCatmul::catmulRom() {
    newPoints.clear();

    guiVec2f p0 = guiVec2f((float)points[0].x, (float)points[0].y);
    guiVec2f p1 = guiVec2f((float)points[1].x, (float)points[1].y);
    guiVec2f p2 = guiVec2f((float)points[2].x, (float)points[2].y);
    guiVec2f p3 = guiVec2f((float)points[3].x, (float)points[3].y);

    float t0 = 0.0f;
    float t1 = getT(t0, p0, p1);
    float t2 = getT(t1, p1, p2);
    float t3 = getT(t2, p2, p3);

    for (float t = t1; t < t2; t += ((t2 - t1) / amountOfPoints)) {
        guiVec2f A1 = p0*(t1 - t) / (t1 - t0) + p1*(t - t0) / (t1 - t0);
        guiVec2f A2 = p1*(t2 - t) / (t2 - t1) + p2*(t - t1) / (t2 - t1);
        //guiVec2f A3 = p3*(t3-t)/(t3-t2) + p2*(t-t2)/(t3-t2);

        guiVec2f B1 = A1*(t2 - t) / (t2 - t0) + A1*(t - t0) / (t2 - t0);
        guiVec2f B2 = A2*(t3 - t) / (t3 - t1) + A2*(t - t1) / (t3 - t1);

        guiVec2f C = B1*(t2 - t) / (t2 - t1) + B2*(t - t1) / (t2 - t1);

        newPoints.push_back(C);
    }
}

float guiCatmul::getT(float t, const guiVec2f& p0, const guiVec2f& p1) {
    float a = pow((p1.x - p0.x), 2.0f) + pow((p1.y - p0.y), 2.0f);
    float b = pow(a, 0.5f);
    float c = pow(b, alpha);

    return (c + t);
}

#ifndef _WIN32
#pragma mark Collision detection
#endif

bool guiLineCircleIntersec(guiVec2f v1, guiVec2f v2, guiVec2f center, float radius) {
    guiVec2f d = v2 - v1;
    guiVec2f f = v1 - center;

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
