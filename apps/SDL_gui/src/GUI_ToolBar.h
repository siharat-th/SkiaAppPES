//
// Created by Jimmy on 3/7/2016 AD.
//

#ifndef GUI_TOOLBAR_GUI_TOOLBAR_H
#define GUI_TOOLBAR_GUI_TOOLBAR_H

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include "GUI_View.h"
#include "GUI_Button.h"
#include "GUI_EditText.h"
#include "GUI_ButtonFontAwesome.hpp"

#define GUI_BAR_TOP         1
#define GUI_BAR_BOTTOM      2
#define GUI_BAR_LEFT        4
#define GUI_BAR_RIGHT       8

class GUI_ButtonFontAwesome;

class GUI_ToolBar : public GUI_View {
public:
    GUI_ToolBar(GUI_View *parent, int bar_type, void(*act_cmd)(GUI_ToolBar *, int, void *) = NULL, int h = 44);
    virtual ~GUI_ToolBar() override;

    GUI_Label* addLabel(std::string title, std::string icon_fn, int align);
    GUI_Button* addButton(std::string title, std::string icon_fn, int tag, int align, bool gap = false);
    GUI_EditText* addEditText(std::string title, int x = 0, int y = 0, int w = 200, int h = 32, int tag = 0, int align = GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, bool gap = false);

    GUI_ButtonFontAwesome* addAwesomeButton(std::string title, int icon_id, int align);

    void(*action_cmd)(GUI_ToolBar *, int, void *);

    // pom
    virtual void clear(GUI_Rect *r = 0) override;
    int _bar_type;
};

#endif //GUI_TOOLBAR_GUI_TOOLBAR_H
