//
//  GUI_Slider.cpp
//  OSX
//
//  Created by SIHARAT THAMMAYA on 11/2/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#include "GUI_Slider.h"

#ifndef _WIN32
#pragma mark GUI_Slider
#endif
GUI_Slider::GUI_Slider(GUI_View* parent, int x, int y, int w, int h, float initValue, int r) :
    GUI_View(parent, "Slider", x, y, w, h),
    value(initValue), 
    iconLeft(NULL),
    iconRight(NULL),
    bgTexture(NULL),
    texBgColor(cWhite),
    isDown(false),
    isDrawLine(true),
    radius(r),
    start_cmd(NULL),
    change_cmd(NULL),
    end_cmd(NULL), 
    hightlight(Hightlight::HIGHTLIGHT_NONE) {
        iconColor = cWhite;
        isFocus = false;
        bgcol = cClear;

        rectLeft = {0, 0, 0, 0};
        rectRight = {0, 0, 0, 0};
        rectSlider = {0, 0, 0, 0};
        rectBgTex = {0, 0, 0, 0};
        
        rectSlider.x = 0;
        rectSlider.y = 0;
        rectSlider.w = w * GUI_scale;
        rectSlider.h = h * GUI_scale;

        setBorder(0);
}

GUI_Slider::~GUI_Slider() {
    if (iconLeft) {
        SDL_DestroyTexture(iconLeft);
    }

    if (iconRight) {
        SDL_DestroyTexture(iconRight);
    }

    if (bgTexture) {
        SDL_DestroyTexture(bgTexture);
    }
}

void GUI_Slider::draw() {
    GUI_View::draw();

    if (bgTexture)
        renderTexture(bgTexture, &rectBgTex, texBgColor);

    if (iconLeft)
        renderTexture(iconLeft, &rectLeft, iconColor);

    if (iconRight)
        renderTexture(iconRight, &rectRight, iconColor);

    int x0 = rectSlider.x;
    int x1 = rectSlider.x + rectSlider.w;
    int y = rectSlider.y + rectSlider.h / 2;
    SDL_Color col = sdl_color(0x8c8c8cff);

    if (GUI_View::_disable)
        col = sdl_color(0xE0E0E0ff);

    if (isDrawLine) {
        GUI_FillRect(x0, (int)(y - 1 * GUI_scale), rectSlider.w, (int)(2 * GUI_scale), col);

        if (hightlight == Hightlight::HIGHTLIGHT_FROM_MIN) {
            GUI_FillRect(x0, (int)(y - 1 * GUI_scale), (int)(rectSlider.w * value), (int)(2 * GUI_scale), cHightLightSelected);
        }

        if (hightlight == Hightlight::HIGHTLIGHT_FROM_MAX) {
            GUI_FillRect((int)(x0 + rectSlider.w * value), (int)(y - 1 * GUI_scale), (int)(rectSlider.w * (1 - value)), (int)(2 * GUI_scale), cHightLightSelected);
        }
    }

    int cx = (int)(x0 + (x1 - x0) * value);
    cx = CLAMP(cx, x0 + radius*GUI_scale, x1 - radius * GUI_scale);
    int cy = y;

    GUI_FillCircle(cx, cy, radius * GUI_scale, col);
    GUI_FillCircle(cx, cy, (radius - 2) * GUI_scale, cWhite);
}

void GUI_Slider::setValue(float v, bool invoke_cb) {
    bool hasChange = fabs(v - value) > 0.001;
    value = v;
    value = (float)CLAMP(v, 0.0, 1.0);

    if (change_cmd && hasChange && invoke_cb)
        change_cmd(this, value);
}

float GUI_Slider::getValue() {
    return value;
}

void GUI_Slider::setIcon(SDL_Texture *icoLeft, SDL_Texture *icoRight) {
    iconLeft = icoLeft;
    iconRight = icoRight;

    SDL_QueryTexture(iconLeft, NULL, NULL, &rectLeft.w, &rectLeft.h);
    SDL_QueryTexture(iconRight, NULL, NULL, &rectRight.w, &rectRight.h);

    rectLeft.x = 0;
    rectLeft.y = (tw_area.h - rectLeft.h) / 2;

    rectRight.x = tw_area.w - rectRight.w;
    rectRight.y = (tw_area.h - rectRight.h) / 2;

    int pad = radius * GUI_scale;
    rectSlider.x = rectLeft.w + pad;
    rectSlider.y = 0;
    rectSlider.w = tw_area.w - rectLeft.w - rectRight.w - pad * 2;
    rectSlider.h = tw_area.h;

    if (bgTexture) {
        rectBgTex.x = rectSlider.x + rectSlider.w / 2 - rectBgTex.w / 2;
        rectBgTex.y = rectSlider.y + rectSlider.h / 2 - rectBgTex.h / 2;
    }
}

void GUI_Slider::setBackground(SDL_Texture *bgtex) {
    bgTexture = bgtex;

    SDL_QueryTexture(bgTexture, NULL, NULL, &rectBgTex.w, &rectBgTex.h);
    rectBgTex.x = rectSlider.x + rectSlider.w / 2 - rectBgTex.w / 2;
    rectBgTex.y = rectSlider.y + rectSlider.h / 2 - rectBgTex.h / 2;
}

bool GUI_Slider::handleEvents(SDL_Event *ev) {
    if (!isVisible())
        return false;

    if (__handleInputEvent == false && ev->type != GUI_EventPaint) {
        return true;
    }

    static Uint32 mouseTime;
    static float startValue = -1;

    switch (ev->type) {
        case SDL_MOUSEBUTTONDOWN:
        {
            if (GUI_View::_disable)
                break;

            SDL_MouseButtonEvent e = ev->button;
            int x = (int)(e.x*GUI_mouseScale);
            int y = (int)(e.y*GUI_mouseScale);
            GUI_mouseCapturedWindow = (GUI_View *)this;
            mouseTime = SDL_GetTicks();

            SDL_Rect rb;
            rb.x = (int)(tw_area.x + rectSlider.x + (rectSlider.w*value) - radius * 3 * GUI_scale);
            rb.y = tw_area.y + rectSlider.y + rectSlider.h / 2 - radius * 2 * GUI_scale;
            rb.w = radius * 6 * GUI_scale;
            rb.h = radius * 4 * GUI_scale;
            SDL_Point pt;
            pt.x = x;
            pt.y = y;

            if (SDL_PointInRect(&pt, &rb)) {
                isDown = true;
                isFocus = true;
                int dx = x - (tw_area.x + rectSlider.x);
                float ratio = (dx / (float)rectSlider.w);

                if (start_cmd)
                    start_cmd(this, ratio);

                startValue = getValue();
                setValue(ratio);
                return true;
            }
            break;
        }

        case SDL_MOUSEMOTION:
        {
            if (GUI_mouseCapturedWindow == this) {
                if (isFocus) {
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);

                    if (hitTest(x, y)) {
                        int dx = x - (tw_area.x + rectSlider.x);
                        float ratio = (dx / (float)rectSlider.w);
                        setValue(ratio);
                        isDown = true;
                        return true;
                    } else {
                        SDL_Rect rb;
                        rb.x = (int)(tw_area.x + rectSlider.x + (rectSlider.w*value) - radius * 3 * GUI_scale);
                        rb.y = tw_area.y + rectSlider.y + rectSlider.h / 2 - radius * 2 * GUI_scale;
                        rb.w = radius * 6 * GUI_scale;
                        rb.h = radius * 4 * GUI_scale;
                        SDL_Point pt;
                        pt.x = x;
                        pt.y = y;

                        if (SDL_PointInRect(&pt, &rb)) {
                            isDown = true;
                            isFocus = true;
                            int dx = x - (tw_area.x + rectSlider.x);
                            float ratio = (dx / (float)rectSlider.w);
                            setValue(ratio);
                            return true;
                        } else {
                            isFocus = false;
                            isDown = false;
                            GUI_mouseCapturedWindow = NULL;

                            if (end_cmd)
                                end_cmd(this, getValue());
                        }
                    }
                }
            }

            break;
        }

        case SDL_MOUSEBUTTONUP:
        {
            if (GUI_mouseCapturedWindow == this) {
                GUI_mouseCapturedWindow = NULL;
                isDown = false;
                isFocus = false;

                if (SDL_GetTicks() - mouseTime <= 200) {
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);
                    int dy = abs(y - (tw_area.y + rectSlider.y + rectSlider.h / 2));

                    if (dy < radius*GUI_scale) {
                        int dx = x - (tw_area.x + rectSlider.x);
                        float ratio = (dx / (float)rectSlider.w);
                        setValue(ratio);
                        GUI_LogNotice("Slider immediate end") << getValue();

                        if (end_cmd)
                            end_cmd(this, getValue());

                        return true;
                    }
                }

                float delta = getValue() - startValue;

                if (startValue == -1)
                    delta = 0;

                GUI_LogNotice("Slider end drag") << getValue() << "delta=" << delta;
                startValue = getValue();

                if (end_cmd && fabs(delta) > 0.01)
                    end_cmd(this, getValue());

                return false;
            }
            break;
        }

        default:
        {
            GUI_View::handleEvents(ev);
            break;
        }
    }

    return false;
}

void GUI_Slider::setDrawLine(bool bDrawLine) {
    isDrawLine = bDrawLine;
}

void GUI_Slider::renderTexture(SDL_Texture *tex, SDL_Rect *dst, SDL_Color color) {
    SDL_SetTextureColorMod(tex, color.r, color.g, color.b);

    if (color.a < 0xff)
        SDL_SetTextureAlphaMod(tex, color.a);

    SDL_RenderCopy(GUI_renderer, tex, NULL, dst);
}

#ifndef _WIN32
#pragma mark GUI_SliderHorizontal
#endif

GUI_SliderHorizontal::GUI_SliderHorizontal(GUI_View* parent, int x, int y, int w, int h, float initValue, int radius) :
    GUI_Slider(parent, x, y, w, h, initValue, radius),
    iconBottom(NULL),
    iconTop(NULL),
    radius(radius) {
    isFocus = false;
    isDown = false;

    rectBottom = {0, 0, 0, 0};
    rectTop = {0, 0, 0, 0};

    rectSlider.x = 0;
    rectSlider.y = 0;
    rectSlider.w = w * GUI_scale;
    rectSlider.h = h * GUI_scale;
}

GUI_SliderHorizontal::~GUI_SliderHorizontal() {
    if (iconBottom) {
        SDL_DestroyTexture(iconBottom);
    }

    if (iconTop) {
        SDL_DestroyTexture(iconTop);
    }
}

void GUI_SliderHorizontal::draw() {
    GUI_View::draw();

    if (iconTop)
        renderTexture(iconTop, &rectTop, iconColor);

    if (iconBottom)
        renderTexture(iconBottom, &rectBottom, iconColor);

    int y0 = rectSlider.y;
    int y1 = rectSlider.y + rectSlider.h;
    int x = rectSlider.x + rectSlider.w / 2;

    GUI_FillRect(x - 1 * GUI_scale, y0, (int)(2 * GUI_scale), rectSlider.h, sdl_color(0x8c8c8cff));

    int cy = (int)(y0 + (y1 - y0) * getValue());
    cy = CLAMP(cy, y0 + radius * GUI_scale, y1 - radius * GUI_scale);
    int cx = x;

    GUI_FillCircle(cx, cy, radius * GUI_scale, sdl_color(0x8c8c8cff));
    GUI_FillCircle(cx, cy, (radius - 2) * GUI_scale, cWhite);
}

bool GUI_SliderHorizontal::handleEvents(SDL_Event *ev) {
    if (!isVisible())
        return false;

    if (__handleInputEvent == false && ev->type != GUI_EventPaint) {
        return true;
    }

    static Uint32 mouseTime;

    switch (ev->type) {
        case SDL_MOUSEBUTTONDOWN:
        {
            SDL_MouseButtonEvent e = ev->button;
            int x = (int)(e.x * GUI_mouseScale);
            int y = (int)(e.y * GUI_mouseScale);
            GUI_mouseCapturedWindow = (GUI_View *)this;
            mouseTime = SDL_GetTicks();

            SDL_Rect rb;
            rb.y = (int)(tw_area.y + rectSlider.y + (rectSlider.h*getValue()) - radius * 3 * GUI_scale);
            rb.x = tw_area.x + rectSlider.x + rectSlider.w / 2 - radius * 2 * GUI_scale;
            rb.h = radius * 6 * GUI_scale;
            rb.w = radius * 4 * GUI_scale;
            SDL_Point pt;
            pt.x = x;
            pt.y = y;

            if (SDL_PointInRect(&pt, &rb)) {
                isDown = true;
                isFocus = true;
                int dy = y - (tw_area.y + rectSlider.y);
                float ratio = (dy / (float)rectSlider.h);

                if (start_cmd)
                    start_cmd(this, ratio);

                setValue(ratio);
                return true;
            }
            break;
        }

        case SDL_MOUSEMOTION:
        {
            if (GUI_mouseCapturedWindow == this) {
                if (isFocus) {
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);

                    if (hitTest(x, y)) {
                        int dy = y - (tw_area.y + rectSlider.y);
                        float ratio = (dy / (float)rectSlider.h);
                        setValue(ratio);
                        isDown = true;
                        return true;
                    } else {
                        SDL_Rect rb;
                        rb.y = (int)(tw_area.y + rectSlider.y + (rectSlider.h*getValue()) - radius * 3 * GUI_scale);
                        rb.x = tw_area.x + rectSlider.x + rectSlider.w / 2 - radius * 2 * GUI_scale;
                        rb.h = radius * 6 * GUI_scale;
                        rb.w = radius * 4 * GUI_scale;
                        SDL_Point pt;
                        pt.x = x;
                        pt.y = y;

                        if (SDL_PointInRect(&pt, &rb)) {
                            isDown = true;
                            isFocus = true;
                            int dy = y - (tw_area.y + rectSlider.y);
                            float ratio = (dy / (float)rectSlider.h);
                            setValue(ratio);
                            return true;
                        } else {
                            isFocus = false;
                            isDown = false;
                            GUI_mouseCapturedWindow = NULL;

                            if (end_cmd)
                                end_cmd(this, getValue());
                        }
                    }
                }
            }

            break;
        }

        case SDL_MOUSEBUTTONUP:
        {
            if (GUI_mouseCapturedWindow == this) {
                GUI_mouseCapturedWindow = NULL;
                isDown = false;
                isFocus = false;

                if (SDL_GetTicks() - mouseTime <= 200) {
                    SDL_MouseButtonEvent e = ev->button;
                    int x = (int)(e.x*GUI_mouseScale);
                    int y = (int)(e.y*GUI_mouseScale);
                    int dx = abs(x - (tw_area.x + rectSlider.x + rectSlider.w / 2));
                    if (dx < radius*GUI_scale) {
                        int dy = y - (tw_area.y + rectSlider.y);
                        float ratio = (dy / (float)rectSlider.h);
                        setValue(ratio);

                        if (end_cmd)
                            end_cmd(this, getValue());

                        return true;
                    }
                }

                if (end_cmd)
                    end_cmd(this, getValue());

                return false;
            }
            break;
        }

        default:
        {
            GUI_View::handleEvents(ev);
            break;
        }
    }

    return false;
}

void GUI_SliderHorizontal::setIcon(SDL_Texture *icoBottom, SDL_Texture *icoTop) {
    iconBottom = icoBottom;
    iconTop = icoTop;

    SDL_QueryTexture(iconBottom, NULL, NULL, &rectBottom.w, &rectBottom.h);
    SDL_QueryTexture(iconTop, NULL, NULL, &rectTop.w, &rectTop.h);

    rectTop.y = 0;
    rectTop.x = (tw_area.w - rectTop.w) / 2;

    rectBottom.y = tw_area.h - rectBottom.h;
    rectBottom.x = (tw_area.w - rectBottom.w) / 2;

    int pad = radius*GUI_scale;
    rectSlider.y = rectTop.h + pad;
    rectSlider.x = 0;
    rectSlider.h = tw_area.h - rectTop.h - rectBottom.h - pad * 2;
    rectSlider.w = tw_area.w;
}
