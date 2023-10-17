//
//  GUI_List.h
//  OSX
//
//  Created by SIHARAT THAMMAYA on 9/2/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#ifndef GUI_List_h
#define GUI_List_h

#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "GUI_View.h"
#include "GUI_Label.h"
#include "GUI_Popup.h"
#include "GUI_Log.h"
#include "GUI_Math.h"
#include <string>
#include <vector>

class GUI_ListCell : public GUI_Label {
    friend class GUI_List;

public:
    GUI_ListCell(GUI_View *parent, const char *text, int x, int y, int width, int height, bool checkable = false, int fontSize = 1);
    virtual ~GUI_ListCell() override;

    virtual void draw() override;
    void setSubmenuArrow(SDL_Texture* tex, bool shared = false);

public:
    std::function<void(GUI_ListCell* cell)> display_cmd;
    bool checkable;
    bool checked;
    bool separator;
    bool enable;
    bool selected;
    //bool scrollable; // Tooh - fix selected index color overlaps side scroll bar

    bool colorListView;
    SDL_Color highlightColor;
    SDL_Color highlightTextColor;
    SDL_Color colorPreview;

    std::string userData;
    GUI_View *userView;
    
    void* __userData;

    SDL_Texture* submenuArrow;
    GUI_Rect submenuArrowRect;
    SDL_Color submenuArrowColor;
    bool sharedSubmenuArrow;
};

class GUI_List : public GUI_View {
    friend class GUI_Dropdown;

public:
    std::function<void(GUI_List* l, const char *sel, int index)> cmd;

    GUI_List(GUI_View* parent,
             const char** text,
             size_t num,
             int x = 0,
             int y = 0,
             int width = 0,
             int height = 0,
             size_t cellHeight = 0,
             bool popup = false,
             std::function<void(GUI_List* l, const char* sel, int index)> cmd = NULL,
             int fontSize = 1);

    GUI_List(GUI_View* parent,
             const char** text,
             size_t num,
             int x = 0,
             int y = 0,
             int width = 0,
             int height = 0,
             size_t cellHeight = 0,
             bool popup = false,
             std::function<void(GUI_List* l, const char *sel, int index)> cmd = NULL,
             std::function<void(GUI_ListCell* cell)> custom_draw_cell = NULL,
             int fontSize = 1);

    virtual ~GUI_List() override;

    void setCells(const char **text, int num);
    GUI_ListCell* addCell(const char *text);
    void deleteCellAtIndex(int index);
    GUI_ListCell* insertCellAtIndex(const char *text, size_t index);
    int getCellIndex(GUI_ListCell* cell);
    void deleteAll();
    void setHighlightColor( SDL_Color color );

    std::vector<GUI_ListCell *> vMenu;

    int setCheck(int tag);
    void setEnable(size_t cell, bool enable);

    void selectChanged(int n);

    virtual void hide() override;
    virtual void show() override;

    virtual void draw() override;
    virtual bool handleEvents(SDL_Event *ev) override;

    virtual void updateSubLayout() override;

    GUI_ListCell* getCell(int tag);
    bool bHilightCellWhenClick;
    int selectedIndex;
    int oldSelectedIndex;
    size_t numCells;

    size_t getSize() {
        return vMenu.size();
    }

    GUI_ListCell* getCellAtIndex(size_t index);

    GUI_View *friendView;

    std::function<void(GUI_ListCell* cell)> draw_cell_cmd;

    void setReverseList(bool reverse);

    // pom
    void setSelectedIndex(int index);
    bool popupAutoClose;

    std::function<void(GUI_List*)> onHilightChanged;
    std::function<void(GUI_List*)> onScrollPadChanged;

    //bool isFocus;
    
private:
    bool breverse;

    int lastMouseX;
    int lastMouseY;

    bool bMousePress;
    Uint32 mouseClickTime;

    SDL_TimerID timerID;
    bool isDown;
    int preferedHight;
    int fontSize;

    bool popupMode;
    int _padding;

    //int corner;

    bool scrollable;
    size_t cellHeight;
    SDL_Color highlightColor;

    GUI_View *scrollPad;

    const char **menuText;

    int target;
    int current;

    void setTarget(int t) {
        target = t;
        current = 0;
    }

    bool _dragging;
};

#endif /* GUI_List_h */

