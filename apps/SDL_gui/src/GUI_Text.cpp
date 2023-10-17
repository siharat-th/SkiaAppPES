//
// Created by Jimmy on 2/10/2016 AD.
//

#include "GUI_Text.h"
#include "GUI_Utils.h"
#include <assert.h>
#include "FontAwesome.h"
#if __USE_SKIA__
#include "include/core/SkSurface.h"
#include "include/core/SkPaint.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkPath.h"
#include "include/core/SkImage.h"
#include "tools/ResourceFactory.h"
#include "tools/Resources.h"
#include "skutil.h"
//SkFont GUI_def_skfont, GUI_title_skfont, GUI_micro_skfont;
SkFont GUI_skfonts[3];
SkFont GUI_fa_skfonts[3];
#endif

#if defined(WIN32)  
#define GUI_FONTPATH ".\\data\\DroidSans.ttf"
#else
#define GUI_FONTPATH "data/DroidSans.ttf"
#endif

#ifdef __ANDROID__
const int GUI_micro_font_size = 11;
const int GUI_nominal_font_size = 14;
const int GUI_title_font_size = 18;
#else
const int GUI_micro_font_size = 11;
const int GUI_nominal_font_size = 14;
const int GUI_title_font_size = 18;
#endif

TTF_Font *GUI_micro_font = NULL;
TTF_Font *GUI_def_font = NULL;
TTF_Font *GUI_title_font = NULL;

TTF_Font* __ttfFontAwesome[3] = {NULL, NULL, NULL};

// pom
#define MAX_FONT 3
TTF_Font *GUI_fonts[MAX_FONT] = {NULL, NULL, NULL};
const char* fontPath = GUI_FONTPATH;
int microFontSize = GUI_micro_font_size;
int nominalFontSize = GUI_nominal_font_size;
int titleFontSize = GUI_title_font_size;
bool guiFontCreated = false;
void GUI_ConfigFont(const char* _fontPath, int _microFontSize, int _nominalFontSize, int _titleFontSize) {
    if (guiFontCreated == false) {
        fontPath = _fontPath;

        if (_microFontSize != -1)
            microFontSize = _microFontSize;

        if (_nominalFontSize != -1)
            nominalFontSize = _nominalFontSize;

        if (_titleFontSize != -1)
            titleFontSize = _titleFontSize;
    }
}

bool GUI_CreateFontAwesome() {
    if (__ttfFontAwesome[0] == NULL) {
        if (!(__ttfFontAwesome[0] = TTF_OpenFont(kFont_awesome_path, (nominalFontSize + 2)*GUI_scale))) {
            printf("font-awesome %s not found\n", kFont_awesome_path);
            return false;
        }
    }

    if (__ttfFontAwesome[1] == NULL) {
#ifndef __ANDROID__
        if (!(__ttfFontAwesome[1] = TTF_OpenFont(kFont_awesome_path, (titleFontSize)*GUI_scale))) {
            printf("font-awesome %s not found\n", kFont_awesome_path);
            return false;
        }
#else
        __ttfFontAwesome[1] = __ttfFontAwesome[0];
#endif
    }
    
#if __USE_SKIA__
    auto tf = MakeResourceAsTypeface(kFont_awesome_path);
    GUI_fa_skfonts[0] = SkFont(tf);
    GUI_fa_skfonts[0].setSize((nominalFontSize + 2)*GUI_scale);
    #ifndef __ANDROID__
        GUI_fa_skfonts[1] = SkFont(tf);
        GUI_fa_skfonts[1].setSize((titleFontSize)*GUI_scale);
    #else
        GUI_fa_skfonts[1] = GUI_fa_skfonts[0];
    #endif
#endif

    return true;
}

bool GUI_CreateFont() {
//    char path[128];
//    auto p = SDL_GetBasePath();
//    sprintf(path, "%s%s", p, fontPath);
    
    if (!(GUI_title_font = TTF_OpenFont(fontPath, titleFontSize * GUI_scale))) { // fontpath from config.h
        GUI_Log("font-spec %s not found\n", fontPath);
        return false;
    }

    if (!(GUI_def_font = TTF_OpenFont(fontPath, nominalFontSize * GUI_scale))) {
        GUI_Log("font-spec %s not found\n", fontPath);
        return false;
    }

    //pom
    if (!(GUI_micro_font = TTF_OpenFont(fontPath, microFontSize * GUI_scale))) {
        GUI_Log("font-spec %s not found\n", fontPath);
        return false;
    }

    GUI_fonts[GUI_FONT_TITLE] = GUI_title_font;
    GUI_fonts[GUI_FONT_NORMAL] = GUI_def_font;
    GUI_fonts[GUI_FONT_MICRO] = GUI_micro_font;
    
#if __USE_SKIA__
    SetResourcePath(SDL_GetBasePath());
    auto r = GetResourcePath();
    cout << r.c_str() << endl;
    auto tf = MakeResourceAsTypeface(fontPath);
//    GUI_title_skfont = SkFont(tf);
//    GUI_title_skfont.setSize(titleFontSize * GUI_scale);
//    GUI_def_skfont = SkFont(tf);
//    GUI_def_skfont.setSize(nominalFontSize * GUI_scale);
//    GUI_micro_skfont = SkFont(tf);
//    GUI_micro_skfont.setSize(microFontSize * GUI_scale);
    
    GUI_skfonts[GUI_FONT_TITLE] = SkFont(tf, titleFontSize * GUI_scale);
    GUI_skfonts[GUI_FONT_NORMAL] = SkFont(tf, nominalFontSize * GUI_scale);
    GUI_skfonts[GUI_FONT_MICRO] = SkFont(tf, microFontSize * GUI_scale);
#endif
    
    guiFontCreated = true;

    if (!GUI_CreateFontAwesome())
        return false;

    return true;
}

SDL_Texture * GUI_createTextTexture(const char *text, int fontSize, SDL_Rect* rect) {
    assert(fontSize >= 0 && fontSize <= 2);

#ifdef __ANDROID__
    if (fontSize == 1)
        fontSize = 0;
#endif

    SDL_Surface *titleSurface;

    if (text && text[0]) {
        titleSurface = TTF_RenderUTF8_Blended(GUI_fonts[fontSize], text, cWhite);
    } else {
        titleSurface = TTF_RenderUTF8_Blended(GUI_fonts[fontSize], " ", cWhite);
    }

    SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(GUI_renderer, titleSurface);

    if (titleTexture == NULL) {
        GUI_Log("CreateTitle Texture failed.\n");
    }

    // pom
    if (rect != NULL) {
        rect->x = rect->y = 0;
        rect->w = titleSurface->w;
        rect->h = titleSurface->h;
    }

    SDL_FreeSurface(titleSurface);
    return titleTexture;
}

SDL_Texture* GUI_createTextureFormUnicode(Uint16 unicode, int fontSize, SDL_Rect* rect) {
    assert(fontSize >= 0 && fontSize <= 1);

    if (__ttfFontAwesome[fontSize]) {
        SDL_Surface* surf = TTF_RenderGlyph_Blended(__ttfFontAwesome[fontSize], unicode, cWhite);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(GUI_renderer, surf);

        if (rect != NULL) {
            rect->x = rect->y = 0;
            rect->w = surf->w;
            rect->h = surf->h;
        }

        SDL_FreeSurface(surf);
        return tex;
    }

    return NULL;
}

SDL_Texture* GUI_createTextureFromUnicode(Uint16 unicode, int fontSize, SDL_Rect* rect) {
    return GUI_createTextureFormUnicode(unicode, fontSize, rect);
}


#if __USE_SKIA__

static void sk_release_direct_surface_storage(void* pixels, void* context){
    if (pixels == context) {
        SkDebugf("expected release context\n");
    }
    sk_free(pixels);
}

static sk_sp<SkSurface> MakeRasterDirectReleaseProc(int w, int h){
    SkImageInfo imageInfo = SkImageInfo::Make(w, h, SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kUnpremul_SkAlphaType);
    size_t pixelLen = w * h * 4; // // it's 8888, so 4 bytes per pixel
    void* pixelPtr = sk_malloc_throw(pixelLen); //sk_malloc_canfail(pixelLen);
    if(pixelPtr){
        sk_sp<SkSurface> surface(SkSurface::MakeRasterDirectReleaseProc(imageInfo, pixelPtr, w*4,
                                                                        sk_release_direct_surface_storage, pixelPtr));
        return surface;
    }
    return nullptr;
}

sk_sp<SkImage> GUI_createTextImage(const char *text, int fontSize, SDL_Rect* rect){
    assert(fontSize >= 0 && fontSize <= 2);

#ifdef __ANDROID__
    if (fontSize == 1)
        fontSize = 0;
#endif
    
    SkRect bounds;
    GUI_skfonts[fontSize].measureText(text, strlen(text), SkTextEncoding::kUTF8, &bounds);
//    bounds.setWH(((bounds.width()+31)/32)*32, bounds.height());
    if(rect!=NULL){
        rect->x = bounds.x();
        rect->y = bounds.y();
        rect->w = bounds.width();
        rect->h = bounds.height();
    }
    
    auto surface = MakeRasterDirectReleaseProc(bounds.width()>2?bounds.width():2,
                                               bounds.height()>2?bounds.height():2);
    if(!surface){
        SkDebugf("cannot create surface\n");
        return nullptr;
    }
    auto canvas = surface->getCanvas();
    canvas->translate(-bounds.x(), -bounds.y());
    SkPaint p;
    p.setStyle(SkPaint::kFill_Style);
    p.setColor(SK_ColorWHITE);
    canvas->clear(sk_color(cClear));
    canvas->drawString(text, 0, 0, GUI_skfonts[fontSize], p);
    canvas->flush();
    return surface->makeImageSnapshot();
}

sk_sp<SkImage> GUI_createImageFromUnicode(Uint16 unicode, int fontSize, SDL_Rect* rect){
    assert(fontSize >= 0 && fontSize <= 1);
    SkUnichar c = unicode;
    uint16_t g = GUI_fa_skfonts[fontSize].unicharToGlyph(c);
    SkPath path;
    GUI_fa_skfonts[fontSize].getPath(g, &path);
    SkRect bounds = path.getBounds();
    if(rect!=NULL){
        rect->x = bounds.x();
        rect->y = bounds.y();
        rect->w = bounds.width();
        rect->h = bounds.height();
    }
    auto surface = MakeRasterDirectReleaseProc(bounds.width(), bounds.height());
    auto canvas = surface->getCanvas();
    canvas->translate(-bounds.x(), -bounds.y());
    SkPaint p;
    canvas->clear(sk_color(cClear));
    canvas->drawPath(path, p);
    canvas->flush();
    return surface->makeImageSnapshot();
}
#endif
