#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

using namespace sk_ui;

SkiaAppLib *app;

#if (OPENGLES2_LOADER_GLAD == 1) 
#include <glad/gles2.h>
#endif

#if (__USE_SKIA__ == 1) && (defined(__arm__) || defined(__aarch64__)) && (!defined(__APPLE__) || !TARGET_IPHONE_SIMULATOR)
#include "include/gpu/gl/GrGLAssembleInterface.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "src/gpu/gl/GrGLUtil.h"

#include <EGL/egl.h>
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLES3/gl3.h>

static GrGLFuncPtr egl_get_gl_proc(void* ctx, const char name[]) {
    SkASSERT(nullptr == ctx);
    // https://www.khronos.org/registry/EGL/extensions/KHR/EGL_KHR_get_all_proc_addresses.txt
    // eglGetProcAddress() is not guaranteed to support the querying of non-extension EGL functions.
    #define M(X) if (0 == strcmp(#X, name)) { return (GrGLFuncPtr) X; }
    M(eglGetCurrentDisplay)
    M(eglQueryString)
    M(glActiveTexture)
    M(glAttachShader)
    M(glBindAttribLocation)
    M(glBindBuffer)
    M(glBindFramebuffer)
    M(glBindRenderbuffer)
    M(glBindTexture)
    M(glBlendColor)
    M(glBlendEquation)
    M(glBlendFunc)
    M(glBufferData)
    M(glBufferSubData)
    M(glCheckFramebufferStatus)
    M(glClear)
    M(glClearColor)
    M(glClearStencil)
    M(glColorMask)
    M(glCompileShader)
    M(glCompressedTexImage2D)
    M(glCompressedTexSubImage2D)
    M(glCopyTexSubImage2D)
    M(glCreateProgram)
    M(glCreateShader)
    M(glCullFace)
    M(glDeleteBuffers)
    M(glDeleteFramebuffers)
    M(glDeleteProgram)
    M(glDeleteRenderbuffers)
    M(glDeleteShader)
    M(glDeleteTextures)
    M(glDepthMask)
    M(glDisable)
    M(glDisableVertexAttribArray)
    M(glDrawArrays)
    M(glDrawElements)
    M(glEnable)
    M(glEnableVertexAttribArray)
    M(glFinish)
    M(glFlush)
    M(glFramebufferRenderbuffer)
    M(glFramebufferTexture2D)
    M(glFrontFace)
    M(glGenBuffers)
    M(glGenFramebuffers)
    M(glGenRenderbuffers)
    M(glGenTextures)
    M(glGenerateMipmap)
    M(glGetBufferParameteriv)
    M(glGetError)
    M(glGetFramebufferAttachmentParameteriv)
    M(glGetIntegerv)
    M(glGetProgramInfoLog)
    M(glGetProgramiv)
    M(glGetRenderbufferParameteriv)
    M(glGetShaderInfoLog)
    M(glGetShaderPrecisionFormat)
    M(glGetShaderiv)
    M(glGetString)
    M(glGetUniformLocation)
    M(glIsTexture)
    M(glLineWidth)
    M(glLinkProgram)
    M(glPixelStorei)
    M(glReadPixels)
    M(glRenderbufferStorage)
    M(glScissor)
    M(glShaderSource)
    M(glStencilFunc)
    M(glStencilFuncSeparate)
    M(glStencilMask)
    M(glStencilMaskSeparate)
    M(glStencilOp)
    M(glStencilOpSeparate)
    M(glTexImage2D)
    M(glTexParameterf)
    M(glTexParameterfv)
    M(glTexParameteri)
    M(glTexParameteriv)
    M(glTexSubImage2D)
    M(glUniform1f)
    M(glUniform1fv)
    M(glUniform1i)
    M(glUniform1iv)
    M(glUniform2f)
    M(glUniform2fv)
    M(glUniform2i)
    M(glUniform2iv)
    M(glUniform3f)
    M(glUniform3fv)
    M(glUniform3i)
    M(glUniform3iv)
    M(glUniform4f)
    M(glUniform4fv)
    M(glUniform4i)
    M(glUniform4iv)
    M(glUniformMatrix2fv)
    M(glUniformMatrix3fv)
    M(glUniformMatrix4fv)
    M(glUseProgram)
    M(glVertexAttrib1f)
    M(glVertexAttrib2fv)
    M(glVertexAttrib3fv)
    M(glVertexAttrib4fv)
    M(glVertexAttribPointer)
    M(glViewport)
    #undef M
    //return SDL_GL_GetProcAddress(name);
	return eglGetProcAddress(name);
}

sk_sp<const GrGLInterface> GrGLMakeNativeInterfaceX() {
    return GrGLMakeAssembledInterface(nullptr, egl_get_gl_proc);
}

const GrGLInterface* GrGLCreateNativeInterfaceX() { return GrGLMakeNativeInterfaceX().release(); }

#endif

#if defined( SK_BUILD_FOR_IOS)
int expected_width = 2048;
int expected_height = 1536;
#elif defined( SK_BUILD_FOR_ANDROID )
int expected_width = 0;
int expected_height = 0;
#else
int expected_width = 1440;
int expected_height = 960;
#endif
 
int WindowWidth = 0;
int WindowHeight = 0;

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

int
main(int argc, char *argv[])
{
#if defined(SK_BUILD_FOR_IOS)
    SDL_SetHint( "SDL_IOS_ORIENTATIONS", "LandscapeLeft LandscapeRight" );
#endif
    
    SkiaAppConfig *appConfig = SkiaAppConfig::getInstance();

    if( !appConfig ) {
        appConfig = new SkiaAppConfig();
    }
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
    
    const char *title = appConfig->appName.c_str();  

    SkDebugf( "======= Display Info ========\n" );
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
    
#if defined( SK_BUILD_FOR_IOS) | defined( SK_BUILD_FOR_ANDROID )
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif      

#if (defined(__arm__) || defined(__aarch64__)) && (!defined(__APPLE__) || !TARGET_IPHONE_SIMULATOR)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);    
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, kStencilBits);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);  
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, kMsaaSampleCount);     
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif

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
    SkDebugf("Display sizexx: %i %i\n", DisplayWidth, DisplayHeight);    
    /*
    if( appConfig ) {
        expected_width = appConfig->expectedWidth;
        expected_height = appConfig->expectedHeight;
    }
    */
    SkDebugf("Expected size: %i %i\n", expected_width, expected_height);    
    WindowWidth = expected_width;
    WindowHeight = expected_height;
    
#if defined( SK_BUILD_FOR_IOS)
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE;
    if (DisplayWidth <= WindowWidth || DisplayHeight <= WindowHeight || WindowWidth == 0 || WindowHeight == 0 )
    {
        WindowWidth = DisplayWidth;
        WindowHeight = DisplayHeight;
        window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
#endif  
#if defined( __FULLSCREEN__ )
    window_flags |= (SDL_WindowFlags)(window_flags | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight, window_flags);
    if (!window) {
        printf("Could not initialize Window\n");
        return 1;
    }
    
    gl_context = SDL_GL_CreateContext(window);
	
#if (OPENGLES2_LOADER_GLAD == 1) 
	int glversion = gladLoadGLES2((GLADloadfunc)SDL_GL_GetProcAddress);
    SDL_Log("Glad load version : %d\n", glversion);
	bool err = glversion == 0;

    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }   

	SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    SDL_Log("\n");
    SDL_Log("Screen bpp: %d\n", SDL_BITSPERPIXEL(mode.format));
    SDL_Log("Vendor     : %s\n", glGetString(GL_VENDOR));
    SDL_Log("Renderer   : %s\n", glGetString(GL_RENDERER));
    SDL_Log("Version    : %s\n", glGetString(GL_VERSION));
    SDL_Log("Extensions : %s\n", glGetString(GL_EXTENSIONS));
    SDL_Log("\n");	
#endif	
	
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &contextType);

    SDL_GL_GetDrawableSize(window, &drawableWidth, &drawableHeight);
    SDL_GetWindowSize( window, &WindowWidth, &WindowHeight );
    fScale = calculateFScale();

    //printf("Drawable size: %i %i\n", drawableWidth, drawableHeight);
    //printf("Window size: %i %i\n", WindowWidth, WindowHeight);
    //printf("Scale factor:: %0.2f\n", fScale);
    
#if (__USE_SKIA__ == 1) && (defined(__arm__) || defined(__aarch64__)) && (!defined(__APPLE__) || !TARGET_IPHONE_SIMULATOR)
    interface = GrGLMakeNativeInterfaceX();
#else
    interface = GrGLMakeNativeInterface();
#endif

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

    //SkPaint paint;
    //SkFont font;

    app = CreateSkiaAppLib();
    app->init();
    
    fScale = calculateFScale();
    app->onAppResize(drawableWidth, drawableHeight, fScale);
    //SkDebugf( "Send Resize %i %i %0.2f\n", (int)drawableWidth, (int)drawableHeight, fScale );
    app->onAppResize(drawableWidth, drawableHeight, fScale);

    /* Enter render loop, waiting for user to quit */
    isQuit = false;
    while (!isQuit) 
    {
        handle_events(&isQuit);

        canvas->clear(SK_ColorWHITE);

        app->onAppPrePaint();
        app->onAppPaint(*canvas);

        canvas->flush();

        SDL_GL_SwapWindow(window);
        SDL_Delay(1);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    exit( 0 );
}
