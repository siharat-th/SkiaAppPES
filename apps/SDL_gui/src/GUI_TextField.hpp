//
//  GUI_TextField.hpp
//  GUITextField
//
//  Created by SIHARAT THAMMAYA on 4/29/17.
//  Copyright © 2017 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef GUI_TextField_hpp
#define GUI_TextField_hpp

#include <SDL.h>
#include <SDL_ttf.h>
#include "GUI_View.h"
#include "GUI_Label.h"
#include <string>
#include "GUI_FontAtlas.hpp"
#include "ft2build.h"

typedef unsigned int  FT_UInt;

class GUI_TextField : public GUI_Label {
    int font_size;
    
public:
    enum Type{
        TEXT_ANY,
        TEXT_DIGIT
    };
    
    enum DrawMode{
        DRAW_MODE_SOLID,
        DRAW_MODE_OUTLINE
    };
    
    static GUI_TextField *create( GUI_View *parent, const char *title, int x, int y, int width, int height, const char* ttfFontPath="data/DroidSans.ttf", int font_ptsize=GUI_nominal_font_size);
    
    static GUI_TextField *create( GUI_View *parent, const char *title, int x, int y, int width, int height, std::map<Uint16, FT_UInt> charMap, const char* ttfFontPath="data/DroidSans.ttf", int font_ptsize=GUI_nominal_font_size, bool createContour=false);
    
    
    ~GUI_TextField();
    
    virtual bool handleEvents( SDL_Event *ev );
    virtual void draw();
    
    inline void setType(GUI_TextField::Type type){
        _type = type;
    }
    
    bool setText(std::string newText);
    std::string getText();
    
    void setNumLines(int numLines);
    
    void setDefaultText(std::string defaultText);
    
    std::function<void(GUI_TextField *)> action_cmd;
    
    void setDrawingMode(DrawMode drawMode);
    DrawMode getDrawingMode();
    
    
private:
    GUI_TextField( GUI_View *parent, const char *title, int x=0, int y=0, int width=0, int height=0,
                  SDL_Color color=cBlack, int align=GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, int fontSize=1);
    
    Type _type;
    int text_index;
    
    GUI_FontAtlas font;
    std::vector<std::string> lines;
    std::string defaultText;
    DrawMode drawMode;
};

#endif /* GUI_TextField_hpp */
