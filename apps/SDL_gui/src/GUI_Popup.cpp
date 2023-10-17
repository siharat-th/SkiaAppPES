//
// Created by Jimmy on 3/7/2016 AD.
//
#include "SDL_gui.h"
#include "GUI_Popup.h"
#include "GUI_Button.h"

GUI_Popup::GUI_Popup(GUI_View *parent, const char *title, int x, int y, int w, int h, bool(*cmd)(GUI_Popup *, int, void *)) :
    GUI_View(parent, title, x, y, w, h, cWhite),
    labelTitle(NULL) {
    
    GUI_SetModal(this, true);

    action_cmd = cmd;

    setLayout(GUI_LAYOUT_VERTICAL);

    labelTitle = new GUI_Label(this, title, 0, 0, -1, 36, cBlack, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER);
    labelTitle->setAlignment(GUI_ALIGN_TOP);
    labelTitle->bgcol = sdl_color(0x80FFFF30);

    labelText = new GUI_Label(this, "Message", 0, 0, -1, -1, cBlack, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER);
    labelText->setAlignment(GUI_ALIGN_TOP | GUI_ALIGN_LEFT);
    labelText->bgcol = cWhite;


    GUI_Label *lb = new GUI_Label(this, "", 0, 0, -1, 50, cBlack, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER);

    lb->setAlignment(GUI_ALIGN_BOTTOM);
    lb->bgcol = sdl_color(0x80FFFF30);
    lb->setLayout(GUI_LAYOUT_HORIZONTAL);

    GUI_Button *bt = new GUI_Button(lb, "OK", 0, 0, 80, 32, cBlack, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, 1, [](GUI_Button *bt) {
        bt->tag = 0;
        GUI_Log("Button: %i\n", bt->tag);
        GUI_Popup *tb = (GUI_Popup *)(bt->parent->parent);

        if (tb->action_cmd) {
            tb->action_cmd(tb, bt->tag, NULL);
        } else {
            if (bt->tag == 0) {
                tb->close();
            }
        }
    });

    bt->setAlignment(GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER);
    bt->setBorder(1);
    bt->corner = 10;
    bt->bgcol = sdl_color(0x80FFFFC0);

    centerToParent();
}

GUI_Popup::~GUI_Popup() {
}

GUI_MessageBox_OLD::GUI_MessageBox_OLD(GUI_View*p, const char *title, const char *message)
    : GUI_Popup(p, title) {
    labelText->setTitle(message);
}

GUI_MessageBox_OLD::~GUI_MessageBox_OLD() {
}
