#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
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
#include "sk_ui.h"
#include "AppCreator.h"
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

using namespace sk_ui;

SkiaAppLib *app;

#if defined( SK_BUILD_FOR_IOS) || defined( SK_BUILD_FOR_ANDROID )
int expected_width = 0;
int expected_height = 0;
#else
int expected_width = 1280;
int expected_height = 720;
#endif

int WindowWidth = expected_width;
int WindowHeight = expected_height;

bool show_mouse_cursor = false;

SDL_Window *window;
int drawableWidth, drawableHeight;

static const int kStencilBits = 8;     // Skia needs 8 stencil bits
static const int kMsaaSampleCount = 0; //4;

SDL_GLContext gl_context;
int contextType;

sk_sp<const GrGLInterface> interface;
sk_sp<GrDirectContext> grContext;

GrGLint buffer;
GrGLFramebufferInfo info;
GrGLTextureInfo texInfo;

SkColorType colorType;

GrBackendRenderTarget target;
SkSurfaceProps props;

sk_sp<SkSurface> surface;
SkCanvas *canvas = NULL;

bool isQuit = false;
float fScale = 1.0;

float ddpi = 0;
float hdpi = 0;
float vdpi = 0;

float_t calculateFScale() {
#if defined( SK_BUILD_FOR_ANDROID )
    fScale = (int)(vdpi / 120);
#else
    if( WindowWidth == 0 ) {
        fScale = 1;
    }
    else {
        fScale = drawableWidth/WindowWidth;
    }
#endif
    if( fScale < 1 ) {
        fScale = 1;
    }
    return fScale;
}

//sk_sp<GrDirectContext> GrDirectContext::MakeGL(sk_sp<const GrGLInterface> glInterface);
static void handle_error()
{
    const char *error = SDL_GetError();
    printf("SDL ERROR: %s\n", error);
    SDL_ClearError();
}

skui::ModifierKey getKeyModifier() {
    skui::ModifierKey mod = skui::ModifierKey::kNone;
    SDL_Keymod m = SDL_GetModState();
    if( (m & KMOD_CTRL) ) {
        mod |= skui::ModifierKey::kControl;
    }
    if( (m & KMOD_SHIFT) ) {
        mod |= skui::ModifierKey::kShift;
    }
    if( (m & KMOD_ALT) ) {
        mod |= skui::ModifierKey::kOption;
    }
    if( (m & KMOD_GUI) ) {
        mod |= skui::ModifierKey::kCommand;
    } 
    return mod;
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
        case SDL_MOUSEBUTTONDOWN:
            app->onAppMouse( event.button.x*fScale, event.button.y*fScale, skui::InputState::kDown, skui::ModifierKey::kNone );
            break;
        case SDL_MOUSEBUTTONUP:
            app->onAppMouse( event.button.x*fScale, event.button.y*fScale, skui::InputState::kUp, skui::ModifierKey::kNone );
            break;
        case SDL_FINGERMOTION:
            app->onAppMouse( event.tfinger.x * drawableWidth, event.tfinger.y * drawableHeight, skui::InputState::kMove, skui::ModifierKey::kNone );
            break;
        case SDL_FINGERDOWN:
            app->onAppMouse( event.tfinger.x * drawableWidth, event.tfinger.y * drawableHeight, skui::InputState::kDown, skui::ModifierKey::kNone );
            break;
        case SDL_FINGERUP:
            app->onAppMouse( event.tfinger.x * drawableWidth, event.tfinger.y * drawableHeight, skui::InputState::kUp, skui::ModifierKey::kNone );
            break;
        case SDL_KEYDOWN:
        {
            SDL_Keycode key = event.key.keysym.sym;
            if (key == SDLK_KP_0)
            {
                show_mouse_cursor = !show_mouse_cursor;
                SDL_ShowCursor(show_mouse_cursor ? SDL_ENABLE : SDL_DISABLE);
            }
            else if (key == SDLK_ESCAPE)
            {
                *fQuit = true;
            }
            break;
        }
        case SDL_KEYUP:
        {
            skui::ModifierKey mod = getKeyModifier();
            SDL_Keycode key = event.key.keysym.sym;

            if( !app->onAppChar( key, mod ) ) {
            }
            break;        
        }        
        case SDL_WINDOWEVENT:
        {
            switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    SDL_GL_GetDrawableSize(window, &drawableWidth, &drawableHeight);
                    SDL_GetWindowSize( window, &WindowWidth, &WindowHeight );

                    target = GrBackendRenderTarget(drawableWidth, drawableHeight, kMsaaSampleCount, kStencilBits, info);
                    surface = SkSurface::MakeFromBackendRenderTarget(   grContext.get(),
                                                                        target,
                                                                        kBottomLeft_GrSurfaceOrigin,
                                                                        colorType, nullptr, &props);
                    canvas = surface->getCanvas();
                    
                    fScale = calculateFScale();
                    app->onAppResize(drawableWidth, drawableHeight, fScale);
                    SkDebugf( "Resize %i %i (%0.2f)\n", drawableWidth, drawableHeight, fScale );
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

void doLoop() {
    handle_events(&isQuit);

    canvas->clear(SK_ColorWHITE);

    app->onAppPrePaint();
    app->onAppPaint(*canvas);

    canvas->flush();

    sk_sp<SkImage> img = surface->makeImageSnapshot();
    

    SDL_GL_SwapWindow(window);
    SDL_Delay(1);
}

// Set the pixel format based on the colortype.
// These degrees of freedom are removed from canvaskit only to keep the interface simpler.
struct ColorSettings {
    ColorSettings(sk_sp<SkColorSpace> colorSpace) {
        if (colorSpace == nullptr || colorSpace->isSRGB()) {
            colorType = kRGBA_8888_SkColorType;
            pixFormat = GL_RGBA8;
        } else {
            colorType = kRGBA_F16_SkColorType;
            pixFormat = GL_RGBA16F;
        }
    };
    SkColorType colorType;
    GrGLenum pixFormat;
};

sk_sp<SkSurface> MakeOnScreenGLSurface(sk_sp<GrDirectContext> dContext, int width, int height,
                                       sk_sp<SkColorSpace> colorSpace) {
    // WebGL should already be clearing the color and stencil buffers, but do it again here to
    // ensure Skia receives them in the expected state.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 0);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    dContext->resetContext(kRenderTarget_GrGLBackendState | kMisc_GrGLBackendState);

    // The on-screen canvas is FBO 0. Wrap it in a Skia render target so Skia can render to it.
    GrGLFramebufferInfo info;
    info.fFBOID = 0;

    GrGLint sampleCnt;
    glGetIntegerv(GL_SAMPLES, &sampleCnt);

    GrGLint stencil;
    glGetIntegerv(GL_STENCIL_BITS, &stencil);

    const auto colorSettings = ColorSettings(colorSpace);
    info.fFormat = colorSettings.pixFormat;
    GrBackendRenderTarget target(width, height, sampleCnt, stencil, info);
    sk_sp<SkSurface> surface(SkSurface::MakeFromBackendRenderTarget(dContext.get(), target,
        kBottomLeft_GrSurfaceOrigin, colorSettings.colorType, colorSpace, nullptr));
    return surface;
}

int
main(int argc, char *argv[])
{
    SkDebugf( "======== System Info ========\n" );
    #ifdef __EMSCRIPTEN__
        SkDebugf( "Build for Emscripten\n" );
    #else
    #if defined(SK_BUILD_FOR_MAC)
        SkDebugf( "Build for MacOS\n" );
    #elif defined(SK_BUILD_FOR_WIN)
        SkDebugf( "Build for Windows\n" );
    #elif defined(SK_BUILD_FOR_UNIX)
        SkDebugf( "Build for UNIX\n" );
    #elif defined(SK_BUILD_FOR_IOS)
        SkDebugf( "Build for iOS\n" );
    #elif defined(SK_BUILD_FOR_ANDROID)
        SkDebugf( "Build for Android\n" );
    #endif
    #endif

    #if defined( SK_CPU_ARM32 )
        SkDebugf( "CPU is ARM32\n" );
    #elif defined(SK_CPU_ARM64)
        SkDebugf( "CPU is ARM64\n" );
    #elif defined(SK_CPU_X86)
        SkDebugf( "CPU is X86\n" );
    #endif

    #if defined( SK_SUPPORT_GPU ) 
        SkDebugf( "GPU is supported\n" );
    #endif
    SkDebugf( "=============================\n" );

    /* seed random number generator */
    srand((unsigned int)time(NULL));    
    
#if defined( SK_BUILD_FOR_IOS) | defined( SK_BUILD_FOR_ANDROID )
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif    

    SkDebugf( "======= Display Info ========\n" );
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    const char title[] = "Hello World";

    SDL_GetDisplayDPI( 0, &ddpi, &hdpi, &vdpi );
    SkDebugf("Display DPI: %0.2f %0.2f %0.2f\n", ddpi, hdpi, vdpi );

    // This code will create a window with the same resolution as the user's desktop.
    int numdisplays = SDL_GetNumVideoDisplays();
    SkDebugf("Number of display: %i\n", numdisplays);
    for (int i = 0; i < numdisplays; i++)
    {
        SDL_DisplayMode desktopDM;
        if (SDL_GetDesktopDisplayMode(i, &desktopDM) != 0)
        {
            handle_error();
            return 1;
        }
        SkDebugf("Display %i size: %i %i\n", i, desktopDM.w, desktopDM.h);
    }
    SkDebugf( "=============================\n" );

    SDL_DisplayMode DM;
    if (SDL_GetDesktopDisplayMode(0, &DM) != 0)
    {
        handle_error();
        return 1;
    }
    int DisplayWidth = DM.w;
    int DisplayHeight = DM.h;
    SkDebugf("Display size: %i %i\n", DisplayWidth, DisplayHeight);    

    WindowWidth = expected_width;
    WindowHeight = expected_height;
    
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE;
    if (DisplayWidth <= WindowWidth || DisplayHeight <= WindowHeight || WindowWidth == 0 || WindowHeight == 0 )
    {
        WindowWidth = DisplayWidth;
        WindowHeight = DisplayHeight;
        window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }    

    window_flags = (SDL_WindowFlags)(window_flags | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, window_flags);
    if (!window) {
        printf("Could not initialize Window\n");
        return 1;
    }
    
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);

    SDL_GL_GetDrawableSize(window, &drawableWidth, &drawableHeight);
    SDL_GetWindowSize( window, &WindowWidth, &WindowHeight );
    fScale = calculateFScale();

    //printf("Drawable size: %i %i\n", drawableWidth, drawableHeight);
    //printf("Window size: %i %i\n", WindowWidth, WindowHeight);
    //printf("Scale factor:: %0.2f\n", fScale);
    

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

    SkDebugf( "Creating target\n" );
    target = GrBackendRenderTarget(drawableWidth, drawableHeight, kMsaaSampleCount, kStencilBits, info);
    SkDebugf( "Creating surface %i %i %i\n", target.isValid(), target.width(), target.height() );

    surface = MakeOnScreenGLSurface( grContext, drawableWidth, drawableHeight, nullptr); 

    if( surface ) {
        SkDebugf( "Creating canvas\n" );
        canvas = surface->getCanvas();
    }
    else {
        SkDebugf( "Surface not valid\n" );
    }

    app = CreateSkiaAppLib();
    app->init();
    
    fScale = calculateFScale();
    app->onAppResize(drawableWidth, drawableHeight, fScale);
    SkDebugf( "Send Resize %i %i %0.2f\n", (int)drawableWidth, (int)drawableHeight, fScale );

#ifdef __EMSCRIPTEN__
    try {
        emscripten_set_main_loop(doLoop, 0, 1);
    } catch (std::exception& e) {
        std::cout << e.what() << '\n';
    }
#else    
    isQuit = false;
    while (!isQuit) 
    {
        doLoop();
    }
#endif

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
