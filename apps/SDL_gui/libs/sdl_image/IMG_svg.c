/*
  SDL_image:  An example image loading library for use with SDL
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* This is an SVG image file loading framework, based on Nano SVG:
 * https://github.com/memononen/nanosvg
 */

#include "SDL_image.h"

#ifdef LOAD_SVG

/* Replace C runtime functions with SDL C runtime functions for building on Windows */
#define acosf   SDL_acosf
#define atan2f  SDL_atan2f
#define cosf    SDL_cosf
#define ceilf   SDL_ceilf
#define fabs    SDL_fabs
#define fabsf   SDL_fabsf
#define floorf  SDL_floorf
#define fmodf   SDL_fmodf
#define free    SDL_free
#define malloc  SDL_malloc
#undef memcpy
#define memcpy  SDL_memcpy
#undef memset
#define memset  SDL_memset
#define pow     SDL_pow
#define qsort   SDL_qsort
#define realloc SDL_realloc
#define sinf    SDL_sinf
#define sqrt    SDL_sqrt
#define sqrtf   SDL_sqrtf
#define sscanf  SDL_sscanf
#define strchr  SDL_strchr
#define strcmp  SDL_strcmp
#define strncmp SDL_strncmp
#undef strncpy
#define strncpy SDL_strlcpy
#define strlen  SDL_strlen
#define strstr  SDL_strstr
#define strtol  SDL_strtol
#define strtoll SDL_strtoll
#define tanf    SDL_tanf
#ifndef FLT_MAX
#define FLT_MAX     3.402823466e+38F
#endif
#undef HAVE_STDIO_H

#define NANOSVG_IMPLEMENTATION

#ifdef __EMSCRIPTEN__
#include <math.h>
#endif

#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

#if defined(_MSC_VER) || defined(__linux__) || defined(__EMSCRIPTEN__)

#if !defined(SDL_LoadFile_RW)

/* Load all the data from an SDL data stream */
void* _SDL_LoadFile_RW(SDL_RWops* src, size_t* datasize, int freesrc) {
    const int FILE_CHUNK_SIZE = 1024;
    Sint64 size;
    size_t size_read, size_total;
    void *data = NULL, *newdata;

    if (!src) {
        SDL_InvalidParamError("src");
        return NULL;
    }

    size = SDL_RWsize(src);
    if (size < 0) {
        size = FILE_CHUNK_SIZE;
    }
    data = SDL_malloc((size_t)(size + 1));

    size_total = 0;
    for (;;) {
        if ((((Sint64)size_total) + FILE_CHUNK_SIZE) > size) {
            size = (size_total + FILE_CHUNK_SIZE);
            newdata = SDL_realloc(data, (size_t)(size + 1));
            if (!newdata) {
                SDL_free(data);
                data = NULL;
                SDL_OutOfMemory();
                goto done;
            }
            data = newdata;
        }

        size_read = SDL_RWread(src, (char*)data + size_total, 1, (size_t)(size - size_total));
        if (size_read == 0) {
            break;
        }
        size_total += size_read;
    }

    if (datasize) {
        *datasize = size_total;
    }
    ((char*)data)[size_total] = '\0';

done:
    if (freesrc && src) {
        SDL_RWclose(src);
    }
    return data;
}

// not use
//void* _SDL_LoadFile(const char* file, size_t* datasize) {
//    return _SDL_LoadFile_RW(SDL_RWFromFile(file, "rb"), datasize, 1);
//}

#else
// Not MSVC and linux
//SDL_LoadFile_RW an existing
#endif

#endif



/* See if an image is contained in a data source */
int IMG_isSVG(SDL_RWops *src)
{
    Sint64 start;
    int is_SVG;
    char magic[4096];
    size_t magic_len;

    if ( !src )
        return 0;
    start = SDL_RWtell(src);
    is_SVG = 0;
    magic_len = SDL_RWread(src, magic, 1, sizeof(magic) - 1);
    magic[magic_len] = '\0';
    if ( SDL_strstr(magic, "<svg") ) {
        is_SVG = 1;
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    return(is_SVG);
}

/* Load a SVG type image from an SDL datasource */
SDL_Surface *IMG_LoadSVG_RW(SDL_RWops *src)
{
    char *data;
    struct NSVGimage *image;
    struct NSVGrasterizer *rasterizer;
    SDL_Surface *surface = NULL;
    float scale = 1.0f;

// noom compile error
#if defined(_MSC_VER) || defined(__linux__) || defined(__EMSCRIPTEN__)

#if !defined(SDL_LoadFile_RW)
    // _SDL_LoadFile_RW an internal 
    data = (char*)_SDL_LoadFile_RW(src, NULL, SDL_FALSE);
#else
    // SDL_LoadFile_RW an existing
    data = (char*)SDL_LoadFile_RW(src, NULL, SDL_FALSE);
#endif

#else
    // Not MSVC and linux
    data = (char *)SDL_LoadFile_RW(src, NULL, SDL_FALSE);
#endif	
    if ( !data ) {
        return NULL;
    }

    /* For now just use default units of pixels at 96 DPI */
    image = nsvgParse(data, "px", 96.0f);
    SDL_free(data);
    if ( !image ) {
        IMG_SetError("Couldn't parse SVG image");
        return NULL;
    }

    rasterizer = nsvgCreateRasterizer();
    if ( !rasterizer ) {
        IMG_SetError("Couldn't create SVG rasterizer");
        nsvgDelete( image );
        return NULL;
    }

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                   (int)(image->width * scale),
                                   (int)(image->height * scale),
                                   32,
                                   0x000000FF,
                                   0x0000FF00,
                                   0x00FF0000,
                                   0xFF000000);
    if ( !surface ) {
        nsvgDeleteRasterizer( rasterizer );
        nsvgDelete( image );
        return NULL;
    }

    nsvgRasterize(rasterizer, image, 0.0f, 0.0f, scale, (unsigned char *)surface->pixels, surface->w, surface->h, surface->pitch);
    nsvgDeleteRasterizer( rasterizer );
    nsvgDelete( image );

    return surface;
}

#else

/* See if an image is contained in a data source */
int IMG_isSVG(SDL_RWops *src)
{
    return(0);
}

/* Load a SVG type image from an SDL datasource */
SDL_Surface *IMG_LoadSVG_RW(SDL_RWops *src)
{
    return(NULL);
}

#endif /* LOAD_SVG */
