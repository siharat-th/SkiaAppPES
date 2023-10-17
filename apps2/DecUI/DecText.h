#ifndef __DEC_TEXT_H__
#define __DEC_TEXT_H__

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkImage.h"
#include "include/core/SkData.h"
#include "DecWidget.h"

#include <future>
#include <thread>
#include <chrono>
//using namespace std::chrono_literals;


class DecText : public DecWidget {
public:    
    DecText(const char* text = "");
    virtual ~DecText() override;

    virtual std::string type() override { return "Text"; }

    virtual bool setText(std::string t);

    DecText &Regular();
    DecText &Bold();
    DecText &Italic();
    DecText &BoldItalic();
    DecText &Icon();

    virtual int contentWidth() override;
    virtual int contentHeight() override;     

protected:

    int _tfIndex;

    virtual void _draw_content(SkCanvas &canvas) override;    
};

DecText &Text( const char *text );

class DecTextEdit : public DecText {
private:
    const uint32_t fUniqueID;

protected:

    // simple circle pool
    static const int maxpool = 10;
    std::function<void()> runnext[maxpool];
    int head = 0;
    int tail = 0;

public:
    std::function<void(DecTextEdit& textedit)> _callback;
    enum class Type { TEXT_ANY, TEXT_DIGIT };

    uint32_t uniqueID() const { return fUniqueID; }

    double fNextTime = -DBL_MAX;
    //double fDrawTime = -DBL_MAX;
    bool fBlink = false;
    bool fDraw = false;

    DecTextEdit(const char* text = "", std::function<void(DecTextEdit& textedit)> callback = nullptr);
    virtual ~DecTextEdit() override;

    DecTextEdit& visible(bool v) override;
    virtual const char* text() override;

    virtual std::string type() override { return "TextEdit"; }

    virtual bool setText(std::string t) override;

    virtual void update() override;
    //virtual void updateAbsPosn() override;
    virtual bool onMouse(int ix,
                         int iy,
                         skui::InputState state,
                         skui::ModifierKey modifiers) override;
    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) override;
    virtual bool onChar(SkUnichar c, skui::ModifierKey mod) override;
    virtual void focus(bool f) override;

    bool _isEnable;

    //DecText& Regular();
    //DecText& Bold();
    //DecText& Italic();
    //DecText& BoldItalic();
    //DecText& Icon();

    //virtual int contentWidth() override;
    //virtual int contentHeight() override;

    inline bool isdigit(char c) { return c >= '0' && c <= '9'; }
    inline void setType(Type type) { _type = type; }

    
    bool isPassword;
    bool isPasswordChanged;
    //std::function<void(DecTextEdit*)> onEnter = nullptr;
    //void (*action_cmd)(DecTextEdit*) = nullptr;

    int text_index;
    //int old_text_index;

    // Tooh - text selection
    int textSelectionScrollIndex;
    int textSelectionStartIndex;
    int textSelectionEndIndex;
    //int textSelectionFontSize;
    bool textSelectionIsSelected();
    void textSelectionGetSelectedIndex(int* startIndex, int* endIndex);

    bool textSelectionIsShiftPressed;
    bool textSelectionIsMouseDown;
    SkPoint textSelectionMousePoint;
    int textSelectionGetIndexAtScreenPosition(int x);
    int textSelectionGetNextIndex(size_t i);
    int textSelectionGetPreviousIndex(int i);
    void textSelectionSetScrollIndex(bool isResetScrollIndex = false);
    bool textSelectionIsUpperLowerCharacter(int& i);
    int textSelectionGetNextMiddleCharacter(int i);
    int textSelectionGetPreviousMiddleCharacter(int i);
    void textSelectionMouseMove();
    void textSelectionUpdate(bool isResetScrollIndex = false);
    void textSelectionSelectAll();                      // Windows: Ctrl+A, Mac: Command+A
    void textSelectionCut(bool doActionCmd = true);     // Windows: Ctrl+X, Mac: Command+X
    void textSelectionCopy();                           // Windows: Ctrl+C, Mac: Command+C
    void textSelectionPaste(bool doActionCmd = true);   // Windows: Ctrl+V, Mac: Command+V
    void textSelectionDelete(bool doActionCmd = true);  // Delete, Backspace
    void textSelectionCancel();
    void textSelectionGoToIndex(int i);
    void textSelectionHome();
    void textSelectionEnd();
    int textSelectionGetAreaWidth();
    bool textSelectionIsThaiText(std::string text);
    bool textSelectionIsValidClipboardText();
    bool textSelectionIsDigitText(std::string strText);

protected:
    //int _tfIndex;
    Type _type;
    virtual void _draw_content(SkCanvas& canvas) override;
    void textSelectionSetCaretPosition(int x, int y);
    std::chrono::high_resolution_clock::time_point touchTimeout;
    bool isJustSelectAll;

};

DecTextEdit& TextEdit(const char* text = "", std::function<void(DecTextEdit& textedit)> callback = nullptr);

#endif //__DEC_TEXT_H__