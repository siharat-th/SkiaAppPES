#include "main.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

GUI_View *topView = NULL;

#if defined(__WIN32__)
int sx = 1024;
int sy = 673;
#else
int sx = 1024;
int sy = 768;
#endif

// Tooh - save window position, size, and maximize
SDL_DisplayMode dm = {0};

void resize(int w, int h);

// MARK: - main method
int main(int argc, char *argv[]) {
#if DEBUG
    GUI_SetLogLevel(GUI_LOG_NOTICE);
#else
    GUI_SetLogLevel(GUI_LOG_SILENT);
#endif

#ifdef __EMSCRIPTEN__
    GUI_SetLogLevel(GUI_LOG_NOTICE);
#endif

    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");

#if defined(__WIN32__) || defined(__MACOSX__) // Tooh
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); //"2"="best"(Linear), "0"="low"(Nearest)
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"); // "1"=Enable vsync
#else
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); //"2"="best"(Linear), "0"="low"(Nearest)
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0"); // "1"=Enable vsync
#endif

    int ret = SDL_GUI_Init();
    if (ret != 0) {
        return ret;
    }

    GUI_CreateLandscapeWindow("SDL_gui Template", sx, sy);

    if (GUI_window == NULL) {
        GUI_LogError("Create Window") << "Error";
        return 1;
    }

#if defined(__WIN32__)
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

    int doubleBuffer;   SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doubleBuffer);
    int sampleBuffers;  SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &sampleBuffers);
    int samplesamples;  SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &samplesamples);
    int majorVersion;   SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majorVersion);
    int minorVersion;   SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minorVersion);

    topView = GUI_CreateTopView("GUI", 0, 0, GUI_windowWidth, GUI_windowHeight);
    topView->bgcol = sdl_color(0x6495edff);
    topView->setBorder(0);
    topView->setLayout(GUI_LAYOUT_VERTICAL);
    topView->handle_event_cmd = [](GUI_View *v, SDL_Event *ev) -> bool {
        if (ev->type == SDL_WINDOWEVENT) {
            if (ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                resize(ev->window.data1, ev->window.data2);
            }
        }
        return false;
    };

#ifdef __MACOSX__
#if DEBUG
    bool showFPS = true;
    GUI_Run(NULL, showFPS);
#else
    GUI_Run();
#endif
#else
    GUI_Run();
#endif
    
    GUI_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

// MARK: - Handle Window resize
void resize(int w, int h) {
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
