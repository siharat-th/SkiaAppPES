//
//  GUI_Switch.h
//  OSX
//
//  Created by SIHARAT THAMMAYA on 10/29/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#ifndef GUI_Switch_h
#define GUI_Switch_h

#include <SDL.h>
#include "GUI_View.h"

class GUI_Switch : public GUI_View {
    bool bSwitchOn;

public:
    GUI_Switch(GUI_View *parent, const char *title, int x = 0, int y = 0, int radius = 12,
               SDL_Color color = cWhite, int align = GUI_ALIGN_LEFT | GUI_ALIGN_TOP, std::function<void(GUI_Switch *, bool)> action_cmd = NULL);

    virtual ~GUI_Switch() override;

    SDL_Color bgColor_switchoff;
    SDL_Color bgColor_switchon;
    SDL_Color borderColor_switchoff;

    virtual void draw() override;
    virtual bool handleEvents(SDL_Event *ev) override;

    void setSwitchOn();
    void setSwitchOff();
    void toggleSwitch();
    bool isSwitchOn();

    bool isDown;
    //bool isFocus;
    int radius;
    std::function<void(GUI_Switch *, bool)> action_cmd;
};

#endif /* GUI_Switch_h */
