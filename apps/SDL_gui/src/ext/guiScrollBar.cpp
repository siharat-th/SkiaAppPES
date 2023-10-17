//
//  guiScrollBar.cpp
//  PINNPES
//
//  Created by Siharat on 8/12/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "guiScrollBar.hpp"
#include "guiExt.h"

namespace gui{

    ScrollBar::ScrollBar(){
        _view = nullptr;
        dragReverseY = dragReverseX = false;
        isVFocus = isVdown = isHFocus = isHdown = false;
    }

    ScrollBar::~ScrollBar(){
        
    }

    void ScrollBar::setup(GUI_View* view, const guiRectangle & contentRect, float *scale){
        _view = view;
        _contentRect = contentRect;
        _viewRect = &_view->tw_area;
        _scale = scale;
    }

    void ScrollBar::draw(){
        int centerX = _view->tw_area.x+((_view->tw_area.w)/2);
        int centerY = _view->tw_area.y+((_view->tw_area.h)/2);
//        float s = 3600 * (*_scale) * GUI_scale;
        float s = 3600 * (*_scale);
        guiRectangle rr;
        rr.setFromCenter((float)centerX, (float)centerY, s, s);
        _contentRect.width = rr.width;
        _contentRect.height = rr.height;
        

        bool scrollable = _contentRect.height > _viewRect->h;
        bool hscrollable = _contentRect.width > _viewRect->w;
        bool mousePressed = gui::GetMousePressed();
        // Let's call it a day
        gui::Theme _theme = gui::defaultTheme;
        if(scrollable){
            int w = 12 * GUI_scale;
            int h = hscrollable ? w : 0;
            GUI_FillRect(_viewRect->x+_viewRect->w-w, _viewRect->y+25*GUI_scale+1, w, _viewRect->h, _theme.color.scroll.background);
            
            float dh = _contentRect.height - _viewRect->h - h;
            float dy = abs(_contentRect.y - _viewRect->y);
            float ratio = (dy/dh);
            
            float bw = (float)(8 * GUI_scale);
            float bh = _viewRect->h - h - (_contentRect.height - _viewRect->h) - 2;
            if(bh<25 * GUI_scale)
                bh = (float)(25 * GUI_scale);
            float y = _viewRect->y + ((_viewRect->h-h-bh) * ratio);
            if(y>_viewRect->y+_viewRect->h-h-bh-1)
                y = _viewRect->y+_viewRect->h-h-bh-1;
            
            if(y<_viewRect->y+25*GUI_scale)
                y = (float)(_viewRect->y+25*GUI_scale);
            
            guiRectangle bar((float)(_viewRect->x+_viewRect->w-w), y, (float)w, bh);
            vScrollbar=bar;
            
            SDL_Color c;
            if(mousePressed==false){
                if(bar.inside((float)gui::GetMouseX(), (float)gui::GetMouseY())){
                    c = _theme.color.scroll.barOnMouseHover;
                    if(mousePressed==false)
                        dragReverseY = true;
                }
                else{
                    c = _theme.color.scroll.bar;
                    if(mousePressed==false)
                        dragReverseY = false;
                }
            }
            else{
                if(dragReverseY){
                    c = _theme.color.scroll.barOnMouseHover;
                }
                else{
                    c = _theme.color.scroll.bar;
                }
            }
            
            GUI_FillRoundRect((int)(_viewRect->x+_viewRect->w-bw-(w-bw)/2), (int)y, (int)bw, (int)bh, (int)(bw/2-1), c);
            GUI_DrawLine(_viewRect->x+_viewRect->w-w,
                       _viewRect->y,
                       _viewRect->x+_viewRect->w-w,
                       _viewRect->y+_viewRect->h-h, _theme.color.border);
        }
        
        
        if(hscrollable){
            int h = 12 * GUI_scale;
            int w = scrollable ? h : 0;
            GUI_FillRect(_viewRect->x+25*GUI_scale+1, _viewRect->y+_viewRect->h-h, _viewRect->w, h, _theme.color.scroll.background);

            float dw = _contentRect.width - _viewRect->w - w;
            float dx = abs(_contentRect.x - _viewRect->x);
            float ratio = (dx/dw);

            float bh = (float)(8 * GUI_scale);
            float bw = (float)(_viewRect->w - w - (_contentRect.width - _viewRect->w) - 2);
            if(bw < 25*GUI_scale)
                bw = (float)(25 * GUI_scale);
            float x = _viewRect->x + ((_viewRect->w-w-bw) * ratio);
            if(x>_viewRect->x+_viewRect->w-w-bw-1)
                x = _viewRect->x+_viewRect->w-w-bw-1;

            if(x<_viewRect->x+25*GUI_scale)
                x = (float)(_viewRect->x+25*GUI_scale);

            guiRectangle bar(x, (float)(_viewRect->y+_viewRect->h-h*1), bw, (float)(h*1));
            hScrollbar=bar;

            SDL_Color c;
            if(mousePressed==false){
                if(bar.inside((float)gui::GetMouseX(), (float)gui::GetMouseY())){
                    c = _theme.color.scroll.barOnMouseHover;
                    if(mousePressed==false)
                        dragReverseX = true;
                }
                else{
                    c = _theme.color.scroll.bar;
                    if(mousePressed==false)
                        dragReverseX = false;
                }
            }
            else{
                if(dragReverseX){
                    c = _theme.color.scroll.barOnMouseHover;
                }
                else{
                    c = _theme.color.scroll.bar;
                }
            }

            GUI_FillRoundRect((int)x, (int)(_viewRect->y+_viewRect->h-bh-(h-bh)/2), (int)bw, (int)bh, (int)(bh/2-1), c);
            GUI_DrawLine(_viewRect->x,
                       _viewRect->y+_viewRect->h-h,
                       _viewRect->x+_viewRect->w-w,
                       _viewRect->y+_viewRect->h-h,
                       _theme.color.border );
        }
        else{
            dragReverseX = false;
        }

        GUI_DrawRect(_viewRect->x, _viewRect->y, _viewRect->w, _viewRect->h, _theme.color.border);
    }

    #include "guiEvents.h"
    bool ScrollBar::handleEvent(SDL_Event *ev){
        
        switch (ev->type) {
            case SDL_MOUSEBUTTONDOWN:
                if(isVFocus) isVdown=true;
                if(isHFocus) isHdown=true;
                break;
                
            case SDL_MOUSEMOTION:
            {
                auto e = ev->motion;
                int x = (int)(e.x*GUI_mouseScale);
                int y = (int)(e.y*GUI_mouseScale);
                Uint32 state = e.state;
                if(state==SDL_PRESSED){
                    // mouse being drag
                    if(isVdown){
                        float d = (float)(y - gui::GetPreviousMouseY());
                        _contentRect.y-=d*3;
                    }
                    else if(isHdown){
                        float d = (float)(x - gui::GetPreviousMouseX());
                        _contentRect.x-=d*2;
                    }
                }
                else{ // SDL_RELEASED
                    if(vScrollbar.inside((float)x, (float)y)){
                        isVFocus=true;
                    }
                    else{
                        isVFocus=false;
                    }
                    
                    if(hScrollbar.inside((float)x, (float)y)){
                        isHFocus=true;
                    }
                    else{
                        isHFocus=false;
                    }
                }
                break;
            }
                
            
            case SDL_MOUSEBUTTONUP:
                isVFocus = isVdown = isHFocus = isHdown = false;
                break;
                
            case SDL_MOUSEWHEEL:
            {
                auto e = ev->wheel;
                float sx = (float)e.x;
                float sy = (float)e.y;
                float movex = 0;
                float movey = 0;
                
                bool hit = _view->hitTest(gui::GetMouseX(), gui::GetMouseY());
                bool scrollable = _contentRect.height > _view->tw_area.h;
                if(scrollable && hit){
                    movey = sy*7;
                    float vb = (float)(_contentRect.y + _contentRect.height);
                    float pb = (float)(_view->tw_area.y + _view->tw_area.h);
                    if(vb<pb){
                        //                dy = pb-vb;
                        movey = pb-vb;
                    }
                }
                else{
                    if(_contentRect.y!=_view->tw_area.y){
                        movey = _view->tw_area.y - _contentRect.y;
                    }
                }
                
                bool hscrollable = _contentRect.width > _view->tw_area.w;
                if(hscrollable && hit){
                    movex = sx*7;
                    float vr = _contentRect.x + _contentRect.width;
                    float pr = (float)(_view->tw_area.x + _view->tw_area.w);
                    if(vr<pr){
                        movex = pr-vr;
                    }
                }
                else{
                    if(_contentRect.x!=_view->tw_area.x){
                        movex = _view->tw_area.x - _contentRect.x;
                    }
                }
                
                
                //        if(movex>movey)
                _contentRect.x+=movex;
                //        else
                _contentRect.y+=movey;
                
                if(_contentRect.x>_viewRect->x+25*GUI_scale)
                    _contentRect.x= (float)(_viewRect->x+25*GUI_scale);
                
                if(_contentRect.y>_viewRect->y+25*GUI_scale)
                    _contentRect.y= (float)(_viewRect->y+25*GUI_scale);
                break;
            }
            
                
            default:
                break;
        }
        
        return false;
    }
    
}
