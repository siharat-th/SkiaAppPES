//
//  GUI_Dropdown.cpp
//  OSX
//
//  Created by SIHARAT THAMMAYA on 10/29/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#include "GUI_Dropdown.h"
#include "GUI_Math.h"

GUI_Dropdown::GUI_Dropdown(GUI_View *parent, const char *title, const char** _menuText, int num, int x, int y, int width, int height,
                           void(*sel_cmd)(GUI_Dropdown*, GUI_List*, int selectedIndex, const char* selectedLabel),
                           SDL_Color color, int align, int fontSize)
    :
    GUI_Button(parent, title, x, y, width, height, color, align, fontSize),
    orgX(x),
    orgY(y),
    orgW(width),
    orgH(height),
    isExpand(false),
    texExpanded(NULL),
    texClosed(NULL),
    menuText(NULL),
    numCell(num),
    maxVisibleCell(num),
    dropdownMenu(NULL),
    bToggleNavigateIcon(true),
    expanded_cmd(NULL),
    __selected_cmd(NULL),
    _selectedIndex(-1),
    currentSelectedIndex(-1), // Tooh - support keyboard 
    colors(NULL),
    numColor(0),
    colorListView(false)
{
    isDown = false;
    isFocus = false;

    selected_cmd = sel_cmd;
    
    GUI_Label::fontSize = fontSize;

    setBorder(1);
    bgcol = cWhite;
    this->menuText = _menuText;

    texExpanded = GUI_createTextureFormUnicode(kIcon_caret_down);
    texClosed = GUI_createTextureFormUnicode(kIcon_caret_left);

    setPadding(10, 5, 10, 5);
}

GUI_Dropdown::GUI_Dropdown(GUI_View *parent, const char *title, const std::vector<string>& _menuText, int x, int y, int width, int height,
                           void(*sel_cmd)(GUI_Dropdown*, GUI_List*, int selectedIndex, const char* selectedLabel),
                           SDL_Color color, int align, int fontSize)
    :
    GUI_Button(parent, title, x, y, width, height, color, align, fontSize),
    orgX(x),
    orgY(y),
    orgW(width),
    orgH(height),
    isExpand(false),
    texExpanded(NULL),
    texClosed(NULL),
    menuText(NULL),
    numCell((int)_menuText.size()),
    maxVisibleCell((int)_menuText.size()),
    dropdownMenu(NULL),
    bToggleNavigateIcon(true),
    expanded_cmd(NULL),
    __selected_cmd(NULL),
    _selectedIndex(-1),
    colors(NULL),
    numColor(0),
    colorListView(false) {
    selected_cmd = sel_cmd;

    isDown = false;
    isFocus = false;

        
    GUI_Label::fontSize = fontSize;

    setBorder(1);
    bgcol = cWhite;

    this->menuText = (const char**)_menuText.data();
    texExpanded = GUI_createTextureFormUnicode(kIcon_caret_down);
    texClosed = GUI_createTextureFormUnicode(kIcon_caret_left);

    setPadding(10, 5, 10, 5);
}

GUI_Dropdown::~GUI_Dropdown() {
    if (texExpanded)
        SDL_DestroyTexture(texExpanded);

    if (texClosed)
        SDL_DestroyTexture(texClosed);
}

void GUI_Dropdown::renderTexture(SDL_Texture *tex, int x, int y, bool positionIsCenter, SDL_Color color) {
    //Setup the destination rectangle to be at the position we want
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;

    //Query the texture to get its width and height to use
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);

    if (positionIsCenter) {
        dst.x -= dst.w / 2;
        dst.y -= dst.h / 2;
    }

    if (color.a != 0)
        SDL_SetTextureColorMod(tex, color.r, color.g, color.b);

    SDL_RenderCopy(GUI_renderer, tex, NULL, &dst);
}

void GUI_Dropdown::draw() {
    // Tooh - fix dropdown not closed when it is not enabled
    if (isExpand && !this->isEnable()) {
        closeDropdownMenu();
    }

    GUI_View::draw();
    SDL_Color c = isEnable() ? cDarkGrey : cLightGrey;

    if (isExpand || !bToggleNavigateIcon) {
        if (texExpanded) {
            int w, h;
            SDL_QueryTexture(texExpanded, NULL, NULL, &w, &h);

            renderTexture(texExpanded, tw_area.w - w - padding[1] * GUI_scale, (tw_area.h - h) / 2, false, c);
        }
    } else {
        if (texClosed) {
            int w, h;
            SDL_QueryTexture(texClosed, NULL, NULL, &w, &h);
            // Tooh - fix arrow icon position
            renderTexture(texClosed, tw_area.w - w - ((padding[1] + 3) * GUI_scale), (tw_area.h - h) / 2, false, c);
        }
    }

    if (colorListView) {
        GUI_FillRect(0, 0, 12 * GUI_scale, tw_area.h, colors[getSelectedIndex()]);
    }
}

bool GUI_Dropdown::handleEvents(SDL_Event *ev) {
    if (isVisible() == false)
        return false;

    if (!isEnable() && ev->type != GUI_EventPaint)
        return false;

    switch (ev->type) {
        case SDL_MOUSEBUTTONDOWN:
        {
            isDown = true;
            isFocus = true;
            GUI_mouseCapturedWindow = (GUI_View *)this;

            if (handle_event_cmd) {
                handle_event_cmd(this, ev);
            }

            return true;
        }
        case SDL_MOUSEMOTION:
        {
            if (isFocus) {
                SDL_MouseButtonEvent e = ev->button;
                int x = (int)(e.x*GUI_mouseScale);
                int y = (int)(e.y*GUI_mouseScale);

                if (hitTest(x, y)) {
                    isDown = true;
                    return true;
                } else {
                    isDown = false;
                }
            }
            return false;
        }
        case SDL_MOUSEBUTTONUP:
        {
            if (isFocus) {
                GUI_mouseCapturedWindow = NULL;
                SDL_MouseButtonEvent e = ev->button;
                int x = (int)(e.x*GUI_mouseScale);
                int y = (int)(e.y*GUI_mouseScale);

                isDown = false;
                isFocus = false;

                if (hitTest(x, y)) {
                    toggleDropdownMenu(); // Tooh - support keyboard
                    isFocus = true; // Tooh - still focus for key down and up event
                    return true;
                }
            }

            return false;
        }

        case SDL_KEYDOWN: // Tooh - support keyboard
        {
            if (!this->isEnable() || !isFocus) {
                return false;
            }

            SDL_Scancode scancode = ev->key.keysym.scancode;
            bool isUpdateDropdownMenu = false;
            bool isSelectedCmd = false;

            if (scancode == SDL_SCANCODE_HOME) {
                if (_selectedIndex > 0) {
                    isUpdateDropdownMenu = true;
                    isSelectedCmd = !isExpand;
                    _selectedIndex = 0;

                    if (strcmp(menuText[_selectedIndex], "---") == 0) {
                        _selectedIndex = getNextIndex(_selectedIndex);
                    }
                }
            } else if (scancode == SDL_SCANCODE_END) {
                if (_selectedIndex < numCell - 1) {
                    isUpdateDropdownMenu = true;
                    isSelectedCmd = !isExpand;
                    _selectedIndex = numCell - 1;

                    if (strcmp(menuText[_selectedIndex], "---") == 0) {
                        _selectedIndex = getPreviousIndex(_selectedIndex);
                    }
                }
            } else if (scancode == SDL_SCANCODE_UP || scancode == SDL_SCANCODE_LEFT) {
                if (_selectedIndex > 0) {
                    isUpdateDropdownMenu = true;
                    isSelectedCmd = !isExpand;
                    _selectedIndex = getPreviousIndex(_selectedIndex);
                }
            } else if (scancode == SDL_SCANCODE_DOWN || scancode == SDL_SCANCODE_RIGHT) {
                if (_selectedIndex < numCell - 1) {
                    isUpdateDropdownMenu = true;
                    isSelectedCmd = !isExpand;
                    _selectedIndex = getNextIndex(_selectedIndex);
                }
            } else if (scancode == SDL_SCANCODE_PAGEUP) {
                if (_selectedIndex > 0) {
                    isUpdateDropdownMenu = true;
                    isSelectedCmd = !isExpand;
                    _selectedIndex -= maxVisibleCell;

                    if (_selectedIndex < 0) {
                        _selectedIndex = 0;

                        if (strcmp(menuText[_selectedIndex], "---") == 0) {
                            _selectedIndex = getNextIndex(_selectedIndex);
                        }
                    } else {
                        if (strcmp(menuText[_selectedIndex], "---") == 0) {
                            _selectedIndex = getPreviousIndex(_selectedIndex);
                        }
                    }
                }
            } else if (scancode == SDL_SCANCODE_PAGEDOWN) {
                if (_selectedIndex < numCell - 1) {
                    isUpdateDropdownMenu = true;
                    isSelectedCmd = !isExpand;
                    _selectedIndex += maxVisibleCell;

                    if (_selectedIndex > numCell - 1) {
                        _selectedIndex = numCell - 1;

                        if (strcmp(menuText[_selectedIndex], "---") == 0) {
                            _selectedIndex = getPreviousIndex(_selectedIndex);
                        }
                    } else {
                        if (strcmp(menuText[_selectedIndex], "---") == 0) {
                            _selectedIndex = getNextIndex(_selectedIndex);
                        }
                    }
                }
            }

            if (isSelectedCmd && (selected_cmd || __selected_cmd)) {
                currentSelectedIndex = _selectedIndex;
                const char* sel = menuText[_selectedIndex];
                setTitle(sel);

                if (selected_cmd) {
                    selected_cmd(this, dropdownMenu, _selectedIndex, sel);
                }

                if (__selected_cmd) {
                    __selected_cmd(this, dropdownMenu, _selectedIndex);
                }
            }

            if (isUpdateDropdownMenu) {
                updateDropdownMenu();
            }

            return true;
        }

        case SDL_KEYUP: // Tooh - support keyboard
        {
            if (!this->isEnable() || !isFocus) {
                return false;
            }

            SDL_Scancode scancode = ev->key.keysym.scancode;
            bool isUpdateDropdownMenu = false;
            bool isSelectedCmd = false;

            if (scancode == SDL_SCANCODE_RETURN || scancode == SDL_SCANCODE_RETURN2 || scancode == SDL_SCANCODE_KP_ENTER) {
                if (isExpand) {
                    isUpdateDropdownMenu = true;
                    isSelectedCmd = true;
                    _selectedIndex = dropdownMenu->selectedIndex;
                }

                toggleDropdownMenu();
            } else if (scancode == SDL_SCANCODE_ESCAPE) {
                if (isExpand) {
                    _selectedIndex = currentSelectedIndex;
                    toggleDropdownMenu();
                    isUpdateDropdownMenu = true;
                }
            }

            if (isSelectedCmd && (selected_cmd || __selected_cmd)) {
                currentSelectedIndex = _selectedIndex;
                const char* sel = menuText[_selectedIndex];
                setTitle(sel);

                if (selected_cmd) {
                    selected_cmd(this, dropdownMenu, _selectedIndex, sel);
                }

                if (__selected_cmd) {
                    __selected_cmd(this, dropdownMenu, _selectedIndex);
                }
            }

            if (isUpdateDropdownMenu) {
                updateDropdownMenu();
            }

            return true;
        }

        default:
        {
            GUI_View::handleEvents(ev);
        }
    }

    return false;
}

void GUI_Dropdown::setMaxVisibleCell(int max) {
    maxVisibleCell = max;

    if (maxVisibleCell<1 || maxVisibleCell>numCell) {
        maxVisibleCell = numCell;
    }
}

void GUI_Dropdown::setToggleNavIcon(bool toggle) {
    bToggleNavigateIcon = toggle;
}

void GUI_Dropdown::setTitle(std::string t, int _fontSize /* not use */) {
    title = std::string(t);
#if __USE_SKIA__
    createTitleImage(fontSize);
#else
    createTitleTexture(fontSize);
#endif
    updateSubLayout();
}

void GUI_Dropdown::closeDropdownMenu() {
    if (dropdownMenu && dropdownMenu->isVisible()) {
        dropdownMenu->hide();
        dropdownMenu->disable();
    }

    isExpand = false;
    isFocus = false; // Tooh - support keyboard
}

void GUI_Dropdown::setSelectIndex(int index, bool invoke) {
    if (index >= 0 && index < numCell) {
        closeDropdownMenu();
        _selectedIndex = index;
        currentSelectedIndex = index;
        const char *sel = menuText[index];
        setTitle(sel);

        if (invoke && selected_cmd) {
            selected_cmd(this, dropdownMenu, index, sel);
        }
    }
}

int GUI_Dropdown::getSelectedIndex() {
    return _selectedIndex;
}

std::vector<GUI_ListCell*> GUI_Dropdown::getListCell() {
    return dropdownMenu->vMenu;
}

void GUI_Dropdown::setColorListView(SDL_Color* colors, int num) {
    this->colors = colors;
    numColor = num;
    colorListView = true;
    setPadding(10, 5, 10, 20);
}

void GUI_Dropdown::updateDropdownMenu() { // Tooh - support keyboard
    if (dropdownMenu) {
        dropdownMenu->selectedIndex = _selectedIndex;

        auto cell = dropdownMenu->getCellAtIndex(_selectedIndex);

        if (cell && !guiInRange(cell->tw_area.y,
                                dropdownMenu->tw_area.y,
                                dropdownMenu->tw_area.y + dropdownMenu->tw_area.h - 1)) {

            if (cell->tw_area.y >= dropdownMenu->tw_area.y + dropdownMenu->tw_area.h - 1) {
                int dy = (dropdownMenu->tw_area.y + dropdownMenu->tw_area.h) - (cell->tw_area.y + cell->tw_area.h);
                dropdownMenu->setTarget(dy);
            }

            if (cell->tw_area.y < dropdownMenu->tw_area.y) {
                int dy = dropdownMenu->tw_area.y - cell->tw_area.y;
                dropdownMenu->setTarget(dy);
            }
        }
    }
}

void GUI_Dropdown::toggleDropdownMenu() { // Tooh - support keyboard
    isExpand = !isExpand;

    if (isExpand) {
        currentSelectedIndex = _selectedIndex;

        if (expanded_cmd)
            expanded_cmd(this);

        if (dropdownMenu == NULL) {
            int numSep = 0;

            for (int i = 0; i < numCell; i++) {
                if (strcmp(menuText[i], "---") == 0)
                    numSep++;
            }

            int maxH = SDL_min(25 * (numCell - numSep) + 10 * numSep, 25 * maxVisibleCell);
            GUI_View *ppx = GUI_TopView;

            if (GUI_modalWindow)
                ppx = GUI_modalWindow;

            dropdownMenu = new GUI_List(ppx, menuText, numCell,
                                        0, //orgX,
                                        0, //orgY,
                                        orgW, maxH, 25, false,
                                        [=](GUI_List *l, const char *sel, int index) {
                GUI_Dropdown *v = (GUI_Dropdown*)l->friendView;

                if (v) {
                    _selectedIndex = index;
                    v->setTitle(sel);
                    v->isExpand = false;

                    if (v->selected_cmd) {
                        v->selected_cmd(v, l, index, sel);

                        if (v->__selected_cmd)
                            __selected_cmd(v, l, index);
                    }
                }

                l->hide();
                l->disable();
            }, GUI_Label::fontSize);

            dropdownMenu->setAlignment(GUI_ALIGN_ABSOLUTE);
            dropdownMenu->bgcol = cWhite;
            dropdownMenu->setBorder(1);
            dropdownMenu->borderColor = cGrey;
            dropdownMenu->friendView = this;

            dropdownMenu->move(0 - dropdownMenu->tw_area.x + this->tw_area.x,
                               0 - dropdownMenu->tw_area.y + this->tw_area.y + this->tw_area.h);

            if (colorListView) {
                int num = min(numColor, numCell);

                for (int i = 0; i < num; i++) {
                    dropdownMenu->vMenu[i]->colorListView = true;
                    dropdownMenu->vMenu[i]->colorPreview = colors[i];
                }
            }
        } else {
            // pom
            // fixed dropdown(list) position
            dropdownMenu->move(0 - dropdownMenu->tw_area.x + this->tw_area.x, 0 - dropdownMenu->tw_area.y + this->tw_area.y + this->tw_area.h);

            dropdownMenu->show();
            dropdownMenu->enable();
        }

        updateDropdownMenu();
    } else {
        if (dropdownMenu && dropdownMenu->isVisible()) {
            dropdownMenu->hide();
            dropdownMenu->disable();
        }
    }
}

int GUI_Dropdown::getNextIndex(int nCurrentIndex) {
    for (int i = nCurrentIndex + 1; i < numCell; i++) {
        if (strcmp(menuText[i], "---") != 0) {
            nCurrentIndex = i;
            break;
        }
    }

    return nCurrentIndex;
}

int GUI_Dropdown::getPreviousIndex(int nCurrentIndex) {
    for (int i = nCurrentIndex - 1; i >= 0; i--) {
        if (strcmp(menuText[i], "---") != 0) {
            nCurrentIndex = i;
            break;
        }
    }

    return nCurrentIndex;
}

void GUI_Dropdown::hide() { // Tooh - fix mouse wheel
    closeDropdownMenu();
    GUI_View::hide();
};

void GUI_Dropdown::enable() { // Tooh - fix mouse wheel
    GUI_View::enable();
    
    if (dropdownMenu) {
        dropdownMenu->enable();
    }
};

void GUI_Dropdown::disable() { // Tooh - fix mouse wheel
    closeDropdownMenu();
    GUI_View::disable();

    if (dropdownMenu) {
        dropdownMenu->disable();
    }
};
