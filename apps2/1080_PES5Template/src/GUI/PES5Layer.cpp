//
//  PES5Layer.cpp
//  
//
//  Created by SIHARAT THAMMAYA on 17/8/2565 BE.
//

#include "PES5Layer.hpp"
#include "tools/Resources.h"

// cross platforms
#ifdef __EMSCRIPTEN__
#else
#if defined(SK_BUILD_FOR_MAC)
#elif defined(SK_BUILD_FOR_WIN)
#elif defined(SK_BUILD_FOR_UNIX)
#elif defined(SK_BUILD_FOR_ANDROID)
#elif defined(SK_BUILD_FOR_IOS)
#endif
#endif

#include "DecUI.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

#include "PES5Topbar.hpp"
#include "PES5Toolbox.hpp"
#include "PES5DocumentView.hpp"
#include "PES5RightPanel.hpp"
#include "PES5Command.hpp"
#include "PES5Template.hpp"

long PES5_GLOBAL_FLAGS = 0;

namespace sk_ui {

Pes5Layer::Pes5Layer( std::string AppTitle ) {
    UI().font(Font().Regular("fonts/Kanit-Regular.ttf")
                      .Bold("fonts/Kanit-Bold.ttf")
                      .Italic("fonts/Kanit-Italic.ttf")
                      .BoldItalic("fonts/Kanit-BoldItalic.ttf")
                      .Icon("fonts/Font Awesome 6 Pro-Regular-400.otf")
                      .Emoji("fonts/NotoColorEmoji.ttf"))
            .root(
                Column()
                    .size(-1, -1)
                    .border(false)
                    .cache(true)
                    ._(Row().size(-1, 40)
                        .hideOnFlag(PES5_GLOBAL_FLAGS & PES5_FLAG_HIDE_TITLE_BAR)
                        .bg_color(0xFFF0F0F0)
                        .border_color(0xFF808080)
                        ._(Spacer().size(-1, -1))

                        ._(Text(AppTitle.c_str()).fontSize(18).align(ALIGN_CENTER))

                        ._(Spacer().size(-1, -1))
                    )
                    ._(CreatePesTopbar().cache(true))
                    ._(Row().size(-1, -1).border(false)
                        ._(CreatePesToolbox().cache(true))
                        ._(CreatePesDocumentView().border(false).cache(true))
                        ._(CreatePesRightPanel().cache(true))
                    )
            );
    // ***
    UpdateGUIRightPanel();
}

#ifdef __EMSCRIPTEN__
bool isMac = []() {
    return (bool)EM_ASM_INT({ return navigator.userAgent.includes('Mac OS X') ? 1 : 0; });
}();
#else
// native
#if defined(SK_BUILD_FOR_MAC)
bool isMac = true;
#else
bool isMac = false;
#endif  //SK_BUILD_FOR_MAC
#endif  //__EMSCRIPTEN__

uint8_t u8platformCtrlKey = isMac ? (uint8_t)skui::ModifierKey::kCommand : (uint8_t)skui::ModifierKey::kControl;

bool Pes5Layer::onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod){
//    SkDebugf("key = %d\n", k);
    uint8_t u8mod = (uint8_t)mod;
    bool isPlatformCtrlKey = (bool)(u8mod & u8platformCtrlKey);
    bool isNoneKey    = (u8mod & ~(uint8_t)skui::ModifierKey::kFirstPress) == 0;
    bool isCommandKey = (bool)(u8mod & (uint8_t)skui::ModifierKey::kCommand);
    bool isControlKey = (bool)(u8mod & (uint8_t)skui::ModifierKey::kControl);
    bool isOptionKey  = (bool)(u8mod & (uint8_t)skui::ModifierKey::kOption);
    bool isShiftKey   = (bool)(u8mod & (uint8_t)skui::ModifierKey::kShift);
    bool isFirstPress = (bool)(u8mod & (uint8_t)skui::ModifierKey::kFirstPress);

    bool isdirty = false;    
    bool isHandled = false;

    if (PES5_IsStitchSimulatorMode()) {
        if (isFirstPress && state == skui::InputState::kDown) {
            switch (k) { 
                default:
                    break;
            
                // play/pause
                //case skui::Key::kSpace:
                //    isdirty = true;
                //    break;

                // exit
                case skui::Key::kOK:
                    PES5_StopStitchSimulatorMode();
                    isdirty = true;
                    break;
            }

#ifdef __EMSCRIPTEN__
            if (isdirty || isHandled) {
                EM_ASM({ globalThis['isHandled'] = true; });
            }
#endif
            if (isdirty) {
                this->_ui->rootWidget->dirty(isdirty, false);
            }
            return DecUILayer::onKey(k, state, mod);
        }
    }


    if(k==skui::Key::kOK && state==skui::InputState::kDown){
        PES5_StopSatinColumnInput();
        PES5_StopDrawingInput();
        PES5_StopLinesToolInput();
        PES5_StopPolygonToolInput();
        PES5_StopPathEditInput();
        PES5_StopStitchEditInput();
        PES5_StopStitchSimulatorMode();
        isdirty = true;
    }
    else if(k==skui::Key::kBack && state==skui::InputState::kDown){
        if(PES5_IsSatinColumnInputStarted()){
            PES5_SatinColumnRemovePreviousPoint();
            isdirty = true;
        }
        else if(PES5_IsLinesToolInputStarted()){
            PES5_LinesToolRemovePreviousPoint();
            isdirty = true;
        }
        else if(PES5_IsPolygonToolInputStarted()){
            PES5_PolygonToolRemovePreviousPoint();
            isdirty = true;
        }
    }
    else if(k==skui::Key::kShift){
        if(state==skui::InputState::kDown){
            PES5_SetShiftPressed(true);
        }
        else if(state==skui::InputState::kUp){
            PES5_SetShiftPressed(false);
        }
    }
    else if(state==skui::InputState::kDown){
        int spd = PES5_IsShiftPressed() ? 5 : 1;
        float tx = 0;
        float ty = 0;
        if(k==skui::Key::kUp){
            ty=-spd;
            isdirty = true;
        }
        else if(k==skui::Key::kDown){
            ty=+spd;
            isdirty = true;
        }
        else if(k==skui::Key::kLeft){
            tx=-spd;
            isdirty = true;
        }
        else if(k==skui::Key::kRight){
            tx=+spd;
            isdirty = true;
        }
        PES5_Translate(tx, ty);
    }

    if(PES5_IsStitchEditInputStarted()){
        // if(state==skui::InputState::kUp){
        if (state == skui::InputState::kDown && isFirstPress) {
            if(k==skui::Key::kBack || k==skui::Key::kDelete){ // remove
                isdirty = PES5_StitchEditRemove();
            }
            else if (k == skui::Key::kI && isPlatformCtrlKey) {  // insert
                //SkDebugf("Ctrl or Command + I\n");
                isdirty = PES5_StitchEditInsert();
            }

            // SkDebugf("k=%i\n", k);
        }
    }
    
    if(isPlatformCtrlKey && state==skui::InputState::kDown){
        if(isShiftKey && k==skui::Key::kZ){
            PES5_Redo();
        }
        else if(k==skui::Key::kZ){
            PES5_Undo();
        }

        switch (k) {
            default:
                break;

            case skui::Key::kA:
                PES5_MSSelectAll();
                isHandled = true;
                break;

            case skui::Key::kH:
                PES5_MSTranslateToCenter();
                isHandled = true;
                break;
        }

    }

#ifdef __EMSCRIPTEN__
    if (isdirty || isHandled) {
        EM_ASM({ globalThis['isHandled'] = true; });
    }
#endif
    if (isdirty) {
        this->_ui->rootWidget->dirty(isdirty, false);
    }
    return DecUILayer::onKey(k, state, mod);
}

}
