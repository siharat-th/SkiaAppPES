//
// Created by Jimmy on 2/10/2016 AD.
//

#ifndef GUI_LABEL_GUI_TEXT_H
#define GUI_LABEL_GUI_TEXT_H

#include <SDL.h>
#include <SDL_ttf.h>
#if __USE_SKIA__
#include "include/core/SkImage.h"
#endif

extern TTF_Font *GUI_def_font, *GUI_title_font, *GUI_micro_font;
extern TTF_Font *GUI_fonts[];

#if __USE_SKIA__
#include "include/core/SkFont.h"
extern SkFont GUI_skfonts[];
#endif


extern const int GUI_micro_font_size;
extern const int GUI_nominal_font_size;
extern const int GUI_title_font_size;

// pom - config font before GUI_CreateFont()
void GUI_ConfigFont(const char* fontPath, int microFontSize = -1, int nominalFontSize = -1, int titleFontSize = -1);
bool GUI_CreateFont();

#define GUI_FONT_NORMAL 0
#define GUI_FONT_TITLE  1
#define GUI_FONT_MICRO  2 // pom

SDL_Texture * GUI_createTextTexture(const char *text, int fontSize = 0, SDL_Rect* rect = NULL);
SDL_Texture* GUI_createTextureFormUnicode(Uint16 unicode, int fontSize = 0, SDL_Rect* rect = NULL);
// pom: fixed typo
SDL_Texture* GUI_createTextureFromUnicode(Uint16 unicode, int fontSize = 0, SDL_Rect* rect = NULL);

#if __USE_SKIA__
sk_sp<SkImage> GUI_createTextImage(const char *text, int fontSize = 0, SDL_Rect* rect = NULL);
sk_sp<SkImage> GUI_createImageFromUnicode(Uint16 unicode, int fontSize = 0, SDL_Rect* rect = NULL);
#endif

#endif //GUI_LABEL_GUI_TEXT_H
