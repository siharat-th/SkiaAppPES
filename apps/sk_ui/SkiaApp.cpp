#include <chrono>
#include<fstream>
#include<string>
#include "SkiaApp.h"
#include "../../apps2/DecUI/DecWidget.h"
#ifdef __USE_PES__
#include "Widgets/PES.h"
#endif
#include "include/core/SkImage.h"
#include "include/core/SkData.h"
#include "include/private/SkBitmaskEnum.h"
#include "include/core/SkEncodedImageFormat.h"
#include "tools/Resources.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif
#ifdef __USE_SDL__
#include "SDL.h"
#endif

#if defined(_MSC_VER)
#include <windows.h>
long long current_timestamp() {
    return GetTickCount64();
}
#else
#include <sys/time.h>

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL);                                          // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;  // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}
#endif
using namespace std;
using namespace sknonstd;

#ifdef __USE_SDL__
extern SDL_Window *window;
#endif

namespace sk_ui {

SkiaAppLib* SkiaAppLib::pInstance = NULL;
SkiaAppLib* SKIAAPP() {
    return SkiaAppLib::pInstance; }

const char* acstrInputStateString[] = {"KeyDown", "KeyUp", "MouseMove", "MouseRight", "MouseLeft"};
const char* getInputStateString(skui::InputState state) {
    return acstrInputStateString[(int)state];
}

const char* szDefaultTypeface = "fonts/Kanit-Light.ttf";
const char* szBoldTypeface = "fonts/Kanit-SemiBold.ttf";
const char* szMonoTypeface = "fonts/UbuntuMono-Regular.ttf";
const char* szIconTypeface = "fonts/Font Awesome 6 Pro-Light-300.otf";

sk_sp<SkTypeface> SkiaAppLib::defaultTypeface;
SkFont SkiaAppLib::defaultFont;

sk_sp<SkTypeface> SkiaAppLib::boldTypeface;
SkFont SkiaAppLib::boldFont;

sk_sp<SkTypeface> SkiaAppLib::monoTypeface;
SkFont SkiaAppLib::monoFont;

sk_sp<SkTypeface> SkiaAppLib::iconTypeface;
SkFont SkiaAppLib::iconFont;

SkiaAppLib* SkiaAppLib::getInstance() { return pInstance; }

SkiaAppConfig *SkiaAppConfig::pInstance = NULL;
SkiaAppConfig *SkiaAppConfig::getInstance() { return pInstance; }
SkiaAppConfig *APPCONFIG() { return SkiaAppConfig::getInstance(); }

SkiaAppConfig::SkiaAppConfig() {
    pInstance = this;
    appName = "SkiaApp";

#if (__USE_EDISON_SIMULATOR__ == 1)
     expectedWidth = 1280;
    expectedHeight = 720;
#else
    expectedWidth = 1280;
    expectedHeight = 960;
#endif    

    version = "0.0";
    build = 0;
}

SkiaAppConfig::~SkiaAppConfig() {

}

SkiaAppLib::SkiaAppLib() {
    SkiaAppLib::pInstance = this;

    AppName = "SkiaApp";
    Version = "0.0";

    AppFlag = 0;

    fontLoaded = 0;
    otaRunning = false;
    otaDownloading = false;
}

//void SkiaAppLib::ota() {
//    Downloader *dl = DOWNLOAD();
//    if( !dl ) return;
//    if( otaRunning ) return;
//
//    SkDebugf( "Running OTA\n" );
//    otaRunning = true;
//    dl->downloadFile( APPCONFIG()->ota, [=](DownloadTask *task) {
//        if( task->error ) {
//            SkDebugf( "Download ERROR\n" );
//            return;
//        }
//        SkDebugf( "JSON:\n%s\n", task->http_data );
//        json config = json::parse(task->http_data);
//        bool appNameValid = false;
//        bool newerBuild = false;
//        if (config.is_object()) {
//            for( auto item : config.items() ) {
//                std::string key = item.key();
//                if( key == "appName" ) {
//                    std::string an = item.value();
//                    if( an == APPCONFIG()->appName ) {
//                        appNameValid = true;
//                        SkDebugf( "AppName valid\n" );
//                    }
//                }
//                else if( key == "build" ) {
//                    int b = item.value();
//                    if( b > APPCONFIG()->build ) {
//                        newerBuild = true;
//                        SkDebugf( "Newer version available\n" );
//                    }
//                    else {
//                        SkDebugf( "No new version\n" );
//                    }
//                }
//                else if( key == "url" ) {
//                    otaURL = item.value();
//                }
//                else {
//                    SkDebugf("%s\n", key.c_str());
//                }
//            }
//        }
//        if( appNameValid && newerBuild ) { 
//            otaDownloading = false;
//            SkDebugf( "Perform UPDATE %s\n", otaURL.c_str() );
//            remove( "__OTA" );
//            Downloader *dl2 = DOWNLOAD();
//            dl2->downloadFile( otaURL, [=](DownloadTask *task) {
//                if( task->error ) {
//                    SkDebugf( "Download OTA file ERROR\n" );
//                    otaRunning = false;
//                    otaDownloading = false;
//                    return;
//                }
//                SkDebugf( "OTA download success %i\n", task->http_count );
//                //otaRunning = false;
//                otaDownloading = false;
//                sk_sp<SkData>otaFile = SkData::MakeFromMalloc( task->http_data, task->http_count );
//                FILE *fptr;
//                fptr = fopen("__OTA","wb");
//                fwrite( task->http_data, 1, task->http_count, fptr); 
//                fclose(fptr);
//            });
//        }
//        else {
//            otaRunning = false;
//            otaDownloading = false;
//        }
//    });
//}


void SkiaAppLib::setFlags( long flags ) {
    SKIAAPP()->AppFlag = flags;
    //SkDebugf( "SetFlags: %li\n", flags );
    //SkDebugf( "App Flags: %li\n", this->AppFlag );
    //SkDebugf( "App Flags: %li\n", SKIAAPP()->AppFlag );
    //SkDebugf( "%li %li\n", (long)this, (long)SKIAAPP() );
}


#ifdef __EMSCRIPTEN__

typedef struct {
    void *instance;
    int type;
    int id;
} DLINFO;

void emscriptenDownloadSucceeded(emscripten_fetch_t *fetch) {
    //printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];

    sk_sp<SkData> data = SkData::MakeWithCopy( fetch->data, fetch->numBytes );
    DLINFO *dinfo = (DLINFO *)fetch->userData;

    int itemid = dinfo->id;
    int type = dinfo->type;

    if( type == 0 ) // Typeface
    {
        SkiaAppLib *app = (SkiaAppLib *)dinfo->instance;
        sk_sp<SkTypeface> tf = SkTypeface::MakeFromData( data );
        if (itemid == 0) {
            app->setDefaultTypeface(tf);
        }
        else if (itemid == 1) {
            app->setIconTypeface(tf);
        }
        else if (itemid == 2) {
            app->setBoldTypeface(tf);
        }
        else if (itemid == 3) {
            app->setMonoTypeface(tf);
        }
        app->fontLoaded = app->fontLoaded | (1 << itemid);
        //SkDebugf( "FontLoaded %i %0X\n", itemid, app->fontLoaded );
        app->visitLayers([=](SkiaAppLayer* layer) { layer->onFontChange(); });    
    }

    emscripten_fetch_close(fetch); // Free data associated with the fetch.

}

void emscriptenDownloadFailed(emscripten_fetch_t *fetch) {
    //printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
    emscripten_fetch_close(fetch); // Also free data on failure.
}

void emscriptenDownloadAsset( std::string url, int type, int itemid, void *instance ) {
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);

    strcpy(attr.requestMethod, "GET");

    std::string full_url;
    if (url[0] == '/') {
        full_url = std::string(url);
    } else if (url.find("resources/") == 0) {
        // full_url = std::string("/") + url;
        full_url = std::string("") + url;
    } else {
        // full_url = std::string("/resources/") + url;
        full_url = GetResourcePath(url.c_str()).c_str();
    }

    DLINFO *dinfo = new DLINFO();
    dinfo->instance = instance;
    dinfo->id = itemid;
    dinfo->type = type;
    
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.userData = (void *)dinfo;
    attr.onsuccess = emscriptenDownloadSucceeded;
    attr.onerror = emscriptenDownloadFailed;
    emscripten_fetch(&attr, full_url.c_str());
    
}
#endif
 
bool SkiaAppLib::init() {
#ifndef __EMSCRIPTEN__
    defaultTypeface = MakeResourceAsTypeface(szDefaultTypeface);
    setDefaultTypeface(defaultTypeface);

    boldTypeface = MakeResourceAsTypeface(szBoldTypeface);
    setBoldTypeface(boldTypeface);

    iconTypeface = MakeResourceAsTypeface(szIconTypeface);
    setIconTypeface(iconTypeface);

    monoTypeface = MakeResourceAsTypeface(szMonoTypeface);
    setMonoTypeface(monoTypeface);

    fontLoaded = 0x0F;

#else

    emscriptenDownloadAsset( szDefaultTypeface, 0, 0, this );
    emscriptenDownloadAsset( szIconTypeface,    0, 1, this );
    emscriptenDownloadAsset( szBoldTypeface,    0, 2, this );
    emscriptenDownloadAsset( szMonoTypeface,    0, 3, this );

#endif
    //valueFrameTime = (float *)DATA()->getDataAddress( "FrameTime" );
    valueFrameTime = nullptr;

    frameCount = 0;
    milliseconds = 0;
    bMakeScreenShot = false;

    onAppInit();
    
    return true;
}

void SkiaAppLib::setAppTitle( std::string title ) {
#ifdef __USE_SDL__
    SDL_SetWindowTitle( window, title.c_str() );
#else
#endif
}

void SkiaAppLib::setWidgetFromData(long instance, sk_sp<SkData> data) {
    //SkDebugf("Got Widget data %li %i\n", instance, (int)data->size());
    SkString str((const char*)data->bytes(), data->size());

    //WidgetLayer* layer = (WidgetLayer*)instance;
    //layer->loadFromText(str.c_str());
}

void SkiaAppLib::visitLayers(std::function<void(SkiaAppLayer*)> visitor) {
    for (int i = 0; i < fLayers.size(); ++i) {
        if (fLayers[i]->fActive) {
            visitor(fLayers[i]);
        }
    }
}

bool SkiaAppLib::signalLayers(std::function<bool(SkiaAppLayer*)> visitor) {
    for (int i = fLayers.size() - 1; i >= 0; --i) {
        if (fLayers[i]->fActive && visitor(fLayers[i])) {
            return true;
        }
    }
    return false;
}

void SkiaAppLib::pushLayer(SkiaAppLayer* layer) { fLayers.push_back(layer); }

void SkiaAppLib::onAppResize(int w, int h, float s) {
    this->visitLayers([=](SkiaAppLayer* layer) {
        layer->onResize(w, h, s);
        //layer->fScale = s;
    });
}

void SkiaAppLib::onAppFocus(bool f) {
    this->visitLayers([=](SkiaAppLayer* layer) {
        layer->focus(f);
    });
}

void SkiaAppLib::onAppInit() {
    this->visitLayers([=](SkiaAppLayer* layer) {
        layer->onInit();
        //layer->fScale = s;
    });
}

void SkiaAppLib::onAppPrePaint() {
    if( fontLoaded != 0x0F ) { // waiting for all font loaded.
        return;
    }    
    //SkDebugf( "OnAppPrepaint\n" );
    //this->visitLayers([](SkiaAppLayer* layer) { layer->onUpdate(); });
    for (int i = 0; i < fLayers.size(); ++i) {
        fLayers[i]->onUpdate();
    }
    //if (msgQueue.size()) {
    //    auto m = msgQueue[0];
    //    this->signalLayers([=](SkiaAppLayer* layer) {
    //        return layer->onMouse(m.x / layer->fScaleFactor, m.y / layer->fScaleFactor, m.state, m.modifiers);
    //    });
    //    msgQueue.erase(msgQueue.begin());
    //}
}

void SkiaAppLib::onAppPaint(SkCanvas& canvas) {
    if( fontLoaded != 0x0F ) { // waiting for all font loaded.
        return;
    }
    //canvas.clear( SK_ColorTRANSPARENT);
    if( frameCount == 0 )
    {
        //beginTimeStamp = std::chrono::high_resolution_clock::now();
        beginTimeStamp = current_timestamp();
        lastTimeStamp = beginTimeStamp;
    }  

    //std::chrono::high_resolution_clock::time_point endTimeStamp = std::chrono::high_resolution_clock::now();
    double endTimeStamp = current_timestamp();
    //std::chrono::nanoseconds elapsed =
    //        std::chrono::duration_cast<std::chrono::nanoseconds>(endTimeStamp - beginTimeStamp);  
    double elapsed = endTimeStamp - beginTimeStamp;

    //auto frameElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(endTimeStamp - lastTimeStamp);
    double frameElapsed = endTimeStamp - lastTimeStamp;
    //frame_milliseconds = frameElapsed.count() * 1e-6;  
    frame_milliseconds = frameElapsed;

    if( valueFrameTime ) {
        *valueFrameTime = frame_milliseconds;
    }

    lastTimeStamp = endTimeStamp;

    //SkDebugf( "Frame time %0.6f\n", (float)frame_milliseconds );          

    //milliseconds += elapsed.count() * 1e-6;
    milliseconds += elapsed;
    beginTimeStamp = endTimeStamp;
    /*
    for (int i = 0; i < fLayers.count(); ++i) {
        fLayers[i]->onUpdate();
    }
     */

    for (int i = 0; i < fLayers.size(); ++i) {
        fLayers[i]->onPrePaint(canvas);        
        fLayers[i]->onPaint(canvas);
        fLayers[i]->onPostPaint(canvas);        
    }
    frameCount++;
    if( bMakeScreenShot ) {
        bMakeScreenShot = false;
        sk_sp<SkImage>img = canvas.getSurface()->makeImageSnapshot();
        int quality = 100;
        sk_sp<SkData>data = img->encodeToData( SkEncodedImageFormat::kPNG, quality );

        int i =0;
        char sz[64];
        while( true ) {
            sprintf( sz, "Screenshot-%04i.png", i );
            ifstream f( sz );
            if( !f )
                break;
            f.close();
            i++;
        }

        ofstream f(sz);
        if( f ) {
            f.write( (const char *)data->bytes(), data->size() );
            f.close();
        }
    }
}

bool SkiaAppLib::onAppChar(SkUnichar c, skui::ModifierKey modifiers) {
    //SkDebugf("SkiaAppLib::onAppChar:0x%04x\n", c);
    if( c == 's'  ) {
        if (Any(modifiers & skui::ModifierKey::kControl) && Any(modifiers & skui::ModifierKey::kOption)) {
            SkDebugf( "Make Screenshot\n" );
            bMakeScreenShot = true;
        }
    }
    return this->signalLayers([=](SkiaAppLayer* layer) { return layer->onChar(c, modifiers); });
} 

bool SkiaAppLib::onAppKey(skui::Key key, skui::InputState state, skui::ModifierKey modifiers) {
    //SkDebugf("SkiaAppLib::onAppKey:0x%04x\n", key);
    return this->signalLayers( [=](SkiaAppLayer* layer) { return layer->onKey(key, state, modifiers); });
}

bool SkiaAppLib::onAppMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers) {
    //SkiaAppMessage msg = {SKIAAPP_MESSAGE_TYPE::kMouse, x, y, state, modifiers};
    //if (msgQueue.size()) {
    //    SkiaAppMessage* lm = &msgQueue[msgQueue.size() - 1];
    //    if (lm->type == msg.type && lm->state == msg.state && lm->modifiers == msg.modifiers) {
    //        lm->x = msg.x;
    //        lm->y = msg.y;
    //    } else {
    //        msgQueue.push_back(msg);
    //    }
    //} else {
    //    msgQueue.push_back(msg);
    //}
    //return false;  
    return this->signalLayers([=](SkiaAppLayer* layer) {
        return layer->onMouse(x / layer->fScaleFactor, y / layer->fScaleFactor, state, modifiers);
    });
}

bool SkiaAppLib::onAppMouseWheel(float deltax, float delta, skui::ModifierKey modifiers) {
    return this->signalLayers(
            [=](SkiaAppLayer* layer) { return layer->onMouseWheel(deltax, delta, modifiers); });
}

bool SkiaAppLib::onAppTouch(intptr_t owner, skui::InputState state, float x, float y) {
    // void* castedOwner = reinterpret_cast<void*>(owner);
    switch (state) {
        case skui::InputState::kUp: {
            // fGesture.touchEnd(castedOwner);
            this->signalLayers([=](SkiaAppLayer* layer) {
                return layer->onMouse(x / layer->fScaleFactor,
                                      y / layer->fScaleFactor,
                                      skui::InputState::kUp,
                                      skui::ModifierKey::kNone);
            });
            break;
        }
        case skui::InputState::kDown: {
            // fGesture.touchBegin(castedOwner, x, y);
            this->signalLayers([=](SkiaAppLayer* layer) {
                return layer->onMouse(x / layer->fScaleFactor,
                                      y / layer->fScaleFactor,
                                      skui::InputState::kDown,
                                      skui::ModifierKey::kNone);
            });
            break;
        }
        case skui::InputState::kMove: {
            // fGesture.touchMoved(castedOwner, x, y);
            this->signalLayers([=](SkiaAppLayer* layer) {
                return layer->onMouse(x / layer->fScaleFactor,
                                      y / layer->fScaleFactor,
                                      skui::InputState::kMove,
                                      skui::ModifierKey::kNone);
            });
            break;
        }
        default: {
            // kLeft and kRight are only for swipes
            SkASSERT(false);
            break;
        }
    }

    return this->signalLayers(
            [=](SkiaAppLayer* layer) { return layer->onTouch(owner, state, x, y); });
}

bool SkiaAppLib::onAppDropFile(std::string dropped_file, uintptr_t /* uint8_t*  */ iptr, size_t length) { 
    return this->signalLayers([=](SkiaAppLayer* layer) { return layer->onDropFile(dropped_file.c_str(), iptr, length); });
}

//void SkiaAppLib::setImage(long instance, sk_sp<SkImage> img) {
//    //SkDebugf("Got image %li %i %i\n", instance, (int)img->width(), (int)img->height());
//    //Image* imgWidget = (Image*)instance;
//    //imgWidget->setImage(img);
//}

void SkiaAppLib::js_callback(long instance, std::string message ) {
    //SkDebugf("Got callback %li %s\n", instance, message.c_str() );
    DecWidget *widget = (DecWidget *)instance;
    widget->js_callback( message );
}

void SkiaAppLib::setConfig( long instance, std::string str ) {
    SkDebugf( "SkiaAppLib::setConfig\n" );
    SkDebugf( "%s\n", str.c_str() );
    //WidgetLayer *layer = (WidgetLayer *)instance;
    //layer->setConfig( str.c_str() );
}

//void SkiaAppLib::setMQTT( long instance, std::string mqtt_string ) {
//    SkDebugf( "SkiaAppLib::setMQTT\n" );
//    SkDebugf( "%s\n", mqtt_string.c_str() );
//    //WidgetMQTT *mqtt = (WidgetMQTT *)instance;
//    //mqtt->SetMQTT(mqtt_string.c_str());
//}

void SkiaAppLib::setDefaultTypeface(sk_sp<SkTypeface> tf) {
    SkString n;
    tf->getFamilyName(&n);
    //SkDebugf("Got typeface %s\n", n.c_str());

    //SkDebugf("Update font\n");
    defaultFont.setTypeface(tf);
    defaultFont.setSize(20);    

}

void SkiaAppLib::setBoldTypeface(sk_sp<SkTypeface> tf) {
    SkString n;
    tf->getFamilyName(&n);
    //SkDebugf("Got typeface %s\n", n.c_str());

    //SkDebugf("Update font\n");
    boldFont.setTypeface(tf);
    boldFont.setSize(20);
}

void SkiaAppLib::setIconTypeface(sk_sp<SkTypeface> tf) {
    iconFont.setTypeface(tf);
    iconFont.setSize(20);
}

void SkiaAppLib::setMonoTypeface(sk_sp<SkTypeface> tf) {
    monoFont.setTypeface(tf);
    monoFont.setSize(20);
}


void SkiaAppLib::setTypefaceFromData(long instance, int fontid, sk_sp<SkData> data) {
    SkDebugf("Got Typeface data %li %i\n", instance, (int)data->size());
    sk_sp<SkTypeface> tf = SkTypeface::MakeFromData(data);
    SkiaAppLib* app = (SkiaAppLib*)instance;
    if (fontid == 0) {
        app->setDefaultTypeface(tf);
    }
    else if (fontid == 1) {
        app->setIconTypeface(tf);
    }
    else if (fontid == 2) {
        app->setBoldTypeface(tf);
    }
    else if (fontid == 3) {
        app->setMonoTypeface(tf);
    }
    app->fontLoaded = app->fontLoaded | (1 << fontid);
    SkDebugf( "FontLoaded %0X\n", app->fontLoaded );    
    this->visitLayers([=](SkiaAppLayer* layer) { layer->onFontChange(); });
}

}  // namespace sk_ui
