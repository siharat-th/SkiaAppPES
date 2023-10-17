#ifndef __DEC_FONT_H__
#define __DEC_FONT_H__

#include <vector>
#include <string>
#include "include/core/SkTypeface.h"
#include "include/core/SkFont.h"

const int FONT_REGULAR = 0;
const int FONT_BOLD = 1;
const int FONT_ITALIC = 2;
const int FONT_BOLDITALIC = 3;
const int FONT_ICON = 4;
const int FONT_EMOJI = 5;
const int FONT_MAX = 6;

class DecFont {
public:
    DecFont();
    ~DecFont();

    DecFont &Regular( std::string fn );
    DecFont &Bold( std::string fn );
    DecFont &Italic( std::string fn );
    DecFont &BoldItalic( std::string fn );
    DecFont &Icon( std::string fn );
    DecFont &Emoji( std::string fn );

    void _Regular( sk_sp<SkData> data );
    void _Bold( sk_sp<SkData> data );
    void _Italic( sk_sp<SkData> data );
    void _BoldItalic( sk_sp<SkData> data );
    void _Icon( sk_sp<SkData> data );
    void _Emoji( sk_sp<SkData> data );

    sk_sp<SkTypeface> typeface( int n ) { return _typeface[n]; }

    sk_sp<SkTypeface> _typeface[6];

protected:

};

DecFont &Font();

#endif //__DEC_FONT_H__