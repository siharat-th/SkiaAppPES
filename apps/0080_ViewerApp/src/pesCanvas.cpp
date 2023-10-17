//
//  pesCanvas.cpp
//  all
//
//  Created by SIHARAT THAMMAYA on 7/2/2565 BE.
//

#include "pesCanvas.hpp"

namespace sk_ui {

int pressedX, pressedY;
int previousX, previousY;

pesVec2f canvasOrigin;
float zoomLevel;
const float maxZoomIn = 10;     // 1000%
const float minZoomOut = 0.25;  // 25%

pesVec2f panValue;
// panAllow
int minX =   -2400;
int maxX =   2400;


pesCanvas::pesCanvas(){
    zoomLevel = 1.0;
    panValue.set(0, 0);
}

void pesCanvas::set(int _w, int _h, float _s){
    this->w = _w;
    this->h = _h;
    this->s = _s;
    canvasOrigin.set(_w/2, _h/2);
}

void pesCanvas::draw(SkCanvas &canvas){
    canvas.clear(SK_ColorWHITE);
    canvas.save();
    {
        canvas.translate(canvasOrigin.x+panValue.x, canvasOrigin.y+panValue.y);
        canvas.scale(zoomLevel, zoomLevel);
        
//        // draw grid
//        var hoopSize = CanvasKit.pesDocument.getInstance().getHoopSizeInMM();
//        var hw = hoopSize.x * 10;
//        var hh = hoopSize.y * 10;
//        
//
//        noStroke();
//        fill(222);
//        rect(0, 0, hw, hh);
//        stroke(200);
//        strokeWeight(1);
//        // for(let x=-500; x<=500; x+=50){
//        //     line(x, -500, x, 500);
//        // }
//        // for(let y=-500; y<=500; y+=50){
//        //     line(-500, y, 500, y);
//        // }
//        for(let x=-hw/2; x<=hw/2; x+=50){
//            line(x, -hh/2, x, hh/2);
//        }
//        for(let y=-hh/2; y<=hh/2; y+=50){
//            line(-hw/2, y, hw/2, y);
//        }
//
//        stroke(160);
//        strokeWeight(2);
//        line(0, -3600, 0, 3600);
//        line(-3600, 0, 3600, 0);
//        stroke(0, 0, 0, 255);
//        fill(255, 255, 255, 255);
//        noFill();
        
        // ...
    }
    canvas.restore();
}

void pesCanvas::onMouse(int x, int y, skui::InputState state){
    // ...
}

void pesCanvas::setOrigin(float x, float y){
    canvasOrigin.set(x, y);
}

void pesCanvas::setZoomLevel(float z){
    zoomLevel = z;
    if(zoomLevel<minZoomOut)
        zoomLevel = minZoomOut;
    if(zoomLevel>maxZoomIn)
        zoomLevel = maxZoomIn;
}

}
