//
// Created by Jimmy on 2/10/2016 AD.
//

#include "GUI_Label.h"
#include "GUI_Text.h"

GUI_Label *GUI_Label::create(GUI_View *parent, const char *title, int x, int y, int width, int height, int fontSize) {
    GUI_Label *label = new GUI_Label(parent, title, x, y, width, height, cBlack, GUI_ALIGN_VCENTER | GUI_ALIGN_LEFT, fontSize);
    label->bgcol = cWhite;
    label->setAlignment(GUI_ALIGN_VCENTER | GUI_ALIGN_LEFT);
    return label;
}

GUI_Label::GUI_Label(GUI_View *p, const char *t, int x, int y, int w, int h, SDL_Color col, int align, int fnt) : GUI_View(p, t, x, y, w, h, cClear), format(align), fontSize(fnt) {
    title_align = format;
    dragable = false;
    setBorder(0);
    titleColor = col;
    _color = col;
#if __USE_SKIA__
    createTitleImage(fnt);
#else
    createTitleTexture(fnt);
#endif
    setPadding(2, 2, 2, 2);
}

GUI_Label::~GUI_Label() {
}

void GUI_Label::setTitle(std::string t, int font_size) {
    if (font_size == -1)
        font_size = this->fontSize;

    title = std::string(t);
#if __USE_SKIA__
    createTitleImage(font_size);
#else
    createTitleTexture(font_size);
#endif
    updateSubLayout();
}

bool GUI_Label::handleEvents(SDL_Event *ev) {
    return GUI_View::handleEvents(ev);
}

void GUI_Label::setPadding(int p0, int p1, int p2, int p3) {
    padding[0] = p0;
    padding[1] = p1;
    padding[2] = p2;
    padding[3] = p3;

    updateSubLayout();
}

void GUI_Label::updateSubLayout() {
#ifdef DEBUG_LAYOUT
    GUI_Log(">> Update Sublayout: %s\n", title.c_str());
#endif

    int w = title_area.w;
    int h = title_area.h;

    if (w < min_width)
        w = min_width;

    if (h < min_height)
        h = min_height;

    int tx = 0;

#if __USE_SKIA__
    if (imageIcon) {
#else
    if (textureIcon) {
#endif
        tx = icon_area.w;

        if (title_area.w > 0) {
            tx += icon_gap*GUI_scale;
        }

        if (icon_area.h > title_area.h) {
            h += (icon_area.h - title_area.h);
        }

        w += tx;
    }

    bool size_changed = false;

    if (ow == 0) {
        int wwx = w + (padding[1] + padding[3]) * GUI_scale;

        if (wwx != tw_area.w) {
            tw_area.w = wwx;
            size_changed = true;
        }
    }

    if (oh == 0) {
        int hhx = h + (padding[0] + padding[2]) * GUI_scale;

        if (hhx != tw_area.h) {
            tw_area.h = hhx;
            size_changed = true;
        }
    }

    if (title_align & GUI_ALIGN_CENTER) {
        icon_area.x = (tw_area.w - w) / 2;
        title_area.x = icon_area.x + tx;
    } else if (title_align & GUI_ALIGN_RIGHT) {
        title_area.x = tw_area.w - padding[1] * GUI_scale - title_area.w;
        icon_area.x = title_area.x - icon_area.w - icon_gap*GUI_scale;
    } else {
        icon_area.x = padding[3] * GUI_scale;
        title_area.x = icon_area.x + tx;
    }

    if (title_align & GUI_ALIGN_VCENTER) {
        title_area.y = ((tw_area.h - title_area.h) / 2);
        icon_area.y = ((tw_area.h - icon_area.h) / 2); //+icon_vshift;;
    } else if (title_align & GUI_ALIGN_BOTTOM) {
        title_area.y = (tw_area.h - title_area.h) - padding[2] * GUI_scale;
        icon_area.y = (tw_area.h - icon_area.h) - padding[2] * GUI_scale; //+icon_vshift;;
    } else {
        icon_area.y = padding[0] * GUI_scale;
        title_area.y = padding[0] * GUI_scale;
    }

    if (size_changed) {
#ifdef DEBUG_LAYOUT
        GUI_Log("   Sublayout Size changed\n");
#endif

        if (parent) {
#ifdef DEBUG_LAYOUT
            GUI_Log("   Force parent to update layout\n");
#endif
            parent->updateLayout();
        }
    }

#ifdef DEBUG_LAYOUT
    GUI_Log("   Updated Sublayout: %s\n", title.c_str());
#endif
}
