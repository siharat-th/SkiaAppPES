//
// Created by Jimmy on 3/7/2016 AD.
//

#include "GUI_ToolBar.h"
#include "GUI_Button.h"
#include "GUI_EditText.h"
#include "GUI_Color.h"
#include "GUI_ButtonFontAwesome.hpp"

GUI_ToolBar::GUI_ToolBar(GUI_View *parent, int bar_type, void(*cmd)(GUI_ToolBar *, int, void *), int h) :
    GUI_View(parent, "ToolBar", 0, 0, -1, h) {

    if (bar_type == GUI_BAR_TOP) {
        setAlignment(GUI_ALIGN_TOP);
    } else if (bar_type == GUI_BAR_BOTTOM) {
        setAlignment(GUI_ALIGN_BOTTOM);
    } else {
        setAlignment(GUI_ALIGN_TOP);
    }

    action_cmd = cmd;
    setBorder(0);
    setLayout(GUI_LAYOUT_HORIZONTAL);
    setPadding(0, 5, 0, 5);
    dragable = false;

    bgcol = sdl_color(GUI_Color_Indigo);
    titleColor = cWhite;
    _bar_type = bar_type;
}

GUI_ToolBar::~GUI_ToolBar() {
}

GUI_ButtonFontAwesome * GUI_ToolBar::addAwesomeButton(std::string title, int icon_id, int align) {
    GUI_ButtonFontAwesome *bttn = GUI_ButtonFontAwesome::create(this, icon_id, title.c_str(),
                                                                0, 0, 32, 32, cWhite,
                                                                GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, GUI_FONT_TITLE);
    bttn->bgColor = cClear;
    bttn->setBorder(1);
    bttn->parent->setAlignment(align);
    bttn->setAlignment(GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER);

    return bttn;
}

GUI_Button * GUI_ToolBar::addButton(std::string title, std::string icon_fn, int tag, int align, bool gap) {
    GUI_Button *bt = new GUI_Button(this, title.c_str(), 0, 0, title.length() > 0 ? 0 : 32, 32, cBlack,
                                    GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL, [](GUI_Button *bt) {
        GUI_Log("Button: %i\n", bt->tag);
        GUI_ToolBar *tb = (GUI_ToolBar *)bt->parent;

        if (tb->action_cmd) {
            tb->action_cmd(tb, bt->tag, NULL);
        }
    });

    bt->tag = tag;
    bt->setAlignment(align | GUI_ALIGN_VCENTER);
    bt->bgcol = sdl_color(0x00000000);
    bt->setLayout(GUI_LAYOUT_VERTICAL);

    if (title.length() > 0) {
        bt->setPadding(0, 5, 0, 0);
    }

    if (gap) {
        if (align & GUI_ALIGN_RIGHT)
            bt->setMargin(0, 5, 0, 0);
        else
            bt->setMargin(0, 0, 0, 5);
    }

    if (icon_fn.length() > 0) {
        SDL_Texture *tx = GUI_LoadDITexture(icon_fn, GUI_renderer);
        bt->setIcon(tx);
    }

    bt->updateSubLayout();

    return bt;
}

GUI_Label * GUI_ToolBar::addLabel(std::string title, std::string icon_fn, int align) {
    GUI_Label *lb = new GUI_Label(this, title.c_str(), 0, 0, 0, 0, cBlack, GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER, GUI_FONT_TITLE);
    lb->tag = tag;
    lb->setAlignment(align | GUI_ALIGN_VCENTER);
    lb->bgcol = cClear;

    if (title.length() > 0)
        lb->setPadding(0, 5, 0, 0);

    if (icon_fn.length() > 0) {
        SDL_Texture *tx = GUI_LoadDITexture(icon_fn, GUI_renderer);
        lb->setIcon(tx);
    }

    return lb;
}

GUI_EditText * GUI_ToolBar::addEditText(std::string title, int x, int y, int w, int h, int tag, int align, bool gap) {
    GUI_EditText *et = new GUI_EditText(this, title.c_str(), x, y, w, h, cBlack, GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER, GUI_FONT_TITLE, [](GUI_EditText *et) {});

    et->tag = tag;
    updateLayout();

    return et;
}

// pom
void GUI_ToolBar::clear(GUI_Rect *r) {
    GUI_View::clear(r);

    if (corner && bgcol.a != 0) {
        if (_bar_type == GUI_BAR_TOP)
            GUI_FillRect(0, tw_area.h - corner * GUI_scale, getPhysicalWidth(), corner * GUI_scale, bgcol);
        else if (_bar_type == GUI_BAR_BOTTOM)
            GUI_FillRect(0, 0, getPhysicalWidth(), corner * GUI_scale, bgcol);
    }
}
