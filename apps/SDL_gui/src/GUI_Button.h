//
// Created by Jimmy on 2/15/2016 AD.
//

#ifndef GUI_LABEL_GUI_BUTTON_H
#define GUI_LABEL_GUI_BUTTON_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "GUI_View.h"
#include "GUI_Label.h"
#include <string>
#include <functional>

class GUI_Button : public GUI_Label {
public:
    static GUI_Button *create(GUI_View *parent, const char *title, int x, int y, int width, int height, std::function<void(GUI_Button*)> act_cmd);

    GUI_Button(GUI_View *parent, const char *title, int x = 0, int y = 0, int width = 0, int height = 0, SDL_Color color = cBlack, int align = GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, int fontSize = 1, std::function<void(GUI_Button*)> act_cmd = NULL);
    GUI_Button(GUI_View *parent, const char *title, bool border, std::function<void(GUI_Button*)> act_cmd = NULL);
    virtual ~GUI_Button() override;

    SDL_Color color_down;
    SDL_Color color_selected;
    bool isDown;
    //bool isFocus;
    bool _bActive;

    virtual void draw() override;
    virtual bool handleEvents(SDL_Event *ev) override;

    // pom
    std::function<void(GUI_Button*)> action_cmd;

    void setActive(bool active);
    void setActive(bool active, SDL_Color bg, SDL_Color fg);
    virtual bool isActive() override;

    // pom
    // MARK: for touch hold then release
    Uint32 touchTime, touchHoldTime;
};

#endif //GUI_LABEL_GUI_BUTTON_H
