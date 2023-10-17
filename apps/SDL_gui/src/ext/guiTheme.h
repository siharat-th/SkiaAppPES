//
//  guiTheme.h
//  VG2EMB
//
//  Created by Siharat on 24/11/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef guiTheme_h
#define guiTheme_h

//#include <SDL.h>

#define defaultTitleColor   gui::defaultTheme.color.label
#define defaultIconColor    gui::defaultTheme.color.icon

namespace gui {
    
    typedef struct Colors_{
        SDL_Color normal, hover, push, downed;
    }Colors;
    
    class Theme{
    public:
        Theme(){}
        
        struct{
            
            // global gui background color (container)
            SDL_Color guiBackground = hex(0xF0F0F0);
            SDL_Color guiBorder = hex(0xdfdfdf);
            
            // general colors that are shared by all components
            SDL_Color label = hex(0x242424);
            SDL_Color icon = hex(0x4f4f4f);
            SDL_Color background = cClear;
            SDL_Color border = hex(0xa0a0a0);
            
            SDL_Color inputAreaBackground = hex(0xffffff);
            
            SDL_Color highlightLabel = hex(0xffffff);
            SDL_Color highlightFill = hex(0x2c80f6);
            SDL_Color highlightStroke = hex(0x6495ed);
            
            struct {
                SDL_Color background = hex(0xf4f4f4);
                Colors fill = {cClear, cClear, hex(0xd8d8d8), hex(0x707070)};
                Colors stroke = {cClear, hex(0x6B6B6B), hex(0x6B6B6B), hex(0x4f4f4f)};
                Colors image = {cWhite, cWhite, cWhite, cWhite};
                Colors glyph = {hex(0x4f4f4f), hex(0x4f4f4f), hex(0x4f4f4f), cWhite};
            } button;
            
            struct {
                SDL_Color text = hex(0x000000);
            }editText;
            
            struct {
                SDL_Color background = hex(0xffffff);
                SDL_Color bar = hex(0xc0c0c0);
                SDL_Color barOnMouseHover = hex(0x7d7d7d);
            }scroll;
            
            struct {
                SDL_Color shadow = sdl_color(0x70707080);
                SDL_Color background = hex(0xf0f0f0);
                SDL_Color border = hex(0xa0a0a0);
                SDL_Color label = hex(0x242424);
            }tooltip;
            
        }color;
        
        static SDL_Color hex(Uint32 rgb, Uint8 a=0xff) {
            SDL_Color sdl_c;
            sdl_c.r = rgb >> 16 & 0xff;
            sdl_c.g = rgb >> 8 & 0xff;
            sdl_c.b = rgb & 0xff;
            sdl_c.a = a;
            return sdl_c;
        }
        
        struct{
            
            // container
            float guiPadding = 10;
            float guiBorder = 1;
            float guiCorner = 5;
            
            // component padding
            float padding = 5.0f;
            float vMargin = 3.0f;
            float corner = 5;
            
            struct {
                float hPadding = 10;
                float vPadding = 0;
            }button;
            
        }layout;
    };
    
    const static Theme defaultTheme;
    
}

#endif /* guiTheme_h */
