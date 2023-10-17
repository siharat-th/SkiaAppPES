//
//  GUI_ScrollView.hpp
//  OSX
//
//  Created by Panutat Tejasen on 5/3/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#ifndef GUI_ScrollView_hpp
#define GUI_ScrollView_hpp

#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "GUI_View.h"
#include <string>

class GUI_ScrollView : public GUI_View {
protected:
    GUI_View *_innerView;

public:
    GUI_ScrollView(GUI_View *parent, const char *title, int x, int y, int width, int height);
    virtual ~GUI_ScrollView() override;

    virtual void add_child(GUI_View* child) override;
    virtual void remove_child(GUI_View* child) override;
    virtual void remove_all_children() override;

    virtual bool handleEvents(SDL_Event *ev) override;
};

#endif /* GUI_ScrollView_hpp */
