//
//  GUI_TextField.cpp
//  GUITextField
//
//  Created by SIHARAT THAMMAYA on 4/29/17.
//  Copyright © 2017 Jimmy Software Co., Ltd. All rights reserved.
//

#include "GUI_TextField.hpp"
#include "GUI_UTF8.hpp"
#include "GUI_UnicodeThai.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


GUI_TextField *GUI_TextField::create( GUI_View *parent, const char *title, int x, int y, int width, int height, const char* ttfFontPath, int font_ptsize) {
    GUI_TextField *edit = new GUI_TextField( parent, title, x, y, width, height, cBlack, GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL );
    edit->bgcol = cWhite;
    edit->setPadding( 5, 5, 5, 5 );
    edit->font.load(ttfFontPath, font_ptsize);
    edit->setText(title);
    return edit;
}

GUI_TextField *GUI_TextField::create( GUI_View *parent, const char *title, int x, int y, int width, int height, std::map<Uint16, FT_UInt> charMap,
                                     const char* ttfFontPath, int font_ptsize, bool createContour){
    GUI_TextField *edit = new GUI_TextField( parent, title, x, y, width, height, cBlack, GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL );
    edit->bgcol = cWhite;
    edit->setPadding( 5, 5, 5, 5 );
    edit->font.setCharMapIndex(charMap);
    edit->font.setCreateContour(true);
    edit->font.load(ttfFontPath, font_ptsize);
    edit->setText(title);
    return edit;
}


GUI_TextField::GUI_TextField( GUI_View *p, const char *t, int x, int y, int w, int h,
                           SDL_Color col, int align, int fnt):
GUI_Label( p, "", x, y, w, h, col, align, fnt ),
text_index(0),
font_size(fnt),
_type(TEXT_ANY),
drawMode(DRAW_MODE_SOLID)
{
    min_width = title_area.w;
    min_height = title_area.h;
    
    action_cmd = nullptr;
    bgcol = cWhite;
//    setBorder( 1 );
    setPadding( 5, 5, 5, 5 );
//    isFocus = false;
}

GUI_TextField::~GUI_TextField()
{
    
}

void GUI_TextField::draw()
{
    GUI_Label::draw();

    int x = padding[3]*GUI_scale;
    int y = padding[0]*GUI_scale;

    if(title.length()<=0){
        if(SDL_strlen(defaultText.c_str())){
            font.setColor(cGrey);
            font.drawString(defaultText, x, y);
        }
    }
    
    // draw caret
    long currentTime = SDL_GetTicks()/500;
    if( isFocus && (currentTime & 1) ) {
        int w, h;
        int text_row_index = 0;
        int t_index = text_index;
        for(auto line : lines){
            int len = strlen(line.c_str());
            if(t_index>len){
                text_row_index++;
                t_index-=len;
            }
            else{
                break;
            }
        }
        if(text_row_index>lines.size()-1){
            text_row_index = lines.size()-1;
            t_index = lines[lines.size()-1].length();
        }
        
        if(lines.size()>0){
            TTF_SizeUTF8( font.font, lines[text_row_index].substr(0, t_index).c_str(), &w, &h );
        }
        else{
            w = h = 0;
        }
        int x = padding[3]*GUI_scale + w;
        int y = padding[0]*GUI_scale + font.getLineHeight() * text_row_index;
        
        GUI_FillRect(x, y, 2*GUI_scale, font.getLineHeight(), cHightLightSelected);
    }
}

bool GUI_TextField::handleEvents(SDL_Event *ev) {
    SDL_Scancode scancode;      /* scancode of last key we pushed */
    
    switch (ev->type) {
        
        case SDL_MOUSEBUTTONDOWN: {
            if( this->isEnable() == false )
                return false;
            SDL_MouseButtonEvent e = ev->button;
            int x = (int)(e.x*GUI_mouseScale);
            int y = (int)(e.y*GUI_mouseScale);
            if( isFocus ) {
                if( !hitTest(x, y) ) {
                    isFocus = false;
                    return false;
                }
            }
            else {
                isFocus = true;
                if( GUI_mouseCapturedWindow && GUI_mouseCapturedWindow!=this)
                    GUI_mouseCapturedWindow->isFocus = false;
                GUI_mouseCapturedWindow = (GUI_TextField *)this;
                SDL_StartTextInput();
            }
            
            if( isFocus ) {
                int xx = padding[3]*GUI_scale +tw_area.x;
                int row = (y-tw_area.y-padding[0]*GUI_scale)/font.getLineHeight();
                int maxrow = lines.size()>=1 ? lines.size()-1 : 0;
                row = (int)guiClamp((float)row, 0, (float)maxrow);
                
                char cc[4];
                cc[3] = 0;
                
                int j=0;
                
                int begin = 0;
                if(row>0){
                    for(int i=0; i<row; i++){
                        begin+=strlen(lines[i].c_str());
                    }
                }
                unsigned long len = (unsigned long)title.length();
                for( int i=begin; i<(int)title.length(); i++ ) {
                    unsigned char c = title.at(i);
                    int ww, hh;
                    if( (c & 0x80) == 0 ) {
                        cc[j++] = c;
                    }
                    else if( (c & 0xc0) == 0xc0 ) {
                        cc[j++] = c;
                        c = title.at(i+j);
                        while( (c & 0xC0) == 0x80 ) {
                            cc[j++] = c;
                            if( i+j >= (int)len )
                                break;
                            c = title.at(i+j);
                        }
                    }
                    cc[j] = 0;
                    string substr = title.substr(begin, (i-begin)+j);
                    TTF_SizeUTF8( font.font, substr.c_str(), &ww, &hh );

                    if( (xx+ww) > x+4 ) {
                        text_index = i;
                        break;
                    }
                    else {
                        text_index = i+j;
                    }
                    i += j-1;
                    j = 0;
                }
                
                return true;
            }
            return false;
        }
            
        case SDL_MOUSEMOTION: {
            if( this->isEnable() == false )
                return false;
            SDL_MouseButtonEvent e = ev->button;
            int x = (int)(e.x*GUI_mouseScale);
            int y = (int)(e.y*GUI_mouseScale);
            if( isFocus ) {
                if( !hitTest(x, y) ) {
//                    isFocus = false;
                    return false;
                }
            }
#if defined(__IPHONEOS__) || defined(__ANDROID__)
            if( isFocus ) {
                int xx = padding[3]*GUI_scale +tw_area.x;
                int row = (y-tw_area.y-padding[0]*GUI_scale)/font.getLineHeight();
                int maxrow = lines.size()>=1 ? lines.size()-1 : 0;
                row = (int)guiClamp((float)row, 0, (float)maxrow);
                GUI_LogNotice("Click row") << row;
                
                char cc[4];
                cc[3] = 0;
                
                int j=0;
                
                int begin = 0;
                if(row>0){
                    for(int i=0; i<row; i++){
                        begin+=strlen(lines[i].c_str());
                    }
                }
                unsigned long len = (unsigned long)title.length();
                for( int i=begin; i<(int)title.length(); i++ ) {
                    unsigned char c = title.at(i);
                    int ww, hh;
                    if( (c & 0x80) == 0 ) {
                        cc[j++] = c;
                    }
                    else if( (c & 0xc0) == 0xc0 ) {
                        cc[j++] = c;
                        c = title.at(i+j);
                        while( (c & 0xC0) == 0x80 ) {
                            cc[j++] = c;
                            if( i+j >= (int)len )
                                break;
                            c = title.at(i+j);
                        }
                    }
                    cc[j] = 0;
                    string substr = title.substr(begin, (i-begin)+j);
                    TTF_SizeUTF8( font.font, substr.c_str(), &ww, &hh );
                    
                    if( (xx+ww) > x+4 ) {
                        text_index = i;
                        break;
                    }
                    else {
                        text_index = i+j;
                    }
                    i += j-1;
                    j = 0;
                }
                
                return true;
            }
#endif
            return false;
        }
            
        case SDL_MOUSEBUTTONUP: {
            if( this->isEnable() == false )
                return false;
            SDL_MouseButtonEvent e = ev->button;
            int x = (int)(e.x*GUI_mouseScale);
            int y = (int)(e.y*GUI_mouseScale);
            if( isFocus ) {
                if( !hitTest(x, y) ) {
                    isFocus = false;
                    GUI_mouseCapturedWindow = NULL;
                    SDL_StopTextInput();
                    return false;
                }
            }
            return false;
        }
        case SDL_KEYDOWN: {
   
            if( isFocus == false )
                return false;
            scancode = ev->key.keysym.scancode;
            if (scancode == SDL_SCANCODE_BACKSPACE) {
                int oi = text_index;
                while( text_index > 0 ) {
                    text_index--;
                    int c = (title.at(text_index) & 0xC0);
                    if( c != 0x80 ) {
                        break;
                    }
                }
                if( oi != text_index ) {
                    title = title.substr(0,text_index)+title.substr(oi);
                    updateSubLayout();
                    setText(title);
                }
            }
            return true;
        }
        case SDL_TEXTINPUT: {

            if( isFocus == false ){
//                GUI_LogNotice("Focus==false") << title;
                return false;
            }

            if(font.isExceedLimit()==false){
                if(_type==TEXT_ANY){
                    title = title.insert( text_index, std::string( ev->text.text ) );
                    text_index += (int)strlen(ev->text.text);
                }
                else if(_type==TEXT_DIGIT){
                    char *c = ev->text.text;
                    if(SDL_isdigit(c[0]) || c[0]=='.'){
                        title = title.insert( text_index, std::string( ev->text.text ) );
                        text_index += (int)strlen(ev->text.text);
                    }
                }
                
                updateSubLayout();
                setText(title);
                
                return true;
            }
            return false;
        }
        
        default:
            return GUI_Label::handleEvents(ev);
            break;
    }
    
    return true;
}


bool GUI_TextField::setText(std::string newText){
    bool hasChange = false;
    if(font.inputText!=newText){
        
        hasChange = true;
        title = newText;
        bool thai_language = false;
        
        for(auto c: GUI_UTF8Iterator(newText)){
            if(th_isthchar(c)){
                GUI_LogNotice("th_isthchar") << c;
                thai_language = true;
                break;
            }
        }
        
        if(thai_language){
            size_t n = th_brk_load_dict("thaidict.txt");
            if(n>0){
                vector<char> hints = th_brkpos_hints(newText);
                vector<char> breaks = th_brk_maximal_impl(newText, hints);
                std::string wordbreaked;
                int i=0;
                for(auto c: GUI_UTF8Iterator(newText)){
                    if(breaks[i++]==1)
                        wordbreaked.append(ofTextConverter::toUTF8(THAI_WBR_UNICHAR));
                    wordbreaked.append(ofTextConverter::toUTF8(c));
                }
                
//                bool pua = TTF_GlyphIsProvided(font.font, 0xF705u);
                
//                if(pua)
                    wordbreaked = th_glyph_adjust(wordbreaked);
                
//                auto adjusts = th_glyph_pos_asjust(wordbreaked);
                font.setInputText(wordbreaked);
//                font.setPosAdjusts(adjusts);
                hints.clear();
                breaks.clear();
            }
        }
        else{
            font.setInputText(newText);
        }
        
        int len = SDL_strlen(newText.c_str());
        if(text_index>len)
            text_index = len;
        
        int w = tw_area.w - (padding[1]+padding[3])*2*GUI_scale;
        int h = tw_area.h - (padding[0]+padding[2])*GUI_scale;
        lines.clear();
        lines = font.splitLine(font.inputText, w, h);
        
        int x =0, y=0;
        if(titleTexture==NULL)
            titleTexture = SDL_CreateTexture(GUI_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tw_area.w-(padding[1]+padding[3])*GUI_scale, h);
        SDL_SetRenderTarget(GUI_renderer, titleTexture);
        SDL_SetRenderDrawColor(GUI_renderer, 0, 0, 0, 0);
        SDL_RenderSetClipRect(GUI_renderer, NULL);
        SDL_RenderSetViewport(GUI_renderer, NULL);
        SDL_RenderClear(GUI_renderer);
        
        if(lines.size()){
            font.setColor(titleColor);
            for(auto line : lines){
                if(drawMode==DRAW_MODE_SOLID){
                    font.drawString(line, x, y);
                }
                else{
                    font.drawStringAsShapes(line, x, y);
                }
                y+=font.getLineHeight();
            }
        }
        else{
            if(SDL_strlen(defaultText.c_str())){
                font.setColor(cGrey);
                font.drawString(defaultText, x, y);
            }
        }
        SDL_SetRenderTarget(GUI_renderer, NULL);
        SDL_SetTextureBlendMode(titleTexture, SDL_BLENDMODE_BLEND);
        
        title_area.x = padding[3] * GUI_scale;
        title_area.y = padding[0] * GUI_scale;
        title_area.w = tw_area.w-(padding[1]+padding[3])*GUI_scale;
        title_area.h = h;
        
        
        if( action_cmd ) {
            action_cmd( this );
        }
    }
    return hasChange;
}

GUI_TextField::DrawMode GUI_TextField::getDrawingMode(){
    return drawMode;
}

std::string GUI_TextField::getText(){
    return font.inputText;
}

void GUI_TextField::setNumLines(int numLines){
    int h = font.getLineHeight()*numLines + (padding[0]+padding[2])*GUI_scale;
    tw_area.h = h;
    updateSubLayout();
}

void GUI_TextField::setDefaultText(std::string _defaultText){
    defaultText = _defaultText;
}

void GUI_TextField::setDrawingMode(DrawMode _drawMode){
    drawMode = _drawMode;
}
