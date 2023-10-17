/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#include "App.h"
#include "../SkiaApp.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkSurface.h"

sk_ui::SkiaAppLib * CreateSkiaAppLib();

using namespace sk_app;
using namespace sk_ui;

SkiaAppLib *appLib = NULL;

Application* Application::Create(int argc, char** argv, void* platformData) {
    return new HelloWorldApp(argc, argv, platformData);
}

HelloWorldApp::HelloWorldApp(int argc, char** argv, void* platformData)
#if defined(APP_NAME)
        : title(APP_NAME)
#else
        : title("Hello World")
#endif
        , fBackendType(Window::kNativeGL_BackendType)
        , fScaleFactor( 1.0 )
{
    SkGraphics::Init();

    fWindow = Window::CreateNativeWindow(platformData);
    fWindow->setRequestedDisplayParams(DisplayParams());

    SkiaAppConfig* appConfig = SkiaAppConfig::getInstance();

    if (!appConfig) {
        appConfig = new SkiaAppConfig();
    }
    appConfig->appName = title.c_str();

    fWindow->setTitle(appConfig->appName.c_str());

    appLib = CreateSkiaAppLib();
    appLib->AppName = title.c_str();
    appLib->init();

    // register callbacks
    fWindow->pushLayer(this);

    fWindow->attach(fBackendType);

    onResize( fWindow->width(), fWindow->height() );
    //appLib->onAppResize( fWindow->width(), fWindow->height(), 2.0 ); // fWindow->scaleFactor() );
    
    //SkDebugf( "Window: %0.2f %i %i\n", fWindow->scaleFactor(), fWindow->width(), fWindow->height() );
}

HelloWorldApp::~HelloWorldApp() {
    delete( appLib );

    fWindow->detach();
    delete fWindow;
}

void HelloWorldApp::focus(bool f) { 
    appLib->onAppFocus(f); 
}

void HelloWorldApp::updateTitle() {
    if (!fWindow || fWindow->sampleCount() < 1) {
        return;
    }

    SkString tt(this->title.c_str());
    tt.append(Window::kRaster_BackendType == fBackendType ? " Raster" : " OpenGL");
    fWindow->setTitle(tt.c_str());
}

void HelloWorldApp::onBackendCreated() {
    this->updateTitle();
    fWindow->show();
    fWindow->inval();
}

void HelloWorldApp::onAttach(sk_app::Window* window) {
    // We not implement anything related to sk_app::Window which not available on Web
}

void HelloWorldApp::onResize(int w, int h) {
    // Sending scaleFactor will make app to response to retina/non-retina correctly
    fScaleFactor = fWindow->scaleFactor();
    iWidth = w;
    iHeight = h;
#if defined(SK_BUILD_FOR_ANDROID)
    fScaleFactor = 2.0;
#elif defined(SK_BUILD_FOR_UNIX)
    if( iWidth > 1920 ) {
        fScaleFactor = 2.0;
    }
#endif

    appLib->onAppResize( w, h, fScaleFactor );
    //SkDebugf( "--Scale %i, %i, %0.2f\n", iWidth, iHeight, fScaleFactor );
}

void HelloWorldApp::onPrePaint() {
    appLib->onAppPrePaint();
}

void HelloWorldApp::onPaint(SkSurface* surface) {
    auto canvas = surface->getCanvas();    
    appLib->onAppPaint( *canvas );
}

void HelloWorldApp::onIdle() {
    // Just re-paint continously
#if defined(SK_BUILD_FOR_ANDROID)    
    if( iWidth != fWindow->width() || iHeight != fWindow->height() ) {
        fWindow->detach();
        fWindow->attach(fBackendType);
        onResize( fWindow->width(), fWindow->height() );
    }  
#else
    if( fScaleFactor != fWindow->scaleFactor() || iWidth != fWindow->width() || iHeight != fWindow->height() ) {
        fWindow->detach();
        fWindow->attach(fBackendType);
        onResize( fWindow->width(), fWindow->height() );
    }    
#endif
    fWindow->inval();
}

bool HelloWorldApp::onChar(SkUnichar c, skui::ModifierKey modifiers) {
    if( !appLib->onAppChar( c, modifiers ) ) {
        if (' ' == c) {
            fBackendType = Window::kRaster_BackendType == fBackendType ? Window::kNativeGL_BackendType
                                                                    : Window::kRaster_BackendType;
            fWindow->detach();
            fWindow->attach(fBackendType);
            onResize( fWindow->width(), fWindow->height() );
        }
    }
    return true;
}

bool HelloWorldApp::onKey(skui::Key key, skui::InputState state, skui::ModifierKey modifiers ) { 
    return appLib->onAppKey( key, state, modifiers );
}

bool HelloWorldApp::onMouse(int x, int y, skui::InputState state, skui::ModifierKey modifiers ) { 
    return appLib->onAppMouse( x, y, state, modifiers );
}

bool HelloWorldApp::onMouseWheel(float deltax, float delta, skui::ModifierKey modifiers) { 
    return appLib->onAppMouseWheel( deltax, delta, modifiers ); 
}

bool HelloWorldApp::onTouch(intptr_t owner, skui::InputState state, float x, float y) { 
    return appLib->onAppTouch( owner, state, x, y );
}

bool HelloWorldApp::onDropFile(const char* dropped_file, uintptr_t iptr, size_t length) {
    return appLib->onAppDropFile(dropped_file, iptr, length);
}

bool HelloWorldApp::onFling(skui::InputState state) { 
    return false; 
}

bool HelloWorldApp::onPinch(skui::InputState state, float scale, float x, float y) { 
    return false; 
}

void HelloWorldApp::onUIStateChanged(const SkString& stateName, const SkString& stateValue) {
    // Not implemented
}

