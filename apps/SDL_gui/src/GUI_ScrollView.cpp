//
//  GUI_ScrollView.cpp
//  OSX
//
//  Created by Panutat Tejasen on 5/3/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#include "GUI_ScrollView.h"


GUI_ScrollView::GUI_ScrollView(GUI_View *parent, const char *title, int x, int y, int width, int height) :
    GUI_View(parent, title, x, y, width, height),
    _innerView(NULL) {
    this->bgcol = sdl_color(0xE8E8E8FF);
    _innerView = new GUI_View(this, "_inner", 0, 0, -1, 0);
    _innerView->setLayout(GUI_LAYOUT_VERTICAL);
    _innerView->setBorder(0);
}

GUI_ScrollView::~GUI_ScrollView() {
    if (_innerView) {
        _innerView->remove_all_children();
    }
}

void GUI_ScrollView::add_child(GUI_View *child) {
    if (_innerView) {
        _innerView->add_child(child);
        child->parent = _innerView;
    } else {
        GUI_View::add_child(child);
    }
}

void GUI_ScrollView::remove_child(GUI_View *child) {
    if (child == _innerView) {
        GUI_View::remove_child(child);
        _innerView = NULL;
    } else {
        if (_innerView)
            _innerView->remove_child(child);
    }
    updateLayout();
}

void GUI_ScrollView::remove_all_children() {
    if (_innerView)
        _innerView->remove_all_children();
}

bool GUI_ScrollView::handleEvents(SDL_Event *ev) {
    if (isVisible() == false)
        return false;

    static int mouse_x = 0;
    static int mouse_y = 0;
    static int last_x = 0;
    static int last_y = 0;
    static bool bMoving = false;
    static SDL_Event evx;

    switch (ev->type) {
        case SDL_MOUSEBUTTONDOWN:
        {
            SDL_MouseButtonEvent e = ev->button;
            mouse_x = (int)(e.x * GUI_mouseScale);
            mouse_y = (int)(e.y * GUI_mouseScale);
            GUI_mouseCapturedWindow = (GUI_View *)this;
            evx.type = ev->type;
            evx.button = ev->button;
            bMoving = false;
            return false;
        }

        case SDL_MOUSEMOTION:
        {
            if (GUI_mouseCapturedWindow == this) {
                SDL_MouseButtonEvent e = ev->button;
                int x = (int)(e.x * GUI_mouseScale);
                int y = (int)(e.y * GUI_mouseScale);

                if (!bMoving) {
                    if (((y - mouse_y) < -5) || ((y - mouse_y) > 5)) {
                        _innerView->move(0, y - mouse_y);

                        if (_innerView->topleft.y > 0) {
                            _innerView->move(0, 0 - _innerView->topleft.y);
                        } else if ((_innerView->tw_area.h > tw_area.h) && (_innerView->topleft.y + _innerView->tw_area.h < tw_area.h)) {
                            _innerView->move(0, tw_area.h - (_innerView->topleft.y + _innerView->tw_area.h));
                        } else if ((_innerView->tw_area.h <= tw_area.h) && (_innerView->topleft.y < 0)) {
                            _innerView->move(0, 0 - _innerView->topleft.y);
                        }

                        bMoving = true;
                    }
                } else {
                    _innerView->move(0, y - last_y);

                    if (_innerView->topleft.y > 0) {
                        _innerView->move(0, 0 - _innerView->topleft.y);
                    } else if ((_innerView->tw_area.h > tw_area.h) && (_innerView->topleft.y + _innerView->tw_area.h < tw_area.h)) {
                        _innerView->move(0, tw_area.h - (_innerView->topleft.y + _innerView->tw_area.h));
                    } else if ((_innerView->tw_area.h <= tw_area.h) && (_innerView->topleft.y < 0)) {
                        _innerView->move(0, 0 - _innerView->topleft.y);
                    }
                }

                last_x = x;
                last_y = y;

                GUI_Log("Parent Clip: %i, %i, %i, %i\n", this->tw_area.x, this->tw_area.y, this->tw_area.w, this->tw_area.h);
                GUI_Log("Inner Clip: %i, %i, %i, %i\n", _innerView->tw_area.x, _innerView->tw_area.y, _innerView->tw_area.w, _innerView->tw_area.h);
            } else
                GUI_View::handleEvents(ev);

            return false;
        }

        case SDL_MOUSEBUTTONUP:
        {
            if (GUI_mouseCapturedWindow == this) {
                GUI_mouseCapturedWindow = NULL;

                SDL_MouseButtonEvent e = ev->button;
                int y = (int)(e.y * GUI_mouseScale);

                if (!bMoving && ((y - mouse_y) > -5) && ((y - mouse_y) < 5)) {
                    GUI_View::handleEvents(&evx);
                    GUI_View::handleEvents(ev);
                }
            } else
                GUI_View::handleEvents(ev);

            bMoving = false;
            return false;
        }

        default:
        {
            GUI_View::handleEvents(ev);
        }
    }

    return false;
}
