#include <string>
#include "include/core/SkColor.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkTypes.h"
#include "include/core/SkFontMetrics.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "src/core/SkStringUtils.h"
#include "src/base/SkUTF.h"
#include "src/core/SkStrike.h"
#include "modules/skunicode/include/SkUnicode.h"
#include "DecFont.h"
#include "DecText.h"
#include <include/core/SkTime.h>
#include <sk_ui/SkiaApp.h>

using namespace std::chrono_literals;

#include <atomic>
static int32_t next_id() {
    static std::atomic<int32_t> nextID{1};
    int32_t id;
    do {
        id = nextID.fetch_add(1, std::memory_order_relaxed);
    } while (id == SK_InvalidGenID);
    return id;
}


// example unicode
//
/*
//

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"

EM_JS(void, say_hello, (const char* str), { console.log('hello ' + UTF8ToString(str)); });
EM_JS(char*, get_unicode_str, (), {
    var jsString = "สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก";
    var lengthBytes = lengthBytesUTF8(jsString) + 1;
    var stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(jsString, stringOnWasmHeap, lengthBytes);
    return stringOnWasmHeap;
});

const char* text =
        "Hello world. this  👩🏾‍⚕️ 👨🏾‍⚕️ 👩🏾‍🌾 👨🏾‍🌾  is a "
        "paragraph. "
        "Hello world. this is a paragraph. Hello world. this is a paragraph. "
        "สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก "
        "สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก "
        "สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก เป็นมนุษย์สุดประเสริฐเลิศคุณค่า "
        "กว่าบรรดาฝูงสัตว์เดรัจฉาน จงฝ่าฟันพัฒนาวิชาการ อย่าล้างผลาญฤๅเข่นฆ่าบีฑาใคร ไม่ถือโทษโกรธแช่งซัดฮึดฮัดด่า "
        "หัดอภัยเหมือนกีฬาอัชฌาสัย ปฏิบัติประพฤติกฎกำหนดใจ พูดจาให้จ๊ะ ๆ จ๋า ๆ น่าฟังเอยฯ";

// say_hello(text);
// char* str = get_unicode_str();
// SkDebugf("UTF8 string says: %s\n", str);
// free(str);
#endif

//
*/

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

void htmlInputTextOverlay(const DecWidget* pWidget,
                          const uint32_t fUniqueID,
                          const char* text,
                          const int* _padding,
                          const int _borderWidth,
                          const int _ax,
                          const int _ay,
                          const int _width,
                          const int _height,
                          const char* _fontfamily,
                          const int _textSize,
                          const bool _isVisible,
                          const bool _isFocus) {
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        var pWidget = $0;
        var id='id'+$1;
        var value=UTF8ToString($2);
        var p0=$3+'px';
        var p1=$4+'px';
        var p2=$5+'px';
        var p3=$6+'px';
        var left=$7;
        var top=$8;
        var width=$9;
        var height=$10;
        var fontfamily=UTF8ToString($11);
        var fontsize=$12;
        var isVisible=$13;
        var isFocus=$14;

        var visibility=isVisible? 'visible':'hidden';

        var target = document.querySelector(`#${id}`);

        var c = document.getElementById( 'canvas' );
        var rect=c.getBoundingClientRect();
        left = left + rect.left;
        top = top + rect.top;

        var padding = `${p0} ${p1} ${p2} ${p3}`;
        var style = `box-sizing: content-box; border: none gray; outline: none #75fb4c; position: absolute; margin: 0; padding: ${padding}; left: ${left}px; top: ${top}px; width: ${width}px; height: ${height}px; font-family:'${fontfamily}'; font-size:${fontsize}px; visibility:${visibility}`;
        if (!target) {
            target = document.createElement("input");
            target.id = id;
            target.setAttribute('autocomplete', 'off');
            target.setAttribute('data', style);
            target.style = style;

            //target.onchange = (function(widget) {
            //    return function(e) { CanvasKit['decUtility']['callback_widget_text'](widget, e.target.value); } } )(pWidget);
            target.onfocus  = (function(widget) {
                return function(e) { CanvasKit['decUtility']['callback_widget_focus'](widget, true); } } )(pWidget);
            target.onblur   = (function(widget) {
                return function(e) { CanvasKit['decUtility']['callback_widget_focus'](widget, false); } } )(pWidget);

            document.body.appendChild(target);
        }
        if (target) {
            if ($2 != 0) {
                target.value = value;
            }
            if (target.getAttribute('data') != style) {
                target.setAttribute('data', style);
                target.style = style;
            }
            if (isFocus) {
                target.focus();
            }
        }
    },
    pWidget,
    fUniqueID,
    text,
    _padding[0],
    _padding[1],
    _padding[2],
    _padding[3],
    _ax + _borderWidth,
    _ay + _borderWidth,
    _width - _borderWidth * 2 - _padding[3] - _padding[1],
    _height - _borderWidth * 2 - _padding[0] - _padding[2],
    _fontfamily,
    _textSize,
    _isVisible,
    _isFocus);
#endif
}

void htmlInputTextSetValue(const DecWidget* pWidget,
                           const uint32_t fUniqueID,
                           const char* text) {
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        var pWidget = $0;
        var id='id'+$1;
        var value = UTF8ToString($2);
        var target = document.querySelector(`#${id}`);
        if (target) {
            if ($2 != 0) {
                target.value = value;
            }
        }
    },
    pWidget,
    fUniqueID,
    text);
#endif
}

void htmlInputTextGetValue(const DecWidget* pWidget,
                           const uint32_t fUniqueID) {
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        var pWidget = $0;
        var id='id'+$1;
        var target = document.querySelector(`#${id}`);
        if (target) {
            CanvasKit['decUtility']['callback_widget_text'](pWidget, target.value);
        }
    },
    pWidget,
    fUniqueID);
#endif
}

void htmlInputTextSetVisible(const DecWidget* pWidget, const uint32_t fUniqueID, const bool _isVisible) {
#ifdef __EMSCRIPTEN__
    EM_ASM(
            {
                var pWidget = $0;
                var id = 'id' + $1;
                var isVisible = $2;
                var target = document.querySelector(`#${id}`);
                if (target) {
                    if (isVisible) {
                        target.style.visibility = 'visible';
                    } else {
                        target.style.visibility = 'hidden';
                    }
                }
            },
            pWidget,
            fUniqueID,
            _isVisible);
#endif
}

DecText& Text(const char* text) {
    DecText* w = new DecText(text);
    //w->text(text);
    return *w;
}

DecTextEdit& TextEdit(const char* text, std::function<void(DecTextEdit& textedit)> callback) {
    DecTextEdit* w = new DecTextEdit(text, callback);
    // w->text(text);
    return *w;
}


DecText::DecText(const char* text) {
    this->text(text);
    _textSize = 14;
    _tfIndex = FONT_REGULAR;

    _background = false;
    _border = false;

    cache( true );
}

DecText::~DecText() {

}

DecText& DecText::Regular() {
    _tfIndex = FONT_REGULAR;
    return *this;
}

DecText &DecText::Bold() {
    _tfIndex = FONT_BOLD;
    return *this;
}
DecText &DecText::Italic() {
    _tfIndex = FONT_ITALIC;
    return *this;
}
DecText &DecText::BoldItalic() {
    _tfIndex = FONT_BOLDITALIC;
    return *this;
}
DecText &DecText::Icon() {
    _tfIndex = FONT_ICON;
    return *this;
}

bool DecText::setText(std::string t) {
    bool hasChange = false;
    if (_text != t) {
        hasChange = true;
        _text = t;
        dirty(true, true);
    }
    if( _ow == 0 ) {
        _width = contentWidth();
    }
    if( _oh == 0 ) {
        _height = contentHeight();
    }
    return hasChange;
}

int DecText::contentWidth() {
    if( _font == NULL ) {
        return 0;
    }
    sk_sp<SkTypeface> typeface = _font->typeface(_tfIndex);

    SkFont font;
    font.setTypeface(typeface);
    font.setSubpixel(true);
    font.setSize(_textSize);

    int textwidth = ceil( font.measureText(_text.c_str(), _text.length(), SkTextEncoding::kUTF8) );
    return _padding[3] + textwidth + _padding[1];
}

int DecText::contentHeight() {
    if( _font == NULL ) {
        return 0;
    }

    SkFont font;
    font.setTypeface(_font->typeface(_tfIndex));
    font.setSubpixel(true);
    font.setSize(_textSize);

    SkFontMetrics metrics;
    font.getMetrics(&metrics);

    int fontHeight = ceil( metrics.fDescent - metrics.fAscent );
    return _padding[0] + fontHeight + _padding[2];
}

void DecText::_draw_content(SkCanvas &canvas) {
    if( _font == NULL ) {
        return;
    }

    if( _width == 0 && _ow == 0 && _text.length() ) {
        _width = contentWidth();
    }
    if( _height == 0 && _oh == 0 && _text.length() ) {
        _height = contentHeight();
    }

    SkAutoCanvasRestore acr(&canvas, true);
    //canvas.translate(_padding[3], _padding[0]);

    SkFont font;
    font.setTypeface(_font->typeface(_tfIndex));
    font.setSubpixel(true);
    font.setSize(_textSize);

    SkFontMetrics metrics;
    font.getMetrics(&metrics);

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(_text_color);

    canvas.drawSimpleText(_text.c_str(), _text.length(), SkTextEncoding::kUTF8,
        _padding[3], _padding[0] - metrics.fAscent, font, paint);

    //auto blob = SkTextBlob::MakeFromText(_text.c_str(), _text.length(), font);
    //canvas.drawTextBlob(blob, _padding[3], _padding[0] - metrics.fAscent, paint);
}


DecTextEdit::DecTextEdit(const char* text, std::function<void(DecTextEdit& textedit)> callback) : DecText(text)
, _callback(callback)
, fUniqueID(next_id())
, text_index(0)
, textSelectionScrollIndex(0)
, textSelectionStartIndex(0)
, textSelectionEndIndex(0)
, _type(Type::TEXT_ANY)
//, action_cmd(NULL)
, isPassword(false)
, isPasswordChanged(false) {
    // inherited: DecText
    //_textSize = 14;
    //_tfIndex = FONT_REGULAR;
    //_background = false;
    // cache(true);

    _isEnable = true;
    _isFocusable = true;

    _border = true;
    _borderWidth = 1;
    _border_color = 0xFF808080;
    _focus_borderWidth = 2;
    _focus_border_color = SK_ColorGREEN;

    _margin[0] = 0;
    _margin[1] = 0;
    _margin[2] = 0;
    _margin[3] = 0;

    _padding[0] = 4;
    _padding[1] = 4;
    _padding[2] = 4;
    _padding[3] = 4;

    // border: none;
    // outline: none;
    // font-style: italic;
    // font-weight: bold;
    // font-family: Roboto;
    // font-size: 14px;

#ifdef __EMSCRIPTEN__
    htmlInputTextOverlay(this, fUniqueID, _text.c_str(), _padding, _focus_borderWidth, _ax, _ay, _width, _height, "Kanit", _textSize, _isVisible, _isFocus);
#endif

    textSelectionIsShiftPressed = false;
    textSelectionIsMouseDown = false;
    textSelectionMousePoint.fX = 0;
    textSelectionMousePoint.fY = 0;
    isJustSelectAll = false;
    touchTimeout = std::chrono::high_resolution_clock::now() + 500ms;
}

DecTextEdit::~DecTextEdit(){}

DecTextEdit& DecTextEdit::visible(bool v) {
    if (v == _isVisible) return *this;

    _isVisible = v;
    fDraw = true;
    //    fDrawTime = -DBL_MAX;

#ifdef __EMSCRIPTEN__
    htmlInputTextSetVisible(this, fUniqueID, _isVisible);
#endif

    return *this;
}

bool DecTextEdit::onMouse(int ix, int iy, skui::InputState state, skui::ModifierKey modifiers) {
    //SkDebugf("[%s]%s:%i %i\n", "", __func__, ix, iy);
    //if (DecText::onMouse(ix, iy, state, modifiers)) {
    //    return false;
    //}
    if (_isEnable == false) {
        textSelectionCancel();
        return false;
    }

    ix = ix - _borderWidth -  _padding[3];
    iy = iy - _borderWidth -  _padding[0];

    switch (state) {
        case skui::InputState::kDown: {
            touchTimeout = std::chrono::high_resolution_clock::now() + 500ms;
            textSelectionIsShiftPressed = false;
            if ((ix <= _width) && (iy <= _height)) {
                bool setCaret = false;

                if (_isFocus) {
                    if (!isJustSelectAll) {
                        textSelectionCancel();
                        //SDL_StartTextInput();
                        setCaret = true;
                        //textSelectionShowContextualMenuList(false);
                    }
                } else {
                    focus(true);
                    setCaret = true;
                }

                //SkDebugf("[%s]%s: _isFocus:%i, setCaret:%i\n", "", __func__, _isFocus, setCaret);

                if (setCaret) {
                    textSelectionSetCaretPosition(ix, iy);
                }
            } else {
                focus(false);
            }

            break;
        }
        case skui::InputState::kMove: {
            if (_isFocus && textSelectionIsMouseDown && textSelectionMousePoint.fX != ix) {
                textSelectionMousePoint.fX = ix;
                textSelectionMousePoint.fY = iy;
                textSelectionMouseMove();
                return true;
            }
            break;
        }
        case skui::InputState::kUp: {
            if (_isFocus) {
                textSelectionIsMouseDown = false;
                if (isJustSelectAll || textSelectionIsSelected() ||
                    //e.button == SDL_BUTTON_RIGHT ||
                    std::chrono::high_resolution_clock::now() >= touchTimeout) {
                    isJustSelectAll = false;
                    //textSelectionShowContextualMenuList(true, x, y);
                }

            } else {
                textSelectionCancel();
            }
            break;
        }
        default:
            break;
    }
    return false;
}

bool DecTextEdit::onKey(skui::Key k, skui::InputState state, skui::ModifierKey modifiers) {
    //SkDebugf("DecTextEdit::on%s: 0x%04x\n", sk_ui::getInputStateString(state), k);
    if (skui::InputState::kDown == state) {
        if (skui::Key::kOK == k) {

            // simple circle pool
            int h = (head + 1) % maxpool;
            if (h != tail) {
                runnext[head = h] = [=]() {
                    //SkDebugf("DecTextEdit::runnext[%i]\n", h);
#ifdef __EMSCRIPTEN__
                    htmlInputTextGetValue(this, fUniqueID);
#endif
                    if (_callback) {
                        _callback(*this);
                    }

                    //if (action_cmd) {
                    //    action_cmd(this);
                    //}
                };
            }
        }
    }
    return true;
}

bool DecTextEdit::onChar(SkUnichar c, skui::ModifierKey mod) {
    SkDebugf("DecTextEdit::onChar: 0x%04x\n", c);

#ifndef __EMSCRIPTEN__
    std::string sub1 = _text.substr( 0, text_index );
    std::string sub2 = _text.substr( text_index );

    SkDebugf("sub1: %s\n", sub1.c_str());
    SkDebugf("sub2: %s\n", sub2.c_str());
    SkDebugf("text_index: %d\n", text_index);

    if (c == 0x7F || c == 0x08) {
        SkDebugf( "Backspace\n" );
        if( sub1.length() > 0 ) {
            SkString str8 = SkString( sub1 );
            std::u16string u16str = SkUnicode::convertUtf8ToUtf16( str8 );
            u16str = u16str.substr( 0, u16str.length()-1 );
            str8 = SkUnicode::convertUtf16ToUtf8( u16str );
            sub1 = std::string(str8.c_str());
            //sub1 = sub1.substr( 0, sub1.length()-1 );
        }
        _text = sub1 + sub2;
        text_index = sub1.length();
    }
    else if (c == 0xf702 ) {
        SkDebugf( "Left Arrow\n" );
    }
    else if (c == 0xf703 ) {
        SkDebugf( "Right Arrow\n" );
    }
    else {
        char buffer[SkUTF::kMaxBytesInUTF8Sequence];
        size_t count = SkUTF::ToUTF8(c, buffer);
        buffer[count] = 0;

        _text = sub1 + std::string( buffer ) + sub2;
        text_index = text_index + count;
    }

    textSelectionStartIndex = text_index;
    textSelectionEndIndex = text_index;

    SkDebugf("_text: %s\n", _text.c_str());
    SkDebugf("text_index: %d\n", text_index);

    //if (c == 0x000A || c == 0x000D) {
    //    SkDebugf("DecTextEdit::onChar: 0x%04x\n", c);
    //    SkDebugf("DecTextEdit::text: %s\n", _text.c_str());
    //
    //}
#endif
    return true;
}


void DecTextEdit::update() {
    DecWidget::update();
    const double now = SkTime::GetNSecs();
//    static const double c50ms = 500000000;
//    if (_isVisible && fDrawTime != -DBL_MAX) {
//        if (c50ms < (now - fDrawTime)) {
//            fDrawTime != -DBL_MAX;
//#ifdef __EMSCRIPTEN__
//            htmlInputTextSetVisible(this, fUniqueID, false);
//#endif
//        }
//    }
    if (now >= fNextTime) {
        constexpr double dHalfSec = 0.5 * 1e9;
        fNextTime = now + dHalfSec;

        // simple circle pool
        for (int i = 0; i < maxpool && tail != head; i++) {
            const std::function<void()>& run = runnext[++tail %= maxpool];
            if (run) {
                run();
                runnext[tail] = nullptr;
            }
        }

        fBlink = !fBlink;
        if (_isVisible) {
            dirty();
        }
    }

    if (_isVisible) {
        if (!fDraw) {
#ifdef __EMSCRIPTEN__
            htmlInputTextOverlay(this, fUniqueID, NULL, _padding, _focus_borderWidth, _ax, _ay, _width, _height, "Kanit", _textSize, _isVisible, _isFocus);
#endif
        }
        fDraw = false;
    }
}

//void DecTextEdit::updateAbsPosn() {
//    DecWidget::updateAbsPosn();
//    static int s_ax = -1;
//    static int s_ay = -1;
//    bool bAbsXChange = s_ax != _ax;
//    bool bAbsYChange = s_ay != _ay;
//    if (bAbsXChange)
//        s_ax = _ax;
//    if (bAbsYChange)
//        s_ay = _ay;
//
//    if (bAbsXChange || bAbsYChange) {
//    }
//
//}

#include "src/core/SkTextBlobPriv.h"
#include "src/core/SkFontPriv.h"
#include "src/core/SkStrikeCache.h"

void DecTextEdit::_draw_content(SkCanvas& canvas) {
//    fDrawTime = SkTime::GetNSecs();
    if (_font == NULL) {
        return;
    }

    // reset Thai text
    if (isPassword && isPasswordChanged != isPassword && textSelectionIsThaiText(_text)) {
        _text = "";
    }

    std::string saveTitle;
    if (isPassword) {
        saveTitle = _text;
        _text = std::string(saveTitle.length(), '*');
    }

#ifdef __EMSCRIPTEN__
    htmlInputTextOverlay(this, fUniqueID, NULL, _padding, _focus_borderWidth, _ax, _ay, _width, _height, "Kanit", _textSize, _isVisible, _isFocus);
#else
    DecText::_draw_content(canvas);
    if (_isEnable && _isFocus) {
        if (fBlink) {
            SkAutoCanvasRestore acr(&canvas, true);
            sk_sp<SkTypeface> typeface = _font->typeface(_tfIndex);

            SkFont font;
            font.setTypeface(typeface);
            font.setSubpixel(true);
            font.setSize(_textSize);

            SkFontMetrics metrics;
            font.getMetrics(&metrics);
            const float fontHeight = -metrics.fAscent + metrics.fDescent;
            //SkDebugf("[%s]%s text_index:%i, ScrollIndex:%i\n", "", __func__, text_index, textSelectionScrollIndex);
            auto substr = _text.substr(textSelectionScrollIndex, (size_t)text_index - textSelectionScrollIndex);

            SkRect rect = SkRect::MakeEmpty();
            //float ww = font.measureText(substr.c_str(), substr.length(), SkTextEncoding::kUTF8, &rect);
            //
            //rect.fTop = _padding[0];
            //rect.fBottom = _padding[0] + fontHeight;
            //rect.fLeft = _padding[3];
            //rect.fRight = _padding[3] + rect.fRight;

            float sumw = 0;
            rect.setLTRB(_padding[3],
                            _padding[0], 
                            _padding[3] + sumw,
                            _padding[0] + fontHeight);

            auto blob = SkTextBlob::MakeFromText(substr.c_str(), substr.length(), font);

            if (blob.get()) {
                for (SkTextBlobRunIterator it(blob.get()); !it.done(); it.next()) {
                    auto [strikeSpec, strikeToSourceScale] = SkStrikeSpec::MakeCanonicalized(it.font(), nullptr);
                    SkBulkGlyphMetrics metrics{strikeSpec};
                    SkSpan<const SkGlyph*> glyphs = metrics.glyphs(SkSpan(it.glyphs(), it.glyphCount()));

                    for (auto glyph : glyphs) {
                        const float w = glyph->advanceX() * strikeToSourceScale;
                        sumw += w;
                        //SkDebugf("glyph w: %g, %g\n", w, sumw);
                    }
                }

                rect.setLTRB(_padding[3],
                             _padding[0],
                             _padding[3] + sumw,
                             _padding[0] + fontHeight);
            }

            //SkDebugf("[%g, %g, %g, %g]\n", rect.fLeft, rect.fTop, rect.fRight, rect.fBottom);

            SkPaint paint;
            paint.setStyle(SkPaint::Style::kStroke_Style);
            paint.setAntiAlias(true);

            //paint.setColor(SK_ColorRED);
            //canvas.drawRect(rect, paint);
            paint.setColor(SK_ColorBLACK);
            canvas.drawLine(SkPoint::Make(rect.fRight, rect.fTop), SkPoint::Make(rect.fRight, rect.fBottom), paint);

            textSelectionMouseMove();
        }
    }
#endif
    fDraw = true;
    if (isPassword) {
        _text = saveTitle;
    }
}


const char* DecTextEdit::text() {
#ifdef __EMSCRIPTEN__
    htmlInputTextGetValue(this, fUniqueID);
#endif

    return _text.c_str();
}

bool DecTextEdit::setText(std::string t) {
#ifdef __EMSCRIPTEN__
    htmlInputTextGetValue(this, fUniqueID);
#endif
    bool hasChange = DecText::setText(t);

    if (hasChange) {
#ifdef __EMSCRIPTEN__
        htmlInputTextSetValue(this, fUniqueID, _text.c_str());
#endif
    }
    return hasChange;
}

void DecTextEdit::focus(bool f) {
    DecWidget::focus(f);

#ifdef __EMSCRIPTEN__
    htmlInputTextOverlay(this, fUniqueID, NULL, _padding, _focus_borderWidth, _ax, _ay, _width, _height, "Kanit", _textSize, _isVisible, _isFocus);
#else
    if (f) {
        // SDL_StartTextInput();


    } else {
        textSelectionIsShiftPressed = false;
        textSelectionIsMouseDown = false;
        textSelectionMousePoint.fX = 0;
        textSelectionMousePoint.fY = 0;
        // touchTime = 0.0f;
        isJustSelectAll = false;

        // SDL_StopTextInput();
    }
#endif
}

int DecTextEdit::textSelectionGetIndexAtScreenPosition(int x) {
    if (_font == NULL) {
        return 0;
    }
    sk_sp<SkTypeface> typeface = _font->typeface(_tfIndex);

    SkFont font;
    font.setTypeface(typeface);
    font.setSubpixel(true);
    font.setSize(_textSize);
    SkRect rect;

    if (_text.length() == 0) {
        text_index = 0;
        textSelectionScrollIndex = 0;
    }

    int index = text_index;
    int left = _x;
    size_t j = 0;
    x += 4;

    int w = 0;
    int h = 0;
    for (size_t i = textSelectionScrollIndex; i < _text.length(); i++) {
        j = textSelectionGetNextIndex(i) - i;

        auto substr = _text.substr(textSelectionScrollIndex, i + j - textSelectionScrollIndex);
        const int ww = font.measureText(substr.c_str(), substr.length(), SkTextEncoding::kUTF8, &rect);

        w = rect.width();
        h = rect.height();

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

int DecTextEdit::textSelectionGetNextIndex(size_t i) {
    size_t j = 0;
    size_t len = _text.length();

    if (i < len) {
        unsigned char c = _text.at(i);

        if ((c & 0x80) == 0) {
            j++;
        } else if ((c & 0xc0) == 0xc0) {
            j++;
            c = _text.at(i + j);

            while ((c & 0xc0) == 0x80) {
                j++;
                if (i + j >= len)
                    break;
                c = _text.at(i + j);
            }
        }
    }
    return i + j;
}

int DecTextEdit::textSelectionGetPreviousIndex(int i) {
    while (i > 0) {
        if (0x80 != (_text.at(--i) & 0xC0)) {
            break;
        }
    }
    return i;
}

void DecTextEdit::textSelectionSetScrollIndex(bool isResetScrollIndex) {
    if (isResetScrollIndex || text_index < textSelectionScrollIndex) {
        textSelectionScrollIndex = 0;
    }

    int i = textSelectionScrollIndex;
    int areaWidth = textSelectionGetAreaWidth();
    int w = 0;
    int h = 0;
    bool success = false;

    if (_font == NULL) {
        return;
    }

    sk_sp<SkTypeface> typeface = _font->typeface(_tfIndex);
    SkFont font;
    font.setTypeface(typeface);
    font.setSubpixel(true);
    font.setSize(_textSize);
    SkRect rect;

    do {
        auto substr = _text.substr(i, (size_t)text_index - i);
        int ww = font.measureText(substr.c_str(), substr.length(), SkTextEncoding::kUTF8, &rect);

        w = rect.width();
        h = rect.height();

        if (w > areaWidth) {
            i = textSelectionGetNextMiddleCharacter(i);
        } else {
            success = true;
        }
    } while (!success && i < (int)_text.length());

    if (success) {
        textSelectionScrollIndex = i;
    }
}

bool DecTextEdit::textSelectionIsUpperLowerCharacter(int& index) {
    bool result = false;
    //int index = *i;
    int c = (unsigned char)_text[index++];
    int unicode = c;

    // convert unicode bytes to int
    if ((c & 0xc0) == 0xc0) {
        unicode = (c & 0x0F) << 12;
        c = (unsigned char)_text[index++];
        unicode += (c & 0x3F) << 6;
        c = (unsigned char)_text[index++];
        unicode += (c & 0x3F);
    }

    //*i = index;
    result = (unicode == 0xE31 || (unicode >= 0xE34 && unicode <= 0xE3A) ||
              (unicode >= 0xE47 && unicode <= 0xE4E));

    return result;
}

int DecTextEdit::textSelectionGetNextMiddleCharacter(int i) {
    i = textSelectionGetNextIndex(i);
    int lastIndex = i;

    while (i < (int)_text.length() && textSelectionIsUpperLowerCharacter(i)) {
        lastIndex = i;
    };

    return lastIndex;
}

int DecTextEdit::textSelectionGetPreviousMiddleCharacter(int i) {
    int previousIndex = i;

    do {
        i = textSelectionGetPreviousIndex(i);
        previousIndex = i;
    } while (i > 0 && textSelectionIsUpperLowerCharacter(previousIndex));

    return i;
}

void DecTextEdit::textSelectionMouseMove() {
    if (_isFocus && textSelectionIsMouseDown) {
        int left = _x;
        int areaWidth = textSelectionGetAreaWidth();
        int right = left + areaWidth;

        if (textSelectionMousePoint.fX <= left) {
            textSelectionScrollIndex = textSelectionGetPreviousMiddleCharacter(textSelectionScrollIndex);
        } else if (textSelectionMousePoint.fX >= right) {
            text_index = textSelectionGetNextMiddleCharacter(text_index);
        }

        text_index = textSelectionGetIndexAtScreenPosition(textSelectionMousePoint.fX);
        textSelectionEndIndex = text_index;
        textSelectionUpdate(false);
    }
}

void DecTextEdit::textSelectionUpdate(bool isResetScrollIndex) {
    std::string save = "";

    if (isPassword) {
        save = _text;
        _text = std::string(save.length(), '*');
    }

    textSelectionSetScrollIndex(isResetScrollIndex);

    if (isPassword) {
        _text = save;
    }
    dirty();
}

void DecTextEdit::textSelectionSelectAll() {
    textSelectionStartIndex = 0;
    textSelectionEndIndex = (int)_text.length();
    text_index = textSelectionEndIndex;
    textSelectionUpdate(true);
}

void DecTextEdit::textSelectionCut(bool doActionCmd) {
    if (!isPassword) {
        textSelectionCopy();
        textSelectionDelete();
        dirty();
    }
}

void DecTextEdit::textSelectionCopy() {
    if (!isPassword && textSelectionIsSelected()) {
        int startIndex = 0;
        int endIndex = 0;
        textSelectionGetSelectedIndex(&startIndex, &endIndex);
        std::string str = _text.substr(startIndex, (size_t)endIndex - startIndex);
        //SDL_SetClipboardText(str.c_str());
        dirty();
    }
}

void DecTextEdit::textSelectionPaste(bool doActionCmd) {
    bool success = false;

    if (textSelectionIsSelected()) {
        textSelectionDelete(false);
        success = true;
    }

    if (textSelectionIsValidClipboardText()) {
        const char* text = "PASTE";  // SDL_GetClipboardText();
        _text = _text.insert(text_index, std::string(text));
        text_index += (int)strlen(text);
        success = true;
    }

    textSelectionUpdate(false);

    //if (success && doActionCmd && action_cmd) {
    //    action_cmd(this);
    //}
}

void DecTextEdit::textSelectionDelete(bool doActionCmd) {
    if (textSelectionIsSelected()) {
        int startIndex = 0;
        int endIndex = 0;
        textSelectionGetSelectedIndex(&startIndex, &endIndex);
        _text = _text.substr(0, startIndex) + _text.substr(endIndex);
        text_index = startIndex;
        textSelectionStartIndex = text_index;
        textSelectionEndIndex = textSelectionStartIndex;
        textSelectionUpdate(false);

        //if (doActionCmd && action_cmd) {
        //    action_cmd(this);
        //}
    }
}

void DecTextEdit::textSelectionCancel() {
    text_index = 0;
    textSelectionScrollIndex = 0;
    textSelectionStartIndex = 0;
    textSelectionEndIndex = textSelectionStartIndex;

    textSelectionUpdate(true);
}

void DecTextEdit::textSelectionGoToIndex(int i) {
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

void DecTextEdit::textSelectionHome() {
    textSelectionGoToIndex(0);
    dirty();
}

void DecTextEdit::textSelectionEnd() {
    textSelectionGoToIndex((int)_text.length());
    dirty();
}

int DecTextEdit::textSelectionGetAreaWidth() {
    int areaWidth = _width - ((_padding[3] + _padding[1]) * 1);
    return areaWidth;
}

bool DecTextEdit::textSelectionIsThaiText(std::string text) {
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

bool DecTextEdit::textSelectionIsValidClipboardText() {
    bool result = false;

    //if (SDL_HasClipboardText()) {
    if (false) {
        const char* text = "";  // SDL_GetClipboardText();

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

bool DecTextEdit::textSelectionIsDigitText(std::string strText) {
    bool result = true;

    for (int i = 0; i < (int)strText.length() && result; i++) {
        unsigned char c = strText.at(i);

        if ((c & 0xc0) == 0xc0 || !(isdigit(c) || c == '.')) {
            result = false;
        }
    }
    return result;
}

bool DecTextEdit::textSelectionIsSelected() {
    return (_isActive && textSelectionStartIndex != textSelectionEndIndex);
}

void DecTextEdit::textSelectionGetSelectedIndex(int* startIndex, int* endIndex) {
    *startIndex = textSelectionStartIndex;
    *endIndex = textSelectionEndIndex;

    if (*endIndex < *startIndex) {
        *startIndex = textSelectionEndIndex;
        *endIndex = textSelectionStartIndex;
    }
}

void DecTextEdit::textSelectionSetCaretPosition(int x, int y) {
    //SkDebugf("[%s]%s text_index:%i\n", "", __func__, text_index);
    textSelectionIsMouseDown = true;
    text_index = textSelectionGetIndexAtScreenPosition(x);
    textSelectionStartIndex = text_index;
    textSelectionEndIndex = textSelectionStartIndex;
    textSelectionMousePoint.fX = x;
    textSelectionMousePoint.fY = y;
    dirty();
}
