//
// Created by Panutat Tejasen on 3/13/2016 AD.
//

#include "GUI_EditText.h"
#include "GUI_ButtonFontAwesome.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Tooh - text selection: contextual menu
#if defined(WIN32)
#include <SDL_syswm.h>

HMENU textSelectionContextualMenu = NULL;
UINT textSelectionContextualMenuCut = 0;
UINT textSelectionContextualMenuCopy = 0;
UINT textSelectionContextualMenuPaste = 0;
UINT textSelectionContextualMenuDelete = 0;
UINT textSelectionContextualMenuSelectAll = 0;
#endif

static const char* textSelectionContextualMenuText[] = {
    "Cut",
    "Copy",
    "Paste",
    "Delete",
    "---",
    "Select All"
};

GUI_List* textSelectionContextualMenuList = NULL;
//extern GUI_View *topView; // pom - use GUI_TopView instead
extern void updatePopupMenuListPosition(GUI_List* submenu);

bool __gui_key_ctrl_pressed = false;
bool GUI_IsKeyCTRLPressed(){
    return __gui_key_ctrl_pressed;
}

void updatePopupMenuListPosition(GUI_List* submenu) {
    if (submenu) {
        auto topView = GUI_TopView;
        int left = submenu->tw_area.x / GUI_scale;
        int right = left + (submenu->tw_area.w / GUI_scale) - 1;
        int top = submenu->tw_area.y / GUI_scale;
        int bottom = top + (submenu->tw_area.h / GUI_scale) - 1;
        int windowLeft = (topView->tw_area.x / GUI_scale) + topView->padding[0];
        int windowRight = windowLeft + (topView->tw_area.w / GUI_scale) - 1 - topView->padding[1];
        int windowTop = (topView->tw_area.y / GUI_scale) + topView->padding[0];
        int windowBottom = windowTop + (topView->tw_area.h / GUI_scale) - 1 - topView->padding[1];

        if (left < windowLeft) {
            left = windowLeft;
        } else if (right > windowRight) {
            int different = right - windowRight;
            left -= different;
        }

        if (top < windowTop) {
            top = windowTop;
        } else if (bottom > windowBottom) {
            int different = bottom - windowBottom;
            top -= different;
        }

        submenu->setPosition(left, top);
    }
}

GUI_EditText *GUI_EditText::create(GUI_View *parent, const char *title, int x, int y, int width, int height) {
    GUI_EditText *edit = new GUI_EditText(parent, title, x, y, width, height, cBlack, GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER, GUI_FONT_NORMAL);
    edit->bgcol = cWhite;
    edit->setPadding(5, 5, 5, 5);
    return edit;
}

GUI_EditText::GUI_EditText(GUI_View *p, const char *t, int x, int y, int w, int h,
                           SDL_Color col, int align, int fnt, void(*act_cmd)(GUI_EditText *)) :
    GUI_Label(p, t, x, y, w, h, col, align, fnt),
    text_index(0),
    font_size(fnt),
    _type(Type::TEXT_ANY),
    onEnter(NULL),
    isPassword(false),
    isPasswordChanged(false) {
    setTitle(" ", fnt);
    min_width = title_area.w;
    min_height = title_area.h;
    setTitle(t, fnt);

    action_cmd = act_cmd;
    bgcol = cWhite;
    setBorder(1);
    setPadding(5, 5, 5, 5);

    // Tooh - text selection
    textSelectionIsShiftPressed = false;
    textSelectionIsMouseDown = false;
    textSelectionMousePoint.x = 0;
    textSelectionMousePoint.y = 0;
    touchTime = 0.0f;
    isJustSelectAll = false;
    textSelectionCreateContextualMenuList();
}

GUI_EditText::~GUI_EditText() {
    if (textSelectionContextualMenuList) {
        textSelectionContextualMenuList->close();
    }
}

void GUI_EditText::draw() {
    // Tooh - text selection supports only left alignment
    if (title_align != (GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER)) {
        title_align = GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER;
#if __USE_SKIA__
    createTitleImage(font_size);
#else
    createTitleTexture(font_size);
#endif
        updateSubLayout();
    }

    string saveTitle;

    // Tooh - reset Thai title
    if (isPassword && isPasswordChanged != isPassword && textSelectionIsThaiText(title)) {
        title = "";
    }

    if (isPassword) {
        saveTitle = title;
        title = string(saveTitle.length(), '*');
    }
    
    if (isPasswordChanged != isPassword) {
        isPasswordChanged = isPassword;
#if __USE_SKIA__
    createTitleImage(font_size);
#else
    createTitleTexture(font_size);
#endif
        updateSubLayout();
    }

    GUI_Label::draw();

    if (isActive()) {
        long currentTime = SDL_GetTicks() / 500;

        if (currentTime & 1) {
            int w, h;

            TTF_SizeUTF8(GUI_fonts[font_size], title.substr(textSelectionScrollIndex, (size_t)text_index - textSelectionScrollIndex).c_str(), &w, &h);
            int x = title_area.x + w;
            
            if (textureIcon && title.length() == 0) {
                x += icon_gap * GUI_scale;
            }

            SDL_SetRenderDrawColor(GUI_renderer, cBlack.r, cBlack.g, cBlack.b, cBlack.a);
            SDL_RenderDrawLine(GUI_renderer, x, padding[0] * GUI_scale, x, tw_area.h - padding[2] * GUI_scale);

            // Tooh - text selection
            textSelectionMouseMove();
        }
    }

    if (isPassword) {
        title = saveTitle;
    }
}

bool GUI_EditText::handleEvents(SDL_Event *ev) {
    SDL_Scancode scancode; /* scancode of last key we pushed */
    switch (ev->type) {
        case SDL_MOUSEBUTTONDOWN:
        {
            if( this->isVisible() == false )
                return false;
            touchTime = (float)SDL_GetTicks();

            if (this->isEnable() == false) {
                textSelectionCancel();
                return false;
            }

            SDL_MouseButtonEvent e = ev->button;
            int x = (int)(e.x * GUI_mouseScale);
            int y = (int)(e.y * GUI_mouseScale);

            textSelectionIsShiftPressed = false;
            bool isHandleEventCmd = false; // Tooh - flag to perform handle_event_cmd

            if (hitTest(x, y)) {
                GUI_mouseCapturedWindow = this;
                bool setCaret = false;

                if (e.button == SDL_BUTTON_LEFT) {
                    if (isFocus) {
                        if (!isJustSelectAll) {
                            textSelectionCancel();
                            SDL_StartTextInput();
                            setCaret = true;
                            textSelectionShowContextualMenuList(false);
                        }
                    } else {
                        setFocus();
                        setCaret = true;
                    }

                    isHandleEventCmd = true;
                } else if (e.button == SDL_BUTTON_RIGHT) { // Tooh - add right-click
                    if (!isFocus) {
                        setFocus();
                        setCaret = true;
                        isHandleEventCmd = true;
                    }
                }

                if (setCaret) {
                    textSelectionSetCaretPosition(x, y);
                }
            } else {
                killFocus();
                return false;
            }

            if (isHandleEventCmd) { // Tooh - perform handle_event_cmd
                if (handle_event_cmd) {
                    handle_event_cmd(this, ev);
                }

                return true;
            }

            return false;
        }

        case SDL_MOUSEMOTION:
        {
            if (this->isEnable() == false) {
                textSelectionCancel();
                return false;
            }

            SDL_MouseButtonEvent e = ev->button;
            int x = (int)(e.x * GUI_mouseScale);
            int y = (int)(e.y * GUI_mouseScale);

            if (e.button == SDL_BUTTON_LEFT) {
                // Tooh - text selection
                if (isFocus && textSelectionIsMouseDown && textSelectionMousePoint.x != x) {
                    textSelectionMousePoint.x = x;
                    textSelectionMousePoint.y = y;
                    textSelectionMouseMove();
                    return true;
                }
            }

            return false;
        }

        case SDL_MOUSEBUTTONUP:
        {
            if (this->isEnable() == false) {
                textSelectionCancel();
                return false;
            }

            SDL_MouseButtonEvent e = ev->button;
            int x = (int)(e.x * GUI_mouseScale);
            int y = (int)(e.y * GUI_mouseScale);

            if (e.button == SDL_BUTTON_LEFT || e.button == SDL_BUTTON_RIGHT) {
                // Tooh - text selection
                if (isFocus) {
                    if (e.button == SDL_BUTTON_RIGHT && !(isJustSelectAll || textSelectionIsSelected())) {
                        textSelectionSetCaretPosition(x, y);
                    }

                    textSelectionIsMouseDown = false;
                    if (isJustSelectAll || textSelectionIsSelected() || e.button == SDL_BUTTON_RIGHT || (float)SDL_GetTicks() - touchTime >= 500.0f) {
                        isJustSelectAll = false;
                        textSelectionShowContextualMenuList(true, x, y);
                    }
                    return false;
                } else {
                    textSelectionCancel();
                }
            /*} else if (e.button == SDL_BUTTON_RIGHT) {
#if defined(WIN32)
                if (textSelectionContextualMenu && hitTest(x, y) && isFocus) {
                    touchTime = 0.0f;
                    textSelectionShowContextualMenuList(false);

                    bool canCutCopy = (!isPassword && textSelectionIsSelected()) ? MF_ENABLED : MF_GRAYED;
                    bool canPaste = textSelectionIsValidClipboardText() ? MF_ENABLED : MF_GRAYED;
                    bool canDelete = textSelectionIsSelected() ? MF_ENABLED : MF_GRAYED;

                    int startIndex = 0;
                    int endIndex = 0;
                    textSelectionGetSelectedIndex(&startIndex, &endIndex);
                    bool canSelectAll = (title.length() > 0 && !(startIndex == 0 && endIndex == title.length())) ? MF_ENABLED : MF_GRAYED;

                    EnableMenuItem(textSelectionContextualMenu, textSelectionContextualMenuCut, canCutCopy);
                    EnableMenuItem(textSelectionContextualMenu, textSelectionContextualMenuCopy, canCutCopy);
                    EnableMenuItem(textSelectionContextualMenu, textSelectionContextualMenuPaste, canPaste);
                    EnableMenuItem(textSelectionContextualMenu, textSelectionContextualMenuDelete, canDelete);
                    EnableMenuItem(textSelectionContextualMenu, textSelectionContextualMenuSelectAll, canSelectAll);

                    SDL_SysWMinfo info = {0};
                    SDL_VERSION(&info.version);
                    SDL_GetWindowWMInfo(GUI_window, &info);
                    HWND hwnd = info.info.win.window;
                    POINT point = {0};
                    GetCursorPos(&point);
                    TrackPopupMenu(GetSubMenu(textSelectionContextualMenu, 0), TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
                }
#endif

                return true;*/
            }

            return false;
        }

        case SDL_KEYDOWN:
        {
            //printf("[%p][%d]SDL_KEYD>text_index: %d\n", this, ev->key.repeat, text_index);
            if( !isFocus )
                return false;
            if( !isVisible() )
                return false;

            bool ok = true;
            
            SDL_Scancode scancode = ev->key.keysym.scancode;
            if (scancode == SDL_SCANCODE_LCTRL || scancode == SDL_SCANCODE_RCTRL) {
                __gui_key_ctrl_pressed = true;
            }

            if (textSelectionContextualMenuList && textSelectionContextualMenuList->isVisible() && textSelectionContextualMenuList->__handleInputEvent) {
                int selIndex = textSelectionContextualMenuList->selectedIndex;

                if (ev->key.keysym.sym == SDLK_UP) {
                    ok = false;
                    if (selIndex > 0) {
                        for (int i = selIndex - 1; i >= 0; i--) {
                            if (textSelectionContextualMenuList->vMenu[i]->enable && textSelectionContextualMenuList->vMenu[i]->separator == false) {
                                textSelectionContextualMenuList->setSelectedIndex(i);
                                break;
                            }
                        }
                    }
                } else if (ev->key.keysym.sym == SDLK_DOWN) {
                    ok = false;
                    for (int i = selIndex + 1; i < textSelectionContextualMenuList->getSize(); i++) {
                        if (textSelectionContextualMenuList->vMenu[i]->enable && textSelectionContextualMenuList->vMenu[i]->separator == false) {
                            textSelectionContextualMenuList->setSelectedIndex(i);
                            break;
                        }
                    }
                } else if (ev->key.keysym.sym == SDLK_ESCAPE) {
                    ok = false;
                    textSelectionShowContextualMenuList(false);
                } else if (ev->key.keysym.sym == SDLK_RETURN || ev->key.keysym.sym == SDLK_KP_ENTER) {
                    ok = false;
                    textSelectionContextualMenuList->selectChanged(selIndex);
                }
            }
            
            if (ok) {
                textSelectionShowContextualMenuList(false);

                if (this->isEnable() == false) {
                    textSelectionCancel();
                    return false;
                }

                bool isCtrlPressed = ev->key.keysym.mod & CUSTOM_KMOD_CTRL;

                if (isCtrlPressed) {
                    if (ev->key.keysym.sym == SDLK_a) {
                        textSelectionSelectAll();
                    } else if (ev->key.keysym.sym == SDLK_x) {
                        textSelectionCut();
                    } else if (ev->key.keysym.sym == SDLK_c) {
                        textSelectionCopy();
                    } else if (ev->key.keysym.sym == SDLK_v) {
                        textSelectionPaste();
                    }
                } else {
                    scancode = ev->key.keysym.scancode;

                    bool isUpdateSubLayout = false;
                    bool isResetScrollIndex = false;
                    bool doActionCmd = false;
                    int len = (int)title.length();
                    int oi = text_index;

                    if (scancode == SDL_SCANCODE_RETURN || scancode == SDL_SCANCODE_KP_ENTER) {
                        if (onEnter) {
                            onEnter(this);
                        }
                    }
                    else if (scancode == SDL_SCANCODE_LSHIFT || scancode == SDL_SCANCODE_RSHIFT) {
                        if (!textSelectionIsShiftPressed) {
                            textSelectionIsShiftPressed = true;

                            if (!textSelectionIsSelected()) {
                                textSelectionStartIndex = text_index;
                                textSelectionEndIndex = textSelectionStartIndex;
                            }
                        }
                    } else if (scancode == SDL_SCANCODE_HOME) {
                        textSelectionHome();
                    } else if (scancode == SDL_SCANCODE_END) {
                        textSelectionEnd();
                    } else if (scancode == SDL_SCANCODE_LEFT) {
                        text_index = textSelectionGetPreviousMiddleCharacter(text_index);

                        if (oi != text_index && text_index < textSelectionScrollIndex) {
                            textSelectionScrollIndex = text_index;
                            isUpdateSubLayout = true;
                        }

                        if (textSelectionIsShiftPressed) {
                            textSelectionEndIndex = text_index;
                            isUpdateSubLayout = true;
                        } else if (textSelectionIsSelected()) {
                            int startIndex = 0;
                            int endIndex = 0;
                            textSelectionGetSelectedIndex(&startIndex, &endIndex);
                            text_index = startIndex;
                            textSelectionStartIndex = text_index;
                            textSelectionEndIndex = textSelectionStartIndex;
                            isUpdateSubLayout = true;
                        }
                    } else if (scancode == SDL_SCANCODE_RIGHT) {
                        text_index = textSelectionGetNextMiddleCharacter(text_index);

                        if (oi != text_index) {
                            int areaWidth = textSelectionGetAreaWidth();
                            int w = 0;
                            int h = 0;
                            TTF_SizeUTF8(GUI_fonts[font_size], title.substr(textSelectionScrollIndex, (size_t)text_index - textSelectionScrollIndex).c_str(), &w, &h);
                            if (w > areaWidth) {
                                isUpdateSubLayout = true;
                            }
                        }

                        if (textSelectionIsShiftPressed) {
                            textSelectionEndIndex = text_index;
                            isUpdateSubLayout = true;
                        } else if (textSelectionIsSelected()) {
                            int startIndex = 0;
                            int endIndex = 0;
                            textSelectionGetSelectedIndex(&startIndex, &endIndex);
                            text_index = endIndex;
                            textSelectionStartIndex = text_index;
                            textSelectionEndIndex = textSelectionStartIndex;
                            isUpdateSubLayout = true;
                        }
                    } else if (scancode == SDL_SCANCODE_DELETE) {
                        if (textSelectionIsSelected()) {
                            textSelectionDelete(false);
                            isUpdateSubLayout = true;
                            doActionCmd = true;
                        } else {
                            int nextIndex = textSelectionGetNextMiddleCharacter(text_index);

                            if (text_index < len) {
                                title = title.substr(0, text_index) + title.substr(nextIndex);
                                isUpdateSubLayout = true;
                                doActionCmd = true;
                            }
                        }
                    } else if (scancode == SDL_SCANCODE_BACKSPACE) {
                        if (textSelectionIsSelected()) {
                            textSelectionDelete(false);
                            isUpdateSubLayout = true;
                            doActionCmd = true;
                        } else {
                            if (isFocus == false) {
                                return false;
                            }
                            text_index = textSelectionGetPreviousIndex(text_index);

                            if (oi != text_index) {
                                int areaWidth = textSelectionGetAreaWidth();
                                int w = 0;
                                int h = 0;
                                TTF_SizeUTF8(GUI_fonts[font_size], title.substr(textSelectionScrollIndex).c_str(), &w, &h);
                                title = title.substr(0, text_index) + title.substr(oi);

                                if (w <= areaWidth) {
                                    int prevIndex = textSelectionGetPreviousMiddleCharacter(textSelectionScrollIndex);
                                    TTF_SizeUTF8(GUI_fonts[font_size], title.substr(prevIndex).c_str(), &w, &h);
                                    if (w <= areaWidth) {
                                        textSelectionScrollIndex = prevIndex;
                                    }
                                } else if (text_index < textSelectionScrollIndex) {
                                    w = 0;
                                    while (textSelectionScrollIndex > 0 && w < areaWidth / 2) {
                                        textSelectionScrollIndex = textSelectionGetPreviousMiddleCharacter(textSelectionScrollIndex);
                                        TTF_SizeUTF8(GUI_fonts[font_size], title.substr(textSelectionScrollIndex, (size_t)text_index - textSelectionScrollIndex).c_str(), &w, &h);
                                    }
                                }

                                isUpdateSubLayout = true;
                                doActionCmd = true;
                            }
                        }
                    }

                    if (isUpdateSubLayout) {
                        textSelectionUpdate(isResetScrollIndex);

                        if (doActionCmd && action_cmd) {
                            action_cmd(this);
                            setFocus(); // Jimmy fix focus
                        }
                    }
                }
            }

            //printf("[%p][%d]SDL_KEYD<text_index: %d\n", this, ev->key.repeat, text_index);
            return true;
        }

        case SDL_KEYUP:
        {
            //printf("[%p][%d]SDL_KEYU>text_index: %d\n", this, ev->key.repeat, text_index);
            if( !isFocus )
                return false;
            if( !isVisible() )
                return false;

            
            SDL_Scancode scancode = ev->key.keysym.scancode;
            if (scancode == SDL_SCANCODE_LCTRL || scancode == SDL_SCANCODE_RCTRL) {
                __gui_key_ctrl_pressed = false;
            }
            
            if (textSelectionContextualMenuList && textSelectionContextualMenuList->isVisible() && textSelectionContextualMenuList->__handleInputEvent) {
            } else {
                textSelectionShowContextualMenuList(false);

                if (this->isEnable() == false) {
                    textSelectionCancel();
                    return false;
                }

                scancode = ev->key.keysym.scancode;

                if (scancode == SDL_SCANCODE_LSHIFT || scancode == SDL_SCANCODE_RSHIFT) {
                    if (textSelectionIsShiftPressed) {
                        textSelectionIsShiftPressed = false;
                    }
                }

                if (isFocus == false) {
                    return false;
                }
            }

            //printf("[%p][%d]SDL_KEYU<text_index: %d\n", this, ev->key.repeat, text_index);
            return true;
        }

        case SDL_TEXTINPUT:
        {
            if( !isVisible() )
                return false;
            
            if (isFocus == false) {
                return false;
            }
            textSelectionShowContextualMenuList(false);

            if (this->isEnable() == false) {
                textSelectionCancel();
                return false;
            }

            // Tooh - text selection
            bool doActionCmd = false;
            if (textSelectionIsSelected()) {
                textSelectionDelete(false);
                doActionCmd = true;
            }

            // pom
            if (_type == Type::TEXT_ANY) {
                if (!isPassword || (isPassword && (ev->text.text[0] & 0xc0) != 0xc0)) { // Tooh - disable Thai characters in password mode
                    title = title.insert(text_index, std::string(ev->text.text));
                    text_index += (int)strlen(ev->text.text);
                    doActionCmd = true;
                }
            } else if (_type == Type::TEXT_DIGIT) {
                char *c = ev->text.text;

                if (SDL_isdigit(c[0]) || c[0] == '.') {
                    title = title.insert(text_index, std::string(ev->text.text));
                    text_index += (int)strlen(ev->text.text);
                    doActionCmd = true;
                }
            }

            textSelectionIsShiftPressed = ev->key.keysym.mod & KMOD_SHIFT;

            if (textSelectionIsShiftPressed) {
                textSelectionStartIndex = text_index;
                textSelectionEndIndex = textSelectionStartIndex;
            }

            textSelectionUpdate(false);

            if (doActionCmd && action_cmd) {
                action_cmd(this);
                setFocus(); // Jimmy fix lost focus
            }

            return true;
        }

#if defined(WIN32)
        // Tooh - text selection: contextual menu
        case SDL_SYSWMEVENT:
            if (ev->syswm.msg->msg.win.msg == WM_COMMAND) {
                if (textSelectionContextualMenu) {
                    DWORD menuId = LOWORD(ev->syswm.msg->msg.win.wParam);

                    if (menuId == textSelectionContextualMenuCut) {
                        textSelectionCut();
                    } else if (menuId == textSelectionContextualMenuCopy) {
                        textSelectionCopy();
                    } else if (menuId == textSelectionContextualMenuPaste) {
                        textSelectionPaste();
                    } else if (menuId == textSelectionContextualMenuDelete) {
                        textSelectionDelete();
                    } else if (menuId == textSelectionContextualMenuSelectAll) {
                        textSelectionSelectAll();
                    }

                    return false;
                }
            }
            break;
#endif

        default:
            return GUI_Label::handleEvents(ev);
            break;
    }

    return true;
}

void GUI_EditText::setTitle(std::string t, int font_size) {
    if (font_size == -1)
        font_size = this->fontSize;

    string saveTitle = "";

    if (isPassword) {
        saveTitle = t;
        title = string(saveTitle.length(), '*');
    } else {
        title = std::string(t);
    }

#if __USE_SKIA__
    createTitleImage(font_size);
#else
    createTitleTexture(font_size);
#endif

    if (isPassword) {
        title = saveTitle;
    }

    updateSubLayout();
}

bool GUI_EditText::setText(std::string newText) {
    bool hasChange = false;

    if (title != newText) {
        hasChange = true;
        setTitle(newText, font_size);

        if (action_cmd) {
            action_cmd(this);
        }
    }

    return hasChange;
}

void GUI_EditText::setFocus() {
    if( tag == 0 ) {
        GUI_LogNotice() << "0";
    }
    GUI_LogNotice() << "SetFocus " << title << " " << tag;
    if( GUI_focusWindow ) {
        GUI_focusWindow->isFocus = false;
    }
    isFocus = true;
    GUI_focusWindow = this;
    GUI_mouseCapturedWindow = this;
    SDL_StartTextInput();
}

void GUI_EditText::killFocus() {
    if( GUI_focusWindow == this )
        GUI_focusWindow = NULL;
    GUI_LogNotice() << "KillFocus " << title << " " << tag;
    isFocus = false;
    textSelectionIsShiftPressed = false;
    textSelectionIsMouseDown = false;
    textSelectionMousePoint.x = 0;
    textSelectionMousePoint.y = 0;
    touchTime = 0.0f;
    isJustSelectAll = false;

    SDL_StopTextInput();

    if (textSelectionContextualMenuList && textSelectionContextualMenuList->isVisible()) {
        textSelectionContextualMenuList->close();
    }

    if (GUI_mouseCapturedWindow && GUI_mouseCapturedWindow == this) {
        GUI_mouseCapturedWindow = NULL;
    }
}

// Tooh - text selection
int GUI_EditText::textSelectionGetIndexAtScreenPosition(int x) {
    if ((int)title.length() == 0) {
        text_index = 0;
        textSelectionScrollIndex = 0;
    }

    int index = text_index;
    int left = tw_area.x + title_area.x;
    size_t j = 0;
    x += 4;

    for (size_t i = textSelectionScrollIndex; i < title.length(); i++) {
        //unsigned char c = title.at(i);
        int w = 0;
        int h = 0;
        j = textSelectionGetNextIndex(i) - i;
        TTF_SizeUTF8(GUI_fonts[font_size], title.substr(textSelectionScrollIndex, i + j - textSelectionScrollIndex).c_str(), &w, &h);

        if (left + w > x) {
            index = i;
            break;
        } else {
            index = i + j;
        }

        i = i + j - 1;
        j = 0;
    }

    return index;
}

int GUI_EditText::textSelectionGetNextIndex(size_t i) {
    size_t j = 0;
    size_t len = title.length();

    if (i < len) {
        unsigned char c = title.at(i);

        if ((c & 0x80) == 0) {
            j++;
        } else if ((c & 0xc0) == 0xc0) {
            j++;
            c = title.at(i + j);

            while ((c & 0xc0) == 0x80) {
                j++;

                if (i + j >= len)
                    break;

                c = title.at(i + j);
            }
        }
    }

    return i + j;
}

int GUI_EditText::textSelectionGetPreviousIndex(int i) {
    while (i > 0) {
        i--;
        int c = (title.at(i) & 0xC0);

        if (c != 0x80) {
            break;
        }
    }

    return i;
}

void GUI_EditText::textSelectionSetScrollIndex(bool isResetScrollIndex) {
    if (isResetScrollIndex || text_index < textSelectionScrollIndex) {
        textSelectionScrollIndex = 0;
    }

    int i = textSelectionScrollIndex;
    int areaWidth = textSelectionGetAreaWidth();
    int w = 0;
    int h = 0;
    bool success = false;

    do {
        TTF_SizeUTF8(GUI_fonts[font_size], title.substr(i, (size_t)text_index - i).c_str(), &w, &h);
        if (w > areaWidth) {
            i = textSelectionGetNextMiddleCharacter(i);
        } else {
            success = true;
        }
    } while (!success && i < (int)title.length());

    if (success) {
        textSelectionScrollIndex = i;
    }
}

bool GUI_EditText::textSelectionIsUpperLowerCharacter(int *i) {
    bool result = false;
    int index = *i;
    int c = (unsigned char)title[index++];
    int unicode = c;

    if ((c & 0xc0) == 0xc0) { // convert unicode bytes to int
        unicode = (c & 0x0F) << 12;
        c = (unsigned char)title[index++];
        unicode += (c & 0x3F) << 6;
        c = (unsigned char)title[index++];
        unicode += (c & 0x3F);
    }

    *i = index;
    result = (unicode == 0xE31 || (unicode >= 0xE34 && unicode <= 0xE3A) || (unicode >= 0xE47 && unicode <= 0xE4E));

    return result;
}

int GUI_EditText::textSelectionGetNextMiddleCharacter(int i) {
    i = textSelectionGetNextIndex(i);
    int lastIndex = i;

    while (i < (int)title.length() && textSelectionIsUpperLowerCharacter(&i)) {
        lastIndex = i;
    };

    return lastIndex;
}

int GUI_EditText::textSelectionGetPreviousMiddleCharacter(int i) {
    int previousIndex = i;

    do {
        i = textSelectionGetPreviousIndex(i);
        previousIndex = i;
    } while (i > 0 && textSelectionIsUpperLowerCharacter(&previousIndex));

    return i;
}

void GUI_EditText::textSelectionMouseMove() {
    if (isFocus && textSelectionIsMouseDown) {
        int left = tw_area.x + title_area.x;
        int areaWidth = textSelectionGetAreaWidth();
        int right = left + areaWidth;

        if (textSelectionMousePoint.x <= left) {
            textSelectionScrollIndex = textSelectionGetPreviousMiddleCharacter(textSelectionScrollIndex);
        } else if (textSelectionMousePoint.x >= right) {
            text_index = textSelectionGetNextMiddleCharacter(text_index);
        }

        text_index = textSelectionGetIndexAtScreenPosition(textSelectionMousePoint.x);
        textSelectionEndIndex = text_index;
        textSelectionUpdate(false);
    }
}

void GUI_EditText::textSelectionUpdate(bool isResetScrollIndex) {
    string saveTitle = "";

    if (isPassword) {
        saveTitle = title;
        title = string(saveTitle.length(), '*');
    }

    textSelectionSetScrollIndex(isResetScrollIndex);

#if __USE_SKIA__
    createTitleImage(font_size);
#else
    createTitleTexture(font_size);
#endif

    if (isPassword) {
        title = saveTitle;
    }

    updateSubLayout();
}

void GUI_EditText::textSelectionSelectAll() {
    textSelectionStartIndex = 0;
    textSelectionEndIndex = (int)title.length();
    text_index = textSelectionEndIndex;
    textSelectionUpdate(true);
}

void GUI_EditText::textSelectionCut(bool doActionCmd) {
    if (!isPassword) {
        textSelectionCopy();
        textSelectionDelete();
    }
}

void GUI_EditText::textSelectionCopy() {
    if (!isPassword && textSelectionIsSelected()) {
        int startIndex = 0;
        int endIndex = 0;
        textSelectionGetSelectedIndex(&startIndex, &endIndex);
        std::string str = title.substr(startIndex, (size_t)endIndex - startIndex);
        SDL_SetClipboardText(str.c_str());
    }
}

void GUI_EditText::textSelectionPaste(bool doActionCmd) {
    bool success = false;

    if (textSelectionIsSelected()) {
        textSelectionDelete(false);
        success = true;
    }

    if (textSelectionIsValidClipboardText()) {
        char* text = SDL_GetClipboardText();
        title = title.insert(text_index, std::string(text));
        text_index += (int)strlen(text);
        success = true;
    }

    textSelectionUpdate(false);

    if (success && doActionCmd && action_cmd) {
        action_cmd(this);
    }
}

void GUI_EditText::textSelectionDelete(bool doActionCmd) {
    if (textSelectionIsSelected()) {
        int startIndex = 0;
        int endIndex = 0;
        textSelectionGetSelectedIndex(&startIndex, &endIndex);
        title = title.substr(0, startIndex) + title.substr(endIndex);
        text_index = startIndex;
        textSelectionStartIndex = text_index;
        textSelectionEndIndex = textSelectionStartIndex;
        textSelectionUpdate(false);

        if (doActionCmd && action_cmd) {
            action_cmd(this);
        }
    }
}

void GUI_EditText::textSelectionCancel() {
    text_index = 0;
    textSelectionScrollIndex = 0;
    textSelectionStartIndex = 0;
    textSelectionEndIndex = textSelectionStartIndex;

    textSelectionUpdate(true);
}

void GUI_EditText::textSelectionGoToIndex(int i) {
    bool isUpdateSubLayout = false;
    bool isResetScrollIndex = false;
    int oi = text_index;
    text_index = i;

    if (oi != text_index) {
        textSelectionScrollIndex = 0;
        isUpdateSubLayout = true;
        isResetScrollIndex = true;
    }

    if (textSelectionIsShiftPressed) {
        textSelectionEndIndex = text_index;
        isUpdateSubLayout = true;
    } else if (textSelectionIsSelected()) {
        textSelectionStartIndex = text_index;
        textSelectionEndIndex = textSelectionStartIndex;
        isUpdateSubLayout = true;
    }

    if (isUpdateSubLayout) {
        textSelectionUpdate(isResetScrollIndex);
    }
}

void GUI_EditText::textSelectionHome() {
    textSelectionGoToIndex(0);
}

void GUI_EditText::textSelectionEnd() {
    textSelectionGoToIndex((int)title.length());
}

int GUI_EditText::textSelectionGetAreaWidth() {
    int areaWidth = tw_area.w - ((padding[3] + padding[1]) * GUI_scale);

    if (textureIcon) {
        areaWidth -= icon_area.w + (icon_gap * GUI_scale);
    }

    return areaWidth;
}

bool GUI_EditText::textSelectionIsThaiText(string text) {
    bool isThai = false;
    int len = (int)text.length();
    int i = 0;

    while (i < len && !isThai) {
        unsigned char c = text.at(i++);

        if ((c & 0xc0) == 0xc0) {
            isThai = true;
        }
    }

    return isThai;
}

bool GUI_EditText::textSelectionIsValidClipboardText() {
    bool result = false;

    if (SDL_HasClipboardText()) {
        char* text = SDL_GetClipboardText();

        if (text) {
            int len = (int)strlen(text);

            if (len > 0) {
                if (_type == Type::TEXT_DIGIT) {
                    if (textSelectionIsDigitText(std::string(text))) {
                        result = true;
                    }
                } else if (isPassword) {
                    if (!textSelectionIsThaiText(std::string(text))) {
                        result = true;
                    }
                } else {
                    result = true;
                }
            }
        }
    }

    return result;
}

bool GUI_EditText::textSelectionIsDigitText(string strText) {
    bool result = true;

    for (int i = 0; i < (int)strText.length() && result; i++) {
        unsigned char c = strText.at(i);

        if ((c & 0xc0) == 0xc0 || !(SDL_isdigit(c) || c == '.')) {
            result = false;
        }
    }

    return result;
}

// Tooh - text selection: contextual menu
#if defined(WIN32)
void GUI_EditText::textSelectionInitContextualMenu(UINT menu, UINT menuCut, UINT menuCopy, UINT menuPaste, UINT menuDelete, UINT menuSelectAll) {
    if (textSelectionContextualMenu == NULL) {
        SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

        textSelectionContextualMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(menu));
        textSelectionContextualMenuCut = menuCut;
        textSelectionContextualMenuCopy = menuCopy;
        textSelectionContextualMenuPaste = menuPaste;
        textSelectionContextualMenuDelete = menuDelete;
        textSelectionContextualMenuSelectAll = menuSelectAll;
    }
}
#endif

void GUI_EditText::textSelectionCreateContextualMenuList() {
    if (textSelectionContextualMenuList == NULL) {
        int numMenu = sizeof(textSelectionContextualMenuText) / sizeof(textSelectionContextualMenuText[0]);
        int numMenuSep = 1;
        numMenu -= numMenuSep;
        int paddingWidth = 4;
        int font_size = 0;
        int cellw = 0;
        int w = 0;
        int h = 0;
        for (int i = 0; i < numMenu; i++) {
            string lstr = LSTR(textSelectionContextualMenuText[i]);
#if __USE_SKIA__
            SkRect bounds;
            GUI_skfonts[font_size].measureText(lstr.c_str(), strlen(lstr.c_str()), SkTextEncoding::kUTF8, &bounds);
#else
            TTF_SizeUTF8(GUI_fonts[font_size], lstr.c_str(), &w, &h);
#endif

            if (w > cellw) {
                cellw = w + (paddingWidth * GUI_scale);
            }
        }

        h /= GUI_scale;
        h += 4;
        cellw /= GUI_scale;
        int padding = paddingWidth * GUI_scale;
        cellw += 50 + (padding / GUI_scale) * 2;
        int numSep = 0;
        int numCells = numMenu + numMenuSep;

        SDL_Rect rect = this->getRect();
        int x = 0;
        int y = rect.y + h;

        int cellh = h;
        int seph = 10;
        h = (numMenu * cellh) + (numMenuSep * seph);
        int maxh = h;

        auto topView = GUI_TopView;
        textSelectionContextualMenuList = new GUI_List(topView, (const char**)textSelectionContextualMenuText, numCells, x, y, cellw, maxh, cellh, true, NULL, font_size);
        textSelectionContextualMenuList->dragable = false;
        textSelectionContextualMenuList->bgcol = sdl_color(0x545a66cc);
        textSelectionContextualMenuList->setAlignment(GUI_ALIGN_ABSOLUTE);
        textSelectionContextualMenuList->setPosition(x, y);

        // icon
        for (int cellIndex = 0; cellIndex < textSelectionContextualMenuList->getSize(); cellIndex++) {
            if (cellIndex != 4) {
                Uint16 unicode = 0;

                if (cellIndex == 0) {
                    unicode = kIcon_scissors;
                } else if (cellIndex == 1) {
                    unicode = kIcon_copy;
                } else if (cellIndex == 2) {
                    unicode = kIcon_paste;
                } else if (cellIndex == 3) {
                    unicode = kIcon_trash;
                } else if (cellIndex == 5) {
                    unicode = kIcon_arrows_alt;
                }

                textSelectionContextualMenuList->getCellAtIndex(cellIndex)->setIcon(GUI_ButtonFA::createTextureFormUnicode(unicode, NULL, NULL, cWhite, 1));
                string lstr = LSTR(textSelectionContextualMenuList->getCellAtIndex(cellIndex)->title);
                textSelectionContextualMenuList->getCellAtIndex(cellIndex)->setTitle(lstr, 0);
            }
        }

        for (auto v : textSelectionContextualMenuList->vMenu) {
            if (v->textureIcon) {
                v->iconColor = cWhite;
                v->icon_area.x = padding;
                v->icon_area.y = (v->tw_area.h - v->icon_area.h) / 2;
                v->title_area.x = 30 * GUI_scale;
            }
        }

        textSelectionContextualMenuList->cmd = [=](GUI_List* l, const char *sel, int index) {
            textSelectionContextualMenuList->hide();

            switch (index) {
                case 0:
                    textSelectionCut();
                    break;

                case 1:
                    textSelectionCopy();
                    break;

                case 2:
                    textSelectionPaste();
                    break;

                case 3:
                    textSelectionDelete();
                    break;

                case 5:
                    textSelectionSelectAll();
                    isJustSelectAll = true;
                    break;
            }

            setFocus();
            GUI_mouseCapturedWindow = this;
            SDL_StartTextInput();
        };

        textSelectionContextualMenuList->onClose = [=](GUI_View* v) {
            textSelectionContextualMenuList = NULL;
        };

        textSelectionContextualMenuList->selectedIndex = 0;
        textSelectionContextualMenuList->hide();
        textSelectionContextualMenuList->isFocus = false;
    }
}

void GUI_EditText::textSelectionShowContextualMenuList(bool show, int x, int y) {
    if (show && textSelectionContextualMenuList == NULL) {
        textSelectionCreateContextualMenuList();
    }

    if (textSelectionContextualMenuList) {
        if (show) {
            x = (x / GUI_scale) + 1;
            y = (y / GUI_scale) + 1;
            
            int left = (tw_area.x / GUI_scale) + padding[0];
            int right = left + (tw_area.w / GUI_scale) - 1 - padding[2];
            int top = (tw_area.y / GUI_scale) + padding[1];
            int bottom = top + (tw_area.h / GUI_scale) - 1 - padding[3];

            if (x < left) {
                x = left;
            } else if (x > right) {
                int different = x - right;
                x -= different;
            }
            
            if (y < top) {
                y = top;
            } else if (y > bottom) {
                int different = y - bottom;
                y -= different;
            }

            textSelectionContextualMenuList->setPosition(x, y);
            updatePopupMenuListPosition(textSelectionContextualMenuList);

            bool canCutCopy = (!isPassword && textSelectionIsSelected());
            bool canPaste = textSelectionIsValidClipboardText();
            bool canDelete = textSelectionIsSelected();

            int startIndex = 0;
            int endIndex = 0;
            textSelectionGetSelectedIndex(&startIndex, &endIndex);
            bool canSelectAll = (title.length() > 0 && !(startIndex == 0 && endIndex == title.length()));

            textSelectionContextualMenuList->setEnable(0, canCutCopy);
            textSelectionContextualMenuList->setEnable(1, canCutCopy);
            textSelectionContextualMenuList->setEnable(2, canPaste);
            textSelectionContextualMenuList->setEnable(3, canDelete);
            textSelectionContextualMenuList->setEnable(5, canSelectAll);

            for (auto v : textSelectionContextualMenuList->vMenu) {
                int cellIndex = v->tag;

                if (cellIndex != 4 && v->enable) {
                    textSelectionContextualMenuList->setSelectedIndex(cellIndex);
                    break;
                }
            }

            textSelectionContextualMenuList->show();
            textSelectionContextualMenuList->isFocus = true;
            SDL_StopTextInput();
        } else {
            textSelectionContextualMenuList->hide();
            textSelectionContextualMenuList->isFocus = false;
            GUI_mouseCapturedWindow = this;
            setFocus();
            SDL_StartTextInput();
        }
    }
}

void GUI_EditText::textSelectionSetCaretPosition(int x, int y) {
    // Tooh - text selection: set caret at mouse click position
    textSelectionIsMouseDown = true;
    text_index = textSelectionGetIndexAtScreenPosition(x);
    textSelectionStartIndex = text_index;
    textSelectionEndIndex = textSelectionStartIndex;
    textSelectionMousePoint.x = x;
    textSelectionMousePoint.y = y;
}
