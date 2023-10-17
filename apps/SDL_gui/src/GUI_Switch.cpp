//
//  GUI_Switch.cpp
//  OSX
//
//  Created by SIHARAT THAMMAYA on 10/29/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#include "GUI_Switch.h"

GUI_Switch::GUI_Switch(GUI_View *parent, const char *title, int x, int y, int radius,
                       SDL_Color color, int align, std::function<void(GUI_Switch *, bool)> act_cmd) :
    GUI_View(parent, title, x, y, (int)(radius*3.25f), radius * 2, color),
    bSwitchOn(false) {
    isDown = false;
    isFocus = false;        
    action_cmd = act_cmd;

    bgColor_switchon = {50, 206, 66, 255};
    bgColor_switchoff = borderColor_switchoff = {0, 0, 0, 64};

    corner = radius;
    this->radius = radius;
}

GUI_Switch::~GUI_Switch() {
}

void GUI_Switch::draw() {
    if (!isVisible()) return;

    if (isSwitchOn())
        bgcol = bgColor_switchon;
    else
        bgcol = bgColor_switchoff;

    predraw();

    if (display_cmd)
        display_cmd(this);

    if (isSwitchOn()) {
        GUI_FillCircle(tw_area.w - radius * GUI_scale, radius * GUI_scale, radius * GUI_scale, borderColor_switchoff);
        GUI_FillCircle(tw_area.w - radius * GUI_scale, radius * GUI_scale, (radius - 2) * GUI_scale, cWhite);
    } else {
        GUI_FillCircle(radius * GUI_scale, radius * GUI_scale, radius * GUI_scale, borderColor_switchoff);
        GUI_FillCircle(radius * GUI_scale, radius * GUI_scale, (radius - 2) * GUI_scale, cWhite);
    }
}

bool GUI_Switch::handleEvents(SDL_Event *ev) {
    if (isVisible() == false)
        return false;

    switch (ev->type) {
        case SDL_MOUSEBUTTONDOWN:
        {
            isDown = true;
            isFocus = true;
            GUI_mouseCapturedWindow = (GUI_View *)this;
            return true;
        }
        case SDL_MOUSEMOTION:
        {
            if (isFocus) {
                SDL_MouseButtonEvent e = ev->button;
                int x = (int)(e.x*GUI_mouseScale);
                int y = (int)(e.y*GUI_mouseScale);

                if (hitTest(x, y)) {
                    isDown = true;
                    return true;
                } else {
                    isDown = false;
                    isFocus = false;
                    GUI_mouseCapturedWindow = NULL;
                }
            }

            return false;
        }
        case SDL_MOUSEBUTTONUP:
        {
            if (isFocus) {
                GUI_mouseCapturedWindow = NULL;
                SDL_MouseButtonEvent e = ev->button;
                int x = (int)(e.x*GUI_mouseScale);
                int y = (int)(e.y*GUI_mouseScale);

                isDown = false;
                isFocus = false;

                if (hitTest(x, y)) {
                    toggleSwitch();

                    if (action_cmd) {
                        action_cmd(this, isSwitchOn());
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

void GUI_Switch::setSwitchOn() {
    if (!bSwitchOn) {
        bSwitchOn = true;

        if (action_cmd) {
            action_cmd(this, isSwitchOn());
        }
    }
}

void GUI_Switch::setSwitchOff() {
    if (bSwitchOn) {
        bSwitchOn = false;

        if (action_cmd) {
            action_cmd(this, isSwitchOn());
        }
    }
}

void GUI_Switch::toggleSwitch() {
    bSwitchOn = !bSwitchOn;

    if (action_cmd) {
        action_cmd(this, isSwitchOn());
    }
}

bool GUI_Switch::isSwitchOn() {
    return bSwitchOn;
}
