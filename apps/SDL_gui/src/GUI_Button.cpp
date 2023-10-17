//
// Created by Jimmy on 2/15/2016 AD.
//

#include "GUI_Button.h"
#include "GUI_View.h"

GUI_Button *GUI_Button::create(GUI_View *parent, const char *title, int x, int y, int width, int height, std::function<void(GUI_Button*)> act_cmd) {
    GUI_Button *bttn = new GUI_Button(parent, title, x, y, width, height, cBlack, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL, act_cmd);
    bttn->bgcol = cWhite;
    bttn->setAlignment(GUI_ALIGN_VCENTER | GUI_ALIGN_CENTER);

    bttn->color_down = sdl_color(0xB0B0B040);

    return bttn;
}

GUI_Button::GUI_Button(GUI_View *p, const char *t, int x, int y, int w, int h, SDL_Color col, int align, int fnt, std::function<void(GUI_Button*)> act_cmd) :
    GUI_Label(p, t, x, y, w, h, col, align, fnt),

    _bActive(true),
    touchTime(-1),
    touchHoldTime(-1) {
    isDown = false;
    isFocus = false;        
    action_cmd = act_cmd;

    color_down = sdl_color(0xFFFFFF40);
}

GUI_Button::GUI_Button(GUI_View *p, const char *t, bool b, std::function<void(GUI_Button*)> act_cmd) : GUI_Label(p, t, 0, 0, 0, 0, cBlack, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, 1),
isDown(false),
_bActive(true),
touchTime(-1),
touchHoldTime(-1) {
    isFocus = false;
    action_cmd = act_cmd;
    color_down = sdl_color(0xFFFFFF40);

    if (b) {
        setPadding(4, 12, 4, 12);
        setBorder(1);
        corner = 16;
    }
}

GUI_Button::~GUI_Button() {
    touchTime = -1;
}

void GUI_Button::draw() {
    predraw();

    if (display_cmd)
        display_cmd(this);

    if (isDown) {
        if (color_down.a != 0) {
            if (corner != 0) {
                GUI_FillRoundRect(0, 0, tw_area.w, tw_area.h, corner * GUI_scale, color_down);
            } else {
                GUI_FillRect(0, 0, tw_area.w, tw_area.h, color_down);
            }
        }
    }

    // pom
    if (textureIcon) {
        if (iconColor.a != 0) {
            SDL_SetTextureColorMod(textureIcon, iconColor.r, iconColor.g, iconColor.b);
        }

        SDL_RenderCopy(GUI_renderer, textureIcon, NULL, &icon_area);
    }

    if (_border > 0) {
        GUI_Rect r = GUI_Rect(_border, _border, tw_area.w - (2 * _border), tw_area.h - (2 * _border));

        if (corner) {
            GUI_DrawRoundRect(0, 0, tw_area.w, tw_area.h, corner * GUI_scale, cBlack);
        } else {
            GUI_DrawRect(0, 0, tw_area.w, tw_area.h, cBlack);
        }

        SDL_IntersectRect(&r, &clip_area, &clip_area);

#ifdef __EMSCRIPTENx__
        float magic_y = GUI_windowHeight*GUI_scale - tw_area.y - tw_area.h;
        SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(tw_area.x + clip_area.x,
                                                         0 - magic_y + clip_area.y,
                                                         clip_area.w,
                                                         clip_area.h));
#else
        SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(clip_area.x,
                                                         clip_area.y,
                                                         clip_area.w,
                                                         clip_area.h));
#endif
    }

    if (titleTexture) {
        SDL_SetTextureColorMod(titleTexture, titleColor.r, titleColor.g, titleColor.b);
        SDL_RenderCopy(GUI_renderer, titleTexture, NULL, &title_area);
    }
}

bool GUI_Button::handleEvents(SDL_Event *ev) {
    if (isVisible() == false)
        return false;

    if (isActive() == false && ev->type != GUI_EventPaint)
        return false;

    switch (ev->type) {
#if _RPi                    
        case SDL_FINGERDOWN:
        {
            GUI_Log("*** Button Finger Down\n");

            if (isActive()) {
                GUI_Log("*** Button Active\n");
                isDown = true;
                isFocus = true;
                GUI_mouseCapturedWindow = (GUI_View *)this;
                touchTime = SDL_GetTicks(); // time in millis
                touchHoldTime = touchTime;
                return true;
            }

            return false;
        }
#endif        
        case SDL_MOUSEBUTTONDOWN:
        {
            if (isActive()) {
                isDown = true;
                isFocus = true;
                GUI_mouseCapturedWindow = (GUI_View *)this;
                touchTime = SDL_GetTicks(); // time in millis
                touchHoldTime = touchTime;
                return true;
            }

            return false;
        }
#if _RPi        
        case SDL_FINGERMOTION:
        {
            if (isActive() && isFocus) {
                SDL_TouchFingerEvent e = ev->tfinger;
                int x = (int)(e.x*GUI_windowWidth / GUI_mouseScale);
                int y = (int)(e.y*GUI_windowHeight / GUI_mouseScale);

                if (hitTest(x, y)) {
                    isDown = true;
                    return true;
                } else {
                    touchTime = -1; // reset touch time(hold) when lost focus
                    touchHoldTime = touchTime;
                    isDown = false;
                }
            }
            return false;
        }
#endif        
        case SDL_MOUSEMOTION:
        {
            if (isActive() && isFocus) {
                SDL_MouseButtonEvent e = ev->button;
                int x = (int)(e.x*GUI_mouseScale);
                int y = (int)(e.y*GUI_mouseScale);

                if (hitTest(x, y)) {
                    isDown = true;
                    return true;
                } else {
                    touchTime = -1; // reset touch time(hold) when lost focus
                    touchHoldTime = touchTime;
                    isDown = false;
                }
            }
            return false;
        }
#if _RPi        
        case SDL_FINGERUP:
        {
            GUI_Log("*** Button Finger Up\n");
            if (isActive() && isFocus) {
                GUI_Log("*** Button Active and Focus\n");
                GUI_mouseCapturedWindow = NULL;
                SDL_TouchFingerEvent e = ev->tfinger;
                int x = (int)(e.x*GUI_windowWidth / GUI_mouseScale);
                int y = (int)(e.y*GUI_windowHeight / GUI_mouseScale);

                isDown = false;
                isFocus = false;
                GUI_Log("*** HitTest %i %i\n", x, y);
                if (hitTest(x, y)) {
                    GUI_Log("*** Finger Up\n");
                    if (touchTime != -1) {
                        touchHoldTime = SDL_GetTicks() - touchTime;
                    }
                    if (action_cmd) {
                        action_cmd(this);
                        return true;
                    }
                }
            }
            return false;
        }
#endif        
        case SDL_MOUSEBUTTONUP:
        {
            if (isActive() && isFocus) {
                GUI_mouseCapturedWindow = NULL;
                SDL_MouseButtonEvent e = ev->button;
                int x = (int)(e.x*GUI_mouseScale);
                int y = (int)(e.y*GUI_mouseScale);

                isDown = false;
                isFocus = false;

                if (hitTest(x, y)) {
                    if (touchTime != -1) {
                        touchHoldTime = SDL_GetTicks() - touchTime;
                    }

                    if (action_cmd) {
                        action_cmd(this);
                        return true;
                    }
                }
            }
            return false;
        }

        default:
        {
            GUI_View::handleEvents(ev);
        }
    }

    return false;
}

void GUI_Button::setActive(bool active) {
    if (active != _bActive) {
        _bActive = active;

        if (titleTexture) {
            Uint8 alphaMod = isActive() ? titleColor.a : titleColor.a / 4;
            SDL_SetTextureAlphaMod(titleTexture, alphaMod);
        }

        if (textureIcon) {
            Uint8 alphaMod = isActive() ? iconColor.a : iconColor.a / 4;
            SDL_SetTextureAlphaMod(textureIcon, alphaMod);
        }
    }
}

void GUI_Button::setActive(bool active, SDL_Color bg, SDL_Color fg) {
    _bActive = active;
    bgcol = bg;
    titleColor = fg;
    iconColor = fg;
    borderColor = fg;
}

bool GUI_Button::isActive() {
    return _bActive;
}
