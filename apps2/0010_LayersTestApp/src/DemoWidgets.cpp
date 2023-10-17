#include "DemoWidgets.h"

// static DecWidget *widget = NULL;

DecTextEdit* _pTextEdit1;
DecTextEdit* _pTextEdit2;

DecWidget& createDemo() {
    return (Flow().bg_color(0xFF6495ED)
                    .gap(10)
                    .padding(10, 10, 10, 10)
                    .onDraw([](DecWidget& widget, SkCanvas& canvas) {
                        SkPaint paint;

                        paint.setColor(SK_ColorBLACK);
                        paint.setStyle(SkPaint::Style::kStroke_Style);
                        canvas.drawRect(
                                SkRect::MakeXYWH(10, 10, widget.width() - 20, widget.height() - 20),
                                paint);
                    })
                    // #1 show columns
                    ._(Widget().size(320, 280)._(Column().bounds(0, 0, -1, -1)
                                                         .bg_color(0xFFee666b)
                                                         .padding(5, 5, 5, 5)
                                                         .gap(10)
                                                         ._(Row().bounds(0, 0, -1, 40)
                                                                    .margin(5, 5, 5, 5)
                                                                    .padding(5, 5, 5, 5)
                                                                    .align(ALIGN_TOPLEFT)
                                                                    .bg_color(0xFF80CEE1)
                                                                    .gap(10)
                                                                    ._(Widget().size(40, -1))
                                                                    ._(Widget().size(40, -1))
                                                                    ._(Widget().size(-1, -1))
                                                                    ._(Widget().size(40, -1))
                                                                    ._(Widget().size(40, -1)))
                                                         ._(Row().bounds(0, 0, -1, 40)
                                                                    .align(ALIGN_TOPLEFT)
                                                                    .bg_color(0xFFfcd87d)
                                                                    .gap(10)
                                                                    ._(Widget().size(30, -1))
                                                                    ._(Widget().size(-1, -1))
                                                                    ._(Widget().size(30, -1))
                                                                    ._(Widget().size(-1, -1))
                                                                    ._(Widget().size(30, -1))
                                                                    ._(Widget().size(30, -1)))
                                                         ._(Row().bounds(0, 0, -1, 40)
                                                                    .bg_color(0xFF80CEE1)
                                                                    .gap(10)
                                                                    ._(Widget().size(-1, -1))
                                                                    ._(Widget().size(30, -1))
                                                                    ._(Widget().size(-1, -1)))
                                                         ._(Row().bounds(0, 0, -1, 50)
                                                                    .bg_color(0xFFfcd87d)
                                                                    .padding(10, 10, 10, 10)
                                                                    .gap(10)
                                                                    ._(Widget().size(30, -1))
                                                                    ._(Widget().size(-1, -1)))
                                                         ._(Row().bounds(0, 0, -1, -1)
                                                                    .bg_color(0xFF80CEE1)
                                                                    .padding(5, 5, 5, 5)
                                                                    .margin(5, 5, 5, 5)
                                                                    .align(ALIGN_TOPLEFT)
                                                                    .gap(10)
                                                                    ._(Widget().size(-1, -1))
                                                                    ._(Widget().size(30, -1)))))
                    // Show columns
                    ._(Widget().size(320, 280)._(Row().bounds(0, 0, -1, -1)
                                                         .bg_color(0xFFee666b)
                                                         .padding(5, 5, 5, 5)
                                                         .gap(10)
                                                         ._(Column().bounds(0, 0, 50, -1)
                                                                    .bg_color(0xFF80CEE1)
                                                                    .padding(5, 5, 5, 5)
                                                                    .margin(5, 5, 5, 5)
                                                                    .align(ALIGN_TOPLEFT)
                                                                    .gap(10)
                                                                    ._(Widget().size(-1, 30))
                                                                    ._(Widget().size(-1, 30))
                                                                    ._(Widget().size(-1, -1))
                                                                    ._(Widget().size(-1, 30))
                                                                    ._(Widget().size(-1, 30)))
                                                         ._(Column().bounds(50, 0, 50, -1)
                                                                    .bg_color(0xFFfcd87d)
                                                                    .align(ALIGN_TOPLEFT)
                                                                    .gap(10)
                                                                    ._(Widget().size(-1, 30))
                                                                    ._(Widget().size(-1, -1))
                                                                    ._(Widget().size(-1, 30))
                                                                    ._(Widget().size(-1, -1))
                                                                    ._(Widget().size(-1, 30))
                                                                    ._(Widget().size(-1, 30)))
                                                         ._(Column().bounds(100, 0, 50, -1)
                                                                    .bg_color(0xFF80CEE1)
                                                                    .gap(10)
                                                                    ._(Widget().size(-1, -1))
                                                                    ._(Widget().size(-1, 30))
                                                                    ._(Widget().size(-1, -1)))
                                                         ._(Column().bounds(150, 0, 50, -1)
                                                                    .bg_color(0xFFfcd87d)
                                                                    .gap(10)
                                                                    ._(Widget().size(-1, 30))
                                                                    ._(Widget().size(-1, -1)))
                                                         ._(Column().bounds(200, 0, -1, -1)
                                                                    .bg_color(0xFF80CEE1)
                                                                    .margin(5, 5, 5, 5)
                                                                    .padding(5, 5, 5, 5)
                                                                    .align(ALIGN_TOPLEFT)
                                                                    .gap(10)
                                                                    ._(Widget().size(-1, -1))
                                                                    ._(Widget().size(-1, 30)))))
                    // Stack & Pages
                    ._(Column().size(-1, 300)
                               .bg_color(0xFF80CEE1)
                               .gap(10)
                               ._(Row().size(-1, -1)
                                          .gap(10)
                                          .bg_color(0xFFee666b)
                                          .padding(5, 5, 5, 5)
                                          ._(Stack().size(-1, -1)
                                                     .bg_color(0xFFA0ffff)
                                                     .padding(5, 5, 5, 5)
                                                     ._(Widget().size(40, 50)
                                                                .align(ALIGN_TOPLEFT)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Stack().size(50, 50)
                                                                .align(ALIGN_TOPCENTER)
                                                                .margin(5, 5, 5, 5)
                                                                ._(Icon(0xf015)
                                                                           .fontSize(24)
                                                                           .align(ALIGN_CENTER)
                                                                           .border(true)))
                                                     ._(Widget().size(40, 50)
                                                                .align(ALIGN_TOPRIGHT)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(40, 100)
                                                                .align(ALIGN_VCENTERLEFT)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(50, 100)
                                                                .align(ALIGN_CENTER)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(40, 100)
                                                                .align(ALIGN_VCENTERRIGHT)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(40, 50)
                                                                .align(ALIGN_BOTTOMLEFT)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(50, 50)
                                                                .align(ALIGN_BOTTOMCENTER)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(40, 50)
                                                                .align(ALIGN_BOTTOMRIGHT)
                                                                .margin(5, 5, 5, 5)))
                                          ._(Pages().size(-1, -1)
                                                     ._(Widget().size(50, 50)
                                                                .align(ALIGN_TOPLEFT)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(120, 50)
                                                                .align(ALIGN_TOPCENTER)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(50, 50)
                                                                .align(ALIGN_TOPRIGHT)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(50, 120)
                                                                .align(ALIGN_VCENTERLEFT)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(120, 120)
                                                                .align(ALIGN_CENTER)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(50, 120)
                                                                .align(ALIGN_VCENTERRIGHT)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(50, 50)
                                                                .align(ALIGN_BOTTOMLEFT)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(120, 50)
                                                                .align(ALIGN_BOTTOMCENTER)
                                                                .margin(5, 5, 5, 5))
                                                     ._(Widget().size(50, 50)
                                                                .align(ALIGN_BOTTOMRIGHT)
                                                                .margin(5, 5, 5, 5)))
                                          ._(Pages().size(-1, -1)
                                                     ._(Stack().bg_color(0xFFFFA0A0)
                                                                .padding(5, 5, 5, 5)
                                                                ._(Text("Page 1").align(
                                                                        ALIGN_CENTER))
                                                                ._(Stack().size(0, 0)
                                                                           .align(ALIGN_TOPLEFT)
                                                                           .margin(5, 5, 5, 5)
                                                                           .padding(5, 5, 5, 5)
                                                                           ._(Icon(0xf015)
                                                                                      .fontSize(32)
                                                                                      .border(1)))
                                                                ._(Column().size(0, 0)
                                                                           .align(ALIGN_BOTTOMRIGHT)
                                                                           .margin(5, 5, 5, 5)
                                                                           .padding(5, 5, 5, 5)
                                                                           .gap(10)
                                                                           ._(Icon(0xf015)
                                                                                      .fontSize(32)
                                                                                      .border(1))
                                                                           ._(Icon(0xf015)
                                                                                      .fontSize(20)
                                                                                      .border(1)
                                                                                      .align(ALIGN_VCENTERLEFT))
                                                                           ._(Icon(0xf015)
                                                                                      .fontSize(20)
                                                                                      .border(1)
                                                                                      .align(ALIGN_CENTER))
                                                                           ._(Icon(0xf015)
                                                                                      .fontSize(20)
                                                                                      .border(1)
                                                                                      .align(ALIGN_VCENTERRIGHT))
                                                                           ._(Icon(0xf015)
                                                                                      .fontSize(32)
                                                                                      .border(1))))
                                                     ._(Stack().bg_color(0xFFA0FFA0)
                                                                ._(Text("Page 2").align(
                                                                        ALIGN_CENTER)))
                                                     ._(Stack().bg_color(0xFFA0A0FF)
                                                                ._(Text("Page 3").align(
                                                                        ALIGN_CENTER))))))
                    ._(Row().size(-1, 240)
                               .gap(10)
                               .bg_color(0xFFA0FFFF)
                               ._(Image("images/ElonMusk.png").align(ALIGN_CENTER).margin_left(10))
                               ._(Stack().size(0, -1)
                                          ._(Image("images/ElonMusk.png")
                                                     .margin(5, 10, 10, 10)
                                                     .align(ALIGN_TOPLEFT))
                                          ._(Stack().size(220, 70)
                                                     .align(ALIGN_BOTTOMCENTER)
                                                     .margin_bottom(5)
                                                     ._(Row().border(true)
                                                                .bg_color(0xFFfcd87d)
                                                                .gap(5)
                                                                .size(0, 0)
                                                                .padding(5, 5, 5, 5)
                                                                .align(ALIGN_BOTTOMCENTER)
                                                                ._(Icon(0xf015).fontSize(32).border(
                                                                        1))
                                                                ._(Text("Hello")
                                                                           .bg_color(0xFFFFA0A0)
                                                                           .border(1)
                                                                           .align(ALIGN_TOPCENTER))
                                                                ._(Text("สวัสดี")
                                                                           .bg_color(0xFFFFA0A0)
                                                                           .border(1)
                                                                           .align(ALIGN_CENTER))
                                                                ._(Text("Hi")
                                                                           .bg_color(0xFFFFA0A0)
                                                                           .border(1)
                                                                           .align(ALIGN_BOTTOMCENTER))
                                                                ._(Icon(0xf015).fontSize(32).border(
                                                                        1)))))
                               ._(Pages().size(320, -1)
                                          // Label test
                                          ._(Column().padding(10, 10, 10, 10)
                                                     .gap(10)
                                                     ._(Label("This is BIG label", 0xf015)
                                                                .fontSize(20))
                                                     ._(Text("This is text"))
                                                     ._(Icon(0xf015))
                                                     ._(Label("Align left", 0xf015))
                                                     ._(Label("Align center", 0xf015)
                                                                .align(ALIGN_CENTER))
                                                     ._(Label("Align right", 0xf015)
                                                                .align(ALIGN_VCENTERRIGHT)))
                                          ._(Column().padding(10, 10, 10, 10)
                                                     .gap(10)
                                                     ._(Label("Button Status", 0xf015).fontSize(20))
                                                     ._(Button("Button",
                                                               0xf015,
                                                               [=](DecButton& bt) {
                                                                   SkDebugf("I was be Clicked\n");
                                                               }))
                                                     ._(CheckBox("CheckBox",
                                                                 [](DecCheckBox& bt) {
                                                                     SkDebugf("I was be Checked\n");
                                                                 }))
                                                     ._(Column().size(0, 0)
                                                                ._(RadioButton("Radio Button 1"))
                                                                ._(RadioButton("Radio Button 2"))
                                                                ._(RadioButton("Radio Button 3")))
                                                     ._(Switch("Switch",
                                                               [](DecSwitch& sw) {
                                                                   SkDebugf("I was be Switch\n");
                                                               })))
                                          ._(Column().padding(10, 10, 10, 10)
                                                     .gap(10)
                                                     ._(Label("Slider & List", 0xf015).fontSize(20))
                                                     ._(Slider(0,
                                                               10,
                                                               1,
                                                               false,
                                                               [](DecSlider& slider) {
                                                                   SkDebugf("Slider value\n");
                                                               }))
                                                     ._(Row().gap(10)
                                                                .border(false)
                                                                ._(Column()._(Label("List 1")
                                                                                      .padding(0,
                                                                                               10,
                                                                                               0,
                                                                                               10))
                                                                           ._(Label("List 2")
                                                                                      .padding(0,
                                                                                               10,
                                                                                               0,
                                                                                               10))
                                                                           ._(Label("List 3")
                                                                                      .padding(0,
                                                                                               10,
                                                                                               0,
                                                                                               10))
                                                                           ._(Label("List 4")
                                                                                      .padding(0,
                                                                                               10,
                                                                                               0,
                                                                                               10)))
                                                                ._(Column()))))
                               // Green stack (fill end of row)
                               ._(Row().size(-1, -1)
                                          .bg_color(0xFFFFFFFF)
                                          .padding(10, 10, 10, 10)
                                          .gap(10)
                                          ._(Column().size(240, -1)
                                                     .border(false)
                                                     .gap(10)
                                                     ._(Container().size(-1, 32)._(
                                                             Widget().size(32, 32).draggableX(
                                                                     true)))
                                                     ._(Row().size(-1, -1)
                                                                .gap(10)
                                                                .border(false)
                                                                ._(Container().size(-1, -1)._(
                                                                        Widget().size(32, 32)
                                                                                .draggable(true)))
                                                                ._(Container().size(32, -1)._(
                                                                        Widget().size(32, 32)
                                                                                .draggableY(
                                                                                        true)))))
                                          ._(Container().size(-1, -1)._(Widget().size(32, 32)))))
                    ._(Widget().size(150, 150)
                               .bg_color(0xFFFFFFFF)
                               ._(Text("Hello ไก่ที่เป่าปี่ดีที่สุด!").align(ALIGN_CENTER)))
                    ._(Column().size(220, -1)
                               ._(Paragraph()
                                          .border(false)
                                          .size(-1, 100)
                                          .padding(10, 10, 10, 10)
                                          .text("สวัสดีชาวโลก นี่คือการทดสอบพารากราฟ แบบปกติ ขนาด 14 "
                                                "และการจัดตัวอักษรในพื้นที่สี่เหลี่ยมที่ต้องตัดคำ ไก่ที่เป่าปี่ดีที่สุด"))
                               ._(QR("https://magicpattern.co/product/magic-pattern-0711/")
                                          .size(-1, -1)
                                          .cache(true)))
                    ._(Paragraph()
                               .text("สวัสดีชาวโลก นี่คือการทดสอบพารากราฟ แบบตัวหนา ขนาด 20 "
                                     "และการจัดตัวอักษรในพื้นที่สี่เหลี่ยมที่ต้องตัดคำ")
                               .setFontSize(20)
                               .setFontStyle(true, false)
                               .border(false)
                               .size(220, 220)
                               .padding(10, 10, 10, 10))
                    ._(Paragraph()
                               .setText("สวัสดีชาวโลก นี่คือการทดสอบพารากราฟ แบบตัวหนา+เอียง ขนาด 18 "
                                        "และการจัดตัวอักษรในพื้นที่สี่เหลี่ยมที่ต้องตัดคำไก่ที่เป่าปี่ดีที่สุด",
                                        18,
                                        true,
                                        true)
                               .border(false)
                               .size(220, 220)
                               .padding(10, 10, 10, 10))

                    ._(Column().size(200, 100)
                               .padding(5, 5, 5, 5)
                               .gap(5)
                               ._(TextEdit("ทดสอบ1 พิมพ์แล้วกด Enter",
                                           [=](DecTextEdit& textedit) {
                                               SkDebugf("PESEditor::_pTextEdit1::callback: %s\n",
                                                        textedit.text());
                                               _pTextEdit2->setText(textedit.text());
                                           })
                                          .size(-1, 0)
                                          .ref((DecWidget**)&_pTextEdit1))

                               ._(TextEdit("ทดสอบ2 พิมพ์แล้วกด Enter",
                                           [=](DecTextEdit& textedit) {
                                               SkDebugf("PESEditor::_pTextEdit2::callback: %s\n",
                                                        textedit.text());
                                               _pTextEdit1->setText(textedit.text());
                                           })
                                          .size(-1, 0)
                                          .ref((DecWidget**)&_pTextEdit2)))

                    ._(Paragraph()
                               .setText("สวัสดีชาวโลก นี่คือการทดสอบพารากราฟ โดยกำหนดค่าเริ่มต้นเป็น แบบปกติ "
                                        "ขนาด 16 ไก่ที่เป่าปี่ดีที่สุด",
                                        16)
                               .addParagraph(
                                       " แบบตัวหนา ขนาด 20",
                                       DecTextStyle()
                                               .setFontSize(20)
                                               .setFontStyle(true)
                                               .setForegroundColor(SK_ColorBLUE)
                                               .get())
                               .addParagraph(
                                       " แบบตัวเอียง ขนาด 24",
                                       DecTextStyle()
                                               .setFontSize(24)
                                               .setFontStyle(false, true)
                                               .setForegroundColor(SK_ColorRED)
                                               .get())
                               .addParagraph(" จากนั้นจึงกลับมาใช้ค่าเริ่มต้น")
                               .selectable(true) 
                               .border(false)
                               .size(-1, -1)
                               .padding(10, 10, 10, 10)));
}
