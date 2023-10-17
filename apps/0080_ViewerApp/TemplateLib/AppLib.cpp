#include "AppLib.h"
#include "../src/MyWidgetLayer.h"

namespace sk_ui {

TemplateAppLib *TemplateAppLib::getInstance() {
    return (TemplateAppLib *)pInstance;
}

TemplateAppLib::TemplateAppLib()
{
    pushLayer( this );
    pushLayer( new MyWidgetLayer("json/app_template.json") );
    pushLayer( new FPSLayer() );
    
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
}

TemplateAppLib::~TemplateAppLib() {
}

void TemplateAppLib::onPaint( SkCanvas& canvas ) {
    canvas.clear(SK_ColorBLACK);
}

}