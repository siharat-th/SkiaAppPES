#include <cassert>
#include <codecvt>
#include <iostream>
#include <locale>
#include <string>
#include "DecFont.h"
#include "DecIcon.h"

DecIcon::DecIcon( const uint16_t code ) {
    _textSize = 14;
    _tfIndex = FONT_ICON;

    if (code == 0) {
        _text = "";
    } else {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        _text = convert.to_bytes(code);
    }

    _width = contentWidth();
    _height = contentHeight();
}

DecIcon::~DecIcon() {
}

void DecIcon::setIcon( const uint16_t code ) {
    if (code == 0) {
        _text = "";
    } else {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        _text = convert.to_bytes(code);
    } 
    dirty();
}

DecIcon &Icon( const uint16_t code ) {
    DecIcon *w = new DecIcon( code );
    return *w;    
}