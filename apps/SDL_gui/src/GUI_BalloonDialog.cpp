//
//  GUI_BalloonDialog.cpp
//  Pukchue2
//
//  Created by SIHARAT THAMMAYA on 11/1/2559 BE.
//  Copyright © 2559 Jimmy Software Co., Ltd. All rights reserved.
//

#include "GUI_BalloonDialog.h"
#include "GUI_Utils.h"
#include "GUI_Slider.h"
#include "GUI_Utils.h"

GUI_EditTextDialog::GUI_EditTextDialog(GUI_View *parent, string text, int x, int y, int w, int h,
                                       void(*act_cmd)(GUI_EditTextDialog *, string result ) )
: GUI_Dialog(parent, "", x, y, w, h)
{
    result_cmd = act_cmd;
    
    setBorder( 1 );
    corner = 4;
    setAlignment(GUI_ALIGN_ABSOLUTE );
    setLayout(GUI_LAYOUT_VERTICAL);
    
    setPadding( 20, 10, 10, 10 );
    
    editText = GUI_EditText::create( this, text.c_str(), 0, 0, -1, 0 );
    
    GUI_View *panel = GUI_View::create( this, "", 0, 0, -1, 0 );
    panel->setLayout( GUI_LAYOUT_HORIZONTAL );
    panel->setBackgroundColor(cClear );
    panel->setMargin( 20, 10, 10, 10 );
    
    GUI_Button *bttnOK = GUI_Button::create( panel, "OK", 0, 0, 80, 0, [=](GUI_Button *bttn) {
        if( result_cmd ) {
            result_cmd( this, editText->title );
        }
        this->close();
    });
    bttnOK->setAlignment( GUI_ALIGN_CENTER );
    bttnOK->setPadding( 5, 10, 5, 10 );
    bttnOK->setMargin( 0, 15, 0, 15 );
    bttnOK->setBorder( 1 );
    bttnOK->corner = 4;
    
    GUI_Button *bttnCancel = GUI_Button::create( panel, "Cancel", 0, 0, 80, 0, [=](GUI_Button *bttn) {
        this->close();
    });
    bttnCancel->setAlignment( GUI_ALIGN_CENTER );
    bttnCancel->setPadding( 5, 10, 5, 10 );
    bttnCancel->setMargin( 0, 0, 0, 15 );
    bttnCancel->setBorder( 1 );
    bttnCancel->corner = 4;
    
    centerToParent();
}

SimpleDialog::SimpleDialog(GUI_View *parent, int x, int y, int w, int h) : GUI_Dialog(parent, "", x, y, w, h),
bClearViewport(false),
mHeight(0),
bcustomLayout(false),
mRowSpacing(0) {
    setLayout(GUI_LAYOUT_VERTICAL);
}

void SimpleDialog::clearViewport(SDL_Color color) {
    SDL_Rect cliprect = {0};
    SDL_Rect viewport = {0};
    SDL_RenderGetClipRect(GUI_renderer, &cliprect);
    SDL_RenderGetViewport(GUI_renderer, &viewport);

    SDL_RenderSetViewport(GUI_renderer, GUI_MakeRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale));
    SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale));
    GUI_FillRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale, color);

    SDL_RenderSetClipRect(GUI_renderer, &cliprect);
    SDL_RenderSetViewport(GUI_renderer, &viewport);
}

void SimpleDialog::setRowSpacing(int h) {
    mRowSpacing = h * GUI_scale;
}

void SimpleDialog::draw() {
    if (isVisible() && bClearViewport)
        clearViewport(sdl_color(0x0000006B));

    GUI_View::draw();
}

void SimpleDialog::customLayout() {
    bcustomLayout = true;
}

void SimpleDialog::setClearViewport(bool clearViewport) {
    bClearViewport = clearViewport;
}

GUI_ToolBar* SimpleDialog::addToolBar(int h) {
    GUI_ToolBar *tb = new GUI_ToolBar(this, GUI_BAR_TOP, NULL, h);
    tb->corner = corner;
    tb->align = GUI_ALIGN_TOP;

    return tb;
}

GUI_ToolBar* SimpleDialog::addToolBarBottom(int h) {
    GUI_ToolBar *tb = new GUI_ToolBar(this, GUI_BAR_BOTTOM, NULL, h);
    tb->corner = corner;
    tb->align = GUI_ALIGN_BOTTOM;
    return tb;
}

GUI_Label* SimpleDialog::addLabel(string title, SDL_Color color, int align) {
    GUI_Label *lb = new GUI_Label(this, title.c_str(), 0, 0, -1, 32, color, align | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL);
    lb->bgcol = cClear;
    lb->setLayout(GUI_LAYOUT_VERTICAL);

    if (title.length() > 0)
        lb->setPadding(0, 5, 0, 5);

    if (align & GUI_ALIGN_RIGHT)
        lb->setMargin(0, 5, 0, 0);
    else
        lb->setMargin(0, 0, 0, 5);

    lb->setAlignment(align);
    return lb;
}

GUI_Label* SimpleDialog::addLabel(string title, Uint16 unicode, SDL_Color color, int align) {
    GUI_Label *lb = new GUI_Label(this, title.c_str(), 0, 0, -1, 32, cBlack, align | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL);
    lb->bgcol = cClear;
    lb->setLayout(GUI_LAYOUT_VERTICAL);

    if (title.length() > 0)
        lb->setPadding(0, 5, 0, 5);

    if (align & GUI_ALIGN_RIGHT)
        lb->setMargin(0, 5, 0, 0);
    else
        lb->setMargin(0, 0, 0, 5);

    if (unicode) {
        SDL_Texture *tx = GUI_ButtonFA::createTextureFormUnicode(unicode, 0, 0, cWhite, 1);
        lb->setIcon(tx);
        lb->iconColor = color;
    }

    lb->setAlignment(align);
    return lb;
}

GUI_Button* SimpleDialog::addButton(string title, int align) {
    GUI_Button *bt = new GUI_Button(this, title.c_str(), 0, 0, title.length() > 0 ? 0 : 32, 32, cBlack, GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL);
    bt->bgcol = sdl_color(0x00000000);

    if (title.length() > 0) {
        bt->setPadding(0, 5, 0, 5);
    }

    if (align & GUI_ALIGN_RIGHT)
        bt->setMargin(0, 5, 0, 0);
    else
        bt->setMargin(0, 0, 0, 5);

    bt->align = align;
    return bt;
}

GUI_Button* SimpleDialog::addButton(Uint16 unicode, int align) {
    GUI_Button *bt = new GUI_Button(this, "", 0, 0, title.length() > 0 ? 0 : 32, 32, cBlack, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL);
    bt->bgcol = sdl_color(0x00000000);

    if (title.length() > 0) {
        bt->setPadding(0, 5, 0, 5);
    }

    if (align & GUI_ALIGN_RIGHT)
        bt->setMargin(0, 5, 0, 0);
    else
        bt->setMargin(0, 0, 0, 5);

    bt->setIcon(GUI_ButtonFA::createTextureFormUnicode(unicode, 0, 0, cWhite, 1));
    bt->iconColor = cBlack;
    bt->align = align;

    return bt;
}

GUI_Button* SimpleDialog::addButton(Uint16 unicode, string title, int align) {
    GUI_Button *bt = new GUI_Button(this, title.c_str(), 0, 0, title.length() > 0 ? 0 : 32, 32, cBlack, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL);

    bt->bgcol = sdl_color(0x00000000);
    bt->setLayout(GUI_LAYOUT_VERTICAL);

    if (title.length() > 0) {
        bt->setPadding(0, 5, 0, 5);
    }

    if (align & GUI_ALIGN_RIGHT) {
        bt->setMargin(0, 5, 0, 0);
    } else
        bt->setMargin(0, 0, 0, 5);

    bt->setIcon(GUI_ButtonFA::createTextureFormUnicode(unicode, 0, 0, cWhite, 1));
    bt->iconColor = titleColor;
    bt->setAlignment(align);
    return bt;
}

GUI_ButtonFA* SimpleDialog::addButtonFA(Uint16 unicode, string title, int align) {
    GUI_ButtonFA *bt = new GUI_ButtonFA(this, unicode, title.c_str(), 0, 0, 45, 28, cBlack, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER);
    bt->setLayout(GUI_LAYOUT_VERTICAL);
    if (title.length() > 0) {
        bt->setPadding(0, 5, 0, 5);
    }

    if (align & GUI_ALIGN_RIGHT) {
        bt->setMargin(0, 5, 0, 0);
    } else
        bt->setMargin(0, 0, 0, 5);

    bt->setAlignment(align);
    return bt;
}



GUI_EditText * SimpleDialog::addEditText(int w, int align) {
    GUI_EditText *et = new GUI_EditText(this, title.c_str(), 0, 0, w, 32, cBlack, GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL);
    et->align = align;

    if (align & GUI_ALIGN_RIGHT)
        et->setMargin(0, 5, 0, 0);
    else {
        et->setMargin(0, 0, 0, 5);
    }

    if (w == -1) {
        et->setMargin(0, 5, 0, 5);
        et->align = GUI_ALIGN_CENTER;
    }

    et->updateSubLayout();
    return et;
}

void SimpleDialog::addVerticalSpace(int h) {
    if (lst_child >= 0) {
        auto c = children[lst_child];

        if (c) {
            c->margin[2] += (h * GUI_scale);
        }
    }
}

void SimpleDialog::addSeparator(SDL_Color c) {
    auto l = new GUI_Label(this, "", 0, 0, -1, 2, cClear, GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER);
    l->bgcol = cGrey;
    l->setMargin(0, 5, 0, 5);
    l->align = GUI_ALIGN_CENTER;
    l->updateSubLayout();
}


GUI_View * SimpleDialog::addHorizontalLayout(int w, int align) {
    GUI_View *et = new GUI_View(this, "", 0, 0, w, 32, cBlack);
    et->align = align;

    if (align)
        et->setMargin(0, 5, 0, 0);
    else {
        et->setMargin(0, 0, 0, 5);
    }

    if (w == -1) {
        et->setMargin(0, 5, 0, 5);
        et->align = GUI_ALIGN_CENTER;
    }

    et->updateSubLayout();
    return et;
}

#ifndef _WIN32
#pragma mark BalloonDialog
#endif
BalloonDialog::BalloonDialog(GUI_View *parent, const char *title, int _x, int _y, int _w, int _h, int padding, BallonType ktype) : SimpleDialog(parent, _x, _y, _w, _h),
type(ktype),
innerPadding(padding) {
    fx = fy = -1;
}

void BalloonDialog::clear(GUI_Rect *rect) {
    if (bClearViewport)
        clearViewport(sdl_color(0x0000006B));

    if (!rect)
        rect = GUI_MakeRect(0, 0, tw_area.w, tw_area.h);

    if (bgcol.a != 0) {
        int padding = innerPadding*GUI_scale;
        GUI_Rect inner;
        inner.x = rect->x + padding;
        inner.y = rect->y + padding;
        inner.w = rect->w - padding * 2;
        inner.h = rect->h - padding * 2;

        int size = padding * 2;
        float halfLength = (float)(sqrt(2 * (size * size)) / 2);
        int cx = 0, cy = 0;

        if (type == kBallonType_TOP) {
            cx = inner.x + inner.w / 2;
            cy = (int)(rect->y + halfLength);

            if (fx != -1) {
                int min = (int)(inner.x + corner*GUI_scale + halfLength);
                int max = (int)(inner.x + inner.w - corner*GUI_scale - halfLength);
                cx = (int)guiClamp((float)(fx - tw_area.x), (float)min, (float)max);
            }
        } else if (type == kBallonType_BOTTOM) {
            cx = inner.x + inner.w / 2;
            cy = (int)(rect->y + rect->h - halfLength);

            if (fx != -1) {
                int min = (int)(inner.x + corner*GUI_scale + halfLength);
                int max = (int)(inner.x + inner.w - corner*GUI_scale - halfLength);
                cx = (int)guiClamp((float)(fx - tw_area.x), (float)min, (float)max);
            }
        } else if (type == kBallonType_LEFT) {
            cx = (int)(rect->x + halfLength);
            cy = inner.y + inner.h / 2;

            if (fy != -1) {
                int min = (int)(inner.y + corner*GUI_scale + halfLength);
                int max = (int)(inner.y + inner.h - corner*GUI_scale - halfLength);
                cy = (int)guiClamp((float)(fy - tw_area.y), (float)min, (float)max);
            }
        } else if (type == kBallonType_RIGHT) {
            cx = (int)(rect->x + rect->w - halfLength);
            cy = inner.y + inner.h / 2;

            if (fy != -1) {
                int min = (int)(inner.y + corner*GUI_scale + halfLength);
                int max = (int)(inner.y + inner.h - corner*GUI_scale - halfLength);
                cy = (int)guiClamp((float)(fy - tw_area.y), (float)min, (float)max);
            }
        }

        //pom
        GUI_FillRect(cx, cy, size, size, 45, bgcol);

        if (corner != 0) {
            GUI_FillRoundRect(inner.x, inner.y, inner.w, inner.h, corner * GUI_scale, bgcol);
        } else {
            GUI_FillRect(inner.x, inner.y, inner.w, inner.h, bgcol);
        }
    }
}

void BalloonDialog::fromLeft(int y) {
    type = kBallonType_LEFT;
    fy = tw_area.y + y * GUI_scale;
}

void BalloonDialog::fromRight(int y) {
    type = kBallonType_RIGHT;
    fy = tw_area.y + y * GUI_scale;
}

void BalloonDialog::fromTop(int x) {
    type = kBallonType_TOP;
    fx = tw_area.x + x * GUI_scale;
}

void BalloonDialog::fromBottom(int x) {
    type = kBallonType_BOTTOM;
    fx = tw_area.x + x * GUI_scale;
}

void BalloonDialog::from_twareaLeft(int y) {
    type = kBallonType_LEFT;
    fy = y;
}

void BalloonDialog::from_twareaRight(int y) {
    type = kBallonType_RIGHT;
    fy = y;
}

void BalloonDialog::from_twareaTop(int x) {
    type = kBallonType_TOP;
    fx = x;
}

void BalloonDialog::from_twareaBottom(int x) {
    type = kBallonType_BOTTOM;
    fx = x;
}
