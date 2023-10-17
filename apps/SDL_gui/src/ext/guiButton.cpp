//
//  guiButton.cpp
//  VG2EMB
//
//  Created by Siharat on 24/11/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "guiButton.hpp"
#include "guiExt.h"
#include "guiRectangle.hpp"

namespace gui {
    
    // MARK: Button
    Button::Button(GUI_View *parent, const std::string & title, int x, int y, int w, int h,SDL_Color color, int align, int fontSize):
    GUI_Label(parent, title.c_str(), x, y, w, h, color, align, fontSize),
    isDown( false ),
    bActive(true),
    action_cmd(nullptr),
    action_hold_cmd(nullptr),
    action_hold_end_cmd(nullptr),
    touchTime(-1),
    holdTime(-1),
    toggle_state(false),
    corners{0,0,0,0}
    {
        isFocus = false;
        setBorder(1);
        bgcol = theme.color.button.background;
        iconColor = theme.color.icon;
        borderColor = theme.color.border;
        titleColor = theme.color.label;
        color_down = theme.color.button.fill.push;
        color_selected = theme.color.highlightFill; //theme.color.highlightStroke;
        
        setCorners((int)theme.layout.corner);
        
        float vv = theme.layout.button.vPadding;
        float hh = theme.layout.button.hPadding;
        setPadding((int)vv, (int)hh, (int)hh, (int)hh);
//        setPadding((int)vv, (int)hh, (int)vv, (int)hh);
    }
    
    Button::~Button(){
        touchTime = -1;
    }
    
    void Button::setCorners(int topleft, int topright, int bottomright, int bottomleft){
        corners[0] = topleft;
        corners[1] = topright;
        corners[2] = bottomright;
        corners[3] = bottomleft;
    }
    
    void Button::setCorners(int r){
        setCorners(r,r,r,r);
    }
    
    void Button::setActive(bool active){
        if(active!=bActive){
            bActive = active;
            
            if(titleTexture!=nullptr){
                unsigned char alphaMod = isActive() ? 255 : 64;
                titleColor.a = alphaMod;
                SDL_SetTextureAlphaMod(titleTexture, alphaMod);
            }
            if(textureIcon!=nullptr){
                unsigned char alphaMod = isActive() ? 255 : 64;
                iconColor.a = alphaMod;
                SDL_SetTextureAlphaMod(textureIcon, alphaMod);
            }
        }
    }
    
    void Button::setActive(bool active, SDL_Color bg, SDL_Color fg){
        bActive = active;
        bgcol  = bg;
        titleColor = fg;
        iconColor = fg;
        borderColor = fg;
    }
    
    bool Button::isActive(){
        return bActive;
    }
    
    void Button::clear(GUI_Rect *r)
    {
        guiRectangle rect(0.0f, 0.0f, (float)tw_area.w, (float)tw_area.h);
        if( bgcol.a != 0) {
            
            SDL_Color color = isDown ? color_down : bgcol;
            
            int c[4] = {
                corners[0]*GUI_scale,
                corners[1]*GUI_scale,
                corners[2]*GUI_scale,
                corners[3]*GUI_scale
                
            };
            GUI_FillRoundRect((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, c, color);
        }
    }
    
    void Button::draw() {
        if (!isVisible())
            return;
        
        predraw();
        
        if (textureIcon) {
            if (iconColor.a != 0) {
                SDL_SetTextureColorMod(textureIcon, iconColor.r, iconColor.g, iconColor.b);
            }
            
            SDL_RenderCopy(GUI_renderer, textureIcon, NULL, &icon_area);
        }
        
        if (display_cmd)
            display_cmd(this);
        
        if (_border > 0) {
            GUI_Rect r = GUI_Rect(_border, _border, tw_area.w - (2 * _border), tw_area.h - (2 * _border));
            SDL_SetRenderDrawBlendMode(GUI_renderer, SDL_BLENDMODE_BLEND);
            if(corners[0]==0 && corners[1]==0 && corners[2]==0 && corners[3]==0){
                if (corner) {
                    GUI_DrawRoundRect(0, 0, tw_area.w, tw_area.h, corner * GUI_scale, borderColor);
                } else {
                    GUI_DrawRect(0, 0, tw_area.w, tw_area.h, borderColor);
                }
            }
            else if(corners[0]==corners[1] && corners[1]==corners[2] && corners[2]==corners[3]){
                GUI_DrawRoundRect(0, 0, tw_area.w, tw_area.h, corners[0] * GUI_scale, borderColor);
            }
            else{
                int c[4] = {corners[0]*GUI_scale,
                    corners[1]*GUI_scale,
                    corners[2]*GUI_scale,
                    corners[3]*GUI_scale};
                GUI_DrawRoundRect(0, 0, tw_area.w, tw_area.h, c, borderColor);
            }
            
            
//            SDL_IntersectRect(&r, &clip_area, &clip_area);
//
//            SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(clip_area.x,
//                                                             clip_area.y,
//                                                             clip_area.w,
//                                                             clip_area.h));
        }
        
        if (titleTexture && titleColor.a) {
            
            // pom
            if (!isEnable()) {
                SDL_Color c = cGrey;
                SDL_SetTextureColorMod(titleTexture, c.r, c.g, c.b);
            } else {
                SDL_SetTextureColorMod(titleTexture, titleColor.r, titleColor.g, titleColor.b);
            }
            
            SDL_RenderCopy(GUI_renderer, titleTexture, NULL, &title_area);
        }
        
        if(action_hold_cmd && isFocus && isDown && holdTime!=-1){
            Uint32 elapsed = SDL_GetTicks() - holdTime;
            if(elapsed>1000){ // at least 1000 ms, action_hold_cmd will invoke
                if(action_hold_cmd)
                    action_hold_cmd(this, elapsed);
            }
        }
    }
    
    bool Button::handleEvents(SDL_Event *ev) {
        if( isVisible() == false )
            return false;
        
        if(isActive()==false && ev->type!=GUI_EventPaint){
            return false;
        }
        
        switch (ev->type) {
                
            case SDL_MOUSEBUTTONDOWN: {
                if(isActive()){
                    isDown = true;
                    isFocus = true;
                    GUI_mouseCapturedWindow = (GUI_View *)this;
                    touchTime = SDL_GetTicks(); // time in millis
                    touchHoldTime = touchTime;
                    holdTime = touchTime;
//                    GUI_LogNotice()<<"start";
                    return true;
                }
                return false;
            }
                
            case SDL_MOUSEMOTION: {
                if( isActive() && isFocus ) {
//                if( isActive()) {
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);
                    
                    if(isFocus){
                        if( hitTest(x, y) ) {
                            isDown = true;
                            return true;
                        }
                        else {
                            touchTime = -1; // reset touch time(hold) when lost focus
                            touchHoldTime = touchTime;
                            holdTime = touchTime;
                            isDown = false;
                        }
                    }
                    else{

                    }
                }
                return false;
            }
                
            case SDL_MOUSEBUTTONUP: {
                holdTime = -1;
//                GUI_LogNotice()<<"stop";
                if( isActive() && isFocus ) {
                    GUI_mouseCapturedWindow = NULL;
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);
                    holdTime = -1;
                    
                    isDown = false;
                    isFocus = false;
                    if( hitTest(x, y) ) {
                        if(touchTime!=-1){
                            touchHoldTime = SDL_GetTicks() - touchTime;
                            if(action_hold_end_cmd){
                                action_hold_end_cmd(this, touchHoldTime);
                            }
                        }
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
    
    bool Button::toggle(){
        toggle_state=!toggle_state;
        if(toggle_state){
            titleColor = color_selected;
            iconColor = color_selected;
        }
        else{
            titleColor = theme.color.label;
            iconColor = theme.color.icon;
            borderColor = theme.color.border;
        }
        return toggle_state;
    }
    
    bool Button::getToggleState(){
        return toggle_state;
    }
    
    
    
    int mouseX=0;
    int mouseY=0;
    // MARK: TexturedButton
    TexturedButton::TexturedButton(GUI_View *parent, SDL_Texture *icon, bool _isTextureColored, int x, int y, int w, int h):
    gui::Button(parent, "", x, y, w, h),
    isTextureColored(_isTextureColored)
    {
        setBorder(1);
        bgcol = theme.color.button.fill.normal;
        iconColor = isTextureColored ? theme.color.button.image.normal : theme.color.button.glyph.normal;
        borderColor = theme.color.button.stroke.normal;
        titleColor = theme.color.button.glyph.normal;
        setIcon(icon);
        
        setCorners((int)theme.layout.corner);
        float vv = theme.layout.button.vPadding;
        float hh = theme.layout.button.hPadding;
        setPadding((int)vv, (int)hh, (int)hh, (int)hh);
//        setPadding((int)vv, (int)hh, (int)vv, (int)hh);
        
//        display_cmd = [=](GUI_View* v){
////            if(GUI_mouseCapturedWindow==NULL){
//            if(v->hitTest(mouseX, mouseY)){
//                borderColor = theme.color.button.stroke.hover;
//            }
//            else{
//                borderColor = theme.color.button.stroke.normal;
//            }
////            }
//            return false;
//        };
    }
    
    TexturedButton::~TexturedButton(){
        
    }
    
    bool TexturedButton::toggle(){
        toggle_state=!toggle_state;
        if(toggle_state){
            bgcol = theme.color.button.fill.downed;
            iconColor = isTextureColored ? theme.color.button.image.downed : theme.color.button.glyph.downed;
            borderColor = theme.color.button.stroke.downed;
            titleColor = theme.color.button.glyph.downed;
        }
        else{
            bgcol = theme.color.button.fill.normal;
            iconColor = isTextureColored ? theme.color.button.image.normal : theme.color.button.glyph.normal;
            borderColor = theme.color.button.stroke.normal;
            titleColor = theme.color.button.glyph.normal;
        }
        return toggle_state;
    }
    
    bool TexturedButton::handleEvents(SDL_Event *ev) {
        if( isVisible() == false )
            return false;
        
//        if(hasToolTip() && hitTest(ofGetMouseX(), ofGetMouseY())){
//            EnableToolTip(toolTip);
//        }else{
//
//        }
        
        if(isActive()==false && ev->type!=GUI_EventPaint)
            return false;
        
        bool downed = getToggleState();
        
        switch (ev->type) {
                
            case SDL_MOUSEBUTTONDOWN: {
                if(isActive()){
                    isDown = true;
                    isFocus = true;
                    GUI_mouseCapturedWindow = (GUI_View *)this;
                    touchTime = SDL_GetTicks(); // time in millis
                    touchHoldTime = touchTime;
                    if(!downed){
                        bgcol = theme.color.button.fill.push;
                        iconColor = isTextureColored ? theme.color.button.image.push : theme.color.button.glyph.push;
                        borderColor = theme.color.button.stroke.push;
                        titleColor = theme.color.button.glyph.push;
                    }
                    return true;
                }
                return false;
            }
                
            case SDL_MOUSEMOTION: {
                if( isActive() && isFocus ) {
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);
                    mouseX = x;
                    mouseY = y;
                    
                    if( hitTest(x, y) ) {
                        isDown = true;
                        if(!downed){
                            bgcol = theme.color.button.fill.push;
                            iconColor = isTextureColored ? theme.color.button.image.push : theme.color.button.glyph.push;
                            borderColor = theme.color.button.stroke.push;
                            titleColor = theme.color.button.glyph.push;
                        }
                        return true;
                    }
                    else {
                        touchTime = -1;
                        touchHoldTime = touchTime;
                        isDown = false;
                        if(!downed){
                            bgcol = theme.color.button.fill.normal;
                            iconColor = isTextureColored ? theme.color.button.image.normal : theme.color.button.glyph.normal;
                            borderColor = theme.color.button.stroke.normal;
                            titleColor = theme.color.button.glyph.normal;
                        }
                    }
                }
//                else if(isActive()){
//
//                    SDL_MouseButtonEvent e = ev->button;
//                    int x = (int)(e.x*GUI_mouseScale);
//                    int y = (int)(e.y*GUI_mouseScale);
//
//                    if(!downed){
//
//                        if( hitTest(x, y) ) {
////                            GUI_mouseCapturedWindow = (GUI_View *)this;
//
//                            bgcol = theme.color.button.fill.hover;
//                            iconColor = isTextureColored ? theme.color.button.image.hover : theme.color.button.glyph.hover;
//                            borderColor = theme.color.button.stroke.hover;
//                            titleColor = theme.color.button.glyph.hover;
//
//                        }
//                        else{
////                            if(GUI_mouseCapturedWindow==this)
////                                GUI_mouseCapturedWindow = NULL;
//
//                            bgcol = theme.color.button.fill.normal;
//                            iconColor = isTextureColored ? theme.color.button.image.hover : theme.color.button.glyph.normal;
//                            borderColor = theme.color.button.stroke.normal;
//                            titleColor = theme.color.button.glyph.normal;
//                        }
//                    }
//                }
                return false;
            }
                
            case SDL_MOUSEBUTTONUP: {
                if( isActive() && isFocus ) {
                    GUI_mouseCapturedWindow = NULL;
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);
                    
                    isDown = false;
                    isFocus = false;
                    if( hitTest(x, y) ) {
                        if(touchTime!=-1){
                            touchHoldTime = SDL_GetTicks() - touchTime;
                        }
                        
                        if(!downed){
                            bgcol = theme.color.button.fill.hover;
                            iconColor = isTextureColored ? theme.color.button.image.hover : theme.color.button.glyph.hover;
                            borderColor = theme.color.button.stroke.hover;
                            titleColor = theme.color.button.glyph.hover;
                        }
                        
                        if( action_cmd ) {
                            action_cmd( this );
                            return true;
                        }
                    }
                    else{
                        if(!downed){
                            bgcol = theme.color.button.fill.normal;
                            iconColor = isTextureColored ? theme.color.button.image.normal : theme.color.button.glyph.normal;
                            borderColor = theme.color.button.stroke.normal;
                            titleColor = theme.color.button.glyph.normal;
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
    
    
    
    
    // MARK: PopupButton
    PopupButton::PopupButton(GUI_View* p, std::string default_title, int x, int y, int w, int h, int align, int fnt)
    :gui::Button(p, default_title, x,y,w,h, cBlack, align, fnt),
    isHighlightFocus(false)
    {
        bgcol = defaultTheme.color.inputAreaBackground;
        titleColor = defaultTheme.color.label;
        borderColor = defaultTheme.color.border;
        iconColor = cWhite;
        title_align = GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER;
        setCorners(3,3,3,3);
        corner = 3;
        
#if __USE_SKIA__
        setIcon(GUI_createImageFromUnicode(kIcon_unsorted, 0));
        createTitleImage(fnt);
#else
        setIcon(GUI_createTextureFormUnicode(kIcon_unsorted, 0));
        createTitleTexture(fnt);
#endif
        
        setPadding(2,5,2,5);
        setMargin(0,0,0,0);
        icon_gap = 5;
        setBorder(1);
    }
    
    PopupButton::~PopupButton(){
        
    }
    
    bool PopupButton::handleEvents( SDL_Event *ev ){
        if( isVisible() == false )
            return false;
        
        if(isActive()==false && ev->type!=GUI_EventPaint)
            return false;
        
        switch (ev->type) {
                
            case SDL_MOUSEBUTTONDOWN: {
                if(isActive()){
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);
                    
                    if(hitTest(x, y)){
                        isDown = true;
                        isFocus = true;
                        isHighlightFocus = true;
                        GUI_mouseCapturedWindow = (GUI_View *)this;
                        touchTime = SDL_GetTicks(); // time in millis
                        touchHoldTime = touchTime;
                        return true;
                    }
                    else{
                        isHighlightFocus = true;
                    }
                }
                return false;
            }
                
            case SDL_MOUSEMOTION: {
                if( isActive()) {
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);
                    
                    
                    if( isFocus ) {
//                        if( !hitTest(x, y) ) {
////                             isFocus = false;
//                            return false;
//                        }
                        if(hitTest(x, y)){
                            isDown = true;
                            return true;
                        }
                        else{
                            touchTime = -1;
                            touchHoldTime = touchTime;
                            isDown = false;
                        }
                    }
                }
                return false;
            }
                
            case SDL_MOUSEBUTTONUP: {
                if( isActive() && isFocus ) {
                    if(GUI_mouseCapturedWindow==this)
                        GUI_mouseCapturedWindow->isFocus = false;
                    GUI_mouseCapturedWindow = NULL;
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);
                    
                    if( this->isEnable() == false )
                        return false;
                    
                    if( isFocus ) {
                        if( !hitTest(x, y) ) {
                            isFocus = false;
                            GUI_mouseCapturedWindow = NULL;
                            return false;
                        }
                    }
                    
                    isDown = false;
                    isFocus = false;
                    
                    SDL_Point p{x,y};
//                    if(SDL_PointInRect(&p, &caret_tw_area)){
                    if(SDL_PointInRect(&p, &tw_area)){
                    
                        if(touchTime!=-1){
                            touchHoldTime = SDL_GetTicks() - touchTime;
                        }
                        if( action_cmd ) {
                            action_cmd( this );
                            return true;
                        }
                    }
                }
                
                isFocus = false;
                isDown = false;
                return false;
            }
                
                
            default: {
                GUI_View::handleEvents( ev );
            }
        }
        return false;
    }
    
    void PopupButton::clear(GUI_Rect *r){

        guiRectangle rect(0.0f, 0.0f, (float)tw_area.w, (float)tw_area.h);

        int c[4] = {
            corners[0]*GUI_scale,
            corners[1]*GUI_scale,
            corners[2]*GUI_scale,
            corners[3]*GUI_scale};
        
        
        GUI_FillRoundRect((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, c, bgcol);
        
        guiRectangle caret_rect((float)(rect.width-caret_tw_area.w), 0.0f, (float)caret_tw_area.w, (float)caret_tw_area.h);
        
        int cc[4] = {0, corners[1]*GUI_scale, corners[1]*GUI_scale, 0};
        SDL_Color color = isDown ? theme.color.highlightStroke : theme.color.highlightFill;
        GUI_FillRoundRect((int)caret_rect.x, (int)caret_rect.y, (int)caret_rect.width, (int)caret_rect.height, cc, color);
    }
    
    void PopupButton::updateSubLayout(){
        int w = title_area.w;
        int h = title_area.h;
        if( w < min_width )
            w = min_width;
        if( h < min_height )
            h = min_height;
        int tx = 0;
        
        if( textureIcon ) {
            tx = icon_area.w;
            if( title_area.w > 0 ) {
                tx += icon_gap*GUI_scale;
            }
            if( icon_area.h > title_area.h ) {
                h += (icon_area.h - title_area.h);
            }
            
            w += tx;
        }
        
        bool size_changed = false;
        if( ow == 0 ) {
            int wwx = w+(padding[1]+padding[3])*GUI_scale;
            if( wwx != tw_area.w ) {
                tw_area.w = wwx;
                size_changed = true;
            }
        }
        if( oh == 0 ) {
            int hhx = h+(padding[0]+padding[2])*GUI_scale;
            if( hhx != tw_area.h ) {
                tw_area.h = hhx;
                size_changed = true;
            }
        }
        
        caret_tw_area = tw_area;
        caret_tw_area.y = tw_area.y;
        caret_tw_area.x=tw_area.x+tw_area.w-icon_area.w-(icon_gap+padding[1])*GUI_scale;
        caret_tw_area.w=(tw_area.x+tw_area.w)-caret_tw_area.x;
        
        if( title_align & GUI_ALIGN_CENTER ) {
            icon_area.x = tw_area.w-icon_area.w-padding[1] * GUI_scale;
            title_area.x = ((tw_area.w-icon_area.w)-title_area.w)/2;
        }
        else if( title_align & GUI_ALIGN_RIGHT ) {
            icon_area.x = tw_area.w-icon_area.w-padding[1] * GUI_scale;
            title_area.x = icon_area.x-title_area.w-(icon_gap+2)*GUI_scale;
        }
        else {
            icon_area.x = tw_area.w-icon_area.w-padding[1] * GUI_scale;
            title_area.x = icon_area.x-title_area.w;
            title_area.x = icon_area.x-title_area.w-icon_gap*GUI_scale;
        }
        if( title_align & GUI_ALIGN_VCENTER ) {
            title_area.y = ((tw_area.h-title_area.h) / 2);
            icon_area.y = ((tw_area.h-icon_area.h) / 2);
        }
        else if( title_align & GUI_ALIGN_BOTTOM ) {
            title_area.y = (tw_area.h-title_area.h) - padding[2] *GUI_scale;
            icon_area.y = (tw_area.h-icon_area.h) - padding[2] *GUI_scale;
        }
        else {
            icon_area.y = padding[0] * GUI_scale;
            title_area.y = padding[0] * GUI_scale;
        }
        
        
        
        if( size_changed ) {
            if( parent ) {
                parent->updateLayout();
            }
        }
    }
    
}
