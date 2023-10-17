//
//  Shaper.hpp
//  FontWorks
//
//  Created by SIHARAT THAMMAYA on 1/27/17.
//  Copyright © 2017 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef Shaper_hpp
#define Shaper_hpp

#include "tesselator.h"
#include <stdio.h>
#include "SDL_gui.h"
#include "guiRectangle.hpp"
#include "guiMesh.hpp"
#include "ofColor.h"

#define  	CIRC_RESOLUTION		    22

/// \brief represents the available polygon winding modes.
///
/// \sa http://glprogramming.com/red/images/Image128.gif
/// \sa http://glprogramming.com/red/chapter11.html

//enum TessWindingRule
//{
//    TESS_WINDING_ODD,
//    TESS_WINDING_NONZERO,
//    TESS_WINDING_POSITIVE,
//    TESS_WINDING_NEGATIVE,
//    TESS_WINDING_ABS_GEQ_TWO,
//};

enum struct PolyWindingMode {
    /// \brief Fill odd winding numbers.
    POLY_WINDING_ODD = TESS_WINDING_ODD,
    /// \brief Fill all non-zero winding numbers.
    POLY_WINDING_NONZERO = TESS_WINDING_NONZERO ,
    /// \brief Fill all winding numbers greater than zero.
    POLY_WINDING_POSITIVE = TESS_WINDING_POSITIVE,
    /// \brief Fill all winding numbers less than zero.
    POLY_WINDING_NEGATIVE = TESS_WINDING_NEGATIVE,
    /// \brief Fill all winding numbers greater than 1 or less than -1.
    ///
    /// This stands for "Fill ABSolute values Greater than EQual to TWO".
    POLY_WINDING_ABS_GEQ_TWO = TESS_WINDING_ABS_GEQ_TWO
};

class GUI_Polyline{
public:
    GUI_Polyline();
    GUI_Polyline(const vector<guiVec2f>& points);

    GUI_Polyline(const GUI_Polyline&) = default;
    GUI_Polyline(GUI_Polyline&&) noexcept = default;
    GUI_Polyline& operator=(const GUI_Polyline&) = default;
    GUI_Polyline& operator=(GUI_Polyline&&) = default;    
    
    static GUI_Polyline fromRect(const GUI_Rect& rect);
    
    void clear();
    void close();
    void setClosed( bool tf );
    bool isClosed() const;
    
    void add(const guiVec2f& point);
    void add(const guiVec2f* verts, int numverts);
    void add(const vector<guiVec2f>& points);
    void add(float x, float y);
    
    void insert(const guiVec2f& point, int index);
    void insert(const vector<guiVec2f>& points, int index);
    
    void resize(size_t size);
    size_t size() const;
    
    const guiVec2f& operator[] (int index) const;
    guiVec2f& operator[] (int index);
    
    vector<guiVec2f> & getVertices();
    const vector<guiVec2f> & getVertices() const;
    
    
    vector<guiVec2f>::iterator begin();
    vector<guiVec2f>::const_iterator begin() const;
    vector<guiVec2f>::reverse_iterator rbegin();
    vector<guiVec2f>::const_reverse_iterator rbegin() const;
    vector<guiVec2f>::iterator end();
    vector<guiVec2f>::const_iterator end() const;
    vector<guiVec2f>::reverse_iterator rend();
    vector<guiVec2f>::const_reverse_iterator rend() const;
    
    void lineTo(float x, float y);
    
    void curveTo(float x, float y, int curveResolution=20);
    
    void bezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & p, int curveResolution=20);
    
    void quadBezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & p, int curveResolution=20);
    
    
    ///////////////////////////////////////////////////////////
    void cubicTo(float x, float y, int curveResolution=20);
    void catmullTo(float x, float y, int curveResolution=20);
    void hermiteTo(float x, float y, int curveResolution=20);
    ///////////////////////////////////////////////////////////
    
    void arc(const guiVec2f & centre, float radiusX, float radiusY, float angleBegin, float angleEnd, int circleResolution = 20, bool bClockwise=true);
    void arcNegative(const guiVec2f & centre, float radiusX, float radiusY, float angleBegin, float angleEnd, int circleResolution = 20);
    
    bool hasChanged();
    void flagHasChanged();
    
    void simplify(float tol=0.2);
    
    /// \brief Gets a smoothed version of the ofPolyline.
    ///
    /// `smoothingSize` is the size of the smoothing window. So if
    /// `smoothingSize` is 2, then 2 points from the left, 1 in the center,
    /// and 2 on the right (5 total) will be used for smoothing each point.
    /// `smoothingShape` describes whether to use a triangular window (0) or
    /// box window (1) or something in between (for example, .5).
    GUI_Polyline getSmoothed(int smoothingSize, float smoothingShape = 0) const;
    
    /// \brief Resamples the line based on the spacing passed in. The larger the
    /// spacing, the more points will be eliminated.
    ///
    /// ~~~~{.cpp}
    /// line.draw();
    /// ofTranslate(400, 0);
    /// line.getResampledBySpacing(100).draw();
    /// ~~~~
    /// ![polyline resample](graphics/resample.jpg)
    GUI_Polyline getResampledBySpacing(float spacing) const;
    
    /// \brief Resamples the line based on the count passed in. The lower the
    /// count passed in, the more points will be eliminated.
    ///
    /// This doesn't add new points to the line.
    GUI_Polyline getResampledByCount(int count) const;
    GUI_Polyline getResampledByCount2(int count) const;
    
    /// \brief Tests whether the x,y coordinates are within a closed ofPolyline.
    static bool inside(float x, float y, const GUI_Polyline & polyline);
    /// \brief Tests whether the x,y coordinates are within a closed ofPolyline.
    bool inside(float x, float y) const;
    
    /// \brief Tests whether the ofPoint is within a closed ofPolyline.
    static bool inside(const ofPoint & p, const GUI_Polyline & polyline);
    /// \brief Tests whether the ofPoint is within a closed ofPolyline.
    bool inside(const ofPoint & p) const;
    
    /// \brief Get the bounding box of the polyline , taking into account
    /// all the points to determine the extents of the polyline.
    guiRectangle getBoundingBox() const;
    
    /// \brief Gets the size of the perimeter of the polyline, good for
    /// determining length of the line, rather than just the bounding box
    /// shape.
    float getPerimeter() const;
    
    /// \brief Gets the precise area bounded by the line
    float getArea() const;
    
    /// \brief Get the center of the area bounded by the line
    ofPoint getCentroid2D() const;
    
    /// \brief Gets the point on the line closest to the target. You can also
    /// optionally pass a pointer to/address of an unsigned int to get the
    /// index of the closest vertex
    ofPoint getClosestPoint(const ofPoint& target, unsigned int* nearestIndex = nullptr) const;
    
    // pom
    ofPoint getAccurateClosestPoint(const ofPoint& target, unsigned int* nearestIndex = nullptr) const;
    
    
    /// \}
    /// \name Other Functions
    /// \{
    
    /// \brief Get (interpolated) index at given length along the path
    ///
    /// Includes info on percentage along segment, e.g. `ret=5.75` =>
    /// 75% along the path between 5th and 6th points
    float getIndexAtLength(float f) const;
    
    /// \brief Get (interpolated) index at given percentage along the path
    ///
    /// Includes info on percentage along segment, e.g. `ret=5.75`
    /// => 75% along the path between 5th and 6th points
    float getIndexAtPercent(float f) const;
    
    /// \brief Get length along path at index
    float getLengthAtIndex(int index) const;
    
    /// \brief Get length along path at interpolated index (e.g. `f=5.75` => 75% along
    /// the path between 5th and 6th points)
    float getLengthAtIndexInterpolated(float findex) const;
    
    /// \brief Get point long the path at a given length (e.g. `f=150` => 150
    /// units along the path)
    ofPoint getPointAtLength(float f) const;
    
    /// \brief Get point along the path at a given percentage (e.g. `f=0.25`
    /// => 25% along the path)
    ofPoint getPointAtPercent(float f) const;
    
    /// \brief Get point along the path at interpolated index (e.g. `f=5.75` =>
    /// 75% along the path between 5th and 6th points)
    ofPoint getPointAtIndexInterpolated(float findex) const;
    
    /// \brief Get angle (degrees) of the path at index
    float getAngleAtIndex(int index) const;
    
    /// \brief Get angle (degrees) at interpolated index (interpolated between
    /// neighboring indices)
    float getAngleAtIndexInterpolated(float findex) const;
    
    /// \brief Get rotation vector at index (magnitude is sin of angle)
    ofVec3f getRotationAtIndex(int index) const;
    
    /// \brief Get rotation vector at interpolated index
    /// (interpolated between neighboring indices) (magnitude is sin of angle)
    ofVec3f getRotationAtIndexInterpolated(float findex) const;
    
    /// \brief Get tangent vector at index
    ofVec3f getTangentAtIndex(int index) const;
    
    /// \brief Get tangent vector at interpolated index
   	/// (interpolated between neighboring indices)
    ofVec3f getTangentAtIndexInterpolated(float findex) const;
    
    /// \brief Get normal vector at index
    ofVec3f getNormalAtIndex(int index) const;
    
    /// \brief Get normal vector at interpolated index
    /// (interpolated between neighboring indices)
    ofVec3f getNormalAtIndexInterpolated(float findex) const;
    
    int getWrappedIndex(int index) const;
    
    // used for calculating the normals
    void setRightVector(ofVec3f v = ofVec3f(0, 0, -1));
    ofVec3f getRightVector() const;
    
    void updateCache(bool bForceUpdate = false) const;
    
    void draw(int x=0, int y=0, SDL_Color color=cBlack, bool bDrawPoints=false) const;
    void drawSmooth(int x=0, int y=0, SDL_Color color=cBlack) const;
    void drawNormal(int x=0, int y=0, SDL_Color color=cRed, float length=25) const;
    
    deque<guiVec2f> curveVertices;
    
    vector<GUI_Polyline> getBorder(float strokeWidth) const;
    
    GUI_Polyline getExpand(float compensate) const;
    GUI_Polyline getContract(float compensate) const;
    
    GUI_Polyline getShrink(float compensate) const;
    GUI_Polyline getGrow(float compensate) const;
    void shrink(float compensate);
    void grow(float compensate);
    
    bool isClockWise() const;
    bool isCounterClockWise() const;
    
    vector<GUI_Polyline> getOffsets(float offset) const;
    
private:
    void setCircleResolution(int res);
    float wrapAngle(float angleRad);
    
    vector<guiVec2f> points;
//    deque<guiVec2f> curveVertices;
    vector<guiVec2f> circlePoints;
    ofVec3f rightVector;
    
    // cache
    mutable vector<float> lengths;    // cumulative lengths, stored per point (lengths[n] is the distance to the n'th point, zero based)
    mutable vector<ofVec3f> tangents;   // tangent at vertex, stored per point
    mutable vector<ofVec3f> normals;    //
    mutable vector<ofVec3f> rotations;   // rotation between adjacent segments, stored per point (cross product)
    mutable vector<float> angles;    // angle (degrees) between adjacent segments, stored per point (asin(cross product))
    mutable ofPoint centroid2D;
    mutable float area;
    mutable guiRectangle bound;
    
    bool bClosed;
    bool bHasChanged;
    mutable bool bCacheIsDirty;   // used only internally, no public API to read
    
//    void updateCache(bool bForceUpdate = false) const;
    
    // given an interpolated index (e.g. 5.75) return neighboring indices and interolation factor (e.g. 5, 6, 0.75)
    void getInterpolationParams(float findex, int &i1, int &i2, float &t) const;
    
    void calcData(int index, ofVec3f &tangent, float &angle, ofVec3f &rotation, ofVec3f &normal) const;
    
    
    
};

class Tessellator{
public:
    Tessellator();
    ~Tessellator();
    
    Tessellator(const Tessellator & mom);
    
    Tessellator & operator=(const Tessellator & mom);
    
    void tessellateToMesh( const vector<GUI_Polyline>& src, PolyWindingMode polyWindingMode, guiMesh & dstmesh, bool bIs2D=true );
    void tessellateToMesh( const GUI_Polyline& src,  PolyWindingMode polyWindingMode, guiMesh& dstmesh, bool bIs2D=true );
    
    /// Polyline instances using the winding mode set in PolyWindingMode.
    void tessellateToPolylines( const vector<GUI_Polyline>& src, PolyWindingMode polyWindingMode, vector<GUI_Polyline>& dstpoly, bool bIs2D=true );
    
    /// \brief Tessellate multiple polylines into a single polyline.
    void tessellateToPolylines( const GUI_Polyline & src, PolyWindingMode polyWindingMode, vector<GUI_Polyline>& dstpoly, bool bIs2D=true );
    
private:
    
    void performTessellation(PolyWindingMode polyWindingMode, guiMesh& dstmesh, bool bIs2D);
    void performTessellation(PolyWindingMode polyWindingMode, vector<GUI_Polyline>& dstpoly, bool bIs2D);
    void init();
    
    TESStesselator * cacheTess;
    TESSalloc tessAllocator;
};

class Shaper {
    
public :
    Shaper();
    
    void clear();
    
    void close(bool backhome=false);
    
    void lineTo(float x, float y);
    
    void moveTo(float x, float y);
    
    void curveTo(float x, float y);
    
    void bezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & p);

    void quadBezierTo(float cx1, float cy1, float cx2, float cy2, float cx3, float cy3);
    void quadBezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & p);
    
    void arc(const guiVec2f & centre, float radiusX, float radiusY, float angleBegin, float angleEnd);
    void arcNegative(const guiVec2f & centre, float radiusX, float radiusY, float angleBegin, float angleEnd);
    
    void triangle(const guiVec2f & p1, const guiVec2f & p2, const guiVec2f & p3);
    
    void circle(const guiVec2f & p, float radius);
    
    void ellipse(const guiVec2f & p, float width, float height);
    
    void rectangle(float x,float y,float w,float h);
    
    void rectRounded(float x, float y, float w, float h, float r);
    
    void rectRounded(const guiRectangle & b, float topLeftRadius,
                float topRightRadius,
                float bottomRightRadius,
                float bottomLeftRadius);
    
    void rectRounded(float x, float y, float w, float h, float topLeftRadius,
                             float topRightRadius,
                             float bottomRightRadius,
                             float bottomLeftRadius);
    
    
    void setCurveResolution(int curveResolution);
    int getCurveResolution() const;
    
    void setCircleResolution(int circleResolution);
    int getCircleResolution() const;
    
    const vector<GUI_Polyline> & getOutline() const;
    
    void tessellate();
    
    void setPolyWindingMode(PolyWindingMode newMode);
    PolyWindingMode getPolyWindingMode();
    
    void simplify(float tol);
    void scale(float x, float y);
    void translate(int x, int y);
    
//    const GUI_Polyline & lastPolyline() const;
    GUI_Polyline & lastPolyline();
    
protected:
    void flagShapeChanged();
    bool hasChanged();
    
    
    PolyWindingMode 	windingMode;
    // polyline / tessellation
    vector<GUI_Polyline>  polylines;
    vector<GUI_Polyline>  tessellatedContour;
    
#ifdef __EMSCRIPTEN__
    static Tessellator tessellator;
#elif HAS_TLS
    static thread_local Tessellator tessellator;
#else
    Tessellator tessellator;
#endif
    
    bool				bHasChanged;
    int					prevCurveRes;
    int					curveResolution;
    int					circleResolution;
    bool 				bNeedsTessellation;
    bool				bNeedsPolylinesGeneration;
    
    
};

class GUI_Path : public Shaper{
public:
    GUI_Path();
    virtual ~GUI_Path() {}
    
    void append(const GUI_Path & path);
    
    // override super method
    virtual void clear() ;
    void close(bool backhome=false);
    
    void lineTo(float x, float y);
    
    void moveTo(float x, float y);
    
    void curveTo(float x, float y);
    
    void bezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & p);
    
    void quadBezierTo(const guiVec2f & cp1, const guiVec2f & cp2, const guiVec2f & p);
    
    void arc(const guiVec2f & centre, float radiusX, float radiusY, float angleBegin, float angleEnd);
    void arcNegative(const guiVec2f & centre, float radiusX, float radiusY, float angleBegin, float angleEnd);
    
    void simplify(float tol);
    void scale(float x, float y);
    void translate(float x, float y);
    void flagShapeChanged();
    bool hasChanged();
    
    void newSubPath();
    
    
    
    // unavailable on SDL but for best compatible with others
    // now supported by sdl2_gfx fillPolygon
    void setFilled(bool hasFill);
    bool isFill();
    bool isStroke() const { return hasOutline() && strokeColor.a!=0; }
    
    void setStrokeWidth(float width); // default 0
    void setColor( const ofColor& color );
    void setHexColor( int hex );
    void setFillColor(const ofColor & color);
    void setFillHexColor(int hex);
    void setStrokeColor(const ofColor & color);
    void setStrokeHexColor( int hex );
    void setUseShapeColor(bool useColor);
    bool getUseShapeColor() const;
    ofColor getFillColor() const;
    ofColor getStrokeColor() const;
    float getStrokeWidth() const;
    
    bool hasOutline() const { return strokeWidth>0; }
    
    
    
    float strokeWidth;
    bool  bFill;
    bool  bUseShapeColor;

    
    
//    enum Interpolate{
//        CATMUL_CLARK,
//        CATMUL_ROM,
//        HERMITE
//    };
//    
//    void setInterpolate(Interpolate interpolate);
//    Interpolate getInterpolateType();
    
    enum struct Mode {
        COMMANDS,
        POLYLINES
    };
    
    void setMode(Mode mode);
    Mode getMode();
    
    struct Command{
        enum class Type{
            _moveTo = 0,
            _lineTo,
            _curveTo,
            _bezierTo,
            _quadBezierTo,
            _arc,
            _arcNegative,
            _close
        };
        
        /// for close
        Command(Type type);
        
        // for close, lineTo and curveTo
        Command(Type type , const ofPoint & p);
        
        /// for bezierTo
        Command(Type type , const ofPoint & p, const ofPoint & cp1, const ofPoint & cp2);
        
        ///for arc
        Command(Type type , const ofPoint & centre, float radiusX, float radiusY, float angleBegin, float angleEnd);
        
        
        Type type;
        ofPoint to;
        ofPoint cp1, cp2;
        float radiusX, radiusY, angleBegin, angleEnd;
        string typeName;
    };
    
    vector<Command> & getCommands();
    const vector<Command> & getCommands() const;
    const vector<GUI_Polyline> & getOutline() const;
    
//    vector<GUI_Polyline> getOutlineOffsets(float offset) const;
//    vector<GUI_Polyline> getOutlineExpand(float offset) const;
//    vector<GUI_Polyline> getOutlineShrink(float offset) const;
    
    vector<GUI_Path> getSubPath();
    
    
    
    virtual void tessellate();
    virtual const guiMesh & getTessellation() const;
    
    
    
    void draw(int x=0, int y=0, SDL_Color color=cBlack) const;
    void drawMesh(int x=0, int y=0) const;
    void drawMeshColor(int x=0, int y=0, SDL_Color color=cClear) const;

    bool inside(float x, float y) const;
    void debug();
    
    void print();
    void save(std::string filename="GUI_Path.svg");
    
    struct Order{
        int commandId;
        int countourId;
        int pointId;
        int vertexId;
        
        Order() {
            commandId = 0;
            countourId = 0;
            pointId = 0;
            vertexId = 0;
        }
        Order(int _commandId,
              int _countourId,
              int _pointId,
              int _vertexId){
            commandId = _commandId;
            countourId = _countourId;
            pointId = _pointId;
            vertexId = _vertexId;
        }
    };
    
    void addCommand(const Command & command);
    
    void rotate(float az, const ofVec3f& axis );
    void rotateAround(float degree, const guiVec2f& pivot);
    
    guiRectangle getBoundingBox() const;
    
    SDL_Color strokeColor;
    SDL_Color fillColor;
    ofColor _fillColor;
    ofColor _strokeColor;
    
    vector<Command>     commands;
    Mode                mode;
private:
    void generatePolylinesFromCommands();
    
    guiMesh             cachedTessellation;
};

vector<GUI_Path> vPPath2vPath( vector<GUI_Path *>v );
vector<GUI_Path *> vPath2vPPath( vector<GUI_Path>v );
void vPathClear( vector<GUI_Path *> &v );
void vPathCopy( vector<GUI_Path *> &src, vector<GUI_Path *> &dest );
//#include "GUI_Path.inl"

#endif /* Shaper_hpp */
