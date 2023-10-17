/*
* Copyright 2017 Google Inc.
*
* Use of this source code is governed by a BSD-style license that can be
* found in the LICENSE file.
*/

#ifndef HelloWorldApp_DEFINED
#define HelloWorldApp_DEFINED

#include "tools/sk_app/Application.h"
#include "tools/sk_app/Window.h"

class SkCanvas;

class HelloWorldApp : public sk_app::Application, sk_app::Window::Layer {
public:
    HelloWorldApp(int argc, char** argv, void* platformData);
    ~HelloWorldApp() override;

    virtual void focus(bool f) override;

    void onIdle() override;
    

    void onBackendCreated() override;
    void onAttach(sk_app::Window* window) override;
    void onResize(int width, int height) override;    
    void onPrePaint() override;
    void onPaint(SkSurface*) override;
    bool onChar(SkUnichar c, skui::ModifierKey modifiers) override;
    bool onKey(skui::Key, skui::InputState, skui::ModifierKey) override;
    bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    bool onMouseWheel(float deltax, float delta, skui::ModifierKey) override;
    bool onTouch(intptr_t owner, skui::InputState, float x, float y) override;
    bool onDropFile(const char* dropped_file, uintptr_t /* uint8_t*  */ iptr = 0, size_t length = 0) override;

    // Platform-detected gesture events
    bool onFling(skui::InputState state) override;
    bool onPinch(skui::InputState state, float scale, float x, float y) override;
    void onUIStateChanged(const SkString& stateName, const SkString& stateValue) override;

private:
    void updateTitle();

    SkString title;
    sk_app::Window* fWindow;
    sk_app::Window::BackendType fBackendType;

    float fScaleFactor;
    int     iWidth;
    int     iHeight;
};

#endif
