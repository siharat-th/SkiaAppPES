#ifndef __DEC_PARAGRAPH_H__
#define __DEC_PARAGRAPH_H__

#include "modules/skparagraph/include/FontCollection.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/src/ParagraphBuilderImpl.h"
#include "modules/skparagraph/include/TypefaceFontProvider.h"
#include "DecWidget.h"
#include "include/private/base/SkTArray.h"

const int TEXTALIGN_LEFT = (int)skia::textlayout::TextAlign::kLeft;
const int TEXTALIGN_RIGHT = (int)skia::textlayout::TextAlign::kRight;
const int TEXTALIGN_CENTER = (int)skia::textlayout::TextAlign::kCenter;
const int TEXTALIGN_JUSTIFY = (int)skia::textlayout::TextAlign::kJustify;

class DecParagraph : public DecWidget {
public:    
    DecParagraph();
    virtual ~DecParagraph() override;

    virtual std::string type() override { return "Paragraph"; }

    DecParagraph &textAlign( int align ) {
        _textAlign = (skia::textlayout::TextAlign)align;
        return *this;
    }

    virtual void onResized(int w, int h) override;

    virtual DecParagraph& bg_color(uint32_t c) override;
    virtual DecParagraph& text_color(uint32_t c) override;

    virtual DecParagraph& text(const char* t) override;
    virtual DecParagraph& setText(const std::string &text, int size = 14, bool bold = false, bool italic = false);
    
    virtual DecParagraph& addParagraph(const std::string &text);
    virtual DecParagraph& addParagraph(const std::string &text, skia::textlayout::TextStyle textStyle);    
    
    virtual DecParagraph& setFontStyle(bool bold = false, bool italic = false);
    virtual DecParagraph& setFontSize(int size = 14);
    virtual DecParagraph& setSelection(int start, int end);
    virtual DecParagraph& selectable(bool b);
    virtual void reFormat();
    virtual void reLayout();

    std::unique_ptr<skia::textlayout::Paragraph> paragraph;
    size_t _firsttextlen;
    std::vector<std::tuple<size_t /* textlen */, skia::textlayout::TextStyle>> otherParagraphs;

    skia::textlayout::TextStyle _textStyle;
    skia::textlayout::ParagraphStyle _paraStyle;
    skia::textlayout::TextAlign _textAlign;

    sk_sp<skia::textlayout::TypefaceFontProvider> _font_provider;
    sk_sp<skia::textlayout::FontCollection> _fontCollection;

    SkTArray<size_t, true> _mapUTF16ToUTF8Index;
    SkTArray<size_t, true> _mapUTF8ToUTF16Index;
    SkOnce fillUTF16MappingOnce;

    bool _selectable;
    int _selectStart;
    int _selectEnd;

    static const std::vector<SkString>* kDefaultFontFamilies;

protected:
    bool _needReformat;
    std::vector<skia::textlayout::TextBox> _boxes;
    virtual void _draw_content(SkCanvas &canvas) override;
};

DecParagraph &Paragraph();

class DecTextStyle {
public:
    skia::textlayout::TextStyle _textStyle;
    DecTextStyle() : _textStyle() { 
        _textStyle.setFontFamilies(*DecParagraph::kDefaultFontFamilies); 
    }
    DecTextStyle(const skia::textlayout::TextStyle& textStyle) : _textStyle(textStyle) {
        _textStyle.setFontFamilies(*DecParagraph::kDefaultFontFamilies);
    }
    DecTextStyle& operator=(const skia::textlayout::TextStyle& textStyle) {
        _textStyle = textStyle;
        _textStyle.setFontFamilies(*DecParagraph::kDefaultFontFamilies);
        return *this;
    }

    skia::textlayout::TextStyle& get() { 
        return _textStyle; 
    }
    
    DecTextStyle& setFontSize(int size = 14) {
        _textStyle.setFontSize(size);
        return *this;
    }

    DecTextStyle& setFontFamily(const char* fontFamily) { 
        _textStyle.setFontFamilies({SkString(fontFamily)});
        return *this;
    }

    DecTextStyle& setFontFamilies(std::vector<SkString> families) {
        _textStyle.setFontFamilies(families);
        return *this;
    }

    DecTextStyle& setFontStyle(bool bold = false, bool italic = false) { 
        SkFontStyle fontStyle(
                bold ? SkFontStyle::Weight::kBold_Weight : SkFontStyle::Weight::kNormal_Weight,
                SkFontStyle::Width::kNormal_Width,
                italic ? SkFontStyle::Slant::kItalic_Slant : SkFontStyle::Slant::kUpright_Slant);
        _textStyle.setFontStyle(fontStyle);
        return *this;
    }

    DecTextStyle& setBackgroundColor(SkColor color) {
        SkPaint paint;
        paint.setColor(color);
        _textStyle.setBackgroundColor(paint);
        return *this;
    }

    DecTextStyle& setForegroundColor(SkColor color) {
        SkPaint paint;
        paint.setColor(color);
        _textStyle.setForegroundColor(paint);
        return *this;
    }
};

#endif //__DEC_PARAGRAPH_H__
