#include "DecParagraph.h"
#include "DecUI.h"
#include "include/core/SkData.h"
#include "include/core/SkEncodedImageFormat.h"
#include "include/core/SkRRect.h"
#include "include/core/SkSurface.h"
#include "modules/skparagraph/include/FontCollection.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/TypefaceFontProvider.h"
#include "modules/skparagraph/src/ParagraphBuilderImpl.h"
#include "modules/skparagraph/src/ParagraphImpl.h"
#include "modules/skparagraph/src/ParagraphPainterImpl.h"
#include "tools/Resources.h"

using namespace skia::textlayout;

// std::string __text = "Hello world. this  👩🏾‍⚕️ 👨🏾‍⚕️ 👩🏾‍🌾
// 👨🏾‍🌾 is a paragraph. Hello world. this is a paragraph. Hello world. this is a
// paragraph. สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก
// สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ
// สวัสดีชาวโลก เป็นมนุษย์สุดประเสริฐเลิศคุณค่า กว่าบรรดาฝูงสัตว์เดรัจฉาน จงฝ่าฟันพัฒนาวิชาการ อย่าล้างผลาญฤๅเข่นฆ่าบีฑาใคร
// ไม่ถือโทษโกรธแช่งซัดฮึดฮัดด่า หัดอภัยเหมือนกีฬาอัชฌาสัย ปฏิบัติประพฤติกฎกำหนดใจ พูดจาให้จ๊ะ ๆ จ๋า ๆ น่าฟังเอยฯ";

const std::vector<SkString>* DecParagraph::kDefaultFontFamilies =
        new std::vector<SkString>{SkString("Kanit"), SkString("Emoji")};

DecParagraph::DecParagraph() {
    _needReformat = false;

    paragraph = nullptr;
    _textSize = 14;
    _textAlign = TextAlign::kLeft;

    _firsttextlen = 0;
    _selectable = false;
    _selectStart = 0;
    _selectEnd = 0;

    SkPaint txtPaint;
    txtPaint.setAntiAlias(true);
    txtPaint.setColor(_text_color);

    SkPaint bgPaint;
    bgPaint.setColor(0x00000000);

    _textStyle.setBackgroundColor(bgPaint);
    _textStyle.setForegroundColor(txtPaint);
    _textStyle.setFontFamilies(*kDefaultFontFamilies);
    _textStyle.setFontSize(_textSize);

    _paraStyle.setTextStyle(_textStyle);
    _paraStyle.setTextAlign(_textAlign);

    // text( "Hello world. this  👩🏾‍⚕️ 👨🏾‍⚕️ 👩🏾‍🌾 👨🏾‍🌾
    // is a paragraph. Hello world. this is a paragraph. Hello world. this is a paragraph.
    // สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ
    // สวัสดีชาวโลก สวัสดีชาวโลกนี่คือการทดสอบการทำงานของพารากราฟ สวัสดีชาวโลก เป็นมนุษย์สุดประเสริฐเลิศคุณค่า
    // กว่าบรรดาฝูงสัตว์เดรัจฉาน จงฝ่าฟันพัฒนาวิชาการ อย่าล้างผลาญฤๅเข่นฆ่าบีฑาใคร ไม่ถือโทษโกรธแช่งซัดฮึดฮัดด่า
    // หัดอภัยเหมือนกีฬาอัชฌาสัย ปฏิบัติประพฤติกฎกำหนดใจ พูดจาให้จ๊ะ ๆ จ๋า ๆ น่าฟังเอยฯ" );
}

DecParagraph::~DecParagraph() {}

DecParagraph& Paragraph() {
    DecParagraph* w = new DecParagraph();
    return *w;
}

DecParagraph& DecParagraph::bg_color(uint32_t c) {
    DecWidget::bg_color(c);

    SkPaint bgPaint;
    bgPaint.setColor(_bg_color);

    _textStyle.setBackgroundColor(bgPaint);
    _needReformat = true;
    return *this;
}

DecParagraph& DecParagraph::text_color(uint32_t c) {
    DecWidget::text_color(c);

    SkPaint txtPaint;
    txtPaint.setAntiAlias(true);
    txtPaint.setColor(_text_color);

    _textStyle.setForegroundColor(txtPaint);
    _needReformat = true;
    return *this;
}

DecParagraph& DecParagraph::text(const char* t) {
    DecWidget::text(t);
    _firsttextlen = _text.length();
    _needReformat = true;
    return *this;
}

DecParagraph& DecParagraph::setText(const std::string& text, int size, bool bold, bool italic) {
    setFontSize(size);
    setFontStyle(bold, italic);
    return this->text(text.c_str());
}

DecParagraph& DecParagraph::setFontStyle(bool bold, bool italic) {
    SkFontStyle fontStyle(
            bold ? SkFontStyle::Weight::kBold_Weight : SkFontStyle::Weight::kNormal_Weight,
            SkFontStyle::Width::kNormal_Width,
            italic ? SkFontStyle::Slant::kItalic_Slant : SkFontStyle::Slant::kUpright_Slant);
    _textStyle.setFontStyle(fontStyle);
    _needReformat = true;
    return *this;
}

DecParagraph& DecParagraph::setFontSize(int size) {
    _textSize = size;
    _textStyle.setFontSize(size);
    _needReformat = true;
    return *this;
}

DecParagraph& DecParagraph::setSelection(int start, int end) {
    _selectStart = start;
    _selectEnd = end;
    return *this;
}

DecParagraph& DecParagraph::selectable(bool b) {
    _selectable =  b;
    return *this;
}

DecParagraph& DecParagraph::addParagraph(const std::string &text) {
    return this->addParagraph(text, _textStyle);
}

DecParagraph& DecParagraph::addParagraph(const std::string &text, skia::textlayout::TextStyle textStyle) {
    if (!text.empty()) {
        _text += text;
        otherParagraphs.push_back({text.length(), textStyle});
        _needReformat = true;
        dirty(true, true);
    }
    return *this;
}

void DecParagraph::onResized(int w, int h) {
    DecWidget::onResized(w, h);
    reLayout();
}

void DecParagraph::reFormat() {
    _needReformat = true;
    if (!_text.length()) return;
    if (width() == 0 || height() == 0) return;
    if (!_ui) return;
    if (!_ui->font()) return;

    // SkDebugf( "Paragraph reformat\n" );

    if (!_font_provider.get()) {
        auto f = _ui->font();
        if (!f->_typeface[FONT_REGULAR]) return;
        if (!f->_typeface[FONT_BOLD]) return;
        if (!f->_typeface[FONT_BOLDITALIC]) return;
        if (!f->_typeface[FONT_ITALIC]) return;
        // if (!f->_typeface[FONT_ICON]) return;
        // if (!f->_typeface[FONT_EMOJI]) return;

        _font_provider = sk_make_sp<skia::textlayout::TypefaceFontProvider>();
        _font_provider->registerTypeface(f->_typeface[FONT_REGULAR], SkString("Kanit"));
        _font_provider->registerTypeface(f->_typeface[FONT_BOLD], SkString("Kanit"));
        _font_provider->registerTypeface(f->_typeface[FONT_BOLDITALIC], SkString("Kanit"));
        _font_provider->registerTypeface(f->_typeface[FONT_ITALIC], SkString("Kanit"));
        _font_provider->registerTypeface(f->_typeface[FONT_ICON], SkString("Icon"));
        _font_provider->registerTypeface(f->_typeface[FONT_EMOJI], SkString("Emoji"));
    }

    if (!_fontCollection.get()) {
        _fontCollection = sk_make_sp<skia::textlayout::FontCollection>();
        _fontCollection->setAssetFontManager(std::move(_font_provider));
        _fontCollection->setDefaultFontManager(SkFontMgr::RefDefault());
    }

    _paraStyle.setTextStyle(_textStyle);
    ParagraphBuilderImpl builder(_paraStyle, _fontCollection);
    const char* cstr = _text.c_str();
    size_t textlen = _firsttextlen;
    builder.addText(cstr, textlen);

    for (auto para : otherParagraphs) {
        cstr += textlen;
        textlen = std::get<0>(para);
        skia::textlayout::TextStyle textStyle(std::get<1>(para));
        builder.pushStyle(textStyle);
        builder.addText(cstr, textlen);
        builder.pop();
    }

    paragraph = builder.Build();

    if (paragraph) reLayout();

    _needReformat = false;
}

void DecParagraph::reLayout() {
    if (!paragraph) {
        reFormat();
        return;
    }
    paragraph->layout(width() - (_padding[1] + _padding[3]));

    skia::textlayout::ParagraphImpl* impl = static_cast<ParagraphImpl*>(paragraph.get());
    //auto text = impl->text();
    //auto textdata = text.data();
    auto textdata = _text.c_str();
    SkDebugf("textdata[%zu]'%.*s'\n", _text.length(), (int)_text.length(), textdata);

    for (auto& line : impl->lines()) {
        TextRange textRange = line.text();
        SkDebugf("line[%d]: offset(%.02f,%.02f), textdata[%zu]'%.*s'\n",
                 (int)(&line - impl->lines().begin()),
                 line.offset().fX,
                 line.offset().fY,
                 textRange.width(),
                 (int)textRange.width(),
                 &textdata[textRange.start]);
        line.iterateThroughVisualRuns(
                true,
                [&](const Run* run, SkScalar runOffset, TextRange textRange, SkScalar* width) {
                    //*width = line.measureTextInsideOneRun(textRange, run, runOffset, 0, true, false)
                    //                 .clip.width();
                    SkDebugf(" RunIndex[%zu], textdata[%zu:%zu] @%f + %f %s, textdata[%zu]'%.*s'\n",
                             run->index(),
                             textRange.start,
                             textRange.end,
                             runOffset,
                             *width,
                             run->leftToRight() ? "left" : "right",
                             textRange.width(),
                             (int)textRange.width(),
                             &textdata[textRange.start]);
                    return true;
                });
    }

    //_boxes.clear();
    if (_selectable) {
        //skia::textlayout::ParagraphImpl* impl = static_cast<ParagraphImpl*>(paragraph.get());
        //const std::string text = impl->text().data();
        const std::string& text = _text;
        SkDebugf("text[%zu]'%.*s'\n", text.size(), (int)text.size(), text.c_str());
        
        // UTF16Mapping;
        fillUTF16MappingOnce([&] {
            // Fill out code points 16
            auto ptr = text.c_str();
            auto end = text.c_str() + text.size();
            while (ptr < end) {
                size_t index = ptr - text.c_str();
                SkUnichar u = SkUTF::NextUTF8(&ptr, end);

                // All utf8 units refer to the same codepoint
                size_t next = ptr - text.c_str();
                for (auto i = index; i < next; ++i) {
                    _mapUTF8ToUTF16Index.emplace_back(_mapUTF16ToUTF8Index.size());
                }
                SkASSERT(_mapUTF8ToUTF16Index.size() == next);

                // One or two codepoints refer to the same text index
                uint16_t buffer[2];
                size_t count = SkUTF::ToUTF16(u, buffer);
                _mapUTF16ToUTF8Index.emplace_back(index);
                if (count > 1) {
                    _mapUTF16ToUTF8Index.emplace_back(index);
                }
            }
            _mapUTF8ToUTF16Index.emplace_back(_mapUTF16ToUTF8Index.size());
            _mapUTF16ToUTF8Index.emplace_back(text.size());
        });

        int from = 0;
        int to = text.size();
        if (text.size() > 9) from += 9;
        if (text.size() > 9) to -= 9;
        /*
        SkDebugf(
            "text[%zu], "
            "mapUTF16ToUTF8Index[%zu], "
            "mapUTF8ToUTF16Index[%zu], "
            "selection UTF8Index[%d: %d], "
            "selection UTF16Index[%zu: %zu]"
            "\n",
            text.size(),
            (int)_mapUTF16ToUTF8Index.size(),
            (int)_mapUTF8ToUTF16Index.size(),
            from,
            to,
            _mapUTF8ToUTF16Index[from],
            _mapUTF8ToUTF16Index[to]);
        */
        RectHeightStyle rect_height_style = RectHeightStyle::kMax;
        RectWidthStyle rect_width_style = RectWidthStyle::kTight;
        _boxes = paragraph->getRectsForRange(_mapUTF8ToUTF16Index[from],
                                             _mapUTF8ToUTF16Index[to],
                                             rect_height_style,
                                             rect_width_style);
    }
}

void DecParagraph::_draw_content(SkCanvas& canvas) {
    if (!_text.length()) return;

    canvas.save();
    canvas.clipRect(SkRect::MakeWH(width(), height()));
    canvas.drawColor(SK_ColorWHITE);

    if (_needReformat) {
        reFormat();
        _boxes.clear();
    }

    // paragraph->paint(&canvas, _padding[3], _padding[0]);
    skia::textlayout::ParagraphImpl* impl = static_cast<ParagraphImpl*>(paragraph.get());

    int x = _padding[3];
    int y = _padding[0];
    for (skia::textlayout::TextLine& line : impl->lines()) {
        float yy = line.offset().fY;
        float hh = line.height();
        if (yy + hh < height() - _padding[2]) {
            CanvasParagraphPainter painter(&canvas);
            line.paint(&painter, x, y);
            //line.paint(&canvas, x, y); // Jimmy - to fix
        }
    }

    if (_selectable) {
        auto drawRects = [](SkCanvas& canvas,
                            int x,
                            int y,
                            SkColor color,
                            std::vector<TextBox>& result,
                            bool fill = false) -> void {
            canvas.save();
            canvas.translate(x, y);
            SkPaint paint;
            if (!fill) {
                paint.setStyle(SkPaint::kStroke_Style);
                paint.setAntiAlias(true);
                paint.setStrokeWidth(1);
            }
            paint.setColor(color);
            for (auto& r : result) {
                canvas.drawRect(r.rect, paint);
            }
            canvas.restore();
        };

        drawRects(canvas,
                  _padding[3],
                  _padding[0],
                  SkColorSetARGB(0x90, 0x20, 0x20, 0xFF),
                  _boxes,
                  true);
    }

    canvas.restore();
}
