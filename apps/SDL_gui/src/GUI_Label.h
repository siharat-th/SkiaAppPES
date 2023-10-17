//
// Created by Jimmy on 2/10/2016 AD.
//

#ifndef GUI_LABEL_GUI_LABEL_H
#define GUI_LABEL_GUI_LABEL_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "GUI_View.h"
#include <string>

class GUI_Label : public GUI_View {
protected:
    int format;
    int fontSize;

public:
    static GUI_Label *create(GUI_View *parent, const char *title, int x, int y, int width, int height, int fontSize = GUI_FONT_NORMAL);

    GUI_Label(GUI_View *parent, const char *title, int x = 0, int y = 0, int width = 0, int height = 0,
              SDL_Color color = cBlack, int align = GUI_ALIGN_LEFT | GUI_ALIGN_TOP, int fontSize = 1);
    virtual ~GUI_Label() override;

    virtual bool handleEvents(SDL_Event *ev) override;
    virtual void updateSubLayout() override;
    virtual void setPadding(int p0, int p1, int p2, int p3) override;
    virtual void setTitle(std::string t, int fontSize = -1) override;

private:
    SDL_Color _color;
};

#endif //GUI_LABEL_GUI_LABEL_H
