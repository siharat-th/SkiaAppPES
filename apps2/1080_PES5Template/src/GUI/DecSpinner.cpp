//
//  DecSpinner.cpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 6/10/2565 BE.
//

#include "DecSpinner.hpp"
#include "DecUI.h"
#include "DecSpacer.h"

namespace sk_ui{

DecSpinner::DecSpinner(float v, float min, float max, float step){
    pText = NULL;
    value(v);
    _min = min;
    _max = max;
    _step = step;
//    cache(false);
    size(-1, 32);
    border(false);
    background(true);
    bg_color(0xFFF0F0F0);
    round(10);
    char c[16];
//    sprintf(c, "%g", _value);
    sprintf(c, "%0.2f",_value);
    
    child( Button("", 0xf068, [=](DecButton & b){
        _value -= step;
        if(_value<_min) _value = _min;
        char c[16];
//        sprintf(c, "%g", _value);
        sprintf(c, "%0.2f",_value);
        if(pText){
            pText->setText(c);
        }
    }) );
    child( Spacer().size( -1, 0 ) );
    child( Text(c)
          .size(-1, 0)
          .align(ALIGN_CENTER)
//          .background(true)
//          .bg_color(0xFFF0F0F0)
          .ref((DecWidget**)&pText) );
    child( Spacer().size( -1, 0 ) );
    child( Button("", 0xf067, [=](DecButton & b){
        _value += step;
        if(_value>_max) _value = _max;
        char c[16];
//        sprintf(c, "%g", _value);
        sprintf(c, "%0.2f",_value);
        if(pText){
            pText->setText(c);
        }
    }) );
}

DecSpinner::~DecSpinner(){
    
}

DecWidget& DecSpinner::value(float v) {
    char c[16];
//    sprintf(c, "%g",_value);
    sprintf(c, "%0.2f",v);
    if(pText){
        pText->setText(c);
    }
    dirty(true, true);
    return DecWidget::value(v);
}

float DecSpinner::value() {
    return DecWidget::value();
}

DecSpinner &Spinner(float value, float min, float max, float step){
    DecSpinner *w = new DecSpinner(value, min, max, step);
    return *w;
}

}
