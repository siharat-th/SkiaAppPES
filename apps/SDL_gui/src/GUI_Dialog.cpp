//
//  GUI_Dialog.cpp
//  PES
//
//  Created by Panutat Tejasen on 11/25/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "GUI_Dialog.hpp"

GUI_Dialog::GUI_Dialog(GUI_View *parent, const char *title, int x, int y, int w, int h, bool(*cmd)(GUI_Dialog *, int, void *)) : GUI_View(parent, title, x, y, w, h, cWhite) {
    dialog_ID = (uint64_t)this;
    on_close = NULL;
    setAlignment(GUI_ALIGN_ABSOLUTE);
    bgcol = {227, 227, 227, 255};
    borderColor = {0, 0, 0, 255};
    setBorder(0);
    
    GUI_PostMessage(GUI_EventInitDialog, 0, 0, this, NULL);
}

GUI_Dialog::~GUI_Dialog() {
}

void GUI_Dialog::close() {
    if( on_close ) {
        on_close( this );
    }
    GUI_View::close();
}
void GUI_Dialog::InitDialog() {
}

bool GUI_Dialog::handleEvents(SDL_Event *ev) {
    SDL_UserEvent *userEvent = (SDL_UserEvent *)ev;
    if (ev->type == GUI_EventInitDialog) {
        GUI_Dialog *pDlg = (GUI_Dialog *)userEvent->data1;
        if( pDlg->dialog_ID == dialog_ID )
        {
            this->InitDialog();
            return true;
        }
    } else {
        return GUI_View::handleEvents(ev);
    }
    
    return false;
}

void GUI_Dialog::setCenter(int x, int y) {
    int w = GUI_windowWidth * GUI_scale;
    int h = GUI_windowHeight * GUI_scale;
    
    if (parent) {
        w = parent->getPhysicalWidth();
        h = parent->getPhysicalHeight();
    }
    
    int diffx = (w - this->getPhysicalWidth()) / 2 - this->topleft.x;
    int diffy = (h - this->getPhysicalHeight()) / 2 - this->topleft.y;
    
    move(diffx, diffy);
}
