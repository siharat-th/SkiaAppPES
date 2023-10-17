//
// Created by Panutat Tejasen on 3/13/2016 AD.
//

#ifndef GUI_EDITTEXT_GUI_EDITTEXT_H
#define GUI_EDITTEXT_GUI_EDITTEXT_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "LocalizedString.hpp"
#include "GUI_View.h"
#include "GUI_Label.h"
#include "GUI_List.h"
#include <string>
#include <stdlib.h>

// Tooh - text selection
#if defined(__MACOSX__)
#define CUSTOM_KMOD_CTRL (KMOD_LGUI|KMOD_RGUI)
#else

#if defined(WIN32)
#define _WINSOCKAPI_  // Tooh - stops Windows.h including winsock.h to prevent macro redefinition in SDL_net.h 
#include <Windows.h>
#endif

#define CUSTOM_KMOD_CTRL KMOD_CTRL
#endif

extern bool GUI_IsKeyCTRLPressed();

class GUI_EditText : public GUI_Label {
    int font_size;

public:
    enum class Type {
        TEXT_ANY,
        TEXT_DIGIT
    };

    static GUI_EditText *create(GUI_View *parent, const char *title, int x, int y, int width, int height);

    GUI_EditText(GUI_View *parent, const char *title, int x = 0, int y = 0, int width = 0, int height = 0, SDL_Color color = cBlack, int align = GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER, int fontSize = 1, void(*act_cmd)(GUI_EditText *) = NULL);
    virtual ~GUI_EditText() override;

    virtual bool handleEvents(SDL_Event *ev) override;
    virtual void draw() override;

    inline void setType(Type type) {
        _type = type;
    }

    bool isPassword;
    bool isPasswordChanged;
    bool setText(std::string newText);
    virtual void setTitle(std::string t, int fontSize = -1) override;
    
    std::function<void(GUI_EditText*)> onEnter;

    void(*action_cmd)(GUI_EditText *);

    int text_index;
    int old_text_index;
    void setFocus(); // Tooh - set focus
    void killFocus(); // Jimmy - kill focus

    // Tooh - text selection
    bool textSelectionIsShiftPressed;
    bool textSelectionIsMouseDown;
    SDL_Point textSelectionMousePoint;
    int textSelectionGetIndexAtScreenPosition(int x);
    int textSelectionGetNextIndex(size_t i);
    int textSelectionGetPreviousIndex(int i);
    void textSelectionSetScrollIndex(bool isResetScrollIndex = false);
    bool textSelectionIsUpperLowerCharacter(int *i);
    int textSelectionGetNextMiddleCharacter(int i);
    int textSelectionGetPreviousMiddleCharacter(int i);
    void textSelectionMouseMove();
    void textSelectionUpdate(bool isResetScrollIndex = false);
    void textSelectionSelectAll();                     // Windows: Ctrl+A, Mac: Command+A
    void textSelectionCut(bool doActionCmd = true);    // Windows: Ctrl+X, Mac: Command+X
    void textSelectionCopy();                          // Windows: Ctrl+C, Mac: Command+C
    void textSelectionPaste(bool doActionCmd = true);  // Windows: Ctrl+V, Mac: Command+V
    void textSelectionDelete(bool doActionCmd = true); // Delete, Backspace
    virtual void textSelectionCancel() override;
    void textSelectionGoToIndex(int i);
    void textSelectionHome();
    void textSelectionEnd();
    int textSelectionGetAreaWidth();
    bool textSelectionIsThaiText(string text);
    bool textSelectionIsValidClipboardText();
    bool textSelectionIsDigitText(string strText);

#if defined(WIN32)
    // Tooh - text selection: contextual menu
    void textSelectionInitContextualMenu(UINT menu = 0, UINT menuCut = 0, UINT menuCopy = 0, UINT menuPaste = 0, UINT menuDelete = 0, UINT menuSelectAll = 0);
#endif

private:
    Type _type;

    // Tooh - text selection: contextual menu
    void textSelectionCreateContextualMenuList();
    void textSelectionShowContextualMenuList(bool show, int x = 0, int y = 0);
    float touchTime;
    bool isJustSelectAll;
    void textSelectionSetCaretPosition(int x, int y);
};

#endif // GUI_EDITTEXT_GUI_EDITTEXT_H
