//
//  PesTracingUtils.hpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 29/9/2565 BE.
//

#ifndef PesTracingUtils_hpp
#define PesTracingUtils_hpp

#include <stdio.h>
#include "potracelib.h"
#include "trace/trace.hpp"
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"

struct GrayMap_def;
typedef GrayMap_def GrayMap;

typedef enum
{
    TRACE_BRIGHTNESS,
    TRACE_BRIGHTNESS_MULTI,
    TRACE_CANNY,
    TRACE_QUANT,
    TRACE_QUANT_COLOR,
    TRACE_QUANT_MONO
} TraceType;

class PreviewResult{
public:
    PreviewResult(){
        preview = nullptr;
    }
    
    sk_sp<SkImage> preview;
};

class PesTracingUtils : public TracingEngine
{
    
public:
    
    PesTracingUtils();
    
    ~PesTracingUtils();
    
    
    void setParamsTurdSize(int val)
    {
        potraceParams->turdsize = val;
    }
    int getParamsTurdSize()
    {
        return potraceParams->turdsize;
    }
    
    void setParamsAlphaMax(double val)
    {
        potraceParams->alphamax = val;
    }
    double getParamsAlphaMax()
    {
        return potraceParams->alphamax;
    }
    
    void setParamsOptiCurve(bool val)
    {
        potraceParams->opticurve = val;
    }
    bool getParamsOptiCurve()
    {
        return potraceParams->opticurve;
    }
    
    void setParamsOptTolerance(double val)
    {
        potraceParams->opttolerance = val;
    }
    double getParamsOptTolerance()
    {
        return potraceParams->opttolerance;
    }
    
    void setTraceType(TraceType val)
    {
        traceType = val;
    }
    TraceType getTraceType()
    {
        return traceType;
    }
    
    /**
     * Sets/gets whether I invert the product of the other filter(s)
     */
    void setInvert(bool val)
    {
        invert = val;
    }
    bool getInvert()
    {
        return invert;
    }
    
    /**
     * Sets the halfway point for black/white
     */
    void setQuantizationNrColors(int val)
    {
        quantizationNrColors = val;
    }
    
    int getQuantizationNrColors()
    {
        return quantizationNrColors;
    }
    
    /**
     * Sets the halfway point for black/white
     */
    void setBrightnessThreshold(double val)
    {
        brightnessThreshold = val;
    }
    double getBrightnessThreshold()
    {
        return brightnessThreshold;
    }
    
    /**
     * Sets the lower consideration point for black/white
     */
    void setBrightnessFloor(double val)
    {
        brightnessFloor = val;
    }
    double getBrightnessFloor()
    {
        return brightnessFloor;
    }
    
    /**
     * Sets upper cutoff for canny non-maximalizing
     */
    void setCannyHighThreshold(double val)
    {
        cannyHighThreshold = val;
    }
    double getCannyHighThreshold()
    {
        return cannyHighThreshold;
    }
    
    /**
     * Sets the number of colors for quant multiscan
     */
    void setMultiScanNrColors(int val)
    {
        multiScanNrColors = val;
    }
    int getMultiScanNrColors()
    {
        return multiScanNrColors;
    }
    
    /**
     * Sets whether we tile regions side-by-side or stack them
     */
    void setMultiScanStack(bool val)
    {
        multiScanStack = val;
    }
    bool getMultiScanStack()
    {
        return multiScanStack;
    }
    
    /**
     * Sets whether we want gaussian smoothing of bitmaps before quantizing
     */
    void setMultiScanSmooth(bool val)
    {
        multiScanSmooth = val;
    }
    bool getMultiScanSmooth()
    {
        return multiScanSmooth;
    }
    
    /**
     * Sets whether we want to remove the background (bottom) trace
     */
    void setMultiScanRemoveBackground(bool val)
    {
        multiScanRemoveBackground= val;
    }
    bool getMultiScanRemoveBackground()
    {
        return multiScanRemoveBackground;
    }
    
    
    /**
     *  This is the working method of this implementing class, and all
     *  implementing classes.  Take a GdkPixbuf, trace it, and
     *  return the path data that is compatible with the d="" attribute
     *  of an SVG <path> element.
     */
//    virtual std::vector<TracingEngineResult> trace( Glib::RefPtr<Gdk::Pixbuf> pixbuf);
    virtual std::vector<TracingEngineResult> trace( const SkBitmap & bitmap, PreviewResult* preview=NULL);
    
    /**
     *  Abort the thread that is executing getPathDataFromPixbuf()
     */
    virtual void abort();
    
    int keepGoing;
    
    std::vector<TracingEngineResult>traceGrayMap(GrayMap *grayMap);
    
    GrayMap *filterSurf(const SkBitmap & bitmap);
    SkBitmap *filterIndexedTosurface(const SkBitmap & bitmap);
    
private:
    
    potrace_param_t *potraceParams;
    TraceType traceType;
    
    //## do I invert at the end?
    bool invert;
    
    //## Color-->b&w quantization
    int quantizationNrColors;
    
    //## brightness items
    double brightnessThreshold;
    double brightnessFloor; //usually 0.0
    
    //## canny items
    double cannyHighThreshold;
    
    //## Color-->multiscan quantization
    int multiScanNrColors;
    bool multiScanStack; //do we tile or stack?
    bool multiScanSmooth;//do we use gaussian filter?
    bool multiScanRemoveBackground; //do we remove the bottom trace?
    /**
     * This is the actual wrapper of the call to Potrace.  nodeCount
     * returns the count of nodes created.  May be NULL if ignored.
     */
    pesPath grayMapToPath(GrayMap *gm, long *nodeCount);
    
//    std::vector<TracingEngineResult>traceBrightnessMulti(GdkPixbuf *pixbuf);
//    std::vector<TracingEngineResult>traceQuant(GdkPixbuf *pixbuf);
//    std::vector<TracingEngineResult>traceSingle(GdkPixbuf *pixbuf);
    
    std::vector<TracingEngineResult>traceBrightnessMulti(const SkBitmap & bitmap, PreviewResult* preview=NULL);
    std::vector<TracingEngineResult>traceQuant(const SkBitmap & bitmap, PreviewResult* preview=NULL);
    std::vector<TracingEngineResult>traceSingle(const SkBitmap & bitmap, PreviewResult* preview=NULL);

    
    
};

#endif /* PesTracingUtils_hpp */
