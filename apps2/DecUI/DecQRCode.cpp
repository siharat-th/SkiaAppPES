#include "DecQRCode.h"

/*
site: https://fukuchi.org/works/qrencode/
git: https://github.com/fukuchi/libqrencode

https://github.com/JimmySoftware/FutureCrewHW/blob/master/ESP32/_0080_AdafruitThermal/examples/E_qr/E_qr.ino?fbclid=IwAR2_K9sUdoshjz6jnl9n1VDFgRKiNSB0ny7Uy160ce9abEwCUUdPdzSE2Ds#L73


https://www.thonky.com/qr-code-tutorial/data-encoding?fbclid=IwAR0qsIkWHFDIdceR_ww-mvkJaYtdIIS2y6xSsfHVoJ6jZ_h0yha6NO2XgWI

Maximum Data Capacity for each Micro QR Code Symbol Version:
=========================================================================
Symbol    Number    Error        Numeric   Alphanumeric   Binary   Kanji
version   of        correction
          modules   level
=========================================================================
 M1        11        -            5         -              -        -
=========================================================================
 M2        13        L            10        6              -        -
                     M            8         5              -        -
=========================================================================
 M3        15        L            23        14             9        6
                     M            18        11             7        4
=========================================================================
 M4        17        L            35        21             15       9
                     M            30        18             13       8
                     Q            21        13             9        5
=========================================================================


*/

DecQRCode::DecQRCode(const char* t) {
    qr = NULL;

    // qrborderwidth = _borderWidth;
    qrborderwidth = 0;
    qrbordercolor = SK_ColorLTGRAY;

    qrmargins = 4.0f;

    // wide range support configs should not be modified.
    casesensitive = 1;
    version = 0;
    level = QR_ECLEVEL_L;
    hint = QR_MODE_8;

    // configs that can be changed at any time.
    micro = 0;
    eightbit = 0;

    if (t) {
        text(t);
    }
}

DecQRCode::~DecQRCode() {}

void DecQRCode::_draw_content(SkCanvas& canvas) {
    if (!qr) {
        return;
    }

    const float qrmargins = this->qrmargins / (micro ? 2.0f : 1);
    const int qrwidth = qr->width;
    const float qrunits = qrmargins + qrwidth + qrmargins;
    const float qrsize = std::min(width(), height());
    const float unitsize = qrsize / qrunits;
    const float qrmarginsize = unitsize * qrmargins;

    const float ox = (width() - qrsize) / 2.0f;
    const float oy = (height() - qrsize) / 2.0f;

    SkAutoCanvasRestore acr(&canvas, true);
    SkPaint paint;
    paint.setStyle(SkPaint::Style::kFill_Style);
    paint.setColor(_text_color);

    canvas.translate(ox + qrmarginsize, oy + qrmarginsize);

    const uint8_t* row = qr->data;
    for (int y = 0; y < qrwidth; y++, row += qrwidth) {
        for (int x = 0; x < qrwidth; x++) {
            SkRect rect = SkRect::MakeXYWH(x * unitsize, y * unitsize, unitsize, unitsize);
            if (*(row + x) & 0x1) {
                canvas.drawRect(rect, paint);
            } else {
            }
        }
    }

    if (qrborderwidth > 0) {
        canvas.translate(-(ox + qrmarginsize), -(oy + qrmarginsize));
        const SkScalar stroke_halfw = qrborderwidth / 2.0f;
        SkRect border_rc = SkRect::MakeXYWH(ox, oy, qrsize, qrsize);
        border_rc.inset(stroke_halfw, stroke_halfw);
        paint.setStrokeWidth(qrborderwidth);
        paint.setStyle(SkPaint::Style::kStroke_Style);
        paint.setColor(qrbordercolor);
        canvas.drawRect(SkRect::MakeXYWH(ox, oy, qrsize, qrsize), paint);
    }
}

DecQRCode& QR(const char* text) {
    DecQRCode* w = new DecQRCode(text);
    return *w;
}

DecWidget& DecQRCode::text(const char* t) {
    DecWidget::text(t);
    int length = strlen(t);
    if (micro) {
        if (length > 15) {
            SkDebugf("Data length:%i\n", length);
            SkDebugf(
                    "Maximum data capacity for Micro QR Code in binary mode is 15 byte.\nexample: "
                    "\"กขคงจ\"= 15 byte\n");
        }

        if (eightbit) {
            qr = QRcode_encodeDataMQR(length, (const unsigned char*)t, version, level);
        } else {
            qr = QRcode_encodeStringMQR(t, version, level, hint, casesensitive);
        }
    } else {
        if (length > 2953) {
            SkDebugf("Data length:%i\n", length);
            SkDebugf("Maximum data capacity for QR Code in binary mode is 2953 byte.\n");
        }

        if (eightbit) {
            qr = QRcode_encodeData(length, (const unsigned char*)t, version, level);
        } else {
            qr = QRcode_encodeString(t, version, level, hint, casesensitive);
        }
    }

    return *this;
}
