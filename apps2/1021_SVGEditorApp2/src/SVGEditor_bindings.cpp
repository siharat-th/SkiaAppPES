//
//  PES5Template_bindings.cpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 9/8/2565 BE.
//

#include <stdio.h>
#include <string>

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "modules/canvaskit/WasmCommon.h"
#include "include/core/SkData.h"
#include "modules/pes/include/pesEngine.h"

#include "include/core/SkEncodedImageFormat.h"
#include "include/encode/SkPngEncoder.h"
#include "include/utils/SkBase64.h"
#include "include/core/SkData.h"
#include "include/core/SkScalar.h"
#include "include/core/SkSurface.h"
#include "include/core/SkVertices.h"
#include "include/pathops/SkPathOps.h"
#include "include/utils/SkTextUtils.h"

#include "modules/pes/include/pesPathUtility.hpp"
#include "SVGEditor.h"
#include "SVGDomEditor2.h"

using namespace emscripten;
using namespace std;

class SVGEditorAPI {
public:
    static void Hello(){
        SkDebugf( "Hello from SVGEditor\n" );
    }
    static void LoadSVG( long instance, const pesBuffer& data ) {
        SkDebugf("Got SVG data %li %i\n", instance, (int)data.size());
        DecSVGEditor* editor = (DecSVGEditor*)instance;
        editor->load( "", (uintptr_t)data.getData(), data.size() );
    }    
    static pesBuffer GetPNGBuffer( long instance, float dpi ) {
        SkDebugf("Get PNG Buffer %li %i\n", instance, (int)dpi);
        DecSVGEditor* editor = (DecSVGEditor*)instance;
        return editor->pSVGDomEditor->getPNGBuffer( dpi );
    }
    static pesBuffer GetPDFBuffer( long instance ) {
        SkDebugf("Get PDF Buffer %li\n", instance);
        DecSVGEditor* editor = (DecSVGEditor*)instance;
        return editor->pSVGDomEditor->getPDFBuffer();
    }
    static pesBuffer GetSVGBuffer( long instance ) {
        SkDebugf("Get SVG Buffer %li\n", instance);
        DecSVGEditor* editor = (DecSVGEditor*)instance;
        return editor->pSVGDomEditor->getSVGBuffer();
    }
    static pesBuffer GetSVGRenderBuffer( long instance ) {
        SkDebugf("Get SVG Render Buffer %li\n", instance);
        DecSVGEditor* editor = (DecSVGEditor*)instance;
        return editor->pSVGDomEditor->getSVGRenderBuffer();
    }    
};

EMSCRIPTEN_BINDINGS(SVGEditorAPI) {
    class_<SVGEditorAPI>("SVGEditorAPI")
        .class_function("Hello", &SVGEditorAPI::Hello)
        .class_function("LoadSVG", &SVGEditorAPI::LoadSVG)
        .class_function("GetPNGBuffer", &SVGEditorAPI::GetPNGBuffer)
        .class_function("GetPDFBuffer", &SVGEditorAPI::GetPDFBuffer)
        .class_function("GetSVGBuffer", &SVGEditorAPI::GetSVGBuffer)
        .class_function("GetSVGRenderBuffer", &SVGEditorAPI::GetSVGRenderBuffer);
}
