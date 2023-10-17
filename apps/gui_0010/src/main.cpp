#include "main.h"
#include <SDL.h>
#include <SDL_timer.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>  // std::max
#include <chrono>
#include <ctime>
#include <iostream>
#include <iostream>  // std::cout
#include <string>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "HTTP_Utils.hpp"
#include "SDLSmartTexture.hpp"
#include "happyhttp.h"

#include "include/core/SkCanvas.h"

using namespace std;

GUI_View* topView = NULL;

GUI_ToolBar* topBar = NULL;
GUI_View* documentView = NULL;
GUI_ToolBar* bottomBar = NULL;

#if defined(WIN32)
int sx = 1024;
int sy = 673;
int min_win_w = sx;
int min_win_h = sy;
#else
int sx = 1024;
int sy = 768;
int min_win_w = sx;
int min_win_h = 723;  // sy; - macbook air - Jimmy
#endif

// Tooh - save window position, size, and maximize
SDL_DisplayMode dm = {0};

static const char* menuTextDesigner[] = {
        "New",
        "Open...",
        "Open Recent",
        "---",
        "Save",
        "Send As...",
        "---",
        "Import",
        "Export",
        "---",
        "Exit",
};

#if defined(WIN32)
#include <shlobj.h>  // for SHGetFolderPath()
HWND hwnd = NULL;    // Tooh - Open and Save File Dialog
HMENU hmenu = NULL;  // Tooh - text selection: contextual menu
#endif

#if defined(__MACOSX__) || defined(__EMSCRIPTEN__) || defined(_RPi) || defined(WIN32)
string currentOutputPath = "";
vector<string> currentParameters;
int currentExportSize = -1;
#endif

string currentOpenPath = "";
string currentOpenFile = "";
void setCurrentOpenFile(string filepath);

#if __USE_SKIA__
sk_sp<SkImage> exImage = nullptr;
#endif

//#include "ofURLFileLoader.hpp"
// MARK: - main method
int main(int argc, char* argv[]) {
    srand(time(NULL));

#if defined(WIN32)
    // Tooh - read argv when user dobble-click on .ppes or .pes file
    currentOpenFile = "";
    currentOpenPath = "";

    printf("argc = %d\n", argc);

    if (argc > 1 && argv) {
        if (argv[0]) {
            printf("argv[0] = %s\n", argv[0]);
            string argv0(argv[0]);
            currentOpenPath = argv0.substr(0, argv0.find_last_of("\\"));
            SetCurrentDirectory(currentOpenPath.c_str());
        }

        if (argv[1]) {
            printf("argv[1] = %s\n", argv[1]);
            string argv1(argv[1]);
            currentOpenFile = argv1;
            currentOpenPath = argv1.substr(0, argv1.find_last_of("\\"));
            printf("currentOpenFile = %s\n", currentOpenFile.c_str());
        }
    }

    printf("currentOpenPath = %s\n", currentOpenPath.c_str());
#endif

    __use_font_awesome_texture = false;

#if DEBUG
    GUI_SetLogLevel(GUI_LOG_NOTICE);
#else
    GUI_SetLogLevel(GUI_LOG_SILENT);
#endif

#ifdef __EMSCRIPTEN__
    GUI_SetLogLevel(GUI_LOG_NOTICE);
#endif

    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");

#if defined(WIN32) || defined(__MACOSX__)             // Tooh
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");  //"2"="best"(Linear), "0"="low"(Nearest)
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");          // "1"=Enable vsync
#else
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");  //"2"="best"(Linear), "0"="low"(Nearest)
#endif

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");  // "1"=Enable vsync

    static const int kStencilBits = 8;      // Skia needs 8 stencil bits
    static const int kMsaaSampleCount = 0;  // 4;

    // Setup SDL_GL Attribute
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#if defined(SK_BUILD_FOR_ANDROID) || defined(SK_BUILD_FOR_IOS)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, kStencilBits);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

#if HAS_HARFBUZZ
    bool has_harfbuzz = true;
#else
    bool has_harfbuzz = false;
#endif

#if HAS_TESSELATOR
    bool has_tess = true;
#else
    bool has_tess = false;
#endif

#if HAS_TLS
    bool has_tls = true;
#else
    bool has_tls = false;
#endif
#if DEBUG
    // GUI_LogNotice("Platform Specific", "HAS_HARFBUZZ: %i  HAS_TESSELATOR: %i  HAS_TLS: %i",
    // has_harfbuzz, has_tess, has_tls);
#endif
    int ret = SDL_GUI_Init();
    if (ret != 0) {
        return ret;
    }

#if defined(WIN32) || defined(__MACOSX__)
    sx = 1024;
    sy = 768;
#elif defined(__IPHONEOS__) || defined(__ANDROID__)
    int sx = 720, sy = 480;
#else
    int sx = 1024, sy = 768;
#endif

    GUI_CreateLandscapeWindow("SDL-gui", sx, sy);

    if (GUI_window == NULL) {
        GUI_LogError("Create Window") << "Error";
        return 1;
    }

#if defined(WIN32) || defined(__MACOSX__)
    // pom
    // min height should be 723 (MacBook Air) - Jimmy
    SDL_SetWindowMinimumSize(GUI_window, min_win_w, min_win_h);
#endif

#if defined(WIN32)
    // Tooh - get window handle
    SDL_SysWMinfo info = {0};
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(GUI_window, &info);
    hwnd = info.info.win.window;

    // Tooh - set window icon
    HICON hicon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hicon);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hicon);
#endif

    int doubleBuffer;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doubleBuffer);
    int sampleBuffers;
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &sampleBuffers);
    int samplesamples;
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &samplesamples);
    int majorVersion;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majorVersion);
    int minorVersion;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minorVersion);

#if __USE_SKIA__
    // pom
    // Create SkSurface from frame buffer object attached to the screen so Skia can render to it
    SDL_RenderSetViewport(GUI_renderer, NULL);
    SDL_RenderSetClipRect(GUI_renderer, NULL);

    SDL_SetRenderDrawColor(GUI_renderer, 0x64, 0x95, 0xed, 0xff);
    SDL_RenderClear(GUI_renderer);

    GUI_glContext = SDL_GL_CreateContext(GUI_window);
    SDL_GL_MakeCurrent(GUI_window, GUI_glContext);
    uint32_t windowFormat = SDL_GetWindowPixelFormat(GUI_window);
    int contextType;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);
    int dw, dh;
    SDL_GL_GetDrawableSize(GUI_window, &dw, &dh);
    auto interface = GrGLMakeNativeInterface();  // const GrGLInterface*
    SkASSERT(interface);
    sk_sp<GrDirectContext> grContext(GrDirectContext::MakeGL(interface));

    skutil sku;
    GrGLint buffer;
    sku.GrGLGetFrameBuffer(interface, &buffer);
    GrGLFramebufferInfo info;
    info.fFBOID = (GrGLuint)buffer;
    SkColorType colorType;
    if (SDL_PIXELFORMAT_RGBA8888 == windowFormat) {
        info.fFormat = GR_GL_RGBA8;
        colorType = kRGBA_8888_SkColorType;
    } else {
        colorType = kBGRA_8888_SkColorType;
        if (SDL_GL_CONTEXT_PROFILE_ES == contextType) {
            info.fFormat = GR_GL_BGRA8;
        } else {
            // We assume the internal format is RGBA8 on desktop GL
            info.fFormat = GR_GL_RGBA8;
            colorType = kRGBA_8888_SkColorType;
        }
    }
    GrBackendRenderTarget target(dw, dh, kMsaaSampleCount, kStencilBits, info);
    SkSurfaceProps props;
    sk_sp<SkSurface> surface(SkSurface::MakeFromBackendRenderTarget(
            grContext.get(), target, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, &props));

    GUI_canvas = surface->getCanvas();
#endif

    topView = GUI_CreateTopView("GUI", 0, 0, GUI_windowWidth, GUI_windowHeight);
    topView->bgcol = sdl_color(0xA0A0A0FF);  // 0x6495edff
    topView->setBorder(0);
    topView->setLayout(GUI_LAYOUT_VERTICAL);

    topBar = new GUI_ToolBar(topView, GUI_BAR_TOP, NULL, 44);
    topBar->bgcol = sdl_color(0x3b3f44ff);
    topBar->titleColor = cWhite;
    if (topBar) {
        auto btMainMenu = topBar->addButton("", "", 1, GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER);
        btMainMenu->titleColor = cWhite;
        btMainMenu->bgcol = cClear;
        btMainMenu->color_down = sdl_color(0x338dfbff);
        btMainMenu->setPadding(0, 10, 0, 10);
        btMainMenu->setIcon(
                GUI_ButtonFA::createTextureFormUnicode(kIcon_bars, NULL, NULL, cWhite, 1));

        static GUI_List* mainMenu = NULL;
        btMainMenu->action_cmd = [=](GUI_Button* bt) {
            SDL_StopTextInput();  // Tooh - text selection
            if (mainMenu == NULL) {
                int cellh = 36;
                int numCells = sizeof(menuTextDesigner) / sizeof(menuTextDesigner[0]);
                int h = (numCells - 3) * cellh + 36;
                int maxh = min(h,
                               GUI_windowHeight - topBar->tw_area.h / GUI_scale -
                                       bottomBar->tw_area.h / GUI_scale);
                mainMenu = new GUI_List(topView,
                                        menuTextDesigner,
                                        numCells,
                                        0,
                                        topBar->tw_area.h,
                                        240,
                                        maxh,
                                        cellh,
                                        false,
                                        NULL,
                                        0);
                mainMenu->setAlignment(GUI_ALIGN_ABSOLUTE);
                mainMenu->setPosition(0, topBar->getViewHeight());
                mainMenu->handle_event_cmd = [](GUI_View* v, SDL_Event* ev) -> bool {
                    switch (ev->type) {
                        case SDL_WINDOWEVENT: {
                            if (ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                                mainMenu->hide();
                                GUI_mouseCapturedWindow = NULL;
                            }
                            break;
                        }
                        case SDL_MOUSEBUTTONDOWN: {
                            int ex, ey;
                            ex = ev->button.x;
                            ey = ev->button.y;

                            int x = (int)(ex * GUI_mouseScale);
                            int y = (int)(ey * GUI_mouseScale);

                            if (!mainMenu->hitTest(x, y)) {
                                mainMenu->hide();
                                GUI_mouseCapturedWindow = NULL;
                                return false;
                            }
                        }
                    }

                    return false;
                };
            }
            if (mainMenu) {
                mainMenu->show();
                GUI_mouseCapturedWindow = mainMenu;
            }
        };

        auto labelTitle = topBar->addLabel("SDL-gui 2.0", "", GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER);
        labelTitle->titleColor = cWhite;

        auto btnRight = topBar->addButton("", "", 32121, GUI_ALIGN_VCENTER | GUI_ALIGN_RIGHT);
        btnRight->setIcon(
                GUI_ButtonFA::createTextureFormUnicode(kIcon_indent, NULL, NULL, cWhite, 1));
        btnRight->setPadding(0, 10, 0, 10);
        btnRight->bgcol = cClear;
        btnRight->color_down = sdl_color(0x338dfbff);
        btnRight->setMargin(0, 10, 0, 0);
    }

    documentView = new GUI_View(topView, "Main", 0, 0, -1, -1, cDarkGrey, [](GUI_View* view) {});

    bottomBar = new GUI_ToolBar(topView, GUI_BAR_BOTTOM, NULL, 20);
    bottomBar->bgcol = sdl_color(0xe3e3e3ff);
    bottomBar->borderColor = sdl_color(0xa0a0a0ff);
    if (bottomBar) {
        auto labelInfo = bottomBar->addLabel("", "", GUI_ALIGN_LEFT | GUI_ALIGN_TOP);
        labelInfo->titleColor = sdl_color(0x787878ff);
        labelInfo->tw_area.w = GUI_windowWidth;
        labelInfo->setTitle(string("Hello world."), 2);

        auto labelVersion = bottomBar->addLabel("", "", GUI_ALIGN_RIGHT | GUI_ALIGN_TOP);
        labelVersion->setTitle(string("Version: 0.01"), 2);
        labelVersion->titleColor = sdl_color(0x787878ff);
    }

#ifdef DEBUG
#if defined(__MACOSX__)
    bool showFPS = true;
    GUI_Run(NULL, showFPS);
#else
    GUI_Run(NULL);
#endif
#else
#if defined(__ANDROID__)
    bool showFPS = false;
    GUI_Run(NULL, showFPS);
#else
    GUI_Run(NULL);
#endif
#endif

    /* shutdown SDL */
    LSTR_clear();
    GUI_ButtonFA::Quit();

    GUI_DeleteFontAwesomeTexture();

    if (GUI_glContext) {
        SDL_GL_DeleteContext(GUI_glContext);
    }

    GUI_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

// MARK: - Handle Window resize
void resize(int w, int h) {
    GUI_LogNotice() << w << ", " << h << "\n";
    int oldHeight = GUI_windowHeight * GUI_scale;
    int oldWidth = GUI_windowWidth * GUI_scale;

    GUI_SetScaleParameters();
    GUI_windowWidth = (GUI_physicalWindowWidth) / GUI_scale;
    GUI_windowHeight = (GUI_physicalWindowHeight) / GUI_scale;

    w = GUI_windowWidth * GUI_scale;
    h = GUI_windowHeight * GUI_scale;
    topView->tw_area.w = w;
    topView->tw_area.h = h;
    topView->updateLayout();
}
