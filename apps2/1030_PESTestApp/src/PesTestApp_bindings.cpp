//
//  PesTestApp_bindings.cpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 9/8/2565 BE.
//

#include <stdio.h>
#include <string>

#include "PESTestLayer.h"

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "modules/canvaskit/WasmCommon.h"

using namespace emscripten;
using namespace std;

class PesTestApp{
public:
    static void UpdateImages(){
        sk_ui::PesTestApp_UpdateImages();
    }
    
    static void UpdateTextImage(){
        sk_ui::PesTestApp_UpdateTextImage();
    }
};

EMSCRIPTEN_BINDINGS(PesTestApp) {
    class_<PesTestApp>("PesTestApp")
            .class_function("UpdateImages", &PesTestApp::UpdateImages)
            .class_function("UpdateTextImage", &PesTestApp::UpdateTextImage);
}
