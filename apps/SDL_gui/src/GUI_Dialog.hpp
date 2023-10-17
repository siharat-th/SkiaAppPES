//
//  GUI_Dialog.hpp
//  PES
//
//  Created by Panutat Tejasen on 11/25/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef GUI_Dialog_hpp
#define GUI_Dialog_hpp

#include <stdio.h>
#include "SDL_gui.h"

class GUI_Dialog : public GUI_View {
public:
    GUI_Dialog(GUI_View *parent, const char *title = "", int x = 0, int y = 0, int w = 640, int h = 480, bool(*act_cmd)(GUI_Dialog *, int, void *) = NULL);
    uint64_t dialog_ID;
    virtual ~GUI_Dialog() override;
    virtual void close() override;
    bool(*action_cmd)(GUI_Dialog *, int, void *);
    void(*on_close)(GUI_Dialog *);
    void setCenter(int x, int y);
    virtual void InitDialog();
    virtual bool handleEvents(SDL_Event *ev) override;
};

#endif /* GUI_Dialog_hpp */
