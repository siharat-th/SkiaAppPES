//
//  GUI_ColorPicker.cpp
//  FontWorks
//
//  Created by SIHARAT THAMMAYA on 12/31/2559 BE.
//  Copyright © 2559 Jimmy Software Co., Ltd. All rights reserved.
//

#include "GUI_ColorPicker.hpp"

template<typename PixelType>
float GUI_Color_<PixelType>::limit() {
    return std::numeric_limits<PixelType>::max();
}

template<>
float GUI_Color_<float>::limit() {
    return 1.f;
}

template<typename PixelType>
GUI_Color_<PixelType>::GUI_Color_(float _r, float _g, float _b, float _a):
r(_r),
g(_g),
b(_b),
a(_a){
}

template<typename PixelType>
GUI_Color_<PixelType> GUI_Color_<PixelType>::fromHsb(float hue,
                                                     float saturation,
                                                     float brightness,
                                                     float alpha){
    GUI_Color_<PixelType> cur;
    cur.setHsb(hue, saturation, brightness, alpha);
    return cur;
}


template<typename PixelType>
GUI_Color_<PixelType> GUI_Color_<PixelType>::fromHex(int hexColor, float alpha){
    GUI_Color_<PixelType> cur;
    cur.setHex(hexColor, alpha);
    return cur;
}

template<typename PixelType>
void GUI_Color_<PixelType>::set(float _r, float _g, float _b, float _a){
    r = _r;
    g = _g;
    b = _b;
    a = _a;
}


template<typename PixelType>
void GUI_Color_<PixelType>::set(float _gray, float _a){
    r = _gray;
    g = _gray;
    b = _gray;
    a = _a;
}


template<>
void GUI_Color_<unsigned char>::setHex(int hexColor, float alpha){
    r = (hexColor >> 16) & 0xff;
    g = (hexColor >> 8) & 0xff;
    b = (hexColor >> 0) & 0xff;
    a = alpha;
}

template<typename PixelType>
void GUI_Color_<PixelType>::setHex (int hexColor, float alpha){
    GUI_Color_ c = GUI_Color_::fromHex(hexColor);
    *this = c;
    a = alpha;
}


template<typename PixelType>
GUI_Color_<PixelType>& GUI_Color_<PixelType>::clamp(){
    r = CLAMP(r, 0.0f, limit());
    g = CLAMP(g, 0.0f, limit());
    b = CLAMP(b, 0.0f, limit());
    a = CLAMP(a, 0.0f, limit());
    return *this;
}

template<typename PixelType>
float GUI_Color_<PixelType>::getHue() const {
    float hue = 0;
    float saturation = 0;
    float brightness = 0;
    getHsb(hue, saturation, brightness);
    return hue;
}

template<typename PixelType>
float GUI_Color_<PixelType>::getHueAngle() const {
    return getHue() * 360. / limit();
}

template<typename PixelType>
float GUI_Color_<PixelType>::getSaturation() const {
    float hue = 0;
    float saturation = 0;
    float brightness = 0;
    getHsb(hue, saturation, brightness);
    return saturation;
}

template<typename PixelType>
float GUI_Color_<PixelType>::getBrightness() const {
    float max = r;
    if(g > max) {
        max = g;
    }
    if(b > max) {
        max = b;
    }
    return max;
}

template<typename PixelType>
float GUI_Color_<PixelType>::getLightness() const {
    return (r + g + b) / 3.f;
}

template<typename PixelType>
void GUI_Color_<PixelType>::getHsb(float& hue,
                                   float& saturation,
                                   float& brightness) const {
    float max = getBrightness();
    
    float min = r;
    if(g < min) {
        min = g;
    }
    if(b < min) {
        min = b;
    }
    
    if(max == min) { // grays
        hue = 0.f;
        saturation = 0.f;
        brightness = max;
        return;
    }
    
    float hueSixth;
    if(r == max) {
        hueSixth = (g - b) / (max - min);
        if(hueSixth < 0.f)
            hueSixth += 6.f;
    } else if (g == max) {
        hueSixth = 2.f + (b - r) / (max - min);
    } else {
        hueSixth = 4.f + (r - g) / (max - min);
    }
    hue = limit() * hueSixth / 6.f;
    saturation = limit() * (max - min) / max;
    brightness = max;
}

template<typename PixelType>
void GUI_Color_<PixelType>::setHue(float hue) {
    float oldHue = 0;
    float saturation = 0;
    float brightness = 0;
    getHsb(oldHue, saturation, brightness);
    setHsb(hue, saturation, brightness, a);
}

template<typename PixelType>
void GUI_Color_<PixelType>::setHueAngle(float angle) {
    setHue(angle / 360. * limit());
}

template<typename PixelType>
void GUI_Color_<PixelType>::setSaturation(float saturation) {
    float hue, oldSaturation, brightness;
    getHsb(hue, oldSaturation, brightness);
    setHsb(hue, saturation, brightness, a );
}


template<typename PixelType>
void GUI_Color_<PixelType>::setBrightness(float brightness) {
    float hue, saturation, oldBrightness;
    getHsb(hue, saturation, oldBrightness);
    setHsb(hue, saturation, brightness, a );
}


template<typename PixelType>
void GUI_Color_<PixelType>::setHsb(float hue, float saturation, float brightness, float alpha) {
    saturation = CLAMP(saturation, 0, limit());
    brightness = CLAMP(brightness, 0, limit());
    if(brightness == 0) { // black
        set(0);
    } else if(saturation == 0) { // grays
        set(brightness);
    } else {
        float hueSix = hue * 6.f / limit();
        float saturationNorm = saturation / limit();
        int hueSixCategory = (int) floorf(hueSix);
        float hueSixRemainder = hueSix - hueSixCategory;
        PixelType pv = (PixelType) ((1.f - saturationNorm) * brightness);
        PixelType qv = (PixelType) ((1.f - saturationNorm * hueSixRemainder) * brightness);
        PixelType tv = (PixelType) ((1.f - saturationNorm * (1.f - hueSixRemainder)) * brightness);
        switch(hueSixCategory) {
            case 0: case 6: // r
                r = brightness;
                g = tv;
                b = pv;
                break;
            case 1: // g
                r = qv;
                g = brightness;
                b = pv;
                break;
            case 2:
                r = pv;
                g = brightness;
                b = tv;
                break;
            case 3: // b
                r = pv;
                g = qv;
                b = brightness;
                break;
            case 4:
                r = tv;
                g = pv;
                b = brightness;
                break;
            case 5: // back to r
                r = brightness;
                g = pv;
                b = qv;
                break;
        }
    }
    
    // finally assign the alpha
    a = alpha;
}

template<>
int GUI_Color_<unsigned char>::getHex() const {
    return
    ((0xff & (unsigned char) r) << 16) |
    ((0xff & (unsigned char) g) << 8) |
    ((0xff & (unsigned char) b));
}

template<typename PixelType>
int GUI_Color_<PixelType>::getHex() const {
    return ((GUI_Color_) *this).getHex();
}

template<typename PixelType>
GUI_Color_<PixelType> & GUI_Color_<PixelType>::operator = (const float& val){
    r = val;
    g = val;
    b = val;
    a = limit();
    return *this;
}

template<typename PixelType>
const PixelType & GUI_Color_<PixelType>::operator [] (std::size_t n) const{
    switch( n ){
        case 0:
            return r;
        case 1:
            return g;
        case 2:
            return b;
        case 3:
            return a;
        default:
            return r;
            break;
    }
}

template<typename PixelType>
PixelType & GUI_Color_<PixelType>::operator [] (std::size_t n){
    switch( n ){
        case 0:
            return r;
        case 1:
            return g;
        case 2:
            return b;
        case 3:
            return a;
        default:
            return r;
            break;
    }
}

template class GUI_Color_<char>;
template class GUI_Color_<unsigned char>;
template class GUI_Color_<short>;
template class GUI_Color_<unsigned short>;
template class GUI_Color_<int>;
template class GUI_Color_<unsigned int>;
template class GUI_Color_<long>;
template class GUI_Color_<unsigned long>;
template class GUI_Color_<float>;
template class GUI_Color_<double>;


// MARK: GUI_ColorPickerHSB
static GUI_Color_<uint8_t> __guiColor, __hueColor;
GUI_ColorPickerHSB::GUI_ColorPickerHSB(GUI_View *parent, int x, int y, int w, int h)
: GUI_View(parent, "ColorPicker", x, y, w, h),
onColorChange_func(NULL)
{
    dragable = false;
    bgcol = cClear;
    setBorder(0);
    
    int xx = 195;
    int yy = 35;
    GUI_Label *hLabel = new GUI_Label(this, "H:    304", xx, yy);
    GUI_Label *sLabel = new GUI_Label(this, "S:    86",  xx, yy+50);
    GUI_Label *bLabel = new GUI_Label(this, "B:    86",  xx, yy+100);
    SDL_Color labelColor = sdl_color(0x535353ff); //{0x6b, 0x6b, 0x6b, 0xff};
    hLabel->titleColor = labelColor;
    sLabel->titleColor = labelColor;
    bLabel->titleColor = labelColor;
    
    hSlider = new GUI_Slider(this, 0, 160, 300, 50);
    sSlider = new GUI_Slider(this, 0, 200, 300, 50);
    bSlider = new GUI_Slider(this, 0, 240, 300, 50);
    
    hSlider->setBackground(GUI_LoadDITexture("gui3/icon-color-rainbow.png", GUI_renderer));
    hSlider->setDrawLine(false);
    hSlider->change_cmd = [=](GUI_Slider *s, float f){
        int hueAngle = (int)(f * 360);
        char c[16];
        sprintf(c, "H:    %d", hueAngle);
        hLabel->setTitle(c);
        __guiColor.setHue(f*255);
        __hueColor.setHue(f*255);
        
        SDL_Color fromHue = {__hueColor.r, __hueColor.g, __hueColor.b, __hueColor.a};
        sSlider->texBgColor = fromHue;
        bSlider->texBgColor = fromHue;
        
        if(onColorChange_func)
            onColorChange_func(this, __guiColor.getHex());
    };
    hSlider->setValue(0.5);
    
    
    sSlider->setBackground(GUI_LoadDITexture("gui3/icon-color-saturate.png", GUI_renderer));
    sSlider->setDrawLine(false);
    sSlider->change_cmd = [=](GUI_Slider *s, float f){
        int saturate = (int)(f * 100);
        char c[16];
        sprintf(c, "S:    %d", saturate);
        sLabel->setTitle(c);
        __guiColor.setSaturation(f*255);
        
        if(onColorChange_func)
            onColorChange_func(this, __guiColor.getHex());
    };
    sSlider->setValue(1.0);
    
    bSlider->setBackground(GUI_LoadDITexture("gui3/icon-color-brightness.png", GUI_renderer));
    bSlider->setDrawLine(false);
    bSlider->change_cmd = [=](GUI_Slider *s, float f){
        int brightness = (int)(f * 100);
        char c[16];
        sprintf(c, "B:    %d", brightness);
        bLabel->setTitle(c);
        __guiColor.setBrightness(f*255);
        
        if(onColorChange_func)
            onColorChange_func(this, __guiColor.getHex());
    };
    bSlider->setValue(1.0);
    
    __guiColor = GUI_Color_<uint8_t>::fromHsb(hSlider->getValue()*255, sSlider->getValue()*255, bSlider->getValue()*255);
    __hueColor = __guiColor;
    
    display_cmd = [](GUI_View* p){
        SDL_Color c = {__guiColor.r, __guiColor.g, __guiColor.b, __guiColor.a};
        GUI_FillCircle(p->tw_area.w/4, p->tw_area.w/4, p->tw_area.w/4, c);
    };
    
    //    GUI_SliderHorizontal *hs = new GUI_SliderHorizontal(this, 100, 10, 50, 300);
    //    hs->setIcon(GUI_LoadDITexture("gui3/437-minus.png", GUI_renderer), GUI_LoadDITexture("gui3/436-plus.png", GUI_renderer));
    //    hs->iconColor = {0, 0, 0, 128};
}


GUI_Color_<uint8_t> GUI_ColorPickerHSB::getGUI_Color(){
    return __guiColor;
}

void GUI_ColorPickerHSB::setGUI_Color(GUI_Color_<uint8_t> &c){
    __guiColor = c;
    hSlider->setValue(__guiColor.getHue()/255);
    sSlider->setValue(__guiColor.getSaturation()/255);
    bSlider->setValue(__guiColor.getBrightness()/255);
}
