#include <vector>
#include <string>
#include "tools/Resources.h"
#include "DecFont.h"

#include "DecUI.h"

DecFont &Font() {
    DecFont *_font = new DecFont();
    return *_font; 
}

DecFont::DecFont() {
    for( int i=0; i<FONT_MAX; i++ ) {
        _typeface[i] = nullptr;
    }
}

DecFont::~DecFont() {

}

void DecFont::_Regular( sk_sp<SkData> data ) {
    _typeface[FONT_REGULAR] = SkTypeface::MakeFromData( data );
}

void DecFont::_Bold( sk_sp<SkData> data ) {
    _typeface[FONT_BOLD] = SkTypeface::MakeFromData( data );
}

void DecFont::_Italic( sk_sp<SkData> data ) {
    _typeface[FONT_ITALIC] = SkTypeface::MakeFromData( data );
}

void DecFont::_BoldItalic( sk_sp<SkData> data ) {
    _typeface[FONT_BOLDITALIC] = SkTypeface::MakeFromData( data );
}

void DecFont::_Icon( sk_sp<SkData> data ) {
    _typeface[FONT_ICON] = SkTypeface::MakeFromData( data );
}

void DecFont::_Emoji( sk_sp<SkData> data ) {
    _typeface[FONT_EMOJI] = SkTypeface::MakeFromData( data );
}

DecFont &DecFont::Regular( std::string filename ) {  
#ifdef __EMSCRIPTEN__
    emscriptenDownloadAsset( this, filename, 
        [](struct emscripten_fetch_t *fetch) {
            sk_sp<SkData> data = SkData::MakeWithCopy( fetch->data, fetch->numBytes );
            DecFont *font = (DecFont *)fetch->userData;
            font->_Regular( data );
        },
        [](struct emscripten_fetch_t *fetch) {
            SkDebugf( "Download failed\n" );
        } 
    );
    //return true;
#else    
    sk_sp<SkData> data = GetResourceAsData(filename.c_str());
    _Regular( data );
#endif    
    return *this;
}

DecFont &DecFont::Bold( std::string filename ) {
#ifdef __EMSCRIPTEN__
    emscriptenDownloadAsset( this, filename, 
        [](struct emscripten_fetch_t *fetch) {
            sk_sp<SkData> data = SkData::MakeWithCopy( fetch->data, fetch->numBytes );
            DecFont *font = (DecFont *)fetch->userData;
            font->_Bold( data );
        },
        [](struct emscripten_fetch_t *fetch) {
            SkDebugf( "Download failed\n" );
        } 
    );
    //return true;
#else    
    sk_sp<SkData> data = GetResourceAsData(filename.c_str());
    _Bold( data );
#endif    
    return *this;
}

DecFont &DecFont::Italic( std::string filename ) {
#ifdef __EMSCRIPTEN__
    emscriptenDownloadAsset( this, filename, 
        [](struct emscripten_fetch_t *fetch) {
            sk_sp<SkData> data = SkData::MakeWithCopy( fetch->data, fetch->numBytes );
            DecFont *font = (DecFont *)fetch->userData;
            font->_Italic( data );
        },
        [](struct emscripten_fetch_t *fetch) {
            SkDebugf( "Download failed\n" );
        } 
    );
    //return true;
#else    
    sk_sp<SkData> data = GetResourceAsData(filename.c_str());
    _Italic( data );
#endif    
    return *this;
}

DecFont &DecFont::BoldItalic( std::string filename ) {
#ifdef __EMSCRIPTEN__
    emscriptenDownloadAsset( this, filename, 
        [](struct emscripten_fetch_t *fetch) {
            sk_sp<SkData> data = SkData::MakeWithCopy( fetch->data, fetch->numBytes );
            DecFont *font = (DecFont *)fetch->userData;
            font->_BoldItalic( data );
        },
        [](struct emscripten_fetch_t *fetch) {
            SkDebugf( "Download failed\n" );
        } 
    );
    //return true;
#else    
    sk_sp<SkData> data = GetResourceAsData(filename.c_str());
    _BoldItalic( data );
#endif    
    return *this;
}

DecFont &DecFont::Icon( std::string filename ) {
#ifdef __EMSCRIPTEN__
    emscriptenDownloadAsset( this, filename, 
        [](struct emscripten_fetch_t *fetch) {
            sk_sp<SkData> data = SkData::MakeWithCopy( fetch->data, fetch->numBytes );
            DecFont *font = (DecFont *)fetch->userData;
            font->_Icon( data );
        },
        [](struct emscripten_fetch_t *fetch) {
            SkDebugf( "Download failed\n" );
        } 
    );
    //return true;
#else    
    sk_sp<SkData> data = GetResourceAsData(filename.c_str());
    _Icon( data );
#endif    
    return *this;
}

DecFont &DecFont::Emoji( std::string filename ) {
#ifdef __EMSCRIPTEN__
    emscriptenDownloadAsset( this, filename, 
        [](struct emscripten_fetch_t *fetch) {
            sk_sp<SkData> data = SkData::MakeWithCopy( fetch->data, fetch->numBytes );
            DecFont *font = (DecFont *)fetch->userData;
            font->_Emoji( data );
        },
        [](struct emscripten_fetch_t *fetch) {
            SkDebugf( "Download failed\n" );
        } 
    );
    //return true;
#else    
    sk_sp<SkData> data = GetResourceAsData(filename.c_str());
    _Emoji( data );
#endif    
    return *this;
}