//
// Created by Jimmy on 2/9/2016 AD.
//

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#ifndef __EMSCRIPTEN__
#include <SDL_net.h>
#endif

#include "SDL_gui.h"
#include <iostream>
#include <stdio.h>  /* defines FILENAME_MAX */

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// pom
#include <thread> // for std::this_thread::yield()

// require sdl2_gfx
#include "SDL2_gfxPrimitives.h"

#include <list>

#if __IPHONEOS__
extern "C" void GUI_iOS_Init();
#endif

int testCount = 0;
int lastCount = 0;

//extern sk_sp<SkImage> exImage;

static int GUI_initialized = 0;

const Uint32 GUI_EventID = SDL_USEREVENT;
const Uint32 GUI_EventPaint = GUI_EventID + 1;
const Uint32 GUI_EventListSelectd = GUI_EventID + 2;
const Uint32 GUI_EventInitDialog = GUI_EventID + 3;
const Uint32 GUI_EventUser = GUI_EventID + 300;

std::vector<GUI_UserMessage> user_message_queue;

//const bool USE_D3D = false;
SDL_Window* GUI_window = NULL;
SDL_Renderer *GUI_renderer = NULL;

SDL_GLContext GUI_glContext = nullptr;
#if __USE_SKIA__
SkCanvas *GUI_canvas = NULL;
#endif

GUI_View *GUI_modalWindow = NULL;
bool GUI_AutoHideModal = true;
bool GUI_AutoCloseModal = true;

int GUI_physicalWindowWidth = 0;
int GUI_physicalWindowHeight = 0;

int GUI_windowWidth = 0;
int GUI_windowHeight = 0;

int GUI_ExpectedWindowWidth = 0;
int GUI_ExpectedWindowHeight = 0;

int GUI_scale = 1;
float GUI_mouseScale = 1.0;

bool GUI_running = false; // set true by GUI_Run()
bool bFirtPainted = false;
static bool quit = false;

bool GUI_TakeScreenShot = false;
std::string GUI_ScreenShotPath = "";

std::function<bool(SDL_Event* ev)> user_handle_events;

static const int MILLESECONDS_PER_FRAME = 1000 / 60; // about 60 frames per second

static void handle_events(SDL_Event *ev);

GUI_View *GUI_TopView = NULL;
GUI_Label *GUI_LabelFPS = NULL;

Uint32 timer_start = 0;
Uint32 frame_count = 0;
Uint32 frameTimeMillis = 0; // pom

#if defined(WIN32)
HWND hwnd = NULL;
#endif

bool bGUI_FPS = false;

#ifdef _OF_EVENTS_
std::shared_ptr<ofCoreEvents> __ofEvents = std::make_shared<ofCoreEvents>();


ofCoreEvents & ofEvents() {
    return *__ofEvents.get();
}
#endif

std::list< GUI_Downloader * > functionsHeartbeat;

void GUI_AddHeartbeat(GUI_Downloader * fn) {
    functionsHeartbeat.push_back(fn);
}

void GUI_RemoveHeartbeat(GUI_Downloader * fn) {
    functionsHeartbeat.remove(fn);
}

int SDL_GUI_Init(void) {
    /* initialize SDL */
#ifdef PES_CLI
    if (SDL_Init(SDL_INIT_TIMER) < 0) {
        GUI_Log("Could not initialize SDL");
        return 1;
    }
#else
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        GUI_Log("Could not initialize SDL");
        return 1;
    }
#endif

    if (IMG_Init(0) != 0) {
        GUI_Log("IMG_Init failed.");
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() != 0) {
        GUI_Log("TTF_Init failed.");
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

#ifndef __EMSCRIPTEN__
    if (SDLNet_Init() != 0) {
        GUI_Log("SDLNet_Init failed.");
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
#endif

    if (GUI_Init() != 0) {
        GUI_Log("GUI_Init failed.");
#ifndef __EMSCRIPTEN__
        SDLNet_Quit();
#endif
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

#ifdef _RPi
    SDL_ShowCursor(SDL_DISABLE);
#endif    

    return 0; // all success
}

void SDL_GUI_Exit(void) {
    GUI_Log("SDL_GUI Exit\n");
#ifndef __EMSCRIPTEN__
    SDLNet_Quit();
#endif
    GUI_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

GUI_Rect *GUI_MakeRect(int x, int y, int w, int h) { // creates pointer to Rect. Each source file gets its own instance.
    static GUI_Rect rp_rect;
    rp_rect.set(x, y, w, h);
    return &rp_rect;
}

int GUI_Init(void) {
    int status = 0;
#if __IPHONEOS__
    GUI_iOS_Init();
#endif

    if (!GUI_initialized) {
        uint32_t ev = SDL_RegisterEvents(1024);
        if (ev != SDL_USEREVENT) {
            GUI_Log("User event ERROR.\n");
            exit(1);
        }
    }

    if (status == 0) {
        ++GUI_initialized;
    }

#ifndef TARGET_CLI
    GUI_LabelFPS = new GUI_Label(NULL, "");
    GUI_LabelFPS->titleColor = cWhite;
    GUI_LabelFPS->bgcol = sdl_color(0x0000004F);
#endif

    return status;
}



void GUI_Quit(void) {
    //GUI_Log( "Quit now\n" );
    if (GUI_TopView) {
        delete GUI_TopView;
        GUI_TopView = NULL;
    }
    if (GUI_renderer) {
        SDL_DestroyRenderer(GUI_renderer);
        GUI_renderer = NULL;
    }

    if (GUI_window) {
        SDL_DestroyWindow(GUI_window);
        GUI_window = NULL;
    }

}

//int GUI_Skia_Init(int & dw, int & dh, GrGLFramebufferInfo & info){
//    if(!GUI_window)
//        return -1;
//    
//    if(!GUI_renderer)
//        return -1;
//    
//    int kStencilBits = 8;  // Skia needs 8 stencil bits
//    int kMsaaSampleCount = 0; //4;
//    
//    SDL_RenderSetViewport(GUI_renderer, NULL);
//    SDL_RenderSetClipRect(GUI_renderer, NULL);
//
//    SDL_SetRenderDrawColor(GUI_renderer, 0x64, 0x95, 0xed, 0xff);
//    SDL_RenderClear(GUI_renderer);
//    
//    GUI_glContext = SDL_GL_CreateContext(GUI_window);
//    SDL_GL_MakeCurrent(GUI_window, GUI_glContext);
//    uint32_t windowFormat = SDL_GetWindowPixelFormat(GUI_window);
//    int contextType;
//    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);
////    int dw, dh;
//    SDL_GL_GetDrawableSize(GUI_window, &dw, &dh);
//    auto interface = GrGLMakeNativeInterface();
//    SkASSERT(interface);
//    sk_sp<GrDirectContext> grContext(GrDirectContext::MakeGL(interface));
//
//    skutil sku;
//    GrGLint buffer;
//    sku.GrGLGetFrameBuffer(interface, &buffer);
////    GrGLFramebufferInfo info;
//    info.fFBOID = (GrGLuint)buffer;
//    SkColorType colorType;
//    if (SDL_PIXELFORMAT_RGBA8888 == windowFormat)
//    {
//        info.fFormat = GR_GL_RGBA8;
//        colorType = kRGBA_8888_SkColorType;
//    }
//    else
//    {
//        colorType = kBGRA_8888_SkColorType;
//        if (SDL_GL_CONTEXT_PROFILE_ES == contextType)
//        {
//            info.fFormat = GR_GL_BGRA8;
//        }
//        else
//        {
//            // We assume the internal format is RGBA8 on desktop GL
//            info.fFormat = GR_GL_RGBA8;
//        }
//    }
////    GrBackendRenderTarget target(dw, dh, kMsaaSampleCount, kStencilBits, info);
////    SkSurfaceProps props;
////    sk_sp<SkSurface> surface(SkSurface::MakeFromBackendRenderTarget(grContext.get(), target,
////                                                                    kBottomLeft_GrSurfaceOrigin,
////                                                                    colorType, nullptr, &props));
////    if(!surface)
////        return -1;
////
////    GUI_canvas = surface->getCanvas();
//    return 0;
//}
//
//void GUI_Skia_Quit(void){
//    if(GUI_glContext){
//        SDL_GL_DeleteContext(GUI_glContext);
//    }
//}

void GUI_SetScaleParameters(void) {
    int gx, gy;
    SDL_GetWindowSize(GUI_window, &gx, &gy);
    //GUI_Log("Intend size: %i %i\n", gx, gy);

    GUI_windowWidth = gx;
    GUI_windowHeight = gy;

    SDL_GL_GetDrawableSize(GUI_window, &GUI_physicalWindowWidth, &GUI_physicalWindowHeight);
    //GUI_Log("Drawable: %i %i\n", GUI_physicalWindowWidth, GUI_physicalWindowHeight);
    if( GUI_physicalWindowHeight < gy ) {
        gy = GUI_physicalWindowHeight;
    }
    if( GUI_physicalWindowWidth < gx ) {
        gx = GUI_physicalWindowWidth;
    }
    if( gx != GUI_windowWidth || gy != GUI_windowHeight ) {
        SDL_SetWindowSize( GUI_window, gx, gy );
        GUI_windowWidth = gx;
        GUI_windowHeight = gy;
    }
    GUI_mouseScale = (float)GUI_physicalWindowWidth / (float)gx;
    //GUI_Log("Display0: %d %d\n", GUI_windowWidth, GUI_windowHeight);

#ifdef __ANDROID__
    if ((GUI_ExpectedWindowWidth > GUI_ExpectedWindowHeight && GUI_physicalWindowWidth < GUI_physicalWindowHeight) ||
        (GUI_ExpectedWindowWidth < GUI_ExpectedWindowHeight && GUI_physicalWindowWidth > GUI_physicalWindowHeight)) {
        int n = GUI_ExpectedWindowWidth;
        GUI_ExpectedWindowWidth = GUI_ExpectedWindowHeight;
        GUI_ExpectedWindowHeight = n;
        GUI_Log("Swap expected size\n");
    }

    int scalex = GUI_physicalWindowWidth / GUI_ExpectedWindowWidth;
    int scaley = GUI_physicalWindowHeight / GUI_ExpectedWindowHeight;

    if (GUI_physicalWindowWidth >= 1920) {
        scalex = 2;
        scaley = 2;
    }

    GUI_Log("Android scale: %i %i\n", scalex, scaley);
#else
    int scalex = GUI_physicalWindowWidth / GUI_windowWidth;
    int scaley = GUI_physicalWindowHeight / GUI_windowHeight;
#endif

    GUI_scale = (scalex < scaley) ? scaley : scalex;
    if( GUI_scale > 1 && GUI_windowHeight < 540 ) {
        GUI_scale--;
        GUI_windowWidth = GUI_physicalWindowWidth / GUI_scale;
        GUI_windowHeight = GUI_physicalWindowHeight / GUI_scale;
    }

    if (GUI_scale < 1)
        GUI_scale = 1;

#ifdef __ANDROID__
    GUI_windowWidth = (GUI_physicalWindowWidth) / GUI_scale;
    GUI_windowHeight = (GUI_physicalWindowHeight) / GUI_scale;
#endif

    //GUI_Log("Display: %d %d\n", GUI_windowWidth, GUI_windowHeight);
    //GUI_Log("Scale: %i, %0.2f\n", GUI_scale, GUI_mouseScale);
}

SDL_Window* GUI_CreateWindow(const char *title, int sx, int sy, bool HARDWARE_ACCELERATED, int x, int y, bool maximized) {
    if (GUI_window) {
        GUI_Log("GUI_window existed.");
        return GUI_window;
    }

    GUI_ExpectedWindowWidth = sx;
    GUI_ExpectedWindowHeight = sy;

#if defined(__IPHONEOS__) || defined(__ANDROID__)
    if (sx == 0 || sy == 0) {
        sx = 720;
        sy = 480;
    }

    int landscape = 0;
    if (sx > sy)
        landscape = 1;

    int modes = SDL_GetNumDisplayModes(0);
    sx = 0;
    sy = 0;
    GUI_Log("  Modes available: %i\n", modes);

    for (int i = 0; i < modes; i++) {
        SDL_DisplayMode mode;
        SDL_GetDisplayMode(0, i, &mode);
        GUI_Log("    :Mode #%i: %i, %i\n", i + 1, mode.w, mode.h);

        if (landscape ? mode.w > sx : mode.h > sy) {
            sx = mode.w;
            sy = mode.h;
        }
    }
#else
    if (sx == 0 || sy == 0) {
        sx = 720;
        sy = 480;
    }
#endif
    if ((GUI_ExpectedWindowWidth > GUI_ExpectedWindowHeight && sx < sy) ||
        (GUI_ExpectedWindowWidth < GUI_ExpectedWindowHeight && sx > sy)) {
        int n = GUI_ExpectedWindowWidth;
        GUI_ExpectedWindowWidth = GUI_ExpectedWindowHeight;
        GUI_ExpectedWindowHeight = n;
    }

    // Get Sccreen size
    SDL_DisplayMode dm;

    if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
        SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        return NULL;
    }

#if defined(WIN32)
    int style = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | (maximized ? SDL_WINDOW_MAXIMIZED : 0);
#elif defined(__MACOSX__)
    int style = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | (maximized ? 0 : 0); // MacOS bug in Seira - no maximize please.
//    int style = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | (maximized ? 0 : 0); // For test non-ratina display
    // style = style | SDL_WINDOW_BORDERLESS;
#elif defined(__IPHONEOS__)
    int style = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS;
#elif defined(__ANDROID__)
    int style = SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_RENDERER_ACCELERATED | SDL_WINDOW_FULLSCREEN;
#elif defined(_RPi)
    int style = SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    GUI_Log("RPI--------------------------\n");
#elif defined(__EMSCRIPTEN__)
    int style = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
#else
    int style = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
#endif
    int gx = 0;
    int gy = 0;
    
    while( gx == 0 || gy == 0 ) {
        //Now create a window with title "SDL" at 0, 0 on the screen with w:800 h:600 and show it
    #ifdef __EMSCRIPTEN__
        GUI_window = SDL_CreateWindow(title, 0, 0, sx, sy, style);
    #else
        if (x == -1 || y == -1) {
            GUI_window = SDL_CreateWindow(title, (dm.w - sx) / 2, (dm.h - sy) / 2, sx, sy, style);
        } else {
            GUI_window = SDL_CreateWindow(title, x, y, sx, sy, style);
        }
    #endif

        if (GUI_window == NULL) {
            GUI_Log("GUI_CreateRenderer Error");
            return NULL;
        }



        SDL_Delay( 500 );
        SDL_GetWindowSize(GUI_window, &gx, &gy);
        if( gx != 0 && gy != 0 )
            break;
    
        SDL_DestroyWindow( GUI_window );
        GUI_Log( "Retry create window\n" );
    }
#if defined(WIN32)
    // Tooh - get window handle
    SDL_SysWMinfo info = {0};
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(GUI_window, &info);
    hwnd = info.info.win.window;
#endif

    GUI_windowWidth = gx;
    GUI_windowHeight = gy;

    // force render driver = opengl
    // - direct3d (default on windows)
    // - opengl
    // - opengles2
#if defined(WIN32)
    //if (USE_D3D) {
    //    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");
    //} else {
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    //}
//#elif defined(__aarch64__) && (!defined(__APPLE__) || !TARGET_IPHONE_SIMULATOR)
//    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
#endif

    //Create a renderer that will draw to the window, -1 specifies that we want to load whichever
    //video driver supports the flags we're passing
    //Flags: SDL_RENDERER_ACCELERATED: We want to use hardware accelerated rendering
    //SDL_RENDERER_PRESENTVSYNC: We want the renderer's present function (update screen) to be
    //synchornized with the monitor's refresh rate
    Uint32 flags = HARDWARE_ACCELERATED ? SDL_RENDERER_ACCELERATED : 0;
#if defined(WIN32) || defined(__linux__)
    flags = flags | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE;
#endif		
    GUI_renderer = SDL_CreateRenderer(GUI_window, -1, flags);
    if (GUI_renderer == NULL) {
        GUI_Log("GUI_CreateRenderer Error");
        GUI_Quit();
        return NULL;
    }

    SDL_RendererInfo rendererinfo;
    SDL_GetRendererInfo(GUI_renderer, &rendererinfo);
    std::cout << "SDL_RENDER_DRIVER selected: " << rendererinfo.name << std::endl;
    if (rendererinfo.flags & SDL_RENDERER_SOFTWARE)
        std::cout << " -the renderer is a software fallback" << std::endl;
    if (rendererinfo.flags & SDL_RENDERER_ACCELERATED)
        std::cout << " -the renderer uses hardware acceleration" << std::endl;
    if (rendererinfo.flags & SDL_RENDERER_PRESENTVSYNC)
        std::cout << " -present is synchronized with the refresh rate" << std::endl;
    if (rendererinfo.flags & SDL_RENDERER_TARGETTEXTURE)
        std::cout << " -the renderer supports rendering to texture" << std::endl;
    cout << "Texture formats: " << endl;
    for (Uint32 i = 0; i < rendererinfo.num_texture_formats; i++) {
        cout << SDL_GetPixelFormatName(rendererinfo.texture_formats[i]) << endl;
    }
    cout << endl;

    GUI_SetScaleParameters();

    // Select the color for drawing. It is set to red here.
    SDL_SetRenderDrawColor(GUI_renderer, 255, 255, 255, 255);
    SDL_RenderClear(GUI_renderer);
#if defined(WIN32)    
    SDL_Texture* texture = IMG_LoadTexture(GUI_renderer, ".\\data\\logo.png");
#else
    SDL_Texture* texture = IMG_LoadTexture(GUI_renderer, "data/logo.png");
#endif

    if (texture == NULL) {
#if defined(WIN32)    
        printf("Load image %s failed.\n", ".\\data\\logo.png");
#else
        printf("Load image %s failed.\n", "data/logo.png");
#endif
    } else {
        SDL_Rect dst;
        SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
        dst.w *= GUI_scale;
        dst.h *= GUI_scale;
        dst.x = (GUI_physicalWindowWidth - dst.w) / 2;
        dst.y = (GUI_physicalWindowHeight - dst.h) / 2;
        SDL_RenderCopy(GUI_renderer, texture, NULL, &dst);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(GUI_renderer);

    if (!GUI_CreateFont()) {
        GUI_Log("ERROR: Could not load fonts\n");
        exit(1);
    }

    return GUI_window;
}

#if defined(WIN32)
void GUI_SetWindowIcon(int icon_id) {
    if (GUI_window) {
        HICON hicon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(icon_id));
        SDL_SysWMinfo info = {0};
        SDL_VERSION(&info.version);
        SDL_GetWindowWMInfo(GUI_window, &info);
        HWND hwnd = info.info.win.window;
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hicon);
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hicon);
    }
}
#endif

SDL_Window *GUI_CreateLandscapeWindow(std::string title, int w, int h, int x, int y, bool maximized, bool HARDWARE_ACCELERATED) {
#if defined(__IPHONEOS__) || defined(__ANDROID__)
    int sx = 720, sy = 480;
#else
    int sx = 1024, sy = 768;
#endif

    if (w != 0)
        sx = w;

    if (h != 0)
        sy = h;

    // Android Test
    //sx = 960;
    //sy = 540;
    SDL_Window *window = GUI_CreateWindow(title.c_str(), sx, sy, HARDWARE_ACCELERATED, x, y, maximized);

    if (window == NULL) {
        GUI_Log("GUI_CreateWindow Error");
        return NULL;
    }

    return window;
}

SDL_Window *GUI_CreatePortraitWindow(std::string title, int w, int h, int x, int y, bool maximized) {
#if defined(__IPHONEOS__) || defined(__ANDROID__)
    int sx = 480, sy = 720;
#else
    int sx = 768, sy = 1024;
#endif

    if (w != 0)
        sx = w;

    if (h != 0)
        sy = h;

    SDL_Window *window = GUI_CreateWindow(title.c_str(), sx, sy, false, x, y, maximized);

    if (window == NULL) {
        GUI_Log("GUI_CreateWindow Error");
        return NULL;
    }

    return window;
}

bool isGUIRunning() {
    return GUI_running && bFirtPainted;
}

void GUI_Run(std::function<bool(SDL_Event* ev)> user_handle_ev, bool bFPS) {
    user_handle_events = user_handle_ev;

    GUI_running = true;

    Uint32 startFrame;
    Uint32 endFrame;
    int delay;

    startFrame = SDL_GetTicks();

    timer_start = SDL_GetTicks();
    frame_count = 0;
    bGUI_FPS = bFPS;

#ifdef __EMSCRIPTEN__
    printf("Set main loop\n");

    // -fno-exceptions >>>
    //try {
    //    emscripten_set_main_loop(doGuiLoop, 0, 1);
    //} catch (std::exception& e) {
    //    std::cout << e.what() << '\n';
    //}
    //
    emscripten_set_main_loop(doGuiLoop, 0, 1);
    // -fno-exceptions <<<

    printf("Main loop OK\n");
#else

    quit = false;

    while (!quit) {
        startFrame = SDL_GetTicks();
        doGuiLoop();



        endFrame = SDL_GetTicks();

        // figure out how much time we have left, and then sleep
        Uint32 frameTime = (endFrame - startFrame);

        if (frameTime <= 0) {
            frameTime = 1;
        }

        frameTimeMillis = frameTime;
        delay = MILLESECONDS_PER_FRAME - frameTime;

        if (delay > 0) {
            SDL_Delay(delay);
        }

        startFrame = endFrame;
    }
#endif

    GUI_running = false;
}

// pom
Uint32 getFrameTime() {
    if (frameTimeMillis < MILLESECONDS_PER_FRAME)
        frameTimeMillis = MILLESECONDS_PER_FRAME;

    return frameTimeMillis;
}

void doGuiLoop() {
    int n = (int)functionsHeartbeat.size();
    
    if (n > 0) {
        GUI_Downloader *dl = NULL;
        std::list<GUI_Downloader *>::const_iterator iterator;
        
        for (iterator = functionsHeartbeat.begin(); iterator != functionsHeartbeat.end(); ++iterator) {
            if ((*iterator)->pump() == false) {
                dl = (*iterator);
                functionsHeartbeat.remove(*iterator++);
                break;
            }
        }
        
        if (dl) {
            delete dl;
        }
    }
    
    testCount = 0;
    SDL_Event ev;
    
    GUI_SetViewport(GUI_Rect(0, 0, GUI_physicalWindowWidth, GUI_physicalWindowHeight));
    GUI_SetClipRect(GUI_Rect(0, 0, GUI_physicalWindowWidth, GUI_physicalWindowHeight));
    
    GUI_Clear(sdl_color(0x6495edff));
//    GUI_canvas->drawImage(exImage, 20, 20);

    //NooM
    if (user_message_queue.size() > 0) {
        GUI_UserMessage user_msg = user_message_queue.front();

        SDL_UserEvent user_event;
        user_event.type = user_msg.message_id;
        user_event.windowID = user_msg.parameter1;
        user_event.code = user_msg.parameter2;
        user_event.data1 = user_msg.extra_parameter1;
        user_event.data2 = user_msg.extra_parameter2;

        handle_events((SDL_Event *)&user_event);

        user_message_queue.erase(user_message_queue.begin());
    }

    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
            case SDL_QUIT:
                quit = true;
                break;

// noom
// fixed resize: solution2
// on windows in normal events SDL_WINDOWEVENT_SIZE_CHANGED will be fire before SDL_WINDOWEVENT_RESIZED, 
// but when turn back from fullscreen state to normal window SDL_WINDOWEVENT_RESIZED came before SDL_WINDOWEVENT_SIZE_CHANGED.
// 
// see: main.cpp -> void resize(int w, int h) 
// function resize() must knows last window-size retrieved from GUI_windowHeight and GUI_windowWidth to relayout some GUIs. 
#if defined(WIN32) || defined(__MACOSX__) || defined(__linux__)
//nothing
#else
            case SDL_WINDOWEVENT:
                switch (ev.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        printf("Event: Window Resized: %i, %i\n", ev.window.data1, ev.window.data2);
                        GUI_windowWidth = ev.window.data1;
                        GUI_windowHeight = ev.window.data2;
                        SDL_GL_GetDrawableSize(GUI_window, &GUI_physicalWindowWidth, &GUI_physicalWindowHeight);
                        GUI_Log("drawable: %d %d\n", GUI_physicalWindowWidth, GUI_physicalWindowHeight);

                        GUI_SetScaleParameters();
                        break;
                }
                break;
#endif
            case SDL_KEYDOWN:
#if defined(__linux__)
                // https://stackoverflow.com/questions/44664331/sdl-keydown-triggering-twice
                // Check if e->key.repeat equals to 0 before handling the events.
                // SDL generates fake repeated keypresses if you hold a key long enough.
                // This is used mostly for text input. The original key press has .repeat == 0, 
                // and fake presses have .repeat == 1.
                if( ev.key.repeat )
                    continue;
#endif            
                if (ev.key.keysym.sym == SDLK_AC_BACK) {
                    quit = true;
                    break;
                }
                break;
        }

        handle_events(&ev);
    }

    SDL_Event event;
    event.type = GUI_EventPaint;
    handle_events(&event);

    frame_count++;
    Uint32 duration = SDL_GetTicks() - timer_start;

    if (bGUI_FPS)
    {
        if (frame_count % 10 == 0) {
            float fps = (float)frame_count / duration * 1000.0f;
            char fps_text[32];
            sprintf(fps_text, "FPS: %0.2f", fps);
            GUI_LabelFPS->setTitle(fps_text);
        }

        GUI_LabelFPS->draw();
    }

    lastCount = testCount;

    if (GUI_TakeScreenShot) {
        SDL_Surface* pScreenShot = SDL_CreateRGBSurface(0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

        if (pScreenShot) {
            SDL_RenderSetViewport(GUI_renderer, NULL);
            SDL_RenderSetClipRect(GUI_renderer, NULL);
            // Read the pixels from the current render target and save them onto the surface
            SDL_RenderReadPixels(GUI_renderer, NULL, SDL_GetWindowPixelFormat(GUI_window), pScreenShot->pixels, pScreenShot->pitch);

            // Create the bmp screenshot file
            std::string s = GUI_ScreenShotPath;

            if (GUI_ScreenShotPath == "")
                s = "/Users/Jim/Desktop/Screenshot.png";

            IMG_SavePNG(pScreenShot, s.c_str());

            // Destroy the screenshot surface
            SDL_FreeSurface(pScreenShot);
        }

        GUI_TakeScreenShot = false;
    }

//    SDL_RenderPresent(GUI_renderer);
    GUI_RenderPresent();
}

void GUI_SetModal(GUI_View *view, bool bAutoHide) {
    if (GUI_modalWindow != view) {
        GUI_modalWindow = view;
    }

    GUI_AutoHideModal = bAutoHide;
    GUI_AutoCloseModal = bAutoHide;
}

bool GUI_IsModal(GUI_View *view) {
    return(GUI_modalWindow == view);
}

void GUI_SetModalAutoClose(bool bAuto) {
    GUI_AutoHideModal = bAuto;
    GUI_AutoCloseModal = bAuto;
}

void GUI_SetModalAutoHide(bool bAuto) {
    GUI_AutoHideModal = bAuto;
    GUI_AutoCloseModal = false;
}

static void handle_events(SDL_Event *ev) {
    if (user_handle_events) {
        if (user_handle_events(ev))
            return;
    }
#ifdef _RPi    
    int x, y;
    SDL_TouchFingerEvent e;
#endif

    if (GUI_mouseCapturedWindow) {
        switch (ev->type) {
#ifdef _RPi
            case SDL_FINGERDOWN:
#endif            
            case SDL_MOUSEBUTTONDOWN:
                if (GUI_mouseCapturedWindow->handleEvents(ev))
                    return;
                break;
#ifdef _RPi
            case SDL_FINGERMOTION:
                e = ev->tfinger;
                x = (int)(e.x*GUI_windowWidth / GUI_mouseScale);
                y = (int)(e.y*GUI_windowHeight / GUI_mouseScale);
                GUI_Log("SDL_FINGERMOTION: %i %i\n", x, y);
#endif            
            case SDL_MOUSEMOTION:
                GUI_mouseCapturedWindow->handleEvents(ev);
                return;
#ifdef _RPi
            case SDL_FINGERUP:
                e = ev->tfinger;
                x = (int)(e.x*GUI_windowWidth / GUI_mouseScale);
                y = (int)(e.y*GUI_windowHeight / GUI_mouseScale);
                GUI_Log("SDL_FINGERUP: %i %i\n", x, y);
#endif            
            case SDL_MOUSEBUTTONUP:
                GUI_mouseCapturedWindow->handleEvents(ev);
                return;
        }
    }
    bool bJustHideModal = false;
    if (GUI_modalWindow) {
        switch (ev->type) {
#ifdef _RPi
            case SDL_FINGERDOWN:
            {
                SDL_TouchFingerEvent e = ev->tfinger;
                int x = (int)(e.x*GUI_windowWidth / GUI_mouseScale);
                int y = (int)(e.y*GUI_windowHeight / GUI_mouseScale);
                GUI_View *w = ((GUI_View *)GUI_modalWindow);

                if (!w->hitTest(x, y)) {
                    if (GUI_AutoHideModal) {
                        w->hide();
                        bJustHideModal = true;
                        if( GUI_AutoCloseModal )
                        w->close();
                        break;
                    }
                    return;
                } else {
                    GUI_modalWindow->handleEvents(ev);
                    return;
                }
                break;
            }
#endif            
            case SDL_MOUSEBUTTONDOWN:
            {
                SDL_MouseButtonEvent e = ev->button;
                int x = (int)(e.x*GUI_mouseScale);
                int y = (int)(e.y*GUI_mouseScale);
                GUI_View *w = ((GUI_View *)GUI_modalWindow);

                if (!w->hitTest(x, y)) {
                    if (GUI_AutoHideModal) {
                        w->hide();
                        bJustHideModal = true;
                        if( GUI_AutoCloseModal )
                        w->close();
                        break;
                    }

                    return;
                } else {
                    GUI_modalWindow->handleEvents(ev);
                    return;
                }

                break;
            }
#ifdef _RPi
            case SDL_FINGERUP:
            {
                SDL_TouchFingerEvent e = ev->tfinger;
                int x = (int)(e.x*GUI_windowWidth / GUI_mouseScale);
                int y = (int)(e.y*GUI_windowHeight / GUI_mouseScale);
                GUI_View *w = ((GUI_View *)GUI_modalWindow);

                if (!w->hitTest(x, y)) {
                    if (GUI_AutoHideModal )
                    {
                        w->hide();
                        bJustHideModal = true;
                        if( GUI_AutoCloseModal )
                        w->close();
                    }
                    return;
                } else {
                    GUI_modalWindow->handleEvents(ev);
                    return;
                }
            }
#endif            
            case SDL_MOUSEBUTTONUP:
            {
                SDL_MouseButtonEvent e = ev->button;
                int x = (int)(e.x*GUI_mouseScale);
                int y = (int)(e.y*GUI_mouseScale);
                GUI_View *w = ((GUI_View *)GUI_modalWindow);

                if (!w->hitTest(x, y)) {
                    if (GUI_AutoHideModal )
                    {
                        w->hide();
                        bJustHideModal = true;
                        if( GUI_AutoCloseModal )
                        w->close();
                    }
                    return;
                } else {
                    GUI_modalWindow->handleEvents(ev);
                    return;
                }
            }
        }

    }
    if( bJustHideModal ) {
        GUI_modalWindow = NULL;
        bJustHideModal = false;
        return;
    }

    if (GUI_TopView) {
        if (GUI_TopView->handleEvents(ev)) {
            if (ev->type == GUI_EventPaint) {
                bFirtPainted = true;
            }

            return;
        }
    }
}

bool GUI_CheckRenderer(void) {
    if (!GUI_renderer) {
        GUI_Log("Uninitialized Renderer access denied.\n");
        return false;
    }

    return true;
}

/*
 * This is a 32-bit pixel function created with help from this
 * website: http://www.libsdl.org/intro.en/usingvideo.html
 *
 * You will need to make changes if you want it to work with
 * 8-, 16- or 24-bit surfaces.  Consult the above website for
 * more information.
 */

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    Uint8 *target_pixel = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
    *(Uint32 *)target_pixel = pixel;
}

void draw_line(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 pixel) {
    if (x1 == x2) {
        for (int y = y1; y <= y2; y++) {
            Uint8 *target_pixel = (Uint8 *)surface->pixels + y * surface->pitch + x1 * 4;
            *(Uint32 *)target_pixel = pixel;
        }
    } else if (y1 == y2) {
        for (int x = x1; x <= x2; x++) {
            Uint8 *target_pixel = (Uint8 *)surface->pixels + y1 * surface->pitch + x * 4;
            *(Uint32 *)target_pixel = pixel;
        }
    }
}

// pom
const int __max_pt_size = 512;
bool __use_font_awesome_texture = false;
SDL_Texture* __faTextureCircleFilled = NULL;
static SDL_Texture* __faTextureCircleBorder = NULL;
static SDL_Texture* __faTextureCircleBorderThin = NULL;
static SDL_Point    __faTextureSize[3];

int GUI_InitFontAwesomeTexture() {
    if (!__use_font_awesome_texture) {
        int status = 0;

        if (TTF_WasInit() == false) {
            status = TTF_Init();
        }

        if (status == 0) {
            int ptsize = __max_pt_size * GUI_scale;
            TTF_Font* ttf = TTF_OpenFont(kFont_awesome_path, ptsize);

            if (ttf) {
                SDL_Surface* surf = TTF_RenderGlyph_Blended(ttf, kIcon_circle, cWhite);
                __faTextureCircleFilled = SDL_CreateTextureFromSurface(GUI_renderer, surf);
                __faTextureSize[0].x = surf->w;
                __faTextureSize[0].y = surf->h;
                SDL_FreeSurface(surf);

                surf = TTF_RenderGlyph_Blended(ttf, kIcon_circle_o, cWhite);
                __faTextureCircleBorder = SDL_CreateTextureFromSurface(GUI_renderer, surf);
                __faTextureSize[1].x = surf->w;
                __faTextureSize[1].y = surf->h;
                SDL_FreeSurface(surf);

                surf = TTF_RenderGlyph_Blended(ttf, kIcon_circle_thin, cWhite);
                __faTextureCircleBorderThin = SDL_CreateTextureFromSurface(GUI_renderer, surf);
                __faTextureSize[2].x = surf->w;
                __faTextureSize[2].y = surf->h;
                SDL_FreeSurface(surf);

                SDL_SetTextureBlendMode(__faTextureCircleFilled, SDL_BLENDMODE_BLEND);
                SDL_SetTextureBlendMode(__faTextureCircleBorder, SDL_BLENDMODE_BLEND);
                SDL_SetTextureBlendMode(__faTextureCircleBorderThin, SDL_BLENDMODE_BLEND);

                __use_font_awesome_texture = true;
                // TTF_CloseFont(ttf); // TODO: Check this line
            }
        }
    }

    return __use_font_awesome_texture;
}

void GUI_DeleteFontAwesomeTexture() {
    if (__use_font_awesome_texture) {
        if (__faTextureCircleFilled)
            SDL_DestroyTexture(__faTextureCircleFilled);

        if (__faTextureCircleBorder)
            SDL_DestroyTexture(__faTextureCircleBorder);

        if (__faTextureCircleBorderThin)
            SDL_DestroyTexture(__faTextureCircleBorderThin);
    }
}

static void srf_mapsympixel(SDL_Surface *srf, int x, int y, int xc, int yc, int symnum, Uint32 pixel) {
    switch (symnum) {
        case 8:	// symmetric in all octents
            set_pixel(srf, y + xc, x + yc, pixel);
            set_pixel(srf, y + xc, -x + yc - 1, pixel);
            set_pixel(srf, -y + xc - 1, x + yc, pixel);
            set_pixel(srf, -y + xc - 1, -x + yc - 1, pixel);

        case 4:	// symmetric in all quadrants
            set_pixel(srf, -x + xc - 1, -y + yc - 1, pixel);
            set_pixel(srf, -x + xc - 1, y + yc, pixel);

        case 2:	// symmetric about only X-axis
            set_pixel(srf, x + xc, -y + yc - 1, pixel);
            set_pixel(srf, x + xc, y + yc, pixel);
            break;

        case 1:	// symmetric about only Y-axis
            set_pixel(srf, -x + xc - 1, y + yc, pixel);
            set_pixel(srf, x + xc, y + yc, pixel);
    }
}

static void srf_mapsympixel2(SDL_Surface *srf, int x, int y, int w, int h, int xc, int yc, int symnum, Uint32 pixel) {
    switch (symnum) {
        case 8:	// symmetric in all octents
            set_pixel(srf, y + xc + w, x + yc + h, pixel);
            set_pixel(srf, y + xc + w, -x + yc - 1, pixel);
            set_pixel(srf, -y + xc - 1, x + yc + h, pixel);
            set_pixel(srf, -y + xc - 1, -x + yc - 1, pixel);

        case 4:	// symmetric in all quadrants
            set_pixel(srf, -x + xc - 1, -y + yc - 1, pixel);
            set_pixel(srf, -x + xc - 1, y + yc + h, pixel);

        case 2:	// symmetric about only X-axis
            set_pixel(srf, x + xc + w, -y + yc - 1, pixel);
            set_pixel(srf, x + xc + w, y + yc + h, pixel);
            break;

        case 1:	// symmetric about only Y-axis
            set_pixel(srf, -x + xc - 1, y + yc, pixel);
            set_pixel(srf, x + xc + w, y + yc, pixel);
    }
}

static void mapsympixel(SDL_Renderer *renderer, int ox, int oy, int x, int y, int xc, int yc, int symnum) {
    switch (symnum) {
        case 8:	// symmetric in all octents
            SDL_RenderDrawPoint(renderer, ox + y + xc, oy + x + yc);
            SDL_RenderDrawPoint(renderer, ox + y + xc, oy - x + yc - 1);
            SDL_RenderDrawPoint(renderer, ox - y + xc - 1, oy + x + yc);
            SDL_RenderDrawPoint(renderer, ox - y + xc - 1, oy - x + yc - 1);

        case 4:	// symmetric in all quadrants
            SDL_RenderDrawPoint(renderer, ox - x + xc - 1, oy - y + yc - 1);
            SDL_RenderDrawPoint(renderer, ox - x + xc - 1, oy + y + yc);

        case 2:	// symmetric about only X-axis
            SDL_RenderDrawPoint(renderer, ox + x + xc, oy - y + yc - 1);
            SDL_RenderDrawPoint(renderer, ox + x + xc, oy + y + yc);
            break;

        case 1:	// symmetric about only Y-axis
            SDL_RenderDrawPoint(renderer, ox - x + xc - 1, oy + y + yc);
            SDL_RenderDrawPoint(renderer, ox + x + xc, oy + y + yc);
    }
}

static void mapsympixel2(SDL_Renderer *renderer, int ox, int oy, int x, int y, int w, int h, int xc, int yc, int symnum) {
    switch (symnum) {
        case 8:	// symmetric in all octents
            SDL_RenderDrawPoint(renderer, ox + y + xc + w, oy + x + yc + h);
            SDL_RenderDrawPoint(renderer, ox + y + xc + w, oy - x + yc - 1);
            SDL_RenderDrawPoint(renderer, ox - y + xc - 1, oy + x + yc + h);
            SDL_RenderDrawPoint(renderer, ox - y + xc - 1, oy - x + yc - 1);

        case 4:	// symmetric in all quadrants
            SDL_RenderDrawPoint(renderer, ox - x + xc - 1, oy - y + yc - 1);
            SDL_RenderDrawPoint(renderer, ox - x + xc - 1, oy + y + yc + h);

        case 2:	// symmetric about only X-axis
            SDL_RenderDrawPoint(renderer, ox + x + xc + w, oy - y + yc - 1);
            SDL_RenderDrawPoint(renderer, ox + x + xc + w, oy + y + yc + h);
            break;

        case 1:	// symmetric about only Y-axis
            SDL_RenderDrawPoint(renderer, ox - x + xc - 1, oy + y + yc);
            SDL_RenderDrawPoint(renderer, ox + x + xc + w, oy + y + yc);
    }
}

void GUI_DrawRect(int x, int y, int w, int h, SDL_Color col) {
#if __USE_SKIA__
    SkCanvas * canvas = GUI_canvas;
    SkRect r;
    r.setXYWH(x, y, w, h);
    SkPaint p;
    p.setStyle(SkPaint::kStroke_Style);
    p.setColor(sk_color(col));
    canvas->drawRect(r, p);
#else
    GUI_DrawRect(GUI_renderer, x, y, w, h, col);
#endif
}

void GUI_FillRect(int x, int y, int w, int h, SDL_Color col) {
#if __USE_SKIA__
    SkCanvas * canvas = GUI_canvas;
    SkRect r;
    r.setXYWH(x, y, w, h);
    SkPaint p;
    p.setStyle(SkPaint::kFill_Style);
    p.setColor(sk_color(col));
    canvas->drawRect(r, p);
#else
    GUI_FillRect(GUI_renderer, x, y, w, h, col);
#endif
}

void GUI_FillRect(int cx, int cy, int w, int h, int degree, SDL_Color col) {
    GUI_FillRect(GUI_renderer, cx, cy, w, h, degree, col);
}

void GUI_DrawRoundRectHW(int _x, int _y, int w, int h, int radius, SDL_Color col) {
    SDL_SetRenderDrawColor(GUI_renderer, col.r, col.g, col.b, col.a);

    int x, y, p = 0 - radius;

    for (x = 0, y = radius - 1, p = 2 - radius; x <= y; x++) {
        mapsympixel2(GUI_renderer, _x, _y, x, y, w - radius - radius, h - radius - radius, radius, radius, 8);

        if (p >= 0) {
            p += (2 * x - 2 * y + 1);
            y--;
        } else
            p += (2 * x + 3);
    }

    SDL_RenderDrawLine(GUI_renderer, _x + radius, _y + 0, _x + w - radius, _y + 0);
    SDL_RenderDrawLine(GUI_renderer, _x + radius, _y + h - 1, _x + w - radius, _y + h - 1);
    SDL_RenderDrawLine(GUI_renderer, _x + 0, _y + radius, _x + 0, _y + h - radius);
    SDL_RenderDrawLine(GUI_renderer, _x + w - 1, _y + radius, _x + w - 1, _y + h - radius);
}

void GUI_DrawRoundRectSW(int _x, int _y, int w, int h, int radius, SDL_Color col) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
#else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
#endif
    SDL_SetRenderDrawColor(GUI_renderer, col.r, col.g, col.b, col.a);
    SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);

    if (surface == NULL) {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));

    int x, y, p = 0 - radius;
    Uint32 uc = SDL_MapRGBA(surface->format, col.r, col.g, col.b, col.a);

    for (x = 0, y = radius - 1, p = 2 - radius; x <= y; x++) {
        srf_mapsympixel2(surface, x, y, w - radius - radius, h - radius - radius, radius, radius, 8, uc);

        if (p >= 0) {
            p += (2 * x - 2 * y + 1);
            y--;
        } else
            p += (2 * x + 3);
    }

    draw_line(surface, radius, 0, w - radius, 0, uc);
    draw_line(surface, radius, h - 1, w - radius, h - 1, uc);
    draw_line(surface, 0, radius, 0, h - radius, uc);
    draw_line(surface, w - 1, radius, w - 1, h - radius, uc);

    SDL_Texture *tx = SDL_CreateTextureFromSurface(GUI_renderer, surface);
    SDL_FreeSurface(surface);

    SDL_RenderCopy(GUI_renderer, tx, NULL, GUI_MakeRect(_x, _y, w, h));
    SDL_DestroyTexture(tx);
}

void GUI_DrawRoundRect(int _x, int _y, int w, int h, int radius, SDL_Color col) {
#if __USE_SKIA__
    int x = _x;
    int y = _y;
    if( radius == w /2 ) {
        return GUI_DrawCircle( x+radius, y+radius, radius, col);
    }
    if ((radius * 2) > w)
        radius = w / 2;
    if ((radius * 2) > h)
        radius = h / 2;

    if( radius <= 2 ) {
        GUI_DrawRect(x, y, w, h, col );
    }
    else{
        SkCanvas * canvas = GUI_canvas;
        SkRect r;
        r.setXYWH(x, y, w, h);
        SkPaint p;
        p.setStyle(SkPaint::kStroke_Style);
        p.setColor(sk_color(col));
        canvas->drawRoundRect(r, radius, radius, p);
    }
#else
    //#if defined(SW_RENDER_SURFACE) || defined(WIN32) // Tooh - Fix stitch render bug on Windows
    #if defined(SW_RENDER_SURFACE) // NooM bug?
        GUI_DrawRoundRectSW(_x, _y, w, h, radius, col);
    #else
        GUI_DrawRoundRectHW(_x, _y, w, h, radius, col); // pom
    #endif
#endif
}

void GUI_FillRoundRectHW(int x, int y, int w, int h, int radius, SDL_Color col) {
#ifdef SW_RENDER_FONTAWESOME
    if (__use_font_awesome_texture) {
        GUI_FillRect(x + radius, y, w - radius * 2, radius, col);
        GUI_FillRect(x, y + radius, w, h - radius * 2, col);
        GUI_FillRect(x + radius, y + h - radius, w - radius * 2, radius, col);

        int ww = __faTextureSize[0].x;
        int hh = __faTextureSize[0].y;
        int size = MIN(ww, hh);
        SDL_Rect srcrect{(ww - size) / 2, (hh - size) / 2, size / 2, size / 2};
        SDL_Rect dstrect{x, y, radius, radius};
        SDL_SetTextureColorMod(__faTextureCircleFilled, col.r, col.g, col.b);
        SDL_SetTextureAlphaMod(__faTextureCircleFilled, col.a);
        SDL_RenderCopy(GUI_renderer, __faTextureCircleFilled, &srcrect, &dstrect);
        SDL_RenderCopyEx(GUI_renderer, __faTextureCircleFilled, &srcrect, GUI_MakeRect(x + w - radius, y, radius, radius), 0, NULL, SDL_FLIP_HORIZONTAL);
        SDL_RenderCopyEx(GUI_renderer, __faTextureCircleFilled, &srcrect, GUI_MakeRect(x, y + h - radius, radius, radius), 0, NULL, SDL_FLIP_VERTICAL);
        
        if (radius & 1) {
            // if radius is odd, it OK after rotate, 
            // ex. radius = 11, cx = 5, cy = 5 
            // equal left-right around center
            SDL_RenderCopyEx(GUI_renderer, __faTextureCircleFilled, &srcrect, GUI_MakeRect(x + w - radius, y + h - radius, radius, radius), 180, NULL, SDL_FLIP_NONE);
        } else {
            // if radius is even, it BUG! after rotate
            // ex. if radius = 12, cx = 6, cy = 6
            // not equal left-right around center
            SDL_RenderCopyEx(GUI_renderer, __faTextureCircleFilled, &srcrect, GUI_MakeRect(x + w - radius - 1, y + h - radius - 1, radius, radius), 180, NULL, SDL_FLIP_NONE);
        }
        return;
    }
#endif
    GUI_FillRoundRect(GUI_renderer, x, y, w, h, radius, col);
}

void GUI_FillRoundRectHW(int cx, int cy, int w, int h, int radius, int degree, SDL_Color col) {
    GUI_FillRoundRect(GUI_renderer, cx, cy, w, h, radius, degree, col);
}

void GUI_FillRoundRectSW(int _x, int _y, int w, int h, int radius, SDL_Color col) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
#else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
#endif
    SDL_SetRenderDrawColor(GUI_renderer, col.r, col.g, col.b, col.a);
    SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);

    if (surface == NULL) {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
    int x, y, p = 0 - radius;
    Uint32 uc = SDL_MapRGBA(surface->format, col.r, col.g, col.b, col.a);
    int xcc = radius;
    int ycc = radius;

    for (x = 0, y = radius - 1, p = 2 - radius; x <= y; x++) {
        draw_line(surface, x + xcc + w - radius - radius, -y + ycc - 1, x + xcc + w - radius - radius, y + ycc + h - radius - radius, uc);
        draw_line(surface, -x + xcc - 1, -y + ycc - 1, -x + xcc - 1, y + ycc + h - radius - radius, uc);
        draw_line(surface, -y + xcc - 1, -x + ycc - 1, -y + xcc - 1, x + ycc + h - radius - radius, uc);
        draw_line(surface, y + xcc + w - radius - radius, -x + ycc - 1, y + xcc + w - radius - radius, x + ycc + h - radius - radius, uc);

        if (p >= 0) {
            p += (2 * x - 2 * y + 1);
            y--;
        } else
            p += (2 * x + 3);
    }

    SDL_FillRect(surface, GUI_MakeRect(radius, 0, w - radius - radius, h), uc);

    SDL_Texture *tx = SDL_CreateTextureFromSurface(GUI_renderer, surface);
    SDL_FreeSurface(surface);

    SDL_RenderCopy(GUI_renderer, tx, NULL, GUI_MakeRect(_x, _y, w, h));
    SDL_DestroyTexture(tx);
}

void GUI_FillRoundRect(int _x, int _y, int w, int h, int radius, SDL_Color col) {
#if __USE_SKIA__
    int x = _x;
    int y = _y;
    if ((radius * 2) > w)
        radius = w / 2;
    if ((radius * 2) > h)
        radius = h / 2;

    if( radius <= 2 ) {
        GUI_FillRect(x, y, w, h, col );
    }
    else{
        SkCanvas * canvas = GUI_canvas;
        SkRect r;
        r.setXYWH(x, y, w, h);
        SkPaint p;
        p.setStyle(SkPaint::kFill_Style);
        p.setColor(sk_color(col));
        canvas->drawRoundRect(r, radius, radius, p);
    }
#else
    #ifdef SW_RENDER_SURFACE
        GUI_FillRoundRectSW(_x, _y, w, h, radius, col);
    #else
        GUI_FillRoundRectHW(_x, _y, w, h, radius, col);
    #endif
#endif
}

// pom
void GUI_DrawRoundRect(int x, int y, int w, int h, int corners[4], SDL_Color c) {
    w-=1;
    h-=1;
    
    if(corners[0])
        arcRGBA(GUI_renderer, x+corners[0], y+corners[0], corners[0], 180, 270, c.r, c.g, c.b, c.a);
    
    if(corners[1])
        arcRGBA(GUI_renderer, x+w-corners[1], y+corners[1], corners[1], 270, 360, c.r, c.g, c.b, c.a);
    
    if(corners[2])
        arcRGBA(GUI_renderer, x+w-corners[2], y+h-corners[2], corners[2], 0, 90, c.r, c.g, c.b, c.a);
    
    if(corners[3])
        arcRGBA(GUI_renderer, x+corners[3], y+h-corners[3], corners[3], 90, 180, c.r, c.g, c.b, c.a);
    
    hlineRGBA(GUI_renderer, x+corners[0], x+w-corners[1], y, c.r, c.g, c.b, c.a);
    hlineRGBA(GUI_renderer, x+corners[3], x+w-corners[2], y+h, c.r, c.g, c.b, c.a);
    
    vlineRGBA(GUI_renderer, x, y+corners[0], y+h-corners[3], c.r, c.g, c.b, c.a);
    vlineRGBA(GUI_renderer, x+w, y+corners[1], y+h-corners[2], c.r, c.g, c.b, c.a);
}

void GUI_FillRoundRect(int x, int y, int w, int h, int corners[4], SDL_Color col) {
    if (__use_font_awesome_texture) {
        GUI_FillRect(x + corners[0], y, w - (corners[0]+corners[1]), h-max(corners[2], corners[3]), col);
        GUI_FillRect(x+max(corners[0], corners[3]), y + corners[1], w-max(corners[0], corners[3]), h-corners[1]-corners[2], col);
        GUI_FillRect(x+corners[3], y+max(corners[0],corners[1]), w-corners[2]-corners[3], h-max(corners[0],corners[1]), col);
        GUI_FillRect(x, y+corners[0], w-max(corners[1],corners[2]), h-corners[0]-corners[3], col);
        
        int ww = __faTextureSize[0].x;
        int hh = __faTextureSize[0].y;
        int size = MIN(ww, hh);
        SDL_Rect srcrect{(ww - size) / 2, (hh - size) / 2, size / 2, size / 2};
        SDL_Rect dstrect[4];
        dstrect[0] = {x, y, corners[0], corners[0]}; // lt
        dstrect[1] = {x+w-corners[1], y, corners[1], corners[1]}; // rt
        dstrect[2] = {x+w-corners[2], y+h-corners[2], corners[2], corners[2]}; // rb
        dstrect[3] = {x, y+h-corners[3], corners[3], corners[3]}; // lb
        
        SDL_SetTextureColorMod(__faTextureCircleFilled, col.r, col.g, col.b);
        SDL_SetTextureAlphaMod(__faTextureCircleFilled, col.a);
        if(corners[0]) SDL_RenderCopy(GUI_renderer, __faTextureCircleFilled, &srcrect, &dstrect[0]);
        if(corners[1]) SDL_RenderCopyEx(GUI_renderer, __faTextureCircleFilled, &srcrect, &dstrect[1], 0, NULL, SDL_FLIP_HORIZONTAL);
        if(corners[2]) SDL_RenderCopyEx(GUI_renderer, __faTextureCircleFilled, &srcrect, &dstrect[2], 180, NULL, SDL_FLIP_NONE);
        if(corners[3]) SDL_RenderCopyEx(GUI_renderer, __faTextureCircleFilled, &srcrect, &dstrect[3], 0, NULL, SDL_FLIP_VERTICAL);
        
        return;
    }
}

void GUI_DrawCircleHW(int cx, int cy, int radius, SDL_Color col) {
#ifdef SW_RENDER_FONTAWESOME
    if (__use_font_awesome_texture) {
        int w = __faTextureSize[2].x;
        int h = __faTextureSize[2].y;
        int size = MIN(w, h);
        SDL_Rect srcrect{(w - size) / 2, (h - size) / 2, size, size};
        SDL_Rect dstrect{cx - radius, cy - radius, radius * 2, radius * 2};
        SDL_SetTextureColorMod(__faTextureCircleBorderThin, col.r, col.g, col.b);
        SDL_SetTextureAlphaMod(__faTextureCircleBorderThin, col.a);
        SDL_RenderCopy(GUI_renderer, __faTextureCircleBorderThin, &srcrect, &dstrect);
        return;
    }
#endif

    SDL_SetRenderDrawColor(GUI_renderer, col.r, col.g, col.b, col.a);
    int x, y, p = 0 - radius;

    for (x = 0, y = radius - 1, p = 2 - radius; x <= y; x++) {
        mapsympixel(GUI_renderer, cx - radius, cy - radius, x, y, radius, radius, 8);

        if (p >= 0) {
            p += (2 * x - 2 * y + 1);
            y--;
        } else
            p += (2 * x + 3);
    }
}

void GUI_DrawCircleSW(int xc, int yc, int radius, SDL_Color col) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
#else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
#endif
    SDL_SetRenderDrawColor(GUI_renderer, col.r, col.g, col.b, col.a);
    SDL_Surface *surface = SDL_CreateRGBSurface(0, radius * 2, radius * 2, 32, rmask, gmask, bmask, amask);

    if (surface == NULL) {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
    int x, y, p = 0 - radius;
    Uint32 uc = SDL_MapRGBA(surface->format, col.r, col.g, col.b, col.a);

    for (x = 0, y = radius - 1, p = 2 - radius; x <= y; x++) {
        srf_mapsympixel(surface, x, y, radius, radius, 8, uc);

        if (p >= 0) {
            p += (2 * x - 2 * y + 1);
            y--;
        } else
            p += (2 * x + 3);
    }

    SDL_Texture *tx = SDL_CreateTextureFromSurface(GUI_renderer, surface);
    SDL_FreeSurface(surface);

    SDL_RenderCopy(GUI_renderer, tx, NULL, GUI_MakeRect(xc - radius, yc - radius, radius * 2, radius * 2));
    SDL_DestroyTexture(tx);
}

void GUI_DrawCircle(int xc, int yc, int radius, SDL_Color col) {
#if __USE_SKIA__
    SkCanvas * canvas = GUI_canvas;
    SkPaint p;
    p.setStyle(SkPaint::kStroke_Style);
    p.setColor(sk_color(col));
    canvas->drawCircle(xc, yc, radius, p);
#else
    #ifdef SW_RENDER_SURFACE
        GUI_DrawCircleSW(xc, yc, radius, col);
    #else
        GUI_DrawCircleHW(xc, yc, radius, col);
    #endif
#endif
}

void GUI_FillCircle0(int cx, int cy, int radius, SDL_Color col) {
#ifdef SW_RENDER_FONTAWESOME
    if (__use_font_awesome_texture) {
        int w = __faTextureSize[0].x;
        int h = __faTextureSize[0].y;
        int size = MIN(w, h);
        SDL_Rect srcrect{(w - size) / 2, (h - size) / 2, size, size};
        SDL_Rect dstrect{cx - radius, cy - radius, radius * 2, radius * 2};
        SDL_SetTextureColorMod(__faTextureCircleFilled, col.r, col.g, col.b);
        SDL_SetTextureAlphaMod(__faTextureCircleFilled, col.a);
        SDL_RenderCopy(GUI_renderer, __faTextureCircleFilled, &srcrect, &dstrect);
        return;
    }
#endif
    GUI_FillCircle(GUI_renderer, cx, cy, radius, col);
}

void GUI_FillCircle(int xc, int yc, int radius, SDL_Color col) {
#if __USE_SKIA__
    SkCanvas * canvas = GUI_canvas;
    SkPaint p;
    p.setStyle(SkPaint::kFill_Style);
    p.setColor(sk_color(col));
    canvas->drawCircle(xc, yc, radius, p);
#else
    #ifdef SW_RENDER_SURFACE
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        Uint32 rmask = 0xff000000;
        Uint32 gmask = 0x00ff0000;
        Uint32 bmask = 0x0000ff00;
        Uint32 amask = 0x000000ff;
    #else
        Uint32 rmask = 0x000000ff;
        Uint32 gmask = 0x0000ff00;
        Uint32 bmask = 0x00ff0000;
        Uint32 amask = 0xff000000;
    #endif

        SDL_Surface *surface = SDL_CreateRGBSurface(0, radius * 2, radius * 2, 32, rmask, gmask, bmask, amask);

        if (surface == NULL) {
            fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
            exit(1);
        }

        SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
        int x, y, p = 0 - radius;
        Uint32 uc = SDL_MapRGBA(surface->format, col.r, col.g, col.b, col.a);
        int xcc = radius;
        int ycc = radius;

        for (x = 0, y = radius - 1, p = 2 - radius; x <= y; x++) {
            draw_line(surface, x + xcc, -y + ycc - 1, x + xcc, y + ycc, uc);
            draw_line(surface, -x + xcc - 1, -y + ycc - 1, -x + xcc - 1, y + ycc, uc);
            draw_line(surface, -y + xcc - 1, -x + ycc - 1, -y + xcc - 1, x + ycc, uc);
            draw_line(surface, y + xcc, -x + ycc - 1, y + xcc, x + ycc, uc);
            if (p >= 0) {
                p += (2 * x - 2 * y + 1);
                y--;
            } else
                p += (2 * x + 3);
        }

        SDL_Texture *tx = SDL_CreateTextureFromSurface(GUI_renderer, surface);
        SDL_FreeSurface(surface);

        SDL_RenderCopy(GUI_renderer, tx, NULL, GUI_MakeRect(xc - radius, yc - radius, radius * 2, radius * 2));
        SDL_DestroyTexture(tx);
    #else
        GUI_FillCircle0(xc, yc, radius, col);
    #endif
#endif
}

void GUI_DrawLine(int x1, int y1, int x2, int y2, SDL_Color col) {
#if __USE_SKIA__
    SkCanvas * canvas = GUI_canvas;
    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setColor(sk_color(col));
    canvas->drawLine(x1, y1, x2, y2, paint);
#else
    GUI_DrawLine(GUI_renderer, x1, y1, x2, y2, col);
#endif
}

void GUI_ClearViewport(SDL_Color color) {
//    SDL_Rect cliprect, viewport;
//    SDL_RenderGetClipRect(GUI_renderer, &cliprect);
//    SDL_RenderGetViewport(GUI_renderer, &viewport);
//
//    SDL_RenderSetViewport(GUI_renderer, GUI_MakeRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale));
//    SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale));
//    GUI_FillRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale, color);
//
//    SDL_RenderSetClipRect(GUI_renderer, &cliprect);
//    SDL_RenderSetViewport(GUI_renderer, &viewport);
    
    GUI_Rect cliprect, viewport;
    GUI_GetClipRect(cliprect);
    GUI_GetViewport(viewport);

    GUI_SetViewport(*GUI_MakeRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale));
    GUI_SetClipRect(*GUI_MakeRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale));
    GUI_FillRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale, color);

    GUI_SetClipRect(cliprect);
    GUI_SetViewport(viewport);
}

GUI_View *GUI_CreateTopView(const char* t, int x, int y, int w, int h, void(*d_cmd)(GUI_View *)) {
    if (GUI_TopView) {
        GUI_Log("GUI_TopView existed");
        printf("ERROR: GUI_TopView existed.");
        exit(1);
    }

    GUI_TopView = new GUI_View(NULL, t, x, y, w, h, cWhite, d_cmd);

    if (w == -1) {
        w = GUI_windowWidth - x;
    }

    if (h == -1) {
        h = GUI_windowHeight - y;
    }

    GUI_TopView->resize(w, h);
    GUI_TopView->setMargin(0, 0, 0, 0);
    GUI_TopView->setPadding(0, 0, 0, 0);
    GUI_TopView->setBorder(0);
    GUI_TopView->bgcol = cWhite;

    return GUI_TopView;
}

namespace sdl {
    namespace framerate {
        Uint32 _getTicks() {
            Uint32 ticks = SDL_GetTicks();

            if (ticks == 0) {
                return 1;
            } else {
                return ticks;
            }
        }

        void SDL_initFramerate(FPSmanager * manager) {
            /*
             * Store some sane values
              */
            manager->framecount = 0;
            manager->rate = FPS_DEFAULT;
            manager->rateticks = (1000.0f / (float)FPS_DEFAULT);
            manager->baseticks = _getTicks();
            manager->lastticks = manager->baseticks;

        }

        int SDL_setFramerate(FPSmanager * manager, Uint32 rate) {
            if ((rate >= FPS_LOWER_LIMIT) && (rate <= FPS_UPPER_LIMIT)) {
                manager->framecount = 0;
                manager->rate = rate;
                manager->rateticks = (1000.0f / (float)rate);
                return (0);
            } else {
                return (-1);
            }
        }

        int SDL_getFramerate(FPSmanager * manager) {
            if (manager == NULL) {
                return (-1);
            } else {
                return ((int)manager->rate);
            }
        }

        int SDL_getFramecount(FPSmanager * manager) {
            if (manager == NULL) {
                return (-1);
            } else {
                return ((int)manager->framecount);
            }
        }

        Uint32 SDL_framerateDelay(FPSmanager * manager) {
            Uint32 current_ticks;
            Uint32 target_ticks;
            Uint32 the_delay;
            Uint32 time_passed = 0;

            /*
             * No manager, no delay
             */
            if (manager == NULL) {
                return 0;
            }

            /*
             * Initialize uninitialized manager
             */
            if (manager->baseticks == 0) {
                SDL_initFramerate(manager);
            }

            /*
             * Next frame
             */
            manager->framecount++;

            /*
             * Get/calc ticks
             */
            current_ticks = sdl::framerate::_getTicks();
            time_passed = current_ticks - manager->lastticks;
            manager->lastticks = current_ticks;
            target_ticks = manager->baseticks + (Uint32)((float)manager->framecount * manager->rateticks);

            if (current_ticks <= target_ticks) {
                the_delay = target_ticks - current_ticks;
                SDL_Delay(the_delay);
            } else {
                manager->framecount = 0;
                manager->baseticks = _getTicks();
            }

            return time_passed;
        }

        FPSmanager manager;
        void GUI_RunWithManager() {

            GUI_running = true;
            //manager;

            SDL_initFramerate(&manager);

            SDL_framerateDelay(&manager);

            quit = false;
            while (!quit) {
                doGuiLoop();
                SDL_framerateDelay(&manager);
            }

            GUI_running = false;
            GUI_Log("GUI stop.\n");
        }

        // pom
        Uint32 getFrameTime() {
            if (frameTimeMillis < MILLESECONDS_PER_FRAME)
                frameTimeMillis = MILLESECONDS_PER_FRAME;
            return frameTimeMillis;
        }

        void doGuiLoop() {
            testCount = 0;
            SDL_Event ev;

            SDL_RenderSetViewport(GUI_renderer, NULL);
            SDL_RenderSetClipRect(GUI_renderer, NULL);

            SDL_SetRenderDrawColor(GUI_renderer, 0x64, 0x95, 0xed, 0xff);
            SDL_RenderClear(GUI_renderer);

            while (SDL_PollEvent(&ev)) {
                switch (ev.type) {
                    case SDL_QUIT:
                        quit = true;
                        break;

                    case SDL_WINDOWEVENT:
                        switch (ev.window.event) {
                            case SDL_WINDOWEVENT_RESIZED:
                                printf("Event: Window Resized: %i, %i\n", ev.window.data1, ev.window.data2);
                                GUI_windowWidth = ev.window.data1;
                                GUI_windowHeight = ev.window.data2;
                                SDL_GL_GetDrawableSize(GUI_window, &GUI_physicalWindowWidth, &GUI_physicalWindowHeight);
                                GUI_Log("drawable: %d %d\n", GUI_physicalWindowWidth, GUI_physicalWindowHeight);

                                GUI_SetScaleParameters();
                                break;
                        }
                        break;

                    case SDL_KEYDOWN:
                        if (ev.key.keysym.sym == SDLK_AC_BACK) {
                            quit = true;
                            break;
                        }

                        break;
                }

                handle_events(&ev);
            }

            SDL_Event event;
            event.type = GUI_EventPaint;
            handle_events(&event);

            if (user_message_queue.size() > 0) {
                GUI_UserMessage user_msg = user_message_queue.front();

                SDL_UserEvent user_event;
                user_event.type = user_msg.message_id;
                user_event.windowID = user_msg.parameter1;
                user_event.code = user_msg.parameter2;
                user_event.data1 = user_msg.extra_parameter1;
                user_event.data2 = user_msg.extra_parameter2;

                handle_events((SDL_Event *)&user_event);

                user_message_queue.erase(user_message_queue.begin());
            }

            frame_count = manager.framecount;
            SDL_RenderPresent(GUI_renderer);
        }
    }
}

bool GUI_PostMessage(Uint32 msg, int param1, int param2, void *extra1, void *extra2) {
    GUI_UserMessage *user_msg = new GUI_UserMessage();
    user_msg->message_id = msg;
    user_msg->parameter1 = param1;
    user_msg->parameter2 = param2;
    user_msg->extra_parameter1 = extra1;
    user_msg->extra_parameter2 = extra2;

    user_message_queue.push_back(*user_msg);

    return true;
}

#ifdef _OF_EVENTS_

#include "ofEvents.h"

static ofEventArgs voidEventArgs;

//--------------------------------------
bool ofGetMousePressed(int button) { //by default any button
    return ofEvents().getMousePressed(button);
}

//--------------------------------------
bool ofGetKeyPressed(int key) {
    return ofEvents().getKeyPressed(key);
}

//--------------------------------------
int ofGetMouseX() {
    return ofEvents().getMouseX();
}

//--------------------------------------
int ofGetMouseY() {
    return ofEvents().getMouseY();
}

//--------------------------------------
int ofGetPreviousMouseX() {
    return ofEvents().getPreviousMouseX();
}

//--------------------------------------
int ofGetPreviousMouseY() {
    return ofEvents().getPreviousMouseY();
}

ofCoreEvents::ofCoreEvents()
    :targetRate(0)
    , bFrameRateSet(false)
    , currentMouseX(0)
    , currentMouseY(0)
    , previousMouseX(0)
    , previousMouseY(0)
    , bPreMouseNotSet(false) {

}

//------------------------------------------
void ofCoreEvents::disable() {
    setup.disable();
    draw.disable();
    update.disable();
    exit.disable();
    keyPressed.disable();
    keyReleased.disable();
    mouseDragged.disable();
    mouseReleased.disable();
    mousePressed.disable();
    mouseMoved.disable();
    mouseScrolled.disable();
    mouseEntered.disable();
    mouseExited.disable();
    touchDown.disable();
    touchUp.disable();
    touchMoved.disable();
    touchDoubleTap.disable();
    touchCancelled.disable();
}

//------------------------------------------
void ofCoreEvents::enable() {
    setup.enable();
    draw.enable();
    update.enable();
    exit.enable();
    keyPressed.enable();
    keyReleased.enable();
    mouseDragged.enable();
    mouseReleased.enable();
    mousePressed.enable();
    mouseMoved.enable();
    mouseScrolled.enable();
    mouseEntered.enable();
    mouseExited.enable();
    touchDown.enable();
    touchUp.enable();
    touchMoved.enable();
    touchDoubleTap.enable();
    touchCancelled.enable();
}

//--------------------------------------
bool ofCoreEvents::getMousePressed(int button) const { //by default any button
    if (button == -1) return pressedMouseButtons.size();
    return pressedMouseButtons.find(button) != pressedMouseButtons.end();
}

//--------------------------------------
bool ofCoreEvents::getKeyPressed(int key) const {
    if (key == -1) return pressedKeys.size();
    return pressedKeys.find(key) != pressedKeys.end();
}

//--------------------------------------
int ofCoreEvents::getMouseX() const {
    return currentMouseX;
}

//--------------------------------------
int ofCoreEvents::getMouseY() const {
    return currentMouseY;
}

//--------------------------------------
int ofCoreEvents::getPreviousMouseX() const {
    return previousMouseX;
}

//--------------------------------------
int ofCoreEvents::getPreviousMouseY() const {
    return previousMouseY;
}

//------------------------------------------
void ofCoreEvents::notifySetup() {
    ofNotifyEvent(setup, voidEventArgs);
}

//#include "ofGraphics.h"
//------------------------------------------
void ofCoreEvents::notifyUpdate() {
    ofNotifyEvent(update, voidEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyDraw() {
    ofNotifyEvent(draw, voidEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyKeyPressed(int key, int keycode, int scancode, int codepoint) {
    // FIXME: modifiers are being reported twice, for generic and for left/right
    // add operators to the arguments class so it can be checked for both
    if (key == OF_KEY_RIGHT_CONTROL || key == OF_KEY_LEFT_CONTROL) {
        pressedKeys.insert(OF_KEY_CONTROL);
        ofKeyEventArgs keyEventArgs(ofKeyEventArgs::Pressed, OF_KEY_CONTROL);
        ofNotifyEvent(keyPressed, keyEventArgs);
    } else if (key == OF_KEY_RIGHT_SHIFT || key == OF_KEY_LEFT_SHIFT) {
        pressedKeys.insert(OF_KEY_SHIFT);
        ofKeyEventArgs keyEventArgs(ofKeyEventArgs::Pressed, OF_KEY_SHIFT);
        ofNotifyEvent(keyPressed, keyEventArgs);
    } else if (key == OF_KEY_LEFT_ALT || key == OF_KEY_RIGHT_ALT) {
        pressedKeys.insert(OF_KEY_ALT);
        ofKeyEventArgs keyEventArgs(ofKeyEventArgs::Pressed, OF_KEY_ALT);
        ofNotifyEvent(keyPressed, keyEventArgs);
    } else if (key == OF_KEY_LEFT_SUPER || key == OF_KEY_RIGHT_SUPER) {
        pressedKeys.insert(OF_KEY_SUPER);
        ofKeyEventArgs keyEventArgs(ofKeyEventArgs::Pressed, OF_KEY_SUPER);
        ofNotifyEvent(keyPressed, keyEventArgs);
    }

    pressedKeys.insert(key);
    ofKeyEventArgs keyEventArgs(ofKeyEventArgs::Pressed, key, keycode, scancode, codepoint);
    ofNotifyEvent(keyPressed, keyEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyKeyReleased(int key, int keycode, int scancode, int codepoint) {
    // FIXME: modifiers are being reported twice, for generic and for left/right
    // add operators to the arguments class so it can be checked for both
    if (key == OF_KEY_RIGHT_CONTROL || key == OF_KEY_LEFT_CONTROL) {
        pressedKeys.erase(OF_KEY_CONTROL);
        ofKeyEventArgs keyEventArgs(ofKeyEventArgs::Released, OF_KEY_CONTROL);
        ofNotifyEvent(keyReleased, keyEventArgs);
    } else if (key == OF_KEY_RIGHT_SHIFT || key == OF_KEY_LEFT_SHIFT) {
        pressedKeys.erase(OF_KEY_SHIFT);
        ofKeyEventArgs keyEventArgs(ofKeyEventArgs::Released, OF_KEY_SHIFT);
        ofNotifyEvent(keyReleased, keyEventArgs);
    } else if (key == OF_KEY_LEFT_ALT || key == OF_KEY_RIGHT_ALT) {
        pressedKeys.erase(OF_KEY_ALT);
        ofKeyEventArgs keyEventArgs(ofKeyEventArgs::Released, OF_KEY_ALT);
        ofNotifyEvent(keyReleased, keyEventArgs);
    } else if (key == OF_KEY_LEFT_SUPER || key == OF_KEY_RIGHT_SUPER) {
        pressedKeys.erase(OF_KEY_SUPER);
        ofKeyEventArgs keyEventArgs(ofKeyEventArgs::Released, OF_KEY_SUPER);
        ofNotifyEvent(keyReleased, keyEventArgs);
    }

    pressedKeys.erase(key);
    ofKeyEventArgs keyEventArgs(ofKeyEventArgs::Released, key, keycode, scancode, codepoint);
    ofNotifyEvent(keyReleased, keyEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyKeyEvent(const ofKeyEventArgs & keyEvent) {
    switch (keyEvent.type) {
        case ofKeyEventArgs::Pressed:
            notifyKeyPressed(keyEvent.key);
            break;

        case ofKeyEventArgs::Released:
            notifyKeyReleased(keyEvent.key);
            break;
    }
}

//------------------------------------------
void ofCoreEvents::notifyMouseEvent(const ofMouseEventArgs & mouseEvent) {
    switch (mouseEvent.type) {
        case ofMouseEventArgs::Moved:
            notifyMouseMoved(mouseEvent.x, mouseEvent.y);
            break;

        case ofMouseEventArgs::Dragged:
            notifyMouseDragged(mouseEvent.x, mouseEvent.y, mouseEvent.button);
            break;

        case ofMouseEventArgs::Pressed:
            notifyMousePressed(mouseEvent.x, mouseEvent.y, mouseEvent.button);
            break;

        case ofMouseEventArgs::Released:
            notifyMouseReleased(mouseEvent.x, mouseEvent.y, mouseEvent.button);
            break;

        case ofMouseEventArgs::Scrolled:
            notifyMouseScrolled(mouseEvent.x, mouseEvent.y, mouseEvent.scrollX, mouseEvent.scrollY);
            break;

        case ofMouseEventArgs::Entered:
            notifyMouseEntered(mouseEvent.x, mouseEvent.y);
            break;

        case ofMouseEventArgs::Exited:
            notifyMouseExited(mouseEvent.x, mouseEvent.y);
            break;
    }
}

//------------------------------------------
void ofCoreEvents::notifyMousePressed(int x, int y, int button) {
    if (bPreMouseNotSet) {
        previousMouseX = x;
        previousMouseY = y;
        bPreMouseNotSet = false;
    } else {
        previousMouseX = currentMouseX;
        previousMouseY = currentMouseY;
    }

    currentMouseX = x;
    currentMouseY = y;
    pressedMouseButtons.insert(button);

    ofMouseEventArgs mouseEventArgs(ofMouseEventArgs::Pressed, x, y, button);
    ofNotifyEvent(mousePressed, mouseEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyMouseReleased(int x, int y, int button) {
    if (bPreMouseNotSet) {
        previousMouseX = x;
        previousMouseY = y;
        bPreMouseNotSet = false;
    } else {
        previousMouseX = currentMouseX;
        previousMouseY = currentMouseY;
    }

    currentMouseX = x;
    currentMouseY = y;
    pressedMouseButtons.erase(button);

    ofMouseEventArgs mouseEventArgs(ofMouseEventArgs::Released, x, y, button);
    ofNotifyEvent(mouseReleased, mouseEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyMouseDragged(int x, int y, int button) {
    if (bPreMouseNotSet) {
        previousMouseX = x;
        previousMouseY = y;
        bPreMouseNotSet = false;
    } else {
        previousMouseX = currentMouseX;
        previousMouseY = currentMouseY;
    }

    currentMouseX = x;
    currentMouseY = y;

    ofMouseEventArgs mouseEventArgs(ofMouseEventArgs::Dragged, x, y, button);
    ofNotifyEvent(mouseDragged, mouseEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyMouseMoved(int x, int y) {
    if (bPreMouseNotSet) {
        previousMouseX = x;
        previousMouseY = y;
        bPreMouseNotSet = false;
    } else {
        previousMouseX = currentMouseX;
        previousMouseY = currentMouseY;
    }

    currentMouseX = x;
    currentMouseY = y;

    ofMouseEventArgs mouseEventArgs(ofMouseEventArgs::Moved, x, y, 0);
    ofNotifyEvent(mouseMoved, mouseEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyMouseScrolled(int x, int y, float scrollX, float scrollY) {
    ofMouseEventArgs mouseEventArgs(ofMouseEventArgs::Scrolled, x, y);
    mouseEventArgs.scrollX = scrollX;
    mouseEventArgs.scrollY = scrollY;
    ofNotifyEvent(mouseScrolled, mouseEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyMouseEntered(int x, int y) {
    ofMouseEventArgs mouseEventArgs(ofMouseEventArgs::Entered, x, y);
    ofNotifyEvent(mouseEntered, mouseEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyMouseExited(int x, int y) {
    ofMouseEventArgs mouseEventArgs(ofMouseEventArgs::Exited, x, y);
    ofNotifyEvent(mouseExited, mouseEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyExit() {
    ofNotifyEvent(exit, voidEventArgs);
}

//------------------------------------------
void ofCoreEvents::notifyWindowResized(int width, int height) {
}

//------------------------------------------
void ofSendMessage(ofMessage msg) {
}

//------------------------------------------
void ofSendMessage(string messageString) {
    ofMessage msg(messageString);
    ofSendMessage(msg);
}

#endif // endif _OF_EVENTS_

