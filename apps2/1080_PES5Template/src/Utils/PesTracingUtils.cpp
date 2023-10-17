//
//  PesTracingUtils.cpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 29/9/2565 BE.
//

#include "PesTracingUtils.hpp"
#include "trace/filterset.hpp"
#include "trace/quantize.hpp"
#include "trace/bitmap.h"
#include "trace/imagemap-skia.hpp"

static void potraceStatusCallback(double progress, void *userData) /* callback fn */
{
//    updateGui();
    
    if (!userData)
        return;
    PesTracingUtils *engine = (PesTracingUtils*)userData;
}

PesTracingUtils::PesTracingUtils() :
keepGoing(1),
traceType(TRACE_BRIGHTNESS),
invert(false),
quantizationNrColors(8),
brightnessThreshold(0.45),
brightnessFloor(0),
cannyHighThreshold(0.65),
multiScanNrColors(2),
multiScanStack(false),
multiScanSmooth(false),
multiScanRemoveBackground(true)
{
    /* get default parameters */
    potraceParams = potrace_param_default();
    potraceParams->turdsize = 25;
    potraceParams->progress.callback = potraceStatusCallback;
    potraceParams->progress.data = (void *)this;
}

PesTracingUtils::~PesTracingUtils()
{
    potrace_param_free(potraceParams);
}



typedef struct
{
    double x;
    double y;
} dPoint;


/**
 * Check a point against a list of points to see if it
 * has already occurred.
 */
static bool hasPoint(std::vector<dPoint> &points, double x, double y)
{
    for (unsigned int i=0; i<points.size() ; i++)
    {
        dPoint p = points[i];
        if (p.x == x && p.y == y)
            return true;
    }
    return false;
}


static long writePaths(PesTracingUtils *engine, potrace_path_t *plist,
                       pesPath & data, std::vector<dPoint> &points)
{
    long nodeCount = 0L;
    
    potrace_path_t *node;
    for (node=plist; node ; node=node->sibling)
    {
        potrace_curve_t *curve = &(node->curve);
        //g_message("node->fm:%d\n", node->fm);
//        GUI_LogNotice("writePaths", "node->fm:%d", node->fm);
        
        if (!curve->n)
            continue;
        const potrace_dpoint_t *pt = curve->c[curve->n - 1];
        double x0 = 0.0;
        double y0 = 0.0;
        double x1 = 0.0;
        double y1 = 0.0;
        double x2 = pt[2].x;
        double y2 = pt[2].y;
        //Have we been here already?
        if (hasPoint(points, x2, y2))
        {
            //g_message("duplicate point: (%f,%f)\n", x2, y2);
//            GUI_LogWarning("duplicate point", "%f,%f", x2, y2);
//            GUI_LogNotice("duplicate point", "node area:%d", node->area);
            continue;
        }
        else
        {
            if(node->area <= engine->getParamsTurdSize()){
                // area of largest path to be ignored
                SkDebugf("ignored node area:%d\n", node->area);
                continue;
            }
            dPoint p;
            p.x = x2; p.y = y2;
            points.push_back(p);
//            GUI_LogNotice("push point", "node area:%d", node->area);
        }
        data.moveTo((float)x2, (float)y2);
        nodeCount++;
        
        for (int i=0 ; i<curve->n ; i++)
        {
            if (!engine->keepGoing)
                return 0L;
            pt = curve->c[i];
            x0 = pt[0].x;
            y0 = pt[0].y;
            x1 = pt[1].x;
            y1 = pt[1].y;
            x2 = pt[2].x;
            y2 = pt[2].y;
            switch (curve->tag[i])
            {
                case POTRACE_CORNER:
                    data.lineTo((float)x1, (float)y1);
                    data.lineTo((float)x2, (float)y2);
                    break;
                case POTRACE_CURVETO:
                    data.bezierTo(pesVec2f((float)x0, (float)y0), pesVec2f((float)x1, (float)y1), pesVec2f((float)x2, (float)y2));
                    break;
                default:
                    break;
            }
            nodeCount++;
        }
//        GUI_LogNotice("writePaths()", "num contour=%lu", data.getOutline().size());
        data.close();
        
        for (potrace_path_t *child=node->childlist; child ; child=child->sibling)
        {
            nodeCount += writePaths(engine, child, data, points);
        }
    }
    
    return nodeCount;
    
}

static GrayMap *filter(PesTracingUtils &engine, const SkBitmap & surf) //GdkPixbuf * pixbuf
{
    if (surf.drawsNothing())
        return NULL;
    
    GrayMap *newGm = NULL;
    
    /*### Color quantization -- banding ###*/
    if (engine.getTraceType() == TRACE_QUANT)
    {
        RgbMap *rgbmap = SkBitmapToRgbMap( surf);
        //rgbMap->writePPM(rgbMap, "rgb.ppm");
        newGm = quantizeBand(rgbmap, engine.getQuantizationNrColors());
        rgbmap->destroy(rgbmap);
        //return newGm;
    }
    
    /*### Brightness threshold ###*/
    else if ( engine.getTraceType() == TRACE_BRIGHTNESS ||
             engine.getTraceType() == TRACE_BRIGHTNESS_MULTI )
    {
        GrayMap *gm = SkBitmapToGrayMap( surf);
        
        newGm = GrayMapCreate(gm->width, gm->height);
        double floor =  3.0 *
        ( engine.getBrightnessFloor() * 256.0 );
        double cutoff =  3.0 *
        ( engine.getBrightnessThreshold() * 256.0 );
        for (int y=0 ; y<gm->height ; y++)
        {
            for (int x=0 ; x<gm->width ; x++)
            {
                double brightness = (double)gm->getPixel(gm, x, y);
                if (brightness >= floor && brightness < cutoff)
                    newGm->setPixel(newGm, x, y, GRAYMAP_BLACK);  //black pixel
                else
                    newGm->setPixel(newGm, x, y, GRAYMAP_WHITE); //white pixel
            }
        }
        
        gm->destroy(gm);
        //newGm->writePPM(newGm, "brightness.ppm");
        //return newGm;
    }
    
    /*### Canny edge detection ###*/
    else if (engine.getTraceType() == TRACE_CANNY)
    {
        GrayMap *gm = SkBitmapToGrayMap( surf);
        newGm = grayMapCanny(gm, 0.1, engine.getCannyHighThreshold());
        gm->destroy(gm);
        //newGm->writePPM(newGm, "canny.ppm");
        //return newGm;
    }
    
    /*### Do I invert the image? ###*/
    if (newGm && engine.getInvert())
    {
        for (int y=0 ; y<newGm->height ; y++)
        {
            for (int x=0 ; x<newGm->width ; x++)
            {
                unsigned long brightness = newGm->getPixel(newGm, x, y);
                brightness = 765 - brightness;
                newGm->setPixel(newGm, x, y, brightness);
            }
        }
    }
    
    return newGm;//none of the above
}

GrayMap* PesTracingUtils::filterSurf(const SkBitmap & surf){
    return filter(*this, surf);
}



static IndexedMap *filterIndexed(PesTracingUtils &engine, const SkBitmap & pixbuf) //GdkPixbuf * pixbuf
{
    if (pixbuf.drawsNothing())
        return NULL;
    
    IndexedMap *newGm = NULL;
    
    RgbMap *gm = SkBitmapToRgbMap(pixbuf);
    if (engine.getMultiScanSmooth())
    {
        RgbMap *gaussMap = rgbMapGaussian(gm);
        newGm = rgbMapQuantize(gaussMap, engine.getMultiScanNrColors());
        gaussMap->destroy(gaussMap);
    }
    else
    {
        newGm = rgbMapQuantize(gm, engine.getMultiScanNrColors());
    }
    gm->destroy(gm);
    
    if (engine.getTraceType() == TRACE_QUANT_MONO)
    {
        //Turn to grays
        for (int i=0 ; i<newGm->nrColors ; i++)
        {
            RGB rgb = newGm->clut[i];
            int grayVal = (rgb.r + rgb.g + rgb.b) / 3;
            rgb.r = rgb.g = rgb.b = grayVal;
            newGm->clut[i] = rgb;
        }
    }
    
    return newGm;
}

SkBitmap* PesTracingUtils::filterIndexedTosurface(const SkBitmap & pixbuf){
    IndexedMap *iMap = filterIndexed(*this, pixbuf);
    if ( iMap ) {
        SkBitmap* surf = IndexedMapToSkBitmap(iMap);
        iMap->destroy( iMap );
        return surf;
    }
    return NULL;
}



//*This is the core inkscape-to-potrace binding
pesPath PesTracingUtils::grayMapToPath(GrayMap *grayMap, long *nodeCount)
{
    pesPath path;
    if (!keepGoing)
    {
        SkDebugf("grayMapToPath(): abort\n");
        return path;
    }
    
    potrace_bitmap_t *potraceBitmap = bm_new(grayMap->width, grayMap->height);
    bm_clear(potraceBitmap, 0);
    
    //##Read the data out of the GrayMap
    for (int y=0 ; y<grayMap->height ; y++)
    {
        for (int x=0 ; x<grayMap->width ; x++)
        {
            BM_UPUT(potraceBitmap, x, y,
                    grayMap->getPixel(grayMap, x, y) ? 0 : 1);
        }
    }
    
    //##Debug
    /*
     FILE *f = fopen("poimage.pbm", "wb");
     bm_writepbm(f, bm);
     fclose(f);
     */
    
    /* trace a bitmap*/
    potrace_state_t *potraceState = potrace_trace(potraceParams,
                                                  potraceBitmap);
    
    //## Free the Potrace bitmap
    bm_free(potraceBitmap);
    
    if (!keepGoing)
    {
//        g_warning("aborted");
        potrace_state_free(potraceState);
        return path;
    }
    
//    Inkscape::SVG::PathString data;
    
    //## copy the path information into our d="" attribute string
    std::vector<dPoint> points;
    long thisNodeCount = writePaths(this, potraceState->plist, path, points);
    
    /* free a potrace items */
    potrace_state_free(potraceState);
    
    if (!keepGoing)
        return path;
    
    if ( nodeCount)
        *nodeCount = thisNodeCount;
    
//    return data.string();
    return path;
}




/**
 *  This is called for a single scan
 */
std::vector<TracingEngineResult> PesTracingUtils::traceSingle(const SkBitmap & thePixbuf, PreviewResult* preview)
{
    
    std::vector<TracingEngineResult> results;
    
    if (thePixbuf.drawsNothing())
        return results;
    
    brightnessFloor = 0.0; //important to set this
    
    GrayMap *grayMap = filter(*this, thePixbuf);
    if (!grayMap)
        return results;
    
    long nodeCount = 0L;
    pesPath d = grayMapToPath(grayMap, &nodeCount);
    
    if(preview!=NULL)
        preview->preview = GrayMapToSkBitmap(grayMap)->asImage();
    grayMap->destroy(grayMap);
    
    char const *style = "fill:#000000";
//    SkDebugf("traceSingle(): style=%s\n", style);
    //g_message("### GOT '%s' \n", d);
    d.setFillHexColor(0x000000);
    d.setFilled(true);
    TracingEngineResult result(style, d, nodeCount);
    results.push_back(result);
    
    return results;
}


/**
 *  This allows routines that already generate GrayMaps to skip image filtering,
 *  increasing performance.
 */
std::vector<TracingEngineResult> PesTracingUtils::traceGrayMap(GrayMap *grayMap)
{
    
    std::vector<TracingEngineResult> results;
    
    brightnessFloor = 0.0; //important to set this
    
    long nodeCount = 0L;
    pesPath d = grayMapToPath(grayMap, &nodeCount);
    
    char const *style = "fill:#000000";
    
    //g_message("### GOT '%s' \n", d);
    SkDebugf("traceGrayMap(): style=%s", style);
    d.setFillHexColor(0x000000);
    TracingEngineResult result(style, d, nodeCount);
    results.push_back(result);
    
    return results;
}

/**
 *  Called for multiple-scanning algorithms
 */
std::vector<TracingEngineResult> PesTracingUtils::traceBrightnessMulti(const SkBitmap & thePixbuf, PreviewResult* preview)
{
    std::vector<TracingEngineResult> results;
    
    if ( thePixbuf.drawsNothing()==false ) {
        double low     = 0.2; //bottom of range
        double high    = 0.9; //top of range
        double delta   = (high - low ) / ((double)multiScanNrColors);
        
        brightnessFloor = 0.0; //Set bottom to black
        
        int traceCount = 0;
        
        for ( brightnessThreshold = low ;
             brightnessThreshold <= high ;
             brightnessThreshold += delta) {
            GrayMap *grayMap = filter(*this, thePixbuf);
            if ( grayMap ) {
                long nodeCount = 0L;
                pesPath d = grayMapToPath(grayMap, &nodeCount);
                
                grayMap->destroy(grayMap);
                
                if ( !d.getCommands().empty() ) {
                    //### get style info
                    int grayVal = (int)(256.0 * brightnessThreshold);
//                    ustring style = ustring::compose("fill-opacity:1.0;fill:#%1%2%3", twohex(grayVal), twohex(grayVal), twohex(grayVal) );
                    std::string style = pesVAArgsToString("fill-opacity:1.0;fill:#%02x%02x%02x", grayVal, grayVal, grayVal);
                    
                    //g_message("### GOT '%s' \n", style.c_str());
//                    d.setFilled(true);
//                    d.setUseShapeColor(true);
                    pesColor c;
                    c.r = c.g = c.b = grayVal;
                    d.setFillColor(c);
                    SkDebugf("traceBrightnessMulti(): threshold[%.2f] style=%s", brightnessThreshold, style.c_str());
                    TracingEngineResult result(style, d, nodeCount);
                    results.push_back(result);
                    
                    if (!multiScanStack) {
                        brightnessFloor = brightnessThreshold;
                    }
                }
            }
        }
        
        //# Remove the bottom-most scan, if requested
        if (results.size() > 1 && multiScanRemoveBackground) {
            results.erase(results.end() - 1);
        }
    }
    
    return results;
}


/**
 *  Quantization
 */
std::vector<TracingEngineResult> PesTracingUtils::traceQuant(const SkBitmap & thePixbuf, PreviewResult* preview)
{
    std::vector<TracingEngineResult> results;
    
    if (thePixbuf.drawsNothing()==false) {
        IndexedMap *iMap = filterIndexed(*this, thePixbuf);
        if ( iMap ) {
            if(preview!=NULL)
                preview->preview = IndexedMapToSkBitmap(iMap)->asImage();
            
            //Create and clear a gray map
            GrayMap *gm = GrayMapCreate(iMap->width, iMap->height);
            for (int row=0 ; row<gm->height ; row++) {
                for (int col=0 ; col<gm->width ; col++) {
                    gm->setPixel(gm, col, row, GRAYMAP_WHITE);
                }
            }
            
            for (int colorIndex=0 ; colorIndex<iMap->nrColors ; colorIndex++) {
                // Make a gray map for each color index
                for (int row=0 ; row<iMap->height ; row++) {
                    for (int col=0 ; col<iMap->width ; col++) {
                        int indx = (int) iMap->getPixel(iMap, col, row);
                        if (indx == colorIndex) {
                            gm->setPixel(gm, col, row, GRAYMAP_BLACK); //black
                        } else if (!multiScanStack) {
                            gm->setPixel(gm, col, row, GRAYMAP_WHITE); //white
                        }
                    }
                }
                
                //## Now we have a traceable graymap
                long nodeCount = 0L;
                pesPath d = grayMapToPath(gm, &nodeCount);
                
                if ( !d.getCommands().empty() ) {
                    //### get style info
                    RGB rgb = iMap->clut[colorIndex];
//                    ustring style = ustring::compose("fill:#%1%2%3", twohex(rgb.r), twohex(rgb.g), twohex(rgb.b) );
                    
                    std::string style = pesVAArgsToString("fill-opacity:1.0;fill:#%02x%02x%02x", rgb.r, rgb.g, rgb.b);
                    
                    //g_message("### GOT '%s' \n", style.c_str());

//#ifndef __EMSCRIPTEN__
//    #ifdef __WIN32__
//                         ofColor c(rgb.r, rgb.g, rgb.b, 255);
//    #else
//                        ofColor c(rgb.b, rgb.g, rgb.r, 255);
//    #endif
//#else
//                    ofColor c(rgb.r, rgb.g, rgb.b, 255);
//#endif
                    
//#ifdef __APPLE_CC__
//                    pesColor c(rgb.b, rgb.g, rgb.r, 255);
//#else
//                    pesColor c(rgb.r, rgb.g, rgb.b, 255);
//#endif
//                    pesColor c(rgb.r, rgb.g, rgb.b, 255);
                    
#ifdef __EMSCRIPTEN__
                    pesColor c(rgb.r, rgb.g, rgb.b, 255);
#else
    #if defined(SK_BUILD_FOR_WIN)
                        pesColor c(rgb.b, rgb.g, rgb.r, 255);
    #elif defined(SK_BUILD_FOR_UNIX)
                        pesColor c(rgb.b, rgb.g, rgb.r, 255);
    #else
                        pesColor c(rgb.r, rgb.g, rgb.b, 255);
    #endif
#endif
                    d.setFillColor(c);
                    d.setFilled(true);
//                    SkDebugf("traceQuant(): Index[%d] style=%s\n", colorIndex, style.c_str());
                    TracingEngineResult result(style, d, nodeCount);
                    results.push_back(result);
                }
                d.clear();
            }// for colorIndex
            
            gm->destroy(gm);
            iMap->destroy(iMap);
        }
        
        //# Remove the bottom-most scan, if requested
        if (results.size() > 1 && multiScanRemoveBackground) {
            results.erase(results.end() - 1);
        }
    }
    
    return results;
}



/**
 *  This is the working method of this interface, and all
 *  implementing classes.  Take a GdkPixbuf, trace it, and
 *  return the path data that is compatible with the d="" attribute
 *  of an SVG <path> element.
 */
std::vector<TracingEngineResult>
PesTracingUtils::trace(const SkBitmap & pixbuf, PreviewResult* preview)
{
    
    SkBitmap thePixbuf(pixbuf);
    
    //Set up for messages
    keepGoing             = 1;
    
    if ( traceType == TRACE_QUANT_COLOR ||
        traceType == TRACE_QUANT_MONO   )
    {
        return traceQuant(thePixbuf, preview);
    }
    else if ( traceType == TRACE_BRIGHTNESS_MULTI )
    {
        return traceBrightnessMulti(thePixbuf, preview);
    }
    else
    {
        return traceSingle(thePixbuf, preview);
    }
}


/**
 *  Abort the thread that is executing getPathDataFromPixbuf()
 */
void PesTracingUtils::abort()
{
    SkDebugf("PesTracingUtils::abort()\n");
    keepGoing = 0;
}
