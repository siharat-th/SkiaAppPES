//
//  GUI_ButtonFontAwesome.cpp
//  FontWorks
//
//  Created by SIHARAT THAMMAYA on 12/31/2559 BE.
//  Copyright © 2559 Jimmy Software Co., Ltd. All rights reserved.
//

#include "GUI_ButtonFontAwesome.hpp"

extern TTF_Font* __ttfFontAwesome[3];

GUI_ButtonFontAwesome* GUI_ButtonFontAwesome::create(GUI_View *parent, Uint16 unicode, const char *label,
                                                     int x, int y, int width, int height,SDL_Color color, int align, int fontSize)
{
    if(__ttfFontAwesome[0]==NULL){
        int status = 0;
        if(!TTF_WasInit()){
            status = TTF_Init();
        }
        
        if(status==0){
            __ttfFontAwesome[0] = TTF_OpenFont(kFont_awesome_path, kSmall_icon_size*GUI_scale);
            __ttfFontAwesome[1] = TTF_OpenFont(kFont_awesome_path, kNormal_icon_size*GUI_scale);
            __ttfFontAwesome[2] = TTF_OpenFont(kFont_awesome_path, 2*kNormal_icon_size*GUI_scale);
            if(!__ttfFontAwesome[0] || !__ttfFontAwesome[1] || !__ttfFontAwesome[2]){
                GUI_LogError("GUI_FontAwesome") << "TTF_OpenFont fail";
                return NULL;
            }
        }
        else{
            GUI_LogError("GUI_FontAwesome") << "TTF_Init fail";
            return NULL;
        }
    }
    
    SDL_Texture* texLabel = GUI_createTextTexture(label, fontSize);
    int lw, lh;
    SDL_QueryTexture(texLabel, NULL, NULL, &lw, &lh);
    SDL_DestroyTexture(texLabel);
    
    GUI_View* innerView = new GUI_View(parent, "", x, y, max(lw, width), height+lh);
    innerView->setLayout(GUI_LAYOUT_VERTICAL);
    innerView->dragable = false;
    innerView->bgcol = cClear;
    innerView->setBorder(0);
    innerView->setPadding(8, 0, 0, 0);

    GUI_ButtonFontAwesome* button = new GUI_ButtonFontAwesome(innerView, unicode, "", 0, 0, width, height, color, align, fontSize);
    button->setAlignment(GUI_ALIGN_CENTER);

    if(strlen(label)>1){
        GUI_Label* l = new GUI_Label(innerView, label, 0, 0, lw, lh, sdl_color(0x242424ff), GUI_ALIGN_CENTER, fontSize);
        l->setAlignment(GUI_ALIGN_CENTER);
    }

    innerView->setAlignment(GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER);

    return button;
}

GUI_ButtonFontAwesome::GUI_ButtonFontAwesome( GUI_View *parent, Uint16 unicode, const char *label, int x, int y, int width, int height,
               SDL_Color color, int align, int fontSize, std::function<void(GUI_ButtonFontAwesome*)> act_cmd)
: GUI_Label(parent, "", x, y, width, height, color, align, fontSize),
isActive(true)
{
    isDown = false;
    isFocus = false;

    action_cmd = act_cmd;

    borderColor = sdl_color(0xa0a0a0ff);
    bgColor = sdl_color(0xfafafaff);
    bgColor_hover = sdl_color(0xd8d8d8ff);
    labelColor = sdl_color(0x242424ff);
    labelColor_hover = sdl_color(0x242424ff);

    iconColor = color;
    iconColor_hover = color;
    GUI_View::iconColor = color;

    if(__ttfFontAwesome[0]){
        SDL_Surface* surf = TTF_RenderGlyph_Blended(__ttfFontAwesome[0], unicode, cWhite);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(GUI_renderer, surf);
        SDL_FreeSurface(surf);
        setIcon(tex);
    }

    setBorder(2);
    corner = 5;
}

GUI_ButtonFontAwesome::~GUI_ButtonFontAwesome(){
    //SDL_DestroyTexture( textureIcon );
    if (textureIcon) {
        if (__shared_ptr_textureIcon == false) {
            SDL_DestroyTexture(textureIcon);
            textureIcon = NULL;
        } else {
            // pom - shared_ptr do not destroy
            textureIcon = NULL;
        }
    }
}

void GUI_ButtonFontAwesome::setActive(bool active){
    isActive = active;
}

void GUI_ButtonFontAwesome::draw(){
    if( isActive && !_disable ){
        if( isDown ){
            bgcol = bgColor_hover;
            titleColor = labelColor_hover;
            GUI_View::iconColor = iconColor_hover;
        }
        else{
            bgcol = bgColor;
            titleColor = labelColor;
            GUI_View::iconColor = iconColor;
        }
    }
    else{
        bgcol = bgColor;
        titleColor = {0, 0, 0, 64};
        GUI_View::iconColor = {0, 0, 0, 64};
    }

    predraw();

    if(_border){
        if( corner ) {
            GUI_DrawRoundRect( 0, 0, tw_area.w, tw_area.h, corner * GUI_scale, borderColor );
        }
        else {
            GUI_DrawRect( 0, 0, tw_area.w, tw_area.h, borderColor );
        }
    }

    if( textureIcon ) {
        SDL_SetTextureColorMod( textureIcon, GUI_View::iconColor.r, GUI_View::iconColor.g, GUI_View::iconColor.b );
        SDL_SetTextureAlphaMod(textureIcon, GUI_View::iconColor.a);
        SDL_RenderCopy( GUI_renderer, textureIcon, NULL, &icon_area );
    }

    if( titleTexture ) {
        SDL_SetTextureColorMod(titleTexture, titleColor.r, titleColor.g, titleColor.b );
        SDL_SetTextureAlphaMod(titleTexture, titleColor.a);
        SDL_RenderCopy(GUI_renderer, titleTexture, NULL, &title_area);
    }
}

bool GUI_ButtonFontAwesome::handleEvents( SDL_Event *ev ){
    if( isVisible()==false)
        return false;

    if(!isActive && ev->type!=GUI_EventPaint)
        return false;
    
    if(_disable && ev->type!=GUI_EventPaint)
        return false;

    switch (ev->type) {
#if _RPi        
        case SDL_FINGERDOWN:
#endif
        case SDL_MOUSEBUTTONDOWN:
        {
            int ex, ey;
            if(ev->type==SDL_FINGERMOTION){ // touch finger x,y are Normalized in the range 0...1
                ex = (int)ev->tfinger.x;
                ey = (int)ev->tfinger.y;
            }
            else{
                ex = ev->button.x;
                ey = ev->button.y;
            }
            
            int x = (int)(ex*GUI_mouseScale);
            int y = (int)(ey*GUI_mouseScale);
            
            if(hitTest(x, y)){
                isDown = true;
                isFocus = true;
                GUI_mouseCapturedWindow = (GUI_View *)this;
                return true;
            }
        }
            
#if _RPi        
        case SDL_FINGERMOTION:
#endif
        case SDL_MOUSEMOTION:
        {
            if( isFocus ) {
                int ex, ey;
                if(ev->type==SDL_FINGERMOTION){ // touch finger x,y are Normalized in the range 0...1
                    ex = (int)ev->tfinger.x;
                    ey = (int)ev->tfinger.y;
                }
                else{
                    ex = ev->button.x;
                    ey = ev->button.y;
                }
                
                int x = (int)(ex*GUI_mouseScale);
                int y = (int)(ey*GUI_mouseScale);
                
                if( hitTest(x, y) ) {
                    isDown = true;
                    return true;
                }
                else {
                    isDown = false;
                }
            }
            return false;
        }
            
#if _RPi        
        case SDL_FINGERUP:
#endif
        case SDL_MOUSEBUTTONUP:
        {
            if( isFocus ) {
                int ex, ey;
                if(ev->type==SDL_FINGERMOTION){ // touch finger x,y are Normalized in the range 0...1
                    ex = (int)ev->tfinger.x;
                    ey = (int)ev->tfinger.y;
                }
                else{
                    ex = ev->button.x;
                    ey = ev->button.y;
                }
                
                GUI_mouseCapturedWindow = NULL;

                int x = (int)(ex*GUI_mouseScale);
                int y = (int)(ey*GUI_mouseScale);
                
                isDown = false;
                isFocus = false;
                if( hitTest(x, y) ) {
                    if( action_cmd ) {
                        action_cmd( this );
                        return true;
                    }
                }
            }
            return false;
        }
        default: {
            GUI_View::handleEvents( ev );
        }
    }
    return false;
}

SDL_Texture* GUI_ButtonFontAwesome::createTextureFormUnicode(Uint16 unicode, int* iWidth, int* iHeight, SDL_Color color, int sizeIndex){
    sizeIndex = (int)guiClamp((float)sizeIndex, 0.0f, 2.0f);
    if(__ttfFontAwesome[sizeIndex]==NULL){
        int status = 0;
        if(!TTF_WasInit()){
            status = TTF_Init();
        }
        
        if(status==0){
            __ttfFontAwesome[0] = TTF_OpenFont(kFont_awesome_path, kSmall_icon_size*GUI_scale);
            __ttfFontAwesome[1] = TTF_OpenFont(kFont_awesome_path, kNormal_icon_size*GUI_scale);
            __ttfFontAwesome[2] = TTF_OpenFont(kFont_awesome_path, kLarge_icon_size*GUI_scale);
            if(!__ttfFontAwesome[0] || !__ttfFontAwesome[1] || !__ttfFontAwesome[2]){
                GUI_LogError("GUI_FontAwesome") << "TTF_OpenFont fail";
                return NULL;
            }
        }
        else{
            GUI_LogError("GUI_FontAwesome") << "TTF_Init fail";
            return NULL;
        }
    }
    
    if(__ttfFontAwesome[sizeIndex]){
        SDL_Surface* surf = TTF_RenderGlyph_Blended(__ttfFontAwesome[sizeIndex], unicode, color);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(GUI_renderer, surf);
        if(iWidth!=NULL){
            *iWidth = surf->w;
        }
        if(iHeight!=NULL){
            *iHeight = surf->h;
        }
        SDL_FreeSurface(surf);
        return tex;
    }
    return NULL;
}

SDL_Surface* GUI_ButtonFontAwesome::createSurfaceFormUnicode(Uint16 unicode, int* iWidth, int* iHeight, SDL_Color color, int sizeIndex)
{
    sizeIndex = (int)guiClamp((float)sizeIndex, 0.0f, 2.0f);
    
    if(__ttfFontAwesome[sizeIndex]){
        SDL_Surface* surf = TTF_RenderGlyph_Blended(__ttfFontAwesome[sizeIndex], unicode, color);
        if(iWidth!=NULL){
            *iWidth = surf->w;
        }
        if(iHeight!=NULL){
            *iHeight = surf->h;
        }
        return surf;
    }
    return NULL;
}

void GUI_ButtonFontAwesome::Quit(){
    if(__ttfFontAwesome[0])
        TTF_CloseFont(__ttfFontAwesome[0]);
    
    if(__ttfFontAwesome[1])
        TTF_CloseFont(__ttfFontAwesome[1]);
    
    if(__ttfFontAwesome[2])
        TTF_CloseFont(__ttfFontAwesome[2]);
    
    
    __ttfFontAwesome[0] = 0;
    __ttfFontAwesome[1] = 0;
    __ttfFontAwesome[2] = 0;
}


