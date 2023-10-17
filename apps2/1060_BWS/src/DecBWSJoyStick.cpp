#include "DeclarativeUI.h"
#include "DecBWSJoyStick.h"
#include "include/core/SkPath.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

// gamepad.axes [L3Dx, L3Dy, R3Dx, R3Dy]
// gamepad.buttons
//  0. A
//  1. B
//  2. X
//  3. Y
//  4. L1
//  5. R1
//  6. L2
//  7. R2 -> Shift
//  8. SELECT
//  9. STRAT
// 10. L3D(down press)
// 11. R3D(down press)
// 12. DPAD-U
// 13. DPAD-D
// 14. DPAD-L
// 15. DPAD-R
// 16. None

void htmlAddGamePadEventListener(const DecWidget* pWidget) {
#ifdef __EMSCRIPTEN__
    EM_ASM(
    {
        var pWidget = $0;
        var strGamePadConnected = "gamepadconnected";
        var strGamePadDisconnected = "gamepaddisconnected";
        window.addEventListener(strGamePadConnected, (event) => {
          CanvasKit['decUtility']['callback_widget_text'](pWidget, strGamePadConnected);
        });
        window.addEventListener(strGamePadDisconnected, (event) => {
          CanvasKit['decUtility']['callback_widget_text'](pWidget, strGamePadDisconnected);
        });
    },
    pWidget);
#endif
}

void htmlGetGamePads(int32_t* pInt4Byte16, int32_t* pInt16Byte64) {
#ifdef __EMSCRIPTEN__
    EM_ASM(
            {
                var pInt4 = $0;
                var pInt16 = $1;
                var len = 0;

                var gamepads = navigator.getGamepads();
                if (!gamepads) {
                    return;
                }

                var gamepad = null;
                len = gamepads.length;
                for (var n = 0; n < len; ++n) {
                    gamepad = gamepads[n];
                    if (gamepad) break;
                }
                if (!gamepad) {
                    return;
                }

                var axes = gamepad.axes;
                len = axes.length;
                if (len > 4) len = 4;
                for (var n = 0, offset = 0; n < len; ++n, offset += 4) {
                    HEAP32[(pInt4 + offset) >> 2] = (axes[n] * 1000) | 0;
                }

                var buttons = gamepad.buttons;
                len = buttons.length;
                if (len > 16) len = 16;
                for (var n = 0, offset = 0; n < len; ++n, offset += 4) {
                    HEAP32[(pInt16 + offset) >> 2] = buttons[n].value | 0;
                }

                //var pres = document.querySelectorAll('pre');
                //pres[1].innerText = "axes:" + axes.map((v, i) => ' ' + i + ':' + (v).toFixed(2));
                //pres[2].innerText = "buttons:" + buttons.map((v, i) => ' ' + i + ':' + v.value);
            },
            pInt4Byte16,
            pInt16Byte64);
#endif
}

DecBWSJoyStick::DecBWSJoyStick() {
    size( 140, 500 );
    border( false );
    background( false );
    bGamepadConnected = false;
    bJoyUp = false;
    bJoyLeft = false;
    bJoyRight = false;
    bJoyDown = false;

    bDUp = false;
    bDLeft = false;
    bDRight = false;
    bDDown = false;

    bA = false;
    bB = false;

    bR2 = false;

    jX = 0;
    jY = 0;
    htmlAddGamePadEventListener(this);
}

DecBWSJoyStick::~DecBWSJoyStick(){

} 

DecBWSJoyStick &BWSJoyStick() {
    DecBWSJoyStick *w = new DecBWSJoyStick();
    return *w;
}

void DecBWSJoyStick::update() {
    if (bGamepadConnected) {
        int32_t axes[4] = {0};
        int32_t buttons[16] = {0};
        htmlGetGamePads(axes, buttons);
        float faxes[4] = {axes[0] / 1000.0f, axes[1] / 1000.0f, axes[2] / 1000.0f, axes[3] / 1000.0f};
        //SkDebugf("[%.03f,%.03f] [%.03f,%.03f]\n", faxes[0], faxes[1], faxes[2], faxes[3]);
        //SkDebugf("%d, %d, %d, %d\n",  buttons[0],  buttons[1],  buttons[2],  buttons[3]);
        //SkDebugf("%d, %d, %d, %d\n",  buttons[4],  buttons[5],  buttons[6],  buttons[7]);
        //SkDebugf("%d, %d, %d, %d\n",  buttons[8],  buttons[9], buttons[10], buttons[11]);
        //SkDebugf("%d, %d, %d, %d\n", buttons[12], buttons[13], buttons[14], buttons[15]);

        bA = buttons[0];
        bB = buttons[1];

        bR2 = buttons[7];

        bDUp = buttons[12];
        bDDown = buttons[13];
        bDLeft = buttons[14];
        bDRight = buttons[15];

        jX = faxes[0];
        jY = faxes[1];
    }
}

void DecBWSJoyStick::drawD( SkCanvas &canvas, int x, int y, int degree, SkPaint &paint ) {
    canvas.save();
    canvas.translate( x, y );

    canvas.rotate( degree );

    SkPath path;
    path.moveTo( 0, 0 );
    path.lineTo( 15, -15 );
    path.lineTo( 15, -45 );
    path.lineTo( -15, -45 );
    path.lineTo( -15, -15 );
    path.close();

    canvas.drawPath( path, paint );
    canvas.restore();
}

void DecBWSJoyStick::_draw_content(SkCanvas& canvas) {
    int w = width();
    int w2 = w/2;
    int h = height();

    SkPath path;
    path.moveTo( 1, w2 );
    path.arcTo( SkPoint::Make( 1, 1 ), SkPoint::Make( w2, 1 ), w2-1 );
    path.arcTo( SkPoint::Make( w-1, 1 ), SkPoint::Make( w-1, w2 ), w2-1 );
    path.lineTo( w-1, h-w2 );
    path.arcTo( SkPoint::Make( w-1, h-1 ), SkPoint::Make( w2, h-1 ), w2-1 );
    path.arcTo( SkPoint::Make( 1, h-1 ), SkPoint::Make( 1, h-w2 ), w2-1 );
    path.close();

    SkPaint paintA;
    paintA.setColor( 0xFFE0E0E0 );
    paintA.setStyle( SkPaint::Style::kFill_Style );

    SkPaint paint;
    paint.setStrokeWidth( 1 );
    paint.setStyle( SkPaint::Style::kStroke_Style );

    // JoyStick outline
    canvas.drawPath( path, paint );

    // D3D outline
    canvas.drawCircle( SkPoint::Make( w2, w-20 ), w2-15, paint );

    // D3D center
    if( jX != 0 || jY != 0 ) {
        canvas.drawCircle( SkPoint::Make( w2 + jX * 30, w-20 + jY * 30 ), 25, paintA );
    }
    canvas.drawCircle( SkPoint::Make( w2 + jX * 30, w-20 + jY * 30 ), 25, paint );
    //SkDebugf("[%.03f,%.03f]\n", jX, jY);

    SkFont font;
    font.setTypeface(_font->typeface(FONT_REGULAR));
    font.setSubpixel(true);
    font.setSize( 20 );

    SkFontMetrics metrics;
    font.getMetrics(&metrics);

    SkPaint paintText;
    paintText.setColor( 0xFFB0B0B0 );
    paintText.setStyle( SkPaint::Style::kFill_Style );

    SkRect rect;

    if( bDUp ) {
        drawD( canvas, w2,      w * 2 - 5 - 20,  0, paintA );
    }
    if( bDRight ) {
        drawD( canvas, w2 + 5,  w * 2 - 20,      90, paintA );
    }
    if( bDDown ) {
        drawD( canvas, w2,      w * 2 + 5 - 20,  180, paintA );
    }
    if( bDLeft ) {
        drawD( canvas, w2 - 5,  w * 2 - 20,      270, paintA );
    }

    drawD( canvas, w2,      w * 2 - 5 - 20,  0, paint );
    drawD( canvas, w2 + 5,  w * 2 - 20,      90, paint );
    drawD( canvas, w2,      w * 2 + 5 - 20,  180, paint );
    drawD( canvas, w2 - 5,  w * 2 - 20,      270, paint );

    std::string text = "R";
    sk_sp<SkTextBlob> blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
    int ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 
    canvas.drawTextBlob(blob, w2 - (ww/2), w * 2 - 45, paintText);  

    text = "F";
    blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
    ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 
    canvas.drawTextBlob(blob, w2 - (ww/2), w * 2 + 40 + metrics.fAscent, paintText);  

    text = "W";
    blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
    ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 
    canvas.drawTextBlob(blob, w2 - (ww/2), w - 52, paintText);  

    text = "S";
    blob = SkTextBlob::MakeFromText(text.c_str(), text.length(), font);
    ww = font.measureText( text.c_str(), text.length(), SkTextEncoding::kUTF8, &rect ); 
    canvas.drawTextBlob(blob, w2 - (ww/2), w + 50 + metrics.fAscent, paintText);  

    if( bA ) {
        canvas.drawCircle( SkPoint::Make( w2 - 30, w * 3 - 25 ), 15, paintA );
    }
    if( bB ) {
        canvas.drawCircle( SkPoint::Make( w2 + 30, w * 3 - 25 ), 15, paintA );
    }

    canvas.drawCircle( SkPoint::Make( w2 - 30, w * 3 - 25 ), 15, paint );
    canvas.drawCircle( SkPoint::Make( w2 + 30, w * 3 - 25 ), 15, paint );

}