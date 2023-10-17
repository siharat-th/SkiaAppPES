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
#include "DecUComposer.h"

using namespace emscripten;
using namespace std;

// toBytes returns a Uint8Array that has a copy of the data in the given SkData.
Uint8Array toBytes(sk_sp<SkData> data);

EMSCRIPTEN_BINDINGS(DecUComposer) {
    class_<DecUComposer>("DecUComposer")
        .class_function("getInstance", &DecUComposer::getInstance, allow_raw_pointers())
        .function( "setDarkMode", &DecUComposer::setDarkMode)
        .function( "OpenSVGFile", &DecUComposer::OpenSVGFile)
        .function( "LoadSVG", &DecUComposer::LoadSVG)
        .function( "SaveSVGFile", &DecUComposer::SaveSVGFile)
        .function( "GetSVGBytes", optional_override([](DecUComposer &c, int index) -> Uint8Array {
            sk_sp<SkData> data = c.GetSVGData( index );
            if (!data) {
                return emscripten::val::null();
            }
            return toBytes(data);
        }))
        .function( "ExportSVGFile", &DecUComposer::ExportSVGFile)
        .function( "GetSVGRenderBytes", optional_override([](DecUComposer &c, int index) -> Uint8Array {
            sk_sp<SkData> data = c.GetSVGRenderData( index );
            if (!data) {
                return emscripten::val::null();
            }
            return toBytes(data);
        }))        
        .function( "ExportPNGFile", &DecUComposer::ExportPNGFile)
        .function( "GetPNGBytes", optional_override([](DecUComposer &c, int index, int dpi=72) -> Uint8Array {
            sk_sp<SkData> data = c.GetPNGData( index, dpi );
            if (!data) {
                return emscripten::val::null();
            }
            return toBytes(data);
        }))
        .function( "GetGroupList", &DecUComposer::GetGroupList )
        .function( "DisplayPartType", &DecUComposer::DisplayPartType );
}
