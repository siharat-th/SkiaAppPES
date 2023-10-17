#include "DeclarativeUI.h"
#include "ParagraphTest.h"

DecContainer &createParagraphTest() {
    DecContainer *w = &Container();
      w->size( -1, -1 )
        .bg_color(0xFF6495ED)
        .border( false )
        ._( Paragraph()
            .setFontSize(14)
            .setFontStyle(false, false)
            .text( "สวัสดีชาวโลก นี่คือการทดสอบพารากราฟ แบบตัวธรรมดา ขนาด 14 และการจัดตัวอักษรในพื้นที่สี่เหลี่ยมที่ต้องตัดคำ")
            .addParagraph( " แบบตัวหนา ขนาด 20 เป็ดไก่ที่เป่าปี่", DecTextStyle().setFontSize(20).setFontStyle(true).setForegroundColor(SK_ColorBLUE).get() )
            .addParagraph( " จากนั้นจึงกลับมาใช้ค่าเริ่มต้น เป็ดไก่ที่เป่าปี่\n" )
            .addParagraph( "สวัสดีชาวโลก นี่คือการทดสอบพารากราฟ แบบตัวธรรมดา ขนาด 14 และการจัดตัวอักษรในพื้นที่สี่เหลี่ยมที่ต้องตัดคำ")
            .border(false)
            .align( ALIGN_ABSOLUTE )
            .x( 100 )
            .y( 100 )
            .size( 220, 220 )
            .padding( 10, 10, 10, 10 )

        )
        ;
		return (*w);
}



