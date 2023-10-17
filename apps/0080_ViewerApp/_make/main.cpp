// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_sdl_opengl3/ folder**
// See imgui_impl_sdl.cpp for details.

#include <stdio.h>
#include <SDL.h>
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "src/gpu/gl/GrGLUtil.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkColor.h"
#include "include/core/SkSurface.h"
#include "include/utils/SkRandom.h"
#include "include/core/SkSwizzle.h"
#include "include/core/SkVertices.h"

#include "AppLib.h"

int expected_width = 1024;
int expected_height = 600;

int WindowWidth = expected_width;
int WindowHeight = expected_height;

bool touchDown = false;
bool isQuit = false;
float fScale = 1.0;

SDL_Window *window;
int drawableWidth, drawableHeight;

static const int kStencilBits = 8;     // Skia needs 8 stencil bits
static const int kMsaaSampleCount = 0; //4;
SDL_GLContext gl_context;
int contextType;
sk_sp<const GrGLInterface> interface;
sk_sp<GrDirectContext> grContext;
GrGLint buffer;
SkColorType colorType;
GrGLFramebufferInfo info;
GrBackendRenderTarget target;
SkSurfaceProps props;

sk_sp<SkSurface> surface;
SkCanvas *canvas = NULL;

using namespace sk_ui;

TemplateAppLib *app;

//sk_sp<GrDirectContext> GrDirectContext::MakeGL(sk_sp<const GrGLInterface> glInterface);
static void handle_error()
{
    const char *error = SDL_GetError();
    printf("SDL ERROR: %s\n", error);
    SDL_ClearError();
}

static void handle_events(bool *fQuit)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_MOUSEMOTION:
            app->onAppMouse( event.button.x*fScale, event.button.y*fScale, skui::InputState::kMove, skui::ModifierKey::kNone );
            break;
        case SDL_FINGERMOTION:
            break;
        case SDL_MOUSEBUTTONDOWN:
            app->onAppMouse( event.button.x*fScale, event.button.y*fScale, skui::InputState::kDown, skui::ModifierKey::kNone );
            break;
        case SDL_MOUSEBUTTONUP:
            app->onAppMouse( event.button.x*fScale, event.button.y*fScale, skui::InputState::kUp, skui::ModifierKey::kNone );
            break;
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
            break;
        case SDL_KEYDOWN:
        {
            SDL_Keycode key = event.key.keysym.sym;
            if (key == SDLK_ESCAPE)
            {
                *fQuit = true;
            }
            break;
        }
        case SDL_WINDOWEVENT:
        {
            switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    SDL_GL_GetDrawableSize(window, &drawableWidth, &drawableHeight);
                    SDL_GetWindowSize( window, &WindowWidth, &WindowHeight );
                    fScale = drawableWidth/WindowWidth;

                    SkDebugf( "Resize %i %i (%0.2f)\n", WindowWidth, WindowHeight, fScale );
                    target = GrBackendRenderTarget(drawableWidth, drawableHeight, kMsaaSampleCount, kStencilBits, info);
                    surface = SkSurface::MakeFromBackendRenderTarget(   grContext.get(), 
                                                                        target,
                                                                        kBottomLeft_GrSurfaceOrigin,
                                                                        colorType, nullptr, &props);
                    canvas = surface->getCanvas();                    
                    app->onAppResize(drawableWidth, drawableHeight, fScale);
                    break;                    
            }    
            break;
        }
        case SDL_QUIT:
            *fQuit = true;
            break;
        default:
            break;
        }
    }
}

// Main code
int main(int, char **)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    printf("Skia\n");

    const char title[] = "Hello World";

    float ddpi;
    float hdpi;
    float vdpi;
    SDL_GetDisplayDPI( 0, &ddpi, &hdpi, &vdpi );
    printf("Display DPI: %0.2f %0.2f %0.2f\n", ddpi, hdpi, vdpi );

    SDL_DisplayMode DM;
    if (SDL_GetDesktopDisplayMode(0, &DM) != 0)
    {
        handle_error();
        return 1;
    }
    int DisplayWidth = DM.w;
    int DisplayHeight = DM.h;
    printf("Display size: %i %i\n", DisplayWidth, DisplayHeight);

    WindowWidth = expected_width;
    WindowHeight = expected_height;
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE;
    if (DisplayWidth <= WindowWidth || DisplayHeight <= WindowHeight)
    {
        WindowWidth = DisplayWidth;
        WindowHeight = DisplayHeight;
        window_flags = SDL_WINDOW_FULLSCREEN;
    }

    // This code will create a window with the same resolution as the user's desktop.
    int numdisplays = SDL_GetNumVideoDisplays();
    printf("Number of display: %i\n", numdisplays);
    for (int i = 0; i < numdisplays; i++)
    {
        SDL_DisplayMode desktopDM;
        if (SDL_GetDesktopDisplayMode(i, &desktopDM) != 0)
        {
            handle_error();
            return 1;
        }
        printf("Display %i size: %i %i\n", i, desktopDM.w, desktopDM.h);
    }

    // Setup window
    window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, window_flags);




    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);

    SDL_GL_GetDrawableSize(window, &drawableWidth, &drawableHeight);
    printf("Drawable size: %i %i\n", drawableWidth, drawableHeight);


    interface = GrGLMakeNativeInterface();
    SkASSERT(interface);

    // setup contexts
    grContext = GrDirectContext::MakeGL(interface);
    SkASSERT(grContext);

    // Wrap the frame buffer object attached to the screen in a Skia render target so Skia can
    // render to it
    GR_GL_GetIntegerv(interface.get(), GR_GL_FRAMEBUFFER_BINDING, &buffer);

    info.fFBOID = (GrGLuint)buffer;
    
    uint32_t windowFormat = SDL_GetWindowPixelFormat(window);
    if (SDL_PIXELFORMAT_RGBA8888 == windowFormat)
    {
        info.fFormat = GR_GL_RGBA8;
        colorType = kRGBA_8888_SkColorType;
    }
    else
    {
        colorType = kBGRA_8888_SkColorType;
        if (SDL_GL_CONTEXT_PROFILE_ES == contextType)
        {
            info.fFormat = GR_GL_BGRA8;
        }
        else
        {
            // We assume the internal format is RGBA8 on desktop GL
            info.fFormat = GR_GL_RGBA8;
        }
    }

    target = GrBackendRenderTarget(drawableWidth, drawableHeight, kMsaaSampleCount, kStencilBits, info);
    surface = SkSurface::MakeFromBackendRenderTarget(   grContext.get(), 
                                                        target,
                                                        kBottomLeft_GrSurfaceOrigin,
                                                        colorType, nullptr, &props);
    canvas = surface->getCanvas();


    SkPaint paint;
    SkFont font;

    app = new TemplateAppLib();
    fScale = drawableWidth/WindowWidth;
    #if defined( SK_BUILD_FOR_ANDROID)
        fScale = 3.0;
        WindowWidth = drawableWidth/3.0;
        WindowHeight = drawableHeight/3.0;
    #endif
    SkDebugf( "Send Resize %i %i %0.2f\n", (int)drawableWidth, (int)drawableHeight, fScale );
    app->onAppResize(drawableWidth, drawableHeight, fScale);

    // Main loop
    while (!isQuit)
    {

        handle_events(&isQuit);

        canvas->clear(SK_ColorWHITE);

        app->onAppPrePaint();

        int w, h;
        SDL_GL_GetDrawableSize(window, &drawableWidth, &drawableHeight);
        SDL_GetWindowSize( window, &w, &h );
        float fs = drawableWidth/w;
        if( fs != fScale || w != WindowWidth || h != WindowHeight ) {
            fScale = fs;
            WindowWidth = w;
            WindowHeight = h;
            app->onAppResize( WindowWidth, WindowHeight, fScale );
        }

        app->onAppPaint(*canvas);

        canvas->flush();

        /* update screen */
        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
