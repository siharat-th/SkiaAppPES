//
// Created by Jimmy on 2/10/2016 AD.
//

#ifndef GUI_VIEW_GUI_UTILS_H
#define GUI_VIEW_GUI_UTILS_H

#include <string>
#include "GUI_Log.h"
#if __USE_SKIA__
#include "include/core/SkColor.h"
#include "include/core/SkPoint.h"
#include "include/core/SkRect.h"
#endif

#ifdef _WIN32
#include <locale> // std::isspace and std::locate
#include <bitset> // std::bitset
#endif

#if defined __linux__
#include <locale> // std::isspace and std::locate
#include <bitset> // std::bitset
#endif

#ifdef _RPi
#include <locale> // std::isspace and std::locate
#include <bitset> // std::bitset
#endif

#ifdef __ANDROID__
#include <locale> // std::isspace and std::locate
#include <bitset> // std::bitset
#endif
#include <string>
#include <cstdlib>

extern SDL_Window *GUI_window;
extern SDL_Renderer *GUI_renderer;

extern int GUI_physicalWindowWidth;
extern int GUI_physicalWindowHeight;

extern int GUI_windowWidth;
extern int GUI_windowHeight;

extern const Uint32 GUI_EventID;
extern const Uint32 GUI_EventPaint;
extern const Uint32 GUI_EventInitDialog;

extern const Uint32 GUI_EventUser;

extern int GUI_scale;
extern float GUI_mouseScale;

struct GUI_Point {
    short x, y;
    GUI_Point();
    GUI_Point(short x, short y);
    void set(short x, short y);
    bool operator==(GUI_Point b);
    bool operator!=(GUI_Point b);
};

struct GUI_Rect :SDL_Rect {
    GUI_Rect();
    GUI_Rect(Sint16 x, Sint16 y, Uint16 dx, Uint16 dy);
    void set(Sint16 x, Sint16 y, Uint16 dx, Uint16 dy);
    
#if __USE_SKIA__
    SkRect toSkRect();
#endif
    
    GUI_Rect getIntersection(const GUI_Rect& rect) const{
        float x0 = std::max(x,rect.x);
        float x1 = std::min(x+w,rect.x+rect.w);

        float w = x1 - x0;
        if(w < 0.0f) return GUI_Rect(0,0,0,0); // short circuit if needed

        float y0 = std::max(y,rect.y);
        float y1 = std::min(y+h,rect.y+rect.h);

         float h = y1 - y0;
         if(h < 0.0f) return GUI_Rect(0,0,0,0);  // short circuit if needed

         return GUI_Rect(x0,y0,w,h);
    }

    
};
GUI_Rect *GUI_MakeRect(int x, int y, int w, int h);

extern SDL_Color cClear, cNop, cWhite, cBlack, cGrey, cRed, cGreen, cBlue, cCyan, cMagenta, cYellow, cDarkGrey, cLightGrey, cCornFlower, cHightLightSelected, cTextSelected;

SDL_Texture *GUI_LoadTextureFromBuffer(unsigned char *data, int size);

void GUI_Log(const char * format, ...);

//#define _NOOM_
#ifdef _NOOM_
/* gfx-utils */
/* Graphics drawing primitives and other support functions, gfx-utils supports SDL2 using rendering functions. */
/* Note: all ___Color routines expect the color to be in format 0xRRGGBBAA */

/* Pixel */
int pixelColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Uint32 color);
int pixelRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Horizontal line */
int hlineColor(SDL_Renderer * renderer, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color);
int hlineRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Vertical line */
int vlineColor(SDL_Renderer * renderer, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color);
int vlineRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y1, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Rectangle */
int rectangleColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int rectangleRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Rounded-Corner Rectangle */
int roundedRectangleColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color);
int roundedRectangleRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Filled rectangle (Box) */
int boxColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int boxRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Rounded-Corner Filled rectangle (Box) */
int roundedBoxColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color);
int roundedBoxRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Line */
int lineColor(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int lineRGBA(SDL_Renderer * renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Circle */
int circleColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Uint32 color);
int circleRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Arc */
int arcColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint32 color);
int arcRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Filled Circle */
int filledCircleColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 r, Uint32 color);
int filledCircleRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Ellipse */
int ellipseColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color);
int ellipseRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* Filled Ellipse */
int filledEllipseColor(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color);
int filledEllipseRGBA(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
#endif // endif _NOOM_

void GUI_DrawRect(SDL_Renderer * renderer, int x, int y, int w, int h, SDL_Color col);
void GUI_FillRect(SDL_Renderer * renderer, int x, int y, int w, int h, SDL_Color col);
void GUI_FillRect(SDL_Renderer * renderer, int cx, int cy, int w, int h, int degree, SDL_Color col);

void GUI_DrawRoundRect(SDL_Renderer * renderer, int x, int y, int w, int h, int radius, SDL_Color col);
void GUI_FillRoundRect(SDL_Renderer * renderer, int x, int y, int w, int h, int radius, SDL_Color col);
void GUI_FillRoundRect(SDL_Renderer * renderer, int cx, int cy, int w, int h, int radius, int degree, SDL_Color col);

void GUI_DrawCircle(SDL_Renderer * renderer, int cx, int cy, int radius, SDL_Color col);
void GUI_FillCircle(SDL_Renderer * renderer, int cx, int cy, int radius, SDL_Color col);

void GUI_DrawLine(SDL_Renderer * renderer, int x1, int y1, int x2, int y2, SDL_Color col);

void GUI_DrawRect(int x, int y, int w, int h, SDL_Color col);
void GUI_FillRect(int x, int y, int w, int h, SDL_Color col);
void GUI_FillRect(int cx, int cy, int w, int h, int degree, SDL_Color col);

void GUI_DrawRoundRect(int x, int y, int w, int h, int radius, SDL_Color col);
void GUI_FillRoundRect(int x, int y, int w, int h, int radius, SDL_Color col);
void GUI_FillRoundRect(int cx, int cy, int w, int h, int radius, int degree, SDL_Color col);

// pom
void GUI_DrawRoundRect(int x, int y, int w, int h, int corners[4], SDL_Color col);
void GUI_FillRoundRect(int x, int y, int w, int h, int radius[4], SDL_Color col);

void GUI_DrawCircle(int cx, int cy, int radius, SDL_Color col);
void GUI_FillCircle(int cx, int cy, int radius, SDL_Color col);

void GUI_DrawLine(int x1, int y1, int x2, int y2, SDL_Color col);

std::string GUI_GetIP();

SDL_Color sdl_color(Uint32 c);
#if __USE_SKIA__
SkColor sk_color(SDL_Color c);
#endif

std::string GUI_GetResourcePath(const std::string &subDir = "");
std::string GUI_GetCurrentPath();

SDL_Texture* GUI_LoadTexture(const std::string &filename, SDL_Renderer *ren);
SDL_Texture* GUI_LoadDITexture(const std::string &filename, SDL_Renderer *ren);

// pom
SDL_Surface* GUI_LoadImageAsSurface(const std::string &filename);

extern const char *GUI_getDocumentDirectory();
#ifndef _WIN32
#pragma mark utility macro
#endif

// pom
#ifndef CLAMP
#define CLAMP(val,min,max) ((val) < (min) ? (min) : ((val > max) ? (max) : (val)))
#endif

#ifndef _WIN32
#pragma mark string utility
#endif

// pom
vector<string> guiSplitString(const string& source, const string& delimiter, bool ignoreEmpty = false, bool trim = false);
string guiJoinString(const vector<string>& stringElements, const string& delimiter);
void guiStringReplace(string& input, const string& searchStr, const string& replaceStr);

bool guiIsStringInString(const string& haystack, const string& needle);
bool guiIsStringEndsWith(std::string const & value, std::string const & ending);
bool guiIsStringStartsWith(std::string const & value, std::string const & starting);

// Check how many times a string contains another string.
std::size_t guiStringTimesInString(const string& haystack, const string& needle);
// pom
// Check all characters in given string are digit
bool guiStringIsDigits(const std::string &str);

string guiTrimFront(const string & src, const string & locale = "");
string guiTrimBack(const string & src, const string & locale = "");
string guiTrim(const string & src, const string & locale = "");

string guiVAArgsToString(const char * format, ...);
string guiVAArgsToString(const char * format, va_list args);

// String conversion
template <class T>
string guiToString(const T& value) {
    ostringstream out;
    out << value;
    return out.str();
}

// Like sprintf "%4f" format, in this example precision=4
template <class T>
string guiToString(const T& value, int precision) {
    ostringstream out;
    out << fixed << setprecision(precision) << value;
    return out.str();
}

/// Like sprintf "% 4d" or "% 4f" format, in this example width=4, fill=' '
template <class T>
string guiToString(const T& value, int width, char fill) {
    ostringstream out;
    out << fixed << setfill(fill) << setw(width) << value;
    return out.str();
}

/// Like sprintf "%04.2d" or "%04.2f" format, in this example precision=2, width=4, fill='0'
template <class T>
string guiToString(const T& value, int precision, int width, char fill) {
    ostringstream out;
    out << fixed << setfill(fill) << setw(width) << setprecision(precision) << value;
    return out.str();
}

// Convert a vector of values to a comma-delimited string.
template<class T>
string guiToString(const vector<T>& values) {
    stringstream out;
    int n = values.size();
    out << "{";

    if (n > 0) {
        for (int i = 0; i < n - 1; i++) {
            out << values[i] << ", ";
        }
        out << values[n - 1];
    }

    out << "}";
    return out.str();
}

template<class T>
T guiFromString(const string & value) {
    T data;
    stringstream ss;
    ss << value;
    ss >> data;
    return data;
}

template<>
string guiFromString(const string & value);

template<>
const char * guiFromString(const string & value);

int guiToInt(const string& intString);
int64_t guiToInt64(const string& intString);

float guiToFloat(const string& floatString);
double guiToDouble(const string& doubleString);

bool guiToBool(const string& boolString);

template <class T>
string guiToHex(const T& value) {
    ostringstream out;
    // pretend that the value is a bunch of bytes
    unsigned char* valuePtr = (unsigned char*)&value;

    // the number of bytes is determined by the datatype
    int numBytes = sizeof(T);

    // the bytes are stored backwards (least significant first)
    for (int i = numBytes - 1; i >= 0; i--) {
        // print each byte out as a 2-character wide hex value
        out << setfill('0') << setw(2) << hex << (int)valuePtr[i];
    }

    return out.str();
}

template <>
string guiToHex(const string& value);

/// Converts a string (e.g., `"abc"`) to its equivalent hexadecimal
/// representation (e.g., `"616263"`).
string guiToHex(const char* value);

/// Converts a hexadecimal representation of an int (little-endian, 32-bit,
/// e.g., `"0xbadf00d"` or `"badf00d"`) to an actual `int`.
int guiHexToInt(const string& intHexString);

/// Converts a hexadecimal representation of an char (e.g., `"61"`) to an actual
/// `char` (e.g., `a`).
char guiHexToChar(const string& charHexString);

/// Converts a hexadecimal representation of an float (little-endian, 32-bit
/// IEEE 754, e.g., `"4060000000000000"`) to an actual float (e.g., `128.f`).
float guiHexToFloat(const string& floatHexString);

/// Converts a hexadecimal representation of an string (e.g.,
/// `"61626364656667"`) to an actual string (`"abcdefg"`).
string guiHexToString(const string& stringHexString);

char guiToChar(const string& charString);

/// The corresponding string of 1s and 0s corresponds to the way value is stored
/// in memory.
template <class T>
string guiToBinary(const T& value) {
    return std::bitset<8 * sizeof(T)>(*reinterpret_cast<const uint64_t*>(&value)).to_string();
}

template <>
string guiToBinary(const string& value);

string guiToBinary(const char* value);

/// Interprets a string consisting only of 1s and 0s as an int (little-endian,
/// 32-bit), and returns the corresponding int value.
int guiBinaryToInt(const string& value);

/// Interprets a string consisting only of 1s and 0s as an char and returns
/// the corresponding int value.
char guiBinaryToChar(const string& value);

/// Interprets a string consisting only of 1s and 0s as an a float
/// (little-endian, 32-bit IEEE 754), and returns the corresponding float value.
float guiBinaryToFloat(const string& value);

/// Interprets a string consisting only of 1s and 0s as 8-bit ASCII characters
/// and returns the corresponding string.
string guiBinaryToString(const string& value);

string guiToLower(const string & src, const string & locale = "");
string guiToUpper(const string & src, const string & locale = "");

const vector<string> split_string(const string& s, const char& c);

template<class T, class BoolFunction>
void guiRemove(vector<T>& values, BoolFunction shouldErase) {
    values.erase(remove_if(values.begin(), values.end(), shouldErase), values.end());
}

//*Adapted from https://gist.github.com/ne-sachirou/882192
//*std::rand() can be replaced with other algorithms as Xorshift for better perfomance
//*Random seed must be initialized by user




std::string generateUUID();

#ifdef WIN32
wstring s2ws(const std::string& str);
string ws2s(const std::wstring& wstr);
#endif

void GUI_SetViewport(GUI_Rect vp);
void GUI_SetClipRect(GUI_Rect rect);
void GUI_GetViewport(GUI_Rect &vp);
void GUI_GetClipRect(GUI_Rect &rect);
void GUI_Clear(SDL_Color clearColor);
void GUI_RenderPresent();

#endif //GUI_VIEW_GUI_UTILS_H
