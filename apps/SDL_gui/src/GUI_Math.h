//
//  GUI_Math.h
//  Pukchue2
//
//  Created by SIHARAT THAMMAYA on 11/13/2559 BE.
//  Copyright © 2559 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef GUI_Math_h
#define GUI_Math_h

#include <stdio.h>
#include <sstream>  //for ostringsream
#include <SDL.h>
#include <vector>
#include <float.h>

#ifdef _WIN32
#pragma warning(disable : 4200) // Tooh - Fix warning C4200 : nonstandard extension used : zero - sized array
#include <algorithm> // std::swap
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

#ifndef TWO_PI
#define TWO_PI 6.28318530717958647693
#endif

#ifndef M_TWO_PI
#define M_TWO_PI 6.28318530717958647693
#endif

#ifndef FOUR_PI
#define FOUR_PI 12.56637061435917295385
#endif

#ifndef HALF_PI
#define HALF_PI 1.57079632679489661923
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (PI / 180.0)
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG (180.0 / PI)
#endif

#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef CLAMP
#define CLAMP(val,min,max) ((val) < (min) ? (min) : ((val > max) ? (max) : (val)))
#endif

#ifndef ABS
#define ABS(x) (((x) < 0) ? -(x) : (x))
#endif

#if !defined(_MSC_VER)
using namespace std;
#else
using std::ostream;
using std::istream;
#endif

float guiNormalize(float value, float min, float max);

float guiMin(float x, float y);
float guiMax(float x, float y);

float guiMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp = false);
float guiClamp(float value, float min, float max);

bool  guiInRange(int t, int min, int max);
bool  guiInRange(float t, float min, float max);

float guiRadToDeg(float radians);
float guiDegToRad(float degrees);

float guiLerp(float start, float stop, float amt);
float guiLerpDegrees(float currentAngle, float targetAngle, float pct);
float guiLerpRadians(float currentAngle, float targetAngle, float pct);

float guiDist(float x1, float y1, float x2, float y2);
float guiDist(float x1, float y1, float z1, float x2, float y2, float z2);

float guiDistSquared(float x1, float y1, float x2, float y2);
float guiDistSquared(float x1, float y1, float z1, float x2, float y2, float z2);

float guiAngleDifferenceDegrees(float currentAngle, float targetAngle);
float guiAngleDifferenceRadians(float currentAngle, float targetAngle);

float guiWrap(float value, float from, float to);
float guiWrapRadians(float angle, float from = static_cast<float>(-PI), float to = static_cast<float>(PI));
float guiWrapDegrees(float angle, float from = -180, float to = +180);

int guiNextPow2(int a);
int guiSign(float n);

#ifndef _WIN32
#pragma mark guiVector2
#endif

class ofVec3f;

class guiVec2f {
public:
    float x;
    float y;

    guiVec2f();
    guiVec2f(float x, float y);
    guiVec2f(const ofVec3f& vec);
    guiVec2f(const SDL_Point & p);

    void set(float x, float y);
    void set(const SDL_Point & p);
    
    // MARK: - Deprecated
    void moveToTarget(float x, float y);
    bool update();
    bool isReachTarget();
    // MARK: -

    guiVec2f& operator+=(const guiVec2f& vec);
    guiVec2f& operator-=(const guiVec2f& vec);
    bool      operator==(const guiVec2f& vec) const;
    bool match(const guiVec2f& vec, float tolerance = 0.0001f) const;

    guiVec2f  operator+(const guiVec2f& vec) const;
    guiVec2f  operator-(const guiVec2f& vec) const;
    guiVec2f  operator*(const guiVec2f& vec) const;
    guiVec2f  operator*(const float f) const;
    guiVec2f&  operator*=(const float f) const;
    guiVec2f  operator/(const float f) const;
    guiVec2f& operator/=(const float f);

    float distance(const guiVec2f& pnt) const;
    float squareDistance(const guiVec2f& pnt) const;
    guiVec2f   getMiddle(const guiVec2f& pnt) const;
    guiVec2f& normalize();

    float length() const;
    float lengthSquared() const;

    // returns The angle in degrees (-180...180)
    float angle(const guiVec2f& vec) const;
    // returns The angle in radians (-PI...PI)
    float angleRad(const guiVec2f& vec) const;

    float dot(const guiVec2f& vec) const;

    guiVec2f& rotate(float degree);
    // rotate around `pivot` rather than around the origin
    guiVec2f& rotate(float degree, const guiVec2f& pivot);

    guiVec2f& rotateRad(float rad);
    guiVec2f& rotateRad(float rad, const guiVec2f& pivot);

    guiVec2f& scale(const float length);

    // Perpendicular normalized vector.
    guiVec2f perpendiculared() const;

    guiVec2f getPerpendicular() const;

    guiVec2f& perpendicular();

    guiVec2f   getInterpolated(const guiVec2f& pnt, float p) const;
    guiVec2f&  interpolate(const guiVec2f& pnt, float p);

    friend ostream& operator<<(ostream& os, const guiVec2f& vec);
    friend istream& operator>>(istream& is, const guiVec2f& vec);
    
    SDL_Point toSDL() const;

//private:
//    float targetX, targetY;
//    bool reachTarget;
};

inline ostream& operator<<(ostream& os, const guiVec2f& vec) {
    os << vec.x << ", " << vec.y;
    return os;
}

inline istream& operator>>(istream& is, guiVec2f& vec) {
    is >> vec.x;
    is.ignore(2);
    is >> vec.y;
    return is;
}

// MARK: - Interpolation
class LerpInt {
public:
    LerpInt(int target) {
        setTarget(_target);
    }

    void setTarget(int target) {
        _target = target;
        _current = 0;
        _isFinished = _target == _current;
    }

    bool nextLerp(float lerpAmount, int* delta = NULL) {
        if (_isFinished)
            return _isFinished;

        if (SDL_abs(_current - _target) > 0) {
            float flerp = (_target - _current) * lerpAmount;
            int ilerp = _target < _current ? (int)floor(flerp) : (int)ceil(flerp);
            _current += ilerp;
            _isFinished = false;

            if (delta != NULL)
                *delta = ilerp;
        } else {
            _current = _target;
            _isFinished = true;
        }

        return _isFinished;
    }

    int getCurrent() {
        return _current;
    }

    bool isFinished() {
        return _isFinished;
    }

private:
    int _target, _current;
    bool _isFinished;
};

template<typename LerpType>
class _LERP {
    LerpType _target, _current;
    bool _isFinished;
    float _timeFinish, _delta;
    float _fSpeed;
    float _keyTime;

public:
    enum EaseType {
        LINEAR,
        HALF_SINE,
        HAFT_COSINE,
        QUATER_COSINE,
        EXPONENTIAL
    };

    _LERP() {
        _isFinished = false;
    }

    void set(LerpType current, LerpType target) {
        _current = current;
        _target = target;
        _isFinished = _current == _target;
    }

    bool nextLerp(float lerpAmount, LerpType* delta = NULL) {
        if (_isFinished)
            return _isFinished;

        if (fabs(_current - _target) > FLT_EPSILON) {
            LerpType flerp = (_target - _current) * lerpAmount;
            _current += flerp;
            _isFinished = false;

            if (delta != NULL)
                *delta = flerp;
        } else {
            _current = _target;
            _isFinished = true;
        }

        return _isFinished;
    }

    void set(LerpType current, LerpType target, float timeFinish_sec, EaseType ease) {
        _current = current;
        _target = target;
        _isFinished = _current == _target;
        _delta = (_target - current);
        _fSpeed = _delta / timeFinish_sec;
        _easeType = ease;
        _keyTime = 0;
    }

    bool step(float frameTime) {
        float amplitude = 1;

        switch (_easeType) {
            case LINEAR:
                amplitude = 1;
                break;

            case HALF_SINE:
                amplitude = guiMap(_keyTime, 0, _timeFinish, sin(0)*DEG_TO_RAD, sin(PI)*DEG_TO_RAD);
                break;

            case HAFT_COSINE:
                amplitude = guiMap(_keyTime, 0, _timeFinish, cos(0)*DEG_TO_RAD, cos(PI)*DEG_TO_RAD);
                break;

            case QUATER_COSINE:
                amplitude = guiMap(_keyTime, 0, _timeFinish, cos(0)*DEG_TO_RAD, cos(M_PI_2)*DEG_TO_RAD);
                break;

            case EXPONENTIAL:
                amplitude = guiMap(_keyTime, 0, _timeFinish, exp(0), exp(2));
                break;

            default:
                break;
        };

        _current += _fSpeed * amplitude * frameTime;
        _keyTime += frameTime;

        if (abs(_current - _target) < 0.001) {
            _current = _target;
            _isFinished = true;
        }

        return _isFinished;
    }

    LerpType getCurrent() {
        return _current;
    }

    bool isFinished() {
        return _isFinished;
    }

    EaseType _easeType;
};

typedef _LERP<float>    LerpFloat;
typedef _LERP<double>   LerpDouble;
typedef _LERP<int>      LerpInteger;

#define Lerp            LerpFloat

// catmall-rom
class guiCatmul {

public:
    void update() {
        catmulRom();
    }

    void catmulRom();

    float getT(float t, const guiVec2f& p0, const guiVec2f& p1);

    //How many points you want on the curve
    float amountOfPoints = 10.0f;

    //set from 0-1
    float alpha = 0.5f;

    //Store points on the Catmull curve so we can visualize them (this is output point)
    std::vector<guiVec2f> newPoints;

    //Use your data in 2d or 3d space as your points or define array with desired points (actually thi is an input points)
    SDL_Point points[];
};

//MARK: - Collision detection
bool guiLineCircleIntersec(guiVec2f v1, guiVec2f v2, guiVec2f center, float radius);

template<class vectype>
bool guiLineSegmentIntersection(const vectype& line1Start, const vectype& line1End, const vectype& line2Start, const vectype& line2End, vectype& intersection) {
    vectype diffLA, diffLB;
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

#endif /* GUI_Math_h */
