//
//  Shaper.cpp
//  FontWorks
//
//  Created by SIHARAT THAMMAYA on 1/27/17.
//  Copyright © 2017 Jimmy Software Co., Ltd. All rights reserved.
//

#include "Shaper.hpp"
//#include "ofxClipper.h"

#ifdef __EMSCRIPTEN__
    Tessellator Shaper::tessellator;
#elif HAS_TLS
    thread_local Tessellator Shaper::tessellator;
#endif

// MARK: Shaper
Shaper::Shaper(){
    windingMode = PolyWindingMode::POLY_WINDING_ODD;
    prevCurveRes = 20;
    curveResolution = 20;
    circleResolution = 20;
    bNeedsTessellation = false;
    bHasChanged = false;
    bNeedsPolylinesGeneration = false;
    clear();
}

void Shaper::clear(){
    polylines.resize(1);
    polylines[0].clear();
    flagShapeChanged();
}

void Shaper::close(bool backhome){
    if(backhome){
        guiVec2f home = polylines[0][0];
        lastPolyline().lineTo(home.x, home.y);
    }
    lastPolyline().setClosed(true);
    flagShapeChanged();
}

void Shaper::lineTo(float x, float y){
    lastPolyline().lineTo(x, y);
    flagShapeChanged();
}

void Shaper::moveTo(float x, float y){
    if(lastPolyline().size()>0){
        // new sub path
        polylines.push_back(GUI_Polyline());
    }
    lastPolyline().add(guiVec2f(x,y));
    flagShapeChanged();
}

void Shaper::curveTo(float x, float y){
    lastPolyline().curveTo(x, y,curveResolution);
    flagShapeChanged();
}

void Shaper::bezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & cp3){
    lastPolyline().bezierTo(cp1, cp2, cp3, curveResolution);
    flagShapeChanged();
}

void Shaper::quadBezierTo(float cx1, float cy1, float cx2, float cy2, float cx3, float cy3){
    quadBezierTo({cx1,cy1}, {cx2,cy2}, {cx3,cy3});
}

void Shaper::quadBezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & cp3){
    lastPolyline().quadBezierTo(cp1, cp2, cp3, curveResolution);
    flagShapeChanged();
}

void Shaper::arc(const guiVec2f & centre, float radiusX, float radiusY, float angleBegin, float angleEnd){
    lastPolyline().arc(centre,radiusX,radiusY,angleBegin,angleEnd,circleResolution);
    flagShapeChanged();
}

void Shaper::arcNegative(const guiVec2f & centre, float radiusX, float radiusY, float angleBegin, float angleEnd){
    lastPolyline().arcNegative(centre,radiusX,radiusY,angleBegin,angleEnd,circleResolution);
    flagShapeChanged();
}

void Shaper::triangle(const guiVec2f & p1, const guiVec2f & p2, const guiVec2f & p3){
    moveTo(p1.x, p1.y);
    lineTo(p2.x, p2.y);
    lineTo(p3.x, p3.y);
    close();
}

void Shaper::circle(const guiVec2f & p, float radius){
    arc(p, radius, radius, 0, 360);
}

void Shaper::ellipse(const guiVec2f & p, float width, float height){
    arc(p, width*.5f, height*.5f, 0, 360);
}

void Shaper::rectangle(float x,float y,float w,float h){
    moveTo(x,y);
    lineTo(x+w,y);
    lineTo(x+w,y+h);
    lineTo(x,y+h);
    lineTo(x,y);
    close();
}

void Shaper::rectRounded(const guiRectangle & b, float topLeftRadius,
                         float topRightRadius,
                         float bottomRightRadius,
                         float bottomLeftRadius){
    
    rectRounded(b.x,b.y,b.width,b.height,topLeftRadius,topRightRadius,bottomRightRadius,bottomLeftRadius);
}

void Shaper::rectRounded(float x, float y, float w, float h, float r){
    rectRounded(x,y,w,h,r,r,r,r);
}

void Shaper::rectRounded(float x, float y, float w, float h, float topLeftRadius,
                         float topRightRadius,
                         float bottomRightRadius,
                         float bottomLeftRadius){
    if(w < 0.0f) {
        x += w;
        w *= -1.0f;
    }
    
    if(h < 0.0f) {
        y += h;
        h *= -1.0f;
    }
    
    // keep radii in check
    float maxRadius = MIN(w / 2.0f, h / 2.0f);
    topLeftRadius        = MIN(topLeftRadius,     maxRadius);
    topRightRadius       = MIN(topRightRadius,     maxRadius);
    bottomRightRadius    = MIN(bottomRightRadius,     maxRadius);
    bottomLeftRadius     = MIN(bottomLeftRadius,     maxRadius);
    
    // if all radii are ~= 0.0f, then render as a normal rectangle
    if((fabs(topLeftRadius)     < FLT_EPSILON) &&
       (fabs(topRightRadius)    < FLT_EPSILON) &&
       (fabs(bottomRightRadius) < FLT_EPSILON) &&
       (fabs(bottomLeftRadius)  < FLT_EPSILON)) {
        
        rectangle(x, y, w, h);
        
    } else {
        float left   = x;
        float right  = x + w;
        float top    = y;
        float bottom = y + h;
        
        
        moveTo(left + topLeftRadius, top);
        
        // top right
        if(fabs(topRightRadius) >= FLT_EPSILON) {
            arc(guiVec2f(right - topRightRadius, top + topRightRadius), topRightRadius, topRightRadius, 270, 360);
        } else {
            lineTo(right, top);
        }
        
        lineTo(right, bottom - bottomRightRadius);
        // bottom right
        if(fabs(bottomRightRadius) >= FLT_EPSILON) {
            arc(guiVec2f(right - bottomRightRadius, bottom - bottomRightRadius), bottomRightRadius, bottomRightRadius, 0, 90);
        }
        
        lineTo(left + bottomLeftRadius, bottom);
        
        // bottom left
        if(fabs(bottomLeftRadius) >= FLT_EPSILON) {
            arc(guiVec2f(left + bottomLeftRadius, bottom - bottomLeftRadius), bottomLeftRadius, bottomLeftRadius, 90, 180);
        }
        
        lineTo(left, top + topLeftRadius);
        
        // top left
        if(fabs(topLeftRadius) >= FLT_EPSILON) {
            arc(guiVec2f(left + topLeftRadius, top + topLeftRadius), topLeftRadius, topLeftRadius, 180, 270);
        }
        close();
        
    }
}

void Shaper::setCurveResolution(int _curveResolution){
    curveResolution = _curveResolution;
}

int Shaper::getCurveResolution() const{
    return curveResolution;
}

void Shaper::setCircleResolution(int _circleResolution){
    circleResolution = _circleResolution;
}

int Shaper::getCircleResolution() const{
    return circleResolution;
}

const vector<GUI_Polyline> & Shaper::getOutline() const{
//#ifdef __ANDROID__
//    return polylines;
//#else
    if(windingMode!=PolyWindingMode::POLY_WINDING_ODD){
        const_cast<Shaper*>(this)->tessellate();
        return tessellatedContour;
    }else{
        // TODO: tesselate not supported PolyWindingMode::POLY_WINDING_ODD mode.
        return polylines;
    }
    
    return polylines;
//#endif
}

void Shaper::tessellate(){
    if(!bNeedsTessellation) return;
    
//#ifndef  __ANDROID__
    if(windingMode!=PolyWindingMode::POLY_WINDING_ODD){
        tessellator.tessellateToPolylines( polylines, windingMode, tessellatedContour);
    }
    else{
        // TODO: Don't have mesh, we on sdl not gl
//        tessellator.tessellateToMesh( polylines, windingMode, cachedTessellation);
//        cachedTessellationValid=true;
        tessellatedContour = polylines;
    }
    
//#else
//    tessellatedContour = polylines;
//#endif
    
    bNeedsTessellation = false;
}

GUI_Polyline & Shaper::lastPolyline(){
    if(polylines.empty() || polylines.back().isClosed()){
        polylines.push_back(GUI_Polyline());
    }
    return polylines.back();
}

//const GUI_Polyline & Shaper::lastPolyline() const{
//    return lastPolyline();
//}

void Shaper::flagShapeChanged(){
    bNeedsTessellation = true;
}

bool Shaper::hasChanged(){
    return bNeedsTessellation;
}

void Shaper::setPolyWindingMode(PolyWindingMode newMode){
    if(windingMode != newMode){
        windingMode = newMode;
        bNeedsTessellation = true;
    }
}

PolyWindingMode Shaper::getPolyWindingMode(){
    return windingMode;
}

void Shaper::simplify(float tol){
    for(int i=0;i<(int)polylines.size();i++){
        GUI_LogNotice("Before simplify") << polylines[i].size();
        polylines[i].simplify(tol);
        GUI_LogNotice("After simplify") << polylines[i].size() << " tolerance=" << tol;
    }
}

void Shaper::scale(float x, float y){
    for(int i=0;i<(int)polylines.size();i++){
        for(int j=0;j<(int)polylines[i].size();j++){
            polylines[i][j].x*=x;
            polylines[i][j].y*=y;
        }
    }
    flagShapeChanged();
}

void Shaper::translate(int x, int y){
    for(int i=0;i<(int)polylines.size();i++){
        for(int j=0;j<(int)polylines[i].size();j++){
            polylines[i][j].x += x;
            polylines[i][j].y += y;
        }
    }
    flagShapeChanged();
}

// MARK: Tessellator
void * memAllocator( void *userData, unsigned int size ){
//    GUI_Log("Tessellator memAllocator\n");
    return malloc(size);
}

void * memReallocator( void *userData, void* ptr, unsigned int size ){
//    GUI_Log("Tessellator memReallocator\n");
    return realloc(ptr,size);
}

void memFree( void *userData, void *ptr ){
//    GUI_Log("Tessellator memFree\n");
    free (ptr);
}


Tessellator::Tessellator()
: cacheTess(nullptr)
{
//    GUI_Log("Tessellator::comstruct\n");
    init();
}

Tessellator::~Tessellator(){
//    GUI_Log("Tessellator::destruct\n");
    tessDeleteTess(cacheTess);
}

Tessellator::Tessellator(const Tessellator & mom) : cacheTess(nullptr), tessAllocator() {
//    GUI_Log("Tessellator::construct(mom)\n");
    if(&mom != this){
        if(cacheTess) tessDeleteTess(cacheTess);
        init();
    }
}

Tessellator & Tessellator::operator=(const Tessellator & mom){
    if(&mom != this){
        if(cacheTess) tessDeleteTess(cacheTess);
        init();
    }
    return *this;
}

void Tessellator::init(){
//    GUI_Log("Tessellator::init\n");
    tessAllocator.memalloc = memAllocator;
    tessAllocator.memrealloc = memReallocator;
    tessAllocator.memfree = memFree;
    tessAllocator.meshEdgeBucketSize=0;
    tessAllocator.meshVertexBucketSize=0;
    tessAllocator.meshFaceBucketSize=0;
    tessAllocator.dictNodeBucketSize=0;
    tessAllocator.regionBucketSize=0;
    tessAllocator.extraVertices=0;
    cacheTess = tessNewTess( &tessAllocator );
}

void Tessellator::tessellateToMesh( const GUI_Polyline& src,  PolyWindingMode polyWindingMode, guiMesh& dstmesh, bool bIs2D ){
//    GUI_Log("Tessellator::tessellateToMesh\n");
    GUI_Polyline& polyline = const_cast<GUI_Polyline&>(src);
    tessAddContour( cacheTess, bIs2D?2:3, &polyline.getVertices()[0], sizeof(vectype), polyline.size());
    
    performTessellation( polyWindingMode, dstmesh, bIs2D );
}

void Tessellator::tessellateToMesh( const vector<GUI_Polyline>& src, PolyWindingMode polyWindingMode, guiMesh & dstmesh, bool bIs2D ){
//    GUI_Log("Tessellator::tessellateToMesh(s)\n");
    // pass vertex pointers to GLU tessellator
    for ( int i=0; i<(int)src.size(); ++i ) {
        if (src[i].size() > 0) {
            GUI_Polyline& polyline = const_cast<GUI_Polyline&>(src[i]);
            
            tessAddContour(cacheTess, bIs2D ? 2 : 3, &polyline.getVertices()[0].x, sizeof(vectype), polyline.size());
        }
    }
    
    performTessellation( polyWindingMode, dstmesh, bIs2D );
}

void Tessellator::tessellateToPolylines( const GUI_Polyline& src,  PolyWindingMode polyWindingMode, vector<GUI_Polyline>& dstpoly, bool bIs2D){
//    GUI_Log("Tessellator::tessellateToPolyline\n");
    if (src.size() > 0) {
        GUI_Polyline& polyline = const_cast<GUI_Polyline&>(src);
        tessAddContour(cacheTess, bIs2D ? 2 : 3, &polyline.getVertices()[0], sizeof(vectype), (int)polyline.size());
    }
    performTessellation( polyWindingMode, dstpoly, bIs2D );
}


//----------------------------------------------------------
void Tessellator::tessellateToPolylines( const vector<GUI_Polyline>& src, PolyWindingMode polyWindingMode, vector<GUI_Polyline>& dstpoly, bool bIs2D ) {
//    GUI_Log("Tessellator::tessellateToPolyline(s)\n");
    // pass vertex pointers to GLU tessellator
    for ( int i=0; i<(int)src.size(); ++i ) {
        if (src[i].size() > 0) {
            GUI_Polyline& polyline = const_cast<GUI_Polyline&>(src[i]);
//            GUI_Log("call tessAddContour()\n");
            tessAddContour(cacheTess, bIs2D ? 2 : 3, &polyline.getVertices()[0].x, sizeof(vectype), (int)polyline.size());
        }
    }
    
    performTessellation( polyWindingMode, dstpoly, bIs2D );
}

void Tessellator::performTessellation(PolyWindingMode polyWindingMode, guiMesh& dstmesh, bool bIs2D ) {
//    GUI_Log("Tessellator::performTessellation(mesh)\n");
    int vertexSize = bIs2D ? 2 : 3;
    if (!tessTesselate(cacheTess, (int)polyWindingMode, TESS_POLYGONS, 3, vertexSize, 0)){
        GUI_LogError("ofTessellator") << "performTessellation(): mesh polygon tessellation failed, winding mode " << (int)polyWindingMode;
        return;
    }
    
    int numVertices = tessGetVertexCount( cacheTess );
    int numIndices = tessGetElementCount( cacheTess )*3;
    
    dstmesh.clear();
    dstmesh.addVertices((vectype*)tessGetVertices(cacheTess),numVertices);
    dstmesh.addIndices((guiIndexType*)tessGetElements(cacheTess),numIndices);
    /*ofIndexType * tessElements = (ofIndexType *)tessGetElements(cacheTess);
     for(int i=0;i<numIndices;i++){
     if(tessElements[i]!=TESS_UNDEF)
     dstmesh.addIndex(tessElements[i]);
     }*/
//    dstmesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
}

void Tessellator::performTessellation(PolyWindingMode polyWindingMode, vector<GUI_Polyline>& dstpoly, bool bIs2D ) {
//    GUI_Log("Tessellator::performTessellation(poly)\n");
    int vertexSize = bIs2D ? 2 : 3;
    if (!tessTesselate(cacheTess, (int)polyWindingMode, TESS_BOUNDARY_CONTOURS, 0, vertexSize, 0)){
        GUI_LogError("Tessellator") << "performTesselation(): polyline boundary contours tessellation failed, winding mode " << (int)polyWindingMode;
        return;
    }
    
    const guiVec2f* verts = (guiVec2f*)tessGetVertices(cacheTess);
    const TESSindex* elems = tessGetElements(cacheTess);
    const int nelems = tessGetElementCount(cacheTess);
    dstpoly.resize(nelems);
    for (int i = 0; i < nelems; ++i)
    {
        int b = elems[i*2];
        int n = elems[i*2+1];
        dstpoly[i].clear();
        dstpoly[i].add(&verts[b],n);
        dstpoly[i].setClosed(true);
    }
}



// MARK: GUI_Polyline
GUI_Polyline::GUI_Polyline(){
    setRightVector();
    clear();
}

GUI_Polyline::GUI_Polyline(const vector<guiVec2f>& points){
    setRightVector();
    clear();
    add(points);
}

GUI_Polyline GUI_Polyline::fromRect(const GUI_Rect& r){
    GUI_Polyline p;
    p.add(guiVec2f((float)r.x, (float)r.y));
    p.add(guiVec2f((float)(r.x+r.w), (float)r.y));
    p.add(guiVec2f((float)(r.x+r.w), (float)(r.y+r.h)));
    p.add(guiVec2f((float)r.x, (float)(r.y+r.h)));
    p.close();
    return p;
}

void GUI_Polyline::clear(){
    setClosed(false);
    points.clear();
    curveVertices.clear();
    flagHasChanged();
}

void GUI_Polyline::close(){
    setClosed(true);
}

void GUI_Polyline::setClosed( bool tf ){
    bClosed = tf;
    flagHasChanged();
}

bool GUI_Polyline::isClosed() const{
    return bClosed;
}

void GUI_Polyline::add(const guiVec2f& p){
    curveVertices.clear();
    points.push_back(p);
    flagHasChanged();
}

void GUI_Polyline::add(const guiVec2f* verts, int numverts){
    curveVertices.clear();
    points.insert( points.end(), verts, verts + numverts );
    flagHasChanged();
}

void GUI_Polyline::add(const vector<guiVec2f>& verts){
    curveVertices.clear();
    points.insert( points.end(), verts.begin(), verts.end() );
    flagHasChanged();
}

void GUI_Polyline::add(float x, float y){
    add(guiVec2f(x, y));
}

void GUI_Polyline::insert(const guiVec2f& p, int index){
    curveVertices.clear();
    points.insert(points.begin()+index, p);
    flagHasChanged();
}

void GUI_Polyline::insert(const vector<guiVec2f>& verts, int index){
    curveVertices.clear();
    points.insert( points.end(), verts.begin(), verts.end() );
    flagHasChanged();
}

void GUI_Polyline::resize(size_t size){
    points.resize(size);
    flagHasChanged();
}

size_t GUI_Polyline::size() const{
    return points.size();
}

const guiVec2f& GUI_Polyline::operator[] (int index) const{
    return points[index];
}

guiVec2f& GUI_Polyline::operator[] (int index){
    flagHasChanged();
    return points[index];
}

vector<guiVec2f>::iterator GUI_Polyline::begin(){
    return points.begin();
}

//--------------------------------------------------
vector<guiVec2f>::iterator GUI_Polyline::end(){
    return points.end();
}

//--------------------------------------------------
vector<guiVec2f>::const_iterator GUI_Polyline::begin() const{
    return points.begin();
}

//--------------------------------------------------
vector<guiVec2f>::const_iterator GUI_Polyline::end() const{
    return points.end();
}

//--------------------------------------------------
vector<guiVec2f>::reverse_iterator GUI_Polyline::rbegin(){
    return points.rbegin();
}

//--------------------------------------------------
vector<guiVec2f>::reverse_iterator GUI_Polyline::rend(){
    return points.rend();
}

//--------------------------------------------------
vector<guiVec2f>::const_reverse_iterator GUI_Polyline::rbegin() const{
    return points.rbegin();
}

//--------------------------------------------------
vector<guiVec2f>::const_reverse_iterator GUI_Polyline::rend() const{
    return points.rend();
}

bool GUI_Polyline::hasChanged(){
    if(bHasChanged){
        bHasChanged=false;
        return true;
    }else{
        return false;
    }
}

void GUI_Polyline::flagHasChanged() {
    bHasChanged = true;
    bCacheIsDirty = true;
}

vector<guiVec2f> & GUI_Polyline::getVertices(){
    flagHasChanged();
    return points;
}

const vector<guiVec2f> & GUI_Polyline::getVertices() const{
    return points;
}

void GUI_Polyline::setCircleResolution(int res){
    if (res > 1 && res != (int)circlePoints.size()){
        circlePoints.resize(res);
        
        float angle = 0.0f;
        const float angleAdder = (float)(M_TWO_PI / res);
        for (int i = 0; i < res; i++){
            circlePoints[i].x = cos(angle);
            circlePoints[i].y = sin(angle);
            angle += angleAdder;
        }
    }
}

float GUI_Polyline::wrapAngle(float angleRadians) {
    return guiWrap(angleRadians, 0.0f, (float)TWO_PI);
}

void GUI_Polyline::lineTo(float x, float y){
    add(guiVec2f(x, y));
}

void GUI_Polyline::bezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & to, int curveResolution){
    // if, and only if poly vertices has points, we can make a bezier
    // from the last point
    curveVertices.clear();
    
    // the resolultion with which we computer this bezier
    // is arbitrary, can we possibly make it dynamic?
    
    if (size() > 0){
        float x0 = points[size()-1].x;
        float y0 = points[size()-1].y;
        
        float   ax, bx, cx;
        float   ay, by, cy;
        float   t, t2, t3;
        float   x, y;
        
        // polynomial coefficients
        cx = 3.0f * (cp1.x - x0);
        bx = 3.0f * (cp2.x - cp1.x) - cx;
        ax = to.x - x0 - cx - bx;
        
        cy = 3.0f * (cp1.y - y0);
        by = 3.0f * (cp2.y - cp1.y) - cy;
        ay = to.y - y0 - cy - by;
        
        for (int i = 1; i <= curveResolution; i++){
            t 	=  (float)i / (float)(curveResolution);
            t2 = t * t;
            t3 = t2 * t;
            x = (ax * t3) + (bx * t2) + (cx * t) + x0;
            y = (ay * t3) + (by * t2) + (cy * t) + y0;
            points.emplace_back(x,y);
        }
    }
    flagHasChanged();
}

void GUI_Polyline::quadBezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & cp3, int curveResolution){
    curveVertices.clear();
    for(int i=0; i <= curveResolution; i++){
        double t = (double)i / (double)(curveResolution);
        double a = (1.0 - t)*(1.0 - t);
        double b = 2.0 * t * (1.0 - t);
        double c = t*t;
        double x = a * cp1.x + b * cp2.x + c * cp3.x;
        double y = a * cp1.y + b * cp2.y + c * cp3.y;
        points.emplace_back(x, y);
    }
    flagHasChanged();
}

void GUI_Polyline::curveTo(float x, float y, int curveResolution){
    
    if(curveVertices.size()==0){
        if(points.size()>0){
            guiVec2f last = points.back();
            curveVertices.push_back(last);
            curveVertices.push_back(last); // double start point
        }
        else{
            curveVertices.push_back(guiVec2f(x,y));
        }
    }
    curveVertices.push_back(guiVec2f(x,y));
    
    if (curveVertices.size() == 4){
        
        float x0 = curveVertices[0].x;
        float y0 = curveVertices[0].y;
        //        float z0 = 0;
        float x1 = curveVertices[1].x;
        float y1 = curveVertices[1].y;
        //        float z1 = 0;
        float x2 = curveVertices[2].x;
        float y2 = curveVertices[2].y;
        //        float z2 = 0;
        float x3 = curveVertices[3].x;
        float y3 = curveVertices[3].y;
        //        float z3 = 0;
        
        float t,t2,t3;
        float x,y;
        
        for (int i = 1; i <= curveResolution; i++){
            
            t 	=  (float)i / (float)(curveResolution);
            t = guiClamp(t, 0.0, 1.0);
            t2 	= t * t;
            t3 	= t2 * t;
            
            x = 0.5f * ( ( 2.0f * x1 ) +
                        ( -x0 + x2 ) * t +
                        ( 2.0f * x0 - 5.0f * x1 + 4 * x2 - x3 ) * t2 +
                        ( -x0 + 3.0f * x1 - 3.0f * x2 + x3 ) * t3 );
            
            y = 0.5f * ( ( 2.0f * y1 ) +
                        ( -y0 + y2 ) * t +
                        ( 2.0f * y0 - 5.0f * y1 + 4 * y2 - y3 ) * t2 +
                        ( -y0 + 3.0f * y1 - 3.0f * y2 + y3 ) * t3 );
            
            points.emplace_back(x,y);
        }
        curveVertices.pop_front();
    }
    flagHasChanged();
}


void GUI_Polyline::catmullTo(float x, float y, int curveResolution){
    if(curveVertices.size()==0){
        if(points.size()>0){
            guiVec2f last = points.back();
            curveVertices.push_back(last);
            curveVertices.push_back(last);
        }
        else{
            curveVertices.push_back(guiVec2f(x,y));
        }
    }
    curveVertices.push_back(guiVec2f(x,y));
    
    if (curveVertices.size() == 4){
        guiVec2f y[4];
        y[0] = curveVertices[0];
        y[1] = curveVertices[1];
        y[2] = curveVertices[2];
        y[3] = curveVertices[3];
        
        guiVec2f a0 = -0.5*y[0] +1.5*y[1] - 1.5*y[2] + 0.5*y[3];
        guiVec2f a1 = y[0] - 2.5*y[1] + 2*y[2] - 0.5*y[3];
        guiVec2f a2 = -0.5*y[0] + 0.5*y[2];
        guiVec2f a3 = y[1];
        
        for (int i = 1; i <= curveResolution; i++){
            
            float  t 	=  (float)i / (float)(curveResolution);
            float t2 = t*t;
            
            guiVec2f p = a0*t*t2 + a1*t2 +a2*t + a3;
            
            points.push_back(p);
        }
        curveVertices.pop_front();
    }
    flagHasChanged();
}

void GUI_Polyline::hermiteTo(float x, float y, int curveResolution){
    if(curveVertices.size()==0){
        if(points.size()>0){
            guiVec2f last = points.back();
            curveVertices.push_back(last);
            curveVertices.push_back(last);
        }
        else{
            curveVertices.push_back(guiVec2f(x,y));
        }
    }
    curveVertices.push_back(guiVec2f(x,y));
    
    if (curveVertices.size() == 4){
        guiVec2f y[4];
        y[0] = curveVertices[0];
        y[1] = curveVertices[1];
        y[2] = curveVertices[2];
        y[3] = curveVertices[3];
        
         guiVec2f c = (y[2] - y[0]) * 0.5f;
         guiVec2f v = y[1] - y[2];
         guiVec2f w = c + v;
         guiVec2f a = w + v + (y[3] - y[1]) * 0.5f;
         guiVec2f b_neg = w + a;
        
        for (int i = 1; i <= curveResolution; i++){
            
            float  t 	=  (float)i / (float)(curveResolution);
            
            guiVec2f p = ((((a*t) - b_neg)*t + c)*t + y[1]);
            points.push_back(p);
        }
        curveVertices.pop_front();
    }
    flagHasChanged();
}

//template<typename Type>
//Type ofInterpolateCubic(const Type& y0, const Type& y1, const Type& y2, const Type& y3, float pct){
//    Type a0,a1,a2,a3;
//    float pct2;
//    
//    pct2 = pct*pct;
//    a0 = y3 - y2 - y0 + y1;
//    a1 = y0 - y1 - a0;
//    a2 = y2 - y0;
//    a3 = y1;
//    
//    return(a0*pct*pct2+a1*pct2+a2*pct+a3);
//}

void GUI_Polyline::cubicTo(float x, float y, int curveResolution){
    if(curveVertices.size()==0){
        if(points.size()>0){
            guiVec2f last = points.back();
            curveVertices.push_back(last);
            curveVertices.push_back(last);
        }
        else{
            curveVertices.push_back(guiVec2f(x,y));
        }
    }
    curveVertices.push_back(guiVec2f(x,y));
    
    if (curveVertices.size() == 4){
        guiVec2f y[4];
        y[0] = curveVertices[0];
        y[1] = curveVertices[1];
        y[2] = curveVertices[2];
        y[3] = curveVertices[3];
        
        guiVec2f a0 = y[3] - y[2]- y[0]+ y[1];
        guiVec2f a1 = y[0] - y[1] - a0;
        guiVec2f a2 = y[2] - y[0];
        guiVec2f a3 = y[1];
        
        
        
        for (int i = 1; i <= curveResolution; i++){
            
            float  t 	=  (float)i / (float)(curveResolution);
            float  t2   =  (1-cos(t*static_cast<float>(PI)))/2;
            
            guiVec2f p = (a0*t*t2 + a1*t2 + a2*t +a3);
            points.push_back(p);
        }
        curveVertices.pop_front();
    }
    flagHasChanged();
}

void GUI_Polyline::arc(const guiVec2f & center, float radiusX, float radiusY, float angleBegin, float angleEnd, int circleResolution, bool bClockwise){
    bool clockwise = bClockwise;
    
    if(circleResolution<=1) circleResolution=2;
    setCircleResolution(circleResolution);
    points.reserve(points.size()+circleResolution);
    
    const float epsilon = 0.0001f;
    
    const size_t nCirclePoints = circlePoints.size();
    float segmentArcSize  = (float)(M_TWO_PI / (float)nCirclePoints);
    
    // convert angles to radians and wrap them into the range 0-M_TWO_PI and
    float angleBeginRad = wrapAngle(guiDegToRad(angleBegin));
    float angleEndRad =   wrapAngle(guiDegToRad(angleEnd));
    
    while(angleBeginRad >= angleEndRad) angleEndRad += (float)M_TWO_PI;
    
    // determine the directional angle delta
    float d = clockwise ? angleEndRad - angleBeginRad : angleBeginRad - angleEndRad;
    // find the shortest angle delta, clockwise delta direction yeilds POSITIVE values
    float deltaAngle = atan2(sin(d),cos(d));
    
    // establish the remaining angle that we have to work through
    float remainingAngle = deltaAngle;
    
    // if the delta angle is in the CCW direction OR the start and stop angles are
    // effectively the same adjust the remaining angle to be a be a full rotation
    if(deltaAngle < 0 || abs(deltaAngle) < epsilon) remainingAngle += (float)M_TWO_PI;
    
    guiVec2f radii(radiusX, radiusY);
    guiVec2f point;
    
    int currentLUTIndex = 0;
    bool isFirstPoint = true; // special case for the first point
    
    while(remainingAngle > 0) {
        if(isFirstPoint) {
            // TODO: should this be the exact point on the circle or
            // should it be an intersecting point on the line that connects two
            // surrounding LUT points?
            //
            // get the EXACT first point requested (for points that
            // don't fall precisely on a LUT entry)
            point = guiVec2f(cos(angleBeginRad), sin(angleBeginRad));
            // set up the get any in between points from the LUT
            float ratio = (float)(angleBeginRad / M_TWO_PI * (float)nCirclePoints);
            currentLUTIndex = clockwise ? (int)ceil(ratio) : (int)floor(ratio);
            float lutAngleAtIndex = currentLUTIndex * segmentArcSize;
            // the angle between the beginning angle and the next angle in the LUT table
            float d = clockwise ? (lutAngleAtIndex - angleBeginRad) : (angleBeginRad - lutAngleAtIndex);
            float firstPointDelta = atan2(sin(d),cos(d)); // negative is in the clockwise direction
            
            // if the are "equal", get the next one CCW
            if(abs(firstPointDelta) < epsilon) {
                currentLUTIndex = clockwise ? (currentLUTIndex + 1) : (currentLUTIndex - 1);
                firstPointDelta = segmentArcSize; // we start at the next lut point
            }
            
            // start counting from the offset
            remainingAngle -= firstPointDelta;
            isFirstPoint = false;
        } else {
            point = guiVec2f(circlePoints[currentLUTIndex].x, circlePoints[currentLUTIndex].y);
            if(clockwise) {
                currentLUTIndex++; // go to the next LUT point
                remainingAngle -= segmentArcSize; // account for next point
                // if the angle overshoots, then the while loop will fail next time
            } else {
                currentLUTIndex--; // go to the next LUT point
                remainingAngle -= segmentArcSize; // account for next point
                // if the angle overshoots, then the while loop will fail next time
            }
        }
        
        // keep the current lut index in range
        if(clockwise) {
            currentLUTIndex = currentLUTIndex % nCirclePoints;
        } else {
            if(currentLUTIndex < 0) currentLUTIndex = (int)nCirclePoints + currentLUTIndex;
        }
        
        // add the point to the poly line
        point = point * radii + center;
        points.push_back(point);
        
        // if the next LUT point moves us past the end angle then
        // add a a point a the exact end angle and call it finished
        if(remainingAngle < epsilon) {
            point = guiVec2f(cos(angleEndRad), sin(angleEndRad));
            point = point * radii + center;
            points.push_back(point);
            remainingAngle = 0; // call it finished, the next while loop test will fail
        }
    }
    flagHasChanged();
}

void GUI_Polyline::arcNegative(const guiVec2f & center, float radiusX, float radiusY, float angleBegin, float angleEnd, int circleResolution){
    arc(center, radiusX, radiusY, angleBegin, angleEnd, circleResolution, false);
}

//----------------------------------------------------------
float GUI_Polyline::getPerimeter() const {
    if(points.size() < 2) {
        return 0;
    } else {
        updateCache();
        if(lengths.size()){
//            GUI_LogNotice("GUI_Polyline::getPerimeter()", "OK return %.3f", lengths.back());
            return lengths.back();
        }
        
        GUI_LogWarning("GUI_Polyline::getPerimeter()", "fail lengths.size()==0 wtf!") << "";
        guiRectangle bbox = getBoundingBox();
        float l = sqrt( bbox.height * bbox.height + bbox.width * bbox.width );
        GUI_LogWarning("GUI_Polyline::getPerimeter()", "fail lengths.size()==0 return bbox.lengthSquad()=%.3f", l) << "";
        return l;
    }
}

//----------------------------------------------------------
float GUI_Polyline::getArea() const{
    updateCache();
    return area;
}

//----------------------------------------------------------
ofPoint GUI_Polyline::getCentroid2D() const{
    updateCache();
    return centroid2D;
}

//----------------------------------------------------------
guiRectangle GUI_Polyline::getBoundingBox() const {
    
//    guiRectangle box;
//    for(size_t i = 0; i < size(); i++) {
//        if(i == 0) {
//            box.set(points[i],0,0);
//        } else {
//            box.growToInclude(points[i]);
//        }
//    }
//    return box;
    
    updateCache();
    return bound;
}

//----------------------------------------------------------
GUI_Polyline GUI_Polyline::getSmoothed(int smoothingSize, float smoothingShape) const {
    int n = size();
    smoothingSize = (int)guiClamp((float)smoothingSize, 0.0f, (float)n);
    smoothingShape = guiClamp((float)smoothingShape, 0.0f, 1.0f);
    
    // precompute weights and normalization
    vector<float> weights;
    weights.resize(smoothingSize);
    // side weights
    for(int i = 1; i < smoothingSize; i++) {
        float curWeight = guiMap((float)i, 0.0f, (float)smoothingSize, 1.0f, smoothingShape);
        weights[i] = curWeight;
    }
    
    // make a copy of this polyline
    GUI_Polyline result = *this;
    
    for(int i = 0; i < n; i++) {
        float sum = 1; // center weight
        for(int j = 1; j < smoothingSize; j++) {
            ofVec3f cur;
            int leftPosition = i - j;
            int rightPosition = i + j;
            if(leftPosition < 0 && bClosed) {
                leftPosition += n;
            }
            if(leftPosition >= 0) {
                cur += points[leftPosition];
                sum += weights[j];
            }
            if(rightPosition >= n && bClosed) {
                rightPosition -= n;
            }
            if(rightPosition < n) {
                cur += points[rightPosition];
                sum += weights[j];
            }
            result[i] += cur * weights[j];
        }
        result[i] /= sum;
    }
    
    return result;
}

//----------------------------------------------------------
GUI_Polyline GUI_Polyline::getResampledBySpacing(float spacing) const {
    if(spacing==0 || size() == 0) return *this;
    GUI_Polyline poly;
    float totalLength = getPerimeter();
    for(float f=0; f<totalLength; f += spacing) {
        ofPoint p = getPointAtLength(f);
        poly.lineTo(p.x, p.y);
    }
    
    if(!isClosed()) {
        if(poly.size() > 0) poly.points.back() = points.back();
        poly.setClosed(false);
    } else {
        poly.setClosed(true);
    }
    
    return poly;
}

//----------------------------------------------------------
GUI_Polyline GUI_Polyline::getResampledByCount(int count) const {
    float perimeter = getPerimeter();
    if(count < 2) {
        //GUI_LogWarning("GUI_Polyline") << "getResampledByCount(): requested " << count <<" points, using minimum count of 2 ";
        count = 2;
    }
    return GUI_Polyline::getResampledBySpacing(perimeter / (count-1));
}

//----------------------------------------------------------
// pom--v2
GUI_Polyline GUI_Polyline::getResampledByCount2(int count) const{
    if(size() == 0) return *this;
    
    if(count < 2) {
        //GUI_LogWarning("GUI_Polyline") << "getResampledByCount(): requested " << count <<" points, using minimum count of 2 ";
        count = 2;
    }
    
    GUI_Polyline poly;
    float fstep = 1.0f / count;
    for(int i=0; i<=count; i++){
        float percent = fstep*i;
        if(percent>1.0)
            percent = 1.0;
        ofPoint p = getPointAtPercent(percent);
        poly.lineTo(p.x, p.y);
    }
    
    if(!isClosed()) {
//        if(poly.size() > 0) poly.points.back() = points.back();
        poly.setClosed(false);
    } else {
        poly.setClosed(true);
    }
    
    
    return poly;
}

//----------------------------------------------------------
// http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
static ofPoint getClosestPointUtil(const ofPoint& p1, const ofPoint& p2, const ofPoint& p3, float* normalizedPosition) {
    // if p1 is coincident with p2, there is no line
    if(p1 == p2) {
        if(normalizedPosition != nullptr) {
            *normalizedPosition = 0;
        }
        return p1;
    }
    
    float u = (p3.x - p1.x) * (p2.x - p1.x);
    u += (p3.y - p1.y) * (p2.y - p1.y);
    // perfect place for fast inverse sqrt...
//    float len = (p2 - p1).length();
//    u /= (len * len);
    float lenSquared = (p2 - p1).lengthSquared();
    u /= lenSquared;
    
    // clamp u
    if(u > 1) {
        u = 1;
    } else if(u < 0) {
        u = 0;
    }
    if(normalizedPosition != nullptr) {
        *normalizedPosition = u;
    }
    return p1.getInterpolated(p2, u);
}

//----------------------------------------------------------
// a much faster but less accurate version would check distances to vertices first,
// which assumes vertices are evenly spaced
ofPoint GUI_Polyline::getClosestPoint(const ofPoint& target, unsigned int* nearestIndex) const {
    const GUI_Polyline & polyline = *this;
    
    if(polyline.size() < 2) {
        if(nearestIndex != nullptr) {
            nearestIndex = 0;
        }
        return target;
    }
    
    float distance = 0;
    ofPoint nearestPoint;
    unsigned int nearest = 0;
    float normalizedPosition = 0;
    unsigned int lastPosition = polyline.size() - 1;
    if(polyline.isClosed()) {
        lastPosition++;
    }
    for(int i = 0; i < (int) lastPosition; i++) {
        bool repeatNext = i == (int) (polyline.size() - 1);
        
        const ofPoint& cur = polyline[i];
        const ofPoint& next = repeatNext ? polyline[0] : polyline[i + 1];
        
        float curNormalizedPosition = 0;
        
        ofPoint curNearestPoint = getClosestPointUtil(cur, next, target, &curNormalizedPosition);
//        int nidx = i;
//        if(curNormalizedPosition>.5){
//            nidx++;
//        }
//        curNearestPoint = polyline[polyline.getWrappedIndex(nidx)];
        
        float curDistance = curNearestPoint.distance(target);
        if(i == 0 || curDistance < distance) {
            distance = curDistance;
            nearest = i;
            nearestPoint = curNearestPoint;
            normalizedPosition = curNormalizedPosition;
        }
    }
    
    if(nearestIndex != nullptr) {
        if(normalizedPosition > .5) {
            nearest++;
            if(nearest == polyline.size()) {
                nearest = 0;
            }
        }
        *nearestIndex = nearest;
    }
    
    return nearestPoint;
}

ofPoint GUI_Polyline::getAccurateClosestPoint(const ofPoint& target, unsigned int* nearestIndex) const{
    if(size() < 2) {
        if(nearestIndex != nullptr) {
            nearestIndex = 0;
        }
        return target;
    }

    unsigned int closestIndex;
    const int numSample = 5;
    getClosestPoint(target, &closestIndex);
    ofPoint closestPoint = points[closestIndex];
    float dist = target.distance(closestPoint);
    unsigned int idx = closestIndex;
    int minIdx = (idx-numSample);
    int maxIdx = (idx+numSample);
    if(isClosed()==false){
        minIdx = MAX(0, minIdx);
        maxIdx = MIN(size()-1, maxIdx);
    }

    for(int i=closestIndex-1; i>=minIdx; i--){
        int wrapIdx = getWrappedIndex(i);
        float d = target.distance(points[wrapIdx]);
        if(d<dist){
            dist = d;
            idx = wrapIdx;
            closestPoint = points[wrapIdx];
        }
    }
    
    for(int i=closestIndex+1; i<=maxIdx; i++){
        int wrapIdx = getWrappedIndex(i);
        float d = target.distance(points[wrapIdx]);
        if(d<dist){
            dist = d;
            idx = wrapIdx;
            closestPoint = points[wrapIdx];
        }
    }
    
    if(nearestIndex!=NULL){
        *nearestIndex = idx;
    }
    return closestPoint;
}

//--------------------------------------------------
bool GUI_Polyline::inside(const ofPoint & p, const GUI_Polyline & polyline){
    return GUI_Polyline::inside(p.x,p.y,polyline);
}

//--------------------------------------------------
bool GUI_Polyline::inside(float x, float y, const GUI_Polyline & polyline){
    guiRectangle bbox = polyline.getBoundingBox();
    if(bbox.isEmpty()==false){
        if(bbox.inside(x, y)==false)
            return false;
    }
    
    int counter = 0;
    size_t i;
    double xinters;
    ofPoint p1,p2;
    
    int N = polyline.size();
    
    p1 = polyline[0];
    for (i=1;i<=N;i++) {
        p2 = polyline[i % N];
        if (y > MIN(p1.y,p2.y)) {
            if (y <= MAX(p1.y,p2.y)) {
                if (x <= MAX(p1.x,p2.x)) {
                    if (p1.y != p2.y) {
                        xinters = ((double)y - p1.y) * ((double)p2.x - p1.x) / ((double)p2.y - p1.y) + p1.x;
                        if (p1.x == p2.x || x <= xinters)
                            counter++;
                    }
                }
            }
        }
        p1 = p2;
    }
    
    if (counter % 2 == 0) return false;
    else return true;
}

//--------------------------------------------------
bool GUI_Polyline::inside(float x, float y) const {
    return GUI_Polyline::inside(x, y, *this);
    
}

//--------------------------------------------------
bool GUI_Polyline::inside(const ofPoint & p) const {
    return GUI_Polyline::inside(p, *this);
}


struct Segment{
    guiVec2f P0;
    guiVec2f P1;
};

static void simplifyDP(float tol, guiVec2f* v, int j, int k, int* mk ){
    if (k <= j+1) // there is nothing to simplify
        return;
//    GUI_Log("static simplifyDP()\n");
    
    // check for adequate approximation by segment S from v[j] to v[k]
    int     maxi	= j;          // index of vertex farthest from S
    float   maxd2	= 0;         // distance squared of farthest vertex
    float   tol2	= tol * tol;        // tolerance squared
    Segment S       = {v[j], v[k]};     // segment from v[j] to v[k]
    auto u			= S.P1 - S.P0;      // segment direction vector
    double  cu		= u.dot(u);         // segment length squared
    
    // test each vertex v[i] for max distance from S
    // compute using the Feb 2001 Algorithm's dist_ofPoint_to_Segment()
    // Note: this works in any dimension (2D, 3D, ...)
    guiVec2f  w;
    guiVec2f  Pb;                // base of perpendicular from v[i] to S
    float  b, cw, dv2;        // dv2 = distance v[i] to S squared
    
    for (int i=j+1; i<k; i++){
        // compute distance squared
        w = v[i] - S.P0;
        cw = w.dot(u);
        if ( cw <= 0 ) dv2 = (v[i] -S.P0).lengthSquared();
        else if ( cu <= cw ) dv2 = (v[i] - S.P1).lengthSquared();
        else {
            b = (float)(cw / cu);
            Pb = S.P0 + u*b;
            dv2 = (v[i] - Pb).lengthSquared();
        }
        // test with current max distance squared
        if (dv2 <= maxd2) continue;
        
        // v[i] is a new max vertex
        maxi = i;
        maxd2 = dv2;
    }
    if (maxd2 > tol2)        // error is worse than the tolerance
    {
        // split the polyline at the farthest vertex from S
        mk[maxi] = 1;      // mark v[maxi] for the simplified polyline
        // recursively simplify the two subpolylines at v[maxi]
        simplifyDP( tol, v, j, maxi, mk );  // polyline v[j] to v[maxi]
        simplifyDP( tol, v, maxi, k, mk );  // polyline v[maxi] to v[k]
    }
    // else the approximation is OK, so ignore intermediate vertices
}

void GUI_Polyline::simplify(float tol){
    if(points.size() < 2) return;
//    GUI_Log("GUI_Polyline::simplify\n");
    size_t n = size();
    
    if(n == 0) {
        return;
    }
    
    vector <guiVec2f> sV;
    sV.resize(n);
    
    size_t i, k, m, pv;      // misc counters
    float  tol2 = tol * tol;       // tolerance squared
    vector<guiVec2f> vt;
    vector<int> mk;
    vt.resize(n);
    mk.resize(n,0);
    
    // STAGE 1.  Vertex Reduction within tolerance of prior vertex cluster
    vt[0] = points[0];              // start at the beginning
    for (i=k=1, pv=0; i<n; i++) {
        guiVec2f diff = points[i] - points[pv];
        if (diff.lengthSquared() < tol2) continue;
        
        vt[k++] = points[i];
        pv = i;
    }
    if (pv < n-1) vt[k++] = points[n-1];      // finish at the end
    
    // STAGE 2.  Douglas-Peucker polyline simplification
    mk[0] = mk[k-1] = 1;       // mark the first and last vertices
    simplifyDP( tol, &vt[0], 0, k-1, &mk[0] );
    
    // copy marked vertices to the output simplified polyline
    for (i=m=0; i<k; i++) {
        if (mk[i]) sV[m++] = vt[i];
    }
    
    //get rid of the unused points
    if( m < (int)sV.size() ){
        points.assign( sV.begin(),sV.begin()+m );
    }else{
        points = sV;
    }
}



//--------------------------------------------------
void GUI_Polyline::setRightVector(ofVec3f v) {
    rightVector = v;
    flagHasChanged();
}

//--------------------------------------------------
ofVec3f GUI_Polyline::getRightVector() const {
    return rightVector;
}


//--------------------------------------------------
float GUI_Polyline::getIndexAtLength(float length) const {
    if(points.size() < 2) return 0;
    updateCache();
    
    float totalLength = getPerimeter();
    length = guiClamp(length, 0, totalLength);
    
    int lastPointIndex = isClosed() ? points.size() : points.size()-1;
    
    size_t i1 = guiClamp(floor(length / totalLength * lastPointIndex), 0.0f, lengths.size()-2.0f);   // start approximation here
    int leftLimit = 0;
    int rightLimit = lastPointIndex;
    
    float distAt1, distAt2;
    if(!lengths.size()){
        GUI_LogError("GUI_Polyline::getIndexAtLength", "again lengths zero size!!! WTF!!!!") << "";
        return 0;
    }
    for(int iterations = 0; iterations < 32; iterations ++) {	// limit iterations
        i1 = (int)guiClamp((float)i1, 0.0f, lengths.size()-1.0f);
        distAt1 = lengths[i1];
        if(distAt1 <= length) {         // if Length at i1 is less than desired Length (this is good)
            distAt2 = lengths[i1+1];
            if(distAt2 >= length) {
                float t = guiMap(length, distAt1, distAt2, 0, 1);
                return i1 + t;
            } else {
                leftLimit = i1;
            }
        } else {
            rightLimit = i1;
        }
        i1 = (leftLimit + rightLimit)/2;
    }
    return 0;
}


//--------------------------------------------------
float GUI_Polyline::getIndexAtPercent(float f) const {
    return getIndexAtLength(f * getPerimeter());
}

//--------------------------------------------------
float GUI_Polyline::getLengthAtIndex(int index) const {
    if(points.size() < 2) return 0;
    updateCache();
    return lengths[getWrappedIndex(index)];
}

//--------------------------------------------------
float GUI_Polyline::getLengthAtIndexInterpolated(float findex) const {
    if(points.size() < 2) return 0;
    updateCache();
    int i1, i2;
    float t;
    getInterpolationParams(findex, i1, i2, t);
    return guiLerp(getLengthAtIndex(i1), getLengthAtIndex(i2), t);
}

//--------------------------------------------------
ofPoint GUI_Polyline::getPointAtLength(float f) const {
    if(points.size() < 2) return ofPoint();
    updateCache();
    return getPointAtIndexInterpolated(getIndexAtLength(f));
}

//--------------------------------------------------
ofPoint GUI_Polyline::getPointAtPercent(float f) const {
    float length = getPerimeter();
    return getPointAtLength(f * length);
}


//--------------------------------------------------
ofPoint GUI_Polyline::getPointAtIndexInterpolated(float findex) const {
    if(points.size() < 2) return ofPoint();
    int i1, i2;
    float t;
    getInterpolationParams(findex, i1, i2, t);
    ofPoint leftPoint(points[i1]);
    ofPoint rightPoint(points[i2]);
    return leftPoint.getInterpolated(rightPoint, t);
}


//--------------------------------------------------
float GUI_Polyline::getAngleAtIndex(int index) const {
    if(points.size() < 2) return 0;
    updateCache();
    return angles[getWrappedIndex(index)];
}

//--------------------------------------------------
float GUI_Polyline::getAngleAtIndexInterpolated(float findex) const {
    if(points.size() < 2) return 0;
    int i1, i2;
    float t;
    getInterpolationParams(findex, i1, i2, t);
    return guiLerp(getAngleAtIndex(i1), getAngleAtIndex(i2), t);
}

//--------------------------------------------------
ofVec3f GUI_Polyline::getRotationAtIndex(int index) const {
    if(points.size() < 2) return ofVec3f();
    updateCache();
    return rotations[getWrappedIndex(index)];
}

//--------------------------------------------------
ofVec3f GUI_Polyline::getRotationAtIndexInterpolated(float findex) const {
    if(points.size() < 2) return ofVec3f();
    int i1, i2;
    float t;
    getInterpolationParams(findex, i1, i2, t);
    return getRotationAtIndex(i1).getInterpolated(getRotationAtIndex(i2), t);
}

//--------------------------------------------------
ofVec3f GUI_Polyline::getTangentAtIndex(int index) const {
    if(points.size() < 2) return ofVec3f();
    updateCache();
    return tangents[getWrappedIndex(index)];
}

//--------------------------------------------------
ofVec3f GUI_Polyline::getTangentAtIndexInterpolated(float findex) const {
    if(points.size() < 2) return ofVec3f();
    int i1, i2;
    float t;
    getInterpolationParams(findex, i1, i2, t);
    return getTangentAtIndex(i1).getInterpolated(getTangentAtIndex(i2), t);
}

//--------------------------------------------------
ofVec3f GUI_Polyline::getNormalAtIndex(int index) const {
    if(points.size() < 2) return ofVec3f();
    updateCache();
    return normals[getWrappedIndex(index)];
}

//--------------------------------------------------
ofVec3f GUI_Polyline::getNormalAtIndexInterpolated(float findex) const {
    if(points.size() < 2) return ofVec3f();
    int i1, i2;
    float t;
    getInterpolationParams(findex, i1, i2, t);
    return getNormalAtIndex(i1).getInterpolated(getNormalAtIndex(i2), t);
}

//--------------------------------------------------
void GUI_Polyline::calcData(int index, ofVec3f &tangent, float &angle, ofVec3f &rotation, ofVec3f &normal) const {
    int i1 = getWrappedIndex(index - 1);
    int i2 = getWrappedIndex(index);
    int i3 = getWrappedIndex(index + 1);
    
    ofPoint p1(points[i1]);
    ofPoint p2(points[i2]);
    ofPoint p3(points[i3]);
    
    ofVec3f v1(p1 - p2); // vector to previous point
    ofVec3f v2(p3 - p2); // vector to next point
    v1.normalize();
    v2.normalize();
    
    tangent = (v2 - v1);
    tangent.normalize();
    
    rotation = v1.getCrossed(v2);
    angle = 180 - guiRadToDeg(acos(guiClamp(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z, -1, 1)));
    
    normal = rightVector.getCrossed(tangent);
    normal.normalize();
}


//--------------------------------------------------
int GUI_Polyline::getWrappedIndex(int index) const {
    if(points.empty()) return 0;
    
    if(index < 0) return isClosed() ? (index + points.size()) % points.size() : 0;
    if(index > int(points.size())-1) return isClosed() ? index % points.size() : points.size() - 1;
    return index;
}

//--------------------------------------------------
void GUI_Polyline::getInterpolationParams(float findex, int &i1, int &i2, float &t) const {
    i1 = (int)floor(findex);
    t = findex - i1;
    i1 = getWrappedIndex(i1);
    i2 = getWrappedIndex(i1 + 1);
}

//--------------------------------------------------
void GUI_Polyline::updateCache(bool bForceUpdate) const {
    if(bCacheIsDirty || bForceUpdate) {
        lengths.clear();
        angles.clear();
        rotations.clear();
        normals.clear();
        tangents.clear();
        area = 0;
        bound.set(0, 0, 0, 0);
        centroid2D.set(0, 0, 0);
        bCacheIsDirty = false;
        
        if(points.size() < 2) return;
        
        // area
        for(size_t i=0;i<points.size()-1;i++){
            area += points[i].x * points[i+1].y - points[i+1].x * points[i].y;
        }
        area += points[points.size()-1].x * points[0].y - points[0].x * points[points.size()-1].y;
        area *= 0.5;
        
        // bounding box
        for(size_t i = 0; i < size(); i++) {
            if(i == 0) {
                bound.set(points[i],0,0);
            } else {
                bound.growToInclude(points[i]);
            }
        }
        
        if(fabsf(area) < FLT_EPSILON) {
            centroid2D = getBoundingBox().getCenter();
        } else {
            // centroid
            // TODO: doesn't seem to work on all concave shapes
            for (size_t i = 0; i < points.size() - 1; i++) {
                centroid2D.x += (points[i].x + points[i+1].x) * (points[i].x*points[i+1].y - points[i+1].x*points[i].y);
                centroid2D.y += (points[i].y + points[i+1].y) * (points[i].x*points[i+1].y - points[i+1].x*points[i].y);
            }
            centroid2D.x += (points[points.size()-1].x + points[0].x) * (points[points.size()-1].x*points[0].y - points[0].x*points[points.size()-1].y);
            centroid2D.y += (points[points.size()-1].y + points[0].y) * (points[points.size()-1].x*points[0].y - points[0].x*points[points.size()-1].y);
            
            centroid2D.x /= (6*area);
            centroid2D.y /= (6*area);
        }
        
        
        // per vertex cache
        lengths.resize(points.size());
        tangents.resize(points.size());
        angles.resize(points.size());
        normals.resize(points.size());
        rotations.resize(points.size());
        
        float angle;
        ofVec3f rotation;
        ofVec3f normal;
        ofVec3f tangent;
        
        float length = 0;
        for(int i=0; i<(int)points.size(); i++) {
            lengths[i] = length;
            
            calcData(i, tangent, angle, rotation, normal);
            tangents[i] = tangent;
            angles[i] = angle;
            rotations[i] = rotation;
            normals[i] = normal;
            
            length += points[i].distance(points[getWrappedIndex(i + 1)]);
        }
        
        if(isClosed()) lengths.push_back(length);
    }
}

vector<GUI_Polyline> GUI_Polyline::getBorder(float strokeWidth) const{
    float halfWidth = strokeWidth*0.5f;
    vector<GUI_Polyline> border;
    vector<guiVec2f> outer;
    vector<guiVec2f> inner;
    outer.resize(size());
    inner.resize(size());
    for(int i=0; i<(int)size(); i++){
        ofVec2f normal = getNormalAtIndex(i);
//        if(getArea()<0)
//            normal = normal * -1;
        outer[i] = points[i] + normal * halfWidth;
        inner[i] = points[i] - normal * halfWidth;
    }
    
    if(outer.size() && inner.size()){
        border.resize(2);
        border[0].add(outer);
        border[1].add(inner);
        if(isClosed()){
            border[0].close();
            border[1].close();
        }
        
        sort(border.begin(), border.end(), [](const GUI_Polyline& p0, const GUI_Polyline& p1)->bool{
            if(fabs(p0.getArea())>fabs(p1.getArea()))
                return true;
            return false;
        });
    }
    return border;
}

//vector<GUI_Polyline> GUI_Polyline::getOffsets(float offset) const{
//    vector<GUI_Polyline> p;
//    p.push_back(*this);
//    return ofx::Clipper::getOffsets(
//                                    p,
//                                    offset,
//                                    ClipperLib::jtSquare, //ClipperLib::jtMiter, jtSquare
//                                    ClipperLib::etClosedLine,
//                                    ofx::Clipper::DEFAULT_MITER_LIMIT,
//                                    ofx::Clipper::DEFAULT_ARC_TOLERANCE,
//                                    ofx::Clipper::DEFAULT_CLIPPER_SCALE
//                                    );
//}

GUI_Polyline GUI_Polyline::getExpand(float compensate) const{
    vector<guiVec2f> outer;
    outer.resize(size());
    for(int i=0; i<(int)size(); i++){
        ofVec2f normal = getNormalAtIndex(i);
        outer[i] = points[i] + normal * compensate;
    }
    
    GUI_Polyline poly(outer);
    if(isClosed()){
        poly.close();
    }
    
    outer.clear();
    return poly;
}

GUI_Polyline GUI_Polyline::getContract(float compensate) const{
    return getExpand(-compensate);
}

GUI_Polyline GUI_Polyline::getShrink(float compensate) const{
    return getGrow(-compensate);
}

GUI_Polyline GUI_Polyline::getGrow(float compensate) const{
    if(isCounterClockWise())
        compensate*=-1;
    
    vector<guiVec2f> outer;
    outer.resize(size());
    for(int i=0; i<(int)size(); i++){
        ofVec2f normal = getNormalAtIndex(i);
        outer[i] = points[i] + normal * compensate;
    }
    
    GUI_Polyline poly(outer);
    if(isClosed()){
        poly.close();
    }
    
    outer.clear();
    return poly;
}

void GUI_Polyline::shrink(float compensate){
    grow(-compensate);
}

void GUI_Polyline::grow(float compensate){
    if(isCounterClockWise())
        compensate*=-1;
    
    for(int i=0; i<(int)size(); i++){
        ofVec2f normal = getNormalAtIndex(i);
        points[i] = points[i] + normal * compensate;
    }
}

bool GUI_Polyline::isClockWise() const{
    return getArea() > 0;
}

bool GUI_Polyline::isCounterClockWise() const{
    return getArea() < 0;
}


// MARK: GUI_Path
//GUI_Path::Command::Command(Type type)
//:type(type){
//    
//}

const string command_type_name[] = {
    "moveTo",
    "lineTo",
    "curveTo",
    "bezierTo",
    "quadBezierTo",
    "arc",
    "arcNegative",
    "close"
};

GUI_Path::Command::Command(Type type)
        : type(type), angleBegin(0), angleEnd(0), radiusX(0), radiusY(0) {
}

//----------------------------------------------------------
GUI_Path::Command::Command(Type type , const ofPoint & p)
    :type(type), angleBegin(0), angleEnd(0), radiusX(0), radiusY(0),
    to(p),
    typeName(command_type_name[(size_t)type]) {
}

//----------------------------------------------------------
GUI_Path::Command::Command(Type type , const ofPoint & p, const ofPoint & cp1, const ofPoint & cp2)
    : type(type), angleBegin(0), angleEnd(0), radiusX(0), radiusY(0),
    to(p),
    cp1(cp1),
    cp2(cp2), typeName(command_type_name[(size_t)type]) {
}

//----------------------------------------------------------
GUI_Path::Command::Command(Type type , const ofPoint & centre, float radiusX, float radiusY, float angleBegin, float angleEnd)
    :type(type),
    to(centre),
    radiusX(radiusX),
    radiusY(radiusY),
    angleBegin(angleBegin),
    angleEnd(angleEnd),
    typeName(command_type_name[(size_t)type]) {
}


GUI_Path::GUI_Path()
:Shaper()
{
    strokeWidth = 0;
    bFill = true;
    mode = Mode::COMMANDS;
    windingMode = PolyWindingMode::POLY_WINDING_ODD;
    bUseShapeColor = true;
    strokeColor = cBlack;
    fillColor = cGrey;
//    cachedTessellationValid = true;
    clear();
}

void GUI_Path::clear(){
    commands.clear();
//    Shaper::clear();
//    orders.clear();
    polylines.resize(1);
    polylines[0].clear();
    cachedTessellation.clear();
    flagShapeChanged();
}

void GUI_Path::setMode(Mode _mode){
    mode = _mode;
}

void GUI_Path::close(bool backhome){
    if(mode==Mode::COMMANDS){
//        if(backhome)
//            addCommand(Command(Command::Type::_lineTo, home));
        
//        addCommand(Command(Command::Type::_close));
        
        int contourId = polylines.size();
        guiVec2f home;// = getOutline()[contourId-1][0];
        addCommand(Command(Command::Type::_close, home));
    }
    else
    {
        lastPolyline().setClosed(true);
    }
    flagShapeChanged();
}

void GUI_Path::lineTo(float x, float y){
    guiVec2f p(x,y);
    if(mode==Mode::COMMANDS){
        addCommand(Command(Command::Type::_lineTo, p));
    }else{
        lastPolyline().lineTo(x, y);
    }
    flagShapeChanged();
}

void GUI_Path::newSubPath(){
    if(mode==Mode::COMMANDS){
    }else{
        polylines.push_back(GUI_Polyline());
    }
}

void GUI_Path::moveTo(float x, float y){
    if(mode==Mode::COMMANDS){
        addCommand(Command(Command::Type::_moveTo, {x, y}));
    }else{
        if(lastPolyline().size()>0) newSubPath();
        lastPolyline().add({x,y});
    }
    flagShapeChanged();
}

void GUI_Path::curveTo(float x, float y){
    if(mode==Mode::COMMANDS){
        addCommand(Command(Command::Type::_curveTo, {x, y}));
    }else{
        lastPolyline().curveTo(x, y,curveResolution);
    }
    flagShapeChanged();
}

void GUI_Path::bezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & p){
    if(mode==Mode::COMMANDS){
        addCommand(Command(Command::Type::_bezierTo, p, cp1, cp2));
    }else{
        lastPolyline().bezierTo(cp1,cp2,p,curveResolution);
    }
    flagShapeChanged();
}

void GUI_Path::quadBezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & p){
    if(mode==Mode::COMMANDS){
        addCommand(Command(Command::Type::_quadBezierTo, p, cp1, cp2));
    }else{
        lastPolyline().quadBezierTo(cp1,cp2,p,curveResolution);
    }
    flagShapeChanged();
}

void GUI_Path::arc(const guiVec2f & centre, float radiusX, float radiusY, float angleBegin, float angleEnd){
    if(mode==Mode::COMMANDS){
        addCommand(Command(Command::Type::_arc, centre, radiusX, radiusY, angleBegin, angleEnd));
    }else{
        lastPolyline().arc(centre,radiusX,radiusY,angleBegin,angleEnd,circleResolution);
    }
    flagShapeChanged();
}

void GUI_Path::arcNegative(const guiVec2f & centre, float radiusX, float radiusY, float angleBegin, float angleEnd){
    if(mode==Mode::COMMANDS){
        addCommand(Command(Command::Type::_arcNegative,centre,radiusX,radiusY,angleBegin,angleEnd));
    }else{
        lastPolyline().arcNegative(centre,radiusX,radiusY,angleBegin,angleEnd,circleResolution);
    }
    flagShapeChanged();
}

void GUI_Path::flagShapeChanged(){
    if(mode==Mode::COMMANDS){
        bHasChanged = true;
        bNeedsPolylinesGeneration = true;
    }else{
        bNeedsTessellation = true;
    }
}

bool GUI_Path::hasChanged(){
    if(mode==Mode::COMMANDS){
        bool changed = bHasChanged;
        bHasChanged = false;
        return changed;
    }else{
        return bNeedsTessellation;
    }
}

//----------------------------------------------------------
GUI_Path::Mode GUI_Path::getMode(){
    return mode;
}

//----------------------------------------------------------
void GUI_Path::simplify(float tolerance){
//    GUI_Log("GUI_Path::simplify\n");
    if(mode==Mode::COMMANDS) generatePolylinesFromCommands();
    for(int i=0;i<(int)polylines.size();i++){
        polylines[i].simplify(tolerance);
    }
}

//----------------------------------------------------------
void GUI_Path::translate(float x, float y){
    guiVec2f p(x,y);
    if(mode==Mode::COMMANDS){
        for(int j=0;j<(int)commands.size();j++){
            commands[j].to += p;
            if(commands[j].type==Command::Type::_bezierTo || commands[j].type==Command::Type::_quadBezierTo){
                commands[j].cp1 += p;
                commands[j].cp2 += p;
            }
        }
    }else{
        for(int i=0;i<(int)polylines.size();i++){
            for(int j=0;j<(int)polylines[i].size();j++){
                polylines[i][j] += p;
            }
        }
    }
    flagShapeChanged();
}

//----------------------------------------------------------
void GUI_Path::scale(float x, float y){
    if(mode==Mode::COMMANDS){
        for(int j=0;j<(int)commands.size();j++){
            commands[j].to.x*=x;
            commands[j].to.y*=y;
            if(commands[j].type==Command::Type::_bezierTo || commands[j].type==Command::Type::_quadBezierTo){
                commands[j].cp1.x*=x;
                commands[j].cp1.y*=y;
                commands[j].cp2.x*=x;
                commands[j].cp2.y*=y;
            }
            if(commands[j].type==Command::Type::_arc || commands[j].type==Command::Type::_arcNegative){
                commands[j].radiusX *= x;
                commands[j].radiusY *= y;
            }
        }
    }else{
        for(int i=0;i<(int)polylines.size();i++){
            for(int j=0;j<(int)polylines[i].size();j++){
                polylines[i][j].x*=x;
                polylines[i][j].y*=y;
            }
        }
    }
    flagShapeChanged();
}

int polyline_sum(const vector<GUI_Polyline> & polylines){
    int sum = 0;
    for(auto p : polylines){
        sum+=p.size();
    }
    return sum;
}

void GUI_Path::generatePolylinesFromCommands(){
    if(mode==Mode::POLYLINES || commands.empty()) return;
//    GUI_Log("GUI_Path::generatePolylinesFromCommands\n");
    if(bNeedsPolylinesGeneration || curveResolution!=prevCurveRes){
        prevCurveRes = curveResolution;
        
        polylines.clear();
        int j=-1;
        
        for(int i=0; i<(int)commands.size();i++){
            switch(commands[i].type){
                case Command::Type::_moveTo:
                    polylines.push_back(GUI_Polyline());
                    j++;
                    polylines[j].add(commands[i].to);
                    break;
                case Command::Type::_lineTo:
                    polylines[j].add(commands[i].to);
                    break;
                case Command::Type::_curveTo:
                    polylines[j].curveTo(commands[i].to.x, commands[i].to.y, curveResolution);
                    break;
                case Command::Type::_bezierTo:
                    polylines[j].bezierTo(commands[i].cp1,commands[i].cp2,commands[i].to, curveResolution);
                    break;
                case Command::Type::_quadBezierTo:
                    polylines[j].quadBezierTo(commands[i].cp1,commands[i].cp2,commands[i].to, curveResolution);
                    break;
                case Command::Type::_arc:
                    polylines[j].arc(commands[i].to,commands[i].radiusX,commands[i].radiusY,commands[i].angleBegin,commands[i].angleEnd, circleResolution);
                    break;
                case Command::Type::_arcNegative:
                    polylines[j].arcNegative(commands[i].to,commands[i].radiusX,commands[i].radiusY,commands[i].angleBegin,commands[i].angleEnd, circleResolution);
                    break;
                case Command::Type::_close:
                    polylines[j].setClosed(true);
                    break;
            }
        }
        
        bNeedsPolylinesGeneration = false;
        bNeedsTessellation = true;
//        cachedTessellationValid=false;
    }
//    GUI_Log("GUI_Path::generatePolylinesFromCommands done\n");
}

void GUI_Path::tessellate(){
//    GUI_Log("GUI_Path::tessellate\n");
    generatePolylinesFromCommands();
//    if(!bNeedsTessellation) return;
    if(!bNeedsTessellation || polylines.empty() || std::all_of(polylines.begin(), polylines.end(), [](const GUI_Polyline & p) {return p.getVertices().empty();}))
        return;
    
//#ifndef __ANDROIDx__
    if(bFill)
    {
//         GUI_Log("GUI_Path::tessellate->tessellateToMesh\n");
        tessellator.tessellateToMesh( polylines, windingMode, cachedTessellation);
//        cachedTessellationValid=true;
    }
    float bkStrokeWidth = strokeWidth;
    strokeWidth = 1.0;
    if(hasOutline() && windingMode!=PolyWindingMode::POLY_WINDING_ODD){
//        GUI_Log("GUI_Path::tessellate->tessellateToPolylines\n");
        tessellator.tessellateToPolylines( polylines, windingMode, tessellatedContour);
    }
    strokeWidth = bkStrokeWidth;
//#else
//    tessellatedContour = polylines;
//#endif
    
    bNeedsTessellation = false;
//    GUI_Log("GUI_Path::tessellate done\n");
}

const vector<GUI_Polyline> & GUI_Path::getOutline() const{
//    GUI_Log("GUI_Path::getOutline\n");
//#ifdef __ANDROID__
//    const_cast<GUI_Path*>(this)->generatePolylinesFromCommands();
//    return polylines;
//#else
    if(windingMode!=PolyWindingMode::POLY_WINDING_ODD){
        const_cast<GUI_Path*>(this)->tessellate();
        return tessellatedContour;
    }else{
        const_cast<GUI_Path*>(this)->generatePolylinesFromCommands();
        return polylines;
    }
//#endif
//    GUI_Log("GUI_Path::getOutline done\n");
}

//vector<GUI_Polyline> GUI_Path::getOutlineOffsets(float offset) const{
//    return ofx::Clipper::getOffsets(getOutline(),
//                                    offset,
//                                    ClipperLib::jtSquare, //jtMiter, jtRound
//                                    ClipperLib::etClosedPolygon,
//                                    ofx::Clipper::DEFAULT_MITER_LIMIT,
//                                    ofx::Clipper::DEFAULT_ARC_TOLERANCE,
//                                    ofx::Clipper::DEFAULT_CLIPPER_SCALE);
//}

//vector<GUI_Polyline> GUI_Path::getOutlineExpand(float offset) const{
//    offset = fabs(offset);
//    vector<GUI_Polyline> offsets = getOutlineOffsets(offset);
//    vector<GUI_Polyline> rt;
//    if(offsets.size()){
//        for(auto os:offsets){
//            ofPoint first = os.getVertices().front();
//            bool bInside = inside(first.x, first.y);
//            if(!bInside){
//                if(fabs(os.getArea())>0){
//                    rt.push_back(os);
//                }
//            }
//        }
//    }
//    return rt;
//}
//
//vector<GUI_Polyline> GUI_Path::getOutlineShrink(float offset) const{
//    offset = fabs(offset);
//    vector<GUI_Polyline> offsets = getOutlineOffsets(offset);
//    vector<GUI_Polyline> rt;
//    if(offsets.size()){
//        for(auto os:offsets){
//            ofPoint first = os.getVertices().front();
//            bool bInside = inside(first.x, first.y);
//            if(bInside){
//                if(fabs(os.getArea())>0){
//                    rt.push_back(os);
//                }
//            }
//        }
//    }
//    std::reverse(rt.begin(), rt.end());
//    return rt;
//}

const guiMesh & GUI_Path::getTessellation() const{
//    GUI_Log("GUI_Path::getTessellation\n");
    const_cast<GUI_Path*>(this)->tessellate();
//    GUI_Log("GUI_Path::getTessellation done\n");
    return cachedTessellation;
}

void GUI_Path::append(const GUI_Path & path){
    if(mode==Mode::COMMANDS){
        for(auto & command: path.getCommands()){
            addCommand(command);
        }
    }else{
        for(auto & poly: path.getOutline()){
            polylines.push_back(poly);
        }
    }
    flagShapeChanged();
}

void GUI_Path::addCommand(const GUI_Path::Command & command){
    if((commands.empty() || commands.back().type==Command::Type::_close) && command.type!=Command::Type::_moveTo){
        commands.push_back(Command(Command::Type::_moveTo, command.to));
    }
    commands.push_back(command);
}

//----------------------------------------------------------
vector<GUI_Path::Command> & GUI_Path::getCommands(){
    if(mode==Mode::POLYLINES){
        GUI_LogWarning("ofPath") << "getCommands(): trying to get path commands from shape with polylines only";
    }else{
        flagShapeChanged();
    }
    return commands;
}

//----------------------------------------------------------
const vector<GUI_Path::Command> & GUI_Path::getCommands() const{
    if(mode==Mode::POLYLINES){
        GUI_LogWarning("ofPath") << "getCommands(): trying to get path commands from shape with polylines only";
    }
    return commands;
}

vector<GUI_Path> GUI_Path::getSubPath(){
    vector<GUI_Path> subPath;
    GUI_Path p;
    for (size_t i = 0; i < commands.size(); i++) {
        if(commands[i].type==GUI_Path::Command::Type::_moveTo && i>1 && commands[i-1].type!=GUI_Path::Command::Type::_close){
            p.flagShapeChanged();
            subPath.push_back(p);
            p.clear();
        }
        
        p.addCommand(commands[i]);
        
        if(commands[i].type==GUI_Path::Command::Type::_close){
            p.flagShapeChanged();
            subPath.push_back(p);
            p.clear();
        }
    }
    if(p.getCommands().size()){
        p.flagShapeChanged();
        subPath.push_back(p);
        p.clear();
    }
    return subPath;
}

//const vector<GUI_Path::Order> & GUI_Path::getOrder() const{
//    return orders;
//}

//void GUI_Path::setFillColor(const SDL_Color & color){
//    fillColor = color;
//}
//
//void GUI_Path::setStrokeColor(const SDL_Color & color){
//    strokeColor = color;
//    bUseShapeColor = true;
//}


void GUI_Path::draw(int x, int y, SDL_Color color) const{
//    GUI_Log("GUI_Path::draw\n");
    if(bUseShapeColor){
        color = strokeColor;
    }
    const vector<GUI_Polyline> & outline = getOutline();
    for(auto & countour : outline){
        countour.draw(x, y, color);
    }
//     GUI_Log("GUI_Path::draw done\n");
}

void GUI_Path::drawMeshColor(int x, int y, SDL_Color color) const {
    GUI_Path * mut_this = const_cast<GUI_Path*>(this);
    if(mut_this->isFill()){
        const guiMesh & mesh = getTessellation();
        mesh.draw(x, y, color);
    }
    if(hasOutline()){
        const vector<GUI_Polyline> & outlines = getOutline();
        ofColor stroke = getStrokeColor();
        for(int i=0; i<(int)outlines.size(); i++)
        {
            outlines[i].draw(x, y, cBlack);
        }
    }
}

// pom
void GUI_Path::drawMesh(int x, int y) const{
    GUI_Path * mut_this = const_cast<GUI_Path*>(this);
    if(mut_this->isFill()){
        const guiMesh & mesh = getTessellation();
        if(getUseShapeColor()){
            ofColor fill = getFillColor();
            SDL_Color color = {fill.r, fill.g, fill.b, fill.a};
            mesh.draw(x, y, color);
        }
        else{
            mesh.draw(x, y, cGrey);
        }
    }
    if(hasOutline()){
        const vector<GUI_Polyline> & outlines = getOutline();
        if(getUseShapeColor()){
            ofColor stroke = getStrokeColor();
            SDL_Color color = {stroke.r, stroke.g, stroke.b, stroke.a};
            for(int i=0; i<(int)outlines.size(); i++)
            {
                outlines[i].draw(x, y, color);
            }
        }
        else{
            for(int i=0; i<(int)outlines.size(); i++)
            {
                outlines[i].draw(x, y, cBlack);
            }
        }
    }
}

//void GUI_Path::draw(int x, int y, SDL_Color _color) const{
//
//    ofColor prevColor;
//    if(getUseShapeColor()){
//        SDL_GetRenderDrawColor(GUI_renderer, &prevColor.r, &prevColor.g, &prevColor.b, &prevColor.a);
//    }
//
//    GUI_Path * mut_this = const_cast<GUI_Path*>(this);
//    if(mut_this->isFill()){
//        const guiMesh & mesh = getTessellation();
//        if(getUseShapeColor()){
//            ofColor fill = getFillColor();
//            SDL_Color color = {fill.r, fill.g, fill.b, fill.a};
//            mesh.draw(x, y, color);
//        }
//        else{
//             mesh.draw(x, y, _color);
//        }
//    }
//
//    if(hasOutline())
//    {
//        const vector<GUI_Polyline> & outlines = getOutline();
//        if(getStrokeWidth()>1){
//            for(int i=0; i<(int)outlines.size(); i++)
//            {
//                const vector<GUI_Polyline> & border = outlines[i].getBorder(getStrokeWidth());
//                GUI_Path borderPath;
//                borderPath.setMode(GUI_Path::Mode::POLYLINES);
//                for(int i=0; i<border[0].size(); i++){
//                    if(i==0)
//                        borderPath.moveTo(border[0][i].x, border[0][i].y);
//                    else
//                        borderPath.lineTo(border[0][i].x, border[0][i].y);
//                }
//                borderPath.close();
//                borderPath.newSubPath();
//                for(int i=0; i<border[1].size(); i++){
//                    if(i==0)
//                        borderPath.moveTo(border[1][i].x, border[1][i].y);
//                    else
//                        borderPath.lineTo(border[1][i].x, border[1][i].y);
//                }
//                borderPath.close();
//                const guiMesh & mesh = borderPath.getTessellation();
//                SDL_Color color = _color;
//                if(getUseShapeColor()){
//                    ofColor c = getStrokeColor();
//                    color = {c.r, c.g, c.b, c.a};
//                }
//                mesh.draw(x,y, color);
//            }
//        }
//        else
//        {
//            SDL_Color color = _color;
//            if(getUseShapeColor()){
//                ofColor c = getStrokeColor();
//                color = {c.r, c.g, c.b, c.a};
//            }
//
////            for(int i=0; i<outlines.size(); i++)
////            {
////                outlines[i].draw(x, y, color);
////            }
//
//            for(auto o:outlines){
//                o.draw(x, y, color);
//            }
//        }
//    }
//}

void GUI_Path::rotate(float az, const ofVec3f& axis ){
    if(mode==Mode::COMMANDS){
        for(int j=0;j<(int)commands.size();j++){
            commands[j].to.rotate(az,axis);
            if(commands[j].type==Command::Type::_bezierTo || commands[j].type==Command::Type::_quadBezierTo){
                commands[j].cp1.rotate(az,axis);
                commands[j].cp2.rotate(az,axis);
            }
            if(commands[j].type==Command::Type::_arc || commands[j].type==Command::Type::_arcNegative){
                commands[j].angleBegin += az;
                commands[j].angleEnd += az;
            }
        }
    }else{
        for(int i=0;i<(int)polylines.size();i++){
            for(int j=0;j<(int)polylines[i].size();j++){
                polylines[i][j].rotate(az,axis);
            }
        }
    }
    flagShapeChanged();
}

void GUI_Path::rotateAround(float degree, const guiVec2f& pivot){
    ofVec3f axis(0,0,1);
    if(mode==Mode::COMMANDS){
        for(int j=0;j<(int)commands.size();j++){
            commands[j].to.rotate(degree, pivot,axis);
            if(commands[j].type==Command::Type::_bezierTo || commands[j].type==Command::Type::_quadBezierTo){
                commands[j].cp1.rotate(degree,pivot,axis);
                commands[j].cp2.rotate(degree,pivot,axis);
            }
            if(commands[j].type==Command::Type::_arc || commands[j].type==Command::Type::_arcNegative){
                commands[j].angleBegin += degree;
                commands[j].angleEnd += degree;
            }
        }
    }else{
        for(int i=0;i<(int)polylines.size();i++){
            for(int j=0;j<(int)polylines[i].size();j++){
                polylines[i][j].rotate(degree,pivot);
            }
        }
    }
    flagShapeChanged();
}

void GUI_Path::print(){
    for(auto c: commands){
        if(c.type==Command::Type::_moveTo){
            GUI_LogNotice("", "path.moveTo( %.0f, %.0f);", c.to.x, c.to.y) << "";
        }
        else if(c.type==Command::Type::_bezierTo){
            GUI_LogNotice("", "path.bezierTo( guiVec2f( %.0f, %.0f), guiVec2f( %.0f, %.0f), guiVec2f( %.0f, %.0f));",
                          c.cp1.x, c.cp1.y, c.cp2.x, c.cp2.y, c.to.x, c.to.y) << "";
        }
    }
}

void GUI_Path::save(std::string filename){
//    std::string writer;
//    writer = "<?xml version=\"1.0\"?>\n";
//    writer.append("<!-- PINN PES 2 SVG Embroidery File -->\n");
//    writer.append("<!-- https://github.com/siharat-th/JSCoreUnity -->\n");
//    writer.append("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.2\" baseProfile=\"tiny\">");
//    
//    
//    writer.append("\n</svg>\n");
//    jsBuffer buff(writer);
//    jsBufferToFile(filename, buff);
}

void GUI_Polyline::draw(int x, int y, SDL_Color color, bool bDrawPoints) const{
//    for(int j=1; j<size(); j++){
//        guiVec2f v0 = points[j-1];
//        guiVec2f v1 = points[j];
//        GUI_DrawLine(GUI_renderer, (int)v0.x+x, (int)v0.y+y, (int)v1.x+x, (int)v1.y+y, color);
//        if(bDrawPoints){
//            if(j==1)
//                GUI_FillCircle(v0.x+x, v0.y+y, 1*GUI_scale, color);
//            GUI_FillCircle(v1.x+x, v1.y+y, 1*GUI_scale, color);
//        }
//    }
//    GUI_Log("GUI_Polyline::draw\n");
    vector<SDL_Point> pts;
    for(auto v:points){
        pts.push_back({(int)round(v.x)+x, (int)round(v.y)+y});
    }
    if(pts.size()){
//        float area = getArea();
//        if(area>=0){
//            color = cRed;
//        }
//        else{
//            color = cGreen;
//        }
        
        SDL_SetRenderDrawColor(GUI_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLines(GUI_renderer, pts.data(), pts.size());
        
        if(bDrawPoints){
            for(auto &v:pts){
               GUI_FillCircle(v.x, v.y, 1*GUI_scale, color);
            }
        }
        pts.clear();
    }
    
    // pom-draw close line
    if(isClosed()){
        guiVec2f v0 = points[size()-1];
        guiVec2f v1 = points[0];
        GUI_DrawLine(GUI_renderer, (int)v0.x+x, (int)v0.y+y, (int)v1.x+x, (int)v1.y+y, color);
    }
//    GUI_Log("GUI_Polyline::draw done\n");
}

void GUI_Polyline::drawSmooth(int x, int y, SDL_Color color) const{
    auto p = getSmoothed(5);
    
    for (size_t j = 1; j < p.size(); j++) {
        guiVec2f v0 = p.points[j-1];
        guiVec2f v1 = p.points[j];
        GUI_DrawLine(GUI_renderer, (int)v0.x+x, (int)v0.y+y, (int)v1.x+x, (int)v1.y+y, color);
    }
    
    // pom-draw close line
    if(isClosed()){
        guiVec2f v0 = p.points[size()-1];
        guiVec2f v1 = p.points[0];
        GUI_DrawLine(GUI_renderer, (int)v0.x+x, (int)v0.y+y, (int)v1.x+x, (int)v1.y+y, color);
    }
    
    p.clear();
}

void GUI_Polyline::drawNormal(int x, int y, SDL_Color color, float length) const{
    SDL_SetRenderDrawColor(GUI_renderer, color.r, color.g, color.b, color.a);
    guiVec2f d((float)x, (float)y);
    for(int i=0; i<(int)size(); i++){
        guiVec2f p0 = points[i] + d;
        guiVec2f n =  getNormalAtIndex(i); //normals[i];
        guiVec2f p1 = p0 + n * length;
        SDL_RenderDrawLine(GUI_renderer, (int)round(p0.x), (int)round(p0.y), (int)round(p1.x), (int)round(p1.y));
    }
}

// unavailable on SDL but for best compatible with others
void GUI_Path::setFilled(bool hasFill){
    if(bFill != hasFill){
        bFill = hasFill;
//        if(!cachedTessellationValid) bNeedsTessellation = true;
        bNeedsTessellation = true;
    }
}

bool GUI_Path::isFill(){
    return bFill;
}

// pom - new
//----------------------------------------------------------
void GUI_Path::setStrokeWidth(float width){
    strokeWidth = width;
}

ofColor GUI_Path::getFillColor() const{
    return _fillColor;
}

ofColor GUI_Path::getStrokeColor() const{
    return _strokeColor;
}

float GUI_Path::getStrokeWidth() const{
    return strokeWidth;
}

//----------------------------------------------------------
bool GUI_Path::getUseShapeColor() const {
    return bUseShapeColor;
}

//----------------------------------------------------------
void GUI_Path::setUseShapeColor(bool useColor){
    bUseShapeColor = useColor;
}

//----------------------------------------------------------
void GUI_Path::setColor( const ofColor& color ) {
    setFillColor( color );
    setStrokeColor( color );
}

//----------------------------------------------------------
void GUI_Path::setHexColor( int hex ) {
    setColor( ofColor().fromHex( hex ) );
}

//----------------------------------------------------------
void GUI_Path::setFillColor(const ofColor & color){
    setUseShapeColor(true);
    _fillColor = color;
    fillColor = {_fillColor.r, _fillColor.g, _fillColor.b, _fillColor.a};
}

//----------------------------------------------------------
void GUI_Path::setFillHexColor( int hex ) {
    setFillColor( ofColor().fromHex( hex ) );
}

//----------------------------------------------------------
void GUI_Path::setStrokeColor(const ofColor & color){
    setUseShapeColor(true);
    _strokeColor = color;
    strokeColor = {color.r, color.g, color.b, color.a};
}

//----------------------------------------------------------
void GUI_Path::setStrokeHexColor( int hex ) {
    setStrokeColor( ofColor().fromHex( hex ) );
};

bool GUI_Path::inside(float x, float y) const{
//    vector<GUI_Polyline> outlines = getOutline();
//    int size = (int)outlines.size();
//    if(size){
//        GUI_Polyline poly;
//        for(int i=0;i<size;i++){
//            if(outlines[i].isClosed()){
//                outlines[i].add(outlines[i][0]);
//            }
//            outlines[i].simplify();
//            poly.add(outlines[i].getVertices());
//        }
//        return poly.inside(x, y);
//    }
//    return false;
    
    const vector<GUI_Polyline> & outlines = getOutline();
    int size = (int)outlines.size();
    if(size){
        GUI_Polyline poly;
        for(int i=0;i<size;i++){
            poly.add(outlines[i].getVertices());
            if(outlines[i].isClosed()){
                poly.add(outlines[i][0]);
            }
        }
        return poly.inside(x, y);
    }
    return false;
}

void GUI_Path::debug(){
    vector<GUI_Polyline> outlines = getOutline();
    int size = (int)outlines.size();
    if(size){
        for (size_t i = 0; i < size; i++) {
            outlines[i].simplify();
            float sum = 0;
            for (size_t j = 0; j < outlines[i].size(); j++) {
                ofPoint v2 = outlines[i][(j+1)%outlines[i].size()];
                ofPoint v1 = outlines[i][j];
                sum+= (v2.x-v1.x)*(v2.y+v1.y);
            }
            if(sum>=0)
                GUI_LogNotice("Outline", "%d is cw", i) << "";
            else
                GUI_LogNotice("Outline", "%d is ccw", i) << "";
        }
    }
}

guiRectangle GUI_Path::getBoundingBox() const{
    guiRectangle bound;
    bool first=true;
    for(auto o:getOutline()){
        if(first){
            bound = o.getBoundingBox();
            first = false;
        }
        else{
            bound.growToInclude(o.getBoundingBox());
        }
    }
    return bound;
}



vector<GUI_Path> vPPath2vPath( vector<GUI_Path *>v ) {
    vector<GUI_Path> v2;
    for( auto p : v ) {
        v2.push_back( *p );
    }
    return v2;
}

vector<GUI_Path *> vPath2vPPath( vector<GUI_Path>v ) {
    vector<GUI_Path *> v2;
    for( auto p : v ) {
        GUI_Path *pp = new GUI_Path();
        *pp = p;
        v2.push_back( pp );
    }
    return v2;
}

void vPathClear( vector<GUI_Path *> &v ) {
    for( auto p : v ) {
        delete( p );
    }
    v.clear();
}

void vPathCopy( vector<GUI_Path *> &src, vector<GUI_Path *> &dest ) {
    vPathClear(dest); // Jimmy ** careful - this cause problem if its duplicated object
    for( auto pp : src ) {
        GUI_Path *p = new GUI_Path();
        *p = *pp;
        dest.push_back(p);
    }
}



