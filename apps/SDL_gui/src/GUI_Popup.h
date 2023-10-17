//
// Created by Jimmy on 3/7/2016 AD.
//

#ifndef GUI_MESSGEBOX_GUP_POPUP_H
#define GUI_MESSGEBOX_GUP_POPUP_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "GUI_View.h"
#include "GUI_Label.h"
#include <string>

class GUI_Popup : public GUI_View {
protected:
    GUI_Label *labelTitle;
    GUI_Label *labelText;
    GUI_Label *labelControl;

public:
    GUI_Popup(GUI_View *parent, const char *title, int x = 0, int y = 0, int w = 320, int h = 240, bool(*act_cmd)(GUI_Popup *, int, void *) = NULL);
    virtual ~GUI_Popup() override;
    bool(*action_cmd)(GUI_Popup *, int, void *);
};

class GUI_MessageBox_OLD : public GUI_Popup {
public:
    GUI_MessageBox_OLD(GUI_View*parent, const char *title, const char *message);
    virtual ~GUI_MessageBox_OLD() override;
};

#endif //GUI_MESSGEBOX_GUP_POPUP_H
