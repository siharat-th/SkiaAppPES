//
//  GUI_ColorPicker.hpp
//  FontWorks
//
//  Created by SIHARAT THAMMAYA on 12/31/2559 BE.
//  Copyright © 2559 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef GUI_ColorPicker_hpp
#define GUI_ColorPicker_hpp

#include <stdio.h>
#include <SDL.h>
#include "SDL_gui.h"
#include <limits>
#include <functional>

//class GUI_Color;

//typedef GUI_Color ofColor;

// MARK: GUI_Color
template<typename PixelType>
class GUI_Color_{
    
public:
    GUI_Color_():
    r(limit()),
    g(limit()),
    b(limit()),
    a(limit()){};
    
    GUI_Color_(float red, float green, float blue, float alpha = limit());
    GUI_Color_(float gray, float alpha = limit());
    
    template<typename SrcType>
    GUI_Color_(const GUI_Color_<SrcType>& color);
    
    GUI_Color_(const GUI_Color_<PixelType>& color, float alpha);
    
    
    static GUI_Color_<PixelType> fromHsb(float hue, float saturation, float brightness, float a=limit());
    static GUI_Color_<PixelType> fromHex(int hexColor, float alpha = limit());
    
    union {
        struct {
            PixelType r;
            PixelType g;
            PixelType b;
            PixelType a;
        };
        PixelType v[4];
    };
    
    void set(float red, float green, float blue, float alpha = limit());
    void set(float gray, float alpha = limit());
    
    void setHex(int hexColor, float alpha = limit());
    
    void setHue(float hue);
    void setHueAngle(float angle);
    void setSaturation(float saturation);
    void setBrightness(float brightness);
    
    void setHsb(float hue,
                float saturation,
                float brightness,
                float alpha = limit());
    
    GUI_Color_<PixelType>& clamp();
    
    int getHex() const;
    float getHue() const;
    float getHueAngle()  const;
    float getSaturation()  const;
    float getBrightness()  const;
    float getLightness() const; // HSL
    
    void getHsb(float& hue, float& saturation, float& brightness) const;
    
    static float limit();
    
    template<typename SrcType>
    GUI_Color_<PixelType>& operator = (const GUI_Color_<SrcType>& color);
    
    GUI_Color_<PixelType>& operator = (const float& value);
    
    const PixelType& operator [] (std::size_t n) const;
    PixelType& operator [] (std::size_t n);
    
    
private:
    template<typename SrcType>
    void copyFrom(const GUI_Color_<SrcType>& mom);
};

//typedef GUI_Color_<unsigned char> GUI_Color;
typedef GUI_Color_<unsigned short> GUI_ShortColor;
typedef GUI_Color_<float> GUI_FloatColor;

template<typename PixelType>
template<typename SrcType>
GUI_Color_<PixelType>::GUI_Color_(const GUI_Color_<SrcType> & mom){
    copyFrom(mom);
}

template<typename PixelType>
template<typename SrcType>
GUI_Color_<PixelType> & GUI_Color_<PixelType>::operator=(const GUI_Color_<SrcType> & mom){
    copyFrom(mom);
    return *this;
}

template<typename PixelType>
template<typename SrcType>
void GUI_Color_<PixelType>::copyFrom(const GUI_Color_<SrcType> & mom){
    const float srcMax = mom.limit();
    const float dstMax = limit();
    const float factor = dstMax / srcMax;
    
    if(sizeof(SrcType) == sizeof(float)) {
        // coming from float we need a special case to clamp the values
        for(int i = 0; i < 4; i++){
            v[i] = CLAMP(mom[i], 0, 1) * factor;
        }
    } else{
        // everything else is a straight scaling
        for(int i = 0; i < 4; i++){
            v[i] = mom[i] * factor;
        }
    }
}


class GUI_Slider;
// MARK: GUI_ColorPickerHSB
class GUI_ColorPickerHSB : public GUI_View{
public:
    GUI_ColorPickerHSB(GUI_View *parent, int x=0, int y=0, int w=-1, int h=-1);
    
    std::function<void(GUI_ColorPickerHSB* sender, int hexColor)> onColorChange_func;
    
    GUI_Color_<uint8_t> getGUI_Color();
    void setGUI_Color(GUI_Color_<uint8_t> &c);
    
private:
    GUI_Slider *hSlider, *sSlider, *bSlider;
};


#endif /* GUI_ColorPicker_hpp */
