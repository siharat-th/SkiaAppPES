//
//  pesCanvas.hpp
//  all
//
//  Created by SIHARAT THAMMAYA on 7/2/2565 BE.
//

#ifndef pesCanvas_hpp
#define pesCanvas_hpp

#include <stdio.h>
#include "sk_ui.h"
#include "include/pathops/SkPathOps.h"
#include "pesEngine.h"

namespace sk_ui {

// ported from canvaskit
class pesCanvas{
public:
    pesCanvas();
    void set(int w, int h, float s);
    void setOrigin(float x, float y);
    void setZoomLevel(float zoomLevel);
    void onMouse(int x, int y, skui::InputState state);
    
    
    void draw(SkCanvas &canvas);
    int w, h;
    float s;
    
//    pesVec2f canvasOrigin;
//    float zoomLevel;
//    float maxZoomIn = 10;
//    float minZoomOut = 0.25;
//    
//    pesVec2f panValue;
    
    
};

}

#endif /* pesCanvas_hpp */
