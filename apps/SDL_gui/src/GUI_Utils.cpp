//
// Created by Jimmy on 2/10/2016 AD.
//

#include <SDL.h>
#include <string>
#include <iostream>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include <assert.h>
#include <errno.h>
#include <bitset>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>	// for gethostbyname()
#include <errno.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#define _WINSOCKAPI_  // Tooh - stops Windows.h including winsock.h to prevent macro redefinition in SDL_net.h 
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#ifndef __MINGW32__
#define strcasecmp stricmp
#endif
#endif

#include "SDL_gui.h"
#include "GUI_Utils.h"
#include "SDL_image.h"

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef _WIN32
#include <direct.h>  // for _chdir(), _getcwd()
#define GetCurrentDir _getcwd
#define	chdir _chdir // pom
#else
#include <unistd.h>
#include <dirent.h> // for DIR, dirent, opendir()
#include <pwd.h>
#define GetCurrentDir getcwd
#endif

#ifdef __ANDROID__
static const char* LOGNAME = "__GUI__";
#endif

#if __IPHONEOS__
extern char __iOS_DOCUMENTS_FOLDER[512];
#endif

GUI_Rect::GUI_Rect() {
    x = y = w = h = 0;
}
GUI_Rect::GUI_Rect(Sint16 _x, Sint16 _y, Uint16 _dx, Uint16 _dy) {
    x = _x; y = _y; w = _dx; h = _dy;
}
void GUI_Rect::set(Sint16 _x, Sint16 _y, Uint16 _dx, Uint16 _dy) {
    x = _x; y = _y; w = _dx; h = _dy;
}

#if __USE_SKIA__
SkRect GUI_Rect::toSkRect(){
    return SkRect::MakeXYWH(x, y, w, h);
}
#endif


GUI_Point::GUI_Point(short x1, short y1) :x(x1), y(y1) {
}

GUI_Point::GUI_Point() : x(0), y(0) {
}

void GUI_Point::set(short x1, short y1) {
    x = x1; y = y1;
}

bool GUI_Point::operator==(GUI_Point b) {
    return x == b.x && y == b.y;
}

bool GUI_Point::operator!=(GUI_Point b) {
    return x != b.x || y != b.y;
}

SDL_Color sdl_color(Uint32 c) {
    SDL_Color sdl_c;
    sdl_c.r = c >> 24 & 0xff;
    sdl_c.g = c >> 16 & 0xff;
    sdl_c.b = c >> 8 & 0xff;
    sdl_c.a = c & 0xff;
    return sdl_c;
}

#if __USE_SKIA__
SkColor sk_color(SDL_Color c){
    return (c.a << 24) | (c.r << 16) | (c.g << 8) | c.b;
}
#endif

static const Uint32
red = 0xff0000ff,
green = 0x00ff00ff,
darkgrey = 0x707070ff,
white = 0xffffffff,
grey = 0xc0c0c0ff,
lightgrey = 0xe0e0e0ff;

SDL_Color
cClear = {0, 0, 0, 0},
cNop = {0, 0, 0, 1},
cWhite = sdl_color(white),
cBlack = sdl_color(0xff),
cGrey = sdl_color(grey),
cRed = sdl_color(red),
cGreen = sdl_color(green),
cBlue = sdl_color(0xffff),
cCyan = sdl_color(0xffffff),
cMagenta = sdl_color(0xff00ffff),
cYellow = sdl_color(0xffff00ff),
cDarkGrey = sdl_color(darkgrey),
cLightGrey = sdl_color(lightgrey),
cCornFlower = sdl_color(0x6495edff),
cHightLightSelected = sdl_color(0x338dfbff),
cTextSelected = sdl_color(0x9cc7e6ff); // Tooh - text selection

// pom
// Note:
//      - default log level is GUI_LOG_NOTICE
//      - set log level=GUI_LOG_VERBOSE(lowest level) will display all log
//      - set log level=GUI_LOG_SILENT(highest level) will disable all log
//      - can be use like this  GUI_LogError("Class::Method") << "error message: " << [variable] << std::endl;
//      - more info see GUI_Log.h/.cpp
void GUI_Log(const char * format, ...) {
#ifdef __ANDROID__
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    __android_log_print(ANDROID_LOG_VERBOSE, LOGNAME, buffer, 1);
#else

    //if (GUI_GetLogLevel() <= GUI_LOG_NOTICE) 
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
#endif
}

#include "SDL2_gfxPrimitives.h"
#ifdef _NOOM_

/* ---- Pixel */
/*!
\brief Draw pixel  in currently set color.

\param renderer The renderer to draw on.
\param x X (horizontal) coordinate of the pixel.
\param y Y (vertical) coordinate of the pixel.

\returns Returns 0 on success, -1 on failure.
*/
int pixel(SDL_Renderer *renderer, Sint16 x, Sint16 y) {
    return SDL_RenderDrawPoint(renderer, x, y);
}

/*!
\brief Draw pixel with blending enabled if a<255.

\param renderer The renderer to draw on.
\param x X (horizontal) coordinate of the pixel.
\param y Y (vertical) coordinate of the pixel.
\param color The color value of the pixel to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int pixelColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return pixelRGBA(renderer, x, y, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw pixel with blending enabled if a<255.

\param renderer The renderer to draw on.
\param x X (horizontal) coordinate of the pixel.
\param y Y (vertical) coordinate of the pixel.
\param r The red color value of the pixel to draw.
\param g The green color value of the pixel to draw.
\param b The blue color value of the pixel to draw.
\param a The alpha value of the pixel to draw.

\returns Returns 0 on success, -1 on failure.
*/
int pixelRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);
    result |= SDL_RenderDrawPoint(renderer, x, y);
    return result;
}

/*!
\brief Draw pixel with blending enabled and using alpha weight on color.

\param renderer The renderer to draw on.
\param x The horizontal coordinate of the pixel.
\param y The vertical position of the pixel.
\param r The red color value of the pixel to draw.
\param g The green color value of the pixel to draw.
\param b The blue color value of the pixel to draw.
\param a The alpha value of the pixel to draw.
\param weight The weight multiplied into the alpha value of the pixel.

\returns Returns 0 on success, -1 on failure.
*/
int pixelRGBAWeight(SDL_Renderer * renderer, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a, Uint32 weight) {
    /*
    * Modify Alpha by weight
    */
    Uint32 ax = a;
    ax = ((ax * weight) >> 8);

    if (ax > 255) {
        a = 255;
    } else {
        a = (Uint8)(ax & 0x000000ff);
    }

    return pixelRGBA(renderer, x, y, r, g, b, a);
}

/* ---- Hline */
/*!
\brief Draw horizontal line in currently set color

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point (i.e. left) of the line.
\param x2 X coordinate of the second point (i.e. right) of the line.
\param y Y coordinate of the points of the line.

\returns Returns 0 on success, -1 on failure.
*/
int hline(SDL_Renderer * renderer, Sint16 x1, Sint16 x2, Sint16 y) {
    return SDL_RenderDrawLine(renderer, x1, y, x2, y);
}

/*!
\brief Draw horizontal line with blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point (i.e. left) of the line.
\param x2 X coordinate of the second point (i.e. right) of the line.
\param y Y coordinate of the points of the line.
\param color The color value of the line to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int hlineColor(SDL_Renderer * renderer, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return hlineRGBA(renderer, x1, x2, y, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw horizontal line with blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point (i.e. left) of the line.
\param x2 X coordinate of the second point (i.e. right) of the line.
\param y Y coordinate of the points of the line.
\param r The red value of the line to draw.
\param g The green value of the line to draw.
\param b The blue value of the line to draw.
\param a The alpha value of the line to draw.

\returns Returns 0 on success, -1 on failure.
*/
int hlineRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);
    result |= SDL_RenderDrawLine(renderer, x1, y, x2, y);
    return result;
}

/* ---- Vline */
/*!
\brief Draw vertical line with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the points of the line.
\param y1 Y coordinate of the first point (i.e. top) of the line.
\param y2 Y coordinate of the second point (i.e. bottom) of the line.
\param color The color value of the line to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int vlineColor(SDL_Renderer * renderer, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return vlineRGBA(renderer, x, y1, y2, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw vertical line with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the points of the line.
\param y1 Y coordinate of the first point (i.e. top) of the line.
\param y2 Y coordinate of the second point (i.e. bottom) of the line.
\param r The red value of the line to draw.
\param g The green value of the line to draw.
\param b The blue value of the line to draw.
\param a The alpha value of the line to draw.

\returns Returns 0 on success, -1 on failure.
*/
int vlineRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y1, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);
    result |= SDL_RenderDrawLine(renderer, x, y1, x, y2);
    return result;
}

/* ---- Rectangle */
/*!
\brief Draw rectangle with blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the rectangle.
\param y1 Y coordinate of the first point (i.e. top right) of the rectangle.
\param x2 X coordinate of the second point (i.e. bottom left) of the rectangle.
\param y2 Y coordinate of the second point (i.e. bottom left) of the rectangle.
\param color The color value of the rectangle to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int rectangleColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return rectangleRGBA(renderer, x1, y1, x2, y2, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw rectangle with blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the rectangle.
\param y1 Y coordinate of the first point (i.e. top right) of the rectangle.
\param x2 X coordinate of the second point (i.e. bottom left) of the rectangle.
\param y2 Y coordinate of the second point (i.e. bottom left) of the rectangle.
\param r The red value of the rectangle to draw.
\param g The green value of the rectangle to draw.
\param b The blue value of the rectangle to draw.
\param a The alpha value of the rectangle to draw.

\returns Returns 0 on success, -1 on failure.
*/
int rectangleRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int result;
    Sint16 tmp;
    SDL_Rect rect;

    /*
    * Test for special cases of straight lines or single point
    */
    if (x1 == x2) {
        if (y1 == y2) {
            return (pixelRGBA(renderer, x1, y1, r, g, b, a));
        } else {
            return (vlineRGBA(renderer, x1, y1, y2, r, g, b, a));
        }
    } else {
        if (y1 == y2) {
            return (hlineRGBA(renderer, x1, x2, y1, r, g, b, a));
        }
    }

    /*
    * Swap x1, x2 if required
    */
    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    /*
    * Swap y1, y2 if required
    */
    if (y1 > y2) {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    /*
    * Create destination rect
    */
    rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1;
    rect.h = y2 - y1;

    /*
    * Draw
    */
    result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);
    result |= SDL_RenderDrawRect(renderer, &rect);
    return result;
}

/* ---- Rounded Rectangle */
/*!
\brief Draw rounded-corner rectangle with blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the rectangle.
\param y1 Y coordinate of the first point (i.e. top right) of the rectangle.
\param x2 X coordinate of the second point (i.e. bottom left) of the rectangle.
\param y2 Y coordinate of the second point (i.e. bottom left) of the rectangle.
\param rad The radius of the corner arc.
\param color The color value of the rectangle to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int roundedRectangleColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return roundedRectangleRGBA(renderer, x1, y1, x2, y2, rad, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw rounded-corner rectangle with blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the rectangle.
\param y1 Y coordinate of the first point (i.e. top right) of the rectangle.
\param x2 X coordinate of the second point (i.e. bottom left) of the rectangle.
\param y2 Y coordinate of the second point (i.e. bottom left) of the rectangle.
\param rad The radius of the corner arc.
\param r The red value of the rectangle to draw.
\param g The green value of the rectangle to draw.
\param b The blue value of the rectangle to draw.
\param a The alpha value of the rectangle to draw.

\returns Returns 0 on success, -1 on failure.
*/
int roundedRectangleRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int result = 0;
    Sint16 tmp;
    Sint16 w, h;
    Sint16 xx1, xx2;
    Sint16 yy1, yy2;

    /*
    * Check renderer
    */
    if (renderer == NULL) {
        return -1;
    }

    /*
    * Check radius vor valid range
    */
    if (rad < 0) {
        return -1;
    }

    /*
    * Special case - no rounding
    */
    if (rad <= 1) {
        return rectangleRGBA(renderer, x1, y1, x2, y2, r, g, b, a);
    }

    /*
    * Test for special cases of straight lines or single point
    */
    if (x1 == x2) {
        if (y1 == y2) {
            return (pixelRGBA(renderer, x1, y1, r, g, b, a));
        } else {
            return (vlineRGBA(renderer, x1, y1, y2, r, g, b, a));
        }
    } else {
        if (y1 == y2) {
            return (hlineRGBA(renderer, x1, x2, y1, r, g, b, a));
        }
    }

    /*
    * Swap x1, x2 if required
    */
    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    /*
    * Swap y1, y2 if required
    */
    if (y1 > y2) {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    /*
    * Calculate width&height
    */
    w = x2 - x1;
    h = y2 - y1;

    /*
    * Maybe adjust radius
    */
    if ((rad * 2) > w) {
        rad = w / 2;
    }
    if ((rad * 2) > h) {
        rad = h / 2;
    }

    /*
    * Draw corners
    */
    xx1 = x1 + rad;
    xx2 = x2 - rad;
    yy1 = y1 + rad;
    yy2 = y2 - rad;
    result |= arcRGBA(renderer, xx1, yy1, rad, 180, 270, r, g, b, a);
    result |= arcRGBA(renderer, xx2, yy1, rad, 270, 360, r, g, b, a);
    result |= arcRGBA(renderer, xx1, yy2, rad, 90, 180, r, g, b, a);
    result |= arcRGBA(renderer, xx2, yy2, rad, 0, 90, r, g, b, a);

    /*
    * Draw lines
    */
    if (xx1 <= xx2) {
        result |= hlineRGBA(renderer, xx1, xx2, y1, r, g, b, a);
        result |= hlineRGBA(renderer, xx1, xx2, y2, r, g, b, a);
    }
    if (yy1 <= yy2) {
        result |= vlineRGBA(renderer, x1, yy1, yy2, r, g, b, a);
        result |= vlineRGBA(renderer, x2, yy1, yy2, r, g, b, a);
    }

    return result;
}

/* ---- Rounded Box */
/*!
\brief Draw rounded-corner box (filled rectangle) with blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the box.
\param y1 Y coordinate of the first point (i.e. top right) of the box.
\param x2 X coordinate of the second point (i.e. bottom left) of the box.
\param y2 Y coordinate of the second point (i.e. bottom left) of the box.
\param rad The radius of the corner arcs of the box.
\param color The color value of the box to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int roundedBoxColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return roundedBoxRGBA(renderer, x1, y1, x2, y2, rad, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw rounded-corner box (filled rectangle) with blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the box.
\param y1 Y coordinate of the first point (i.e. top right) of the box.
\param x2 X coordinate of the second point (i.e. bottom left) of the box.
\param y2 Y coordinate of the second point (i.e. bottom left) of the box.
\param rad The radius of the corner arcs of the box.
\param r The red value of the box to draw.
\param g The green value of the box to draw.
\param b The blue value of the box to draw.
\param a The alpha value of the box to draw.

\returns Returns 0 on success, -1 on failure.
*/
int roundedBoxRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2,
                   Sint16 y2, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int result;
    Sint16 w, h, r2, tmp;
    Sint16 cx = 0;
    Sint16 cy = rad;
    Sint16 ocx = (Sint16)0xffff;
    Sint16 ocy = (Sint16)0xffff;
    Sint16 df = 1 - rad;
    Sint16 d_e = 3;
    Sint16 d_se = -2 * rad + 5;
    Sint16 xpcx, xmcx, xpcy, xmcy;
    Sint16 ypcy, ymcy, ypcx, ymcx;
    Sint16 x, y, dx, dy;

    /*
    * Check destination renderer
    */
    if (renderer == NULL) {
        return -1;
    }

    /*
    * Check radius vor valid range
    */
    if (rad < 0) {
        return -1;
    }

    /*
    * Special case - no rounding
    */
    if (rad <= 1) {
        return rectangleRGBA(renderer, x1, y1, x2, y2, r, g, b, a);
    }

    /*
    * Test for special cases of straight lines or single point
    */
    if (x1 == x2) {
        if (y1 == y2) {
            return (pixelRGBA(renderer, x1, y1, r, g, b, a));
        } else {
            return (vlineRGBA(renderer, x1, y1, y2, r, g, b, a));
        }
    } else {
        if (y1 == y2) {
            return (hlineRGBA(renderer, x1, x2, y1, r, g, b, a));
        }
    }

    /*
    * Swap x1, x2 if required
    */
    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    /*
    * Swap y1, y2 if required
    */
    if (y1 > y2) {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    /*
    * Calculate width&height
    */
    w = x2 - x1 + 1;
    h = y2 - y1 + 1;

    /*
    * Maybe adjust radius
    */
    r2 = rad + rad;
    if (r2 > w) {
        rad = w / 2;
        r2 = rad + rad;
    }
    if (r2 > h) {
        rad = h / 2;
    }

    /* Setup filled circle drawing for corners */
    x = x1 + rad;
    y = y1 + rad;
    dx = x2 - x1 - rad - rad;
    dy = y2 - y1 - rad - rad;

    /*
    * Set color
    */
    result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);

    /*
    * Draw corners
    */
    do {
        xpcx = x + cx;
        xmcx = x - cx;
        xpcy = x + cy;
        xmcy = x - cy;
        if (ocy != cy) {
            if (cy > 0) {
                ypcy = y + cy;
                ymcy = y - cy;
                result |= hline(renderer, xmcx, xpcx + dx, ypcy + dy);
                result |= hline(renderer, xmcx, xpcx + dx, ymcy);
            } else {
                result |= hline(renderer, xmcx, xpcx + dx, y);
            }
            ocy = cy;
        }
        if (ocx != cx) {
            if (cx != cy) {
                if (cx > 0) {
                    ypcx = y + cx;
                    ymcx = y - cx;
                    result |= hline(renderer, xmcy, xpcy + dx, ymcx);
                    result |= hline(renderer, xmcy, xpcy + dx, ypcx + dy);
                } else {
                    result |= hline(renderer, xmcy, xpcy + dx, y);
                }
            }
            ocx = cx;
        }

        /*
        * Update
        */
        if (df < 0) {
            df += d_e;
            d_e += 2;
            d_se += 2;
        } else {
            df += d_se;
            d_e += 2;
            d_se += 4;
            cy--;
        }
        cx++;
    } while (cx <= cy);

    /* Inside */
    if (dx > 0 && dy > 0) {
        result |= boxRGBA(renderer, x1, y1 + rad + 1, x2, y2 - rad, r, g, b, a);
    }

    return (result);
}

/* ---- Box */
/*!
\brief Draw box (filled rectangle) with blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the box.
\param y1 Y coordinate of the first point (i.e. top right) of the box.
\param x2 X coordinate of the second point (i.e. bottom left) of the box.
\param y2 Y coordinate of the second point (i.e. bottom left) of the box.
\param color The color value of the box to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int boxColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return boxRGBA(renderer, x1, y1, x2, y2, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw box (filled rectangle) with blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point (i.e. top right) of the box.
\param y1 Y coordinate of the first point (i.e. top right) of the box.
\param x2 X coordinate of the second point (i.e. bottom left) of the box.
\param y2 Y coordinate of the second point (i.e. bottom left) of the box.
\param r The red value of the box to draw.
\param g The green value of the box to draw.
\param b The blue value of the box to draw.
\param a The alpha value of the box to draw.

\returns Returns 0 on success, -1 on failure.
*/
int boxRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int result;
    Sint16 tmp;
    SDL_Rect rect;

    /*
    * Test for special cases of straight lines or single point
    */
    if (x1 == x2) {
        if (y1 == y2) {
            return (pixelRGBA(renderer, x1, y1, r, g, b, a));
        } else {
            return (vlineRGBA(renderer, x1, y1, y2, r, g, b, a));
        }
    } else {
        if (y1 == y2) {
            return (hlineRGBA(renderer, x1, x2, y1, r, g, b, a));
        }
    }

    /*
    * Swap x1, x2 if required
    */
    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    /*
    * Swap y1, y2 if required
    */
    if (y1 > y2) {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    /*
    * Create destination rect
    */
    rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1 + 1;
    rect.h = y2 - y1 + 1;

    /*
    * Draw
    */
    result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);
    result |= SDL_RenderFillRect(renderer, &rect);
    return result;
}

/* ----- Line */
/*!
\brief Draw line with alpha blending using the currently set color.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point of the line.
\param y1 Y coordinate of the first point of the line.
\param x2 X coordinate of the second point of the line.
\param y2 Y coordinate of the second point of the line.

\returns Returns 0 on success, -1 on failure.
*/
int line(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2) {
    /*
    * Draw
    */
    return SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

/*!
\brief Draw line with alpha blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point of the line.
\param y1 Y coordinate of the first point of the line.
\param x2 X coordinate of the second point of the line.
\param y2 Y coordinate of the seond point of the line.
\param color The color value of the line to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int lineColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return lineRGBA(renderer, x1, y1, x2, y2, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw line with alpha blending.

\param renderer The renderer to draw on.
\param x1 X coordinate of the first point of the line.
\param y1 Y coordinate of the first point of the line.
\param x2 X coordinate of the second point of the line.
\param y2 Y coordinate of the second point of the line.
\param r The red value of the line to draw.
\param g The green value of the line to draw.
\param b The blue value of the line to draw.
\param a The alpha value of the line to draw.

\returns Returns 0 on success, -1 on failure.
*/
int lineRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    /*
    * Draw
    */
    int result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);
    result |= SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    return result;
}

/* ----- Circle */
/*!
\brief Draw circle with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the center of the circle.
\param y Y coordinate of the center of the circle.
\param rad Radius in pixels of the circle.
\param color The color value of the circle to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int circleColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return ellipseRGBA(renderer, x, y, rad, rad, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw circle with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the center of the circle.
\param y Y coordinate of the center of the circle.
\param rad Radius in pixels of the circle.
\param r The red value of the circle to draw.
\param g The green value of the circle to draw.
\param b The blue value of the circle to draw.
\param a The alpha value of the circle to draw.

\returns Returns 0 on success, -1 on failure.
*/
int circleRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    return ellipseRGBA(renderer, x, y, rad, rad, r, g, b, a);
}

/* ----- Arc */
/*!
\brief Arc with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the center of the arc.
\param y Y coordinate of the center of the arc.
\param rad Radius in pixels of the arc.
\param start Starting radius in degrees of the arc. 0 degrees is down, increasing counterclockwise.
\param end Ending radius in degrees of the arc. 0 degrees is down, increasing counterclockwise.
\param color The color value of the arc to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int arcColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return arcRGBA(renderer, x, y, rad, start, end, c[0], c[1], c[2], c[3]);
}

/*!
\brief Arc with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the center of the arc.
\param y Y coordinate of the center of the arc.
\param rad Radius in pixels of the arc.
\param start Starting radius in degrees of the arc. 0 degrees is down, increasing counterclockwise.
\param end Ending radius in degrees of the arc. 0 degrees is down, increasing counterclockwise.
\param r The red value of the arc to draw.
\param g The green value of the arc to draw.
\param b The blue value of the arc to draw.
\param a The alpha value of the arc to draw.

\returns Returns 0 on success, -1 on failure.
*/
/* TODO: rewrite algorithm; arc endpoints are not always drawn */
int arcRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int result;
    Sint16 cx = 0;
    Sint16 cy = rad;
    Sint16 df = 1 - rad;
    Sint16 d_e = 3;
    Sint16 d_se = -2 * rad + 5;
    Sint16 xpcx, xmcx, xpcy, xmcy;
    Sint16 ypcy, ymcy, ypcx, ymcx;
    Uint8 drawoct;
    int startoct, endoct, oct, stopval_start = 0, stopval_end = 0;
    double dstart, dend, temp = 0.;

    /*
    * Sanity check radius
    */
    if (rad < 0) {
        return (-1);
    }

    /*
    * Special case for rad=0 - draw a point
    */
    if (rad == 0) {
        return (pixelRGBA(renderer, x, y, r, g, b, a));
    }

    // Octant labelling
    //      
    //  \ 5 | 6 /
    //   \  |  /
    //  4 \ | / 7
    //     \|/
    //------+------ +x
    //     /|\
	//  3 / | \ 0
    //   /  |  \
	//  / 2 | 1 \
	//      +y

    // Initially reset bitmask to 0x00000000
    // the set whether or not to keep drawing a given octant.
    // For example: 0x00111100 means we're drawing in octants 2-5
    drawoct = 0;

    /*
    * Fixup angles
    */
    start %= 360;
    end %= 360;
    // 0 <= start & end < 360; note that sometimes start > end - if so, arc goes back through 0.
    while (start < 0) start += 360;
    while (end < 0) end += 360;
    start %= 360;
    end %= 360;

    // now, we find which octants we're drawing in.
    startoct = start / 45;
    endoct = end / 45;
    oct = startoct - 1; // we increment as first step in loop

    // stopval_start, stopval_end; 
    // what values of cx to stop at.
    do {
        oct = (oct + 1) % 8;

        if (oct == startoct) {
            // need to compute stopval_start for this octant.  Look at picture above if this is unclear
            dstart = (double)start;
            switch (oct) {
                case 0:
                case 3:
                    temp = sin(dstart * M_PI / 180.);
                    break;
                case 1:
                case 6:
                    temp = cos(dstart * M_PI / 180.);
                    break;
                case 2:
                case 5:
                    temp = -cos(dstart * M_PI / 180.);
                    break;
                case 4:
                case 7:
                    temp = -sin(dstart * M_PI / 180.);
                    break;
            }
            temp *= rad;
            stopval_start = (int)temp; // always round down

            // This isn't arbitrary, but requires graph paper to explain well.
            // The basic idea is that we're always changing drawoct after we draw, so we
            // stop immediately after we render the last sensible pixel at x = ((int)temp).

            // and whether to draw in this octant initially
            if (oct % 2) drawoct |= (1 << oct); // this is basically like saying drawoct[oct] = true, if drawoct were a bool array
            else		 drawoct &= 255 - (1 << oct); // this is basically like saying drawoct[oct] = false
        }
        if (oct == endoct) {
            // need to compute stopval_end for this octant
            dend = (double)end;
            switch (oct) {
                case 0:
                case 3:
                    temp = sin(dend * M_PI / 180);
                    break;
                case 1:
                case 6:
                    temp = cos(dend * M_PI / 180);
                    break;
                case 2:
                case 5:
                    temp = -cos(dend * M_PI / 180);
                    break;
                case 4:
                case 7:
                    temp = -sin(dend * M_PI / 180);
                    break;
            }
            temp *= rad;
            stopval_end = (int)temp;

            // and whether to draw in this octant initially
            if (startoct == endoct) {
                // note:      we start drawing, stop, then start again in this case
                // otherwise: we only draw in this octant, so initialize it to false, it will get set back to true
                if (start > end) {
                    // unfortunately, if we're in the same octant and need to draw over the whole circle, 
                    // we need to set the rest to true, because the while loop will end at the bottom.
                    drawoct = 255;
                } else {
                    drawoct &= 255 - (1 << oct);
                }
            } else if (oct % 2) drawoct &= 255 - (1 << oct);
            else			  drawoct |= (1 << oct);
        } else if (oct != startoct) { // already verified that it's != endoct
            drawoct |= (1 << oct); // draw this entire segment
        }
    } while (oct != endoct);

    // so now we have what octants to draw and when to draw them. all that's left is the actual raster code.

    /*
    * Set color
    */
    result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);

    /*
    * Draw arc
    */
    do {
        ypcy = y + cy;
        ymcy = y - cy;
        if (cx > 0) {
            xpcx = x + cx;
            xmcx = x - cx;

            // always check if we're drawing a certain octant before adding a pixel to that octant.
            if (drawoct & 4)  result |= pixel(renderer, xmcx, ypcy);
            if (drawoct & 2)  result |= pixel(renderer, xpcx, ypcy);
            if (drawoct & 32) result |= pixel(renderer, xmcx, ymcy);
            if (drawoct & 64) result |= pixel(renderer, xpcx, ymcy);
        } else {
            if (drawoct & 96) result |= pixel(renderer, x, ymcy);
            if (drawoct & 6)  result |= pixel(renderer, x, ypcy);
        }

        xpcy = x + cy;
        xmcy = x - cy;
        if (cx > 0 && cx != cy) {
            ypcx = y + cx;
            ymcx = y - cx;
            if (drawoct & 8)   result |= pixel(renderer, xmcy, ypcx);
            if (drawoct & 1)   result |= pixel(renderer, xpcy, ypcx);
            if (drawoct & 16)  result |= pixel(renderer, xmcy, ymcx);
            if (drawoct & 128) result |= pixel(renderer, xpcy, ymcx);
        } else if (cx == 0) {
            if (drawoct & 24)  result |= pixel(renderer, xmcy, y);
            if (drawoct & 129) result |= pixel(renderer, xpcy, y);
        }

        /*
        * Update whether we're drawing an octant
        */
        if (stopval_start == cx) {
            // works like an on-off switch.  
            // This is just in case start & end are in the same octant.
            if (drawoct & (1 << startoct)) drawoct &= 255 - (1 << startoct);
            else						   drawoct |= (1 << startoct);
        }
        if (stopval_end == cx) {
            if (drawoct & (1 << endoct)) drawoct &= 255 - (1 << endoct);
            else						 drawoct |= (1 << endoct);
        }

        /*
        * Update pixels
        */
        if (df < 0) {
            df += d_e;
            d_e += 2;
            d_se += 2;
        } else {
            df += d_se;
            d_e += 2;
            d_se += 4;
            cy--;
        }
        cx++;
    } while (cx <= cy);

    return (result);
}

/* ----- Filled Circle */
/*!
\brief Draw filled circle with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the center of the filled circle.
\param y Y coordinate of the center of the filled circle.
\param rad Radius in pixels of the filled circle.
\param color The color value of the filled circle to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int filledCircleColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return filledEllipseRGBA(renderer, x, y, rad, rad, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw filled circle with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the center of the filled circle.
\param y Y coordinate of the center of the filled circle.
\param rad Radius in pixels of the filled circle.
\param r The red value of the filled circle to draw.
\param g The green value of the filled circle to draw.
\param b The blue value of the filled circle to draw.
\param a The alpha value of the filled circle to draw.

\returns Returns 0 on success, -1 on failure.
*/
int filledCircleRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    // jim
    rad--;

    int result;
    Sint16 cx = 0;
    Sint16 cy = rad;
    Sint16 ocx = (Sint16)0xffff;
    Sint16 ocy = (Sint16)0xffff;
    Sint16 df = 1 - rad;
    Sint16 d_e = 3;
    Sint16 d_se = -2 * rad + 5;
    Sint16 xpcx, xmcx, xpcy, xmcy;
    Sint16 ypcy, ymcy, ypcx, ymcx;

    /*
    * Sanity check radius
    */
    if (rad < 0) {
        return (-1);
    }

    /*
    * Special case for rad=0 - draw a point
    */
    if (rad == 0) {
        return (pixelRGBA(renderer, x, y, r, g, b, a));
    }

    /*
    * Set color
    */
    result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);

    /*
    * Draw
    */
    do {
        xpcx = x + cx;
        xmcx = x - cx;
        xpcy = x + cy;
        xmcy = x - cy;
        if (ocy != cy) {
            if (cy > 0) {
                ypcy = y + cy;
                ymcy = y - cy;
                result |= hline(renderer, xmcx, xpcx, ypcy);
                result |= hline(renderer, xmcx, xpcx, ymcy);
            } else {
                result |= hline(renderer, xmcx, xpcx, y);
            }
            ocy = cy;
        }
        if (ocx != cx) {
            if (cx != cy) {
                if (cx > 0) {
                    ypcx = y + cx;
                    ymcx = y - cx;
                    result |= hline(renderer, xmcy, xpcy, ymcx);
                    result |= hline(renderer, xmcy, xpcy, ypcx);
                } else {
                    result |= hline(renderer, xmcy, xpcy, y);
                }
            }
            ocx = cx;
        }

        /*
        * Update
        */
        if (df < 0) {
            df += d_e;
            d_e += 2;
            d_se += 2;
        } else {
            df += d_se;
            d_e += 2;
            d_se += 4;
            cy--;
        }
        cx++;
    } while (cx <= cy);

    return (result);
}

/* ----- Ellipse */
/*!
\brief Draw ellipse with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the center of the ellipse.
\param y Y coordinate of the center of the ellipse.
\param rx Horizontal radius in pixels of the ellipse.
\param ry Vertical radius in pixels of the ellipse.
\param color The color value of the ellipse to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int ellipseColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return ellipseRGBA(renderer, x, y, rx, ry, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw ellipse with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the center of the ellipse.
\param y Y coordinate of the center of the ellipse.
\param rx Horizontal radius in pixels of the ellipse.
\param ry Vertical radius in pixels of the ellipse.
\param r The red value of the ellipse to draw.
\param g The green value of the ellipse to draw.
\param b The blue value of the ellipse to draw.
\param a The alpha value of the ellipse to draw.

\returns Returns 0 on success, -1 on failure.
*/
int ellipseRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int result;
    int ix, iy;
    int h, i, j, k;
    int oh, oi, oj, ok;
    int xmh, xph, ypk, ymk;
    int xmi, xpi, ymj, ypj;
    int xmj, xpj, ymi, ypi;
    int xmk, xpk, ymh, yph;

    /*
    * Sanity check radii
    */
    if ((rx < 0) || (ry < 0)) {
        return (-1);
    }

    /*
    * Special case for rx=0 - draw a vline
    */
    if (rx == 0) {
        return (vlineRGBA(renderer, x, y - ry, y + ry, r, g, b, a));
    }
    /*
    * Special case for ry=0 - draw a hline
    */
    if (ry == 0) {
        return (hlineRGBA(renderer, x - rx, x + rx, y, r, g, b, a));
    }

    /*
    * Set color
    */
    result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);

    /*
    * Init vars
    */
    oh = oi = oj = ok = 0xFFFF;

    /*
    * Draw
    */
    if (rx > ry) {
        ix = 0;
        iy = rx * 64;

        do {
            h = (ix + 32) >> 6;
            i = (iy + 32) >> 6;
            j = (h * ry) / rx;
            k = (i * ry) / rx;

            if (((ok != k) && (oj != k)) || ((oj != j) && (ok != j)) || (k != j)) {
                xph = x + h;
                xmh = x - h;
                if (k > 0) {
                    ypk = y + k;
                    ymk = y - k;
                    result |= pixel(renderer, xmh, ypk);
                    result |= pixel(renderer, xph, ypk);
                    result |= pixel(renderer, xmh, ymk);
                    result |= pixel(renderer, xph, ymk);
                } else {
                    result |= pixel(renderer, xmh, y);
                    result |= pixel(renderer, xph, y);
                }
                ok = k;
                xpi = x + i;
                xmi = x - i;
                if (j > 0) {
                    ypj = y + j;
                    ymj = y - j;
                    result |= pixel(renderer, xmi, ypj);
                    result |= pixel(renderer, xpi, ypj);
                    result |= pixel(renderer, xmi, ymj);
                    result |= pixel(renderer, xpi, ymj);
                } else {
                    result |= pixel(renderer, xmi, y);
                    result |= pixel(renderer, xpi, y);
                }
                oj = j;
            }

            ix = ix + iy / rx;
            iy = iy - ix / rx;

        } while (i > h);
    } else {
        ix = 0;
        iy = ry * 64;

        do {
            h = (ix + 32) >> 6;
            i = (iy + 32) >> 6;
            j = (h * rx) / ry;
            k = (i * rx) / ry;

            if (((oi != i) && (oh != i)) || ((oh != h) && (oi != h) && (i != h))) {
                xmj = x - j;
                xpj = x + j;
                if (i > 0) {
                    ypi = y + i;
                    ymi = y - i;
                    result |= pixel(renderer, xmj, ypi);
                    result |= pixel(renderer, xpj, ypi);
                    result |= pixel(renderer, xmj, ymi);
                    result |= pixel(renderer, xpj, ymi);
                } else {
                    result |= pixel(renderer, xmj, y);
                    result |= pixel(renderer, xpj, y);
                }
                oi = i;
                xmk = x - k;
                xpk = x + k;
                if (h > 0) {
                    yph = y + h;
                    ymh = y - h;
                    result |= pixel(renderer, xmk, yph);
                    result |= pixel(renderer, xpk, yph);
                    result |= pixel(renderer, xmk, ymh);
                    result |= pixel(renderer, xpk, ymh);
                } else {
                    result |= pixel(renderer, xmk, y);
                    result |= pixel(renderer, xpk, y);
                }
                oh = h;
            }

            ix = ix + iy / ry;
            iy = iy - ix / ry;

        } while (i > h);
    }

    return (result);
}

/* ---- Filled Ellipse */
/*!
\brief Draw filled ellipse with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the center of the filled ellipse.
\param y Y coordinate of the center of the filled ellipse.
\param rx Horizontal radius in pixels of the filled ellipse.
\param ry Vertical radius in pixels of the filled ellipse.
\param color The color value of the filled ellipse to draw (0xRRGGBBAA).

\returns Returns 0 on success, -1 on failure.
*/
int filledEllipseColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color) {
    Uint8 *c = (Uint8 *)&color;
    return filledEllipseRGBA(renderer, x, y, rx, ry, c[0], c[1], c[2], c[3]);
}

/*!
\brief Draw filled ellipse with blending.

\param renderer The renderer to draw on.
\param x X coordinate of the center of the filled ellipse.
\param y Y coordinate of the center of the filled ellipse.
\param rx Horizontal radius in pixels of the filled ellipse.
\param ry Vertical radius in pixels of the filled ellipse.
\param r The red value of the filled ellipse to draw.
\param g The green value of the filled ellipse to draw.
\param b The blue value of the filled ellipse to draw.
\param a The alpha value of the filled ellipse to draw.

\returns Returns 0 on success, -1 on failure.
*/
int filledEllipseRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int result;
    int ix, iy;
    int h, i, j, k;
    int oh, oi, oj, ok;
    int xmh, xph;
    int xmi, xpi;
    int xmj, xpj;
    int xmk, xpk;

    /*
    * Sanity check radii
    */
    if ((rx < 0) || (ry < 0)) {
        return (-1);
    }

    /*
    * Special case for rx=0 - draw a vline
    */
    if (rx == 0) {
        return (vlineRGBA(renderer, x, y - ry, y + ry, r, g, b, a));
    }
    /*
    * Special case for ry=0 - draw a hline
    */
    if (ry == 0) {
        return (hlineRGBA(renderer, x - rx, x + rx, y, r, g, b, a));
    }

    /*
    * Set color
    */
    result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);

    /*
    * Init vars
    */
    oh = oi = oj = ok = 0xFFFF;

    /*
    * Draw
    */
    if (rx > ry) {
        ix = 0;
        iy = rx * 64;

        do {
            h = (ix + 32) >> 6;
            i = (iy + 32) >> 6;
            j = (h * ry) / rx;
            k = (i * ry) / rx;

            if ((ok != k) && (oj != k)) {
                xph = x + h;
                xmh = x - h;
                if (k > 0) {
                    result |= hline(renderer, xmh, xph, y + k);
                    result |= hline(renderer, xmh, xph, y - k);
                } else {
                    result |= hline(renderer, xmh, xph, y);
                }
                ok = k;
            }
            if ((oj != j) && (ok != j) && (k != j)) {
                xmi = x - i;
                xpi = x + i;
                if (j > 0) {
                    result |= hline(renderer, xmi, xpi, y + j);
                    result |= hline(renderer, xmi, xpi, y - j);
                } else {
                    result |= hline(renderer, xmi, xpi, y);
                }
                oj = j;
            }

            ix = ix + iy / rx;
            iy = iy - ix / rx;

        } while (i > h);
    } else {
        ix = 0;
        iy = ry * 64;

        do {
            h = (ix + 32) >> 6;
            i = (iy + 32) >> 6;
            j = (h * rx) / ry;
            k = (i * rx) / ry;

            if ((oi != i) && (oh != i)) {
                xmj = x - j;
                xpj = x + j;
                if (i > 0) {
                    result |= hline(renderer, xmj, xpj, y + i);
                    result |= hline(renderer, xmj, xpj, y - i);
                } else {
                    result |= hline(renderer, xmj, xpj, y);
                }
                oi = i;
            }
            if ((oh != h) && (oi != h) && (i != h)) {
                xmk = x - k;
                xpk = x + k;
                if (h > 0) {
                    result |= hline(renderer, xmk, xpk, y + h);
                    result |= hline(renderer, xmk, xpk, y - h);
                } else {
                    result |= hline(renderer, xmk, xpk, y);
                }
                oh = h;
            }

            ix = ix + iy / ry;
            iy = iy - ix / ry;

        } while (i > h);
    }

    return (result);
}

#endif

void gfx_utils_error(const char* fn, int result) {
    GUI_Log("gfx_utils fn: \"%s\" error: %x (%d).\n", fn, result, result);
}

void GUI_DrawRect(SDL_Renderer * renderer, int x, int y, int w, int h, SDL_Color col) {
    SDL_Rect rect = {x, y, w, h};

    int result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (col.a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    result |= SDL_RenderDrawRect(renderer, &rect);

    if (result) gfx_utils_error("GUI_DrawRect", result);
}

void GUI_FillRect(SDL_Renderer * renderer, int x, int y, int w, int h, SDL_Color col) {
    SDL_Rect rect = {x, y, w, h};

    int result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (col.a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    result |= SDL_RenderFillRect(renderer, &rect);

    if (result) gfx_utils_error("GUI_FillRect", result);
}

void GUI_FillRect(SDL_Renderer * renderer, int cx, int cy, int w, int h, int degree, SDL_Color col) {
    int result = 0;

    // Create the texture that will be manipulated
    SDL_Texture *textr = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

    if (!textr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture. Aborting!");
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Reason: %s", SDL_GetError());
        gfx_utils_error("GUI_FillRect(rotate) Failed to create texture.", 1);
        return;
    }

    // Here is setting the blend mode 
    SDL_SetTextureBlendMode(textr, SDL_BLENDMODE_BLEND);

    // Set textr as the target and paint it by BLACK & ALPHA = 0
    SDL_SetRenderTarget(renderer, textr);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // Set textr as the target and paint it by 'col'
    result |= SDL_SetRenderDrawBlendMode(renderer, (col.a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    SDL_Rect r = {0, 0, w, h};
    SDL_RenderFillRect(renderer, &r);

    // Restore the renderer's default target
    SDL_SetRenderTarget(renderer, NULL);

    SDL_RenderCopyEx(renderer, textr, NULL, GUI_MakeRect(cx - w / 2, cy - h / 2, w, h), degree, NULL, SDL_FLIP_NONE);
    SDL_DestroyTexture(textr);

    if (result) gfx_utils_error("GUI_FillRect(rotate)", result);
}

void GUI_DrawRoundRect(SDL_Renderer * renderer, int x, int y, int w, int h, int radius, SDL_Color col) {
    if ((radius * 2) > w)
        radius = w / 2;
    if ((radius * 2) > h)
        radius = h / 2;

    int result = roundedRectangleRGBA(renderer, x, y, x + w - 1, y + h - 1, radius, col.r, col.g, col.b, col.a);
    if (result) gfx_utils_error("GUI_DrawRoundRect", result);
}

void GUI_FillRoundRect(SDL_Renderer * renderer, int x, int y, int w, int h, int radius, SDL_Color col) {
    if ((radius * 2) > w)
        radius = w / 2;
    if ((radius * 2) > h)
        radius = h / 2;

    int result = roundedBoxRGBA(renderer, x, y, x + w - 1, y + h - 1, radius, col.r, col.g, col.b, col.a);

    if (result) gfx_utils_error("GUI_FillRoundRect", result);
}

void GUI_FillRoundRect(SDL_Renderer * renderer, int cx, int cy, int w, int h, int radius, int degree, SDL_Color col) {
    int result = 0;

    // Create the texture that will be manipulated
    SDL_Texture *textr = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

    if (!textr) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture. Aborting!");
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Reason: %s", SDL_GetError());
        gfx_utils_error("GUI_FillRoundRect(rotate) Failed to create texture.", 1);
        return;
    }

    // Here is setting the blend mode 
    SDL_SetTextureBlendMode(textr, SDL_BLENDMODE_BLEND);

    // Set textr as the target and paint it by BLACK & ALPHA = 0
    SDL_SetRenderTarget(renderer, textr);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // FillRoundRect
    // pom
    result = roundedBoxRGBA(renderer, 0, 0, w, h, radius, col.r, col.g, col.b, col.a);

    // Restore the renderer's default target
    SDL_SetRenderTarget(renderer, NULL);

    //SDL_Point center = {cx, cy};
    SDL_RenderCopyEx(renderer, textr, NULL, GUI_MakeRect(cx - w / 2, cy - h / 2, w, h), degree, NULL, SDL_FLIP_NONE);
    SDL_DestroyTexture(textr);

    if (result) gfx_utils_error("GUI_FillRoundRect(rotate)", result);
}
void GUI_DrawCircle(SDL_Renderer * renderer, int cx, int cy, int radius, SDL_Color col) {
#ifdef SW_RENDER_SURFACE
    GUI_DrawRoundRect(renderer, cx - radius, cy - radius, radius * 2, radius * 2, radius, col);
#else
    int result = circleRGBA(renderer, cx, cy, radius, col.r, col.g, col.b, col.a);
    if (result) gfx_utils_error("GUI_DrawCircle", result);
#endif
}

void GUI_FillCircle(SDL_Renderer * renderer, int cx, int cy, int radius, SDL_Color col) {
#ifdef SW_RENDER_SURFACE
    GUI_FillRoundRect(renderer, cx - radius, cy - radius, radius * 2, radius * 2, radius, col);
#else
    int result = filledCircleRGBA(renderer, cx, cy, radius, col.r, col.g, col.b, col.a);
    if (result) gfx_utils_error("GUI_FillCircle", result);
#endif
}

void GUI_DrawLine(SDL_Renderer * renderer, int x1, int y1, int x2, int y2, SDL_Color col) {
    int result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (col.a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    result |= SDL_RenderDrawLine(renderer, x1, y1, x2, y2);

    if (result) gfx_utils_error("GUI_DrawLine", result);
}

SDL_Texture* GUI_LoadTexture(const std::string &filename, SDL_Renderer *ren) {
    std::string imagePath;
    printf( "imagePath (fn): %s\n", filename.c_str() );
    if (filename[0] != '/')
        imagePath = GUI_GetResourcePath() + filename;
    else
        imagePath = "" + filename;

#if defined(_MSC_VER)
    std::replace(imagePath.begin(), imagePath.end(), '/', '\\');
#endif

    SDL_Texture* tex = IMG_LoadTexture(ren, imagePath.c_str());

    if (tex == NULL) {
        imagePath = GUI_GetCurrentPath() + "/data/" + filename;
        tex = IMG_LoadTexture(ren, imagePath.c_str());
        if( tex == NULL ) {
            printf( "Failed\n" );
        }
    }

    return tex;
}

// pom
SDL_Surface* GUI_LoadImageAsSurface(const std::string &filename) {
    std::string imagePath;

    if (filename[0] != '/' && filename[0] != '\\' && filename[1] != ':')
        imagePath = GUI_GetResourcePath() + filename;
    else
        imagePath = "" + filename;

    GUI_Log("LoadImageAsSurface:%s\n", imagePath.c_str());
    SDL_Surface *surface = IMG_Load(imagePath.c_str());

    if (surface == NULL) {
        GUI_Log("LoadImageAsSurface %s failed\n", filename.c_str());
        imagePath = GUI_GetCurrentPath() + "\\data\\" + filename;
        surface = IMG_Load(imagePath.c_str());
    }

    return surface;
}

SDL_Texture* GUI_LoadDITexture(const std::string &filename, SDL_Renderer *ren) {
    std::string imagePath;

    std::string fn = "" + filename;

    if (GUI_scale != 1) {
        size_t index = 0;

        while (true) {
            /* Locate the substring to replace. */
            index = fn.find(".png", index);
            if (index == std::string::npos) break;

            /* Make the replacement. */
            fn.replace(index, 4, "@2x.png");

            /* Advance index forward so the next iteration doesn't pick it up as well. */
            index += 7;
        }
    }

	if (filename[0] != '/') {
		if (filename.size() > 2 && filename[1] == ':') {
			imagePath = "" + fn;
		}
		else
			imagePath = GUI_GetResourcePath() + fn;
	}
    else
        imagePath = "" + fn;

#if defined(_MSC_VER)
    std::replace(imagePath.begin(), imagePath.end(), '/', '\\');
#endif

    //GUI_Log("IMG_LoadTexture imagePath: %s\n", imagePath.c_str());

    SDL_Texture *tex = IMG_LoadTexture(ren, imagePath.c_str());

    if (tex == NULL) {
        imagePath = GUI_GetCurrentPath() + "\\data\\" + filename;
        tex = IMG_LoadTexture(ren, imagePath.c_str());
        if (tex == NULL) {
            GUI_Log("Load image %s failed.", fn.c_str());
        }
    }

    return tex;
}

std::string GUI_GetCurrentPath() {
    char cCurrentPath[FILENAME_MAX];

    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
#if defined(_MSC_VER)
        return std::string();
#else
        return NULL;
#endif
    }

    return std::string(cCurrentPath);
}

/*
 * Get the resource path for resources located in res/subDir
 * It's assumed the project directory is structured like:
 * bin/
 *  the executable
 * res/
 *  Lesson1/
 *  Lesson2/
 *
 * Paths returned will be Project_Root/res/subDir
 */
std::string GUI_GetResourcePath(const std::string &subDir) {
#ifdef _WIN32
    const char PATH_SEP = '\\';
#else
    const char PATH_SEP = '/';
#endif
    static std::string baseRes;

    baseRes = "";
#if defined (__ANDROID__) || (__MACOSX__) || (__linux__)
    baseRes = baseRes + "data" + PATH_SEP;
#else

    if (baseRes.empty()) {
        char *basePath = SDL_GetBasePath();

        if (basePath) {
            baseRes = basePath;
            SDL_free(basePath);
        } else {
            std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
            return "";
        }

        //We replace the last bin/ with res/ to get the the resource path
        size_t pos = baseRes.rfind("bin");
        baseRes = baseRes.substr(0, pos) + "data" + PATH_SEP;
    }
#endif
    return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}

char _docDir[512];
const char *GUI_getDocumentDirectory() {
#if __IPHONEOS__
    strcpy(_docDir, __iOS_DOCUMENTS_FOLDER);
#elif __MACOSX__
    struct passwd *pwx = getpwuid(getuid());

    const char *homedir = pwx->pw_dir;

    strcpy(_docDir, homedir);
    strcat(_docDir, "/Documents" );

#elif __ANDROID__
    strcpy(_docDir, "/mnt/sdcard/Download");
#elif __EMSCRIPTEN__
    getcwd(_docDir, 512);
    sprintf(_docDir, "%sdata", _docDir);
#else
    getcwd(_docDir, 512);
#endif
    return _docDir;
}

std::string GUI_GetIP() {
    char buffer[32];
    buffer[0] = 0;

//    try {
        int sock = (int)socket(AF_INET, SOCK_DGRAM, 0);
        assert(sock != -1);

        const char* kGoogleDnsIp = "8.8.8.8";
        uint16_t kDnsPort = 53;
        struct sockaddr_in serv;
        memset(&serv, 0, sizeof(serv));
        serv.sin_family = AF_INET;
        serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
        serv.sin_port = htons(kDnsPort);

        int err = connect(sock, (const sockaddr*)&serv, sizeof(serv));

        if (err == -1) {
            GUI_Log("Return 0.0.0.0\n");
            return std::string("0.0.0.0");
        }

        sockaddr_in name;
        socklen_t namelen = sizeof(name);
        err = getsockname(sock, (sockaddr*)&name, &namelen);
        assert(err != -1);

        const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, (unsigned int)32);
        assert(p);

#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
//    } catch (...) {
//        GUI_Log("Exception\n");
//    }

    return std::string(buffer);
}

#ifndef _WIN32
#pragma mark string utility
#endif

// pom
vector<string> guiSplitString(const string& source, const string& delimiter, bool ignoreEmpty, bool trim) {
    vector<string> result;

    if (delimiter.empty()) {
        result.push_back(source);
        return result;
    }

    string::const_iterator substart = source.begin(), subend;

    while (true) {
        subend = search(substart, source.end(), delimiter.begin(), delimiter.end());
        string sub(substart, subend);

        if (trim) {
            sub = guiTrim(sub);
        }

        if (!ignoreEmpty || !sub.empty()) {
            result.push_back(sub);
        }

        if (subend == source.end()) {
            break;
        }

        substart = subend + delimiter.size();
    }

    return result;
}

string guiJoinString(const vector<string>& stringElements, const string& delimiter) {
    string str;

    if (stringElements.empty()) {
        return str;
    }

    auto numStrings = stringElements.size();
    string::size_type strSize = delimiter.size() * (numStrings - 1);

    for (const string &s : stringElements) {
        strSize += s.size();
    }

    str.reserve(strSize);
    str += stringElements[0];

    for (decltype(numStrings) i = 1; i < numStrings; ++i) {
        str += delimiter;
        str += stringElements[i];
    }

    return str;
}

void guiStringReplace(string& input, const string& searchStr, const string& replaceStr) {
    auto pos = input.find(searchStr);

    while (pos != std::string::npos) {
        input.replace(pos, searchStr.size(), replaceStr);
        pos += replaceStr.size();
        std::string nextfind(input.begin() + pos, input.end());
        auto nextpos = nextfind.find(searchStr);

        if (nextpos == std::string::npos) {
            break;
        }

        pos += nextpos;
    }
}

bool guiIsStringInString(const string& haystack, const string& needle) {
    return haystack.find(needle) != std::string::npos;
}

bool guiIsStringEndsWith(std::string const & value, std::string const & ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool guiIsStringStartsWith(std::string const & value, std::string const & starting) {
    if (starting.size() > value.size()) return false;
    return !value.compare(0, starting.length(), starting);
}

// Check how many times a string contains another string.
std::size_t guiStringTimesInString(const string& haystack, const string& needle) {
    const size_t step = needle.size();

    size_t count(0);
    size_t pos(0);

    while ((pos = haystack.find(needle, pos)) != std::string::npos) {
        pos += step;
        ++count;
    }

    return count;
}

// Check all characters in given string are digit
bool guiStringIsDigits(const std::string &str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}

//--------------------------------------------------
// helper method to get locale from name
#ifndef __ANDROID__
static std::locale guiGetLocale(const string & locale) {
    std::locale loc;

//    try {
        loc = std::locale(locale.c_str());
//    } catch (...) {
//        GUI_LogWarning("GUI_Utils") << "Couldn't create locale " << locale << " using default, " << loc.name();
//    }

    return loc;
}

string guiTrimFront(const string & src, const string & locale) {
    auto dst = src;
    std::locale loc = guiGetLocale(locale);
    dst.erase(dst.begin(), std::find_if_not(dst.begin(), dst.end(), [&](char & c) {return std::isspace<char>(c, loc); }));
    return dst;
}

bool isspace(char c) {
	if (c == 32 || c == '\n' || c == '\r' || c == '\t')
		return true;
	else
		return false;

}

string guiTrimBack(const string & src, const string & locale) {
    auto dst = src;
    std::locale loc = guiGetLocale(locale);
#ifdef __WIN32__
	dst.erase(std::find_if_not(dst.rbegin(), dst.rend(), [&](char & c) {return isspace(c); }).base(), dst.end());
#else
    dst.erase(std::find_if_not(dst.rbegin(), dst.rend(), [&](char & c) {return std::isspace<char>(c, loc); }).base(), dst.end());
#endif
    return dst;
}

string guiTrim(const string & src, const string & locale) {
    return guiTrimFront(guiTrimBack(src));
}

#else

string guiTrimBack(const string & src, const string & locale) {
    auto dst = src;
    dst.erase(std::find_if_not(dst.rbegin(), dst.rend(), [&](char & c) {return isspace(c); }).base(), dst.end());
    return dst;
}

string guiTrimFront(const string & src, const string & locale) {
    auto dst = src;
    dst.erase(dst.begin(), std::find_if_not(dst.begin(), dst.end(), [&](char & c) {return isspace(c); }));
    return dst;
}

string guiTrim(const string & src, const string & locale) {
    return guiTrimFront(guiTrimBack(src));
}
#endif

string guiVAArgsToString(const char * format, ...) {
    // variadic args to string:
    // http://www.codeproject.com/KB/string/string_format.aspx
    char aux_buffer[10000];
    string retStr("");

    if (nullptr != format) {
        va_list marker;

        // initialize variable arguments
        va_start(marker, format);

        // Get formatted string length adding one for nullptr
        size_t len = vsprintf(aux_buffer, format, marker) + 1;

        // Reset variable arguments
        va_end(marker);

        if (len > 0) {
            va_list args;

            // initialize variable arguments
            va_start(args, format);

            // Create a char vector to hold the formatted string.
            vector<char> buffer(len, '\0');
            vsprintf(&buffer[0], format, args);
            retStr = &buffer[0];
            va_end(args);
        }

    }

    return retStr;
}

string guiVAArgsToString(const char * format, va_list args) {
    // variadic args to string:
    // http://www.codeproject.com/KB/string/string_format.aspx
    char aux_buffer[10000];
    string retStr("");

    if (nullptr != format) {

        // Get formatted string length adding one for nullptr
        vsprintf(aux_buffer, format, args);
        retStr = aux_buffer;

    }

    return retStr;
}

template<>
string guiFromString(const string & value) {
    return value;
}

template<>
const char * guiFromString(const string & value) {
    return value.c_str();
}

int guiToInt(const string& intString) {
    int x = 0;
    istringstream cur(intString);
    cur >> x;
    return x;
}

int64_t guiToInt64(const string& intString) {
    int64_t x = 0;
    istringstream cur(intString);
    cur >> x;
    return x;
}

float guiToFloat(const string& floatString) {
    float x = 0;
    istringstream cur(floatString);
    cur >> x;
    return x;
}

double guiToDouble(const string& doubleString) {
    double x = 0;
    istringstream cur(doubleString);
    cur >> x;
    return x;
}

bool guiToBool(const string& boolString) {
    if (boolString == "true" || boolString == "TRUE") {
        return true;
    }

    if (boolString == "false" || boolString == "FALSE") {
        return false;
    }

    bool x = false;
    istringstream cur(boolString);
    cur >> x;
    return x;
}

template <>
string guiToHex(const string& value) {
    ostringstream out;
    // how many bytes are in the string
    std::size_t numBytes = value.size();
    for (std::size_t i = 0; i < numBytes; i++) {
        // print each byte as a 2-character wide hex value
        out << setfill('0') << setw(2) << hex << (unsigned int)((unsigned char)value[i]);
    }
    return out.str();
}

string guiToHex(const char* value) {
    return guiToHex((string)value);
}

int guiHexToInt(const string& intHexString) {
    int x = 0;
    istringstream cur(intHexString);
    cur >> hex >> x;
    return x;
}

char guiHexToChar(const string& charHexString) {
    int x = 0;
    istringstream cur(charHexString);
    cur >> hex >> x;
    return (char)x;
}

float guiHexToFloat(const string& floatHexString) {
    union intFloatUnion {
        int x;
        float f;
    } myUnion;

    myUnion.x = 0;
    istringstream cur(floatHexString);
    cur >> hex >> myUnion.x;
    return myUnion.f;
}

string guiHexToString(const string& stringHexString) {
    stringstream out;
    stringstream stream(stringHexString);
    // a hex string has two characters per byte
    std::size_t numBytes = stringHexString.size() / 2;

    for (std::size_t i = 0; i < numBytes; i++) {
        string curByte;
        // grab two characters from the hex string
        stream >> setw(2) >> curByte;
        // prepare to parse the two characters
        stringstream curByteStream(curByte);
        int cur = 0;
        // parse the two characters as a hex-encoded int
        curByteStream >> hex >> cur;
        // add the int as a char to our output stream
        out << (char)cur;
    }

    return out.str();
}

char guiToChar(const string& charString) {
    char x = '\0';
    istringstream cur(charString);
    cur >> x;
    return x;
}

template <>
string guiToBinary(const string& value) {
    stringstream out;
    std::size_t numBytes = value.size();

    for (std::size_t i = 0; i < numBytes; i++) {
        std::bitset<8> bitBuffer(value[i]);
        out << bitBuffer;
    }

    return out.str();
}

string guiToBinary(const char* value) {
    return guiToBinary((string)value);
}

int guiBinaryToInt(const string& value) {
    const int intSize = sizeof(int) * 8;
    std::bitset<intSize> binaryString(value);
    return (int)binaryString.to_ulong();
}

char guiBinaryToChar(const string& value) {
    const int charSize = sizeof(char) * 8;
    std::bitset<charSize> binaryString(value);
    return (char)binaryString.to_ulong();
}

float guiBinaryToFloat(const string& value) {
    const int floatSize = sizeof(float) * 8;
    std::bitset<floatSize> binaryString(value);

    union ulongFloatUnion {
        unsigned long result;
        float f;
    } myUFUnion;

    myUFUnion.result = binaryString.to_ulong();
    return myUFUnion.f;
}

string guiBinaryToString(const string& value) {
    ostringstream out;
    stringstream stream(value);
    std::bitset<8> byteString;
    std::size_t numBytes = value.size() / 8;

    for (std::size_t i = 0; i < numBytes; i++) {
        stream >> byteString;
        out << (char)byteString.to_ulong();
    }

    return out.str();
}
#ifndef __ANDROID__

string guiToLower(const string & src, const string & locale) {
    std::locale loc = guiGetLocale(locale);
    ostringstream out;

    for (std::string::size_type i = 0; i < src.length(); ++i) {
        out << std::tolower(src[i], loc);
    }

    return out.str();
}
string guiToUpper(const string & src, const string & locale) {
    std::locale loc = guiGetLocale(locale);
    ostringstream out;

    for (std::string::size_type i = 0; i < src.length(); ++i) {
        out << std::toupper(src[i], loc);
    }

    return out.str();
}
#else
string guiToLower(const string & src, const string & locale) {
    char cc[2];
    string out = "";

    for (std::string::size_type i = 0; i < src.length(); ++i) {
        cc[0] = std::tolower(src[i]);
        cc[1] = 0;
        out = out + cc;
    }

    return out;
}

string guiToUpper(const string & src, const string & locale) {
    char cc[2];
    string out = "";

    for (std::string::size_type i = 0; i < src.length(); ++i) {
        cc[0] = std::toupper(src[i]);
        cc[1] = 0;
        out = out + cc;
    }

    return out;
}
#endif

const vector<string> split_string(const string& s, const char& c) {
    string buff{""};
    vector<string> v;

    for (auto n : s) {
        if (n != c) buff += n; else
            if (n == c && buff != "") {
                v.push_back(buff); buff = "";
            }
    }

    if (buff != "") v.push_back(buff);

    return v;
}

SDL_Texture *GUI_LoadTextureFromBuffer(unsigned char *data, int size) {
    /* load the bmp */
    SDL_Surface *bmp_surface;

    SDL_RWops *rw = SDL_RWFromMem(data, size);
    bmp_surface = IMG_LoadPNG_RW(rw);

    if (bmp_surface == NULL) {
        printf("ERROR: could not load bmp\n");
        exit(1);
    }

    /* convert RGBA surface to texture */
    SDL_Texture *tx = SDL_CreateTextureFromSurface(GUI_renderer, bmp_surface);

    if (tx == 0) {
        printf("ERROR: could not create texture");
        exit(1);
    }

    SDL_SetTextureBlendMode(tx, SDL_BLENDMODE_BLEND);

    /* free up allocated memory */
    SDL_FreeSurface(bmp_surface);

    return tx;
}

static const std::string CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

std::string generateUUID(){
    std::string uuid = std::string(36,' ');
    int rnd = 0;
    int r = 0;
    
    uuid[8] = '-';
    uuid[13] = '-';
    uuid[18] = '-';
    uuid[23] = '-';
    
    uuid[14] = '4';
    
    for(int i=0;i<36;i++){
        if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) {
            if (rnd <= 0x02) {
                rnd = 0x2000000 + (std::rand() * 0x1000000) | 0;
            }
            rnd >>= 4;
            uuid[i] = CHARS[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
        }
    }
    return uuid;
}


GUI_Rect __viewport, __cliprect;

void GUI_SetViewport(GUI_Rect vp){
#if __USE_SKIA__
    SkCanvas * canvas = GUI_canvas;
    canvas->resetMatrix();
    canvas->translate(vp.x, vp.y);
    __viewport.set(vp.x, vp.y, vp.w, vp.h);
#else
    SDL_RenderSetViewport(GUI_renderer, &vp);
#endif
}

void GUI_GetViewport(GUI_Rect & vp){
#if __USE_SKIA__
    vp.set(__viewport.x, __viewport.y, __viewport.w, __viewport.h);
#else
    SDL_RenderGetViewport(GUI_renderer, &vp);
#endif
}

void GUI_SetClipRect(GUI_Rect rect){
#if __USE_SKIA__
    SkCanvas * canvas = GUI_canvas;
    canvas->restoreToCount(0);
    canvas->save();
    canvas->resetMatrix();
    canvas->translate(__viewport.x, __viewport.y);
    canvas->clipRect(rect.toSkRect(), SkClipOp::kIntersect);
    __cliprect.set(rect.x, rect.y, rect.w, rect.h);
#else
    SDL_RenderSetClipRect(GUI_renderer, &rect);
#endif
}

void GUI_GetClipRect(GUI_Rect & rect){
#if __USE_SKIA__
    rect.set(__cliprect.x, __cliprect.y, __cliprect.w, __cliprect.h);
#else
    SDL_RenderGetClipRect(GUI_renderer, &rect);
#endif
}

void GUI_Clear(SDL_Color clearColor){
#if __USE_SKIA__
    SkCanvas * canvas = GUI_canvas;
    canvas->clear(sk_color(clearColor));
#else
    SDL_SetRenderDrawColor(GUI_renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear(GUI_renderer);
#endif
}

void GUI_RenderPresent(){
#if __USE_SKIA__
    if(GUI_window && GUI_canvas){
        SkCanvas * canvas = GUI_canvas;
        canvas->flush();
        SDL_GL_SwapWindow(GUI_window);
    }
#else
    SDL_RenderPresent(GUI_renderer);
#endif
}
