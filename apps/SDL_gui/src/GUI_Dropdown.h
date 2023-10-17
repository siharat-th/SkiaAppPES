//
//  GUI_Dropdown.h
//  OSX
//
//  Created by SIHARAT THAMMAYA on 10/29/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#ifndef GUI_Dropdown_h
#define GUI_Dropdown_h
#include "SDL_gui.h"
#include "GUI_List.h"
#include "GUI_Button.h"
#include "GUI_Math.h"

class GUI_Dropdown : public GUI_Button {
    friend class GUI_List;

public:
    GUI_Dropdown(GUI_View *parent, const char *title, const char** menuText, int num, int x = 0, int y = 0, int width = 0, int height = 0,
                 void(*sel_cmd)(GUI_Dropdown*, GUI_List*, int selectedIndex, const char* selectedLabel) = NULL,
                 SDL_Color color = cBlack, int align = GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, int fontSize = 1);

    GUI_Dropdown(GUI_View *parent, const char *title, const std::vector<string> &menuText, int x = 0, int y = 0, int width = 0, int height = 0,
                 void(*sel_cmd)(GUI_Dropdown*, GUI_List*, int selectedIndex, const char* selectedLabel) = NULL,
                 SDL_Color color = cBlack, int align = GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, int fontSize = 1);

    virtual ~GUI_Dropdown() override;

    virtual void draw() override;
    virtual bool handleEvents(SDL_Event *ev) override;

    void renderTexture(SDL_Texture *tex, int x, int y, bool positionIsCenter = false, SDL_Color color = cWhite);

    bool isExpand;
    //bool isDown;
    //bool isFocus;

    void setMaxVisibleCell(int max);

    std::function<void(GUI_Dropdown*, GUI_List*, int selectedIndex, const char* selectedLabel)> selected_cmd;
    std::function<void(GUI_Dropdown*)> expanded_cmd;

    std::function<void(GUI_Dropdown*, GUI_List*, int selectedIndex)> __selected_cmd;

    void setToggleNavIcon(bool toggle);
    virtual void setTitle(std::string t, int fontSize = -1) override;
    void closeDropdownMenu();
    void setSelectIndex(int index, bool invoke_cb = true);
    int getSelectedIndex();

    std::vector<GUI_ListCell*> getListCell();
    void setColorListView(SDL_Color* colors, int num);

    GUI_List* dropdownMenu;

    void setNumcell(int ncell) {
        numCell = ncell;
    }

    int numCell;

    // Tooh - support keyboard
    int currentSelectedIndex;
    void updateDropdownMenu();
    void toggleDropdownMenu();
    int getNextIndex(int nCurrentIndex);
    int getPreviousIndex(int nCurrentIndex);

    // Tooh - fix mouse wheel
    virtual void hide() override;
    virtual void enable() override;
    virtual void disable() override;

protected:
    int orgX, orgY, orgW, orgH;
    SDL_Texture* texExpanded;
    SDL_Texture* texClosed;

    const char** menuText;

    int maxVisibleCell;

    bool bToggleNavigateIcon;
    int _selectedIndex;
    SDL_Color* colors;
    int numColor;
    bool colorListView;
};

#endif /* GUI_Dropdown_h */
