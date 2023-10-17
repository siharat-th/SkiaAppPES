//
//  guiToolTips.cpp
//  VG2EMB
//
//  Created by Siharat on 25/11/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "guiToolTips.hpp"
#include "guiTheme.h"

namespace gui {
    
    static Uint32 t1=0, t2=0;
    static float __fadeValue = 255;
    bool found_tooltip = false;
    string tooltip = "";
    SDL_Point pos{0,0};
    
    bool ToolTips_HitTest(int mx, int my){
        if(GUI_TopView || GUI_modalWindow){
//            GUI_View* wb = GUI_TopView->hitTest(mx, my, true);
            GUI_View* wb = nullptr;
            if(GUI_modalWindow){
                wb = GUI_modalWindow->hitTest(mx, my, true);
            }
            else if(GUI_TopView){
                wb = GUI_TopView->hitTest(mx, my, true);
            }
            if(wb && wb->hasToolTip()){
                if(t1==0){
                    t1=SDL_GetTicks()+1000;
                }
                else if(SDL_GetTicks()>=t1){
                    found_tooltip = true;
                    t2=0;
//                    if(tooltip!=wb->getToolTip()){
                    if(tooltip.compare(wb->getToolTip())!=0){
                        tooltip=wb->getToolTip();
                        pos.y = my;
                        
                        pos.x = mx;
                    }
                }
            }
            else{
                if(t2==0)
                    t2=SDL_GetTicks()+500;
                if(SDL_GetTicks()>=t2){
                    found_tooltip = false;
                    t1=0;
                    tooltip.clear();
                }
            }
            
            return found_tooltip;
        }
        
        return false;
    }
    
    bool ToolTips_IsFound(){
        return found_tooltip;
    }
    
    bool ToolTips_IsFadeOut(){
        if(!found_tooltip && __fadeValue<=0){
            return false;
        }
        return true;
    }
    
    std::string ToolTips_Get(){
        return tooltip;
    }
    
    
    
    
    SDL_Texture* __tooltip_texture = nullptr;
    std::string __tooltip_cache;
    SDL_Rect __tooltip_rect = {0,0,0,0};
    void drawToolTip(const std::string &text, int x, int y){
       
        if(text.empty()==false){
            if(__tooltip_texture==nullptr || __tooltip_cache!=text){
                if(__tooltip_texture!=nullptr){
                    SDL_DestroyTexture(__tooltip_texture);
                    __tooltip_texture = nullptr;
                    __tooltip_cache.clear();
                    __tooltip_rect = {0,0,0,0};
                }
                __tooltip_texture = GUI_createTextTexture(text.c_str(), GUI_FONT_MICRO, &__tooltip_rect);
                if(__tooltip_texture){
                    SDL_Color c = defaultTheme.color.tooltip.label;
                    SDL_SetTextureColorMod(__tooltip_texture, c.r, c.g, c.b);
                    __tooltip_cache = text;
                }
            }
            
            if(__tooltip_texture!=nullptr){
                int paddle = 5*GUI_scale;
                int dx = (x+2*GUI_scale+__tooltip_rect.w+paddle*2) - GUI_windowWidth*GUI_scale;
                if(dx>0)
                    x-=dx;
                int dy = (y+2*GUI_scale+__tooltip_rect.h) - GUI_windowHeight*GUI_scale;
                if(dy>0)
                    y-=dy;
                
                
                GUI_FillRect(GUI_renderer, x+2*GUI_scale, y+2*GUI_scale, __tooltip_rect.w+paddle*2, __tooltip_rect.h, defaultTheme.color.tooltip.shadow);
                GUI_FillRect(GUI_renderer, x, y, __tooltip_rect.w+paddle*2, __tooltip_rect.h, defaultTheme.color.tooltip.background);
                GUI_DrawRect(GUI_renderer, x, y, __tooltip_rect.w+paddle*2, __tooltip_rect.h, defaultTheme.color.tooltip.border);
                SDL_Rect dstRect = {x+paddle, y, __tooltip_rect.w, __tooltip_rect.h};
                __fadeValue = 255;
                SDL_SetTextureAlphaMod(__tooltip_texture, (Uint8)round(__fadeValue));
                SDL_RenderCopy(GUI_renderer, __tooltip_texture, &__tooltip_rect, &dstRect);
            }
            else{
                GUI_LogWarning("drawToolTip", "load text texture fail");
            }
        }
    }
    
    void fadeoutToolTip(const std::string &text, int x, int y){
        if(text.empty()==false && __tooltip_texture!=nullptr){
            __fadeValue = guiLerp(__fadeValue, 0.0f, 0.1f);
            if(__fadeValue<=0){
                tooltip.clear();
                return;
            }
            
            int paddle = 5*GUI_scale;
            int dx = (x+2*GUI_scale+__tooltip_rect.w+paddle*2) - GUI_windowWidth*GUI_scale;
            if(dx>0)
                x-=dx;
            int dy = (y+2*GUI_scale+__tooltip_rect.h) - GUI_windowHeight*GUI_scale;
            if(dy>0)
                y-=dy;
            
            Uint8 alpha = (Uint8)round(__fadeValue);
            
            SDL_Color bgcol = defaultTheme.color.tooltip.background;
            bgcol.a = alpha;
            GUI_FillRect(GUI_renderer, x, y, __tooltip_rect.w+paddle*2, __tooltip_rect.h, bgcol);
            
            SDL_Color bordercol = defaultTheme.color.tooltip.border;
            bordercol.a = alpha;
            GUI_DrawRect(GUI_renderer, x, y, __tooltip_rect.w+paddle*2, __tooltip_rect.h, bordercol);
            
            SDL_Rect dstRect = {x+paddle, y, __tooltip_rect.w, __tooltip_rect.h};
            SDL_SetTextureAlphaMod(__tooltip_texture, alpha);
            SDL_RenderCopy(GUI_renderer, __tooltip_texture, NULL, &dstRect);
        }
    }
    
#define mouse_cursor_height 20*GUI_scale
    bool ToolTips_Render(){
        int x = pos.x;
        int y = pos.y+mouse_cursor_height;
        if(found_tooltip)
        {
            gui::drawToolTip(tooltip, x, y);
        }else
        {
            gui::fadeoutToolTip(__tooltip_cache, x, y);
            if(__fadeValue<=0)
                return false;
        }
        return true;
    }
}
