//
//  GUI_List.cpp
//  OSX
//
//  Created by SIHARAT THAMMAYA on 9/2/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#include "GUI_List.h"
#include "SDL_gui.h"
#include <string>
#include <iostream>

GUI_ListCell::GUI_ListCell(GUI_View *pw, const char *t, int x, int y, int w, int h, bool chk, int fontSize) :
    GUI_Label(pw, "", x, y, w, h, cBlack),
    checkable(chk),
    checked(false),
    separator(false),
    enable(true),
    selected(false),
    colorListView(false),
    highlightTextColor(cWhite),
    //scrollable(false), // Tooh - fix selected index color overlaps side scroll bar
    userView(NULL),
    submenuArrow(NULL),
    submenuArrowRect(0, 0, 0, 0),
    sharedSubmenuArrow(false),
    submenuArrowColor(cWhite) {
    title_align = GUI_ALIGN_VCENTER | GUI_ALIGN_LEFT;
    iconColor = cWhite;
    setBorder(false);
    highlightColor = sdl_color(0x2D89F9FF);
    if (!strcmp(t, "---")) {
        separator = true;
    } else {
        title = string(t);
        titleColor = cWhite;
#if __USE_SKIA__
    createTitleImage(fontSize);
#else
    createTitleTexture(fontSize);
#endif
        title_area.x = 8;
    }

    colorPreview = cGrey;
    display_cmd = NULL;
    setPadding(0, 0, 0, 4);
        __userData = NULL;
}

GUI_ListCell::~GUI_ListCell() {
    userData.clear();

    if (submenuArrow) {
        SDL_DestroyTexture(submenuArrow);
        submenuArrow = NULL;
    }
}

void GUI_ListCell::setSubmenuArrow(SDL_Texture *tex, bool shared) {
    if (tex) {
        sharedSubmenuArrow = shared;
        SDL_QueryTexture(tex, NULL, NULL, &submenuArrowRect.w, &submenuArrowRect.h);

        if (submenuArrow && !sharedSubmenuArrow) {
            SDL_DestroyTexture(submenuArrow);
        }

        submenuArrow = tex;
        submenuArrowRect.x = tw_area.w - (7 * GUI_scale) - padding[1] - submenuArrowRect.w - (2 * GUI_scale);
        submenuArrowRect.y =  (tw_area.h - submenuArrowRect.h) / 2;

        updateSubLayout();
    } else if (submenuArrow) {
        if (!sharedSubmenuArrow) {
            SDL_DestroyTexture(submenuArrow);
        }

        submenuArrow = NULL;
        updateSubLayout();
    }
}

void GUI_ListCell::draw() {
    GUI_Label::draw();

    if (!display_cmd) {
        if (separator) {
            GUI_DrawLine(0, tw_area.h / 2, tw_area.w, tw_area.h / 2, sdl_color(0x7F7F7FFF));
            return;
        }

        if (selected) {
            //GUI_Log( "Selected: %i\n", selected );
            if (enable) {
                if( titleTexture )
                    SDL_SetTextureColorMod(titleTexture, highlightTextColor.r, highlightTextColor.g, highlightTextColor.b);

                if (textureIcon) {
                    SDL_SetTextureColorMod(textureIcon, iconColor.r, iconColor.g, iconColor.b);
                }

                // Tooh - fix selected index color overlaps side scroll bar on Windows
//#if defined(WIN32)
//                int scrollWidth = scrollable ? (7 * GUI_scale) : 0;
//#else
//                //int scrollWidth = 0;
//#endif
//                GUI_FillRect(0, 0, tw_area.w - scrollWidth, tw_area.h, highlightColor );
                GUI_FillRect(0, 0, tw_area.w, tw_area.h, highlightColor);
            } else {
                if( titleTexture )
                    SDL_SetTextureColorMod(titleTexture, 0x9F, 0x9F, 0x9F);

                if (textureIcon) {
                    SDL_SetTextureColorMod(textureIcon, 0x9F, 0x9F, 0x9F);
                }
            }
        } else {
            if (parent->parent->bgcol.r == 0xff && parent->parent->bgcol.g == 0xff && parent->parent->bgcol.b == 0xff) {
                if (enable) {
                    if( titleTexture )
                        SDL_SetTextureColorMod(titleTexture, 0, 0, 0);

                    if (textureIcon) {
                        SDL_SetTextureColorMod(textureIcon, iconColor.r, iconColor.g, iconColor.b);
                    }
                } else {
                    if( titleTexture )
                        SDL_SetTextureColorMod(titleTexture, 0x9F, 0x9F, 0x9F);

                    if (textureIcon) {
                        SDL_SetTextureColorMod(textureIcon, 0x9F, 0x9F, 0x9F);
                    }
                }
            } else {
                if (enable) {
                    if( titleTexture )
                        SDL_SetTextureColorMod(titleTexture, highlightTextColor.r, highlightTextColor.g, highlightTextColor.b);

                    if (textureIcon) {
                        SDL_SetTextureColorMod(textureIcon, iconColor.r, iconColor.g, iconColor.b);
                    }
                } else {
                    if( titleTexture )
                        SDL_SetTextureColorMod(titleTexture, 0x9F, 0x9F, 0x9F);

                    if (textureIcon) {
                        SDL_SetTextureColorMod(textureIcon, 0x9F, 0x9F, 0x9F);
                    }
                }
            }
        }

        if (colorListView) {
            title_area.x = 20 * GUI_scale;
            GUI_FillRect(0, 0, 6 * GUI_scale, tw_area.h - GUI_scale, colorPreview);
        } else {
            if (!textureIcon)
                title_area.x = padding[3] * GUI_scale;
        }

        if (textureIcon) {
            SDL_RenderCopy(GUI_renderer, textureIcon, NULL, &icon_area);
        }

        if (titleTexture) {
            SDL_RenderCopy(GUI_renderer, titleTexture, NULL, &title_area);
        }

        if (submenuArrow) {
            if (enable) {
                SDL_SetTextureColorMod(submenuArrow, submenuArrowColor.r, submenuArrowColor.g, submenuArrowColor.b);
            } else {
                SDL_SetTextureColorMod(submenuArrow, 0x9F, 0x9F, 0x9F);
            }

            SDL_RenderCopy(GUI_renderer, submenuArrow, NULL, &submenuArrowRect);
        }
    } else {
        display_cmd(this);
    }
}

// GUI_List
void GUI_List::setEnable(size_t cell, bool enable) {
    if (cell < numCells) {
        getCell(cell)->enable = enable;
    }
}

GUI_List::GUI_List(GUI_View* parent,
                   const char** text,
                   size_t num,
                   int x,
                   int y,
                   int w,
                   int h,
                   size_t cellHeight,
                   bool pu,
                   std::function<void(GUI_List* l, const char* sel, int index)> c,
                   int fnt) : 
    GUI_View(parent, "List", x, y, w, h, cClear),
    cmd(c),
    _padding(4),
    popupMode(pu),
    popupAutoClose(true),
    selectedIndex(-1),
    oldSelectedIndex(-1),
    bMousePress(false),
    numCells(num),
    cellHeight(cellHeight),
    timerID(-1),
    isDown(false),
    scrollPad(NULL),
    scrollable(false),
    preferedHight(h),
    fontSize(fnt),
    bHilightCellWhenClick(false),
    target(0),
    current(0),
    draw_cell_cmd(NULL),
    breverse(false),
    _dragging(false),
    onHilightChanged(NULL),
    onScrollPadChanged(NULL),
    lastMouseX(0),
    lastMouseY(0),
    mouseClickTime(0),
    menuText(NULL),
    friendView(NULL) {

    setBorder(1);
    corner = 0;
    isFocus = false;
    highlightColor = sdl_color(0x2D89F9FF);
    if (num > 0) 
        setCells(text, num);
    bgcol = sdl_color(0x1F1F1F88);
}

GUI_List::GUI_List(GUI_View* parent,
                   const char** text,
                   size_t num,
                   int x,
                   int y,
                   int width,
                   int _preferedheight,
                   size_t cellHeight,
                   bool popup,
                   std::function<void(GUI_List* l, const char *sel, int index)> cmd,
                   std::function<void(GUI_ListCell* cell)> custom_draw_cell,
                   int fontSize) :
    GUI_View(parent, "CustomList", x, y, width, _preferedheight, cClear),
    cmd(cmd),
    draw_cell_cmd(custom_draw_cell),
    popupMode(popup),
    popupAutoClose(true),
    selectedIndex(-1),
    oldSelectedIndex(-1),
    _padding(4),
    bMousePress(false),
    numCells(num),
    cellHeight(cellHeight),
    timerID(-1),
    isDown(false),
    scrollPad(NULL),
    scrollable(false),
    preferedHight(_preferedheight),
    fontSize(fontSize),
    bHilightCellWhenClick(false),
    target(0),
    current(0),
    breverse(false),
    _dragging(false),
    onHilightChanged(NULL), 
    onScrollPadChanged(NULL),
    lastMouseX(0),
    lastMouseY(0),
    mouseClickTime(0),
    menuText(NULL),
    friendView(NULL) {

    setBorder(1);
    corner = 0;
    isFocus = false;
    highlightColor = sdl_color(0x2D89F9FF);
    if (num > 0)
        setCells(text, num);
    bgcol = sdl_color(0x1F1F1F88);
}

GUI_List::~GUI_List() {
    if (timerID > 0) {
        SDL_RemoveTimer(timerID);
        timerID = -1;
    }

    vMenu.clear();
}

void GUI_List::setHighlightColor( SDL_Color color ) {
    highlightColor = color;
    for (size_t i = 0; i < numCells; i++) {
        GUI_ListCell *cell = getCellAtIndex(i);
        cell->highlightColor = color;
    }
}


void GUI_List::setCells(const char **text, int num) {
    disable();

    menuText = text;
    numCells = num;
    vMenu.clear();

    size_t numSeps = 0;

    for (size_t i = 0; i < numCells; i++) {
        if (menuText[i] && menuText[i][0]) {
            if (!strcmp(menuText[i], "---")) {
                numSeps++;
            }
        }
    }

    size_t cch = 0;

    if (cellHeight == 0) {
        cch = 25;
    } else
        cch = cellHeight;

    int spw = tw_area.w + _padding * 2;
    int sph = (cch * (numCells - numSeps)) + 10U * numSeps;

    if (scrollPad) {
        delete scrollPad;
        scrollPad = NULL;
    }

    if (scrollPad == NULL) {
        scrollPad = new GUI_View(this, "ScrollPad", 0, 0, spw, sph, cClear);
        scrollPad->dragable = false;
        scrollPad->setBorder(false);
    } else {
        scrollPad->tw_area.w = spw;
        scrollPad->tw_area.h = sph;
    }

    if (preferedHight == 0)
        tw_area.h = cch * numCells;

    scrollable = false;

    if (scrollPad->tw_area.h > tw_area.h) {
        scrollable = true;
        scrollPad->tw_area.w = tw_area.w - 12 * GUI_scale; 
        //GUI_LogVerbose("GUI_List will scrollable");
    } else {
        scrollPad->tw_area.w = tw_area.w;
        //GUI_LogVerbose("GUI_List not scrollable");
    }

    int _yy = 0;

    for (size_t i = 0; i < numCells; i++) {
        if (menuText[i] && menuText[i][0]) {
            GUI_Rect r = getAbsoluteArea();
            int w = scrollable ? 0 * GUI_scale : 0;

            GUI_ListCell *cell = NULL;

            if (!strcmp(menuText[i], "---")) {
                cell = new GUI_ListCell(scrollPad, menuText[i], 0, _yy, r.w - w, 10, false, fontSize);
                _yy += 10;
            } else {
                if (menuText[i][0] == '\x10')
                    cell = new GUI_ListCell(scrollPad, &menuText[i][1], 0, _yy, r.w - w, cch, false, fontSize);
                else
                    cell = new GUI_ListCell(scrollPad, menuText[i], 0, _yy, r.w - w, cch, false, fontSize);

                _yy += cch;
            }
            cell->highlightColor = highlightColor;
            cell->tag = i;
            cell->dragable = false;
            cell->setAlignment(GUI_ALIGN_ABSOLUTE);
            //cell->scrollable = scrollable; // Tooh - fix selected index color overlaps side scroll bar

            //if (draw_cell_cmd) {
            //    cell->display_cmd = [=](GUI_View* listcell) {
            //        draw_cell_cmd((GUI_ListCell*)listcell);
            //    };
            //}
            cell->display_cmd = draw_cell_cmd;

            GUI_LogVerbose() << menuText[i] << "," << menuText[i][0];
            vMenu.push_back(cell);
        } else {
            GUI_LogError("GUI_List::setCells()") << menuText[i] << "," << menuText[i][0];
            break;
        }
    }

    if (popupMode) {
        GUI_mouseCapturedWindow = this;
    }

    enable();
}

GUI_ListCell* GUI_List::addCell(const char *text) {
    disable();

    numCells = numCells + 1;
    size_t cch = 0;

    if (cellHeight == 0) {
        cch = 25;
    } else
        cch = cellHeight;

    int spw = tw_area.w + _padding * 2;
    int sph = cch * numCells;

    if (scrollPad == NULL) {
        scrollPad = new GUI_View(this, "ScrollPad", 0, 0, spw, sph, cClear);
        scrollPad->dragable = false;
        scrollPad->setBorder(false);
    } else {
        scrollPad->tw_area.w = spw;
        scrollPad->tw_area.h = sph*GUI_scale;
    }

    if (preferedHight == 0){
        tw_area.h = cch * numCells * GUI_scale;
    }

    scrollable = false;

    if (scrollPad->tw_area.h > tw_area.h) {
        scrollable = true;
        scrollPad->tw_area.w = tw_area.w - 12 * GUI_scale;
        //GUI_LogVerbose("GUI_List will scrollable");
    } else {
        scrollPad->tw_area.w = tw_area.w;
        //GUI_LogVerbose("GUI_List not scrollable");
    }

    GUI_ListCell *cell = NULL;

    if (!breverse) {
        int x = 0;
        int yy = (int)(vMenu.size()*cch);
        int w = scrollable ? 0 * GUI_scale : 0;
        GUI_Rect r = getAbsoluteArea();
        cell = new GUI_ListCell(scrollPad, text, x, yy, r.w - w, cch, false, fontSize);
        cell->setAlignment(GUI_ALIGN_ABSOLUTE);
        cell->highlightColor = highlightColor;
        cell->bgcol = cRed;
        cell->tag = (int)vMenu.size();
        cell->setAlignment(GUI_ALIGN_ABSOLUTE);
        //cell->display_cmd = [=](GUI_View* listcell) {
        //    if (draw_cell_cmd != NULL)
        //        draw_cell_cmd(cell);
        //};
        cell->display_cmd = draw_cell_cmd;

        vMenu.push_back(cell);

        for (size_t i = 0; i < numCells; i++) {
            vMenu[i]->tw_area.w = cell->tw_area.w;
        }
    } else {
        int x = 0;
        int yy = 0;
        int w = scrollable ? 0 * GUI_scale : 0;
        GUI_Rect r = getAbsoluteArea();
        cell = new GUI_ListCell(scrollPad, text, x, yy, r.w - w, cch, false, fontSize);
        cell->setAlignment(GUI_ALIGN_ABSOLUTE);
        cell->highlightColor = highlightColor;
        cell->bgcol = cRed;
        cell->tag = (int)vMenu.size();

        //if (draw_cell_cmd != NULL) {
        //    cell->display_cmd = [=](GUI_View* listcell) {
        //        if (draw_cell_cmd != NULL)
        //            draw_cell_cmd(cell);
        //    };
        //}
        cell->display_cmd = draw_cell_cmd;

        vMenu.push_back(cell);
        int movey = cell->tw_area.h;

        for (size_t i = 0; i < numCells; i++) {
            vMenu[i]->tw_area.w = cell->tw_area.w;

            if (i < (int)vMenu.size() - 1) {
                vMenu[i]->move(0, movey);
            }
        }
    }

    if (popupMode) {
        GUI_mouseCapturedWindow = this;
    }

    if (cell) {
        cell->dragable = false;
    }

    enable();

    return cell;
}

GUI_ListCell* GUI_List::insertCellAtIndex(const char* text, size_t index) {
    disable();

    numCells = numCells + 1;
    size_t cch = 0;

    if (cellHeight == 0) {
        cch = 25;
    } else
        cch = cellHeight;

    int spw = tw_area.w + _padding * 2;
    int sph = cch * numCells;

    if (scrollPad == NULL) {
        scrollPad = new GUI_View(this, "ScrollPad", 0, 0, spw, sph, cClear);
        scrollPad->dragable = false;
        scrollPad->setBorder(false);
    } else {
        scrollPad->tw_area.w = spw;
        scrollPad->tw_area.h = sph*GUI_scale;
    }

    if (preferedHight == 0)
        tw_area.h = cch * numCells;

    scrollable = false;

    if (scrollPad->tw_area.h > tw_area.h) {
        scrollable = true;
        scrollPad->tw_area.w = tw_area.w - 12 * GUI_scale;
        //GUI_LogVerbose("GUI_List will scrollable");
    } else {
        scrollPad->tw_area.w = tw_area.w;
        //GUI_LogVerbose("GUI_List not scrollable");
    }

    GUI_ListCell *cell = NULL;

    if (!breverse) {
        int x = 0;
        int yy = index*cch;
        int w = scrollable ? 0 * GUI_scale : 0;
        GUI_Rect r = getAbsoluteArea();
        cell = new GUI_ListCell(scrollPad, text, x, yy, r.w - w, cch, false, fontSize);
        cell->tag = index;
        cell->dragable = false;
        cell->highlightColor = highlightColor;
        cell->setAlignment(GUI_ALIGN_ABSOLUTE);
        //cell->display_cmd = [=](GUI_View* listcell) {
        //    if (draw_cell_cmd != NULL)
        //        draw_cell_cmd(cell);
        //};
        cell->display_cmd = draw_cell_cmd;

        vMenu.insert(vMenu.begin() + index, cell);
        int movey = cell->tw_area.h;

        for (size_t i = index + 1; i < numCells; i++) {
            vMenu[i]->tag = i;
            vMenu[i]->tw_area.w = cell->tw_area.w;
            vMenu[i]->move(0, movey);
        }
    } else {
        int x = 0;
        int yy = ((int)vMenu.size() - index) * cch;
        int w = scrollable ? 0 * GUI_scale : 0;
        GUI_Rect r = getAbsoluteArea();
        cell = new GUI_ListCell(scrollPad, text, x, yy, r.w - w, cch, false, fontSize);
        cell->tag = index;
        cell->dragable = false;
        cell->highlightColor = highlightColor;
        cell->setAlignment(GUI_ALIGN_ABSOLUTE);
        //cell->display_cmd = [=](GUI_View* listcell) {
        //    if (draw_cell_cmd != NULL)
        //        draw_cell_cmd(cell);
        //};
        cell->display_cmd = draw_cell_cmd;

        vMenu.insert(vMenu.begin() + index, cell);
        int movey = cell->tw_area.h;

        for (size_t i = 0; i < numCells; i++) {
            vMenu[i]->tag = i;
            vMenu[i]->tw_area.w = cell->tw_area.w;
            if (i < index) {
                vMenu[i]->move(0, movey);
            }
        }
    }

    enable();

    return cell;
}

void GUI_List::deleteCellAtIndex(int index) {
    if (index < 0) return; // out of lower bound
    if (index >= (int)vMenu.size()) return; // out of upper bound

    // in range
    disable();

    int movey = -vMenu[index]->tw_area.h;
    vMenu[index]->close();
    vMenu.erase(vMenu.begin() + index);

    numCells = vMenu.size();
    size_t cch = 0;

    if (cellHeight == 0) {
        cch = 25;
    } else
        cch = cellHeight;

    int spw = tw_area.w + _padding * 2;
    int sph = cch * numCells;

    if (scrollPad == NULL) {
        scrollPad = new GUI_View(this, "ScrollPad", 0, 0, spw, sph, cClear);
        scrollPad->dragable = false;
        scrollPad->setBorder(false);
    } else {
        scrollPad->tw_area.w = spw;
        scrollPad->tw_area.h = sph*GUI_scale;
    }

    if (preferedHight == 0)
        tw_area.h = cch * numCells;

    scrollable = false;

    if (scrollPad->tw_area.h > tw_area.h) {
        scrollable = true;
        scrollPad->tw_area.w = tw_area.w - 12 * GUI_scale;
        //GUI_LogVerbose("GUI_List will scrollable");
    } else {
        scrollPad->tw_area.w = tw_area.w;
        //GUI_LogVerbose("GUI_List not scrollable");
    }

    int w = scrollable ? 0 * GUI_scale : 0;
    GUI_Rect r = getAbsoluteArea();
    int ww = r.w * GUI_scale - w * GUI_scale;

    if (!breverse) {
        for (size_t i = 0; i < numCells; i++) {
            vMenu[i]->tw_area.w = ww;

            if (i >= index)
                vMenu[i]->move(0, movey);

            vMenu[i]->tag = i;
        }
    } else {
        for (size_t i = 0; i < numCells; i++) {
            vMenu[i]->tw_area.w = ww;

            if (i < index)
                vMenu[i]->move(0, movey);

            vMenu[i]->tag = i;
        }
    }

    enable();

    int newIndex = (int)guiClamp((float)index, 0.0f, numCells - 1.0f);
    selectChanged(newIndex);

    if (!scrollable && scrollPad->tw_area.y < tw_area.y) {
        int dy = tw_area.y - scrollPad->tw_area.y;
        setTarget(dy);
    }
}

void GUI_List::deleteAll() {
    vMenu.clear();
    numCells = 0;
    selectedIndex = -1;
    scrollable = false;

    if (scrollPad) {
        scrollPad->remove_all_children();
        scrollPad->tw_area.h = 0;
        scrollPad->setPosition(0, 0);
    }
}

GUI_ListCell* GUI_List::getCellAtIndex(size_t index) {
    if (index < getSize()) {
        return vMenu[index];
    }

    return NULL;
}

int GUI_List::getCellIndex(GUI_ListCell* cell) {
    int index = -1;

    for (int i = 0; i < (int)vMenu.size(); i++) {
        if (cell == vMenu[i])
            return i;
    }

    return index;
}

void GUI_List::updateSubLayout() {
    disable();

    numCells = vMenu.size();
    size_t cch = 0;

    if (cellHeight == 0) {
        cch = 25;
    } else
        cch = cellHeight;

    int spw = tw_area.w + _padding * 2;
    int sph = cch * numCells;

    if (scrollPad == NULL) {
        scrollPad = new GUI_View(this, "ScrollPad", 0, 0, spw, sph, cClear);
        scrollPad->dragable = false;
        scrollPad->setBorder(false);
    } else {
        scrollPad->tw_area.w = spw;
        scrollPad->tw_area.h = sph*GUI_scale;
    }

    if (preferedHight == 0)
        tw_area.h = cch * numCells;

    scrollable = false;

    if (scrollPad->tw_area.h > tw_area.h) {
        scrollable = true;
        scrollPad->tw_area.w = tw_area.w - 12 * GUI_scale;
        //GUI_LogVerbose("GUI_List will scrollable");
    } else {
        scrollPad->tw_area.w = tw_area.w;
        // GUI_LogVerbose("GUI_List not scrollable");
    }

    int w = scrollable ? 0 * GUI_scale : 0;
    GUI_Rect r = getAbsoluteArea();
    int ww = r.w * GUI_scale - w * GUI_scale;

    for (size_t i = 0; i < numCells; i++) {
        vMenu[i]->tw_area.w = ww;
    }

    if (popupMode) {
        GUI_mouseCapturedWindow = this;
    }

    // fix content position
    //                    |----view----|
    //              vpy0->|            |<-vpy1
    //
    // case 1: content <= view
    // rule: cty0 <= vpy0
    //                    |----view----|
    //              vpy0->|            |<-vpy1
    // right:             |-content-|
    // wrong:                |-content-|
    // wrong:               |-content-|
    //
    // case 2: content > view
    // rule: cty0 <= vpy0 && cty1 >= vpy1
    //                    |----view----|
    //              vpy0->|            |<-vpy1
    // right:             |----content-----|
    // right:         |----content-----|
    // wrong:               |----content-----|
    // wrong:       |----content-----|
    //
    GUI_Rect* pRect_viewport = &tw_area;
    GUI_Rect* pRect_content = &scrollPad->tw_area;
    int vph = pRect_viewport->h;
    int vpy0 = pRect_viewport->y;
    int vpy1 = vpy0 + pRect_viewport->h;

    int cth = pRect_content->h;
    int cty0 = pRect_content->y;
    int cty1 = cty0 + cth;
    if (cth <= vph) {
        if (cty0 > vpy0) {
            scrollPad->move(0, vpy0 - cty0);
        }
    } else {
        if (cty0 > vpy0) {
            scrollPad->move(0, vpy0 - cty0);
        } else if (cty1 < vpy1) {
            scrollPad->move(0, vpy1 - cty1);
        }
    }

    enable();
}

GUI_ListCell *GUI_List::getCell(int tag) {
    for (int i = 0; i < (int)vMenu.size(); i++) {
        GUI_ListCell *cell = vMenu.at(i);

        if (cell->tag == tag) {
            return cell;
        }
    }
    return NULL;
}

int GUI_List::setCheck(int tag) {
    int ret = -1;

    for (size_t i = 0; i < vMenu.size(); i++) {
        GUI_ListCell *cell = vMenu.at(i);

        if (cell->tag == tag) {
            cell->checked = true;
            ret = i;
        } else {
            cell->checked = false;
        }
    }

    return ret;
}

void GUI_List::draw() {
    if (current != target && scrollPad != NULL) {
        float flerp = (target - current) * 0.1f;
        int ilerp = target < current ? (int)floor(flerp) : (int)ceil(flerp);
        current += ilerp;
        scrollPad->move(0, ilerp);
    }

    GUI_View::draw();

    if (corner) {
        GUI_FillRoundRect(0, 0, tw_area.w, tw_area.h, 8, bgcol);
    } else {
        GUI_FillRect(0, 0, tw_area.w, tw_area.h, bgcol);

        if (_border) {
            GUI_DrawRect(0, 0, tw_area.w, tw_area.h, sdl_color(0x1F1F1FFF));
        }
    }

    for (size_t i = 0; i < vMenu.size(); i++) {
        GUI_ListCell *cell = vMenu.at(i);

        if (i == selectedIndex) {
            cell->selected = true;
        } else {
            cell->selected = false;
        }
    }

    if (scrollable) {
        int w = 11 * GUI_scale;
        //SDL_Color c = {64, 64, 64, 227};
        SDL_Color c = {0xf0, 0xf0, 0xf0, 255};
        GUI_FillRect(tw_area.w - w - GUI_scale, 0, w, tw_area.h, c);
        //GUI_DrawLine(tw_area.w - w, 0, tw_area.w - w, tw_area.h, sdl_color(0xdfdfdfFF));

        int dh = scrollPad->tw_area.h - scrollPad->parent->tw_area.h;
        int dy = abs(scrollPad->tw_area.y - scrollPad->parent->tw_area.y);
        float ratio = ((float)dy / (float)dh);

        int bw = 9 * GUI_scale;
        int bh = 24 * GUI_scale;
        int y = (int)(scrollPad->parent->tw_area.h * ratio);

        if (y > scrollPad->parent->tw_area.h - bh - GUI_scale)
            y = scrollPad->parent->tw_area.h - bh - GUI_scale;

        if (y < GUI_scale)
            y = GUI_scale;

        GUI_FillRoundRect(tw_area.w - bw - 2 * GUI_scale, y, bw, bh, (bw + 1) / 2, sdl_color(0xc0c0c0ff));
    }
}

void GUI_List::setSelectedIndex(int n) {
    if (n >= 0 && n < (int)vMenu.size()) {
        if (vMenu[n]->enable && !vMenu[n]->separator) {
            selectedIndex = n;

            if (scrollPad != nullptr) {
                GUI_ListCell *cell = vMenu.at(selectedIndex);

                if ((cell->tw_area.y + cell->tw_area.h) >= (tw_area.y + tw_area.h)) {
                    int dy = (tw_area.y + tw_area.h) - (cell->tw_area.y + cell->tw_area.h);
                    setTarget(dy);
                }

                if (cell->tw_area.y < tw_area.y) {
                    int dy = tw_area.y - cell->tw_area.y;
                    setTarget(dy);
                }
            }
        }
    }
    else if( n == -1 )
        selectedIndex = -1;
}

void GUI_List::selectChanged(int n) {
    if (n >= 0 && n < (int)vMenu.size()) {
        if (vMenu[n]->enable && !vMenu[n]->separator) {
            selectedIndex = n;

            if (scrollPad != nullptr) {
                GUI_ListCell *cell = vMenu.at(selectedIndex);

                if ((cell->tw_area.y + cell->tw_area.h) >= (tw_area.y + tw_area.h)) {
                    int dy = (tw_area.y + tw_area.h) - (cell->tw_area.y + cell->tw_area.h);
                    setTarget(dy);
                }

                if (cell->tw_area.y < tw_area.y) {
                    int dy = tw_area.y - cell->tw_area.y;
                    setTarget(dy);
                }
            }

            if (cmd) {
                GUI_ListCell *cell = vMenu.at(selectedIndex);
                cmd(this, cell->title.c_str(), selectedIndex);
            }
        }
    }

    if (vMenu.size() == 0)
        selectedIndex = -1;
}

bool GUI_List::handleEvents(SDL_Event *ev) {
    if (!isEnable()) {
        return false;
    }

    static float prevTouchInMS = 0;

    switch (ev->type) {
#ifdef _RPi
        case SDL_FINGERDOWN:
#endif
        case SDL_MOUSEBUTTONDOWN:
        {
            prevTouchInMS = (float)SDL_GetTicks();

            int x = 0;
            int y = 0;

            if (ev->type == SDL_MOUSEBUTTONDOWN) {
                SDL_MouseButtonEvent e = ev->button;
                x = (int)(e.x*GUI_mouseScale);
                y = (int)(e.y*GUI_mouseScale);
            } else {
                SDL_TouchFingerEvent e = ev->tfinger;
                x = (int)(e.x*GUI_windowWidth / GUI_mouseScale);
                y = (int)(e.y*GUI_windowHeight / GUI_mouseScale);
            }

            lastMouseX = x;
            lastMouseY = y;

            if (popupMode) {
                if (!this->hitTest(x, y)) {
                    return true;
                }
            }

            bMousePress = true;
            mouseClickTime = SDL_GetTicks();

            if (!popupMode) {
                if (scrollable || bHilightCellWhenClick)
                    GUI_mouseCapturedWindow = this;
            }

            if(scrollPad){
                GUI_View *w = scrollPad->hitTest(x, y);
                if( w && w->parent && w->parent->parent == scrollPad ) {
                    w->handleEvents(ev);
                    w = NULL;
                    if (popupMode) {
                        GUI_mouseCapturedWindow = this;
                    }
                }

                if (w && w != scrollPad) {
                    selectedIndex = w->tag;
                }
            }

            if (handle_event_cmd) {
                handle_event_cmd(this, ev);
            }

            return true;
        }

#ifdef _RPi
        case SDL_FINGERMOTION:
#endif
        case SDL_MOUSEMOTION:
        {
            int x = 0;
            int y = 0;

            if (ev->type == SDL_MOUSEMOTION) {
                SDL_MouseButtonEvent e = ev->button;
                x = (int)(e.x * GUI_mouseScale);
                y = (int)(e.y * GUI_mouseScale);
            } else {
                SDL_TouchFingerEvent e = ev->tfinger;
                x = (int)(e.x * GUI_windowWidth / GUI_mouseScale);
                y = (int)(e.y * GUI_windowHeight / GUI_mouseScale);
            }
            //GUI_Log( "List Mouse: %i, %i\n", x, y );

            if (bMousePress) {
                if (abs(x - lastMouseX) > 5 || abs(y - lastMouseY) > 5) {
                    if (scrollable) {
                        bMousePress = false;
                        //                        dragging = true;
                        _dragging = true;
                    }
                }
            }

            if (_dragging && scrollPad) {
                int diffY = y - lastMouseY;
                scrollPad->move(0, diffY);

                if (scrollPad->tw_area.y - scrollPad->parent->tw_area.y > 0) {
                    scrollPad->move(0, scrollPad->parent->tw_area.y - scrollPad->tw_area.y);
                }

                if (scrollPad->tw_area.y + scrollPad->tw_area.h < scrollPad->parent->tw_area.y + tw_area.h) {
                    scrollPad->move(0, (scrollPad->parent->tw_area.y + tw_area.h) - (scrollPad->tw_area.y + scrollPad->tw_area.h));
                }

                lastMouseX = x;
                lastMouseY = y;
                
                if (onScrollPadChanged) {
                    onScrollPadChanged(this);
                }
            } else if (!bHilightCellWhenClick && scrollPad) {
                GUI_View *w = scrollPad->hitTest(x, y, true);
                if( w && w->parent && w->parent->parent == scrollPad )
                    w = w->parent;

                if (w && w->parent == scrollPad) {
                    if (selectedIndex != w->tag) {
                        selectedIndex = w->tag;

                        if (onHilightChanged) {
                            onHilightChanged(this);
                        }
                    }
                }
                else {
                    if (selectedIndex != -1) {
                        selectedIndex = -1;

                        if (onHilightChanged) {
                            onHilightChanged(this);
                        }
                    }
                }
            }

            return true;
        }

#ifdef _RPi
        case SDL_FINGERUP:
#endif
        case SDL_MOUSEBUTTONUP:
        {
            int x = 0;
            int y = 0;

            if (ev->type == SDL_MOUSEBUTTONUP) {
                SDL_MouseButtonEvent e = ev->button;
                x = (int)(e.x * GUI_mouseScale);
                y = (int)(e.y * GUI_mouseScale);
            } else {
                SDL_TouchFingerEvent e = ev->tfinger;
                x = (int)(e.x * GUI_windowWidth / GUI_mouseScale);
                y = (int)(e.y * GUI_windowHeight / GUI_mouseScale);
            }

            if (_dragging) {
                _dragging = false;
            }

            if (!popupMode) {
                GUI_mouseCapturedWindow = NULL;
            } else {
                if (!this->hitTest(x, y)) {
                    if( popupAutoClose )
                        close();
                    else
                        hide();
                    return true;
                }
            }

            if (!bMousePress)
                break;

            if (abs(x - lastMouseX) > 5 || abs(y - lastMouseY) > 5) {
                if (scrollable) {
                    break;
                }
            }

            if (scrollPad) {
                GUI_View *w = scrollPad->hitTest(x, y);
                if( w && w->parent && w->parent->parent == scrollPad ) {
                    w->handleEvents(ev);
                    w = NULL;
                    if (popupMode) {
                        GUI_mouseCapturedWindow = this;
                    }
                }
                if (w) {
                    float now = (float)SDL_GetTicks();

                    if (now - prevTouchInMS <= 200.0f) {
                        if (popupMode) {
                            if( popupAutoClose )
                                close();
                            else
                                hide();
                        }
                    }

                    selectChanged(w->tag);
                }

                bMousePress = false;
                return true;
            }
            break;
        }

        case SDL_MOUSEWHEEL:
        {
            if (scrollable && !_disable && !_hidden) {
                int x = 0;
                int y = 0;
                SDL_GetMouseState(&x, &y);  // Tooh - fix mouse wheel
                x = (int)(x * GUI_mouseScale);
                y = (int)(y * GUI_mouseScale);

                if (this->hitTest(x, y)) {
                    SDL_MouseWheelEvent e = ev->wheel;
                    int y = (int)(e.y * GUI_mouseScale);

                    if (!scrollPad) return true;

                    scrollPad->move(0, y * 16);

                    if (scrollPad->tw_area.y - scrollPad->parent->tw_area.y > 0) {
                        scrollPad->move(0, scrollPad->parent->tw_area.y - scrollPad->tw_area.y);
                    } else if (scrollPad->tw_area.y + scrollPad->tw_area.h <
                               scrollPad->parent->tw_area.y + tw_area.h) {
                        if (scrollPad->tw_area.h < tw_area.h) {
                            scrollPad->move(0, tw_area.y - scrollPad->tw_area.y);
                        } else {
                            scrollPad->move(0,
                                            (scrollPad->parent->tw_area.y + tw_area.h) -
                                                    (scrollPad->tw_area.y + scrollPad->tw_area.h));
                        }
                    }

                    return true;
                }
            }
            break;
        }

        default:
            GUI_View::handleEvents(ev);
            break;
    }

    return false;
}

void GUI_List::hide() {
    if (popupMode) {
        GUI_mouseCapturedWindow = NULL;
    }

    GUI_View::hide();
}

void GUI_List::show() {
    if (popupMode) {
        GUI_mouseCapturedWindow = this;
    }

    GUI_View::show();
}

void GUI_List::setReverseList(bool reverse) {
    breverse = reverse;
}

