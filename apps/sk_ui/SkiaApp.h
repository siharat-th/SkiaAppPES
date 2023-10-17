#ifndef __SKIA_APPLIB_H__
#define __SKIA_APPLIB_H__

#include <chrono>
#include "imgui.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkFont.h"
#include "include/core/SkTypes.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkString.h"
#include "include/core/SkSurface.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkBlendMode.h"
#include "include/effects/SkGradientShader.h"
#include "include/private/base/SkTArray.h"
#include "include/private/base/SkTDArray.h"
#include "include/private/base/SkTPin.h"
#include "include/effects/SkDashPathEffect.h"
#include "include/effects/SkColorMatrixFilter.h"
#include "tools/skui/ModifierKey.h"
#include "tools/skui/InputState.h"
#include "tools/skui/Key.h"
#include "tools/viewer/TouchGesture.h"
#include "SkiaAppLayer.h"

namespace sk_ui {

class SkiaAppLayer;
class SkiaAppConfig;
class SkiaAppLib;

enum SKIAAPP_MESSAGE_TYPE {
    kMouse,
};

class SkiaAppMessage {
public:
    SKIAAPP_MESSAGE_TYPE type;
    int x;
    int y;
    skui::InputState state;
    skui::ModifierKey modifiers;
};

class SkiaAppConfig {
public:
    SkiaAppConfig();
    virtual ~SkiaAppConfig();

    static SkiaAppConfig *pInstance;
    static SkiaAppConfig *getInstance(); 

    std::string appName;
    int expectedWidth;
    int expectedHeight;

    float build;
    std::string version;

    std::string ota;
};
extern SkiaAppConfig *APPCONFIG();

class SkiaAppLib {
public:
    SkiaAppLib();
    virtual ~SkiaAppLib() {
        //SkiaAppConfig* pSkiaAppConfig = APPCONFIG();
        //if (pSkiaAppConfig) {
        //    delete pSkiaAppConfig;
        //}
    }

    const long FLAG_FRONTEND_RUN = 1;

    void setFlags( long flags=0 );
    bool init();
    //void ota();

    bool otaRunning;
    bool otaDownloading;
    std::string otaURL;

    std::string AppName;
    std::string Version;

    static SkiaAppLib *pInstance;
    static SkiaAppLib *getInstance();

    //std::vector<SkiaAppMessage> msgQueue;

    static sk_sp<SkTypeface> defaultTypeface;
    static SkFont defaultFont;

    static sk_sp<SkTypeface> boldTypeface;
    static SkFont boldFont;

    static sk_sp<SkTypeface> monoTypeface;
    static SkFont monoFont;    

    static sk_sp<SkTypeface> iconTypeface;
    static SkFont iconFont;

    long frameCount;
    bool bMakeScreenShot;

    //std::chrono::high_resolution_clock::time_point beginTimeStamp;
    //std::chrono::high_resolution_clock::time_point lastTimeStamp;  
    double beginTimeStamp;
    double lastTimeStamp;
    
    unsigned long milliseconds;
    unsigned long frame_milliseconds;
    unsigned long getTick() { return milliseconds; }

    float *valueFrameTime;

    void setAppTitle( std::string title );

    void visitLayers(std::function<void(SkiaAppLayer*)> visitor);
    bool signalLayers(std::function<bool(SkiaAppLayer*)> visitor);

    void pushLayer(SkiaAppLayer* layer);    
    void onAppResize(int w, int h, float s);   

    void onAppFocus(bool f);

    void onAppInit(); 
    void onAppPrePaint(); 
    void onAppPaint(SkCanvas& canvas);  
    bool onAppChar(SkUnichar c, skui::ModifierKey modifiers);
    bool onAppKey(skui::Key key, skui::InputState state, skui::ModifierKey modifiers);
    bool onAppMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers);
    bool onAppMouseWheel(float deltax, float delta, skui::ModifierKey modifiers);
#ifdef __EMSCRIPTEN__
    bool onAppCharEm(SkUnichar c, uint8_t modifiers) { return onAppChar(c, static_cast<skui::ModifierKey>(modifiers)); }
    bool onAppKeyEm(skui::Key key, skui::InputState state, uint8_t modifiers) { 
        //uint8_t u8mod = (uint8_t)modifiers;

        //bool a5bits[5] = {
        //    (bool)(u8mod & 0x01),
        //    (bool)(u8mod & 0x02),
        //    (bool)(u8mod & 0x04),
        //    (bool)(u8mod & 0x08),
        //    (bool)(u8mod & 0x10)
        //};

        //SkDebugf("onAppKeyEm mod: %u%u%u%u%u\n",
        //            a5bits[4],
        //            a5bits[3],
        //            a5bits[2],
        //            a5bits[1],
        //            a5bits[0]);

        return onAppKey(key, state, static_cast<skui::ModifierKey>(modifiers)); 
    }
    bool onAppMouseEm(int x, int y, skui::InputState state, uint8_t modifiers) { 
        //uint8_t u8mod = (uint8_t)modifiers;

        //bool a5bits[5] = {
        //    (bool)(u8mod & 0x01),
        //    (bool)(u8mod & 0x02),
        //    (bool)(u8mod & 0x04),
        //    (bool)(u8mod & 0x08),
        //    (bool)(u8mod & 0x10)
        //};

        //SkDebugf("onAppMouseEm mod: %u%u%u%u%u\n",
        //            a5bits[4],
        //            a5bits[3],
        //            a5bits[2],
        //            a5bits[1],
        //            a5bits[0]);

        return onAppMouse(x, y, state, static_cast<skui::ModifierKey>(modifiers)); 
    }
    bool onAppMouseWheelEm(float deltax, float delta, uint8_t modifiers) { return onAppMouseWheel(deltax, delta, static_cast<skui::ModifierKey>(modifiers)); }
#endif
    bool onAppTouch(intptr_t owner, skui::InputState state, float x, float y);
    bool onAppDropFile(std::string dropped_file, uintptr_t /* uint8_t*  */ iptr = 0, size_t length = 0);
    //void setImage(long instance, sk_sp<SkImage> img);
    void js_callback(long instance, std::string message);
    void setConfig( long instance, std::string str );
    //void setMQTT( long instance, std::string str );

    void setDefaultTypeface(sk_sp<SkTypeface> tf);
    void setBoldTypeface(sk_sp<SkTypeface> tf);
    void setIconTypeface(sk_sp<SkTypeface> tf);
    void setMonoTypeface(sk_sp<SkTypeface> tf);
    void setTypefaceFromData(long instance, int fontid, sk_sp<SkData> data);

    void setWidgetFromData( long instance, sk_sp<SkData> data );
    void loadSVGFromData( long instance, sk_sp<SkData> data );
    int fontLoaded;
    long AppFlag;
protected:
    SkTDArray<SkiaAppLayer*>      fLayers;
    //TouchGesture           fGesture;
};



extern SkiaAppLib* SKIAAPP();

extern const char* getInputStateString(skui::InputState state);

}   // namespace sk_ui

#endif //__SKIA_APPLIB_H__
