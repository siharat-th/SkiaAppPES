//
//  GUI_BalloonDialog.h
//  Pukchue2
//
//  Created by SIHARAT THAMMAYA on 11/1/2559 BE.
//  Copyright © 2559 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef GUI_BalloonDialog_h
#define GUI_BalloonDialog_h

#include <stdio.h>
#include "SDL_gui.h"


class GUI_ButtonFontAwesome;
class GUI_Slider;
class GUI_ToolBar;

#ifndef _WIN32
#pragma mark Dialog
#endif

class GUI_EditTextDialog : public GUI_Dialog {
    GUI_EditText *editText;
    void(*result_cmd)(GUI_EditTextDialog *, string result);
public:
    GUI_EditTextDialog(GUI_View *parent, string text="", int x = 0, int y = 0, int w = 300, int h = 0,
                       void(*act_cmd)(GUI_EditTextDialog *, string result ) = NULL);
};

class SimpleDialog : public GUI_Dialog {
public:
    SimpleDialog(GUI_View *parent, int x = 0, int y = 0, int w = 640, int h = 480);

    virtual void draw() override;
    void customLayout();
    void setClearViewport(bool clearViewport);
    void setRowSpacing(int h);

    GUI_ToolBar* addToolBar(int h = 44);
    GUI_ToolBar* addToolBarBottom(int h = 44);

    GUI_Label * addLabel(string title, SDL_Color color = cBlack, int align = 0);
    GUI_Label * addLabel(string title, Uint16 unicode, SDL_Color color = cBlack, int align = 0);

    void addVerticalSpace(int h);
    void addSeparator(SDL_Color c = cGrey);

    GUI_Button* addButton(string title, int align = 0);
    GUI_Button* addButton(Uint16 unicode, int align = 0);
    GUI_Button* addButton(Uint16 unicode, string title, int align = 0);

    GUI_ButtonFontAwesome* addButtonFA(Uint16 unicode, string title, int align = 0);

    GUI_EditText* addEditText(int w = -1, int align = 0);

    GUI_View* addHorizontalLayout(int w = -1, int align = 0);

protected:
    void clearViewport(SDL_Color color);
    bool bClearViewport;
    int mHeight;
    int mRowSpacing;
    bool bcustomLayout;
};

#ifndef _WIN32
#pragma mark BalloonDialog
#endif

class BalloonDialog : public SimpleDialog {
public:
    enum BallonType {
        kBallonType_LEFT,
        kBallonType_RIGHT,
        kBallonType_TOP,
        kBallonType_BOTTOM
    };

    BalloonDialog(GUI_View *parent, const char *title, int x = 0, int y = 0, int w = 640, int h = 480, int padding = 12, BallonType kType = kBallonType_LEFT);
    virtual void clear(GUI_Rect *rect = 0) override;

    void fromLeft(int y = -1);
    void fromRight(int y = -1);
    void fromTop(int x = -1);
    void fromBottom(int x = -1);

    void from_twareaLeft(int y = -1);
    void from_twareaRight(int y = -1);
    void from_twareaTop(int x = -1);
    void from_twareaBottom(int x = -1);

private:
    BallonType type;
    int fx, fy;
    int innerPadding;
};

#endif /* GUI_BalloonDialog_hpp */
