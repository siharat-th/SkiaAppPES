//
//  guiScrollBar.hpp
//  PINNPES
//
//  Created by Siharat on 8/12/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef guiScrollBar_hpp
#define guiScrollBar_hpp

#include <stdio.h>
#include "SDL_gui.h"
#include "guiRectangle.hpp"

namespace gui{
    
    class ScrollBar{
        
    public:
        ScrollBar();
        ~ScrollBar();
        
        void setup(GUI_View* view, const guiRectangle & contentRect, float *scale);
        void draw();
        
        bool handleEvent(SDL_Event *ev);
        
    private:
        
        GUI_View* _view;
        
        guiRectangle vScrollbar, hScrollbar;
        guiRectangle _contentRect;
        GUI_Rect* _viewRect;
        float* _scale;
        
        bool dragReverseY, dragReverseX;
        bool isVFocus, isVdown, isHFocus, isHdown;
    };
    
}

#endif /* guiScrollBar_hpp */
