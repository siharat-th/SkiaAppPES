//
//  GUI_Slider.h
//  OSX
//
//  Created by SIHARAT THAMMAYA on 11/2/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#ifndef GUI_Slider_h
#define GUI_Slider_h

#include <SDL.h>
#include <functional>
#include "SDL_gui.h"

class GUI_Slider : public GUI_View {
public:
    enum class Hightlight {
        HIGHTLIGHT_NONE,
        HIGHTLIGHT_FROM_MIN,
        HIGHTLIGHT_FROM_MAX
    };

    GUI_Slider(GUI_View* parent, int x = 0, int y = 0, int w = -1, int h = -1, float initValue = 0.5f, int radius = 12);
    virtual ~GUI_Slider() override;

    virtual void draw() override;
    virtual bool handleEvents(SDL_Event *ev) override;

    void setValue(float value, bool invoke_cb = true);
    float getValue();

    void setIcon(SDL_Texture *iconLeft, SDL_Texture *iconRight);
    void setBackground(SDL_Texture *bgtex);
    void renderTexture(SDL_Texture *tex, SDL_Rect *dst, SDL_Color color = cWhite);
    void setDrawLine(bool bDrawLine);

    void setHilight(Hightlight hl) {
        hightlight = hl;
    }

    //SDL_Color iconColor;
    SDL_Color texBgColor;

    std::function<void(GUI_Slider*, float)> start_cmd;
    std::function<void(GUI_Slider*, float)> change_cmd;
    std::function<void(GUI_Slider*, float)> end_cmd;

protected:
    Hightlight hightlight;
    bool isDown; //, isFocus;

private:
    float value;
    SDL_Texture *iconLeft, *iconRight;
    SDL_Texture *bgTexture;
    SDL_Rect rectLeft, rectRight, rectSlider, rectBgTex;
    bool isDrawLine;
    int radius;
};

class GUI_SliderHorizontal : public GUI_Slider {
public:
    GUI_SliderHorizontal(GUI_View* parent, int x = 0, int y = 0, int w = -1, int h = -1, float initValue = 0.5f, int radius = 12);
    virtual ~GUI_SliderHorizontal() override;

    virtual void draw() override;
    virtual bool handleEvents(SDL_Event *ev) override;

    void setIcon(SDL_Texture *iconBottom, SDL_Texture *iconTop);
/// <summary>
/// 
/// </summary>
private:
    SDL_Texture *iconBottom, *iconTop;
    SDL_Rect rectBottom, rectTop, rectSlider;
    //bool isDown;
    int radius;
};

#endif /* GUI_Slider_h */
