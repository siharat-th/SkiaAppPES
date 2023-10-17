/*
 * Copyright 2019 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "include/core/SkCanvas.h"
#include "include/core/SkData.h"
#include "include/core/SkScalar.h"
#include "include/core/SkTypes.h"
#include "include/core/SkVertices.h"
#include "modules/pes/include/UnicodeHelper.h"
#include "modules/pes/include/pesCubicSuperPath.hpp"
#include "modules/pes/include/pesEngine.h"
#include "modules/pes/include/pesPathUtility.hpp"
#include "modules/pes/include/utf8.h"

#include <string>

//#include "modules/canvaskit/WasmAliases.h"
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "modules/canvaskit/WasmCommon.h"

#if SK_SUPPORT_GPU
#include "include/gpu/GrBackendSurface.h"
//#include "include/gpu/GrContext.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/gpu/gl/GrGLTypes.h"

#include <GL/gl.h>
#include <emscripten/html5.h>
#endif

#ifndef jsPtr
#define jsPtr std::shared_ptr
#endif

#include "include/core/SkSurface.h"

using namespace emscripten;
using namespace std;

enum class PesClipperJoinType {
    SQUARE = ClipperLib::jtSquare,
    ROUND = ClipperLib::jtRound,
    MITER = ClipperLib::jtMiter
};

enum class PesClipperEndType {
    CLOSED_POLYGON = ClipperLib::etClosedPolygon,
    CLOSED_LINE = ClipperLib::etClosedLine,
    OPEN_BUTT = ClipperLib::etOpenButt,
    OPEN_SQUARE = ClipperLib::etOpenSquare,
    OPEN_ROUND = ClipperLib::etOpenRound
};

class pesUtility {
public:
    static string GetPESVersion() { return "5.101"; }
    static pesColor GetBrotherColor(int index) { return pesGetBrotherColor(index); }

    static pesPath SKPathToPesPath(const SkPath& path) { return toPes(path); }

    static SkPath PesPathToSkPath(const pesPath& path) { return toSk(path); }

    // to map from raw memory to a uint8array
    static Uint8Array PesBufferToByteArray(const pesBuffer& data) {
        return Uint8Array(typed_memory_view(data.size(), data.getData()));
    }

    static pesColor HtmlColorToPesColor(const string& str) {
        pesColor c;
        auto len = str.length();
        const char* s = str.c_str();
        unsigned int r, g, b;
        if (len == 4 && s[0] == '#') {
            if (sscanf(s + 1, "%1x%1x%1x", &r, &g, &b) == 3) {
                c.set(r, g, b);
            }
        } else if (len == 7 && s[0] == '#') {
            if (sscanf(s + 1, "%2x%2x%2x", &r, &g, &b) == 3) {
                c.set(r, g, b);
            }
        }
        return c;
    }

    static string PesColorToHtmlColor(const pesColor& c) {
        // hex to web
        return "#" + pesToHex(c.r) + pesToHex(c.g) + pesToHex(c.b);
    }

    // Tile Pattern(for Embossed fill)
    static SkPath LoadPASFromBuffer(const pesBuffer& buffer) {
        pesPath path = pesLoadPASFromBuffer(buffer);
        return toSk(path);
    }

    // Motif Pattern(for motif line/fill)
    static SkPath LoadPMFFromBuffer(const pesBuffer& buffer) {
        pesPath path = pesLoadPMFFromBuffer(buffer);
        return toSk(path);
    }

    static void sk_release_direct_surface_storage(void* pixels, void* context) {
        if (pixels == context) {
            SkDebugf("expected release context\n");
        }
        sk_free(pixels);
    }

    static sk_sp<SkSurface> MakeRasterDirectReleaseProc(int w, int h) {
        SkImageInfo imageInfo = SkImageInfo::Make(w, h, SkColorType::kRGBA_8888_SkColorType,
                                                  SkAlphaType::kUnpremul_SkAlphaType);
        size_t pixelLen = w * h * 4;                 // // it's 8888, so 4 bytes per pixel
        void* pixelPtr = sk_malloc_throw(pixelLen);  // sk_malloc_canfail(pixelLen);
        if (pixelPtr) {
            sk_sp<SkSurface> surface(SkSurface::MakeRasterDirectReleaseProc(
                    imageInfo, pixelPtr, w * 4, sk_release_direct_surface_storage, pixelPtr));
            return surface;
        }
        return nullptr;
    }

    static bool IsFloatEqual(float a, float b, float epsilon) { return fabs(a - b) <= epsilon; }
    
    static SkPath PesRunStitch(SkPath path, float runPitch){
        return pesRunStitch(path, runPitch);
    }
    
    static SkPath PesFillStitch(SkPath path, pesData::FillType fillType, float density){
        return pesFillStitch(path, fillType, density);
    }
    
    static SkPath PesSatinStitch(SkPath path, float width, float density){
        return pesSatinStitch(path, width, density);
    }
};

//vector<pesStitchBlock>
//SkPath pesStitchToSkPath(const pesStitchBlockList & blocks);
//SkPath pesRunStitch(SkPath path, float runPitch=1.0); // runPitch default 1.0 mm
//SkPath pesFillStitch(SkPath path, pesData::FillType fillType=pesData::FILL_TYPE_NORMAL, float density=2.5);
//SkPath pesSatinStitch(SkPath path, float width=1.0, float density=2.5);

class UTF8Iterator {
public:
    UTF8Iterator(const string& str) {
        utf8::replace_invalid(str.begin(), str.end(), back_inserter(src_valid));
    }
    utf8::iterator<std::string::const_iterator> begin() const {
        return utf8::iterator<std::string::const_iterator>(src_valid.begin(), src_valid.begin(),
                                                           src_valid.end());
    }
    utf8::iterator<std::string::const_iterator> end() const {
        return utf8::iterator<std::string::const_iterator>(src_valid.end(), src_valid.begin(),
                                                           src_valid.end());
    }
    utf8::iterator<std::string::const_reverse_iterator> rbegin() const {
        return utf8::iterator<std::string::const_reverse_iterator>(
                src_valid.rbegin(), src_valid.rbegin(), src_valid.rend());
    }
    utf8::iterator<std::string::const_reverse_iterator> rend() const {
        return utf8::iterator<std::string::const_reverse_iterator>(
                src_valid.rend(), src_valid.rbegin(), src_valid.rend());
    }

private:
    std::string src_valid;
};

class pesFactory {
public:
    // pesVec2f, pesVec3f
    static pesVec2f MakeVec2f() { return pesVec2f(); }

    static pesVec3f MakeVec3f() { return pesVec3f(); }

    // pesColor
    static pesColor MakeColor() { return pesColor(); }

    static pesRectangle MakeRect() { return pesRectangle(); }

    // pesPath
    static pesPath MakePath() { return pesPath(); }

    // pesPolyline
    static pesPolyline MakePolyline() { return pesPolyline(); }

    // pesData
    static pesData MakePesData() { return pesData(); }

    // pesData::Parameter
    static pesData::Parameter MakeParam() { return pesData::Parameter(); }

    // pesBuffer
    static pesBuffer MakeBuffer() { return pesBuffer(); }

    // pesStitchBlock
    static pesStitchBlock MakeStitchBlock() { return pesStitchBlock(); }

    // csp
    static pesCubicSuperPath MakeCSP() { return pesCubicSuperPath(); }
};

// function func() {
//  var ptr = callSomething(len);
//  return new Uint8Array(HEAPU8.subarray(ptr, ptr+len)); // create a new view
//}

EMSCRIPTEN_BINDINGS(Pes) {
    // MARK: pesUtility
    class_<pesUtility>("pesUtility")
            .class_function("GetPESVersion", &pesUtility::GetPESVersion)
            .class_function("GetBrotherColor", &pesUtility::GetBrotherColor)
            .class_function("SKPathToPesPath", &pesUtility::SKPathToPesPath)
            .class_function("PesPathToSkPath", &pesUtility::PesPathToSkPath)
            .class_function("PesBufferToByteArray", &pesUtility::PesBufferToByteArray)
            .class_function("HtmlColorToPesColor", &pesUtility::HtmlColorToPesColor)
            .class_function("PesColorToHtmlColor", &pesUtility::PesColorToHtmlColor)
            .class_function("LoadPASFromBuffer", &pesUtility::LoadPASFromBuffer)
            .class_function("LoadPMFFromBuffer", &pesUtility::LoadPMFFromBuffer)
            .class_function("MakeRasterDirectReleaseProc", &pesUtility::MakeRasterDirectReleaseProc)
            .class_function("IsFloatEqual", &pesUtility::IsFloatEqual)
            .class_function("PesRunStitch", &pesUtility::PesRunStitch)
            .class_function("PesFillStitch", &pesUtility::PesFillStitch)
            .class_function("PesSatinStitch", &pesUtility::PesSatinStitch);

    // MARK: pesFactory
    class_<pesFactory>("pesFactory")
            .class_function("MakeVec2f", &pesFactory::MakeVec2f)
            .class_function("MakeVec3f", &pesFactory::MakeVec3f)
            .class_function("MakeColor", &pesFactory::MakeColor)
            .class_function("MakeRect", &pesFactory::MakeRect)
            .class_function("MakePath", &pesFactory::MakePath)
            .class_function("MakePolyline", &pesFactory::MakePolyline)
            .class_function("MakePesData", &pesFactory::MakePesData)
            .class_function("MakeParam", &pesFactory::MakeParam)
            .class_function("MakeBuffer", &pesFactory::MakeBuffer)
            .class_function("MakeStitchBlock", &pesFactory::MakeStitchBlock)
            .class_function("MakeCSP", &pesFactory::MakeCSP);

    // MARK: Types
    class_<pesVec2f>("pesVec2f")
            .constructor<const pesVec3f&>()
            .function("copy",
                      optional_override([](pesVec2f& self) -> pesVec2f { return pesVec2f(self); }))
            .function("set", select_overload<void(float, float)>(&pesVec2f::set))
            .function("length", &pesVec2f::length)
            .function("lengthSquared", &pesVec2f::lengthSquared)
            .function("normalize", &pesVec2f::normalize)
            .function("perpendicular", &pesVec2f::perpendicular)
            .function("distance", &pesVec2f::distance)
            .function("getMiddle", &pesVec2f::getMiddle)
            .function("angle", &pesVec2f::angle)
            .function("dot", &pesVec2f::dot)
            .property("x", &pesVec2f::x)
            .property("y", &pesVec2f::y);

    class_<pesVec3f>("pesVec3f")
            .constructor<const pesVec2f&>()
            .function("copy",
                      optional_override([](pesVec3f& self) -> pesVec3f { return pesVec3f(self); }))
            .function("set", select_overload<void(float, float, float)>(&pesVec3f::set))
            .function("length", &pesVec3f::length)
            .function("lengthSquared", &pesVec3f::lengthSquared)
            .function("normalize", &pesVec3f::normalize)
            .function("perpendicular", &pesVec3f::perpendicular)
            .function("distance", &pesVec3f::distance)
            .function("getMiddle", &pesVec3f::getMiddle)
            .function("angle", &pesVec3f::angle)
            .function("dot", &pesVec3f::dot)
            .property("x", &pesVec3f::x)
            .property("y", &pesVec3f::y)
            .property("z", &pesVec3f::z);

    class_<pesRectangle>("pesRectangle")
            .function("copy", optional_override([](pesRectangle& self) -> pesRectangle {
                          return pesRectangle(self);
                      }))
            .function("set", select_overload<void(float, float, float, float)>(&pesRectangle::set))
            .function("setFromCenter", select_overload<void(float, float, float, float)>(
                                               &pesRectangle::setFromCenter))
            .function("growToInclude",
                      select_overload<void(float, float)>(&pesRectangle::growToInclude))
            .function("growToInclude",
                      select_overload<void(const pesRectangle&)>(&pesRectangle::growToInclude))
            .function("getCenter", &pesRectangle::getCenter)
            .function("intersects", select_overload<bool(const pesRectangle& rect) const>(
                                            &pesRectangle::intersects))
            .function("inside", &pesRectangle::inside)
            .property("x", &pesRectangle::x)
            .property("y", &pesRectangle::y)
            .property("width", &pesRectangle::width)
            .property("height", &pesRectangle::height);

    class_<pesColor>("pesColor")
            .function("copy",
                      optional_override([](pesColor& self) -> pesColor { return pesColor(self); }))
            .property("r", &pesColor::r)
            .property("g", &pesColor::g)
            .property("b", &pesColor::b)
            .property("a", &pesColor::a);

    register_vector<pesVec2f>("vector<pesVec2f>");
    class_<pesPolyline>("pesPolyline")
            .smart_ptr<jsPtr<pesPolyline>>("jsPtr<pesPolyline>")
            .function("clear", &pesPolyline::clear)
            .function("close", &pesPolyline::close)
            .function("isClosed", &pesPolyline::isClosed)
            .function("size", &pesPolyline::size)
            .function("addVertex", select_overload<void(float, float)>(&pesPolyline::addVertex))
            .function("addVertices",
                      select_overload<void(const vector<pesVec2f>&)>(&pesPolyline::addVertices))

            .function("getVertices", optional_override([](pesPolyline& self) -> vector<pesVec2f> {
                          return self.getVertices();
                      }))

            .function("simplify", &pesPolyline::simplify)
            .function("getResampledByCount", &pesPolyline::getResampledByCount2)
            .function("getBoundingBox", &pesPolyline::getBoundingBox)
            .function("getPerimeter", &pesPolyline::getPerimeter)
            .function("getArea", &pesPolyline::getArea)
            .function("isClockWise", &pesPolyline::isClockWise)
            .function("inside", optional_override([](pesPolyline& self, float x, float y) -> bool {
                          return self.inside(x, y);
                      }))
            // bool inside(float x, float y) const;
            ;

    register_vector<pesPolyline>("vector<pesPolyline>");
    register_vector<pesPath>("vector<pesPath>");
    class_<pesPath>("pesPath")
            .constructor<>()
            .function("append", &pesPath::append)
            .function("clear", &pesPath::clear)
            .function("close", &pesPath::close)

            .function("moveTo", &pesPath::moveTo)
            .function("lineTo", &pesPath::lineTo)
            .function("bezierTo", &pesPath::bezierTo)
            .function("quadBezierTo", &pesPath::quadBezierTo)

            .function("rectangle",
                      optional_override([](pesPath& self, float x, float y, float w,
                                           float h) -> void { self.rectangle(x, y, w, h); }))

            .function("simplify", &pesPath::simplify)
            .function("scale", &pesPath::scale)
            .function("translate", &pesPath::translate)
            .function("rotate", &pesPath::rotate)
            .function("rotateAround", &pesPath::rotateAround)

            .function("getBoundingBox", &pesPath::getBoundingBox)
            .function("getOutline", &pesPath::getOutline)
            .function("getSubPath", &pesPath::getSubPath)

            .function("isFill", optional_override([](pesPath& self) -> bool { return self.bFill; }))
            .function("isStroke", &pesPath::isStroke)
            .function("getStrokeWidth", &pesPath::getStrokeWidth)
            .function("getFillColor", &pesPath::getFillColor)
            .function("getStrokeColor", &pesPath::getStrokeColor)

            .function("setFilled", &pesPath::setFilled)
            .function("setStrokeWidth", &pesPath::setStrokeWidth)
            .function("setStrokeHexColor", &pesPath::setStrokeHexColor)
            .function("setFillHexColor", &pesPath::setFillHexColor)
            .function("setFillColor", &pesPath::setFillColor)
            .function("setStrokeColor", &pesPath::setStrokeColor)
            .function("inside", &pesPath::inside);

    // csp
    enum_<pesCubicSuperPath::KnotType>("PesKnotType")
            .value("KNOT_TYPE_CURVE", pesCubicSuperPath::KNOT_TYPE_CURVE)
            .value("KNOT_TYPE_CORNER", pesCubicSuperPath::KNOT_TYPE_CORNER);

    class_<pesCubicSuperPath::Knot>("PesKnot")
            .constructor<pesCubicSuperPath::KnotType, pesVec2f>()
            .property("type", &pesCubicSuperPath::Knot::type)
            .property("p", &pesCubicSuperPath::Knot::p);

    register_vector<pesCubicSuperPath::Knot>("vector<PesKnot>");
    class_<pesCubicSuperPath>("pesCubicSuperPath")
            .function("reset", &pesCubicSuperPath::reset)
            .function("calculateCSP", &pesCubicSuperPath::calculateCSP)
            .function("addCornerPoint",
                      optional_override([](pesCubicSuperPath& self, float x, float y) {
                          self.addCornerPoint(pesVec2f(x, y));
                      }))
            .function("addCurvePoint",
                      optional_override([](pesCubicSuperPath& self, float x, float y) {
                          self.addCurvePoint(pesVec2f(x, y));
                      }))
            .property("path", &pesCubicSuperPath::path)
            .property("knots", &pesCubicSuperPath::knots);

    enum_<PesClipperJoinType>("PesClipperJoinType")
            .value("SQUARE", PesClipperJoinType::SQUARE)
            .value("ROUND", PesClipperJoinType::ROUND)
            .value("MITER", PesClipperJoinType::MITER);

    enum_<PesClipperEndType>("PesClipperEndType")
            .value("CLOSED_POLYGON", PesClipperEndType::CLOSED_POLYGON)
            .value("CLOSED_LINE", PesClipperEndType::CLOSED_LINE)
            .value("OPEN_BUTT", PesClipperEndType::OPEN_BUTT)
            .value("OPEN_SQUARE", PesClipperEndType::OPEN_SQUARE)
            .value("OPEN_ROUND", PesClipperEndType::OPEN_ROUND);

    // MARK: Math
    class_<pesClipper>("pesClipper")
            .class_function("getOffsetsFromPolyline",
                            optional_override([](pesClipper& self,
                                                 const vector<pesPolyline>& polylines,
                                                 double offset,
                                                 PesClipperJoinType jointype,
                                                 PesClipperEndType endtype) -> vector<pesPolyline> {
                                return pesClipper::getOffsets(polylines, offset,
                                                              ClipperLib::JoinType(jointype),
                                                              ClipperLib::EndType(endtype));
                            }));

    class_<pesEffect>("pesEffect")
            .function("applyPaths", &pesEffect::applyPaths)
            .function("applyStitches", &pesEffect::applyStitches)
            .class_function("CreateEffect", &pesEffect::CreateEffect)
            .property("bItalic", &pesEffect::bItalic)
            .property("bCreateBorder", &pesEffect::bCreateBorder)
            .property("borderGap", &pesEffect::borderGap)
            .property("borderGapY", &pesEffect::borderGapY);

    class_<pesEffectNormal, base<pesEffect>>("pesEffectNormal").constructor<>();

    class_<pesEffectArchTop, base<pesEffect>>("pesEffectArchTop")
            .constructor<>()
            .property("angle", &pesEffectArchTop::angle)
            .property("radius", &pesEffectArchTop::radius);

    class_<pesEffectArchBottom, base<pesEffect>>("pesEffectArchBottom")
            .constructor<>()
            .property("angle", &pesEffectArchBottom::angle)
            .property("radius", &pesEffectArchBottom::radius);

    class_<pesEffectSineWave, base<pesEffect>>("pesEffectSineWave")
            .constructor<>()
            .property("magnitude", &pesEffectSineWave::magnitude);

    class_<pesEffectChevron, base<pesEffect>>("pesEffectChevron").constructor<bool>();

    class_<pesEffectCircle, base<pesEffect>>("pesEffectCircle").constructor<>();

    class_<pesEffectSlant, base<pesEffect>>("pesEffectSlant")
            .constructor<bool>()
            .property("angle", &pesEffectSlant::angle);

    class_<pesEffectFadeRight, base<pesEffect>>("pesEffectFadeRight").constructor<>();

    class_<pesEffectFadeLeft, base<pesEffect>>("pesEffectFadeLeft").constructor<>();

    class_<pesEffectFadeUp, base<pesEffect>>("pesEffectFadeUp")
            .constructor<>()
            .property("slantFactor", &pesEffectFadeUp::slantFactor);

    class_<pesEffectFadeDown, base<pesEffect>>("pesEffectFadeDown")
            .constructor<>()
            .property("slantFactor", &pesEffectFadeDown::slantFactor);

    class_<pesEffectTriangleUp, base<pesEffect>>("pesEffectTriangleUp").constructor<>();

    class_<pesEffectTriangleDown, base<pesEffect>>("pesEffectTriangleDown").constructor<>();

    class_<pesEffectInflate, base<pesEffect>>("pesEffectInflate").constructor<>();

    // MARK: Utility
    class_<pesBuffer>("pesBuffer")
            .smart_ptr_constructor("pesBuffer", &std::make_shared<pesBuffer>)
            .function("clear", &pesBuffer::clear)
            .function("size", &pesBuffer::size)

            .function("_set",
                      optional_override([](pesBuffer& self, uint8_t* cPtr, size_t len) -> void {
                          const char* data = reinterpret_cast<const char*>(cPtr);
                          self.set(data, len);
                      }),
                      allow_raw_pointers())
            .function("set", select_overload<void(const string&)>(&pesBuffer::set))

            .function("appendU32", &pesBuffer::appendU32)
            .function("appendU16", &pesBuffer::appendU16)
            .function("appendU8", select_overload<void(uint8_t)>(&pesBuffer::appendU8))
            .function("appendU8", select_overload<void(uint8_t, int)>(&pesBuffer::appendU8))

            .function("append", select_overload<void(const string&)>(&pesBuffer::append))
            .function("append",
                      optional_override([](pesBuffer& self, uintptr_t cPtr, size_t len) -> void {
                          const char* data = reinterpret_cast<const char*>(cPtr);
                          self.append(data, len);
                      }),
                      allow_raw_pointers())
            .function("append", select_overload<void(int)>(&pesBuffer::append))
            .function("append", select_overload<void(unsigned int)>(&pesBuffer::append))
            .function("append", select_overload<void(short)>(&pesBuffer::append))
            .function("append", select_overload<void(unsigned short)>(&pesBuffer::append))
            .function("append", select_overload<void(float)>(&pesBuffer::append))

            .function("getData", optional_override([](pesBuffer& self) -> val {
                          return val(typed_memory_view(
                                  self.size(), self.getData()));  // return type js ArrayBuffer
                                                                  // instead  char pointer
                      }))

            .function("getText", &pesBuffer::getText);
    ;

    // MARK: PES Data Parameter
    enum_<pesData::ObjectType>("PesObjectType")
            .value("OBJECT_TYPE_PES2_TEXT", pesData::OBJECT_TYPE_PES2_TEXT)
            .value("OBJECT_TYPE_PES", pesData::OBJECT_TYPE_PES)
            .value("OBJECT_TYPE_SHAPE", pesData::OBJECT_TYPE_SHAPE)
            .value("OBJECT_TYPE_BACKGROUND", pesData::OBJECT_TYPE_BACKGROUND)
            .value("OBJECT_TYPE_SCALABLE_SVG_FILE", pesData::OBJECT_TYPE_SCALABLE_SVG_FILE)
            .value("OBJECT_TYPE_SCALABLE_PPEF_TEXT", pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT)
            .value("OBJECT_TYPE_SCALABLE_TTF_TEXT", pesData::OBJECT_TYPE_SCALABLE_TTF_TEXT)
            .value("OBJECT_TYPE_SCALABLE_SATINCOLUMN", pesData::OBJECT_TYPE_SCALABLE_SATINCOLUMN)
            .value("OBJECT_TYPE_SCALABLE_PPEF_TEXT_V2", pesData::OBJECT_TYPE_SCALABLE_PPEF_TEXT_V2);

    enum_<pesData::StrokeType>("PesStrokeType")
            .value("STROKE_TYPE_NONE", pesData::STROKE_TYPE_NONE)
            .value("STROKE_TYPE_RUNNING", pesData::STROKE_TYPE_RUNNING)
            .value("STROKE_TYPE_RUNNING_DOUBLE", pesData::STROKE_TYPE_RUNNING_DOUBLE)
            .value("STROKE_TYPE_RUNNING_TRIPLE", pesData::STROKE_TYPE_RUNNING_TRIPLE)
            .value("STROKE_TYPE_RUNNING_BEAN", pesData::STROKE_TYPE_RUNNING_BEAN)
            .value("STROKE_TYPE_SATIN", pesData::STROKE_TYPE_SATIN)
            .value("STROKE_TYPE_SATIN_OUTER", pesData::STROKE_TYPE_SATIN_OUTER)
            .value("STROKE_TYPE_SATIN_INNER", pesData::STROKE_TYPE_SATIN_INNER)
            .value("STROKE_TYPE_MOTIF", pesData::STROKE_TYPE_MOTIF)
            .value("STROKE_TYPE_DYNAMIC", pesData::STROKE_TYPE_DYNAMIC);

    enum_<pesData::FillType>("PesFillType")
            .value("FILL_TYPE_NONE", pesData::FILL_TYPE_NONE)
            .value("FILL_TYPE_NORMAL", pesData::FILL_TYPE_NORMAL)
            .value("FILL_TYPE_PATTERN", pesData::FILL_TYPE_PATTERN)
            .value("FILL_TYPE_MOTIF", pesData::FILL_TYPE_MOTIF)
            .value("FILL_TYPE_SATIN_COLUMN", pesData::FILL_TYPE_SATIN_COLUMN)
            .value("FILL_TYPE_COLOR", pesData::FILL_TYPE_COLOR)
            .value("FILL_TYPE_DISABLE", pesData::FILL_TYPE_DISABLE)
            .value("FILL_TYPE_CROSS_STITCH", pesData::FILL_TYPE_CROSS_STITCH)
            .value("FILL_TYPE_DYNAMIC2", pesData::FILL_TYPE_DYNAMIC2)
            .value("FILL_TYPE_DYNAMIC", pesData::FILL_TYPE_DYNAMIC);

    enum_<pesData::JoinType>("PesJoinType")
            .value("JOINT_TYPE_SQUARE", pesData::JOINT_TYPE_SQUARE)
            .value("JOINT_TYPE_ROUND", pesData::JOINT_TYPE_ROUND)
            .value("JOINT_TYPE_MITER", pesData::JOINT_TYPE_MITER);

    class_<pesData::Parameter>("pesDataParameter")
            .constructor<>()
            .function("copy", optional_override([](pesData::Parameter& self) -> pesData::Parameter {
                          return pesData::Parameter(self);
                      }))
            .function("setType", &pesData::Parameter::setType)
            .property("type", &pesData::Parameter::type)
            .property("text", &pesData::Parameter::text)
            //        .property("typeLabel", &pesData::Parameter::typeLabel)
            .property("tag", &pesData::Parameter::tag)
            .property("fontIndex", &pesData::Parameter::fontIndex)
            //        .property("fontSizeIndex", &pesData::Parameter::fontSizeIndex)
            .property("colorIndex", &pesData::Parameter::colorIndex)
            .property("baselineIndex", &pesData::Parameter::baselineIndex)
            .property("radiusValue", &pesData::Parameter::radiusValue)
            .property("angleValue", &pesData::Parameter::angleValue)
            //        .property("rotateValue", &pesData::Parameter::rotateValue)
            .property("extraLetterSpace", &pesData::Parameter::extraLetterSpace)
            .property("extraSpace", &pesData::Parameter::extraSpace)
            .property("locked", &pesData::Parameter::locked)
            .property("visible", &pesData::Parameter::visible)

            // shape parameter(Legacy)
            .property("shapeIndex", &pesData::Parameter::shapeIndex)
            //        .property("width", &pesData::Parameter::width)
            //        .property("height", &pesData::Parameter::height)
            //        .property("lineWidth", &pesData::Parameter::lineWidth)
            .property("density", &pesData::Parameter::density)

            // scalable stitch parameter
            //        .property("convertToStitch", &pesData::Parameter::convertToStitch)
            .property("strokeType", &pesData::Parameter::strokeType)
            //        .property("strokeTypeIndex", &pesData::Parameter::strokeTypeIndex)
            .property("strokeRunPitch", &pesData::Parameter::strokeRunPitch)
            .property("strokeWidth", &pesData::Parameter::strokeWidth)
            .property("strokeDensity", &pesData::Parameter::strokeDensity)
            .property("strokeRunningInset", &pesData::Parameter::strokeRunningInset)
            .property("strokeJoinType", &pesData::Parameter::strokeJoinType)
            //        .property("strokeJoinTypeIndex", &pesData::Parameter::strokeJoinTypeIndex)
            .property("isSatinColumnPath", &pesData::Parameter::isSatinColumnPath)
            .property("isPPEFPath", &pesData::Parameter::isPPEFPath)
            .property("isTTFPath", &pesData::Parameter::isTTFPath)
            .property("pullCompensate", &pesData::Parameter::pullCompensate)
            //        .property("fontWeight", &pesData::Parameter::fontWeight)
            .property("bound", &pesData::Parameter::bound)
            .property("fillType", &pesData::Parameter::fillType)
            //        .property("fillTypeIndex", &pesData::Parameter::fillTypeIndex)
            .property("fillColorIndex", &pesData::Parameter::fillColorIndex)

            // pattern
            .property("motif", &pesData::Parameter::motif)
            .property("fill", &pesData::Parameter::fill)
            .property("patternFill", &pesData::Parameter::patternFill)
            .property("motifFill", &pesData::Parameter::motifFill)

            // ppef
            .property("italic", &pesData::Parameter::italic)
            .property("border", &pesData::Parameter::border)
            .property("borderColorIndex", &pesData::Parameter::borderColorIndex)
            .property("waveMagnitude", &pesData::Parameter::waveMagnitude)
            .property("slantUpAngle", &pesData::Parameter::slantUpAngle)
            .property("slantDownAngle", &pesData::Parameter::slantDownAngle)
            .property("fadeUpSlant", &pesData::Parameter::fadeUpSlant)
            .property("fadeDownSlant", &pesData::Parameter::fadeDownSlant)
            .property("ppefScaleX", &pesData::Parameter::ppefScaleX)
            .property("ppefScaleY", &pesData::Parameter::ppefScaleY)
            //        .property("lastFontSizeIndex", &pesData::Parameter::lastFontSizeIndex)
            .property("borderGap", &pesData::Parameter::borderGap)
            .property("borderGapY", &pesData::Parameter::borderGapY)
            //        .property("ppefReverseChar", &pesData::Parameter::ppefReverseChar)
            //        .property("ppefReverseSegment", &pesData::Parameter::ppefReverseSegment)
            .property("rotateDegree", &pesData::Parameter::rotateDegree)

            // pes5
            .property("fontName", &pesData::Parameter::fontName)
            .property("fontSize", &pesData::Parameter::fontSize)
            .property("lastFontSize", &pesData::Parameter::lastFontSize)
            .property("useColorFromPicker", &pesData::Parameter::useColorFromPicker)
            .property("colors", &pesData::Parameter::colors);

    value_object<pesData::Parameter::motif_t>("motifParam")
            .field("size", &pesData::Parameter::motif_t::size)
            .field("spacing", &pesData::Parameter::motif_t::spacing)
            .field("runPitch", &pesData::Parameter::motif_t::runPitch)
            .field("patternIndex", &pesData::Parameter::motif_t::patternIndex)
            .field("patternName", &pesData::Parameter::motif_t::patternName)
            .field("patternBuffer", &pesData::Parameter::motif_t::patternBuffer);

    value_object<pesData::Parameter::fill_t>("fillParam")
            .field("underlay", &pesData::Parameter::fill_t::underlay)
            .field("density", &pesData::Parameter::fill_t::density)
            .field("sewDirection", &pesData::Parameter::fill_t::sewDirection)
            .field("presetIndex", &pesData::Parameter::fill_t::presetIndex);

    value_object<pesData::Parameter::patternFill_t>("patternFillParam")
            .field("underlay", &pesData::Parameter::patternFill_t::underlay)
            .field("density", &pesData::Parameter::patternFill_t::density)
            .field("sewDirection", &pesData::Parameter::patternFill_t::sewDirection)
            .field("patternIndex", &pesData::Parameter::patternFill_t::patternIndex)
            .field("patternName", &pesData::Parameter::patternFill_t::patternName)
            .field("patternSize", &pesData::Parameter::patternFill_t::patternSize)
            .field("patternBuffer", &pesData::Parameter::patternFill_t::patternBuffer);

    value_object<pesData::Parameter::motifFill_t>("motifFillParam")
            .field("size", &pesData::Parameter::motifFill_t::size)
            .field("spacing", &pesData::Parameter::motifFill_t::spacing)
            .field("rowSpacing", &pesData::Parameter::motifFill_t::rowSpacing)
            .field("patternIndex", &pesData::Parameter::motifFill_t::patternIndex)
            .field("patternName", &pesData::Parameter::motifFill_t::patternName)
            .field("patternBuffer", &pesData::Parameter::motifFill_t::patternBuffer);

    // MARK: PES Data
    register_vector<pesStitchBlock>("vector<pesStitchBlock>");
    register_vector<uint8_t>("vector<uint8_t>");
    register_vector<int8_t>("vector<int8_t>");
    class_<pesData>("pesData")
            .constructor<>()
            .smart_ptr<jsPtr<pesData>>("jsPtr<pesData>")
            .function("clear", &pesData::clear)
            .function("clearPaths", optional_override([](pesData& self) {
                          for (auto& p : self.paths) {
                              p.clear();
                          }
                          self.paths.clear();
                      }))
            .function("loadPES2TextFromBuffer",
                      select_overload<bool(const pesBuffer&, const pesBuffer&, string, bool)>(
                              &pesData::loadPES2TextFromBuffer))
            .function("loadPESFromBuffer",
                      select_overload<bool(const pesBuffer&, bool)>(&pesData::loadPESFromBuffer))
            .function("loadPINNFromBuffer", select_overload<bool(const pesBuffer&, bool, bool)>(
                                                    &pesData::loadPINNFromBuffer))
            .function("loadSVGFromBuffer",
                      select_overload<bool(const pesBuffer&, bool)>(&pesData::loadSVGFromBuffer))
            .function("loadBackgroundFromBuffer",
                      select_overload<bool(const pesBuffer&, bool)>(&pesData::loadBackgroundFromBuffer))
            .function("loadPPEFFromSVGBuffer", select_overload<bool(const pesBuffer&, bool)>(
                                                       &pesData::loadPPEFFromSVGBuffer))
            .function("loadTTFFromSVGBuffer",
                      select_overload<bool(const pesBuffer&, bool)>(&pesData::loadTTFFromSVGBuffer))
            .function("loadSatinColumnFromSVGBuffer", &pesData::loadSatinColumnFromSVGBuffer)

            .function("isSameType", &pesData::isSameType)
            .function("canMerge", &pesData::canMerge)
            .function("merge", &pesData::merge)
            .function("calcBoundingBox", &pesData::calcBoundingBox)
            .function("recalculate", &pesData::recalculate)
            .function("translate", &pesData::translate)
            .function("scale", &pesData::scale)
            .function("scaleStitches", &pesData::scaleStitches)
            .function("rotate", &pesData::rotate)
            .function("applyFill", &pesData::applyFill)
            .function("applyPPEFFill", &pesData::applyPPEFFill)
            .function("applyPPEF_V2_Fill", &pesData::applyPPEF_V2_Fill)
            .function("applyStroke", &pesData::applyStroke)
            .function("isScalable", &pesData::isScalable)

            //        .function("getStitchBlockList", &pesData::getStitchBlockList)
            .function("getStitchBlockList",
                      optional_override([](pesData& self) -> vector<pesStitchBlock> {
                          vector<pesStitchBlock> blocks;
                          self.getStitchBlockList(blocks);
                          return blocks;
                      }))

            .function("getTotalStitches", &pesData::getTotalStitches)
            .function("getTotalJump", &pesData::getTotalJump)
            .function("getBoundingBox", &pesData::getBoundingBox)
            .function("getScalableBoundingBox", &pesData::getScalableBoundingBox)
            .function("getSVGBuffer", &pesData::getSVGBuffer)
            .function("getRotatedBoundingBox", &pesData::getRotatedBoundingBox)
    
            .function("canFlip", &pesData::canFlip)
            .function("horizontalFlip", &pesData::horizontalFlip)
            .function("verticalFlip", &pesData::verticalFlip)
            .function("saveParameter", &pesData::saveParameter)
            .function("savePaths", &pesData::savePaths)

            .property("parameter", &pesData::parameter)
            .property("savedParameter", &pesData::savedParameter)
            .property("savedPaths", &pesData::savedPaths)
            .property("numColorChange", &pesData::numColorChange)
            .property("colors", &pesData::colors)
            .property("paths", &pesData::paths)
            .property("fillBlocks", &pesData::fillBlocks)
            .property("strokeBlocks", &pesData::strokeBlocks);

    // MARK: PES Data Stitch
    class_<pesStitchBlock>("pesStitchBlock")
            .smart_ptr<jsPtr<pesStitchBlock>>("jsPtr<pesStitchBlock>")
            .function("addStitch", select_overload<void(float, float)>(&pesStitchBlock::addStitch))
            .function("addJump",
                      select_overload<void(float, float, bool)>(&pesStitchBlock::addJump))
            .function("clear", &pesStitchBlock::clear)

            .function("scale", select_overload<void(float, float)>(&pesStitchBlock::scale))
            .function("rotate", &pesStitchBlock::rotate)
            .function("rotateAround", &pesStitchBlock::rotateAround)
            .function("translate", select_overload<void(float, float)>(&pesStitchBlock::translate))

            .function("removeLast", &pesStitchBlock::removeLast)
            .function("lastIsJump", &pesStitchBlock::lastIsJump)
            .function("lastIsEnd", &pesStitchBlock::lastIsEnd)
            .function("isJump", &pesStitchBlock::isJump)

            .function("size", &pesStitchBlock::size)
            .function("getTotalStitches", &pesStitchBlock::getTotalStitches)
            .function("getTotalJumps", &pesStitchBlock::getTotalJumps)

            .function("setNearestColor", &pesStitchBlock::setNearestColor)
            .function("setColorFromIndex", &pesStitchBlock::setColorFromIndex)

            .property("colorIndex", &pesStitchBlock::colorIndex)
            .property("color", &pesStitchBlock::color)
            .property("types", &pesStitchBlock::types)
            .property("polyline", &pesStitchBlock::polyline)
    
            .function("getColor", &pesStitchBlock::getColor)
            .function("getPolyline", &pesStitchBlock::getPolyline);

    // MARK: PES Document
    register_vector<shared_ptr<pesData>>("vector<shared_ptr<pesData>>");
    class_<pesDocument>("pesDocument")
            .class_function("getInstance", &pesDocument::getInstance, allow_raw_pointers())
            .class_function("createInstance", &pesDocument::createInstance, allow_raw_pointers())
                   
            .function("getAppName", &pesDocument::getAppName)
            .function("setAppName", &pesDocument::setAppName)
            .function("getName", &pesDocument::getName)
            .function("setName", &pesDocument::setName)
            .function("newDocument", &pesDocument::newDocument)
            .function("loadPPESFromBuffer", &pesDocument::loadPPESFromBuffer)
            .function("loadPPESFromMemory", &pesDocument::loadPPESFromMemory)
            .function("loadPPES",
                      optional_override([](pesDocument& self, const std::string& data) -> bool {
                          return self.loadPPESFromBuffer(pesBuffer(data.c_str(), data.size()));
                      }))
            .function("loadPES",
                      optional_override([](pesDocument& self, const std::string& data) -> bool {
                          pesData pes;
                          if (pes.loadPESFromBuffer(pesBuffer(data.c_str(), data.size()))) {
                              self.addObject(pes);
                              return true;
                          }
                          return false;
                      }))
            .function("addObject", optional_override([](pesDocument& self, pesData& data) -> int {
                          return self.addObject(data, false);
                      }))
            .function("addObjectToXY", optional_override([](pesDocument& self, pesData& data, float x, float y) -> int {
                          return self.addObjectToXY(data, x, y, false);
                      }))
            .function("addObjectNolimit", optional_override([](pesDocument& self, pesData& data) -> int {
                          return self.addObject(data, true);
                      }))
            .function("addObjectToXYNolimit", optional_override([](pesDocument& self, pesData& data, float x, float y) -> int {
                          return self.addObjectToXY(data, x, y, true);
                      }))
            .function("classifyObject", &pesDocument::classifyObject)
            .function("clipPathForObject", &pesDocument::clipPathForObject)

            .function("moveObjectBack", &pesDocument::moveObjectBack)
            .function("moveObjectFront", &pesDocument::moveObjectFront)
            .function("mergeObject", &pesDocument::mergeObject)
            .function("duplicateObject", &pesDocument::duplicateObject)
            .function("deleteObject", &pesDocument::deleteObject)
    
            .function("mergeAll", &pesDocument::mergeAll)

            .function("getDataObjects", select_overload<const vector<shared_ptr<pesData>>&()const>(&pesDocument::getDataObjects))
            .function("getDataObject", &pesDocument::getDataObject)
            .function("getObjectCount", &pesDocument::getObjectCount)
            .function("getDataParameter", &pesDocument::getDataParameter)

            .function("setDataParameterText", &pesDocument::setDataParameterText)
            .function("setDataParameterFont", &pesDocument::setDataParameterFont)
            .function("setDataParameterFontSize", &pesDocument::setDataParameterFontSize)
            .function("setDataParameterTextEffect", &pesDocument::setDataParameterTextEffect)
            .function("setDataParameterTextEffectAngle", &pesDocument::setDataParameterTextEffectAngle)
            .function("setDataParameterTextEffectRadius", &pesDocument::setDataParameterTextEffectRadius)
            .function("setDataParameterFillColor", &pesDocument::setDataParameterFillColor)
            .function("setDataParameterFillTypeIndex", &pesDocument::setDataParameterFillTypeIndex)
            .function("setDataParameterStrokeColor", &pesDocument::setDataParameterStrokeColor)
            .function("setDataParameterStrokeTypeIndex", &pesDocument::setDataParameterStrokeTypeIndex)
            .function("setDataParameterTextBorder", &pesDocument::setDataParameterTextBorder)
            .function("setDataParameterTextBorderGapX", &pesDocument::setDataParameterTextBorderGapX)
            .function("setDataParameterTextBorderGapY", &pesDocument::setDataParameterTextBorderGapY)
            .function("setDataParameterTextBorderColor", &pesDocument::setDataParameterTextBorderColor)
            .function("setDataParameterTextItalic", &pesDocument::setDataParameterTextItalic)
            .function("setDataParameterTextDensity", &pesDocument::setDataParameterTextDensity)
            .function("setDataParameterTextPullCompensate", &pesDocument::setDataParameterTextPullCompensate)
    
            .function("setDataParameterTextExtraLetterSpace", &pesDocument::setDataParameterTextExtraLetterSpace)
            .function("setDataParameterTextExtraSpace", &pesDocument::setDataParameterTextExtraSpace)

            .function("setDataParameterStrokeRunPitch", &pesDocument::setDataParameterStrokeRunPitch)
            .function("setDataParameterStrokeWidth", &pesDocument::setDataParameterStrokeWidth)
            .function("setDataParameterStrokeDensity", &pesDocument::setDataParameterStrokeDensity)
            .function("setDataParameterStrokeRunningInset", &pesDocument::setDataParameterStrokeRunningInset)
    
            .function("setDataParameterStrokeMotifSize", &pesDocument::setDataParameterStrokeMotifSize)
            .function("setDataParameterStrokeMotifSpacing", &pesDocument::setDataParameterStrokeMotifSpacing)
    
            .function("setDataParameterFillDensity", &pesDocument::setDataParameterFillDensity)
            .function("setDataParameterFillDirection", &pesDocument::setDataParameterFillDirection)
    
            .function("setDataParameterFillPatternDensity", &pesDocument::setDataParameterFillPatternDensity)
            .function("setDataParameterFillPatternDirection", &pesDocument::setDataParameterFillPatternDirection)
            .function("setDataParameterFillPatternSize", &pesDocument::setDataParameterFillPatternSize)
    
            .function("setDataParameterFillMotifSize", &pesDocument::setDataParameterFillMotifSize)
            .function("setDataParameterFillMotifSpacing", &pesDocument::setDataParameterFillMotifSpacing)
            .function("setDataParameterFillMotifRowSpacing", &pesDocument::setDataParameterFillMotifRowSpacing)

//            .function("getLayerTypeString", &pesDocument::getLayerTypeString)
//            .function("getLayerImageString", &pesDocument::getLayerImageString)
//            .function("selectLayer", &pesDocument::selectLayer)

            .function("setHoopSizeInMM", select_overload<void(float, float)>(&pesDocument::setHoopSizeInMM))
            .function("getHoopSizeInMM", &pesDocument::getHoopSizeInMM)
            .function("setShowJumpStitch", &pesDocument::setShowJumpStitch)
            .function("setDrawStitchTexture", &pesDocument::setDrawStitchTexture)
                     
            .function("setTrimAtLength", &pesDocument::setTrimAtLength)
            .function("getTrimAtLength", &pesDocument::getTrimAtLength)

            .function("getPPESBuffer", &pesDocument::getPPESBuffer)
            .function("getPES1Buffer", &pesDocument::getPES1Buffer)
            .function("getPES5Buffer", &pesDocument::getPES5Buffer)
            .function("getPINNBuffer", &pesDocument::getPINNBuffer)
            .function("getDSTBuffer", &pesDocument::getDSTBuffer)
            .function("getEXPBuffer", &pesDocument::getEXPBuffer)
            .function("getJEFBuffer", &pesDocument::getJEFBuffer)
            .function("getXXXBuffer", &pesDocument::getXXXBuffer)
            .function("getGCodeBuffer", &pesDocument::getGCodeBuffer)
//            .function("getDXFBuffer", &pesDocument::getDXFBuffer)
            .function("getPNGBuffer", &pesDocument::getPNGBuffer)
            .function("getThumbnailPNGBuffer", &pesDocument::getThumbnailPNGBuffer)
            .function("getPreviewPNGBuffer", &pesDocument::getPreviewPNGBuffer)
            .function("getPNG", optional_override([](pesDocument &self) -> Uint8Array {
                          pesBuffer buff = self.getPNGBuffer();
                          return emscripten::val(typed_memory_view(buff.size(), (uint8_t*)buff.getData())).call<Uint8Array>("slice");
                      }))
            .function("getThumbnailPNG", optional_override([](pesDocument &self) -> Uint8Array {
                          pesBuffer buff = self.getThumbnailPNGBuffer();
                          return emscripten::val(typed_memory_view(buff.size(), (uint8_t*)buff.getData())).call<Uint8Array>("slice");
                      }))
            .function("getThumbnailPNG", optional_override([](pesDocument &self, int wmax200, int hmax200, int index) -> Uint8Array {
                          pesBuffer buff = self.getThumbnailPNGBuffer(wmax200, hmax200, index);
                          return emscripten::val(typed_memory_view(buff.size(), (uint8_t*)buff.getData())).call<Uint8Array>("slice");
                      }))
            .function("getPreviewPNG", optional_override([](pesDocument& self) -> Uint8Array {
                          pesBuffer buff = self.getPreviewPNGBuffer();
                          return emscripten::val(typed_memory_view(buff.size(), (uint8_t*)buff.getData())).call<Uint8Array>("slice");
                      }))
            .function("getPesData", optional_override([](pesDocument& self, int index) -> emscripten::val {
                          pesData* p = nullptr;
                          if (index < self.getObjectCount()) {
                              p = self.getDataObject(index).get();
                          }
                          return emscripten::val(p);
                      }))

            .function("isDirty", &pesDocument::isDirty)
            .function("setDirty", &pesDocument::setDirty)

            .function("makePesScalableImageSnapshot", &pesDocument::makePesScalableImageSnapshot)
            .function("makePesImageSnapshot", &pesDocument::makePesImageSnapshot)
            .function("getPesMaxObjectCount", &pesDocument::getPesMaxObjectCount)
            .function("setPesMaxObjectCount", &pesDocument::setPesMaxObjectCount)


            ;
}
