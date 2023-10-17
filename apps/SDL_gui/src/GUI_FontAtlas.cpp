//
//  GUI_FontAtlas.cpp
//  GUITextField
//
//  Created by SIHARAT THAMMAYA on 4/29/17.
//  Copyright © 2017 Jimmy Software Co., Ltd. All rights reserved.
//

#include "GUI_FontAtlas.hpp"
#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_STROKER_H
#include FT_GLYPH_H
#include FT_TRUETYPE_IDS_H

#include <SDL_ttf.h>
#include <algorithm>
#include <numeric>
#include "GUI_UnicodeThai.hpp"

#define GLM_SWIZZLE
#define GLM_FORCE_SIZE_FUNC
#include "glm/glm.hpp"
#include "glm/ext.hpp"

// MARK: GUI_Unicode
const GUI_Unicode::range GUI_Unicode::Space {32, 32};
const GUI_Unicode::range GUI_Unicode::Latin {32, 0x007F};
const GUI_Unicode::range GUI_Unicode::Latin1Supplement {32,0x00FF};
const GUI_Unicode::range GUI_Unicode::LatinExtendedAdditional {0x1E00, 0x1EFF};

const GUI_Unicode::range GUI_Unicode::CurrencySymbols {0x20A0, 0x20CF};
const GUI_Unicode::range GUI_Unicode::LetterLikeSymbols {0x2100, 0x214F};
const GUI_Unicode::range GUI_Unicode::NumberForms {0x2150, 0x218F};
const GUI_Unicode::range GUI_Unicode::Arrows {0x2190, 0x21FF};
const GUI_Unicode::range GUI_Unicode::MathOperators {0x2200, 0x22FF};

const GUI_Unicode::range GUI_Unicode::GeometricShapes {0x25A0, 0x25FF};
const GUI_Unicode::range GUI_Unicode::MiscSymbols {0x2600, 0x26FF};

const GUI_Unicode::range GUI_Unicode::Thai {0x0E00, 0x0E7F};
const GUI_Unicode::range GUI_Unicode::ThaiExtended {0xF700, 0xF719};

const GUI_Unicode::range GUI_Unicode::MiscSymbolsAndPictographs {0x1F300, 0x1F5FF};
const GUI_Unicode::range GUI_Unicode::Emoticons {0x1F600, 0x1F64F};
const GUI_Unicode::range GUI_Unicode::TransportAndMap {0x1F680, 0x1F6FF};

// MARK: GUI_Alphabet
const std::initializer_list<GUI_Unicode::range> GUI_Alphabet::Latin {
    GUI_Unicode::Latin1Supplement,
    GUI_Unicode::LatinExtendedAdditional
};

const std::initializer_list<GUI_Unicode::range> GUI_Alphabet::Thai {
    GUI_Unicode::Space,
    GUI_Unicode::Thai,
    GUI_Unicode::ThaiExtended
};

const std::initializer_list<GUI_Unicode::range> GUI_Alphabet::Emoji {
    GUI_Unicode::Space,
    GUI_Unicode::Emoticons
};

const GUI_FontAtlas::glyphProps GUI_FontAtlas::invalidProps{
    0,
    0,
    0,
    0,
    0,0,
    0,0,0,0,
    0,
    0,0,
    0,0,
};

/* Cached glyph information */
typedef struct cached_glyph {
    int stored;
    FT_UInt index;
    FT_Bitmap bitmap;
    FT_Bitmap pixmap;
    int minx;
    int maxx;
    int miny;
    int maxy;
    int yoffset;
    int advance;
    Uint16 cached;
} c_glyph;

/* The structure used to hold internal font information */
struct _TTF_Font {
    /* Freetype2 maintains all sorts of useful info itself */
    FT_Face face;
    
    /* We'll cache these ourselves */
    int height;
    int ascent;
    int descent;
    int lineskip;
    
    /* The font style */
    int face_style;
    int style;
    int outline;
    
    /* Whether kerning is desired */
    int kerning;
    
    /* Extra width in glyph bounds for text styles */
    int glyph_overhang;
    float glyph_italics;
    
    /* Information in the font for underlining */
    int underline_offset;
    int underline_height;
    
    /* Cache for style-transformed glyphs */
    c_glyph *current;
    c_glyph cache[257]; /* 257 is a prime */
    
    /* We are responsible for closing the font stream */
    SDL_RWops *src;
    int freesrc;
    FT_Open_Args args;
    
    /* For non-scalable formats, we must remember which font index size */
    int font_size_family;
    
    /* really just flags passed into FT_Load_Glyph */
    int hinting;
};

#define NUM_GRAYS       256

#define FT_FLOOR(X) ((X & -64) / 64)
#define FT_CEIL(X)  (((X + 63) & -64) / 64)

#define CACHED_METRICS  0x10
#define CACHED_BITMAP   0x01
#define CACHED_PIXMAP   0x02

/* Handle a style only if the font does not already handle it */
#define TTF_HANDLE_STYLE_BOLD(font) (((font)->style & TTF_STYLE_BOLD) && \
!((font)->face_style & TTF_STYLE_BOLD))
#define TTF_HANDLE_STYLE_ITALIC(font) (((font)->style & TTF_STYLE_ITALIC) && \
!((font)->face_style & TTF_STYLE_ITALIC))
#define TTF_HANDLE_STYLE_UNDERLINE(font) ((font)->style & TTF_STYLE_UNDERLINE)
#define TTF_HANDLE_STYLE_STRIKETHROUGH(font) ((font)->style & TTF_STYLE_STRIKETHROUGH)

/* Font styles that does not impact glyph drawing */
#define TTF_STYLE_NO_GLYPH_CHANGE   (TTF_STYLE_UNDERLINE | TTF_STYLE_STRIKETHROUGH)

void Flush_Glyph( c_glyph* glyph );
void Flush_Cache( TTF_Font* font );
FT_Error Load_Glyph( TTF_Font* font, Uint16 ch, c_glyph* cached, int want, FT_Int charIndex=-1 );
FT_Error Find_Glyph( TTF_Font* font, Uint16 ch, int want, FT_Int charIndex=-1 );


static bool printVectorInfo = false;
static int TTF_byteswapped = 0;
//static guiTTFCharacter makeContoursForCharacter(FT_Face face, std::vector<guiVec2f> *keypoint=NULL){
//    
//    //int num			= face->glyph->outline.n_points;
//    int nContours	= face->glyph->outline.n_contours;
//    int startPos	= 0;
//    
//    char * tags		= face->glyph->outline.tags;
//    FT_Vector * vec = face->glyph->outline.points;
//    
//    guiTTFCharacter charOutlines;
//    std::vector<guiVec2f> keys;
//    
//    for(int k = 0; k < nContours; k++){
//        if( k > 0 ){
//            startPos = face->glyph->outline.contours[k-1]+1;
//        }
//        int endPos = face->glyph->outline.contours[k]+1;
//        
//        if(printVectorInfo){
//            GUI_LogNotice("guiTTFCharacter") << "--NEW CONTOUR";
//        }
//        
//        //vector <ofPoint> testOutline;
//        glm::vec3 lastPoint;
//        
//        for(int j = startPos; j < endPos; j++){
//            
//            if( FT_CURVE_TAG(tags[j]) == FT_CURVE_TAG_ON ){
//                lastPoint = {(float)vec[j].x, (float)-vec[j].y, 0.f};
//                if(printVectorInfo){
//                    GUI_LogNotice("guiTTFCharacter") << "flag[" << j << "] is set to 1 - regular point - " << lastPoint.x <<  lastPoint.y;
//                }
//                charOutlines.lineTo(lastPoint.x/64, lastPoint.y/64);
//                keys.emplace_back(lastPoint.x/64, lastPoint.y/64);
//            }else{
//                if(printVectorInfo){
//                    GUI_LogNotice("guiTTFCharacter") << "flag[" << j << "] is set to 0 - control point";
//                }
//                
//                if( FT_CURVE_TAG(tags[j]) == FT_CURVE_TAG_CUBIC ){
//                    if(printVectorInfo){
//                        GUI_LogNotice("guiTTFCharacter") << "- bit 2 is set to 2 - CUBIC";
//                    }
//                    
//                    int prevPoint = j-1;
//                    if( j == 0){
//                        prevPoint = endPos-1;
//                    }
//                    
//                    int nextIndex = j+1;
//                    if( nextIndex >= endPos){
//                        nextIndex = startPos;
//                    }
//                    
//                    glm::vec3 nextPoint( (float)vec[nextIndex].x,  -(float)vec[nextIndex].y, 0.f );
//                    
//                    //we need two control points to draw a cubic bezier
//                    bool lastPointCubic =  ( FT_CURVE_TAG(tags[prevPoint]) != FT_CURVE_TAG_ON ) && ( FT_CURVE_TAG(tags[prevPoint]) == FT_CURVE_TAG_CUBIC);
//                    
//                    if( lastPointCubic ){
//                        glm::vec3 controlPoint1((float)vec[prevPoint].x,	(float)-vec[prevPoint].y, 0.f);
//                        glm::vec3 controlPoint2((float)vec[j].x, (float)-vec[j].y, 0.f);
//                        glm::vec3 nextPoint((float) vec[nextIndex].x,	-(float) vec[nextIndex].y, 0.f);
//                        
//                        //cubic_bezier(testOutline, lastPoint.x, lastPoint.y, controlPoint1.x, controlPoint1.y, controlPoint2.x, controlPoint2.y, nextPoint.x, nextPoint.y, 8);
//                        charOutlines.bezierTo({controlPoint1.x/64, controlPoint1.y/64}, {controlPoint2.x/64, controlPoint2.y/64}, {nextPoint.x/64, nextPoint.y/64});
//                        keys.emplace_back(nextPoint.x/64, nextPoint.y/64);
//                    }
//                    
//                }else{
//                    
//                    glm::vec3 conicPoint( (float)vec[j].x,  -(float)vec[j].y, 0.f );
//                    
//                    if(printVectorInfo){
//                        GUI_LogNotice("guiTTFCharacter") << "- bit 2 is set to 0 - conic- ";
//                        GUI_LogNotice("guiTTFCharacter") << "--- conicPoint point is " << conicPoint.x << conicPoint.y;
//                    }
//                    
//                    //If the first point is connic and the last point is connic then we need to create a virutal point which acts as a wrap around
//                    if( j == startPos ){
//                        bool prevIsConnic = (  FT_CURVE_TAG( tags[endPos-1] ) != FT_CURVE_TAG_ON ) && ( FT_CURVE_TAG( tags[endPos-1]) != FT_CURVE_TAG_CUBIC );
//                        
//                        if( prevIsConnic ){
//                            glm::vec3 lastConnic((float)vec[endPos - 1].x, (float)-vec[endPos - 1].y, 0.f);
//                            lastPoint = (conicPoint + lastConnic) / 2;
//                            
//                            if(printVectorInfo){
//                                GUI_LogNotice("guiTTFCharacter") << "NEED TO MIX WITH LAST";
//                                GUI_LogNotice("guiTTFCharacter") << "last is " << lastPoint.x << " " << lastPoint.y;
//                            }
//                        }
//                    }
//                    
//                    //bool doubleConic = false;
//                    
//                    int nextIndex = j+1;
//                    if( nextIndex >= endPos){
//                        nextIndex = startPos;
//                    }
//                    
//                    glm::vec3 nextPoint( (float)vec[nextIndex].x,  -(float)vec[nextIndex].y, 0.f );
//                    
//                    if(printVectorInfo){
//                        GUI_LogNotice("guiTTFCharacter") << "--- last point is " << lastPoint.x << " " <<  lastPoint.y;
//                    }
//                    
//                    bool nextIsConnic = (  FT_CURVE_TAG( tags[nextIndex] ) != FT_CURVE_TAG_ON ) && ( FT_CURVE_TAG( tags[nextIndex]) != FT_CURVE_TAG_CUBIC );
//                    
//                    //create a 'virtual on point' if we have two connic points
//                    if( nextIsConnic ){
//                        nextPoint = (conicPoint + nextPoint) / 2;
//                        if(printVectorInfo){
//                            GUI_LogNotice("guiTTFCharacter") << "|_______ double connic!";
//                        }
//                    }
//                    if(printVectorInfo){
//                        GUI_LogNotice("guiTTFCharacter") << "--- next point is " << nextPoint.x << " " << nextPoint.y;
//                    }
//                    
//                    //quad_bezier(testOutline, lastPoint.x, lastPoint.y, conicPoint.x, conicPoint.y, nextPoint.x, nextPoint.y, 8);
//                    charOutlines.quadBezierTo({lastPoint.x/64, lastPoint.y/64}, {conicPoint.x/64, conicPoint.y/64}, {nextPoint.x/64, nextPoint.y/64});
//                    keys.emplace_back(nextPoint.x/64, nextPoint.y/64);
//                    if( nextIsConnic ){
//                        lastPoint = nextPoint;
//                    }
//                }
//            }
//            
//        }
//        
//        guiVec2f home = charOutlines.getOutline()[k][0];
//        charOutlines.lineTo(home.x, home.y);
//        charOutlines.close();
//    }
//    
//    charOutlines.simplify(0.3);
//    if(keypoint!=NULL)
//        *keypoint = keys;
//    return charOutlines;
//}

static void makeCloseCountour(guiTTFCharacter& charOutlines, int contourId){
    guiVec2f home = charOutlines.getOutline()[contourId][0];
    charOutlines.lineTo(home.x, home.y);
    charOutlines.close();
}

// pom - v2.0 Fixed conic point
static guiTTFCharacter makeContoursForCharacter(FT_Face &face, std::vector<guiVec2f> *keypoint=NULL){
    bool vFlip=true;
    float x1, y1, x2, y2, x3, y3,x4,y4;
    const FT_Outline* outline= &face->glyph->outline;
    
    guiTTFCharacter charOutlines;
    std::vector<guiVec2f> keys;
    
    FT_Vector   v_last;
    FT_Vector   v_control;
    FT_Vector   v_start;
    
    FT_Vector*  point;
    FT_Vector*  limit;
    char*       tags;
    
    FT_Error    error;
    
    FT_Int   n;         /* index of contour in outline     */
    FT_UInt  first;     /* index of first point in contour */
    FT_Int   tag;       /* current point's state           */
    
    if ( !outline )return charOutlines;
    
    first = 0;
    
    for (n = 0; n < outline->n_contours; n++) {
        int  last;  // index of last point in contour
        
        last  = outline->contours[n];
        limit = outline->points + last;
        
        v_start = outline->points[first];
        v_last  = outline->points[last];
        
        v_control = v_start;
        
        point = outline->points + first;
        tags  = outline->tags  + first;
        tag   = FT_CURVE_TAG(tags[0]);
        
        // A contour cannot start with a cubic control point!
        if (tag == FT_CURVE_TAG_CUBIC)
            return charOutlines;
        
        // check first point to determine origin
        if ( tag == FT_CURVE_TAG_CONIC) {
            // first point is conic control.  Yes, this happens.
            if (FT_CURVE_TAG(outline->tags[last]) == FT_CURVE_TAG_ON) {
                // start at last point if it is on the curve
                v_start = v_last;
                limit--;
            } else {
                // if both first and last points are conic,
                // start at their middle and record its position
                // for closure
                v_start.x = (v_start.x + v_last.x) / 2;
                v_start.y = (v_start.y + v_last.y) / 2;
                
                v_last = v_start;
            }
            point--;
            tags--;
        }
        x1 = v_start.x/64.0f;
        y1 = v_start.y/64.0f;
        if (vFlip) y1 = -y1;
        charOutlines.moveTo(x1,y1);
        keys.emplace_back(x1, y1);
        x4=x1;y4=y1;
        
        
        while(point < limit) {
            point++;
            tags++;
            
            tag = FT_CURVE_TAG(tags[0]);
            switch(tag) {
                case FT_CURVE_TAG_ON: { // emit a single line_to
                    x1 = point->x/64.0f;
                    y1 = point->y/64.0f;
                    if (vFlip) y1 = -y1;
                    charOutlines.lineTo(x1,y1);
                    keys.emplace_back(x1, y1);
                    x4=x1;y4=y1;
                    
                    continue;
                }
                    
                case FT_CURVE_TAG_CONIC: { // consume conic arcs
                    v_control.x = point->x;
                    v_control.y = point->y;
                    
                Do_Conic:
                    if (point < limit) {
                        FT_Vector vec;
                        FT_Vector v_middle;
                        
                        point++;
                        tags++;
                        tag = FT_CURVE_TAG(tags[0]);
                        
                        vec.x = point->x;
                        vec.y = point->y;
                        
                        if (tag == FT_CURVE_TAG_ON) {
                            x1 = v_control.x/64.0f;
                            y1 = v_control.y/64.0f;
                            x2 = vec.x/64.0f;
                            y2 = vec.y/64.0f;
                            if (vFlip) { y1 = -y1; y2 = -y2; }
                            charOutlines.quadBezierTo(x4,y4,x1,y1,x2,y2);
                            keys.emplace_back(x2, y2);
                            x4=x2;y4=y2;
                            
                            continue;
                        }
                        
                        if (tag != FT_CURVE_TAG_CONIC)return charOutlines;
                        
                        v_middle.x = (v_control.x + vec.x) / 2.0f;
                        v_middle.y = (v_control.y + vec.y) / 2.0f;
                        
                        x1 = v_control.x/64.0f;
                        y1 = v_control.y/64.0f;
                        x2 = v_middle.x/64.0f;
                        y2 = v_middle.y/64.0f;
                        if (vFlip) { y1 = -y1; y2 = -y2; }
                        charOutlines.quadBezierTo(x4,y4,x1,y1,x2,y2);
                        keys.emplace_back(x2, y2);
                        x4=x2;y4=y2;
                        
                        v_control = vec;
                        goto Do_Conic;
                    }
                    
                    x1 = v_control.x/64.0f;
                    y1 = v_control.y/64.0f;
                    x2 = v_start.x/64.0f;
                    y2 = v_start.y/64.0f;
                    if (vFlip) { y1 = -y1; y2 = -y2; }
                    charOutlines.quadBezierTo(x4,y4,x1,y1,x2,y2);
                    keys.emplace_back(x2, y2);
                    x4=x2;y4=y2;
                    
                    goto Close;
                }
                    
                default: { // FT_CURVE_TAG_CUBIC
                    FT_Vector vec1, vec2;
                    
                    if (point + 1 > limit || FT_CURVE_TAG(tags[1]) != FT_CURVE_TAG_CUBIC)
                        return charOutlines;
                    
                    vec1.x = point[0].x;
                    vec1.y = point[0].y;
                    vec2.x = point[1].x;
                    vec2.y = point[1].y;
                    
                    point += 2;
                    tags  += 2;
                    
                    if (point <= limit) {
                        FT_Vector vec;
                        
                        vec.x = point->x;
                        vec.y = point->y;
                        
                        x1 = vec1.x/64.0f;
                        y1 = vec1.y/64.0f;
                        x2 = vec2.x/64.0f;
                        y2 = vec2.y/64.0f;
                        x3 = vec.x/64.0f;
                        y3 = vec.y/64.0f;
                        if (vFlip) { y1 = -y1; y2 = -y2; y3 = -y3; }
                        charOutlines.bezierTo({x1,y1},{x2,y2},{x3,y3});
                        keys.emplace_back(x3, y3);
                        x4=x3;y4=y3;
                        
                        continue;
                    }
                    
                    x1 = vec1.x/64.0f;
                    y1 = vec1.y/64.0f;
                    x2 = vec2.x/64.0f;
                    y2 = vec2.y/64.0f;
                    x3 = v_start.x/64.0f;
                    y3 = v_start.y/64.0f;
                    if (vFlip) { y1 = -y1; y2 = -y2; y3 = -y3; }
                    charOutlines.bezierTo({x1,y1}, {x2,y2} ,{x3,y3});
                    keys.emplace_back(x3, y3);
                    x4=x3;y4=y3;
                    
                    goto Close;
                }
            }
        }

        makeCloseCountour(charOutlines, n);
        
    Close:
        first = last + 1;
    }
    
    charOutlines.simplify(0.3);
    if(keypoint!=NULL)
        *keypoint = keys;
    return charOutlines;
}


// MARK: GUI_FontAtlas
GUI_FontAtlas::GUI_FontAtlas()
:setting("",0)
{
    bLoadedOk = false;
    texAtlas  = nullptr;
    font = nullptr;
    letterSpacing = 1;
    spaceSize = 1;
    ascenderHeight = 0;
    descenderHeight = 0;
    lineHeight = 0;
    fontTextureWidth = fontTextureHeight = 0;
    createContour = false;
}

GUI_FontAtlas::~GUI_FontAtlas(){
    charMap.clear();
    
    if(texAtlas!=nullptr)
        SDL_DestroyTexture(texAtlas);
    
    if(font!=nullptr)
        TTF_CloseFont(font);
}

bool GUI_FontAtlas::load(const std::string& filename, int fontsize){
    GUI_FontAtlasSetting setting(filename, fontsize);
    setting.ranges = {GUI_Unicode::Latin, GUI_Unicode::Thai, GUI_Unicode::ThaiExtended};
    return load(setting);
}

void GUI_FontAtlas::stringAppendUTF8(string & str, uint32_t utf8){
    try{
        utf8::append(utf8, back_inserter(str));
    }catch(...){}
}

guiTTFCharacter GUI_FontAtlas::getCharacterAsPoints(Uint16 ch){
    if(createContour==false){
        GUI_LogError("GUI_FontAtlas") << "getCharacterAsPoints(): contours not created";
        return guiTTFCharacter();
    }
    
    return charOutlinesContour[ch];
}

vector<guiTTFCharacter> GUI_FontAtlas::getStringAsPoints(const std::string& str){
    vector<guiTTFCharacter> shapes;
    
    if(bLoadedOk==false){
        GUI_LogError("GUI_FontAtlas") << "getStringAsPoints(): font not allocated: line " << __LINE__ << " in " << __FILE__;
        return shapes;
    }
    
    int startx = 0;
    int y = 0, x=0;
    uint32_t prevC = 0;
    int sizeAdjusts = adjusts.size();
    int index = 0;
    y+=ascenderHeight;
    for(auto c: GUI_UTF8Iterator(str)){
        try{
            if (c == '\n') {
                y += lineHeight;
                x = startx; //reset X Pos back to zero
                prevC = 0;
                index--;
            }
            else if (c == '\t') {
                x += getGlyphProperties(' ').advance * letterSpacing * 4; // assume tab size = 4
                prevC = c;
            } else if (c == ' ') {
                x += getGlyphProperties(' ').advance * letterSpacing * spaceSize;
                prevC = c;
            }
            else if(isValidGlyph(c)) {
                
                const auto & props = getGlyphProperties(c);
                if(sizeAdjusts>0 && index<sizeAdjusts)
                {
                    y+=(adjusts[index].y * lineHeight);
                }
                if(prevC>0){
                    x += getKerning(c,prevC);
                }
                
                shapes.push_back(getCharacterAsPoints(c));
                shapes.back().translate(x, y);
                
                x += props.advance * letterSpacing;
                prevC = c;
                
                if(sizeAdjusts>0 && index<sizeAdjusts)
                {
                    y-=(adjusts[index].y * lineHeight);
                }
            }
            index++;
        }catch(...){
            break;
        }
    }
    return shapes;
}

bool GUI_FontAtlas::load(const GUI_FontAtlasSetting & _setting){
    setting = _setting;
    bLoadedOk = false;
    
    font = TTF_OpenFont(setting.fontName.c_str(), setting.fontSize*GUI_scale);
    if(font==nullptr){
        GUI_LogError("GUI_FontAtlas") << "load(): TTF_OpenFont() fail!";
        return false;
    }
    
//    FT_UInt idx = FT_Get_Name_Index(font->face, (FT_String*)("yoYingthai.less"));
//    idx = FT_Get_Name_Index(font->face, (FT_String*)("uni0E01"));
    
    if(setting.ranges.empty()){
        setting.ranges.push_back(GUI_Unicode::Latin);
    }
    int border = 1;
    TTF_SetFontKerning(font, 1);
    
    lineHeight = font->lineskip;
    ascenderHeight = font->ascent;
    descenderHeight = font->descent;
    
    auto nGlyphs = std::accumulate(setting.ranges.begin(), setting.ranges.end(), 0u,
                                   [](uint32_t acc, GUI_Unicode::range range){
                                       return acc + range.getNumGlyphs();
                                   });
    
    cps.resize(nGlyphs);
    
    if(createContour){
        charOutlinesContour.clear();
    }
    
    vector<GUI_FontAtlas::glyph> all_glyphs;
    
    uint32_t areaSum=0;
    
    //--------------------- load each char -----------------------
    auto i = 0u;
    for(auto & range: setting.ranges){
        for (uint32_t g = range.begin; g <= range.end; g++, i++){
            all_glyphs.push_back(loadGlyph(g));
            all_glyphs[i].props.characterIndex	= i;
            glyphIndexMap[g] = i;
            cps[i] = all_glyphs[i].props;
            areaSum += (cps[i].tW+border*2)*(cps[i].tH+border*2);
            
//            std::string str;
//            stringAppendUTF8(str, g);
//            GUI_LogNotice("GUI_FontAtlas", "load character(%d)\t%s[U+%04X]", i, str.c_str(), g);
            
            if(createContour){
                std::vector<guiVec2f> keypoint;
                charOutlinesContour[g] = makeContoursForCharacter(font->face, &keypoint);
//                charOutlinesContour[g].simplify(0.3);
                keyPoints[g] = keypoint;
            }
        }
    }
    GUI_LogNotice("GUI_FontAtlas") << "load(): areaSum: " << areaSum;
    
    vector<GUI_FontAtlas::glyphProps> sortedCopy = cps;
    sort(sortedCopy.begin(),sortedCopy.end(),[](const GUI_FontAtlas::glyphProps & c1, const GUI_FontAtlas::glyphProps & c2){
        if(c1.tH == c2.tH) return c1.tW > c2.tW;
        else return c1.tH > c2.tH;
    });
    
    // pack in a texture, algorithm to calculate min w/h from
    // http://upcommons.upc.edu/pfc/bitstream/2099.1/7720/1/TesiMasterJonas.pdf
    bool packed = false;
    float alpha = logf(areaSum)*1.44269f;
    int w;
    int h;
    while(!packed){
        w = pow(2,floor((alpha/2.f) + 0.5f)); // there doesn't seem to be a round in cmath for windows.
        //w = pow(2,round(alpha/2.f));
        h = pow(2, (int)round(alpha - round(alpha/2.f)));
        int x=0;
        int y=0;
        auto maxRowHeight = sortedCopy[0].tH + border*2;
        packed = true;
        for(auto & glyph: sortedCopy){
            if(x+glyph.tW + border*2>w){
                x = 0;
                y += maxRowHeight;
                maxRowHeight = glyph.tH + border*2;
                if(y + maxRowHeight > h){
                    alpha++;
                    packed = false;
                    break;
                }
            }
            x+= glyph.tW + border*2;
        }
        
    }
    GUI_LogNotice("GUI_FontAtlas") << "pack in to texture w=" << w << " h=" << h;
    
    texAtlas = SDL_CreateTexture(GUI_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if(texAtlas==nullptr){
        GUI_LogError("GUI_FontAtlas") << "load(): Texture atlas allocated fail!";
        return false;
    }
    fontTextureWidth = w;
    fontTextureHeight = h;
    
    int x=0;
    int y=0;
    auto maxRowHeight = sortedCopy[0].tH + border*2;
    SDL_SetRenderTarget(GUI_renderer, texAtlas);
    SDL_SetRenderDrawColor(GUI_renderer, 0, 0, 0, 0);
    SDL_RenderClear(GUI_renderer);
    for(auto & glyph: sortedCopy){
        SDL_Surface* s = all_glyphs[glyph.characterIndex].surf;
        if(s==nullptr){
            GUI_LogWarning("GUI_FontAtlas") << "load(): render texture atlas glyph surface=NULL(" << glyph.characterIndex << ")";
            continue;
        }
        
        if(x+glyph.tW + border*2>w){
            x = 0;
            y += maxRowHeight;
            maxRowHeight = glyph.tH + border*2;
        }
        
        cps[glyph.characterIndex].t1		= x + border;
        cps[glyph.characterIndex].v1		= y + border;
        
        SDL_Texture* tex = SDL_CreateTextureFromSurface(GUI_renderer, s);
        if(tex){
            int w = s->w;
            int h = s->h;
            SDL_Rect src = {0, 0, w, h};
            SDL_Rect dst = {x+border, y+border, cps[glyph.characterIndex].tW, cps[glyph.characterIndex].tH};
            SDL_RenderCopy(GUI_renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
        }
        SDL_FreeSurface(all_glyphs[glyph.characterIndex].surf);
        
        x+= glyph.tW + border*2;
    }
    SDL_SetRenderTarget(GUI_renderer, NULL);
    SDL_SetTextureBlendMode(texAtlas, SDL_BLENDMODE_BLEND);
    
    bLoadedOk = true;
    return true;
}

GUI_FontAtlas::glyph GUI_FontAtlas::loadGlyph(uint32_t utf8) const{
    if(utf8==0x0000u){
        GUI_LogNotice();
    }
    
    glyph aGlyph;
    aGlyph.surf = nullptr;
    //    auto err = FT_Load_Glyph( font->face, FT_Get_Char_Index( font->face, utf8 ), FT_LOAD_FORCE_AUTOHINT );
    FT_Int charIndex = -1;
    if(charMap.size()){
        auto cm = charMap.find(utf8);
        if(cm!=charMap.end()){
            charIndex = cm->second;
        }
    }
    
    auto err = Find_Glyph(font, utf8, CACHED_METRICS|CACHED_PIXMAP, charIndex);
    if(err){
        GUI_LogError("GUI_FontAtlas") << "loadGlyph(): FT_Load_Glyph failed for utf8 code " << utf8 << ": FT_Error " << err;
        return aGlyph;
    }
    
    FT_Render_Glyph(font->face->glyph, FT_RENDER_MODE_NORMAL);
    // -------------------------
    // info about the character:
    aGlyph.props.glyph		= utf8;
    aGlyph.props.height 	= font->face->glyph->metrics.height>>6;
    aGlyph.props.width 		= font->face->glyph->metrics.width>>6;
    aGlyph.props.bearingX	= font->face->glyph->metrics.horiBearingX>>6;
    aGlyph.props.bearingY	= font->face->glyph->metrics.horiBearingY>>6;
    aGlyph.props.xmin		= font->face->glyph->bitmap_left;
    aGlyph.props.xmax		= aGlyph.props.xmin + aGlyph.props.width;
    aGlyph.props.ymin		= -font->face->glyph->bitmap_top;
    aGlyph.props.ymax		= aGlyph.props.ymin + aGlyph.props.height;
    aGlyph.props.advance	= font->face->glyph->metrics.horiAdvance>>6;
    aGlyph.props.tW			= aGlyph.props.width;
    aGlyph.props.tH			= aGlyph.props.height;
    
    int width;
    int height;
    if ( ( TTF_SizeUNICODE(font, (const Uint16 *)&utf8, &width, &height) < 0 ) || !width || (utf8==THAI_WBR_UNICHAR) || (utf8==OFX_UNICODE_CTRL_BREAK_PERMITTED)) {
        TTF_SetError("Text has zero width");
        aGlyph.props.width = 0;
        aGlyph.props.advance = 0;
        aGlyph.props.tW	= 0;
        aGlyph.props.tH = 0;
        return aGlyph;
    }
    
    c_glyph *glyph = font->current;
    
    height = glyph->pixmap.rows;
    aGlyph.surf = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
                                       0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    width = glyph->pixmap.width;
    if(width==0 || height==0) return aGlyph;
    aGlyph.props.tW			= aGlyph.surf->w;
    aGlyph.props.tH			= aGlyph.surf->h;
    
    
    Uint32 *dst_check = (Uint32*)aGlyph.surf->pixels + aGlyph.surf->pitch/4 * aGlyph.surf->h;
    int xstart = 0;
    Uint32 pixel = 0xFFFFFF; // white
    
    SDL_FillRect(aGlyph.surf, NULL, pixel);
    if ((glyph->minx < 0) ) {
        xstart -= glyph->minx;
    }
    
    Uint8 *src;
    Uint32 *dst;
    Uint32 alpha;
    for ( int row = 0; row < glyph->pixmap.rows; ++row ) {
        /* Make sure we don't go either over, or under the
         * limit */
        if ( row < 0 ) {
            continue;
        }
        if ( row >= aGlyph.surf->h ) {
            continue;
        }
        dst = (Uint32*) aGlyph.surf->pixels +
        (row+0) * aGlyph.surf->pitch/4 +
        xstart + glyph->minx;
        
        /* Added code to adjust src pointer for pixmaps to
         * account for pitch.
         * */
        src = (Uint8*) (glyph->pixmap.buffer + glyph->pixmap.pitch * row);
        //        for (int col = width; col>0 && dst < dst_check; --col) {
        for (int col = width; col>0 && dst < dst_check; --col) {
            alpha = *src++;
            *dst++ |= pixel | (alpha << 24);
        }
    }
    xstart += glyph->advance;
    
    return aGlyph;
}

bool GUI_FontAtlas::isLoaded() const{
    return bLoadedOk;
}

std::size_t	GUI_FontAtlas::getNumCharacters() const{
    return cps.size();
}

int GUI_FontAtlas::getLineHeight() const{
    return lineHeight;
}

void GUI_FontAtlas::setLineHeight(int height){
    lineHeight = height;
}

float GUI_FontAtlas::getAscenderHeight() const{
    return ascenderHeight;
}

float GUI_FontAtlas::getDescenderHeight() const{
    return descenderHeight;
}

float GUI_FontAtlas::getLetterSpacing() const{
    return letterSpacing;
}

void GUI_FontAtlas::setLetterSpacing(float spacing){
    letterSpacing = spacing;
}

float GUI_FontAtlas::getSpaceSize() const{
    return spaceSize;
}

void GUI_FontAtlas::setSpaceSize(float size){
    spaceSize = size;
}

int GUI_FontAtlas::getFontTextureWidth() const{
    return fontTextureWidth;
}

int GUI_FontAtlas::getFontTextureHeight() const{
    return fontTextureHeight;
}

void GUI_FontAtlas::setInputText(std::string utf8){
    inputText = utf8;
    outputText = std::string(inputText);
    exceedLimit = false;
}

void GUI_FontAtlas::setCreateContour(bool _createContour){
    createContour = _createContour;
}

void GUI_FontAtlas::drawChar(uint32_t c, int x, int y) const{
    if (!isValidGlyph(c)){
        GUI_LogError("GUI_FontAtlas") << "drawChar(): char " << c  << " not allocated: line " << __LINE__ << " in " << __FILE__;
        return;
    }
    
    int xmin, ymin, xmax, ymax;
    auto props = getGlyphProperties(c);
    int t1 = props.t1;
    int v1 = props.v1;
    int tw = props.tW;
    int th = props.tH;
    
    xmin		= props.xmin+x;
    ymin		= props.ymin;
    xmax		= props.xmax+x;
    ymax		= props.ymax;
    
    ymin += y;
    ymax += y;
    
    SDL_Rect src = {t1, v1, tw, th};
    SDL_Rect dst = {xmin, ymin, tw, th};
    SDL_RenderCopy(GUI_renderer, texAtlas, &src, &dst);
}

void GUI_FontAtlas::iterateString(const string & str, int x, int y, std::function<void(uint32_t, int x, int y)> f) const{
    
    int startx = x;
    uint32_t prevC = 0;
    int sizeAdjusts = adjusts.size();
    int index = 0;
    y+=ascenderHeight;
    for(auto c: GUI_UTF8Iterator(str)){
        try{
            if (c == '\n') {
                y += lineHeight;
                x = startx; //reset X Pos back to zero
                prevC = 0;
                index--;
            }
            else if (c == '\t') {
                x += getGlyphProperties(' ').advance * letterSpacing * 4; // assume tab size = 4
                prevC = c;
            } else if (c == ' ') {
                x += getGlyphProperties(' ').advance * letterSpacing * spaceSize;
                prevC = c;
            }
            else if(isValidGlyph(c)) {
                
                const auto & props = getGlyphProperties(c);
                if(sizeAdjusts>0 && index<sizeAdjusts)
                {
                    y+=(adjusts[index].y * lineHeight);
                }
                if(prevC>0){
                    x += getKerning(c,prevC);
                }
                f(c,x, y);
                x += props.advance * letterSpacing;
                prevC = c;
                
                if(sizeAdjusts>0 && index<sizeAdjusts)
                {
                    y-=(adjusts[index].y * lineHeight);
                }
            }
            index++;
        }catch(...){
            break;
        }
    }
}

void GUI_FontAtlas::drawString(const std::string& str, int x, int y) const{
    if (!bLoadedOk){
        GUI_LogError("GUI_FontAtlas") << "drawString(): font not allocated";
        return;
    }
    iterateString(str, x, y,[&](uint32_t c, int x,int  y){
        drawChar(c, x, y);
    });
}

void GUI_FontAtlas::drawCharAsShapes(uint32_t c, int x, int y){
    if (!isValidGlyph(c)){
        GUI_LogError("GUI_FontAtlas") << "drawCharAsShapes(): char " << c  << " not allocated: line " << __LINE__ << " in " << __FILE__;
        return;
    }
    
    guiTTFCharacter ttfch = charOutlinesContour[c];
    vector<GUI_Polyline> outlines = ttfch.getOutline();
    
    for(auto outline:outlines){
        vector<guiVec2f> vertices = outline.getVertices();
        for(int i=1; i<vertices.size(); i++){
            guiVec2f v0 = vertices[i-1];
            guiVec2f v1 = vertices[i];
            GUI_DrawLine(GUI_renderer, (int)v0.x+x, (int)v0.y+y, (int)v1.x+x, (int)v1.y+y, cWhite);
//            int radius = first? 10 : 5;
//            GUI_FillCircle(GUI_renderer, (int)v1.x+x, (int)v1.y+y, radius, cRed);
//            first = false;
        }
    }
    
    std::vector<guiVec2f> keypoint = keyPoints[c];
    int numpoint = keypoint.size();
    for(int i=0; i<numpoint; i++){
        guiVec2f p = keypoint[i];
        GUI_FillCircle(GUI_renderer, (int)p.x+x, (int)p.y+y, 5, cRed);
    }
    GUI_FillCircle(GUI_renderer, (int)keypoint[0].x+x, (int)keypoint[0].y+y, 15, cRed);
    GUI_FillCircle(GUI_renderer, (int)keypoint[1].x+x, (int)keypoint[1].y+y, 10, cRed);
}

void GUI_FontAtlas::drawStringAsShapes(const std::string& str, int x, int y){
    if (!bLoadedOk){
        GUI_LogError("GUI_FontAtlas") << "drawStringAsShapes(): font not allocated";
        return;
    }
    if (!createContour){
        GUI_LogError("GUI_FontAtlas") << "drawStringAsShapes(): contours not created for this font.";
        return;
    }
    
    iterateString(str, x, y,[&](uint32_t c, int x,int  y){
        drawCharAsShapes(c, x, y);
    });
}

void GUI_FontAtlas::drawOutputText(int x, int y){
    drawString(outputText, x, y);
}

const SDL_Texture* GUI_FontAtlas::getFontTexture() const{
    return texAtlas;
}

void GUI_FontAtlas::setColor(SDL_Color c) const{
    SDL_SetTextureColorMod(texAtlas, c.r, c.g, c.b);
    SDL_SetTextureAlphaMod(texAtlas, c.a);
}

bool GUI_FontAtlas::isValidGlyph(uint32_t glyph) const{
    return std::any_of(setting.ranges.begin(), setting.ranges.end(),
                       [&](GUI_Unicode::range range){
                           return glyph >= range.begin && glyph <= range.end;
                       });
}

int GUI_FontAtlas::getKerning(uint32_t c, uint32_t prevC) const{
    if(FT_HAS_KERNING( font->face )){
        FT_Vector kerning;
        FT_Get_Kerning(font->face, FT_Get_Char_Index(font->face, prevC), FT_Get_Char_Index(font->face, c), FT_KERNING_UNFITTED, &kerning);
        return kerning.x>>6;
    }else{
        return 0;
    }
}

const GUI_FontAtlas::glyphProps & GUI_FontAtlas::getGlyphProperties(uint32_t glyph) const{
    if(isValidGlyph(glyph)){
        return cps[indexForGlyph(glyph)];
    }else{
        return invalidProps;
    }
}

size_t GUI_FontAtlas::indexForGlyph(uint32_t glyph) const{
    return glyphIndexMap.find(glyph)->second;
}

std::string GUI_FontAtlas::splitLineConcat(std::string c, float rectWidthLimit, float rectHeightLimit)
{
    vector<string> splitLines = splitLine(c, rectWidthLimit, rectHeightLimit);
    string splitLineFormated;
    for(int i=0; i<splitLines.size(); i++)
    {
        splitLineFormated+=splitLines[i];
        if(i<splitLines.size()-1)
        {
            splitLineFormated+="\n";
        }
    }
    //    GUI_LogNotice("splitLineConcat") << splitLineFormated;
    return splitLineFormated;
}

std::string walkAndFill(ofUTF8Ptr begin, ofUTF8Ptr & iter, ofUTF8Ptr end){
    
    string finalLine = "";
    
    ofUTF8Ptr i = begin;
    while (i < iter) { // re-fill the finalLine from the begining to the last Space
        finalLine += ofTextConverter::toUTF8(ofUTF8::getNext(i)); // get the next unichar and iterate
        if(i == end){
            break;
        }
    }
    return finalLine;
}

SDL_Rect GUI_FontAtlas::getStringBoundingBox(std::string c, float x, float y){
    SDL_Rect myRect;
    
    if (!bLoadedOk){
        GUI_LogError("GUI_FontAtlas") << "getStringBoundingBox(): font not allocated";
        return myRect;
    }
    
    int height = 0;
    int width = 0;
    
    std::string th_wbr_utf8, th_ctrl_brk_utf8;
    stringAppendUTF8(th_wbr_utf8, THAI_WBR_UNICHAR);
    stringAppendUTF8(th_ctrl_brk_utf8, OFX_UNICODE_CTRL_BREAK_PERMITTED);
    
    if(guiIsStringInString(c, th_wbr_utf8))
        guiStringReplace(c, th_wbr_utf8, "");
    
    if(guiIsStringInString(c, th_ctrl_brk_utf8))
        guiStringReplace(c, th_ctrl_brk_utf8, "");
    
    TTF_SizeUTF8(font, c.c_str(), &width, &height);
    
    myRect = {(int)x, (int)y, width, height};
    return myRect;
}

vector<string> GUI_FontAtlas::splitLine(string c, float rectWidthLimit, float rectHeightLimit)
{
    vector<string>splitLines;
    ofUTF8Ptr iter = ofUTF8::beginPtr(c);
    ofUTF8Ptr lineStart = iter;
    ofUTF8Ptr lastSpace;
    ofUTF8Ptr stop = ofUTF8::endPtr(c);
    exceedLimit = false;
    
    string thisLine = "";
    bool foundSpace = false;
    bool foundNewLine = false;
    bool firstChar = true;
    bool wordsWereTruncated = false;
    
    while(iter < stop) {
        ofUniChar c = ofUTF8::getNext(iter); // get the next unichar and iterate
        if ( isWordBreak(c) && ! firstChar){
            foundSpace = true;
            lastSpace = iter;
        }
        if(firstChar)
        {
            firstChar = false;
        }
        if ( ofTextConverter::toUTF8(c) == "\n" ){
            foundNewLine = true;
        }
        thisLine += ofTextConverter::toUTF8(c);
        
        SDL_Rect r = getStringBoundingBox(thisLine.c_str(), 0, 0);
        
        if ( r.w > rectWidthLimit || foundNewLine ) { //we went too far, lets jump back to our closest space
            if(foundNewLine){
                if (thisLine == "\n"){ //if the whole line is only \n, replace with a space to avoid weird things
                    thisLine = " ";
                }else{	//otherwise remove the "\n"
                    thisLine = thisLine.substr(0, SDL_strlen(thisLine.c_str())-1);
                }
                guiStringReplace(thisLine, ofTextConverter::toUTF8(THAI_WBR_UNICHAR), "");
                splitLines.push_back(thisLine);
                
            }else{
                if (foundSpace){
                    //cout << "## foundSpace! (" << thisLine << ")" << endl;
                    string finalLine = walkAndFill(lineStart, iter, lastSpace);
                    guiStringReplace(finalLine, ofTextConverter::toUTF8(THAI_WBR_UNICHAR), "");
                    splitLines.push_back(finalLine);
                    iter = lastSpace;
                }else{
                    //cout << "## no Space! (" << thisLine << ")" << endl;
                    guiStringReplace(thisLine, ofTextConverter::toUTF8(THAI_WBR_UNICHAR), "");
                    splitLines.push_back(thisLine);
                    wordsWereTruncated = true;
                }
            }
            //reset counter vars
            lineStart = iter;
            r.w = 0;
            thisLine = "";
            foundSpace = foundNewLine = false;
        }
        else{
            if(iter == stop){ //last line!
                string finalLine = walkAndFill(lineStart, iter, stop);
                guiStringReplace(finalLine, ofTextConverter::toUTF8(THAI_WBR_UNICHAR), "");
                splitLines.push_back(finalLine);
                break;
            }
        }
        
        if(splitLines.size()>0)
        {
            float height = splitLines.size() * lineHeight + lineHeight;
            if(height>rectHeightLimit)
            {
                splitLines[splitLines.size()-1]+="...";
                guiStringReplace(splitLines[splitLines.size()-1], " ...", "...");
//                string ss;
//                ss+= " ";
//                ss+= ofTextConverter::toUTF8(OFX_UNICODE_CTRL_BREAK_PERMITTED);
//                ss+="...";
//                guiStringReplace(splitLines[splitLines.size()-1], ss, "...");
                exceedLimit = true;
                break;
            }
        }
    }

    return splitLines;
}

bool GUI_FontAtlas::isExceedLimit(){
    return exceedLimit;
}

bool GUI_FontAtlas::isWordBreak(ofUniChar c){
    return std::isspace(c) || c==OFX_UNICODE_CTRL_BREAK_PERMITTED;
}

void GUI_FontAtlas::setPosAdjusts(vector<guiVec2f> _adjusts)
{
    adjusts = _adjusts;
}

void GUI_FontAtlas::setCharMapIndex(std::map<Uint16, FT_UInt> _charMap){
    charMap.clear();
    charMap = _charMap;
}


// MARK: GUI_UTF8Iterator
GUI_UTF8Iterator::GUI_UTF8Iterator(const string & str){
    try{
        utf8::replace_invalid(str.begin(),str.end(),back_inserter(src_valid));
    }catch(...){
    }
}

utf8::iterator<std::string::const_iterator> GUI_UTF8Iterator::begin() const{
    try {
        return utf8::iterator<std::string::const_iterator>(src_valid.begin(), src_valid.begin(), src_valid.end());
    }
    catch (...) {
        return utf8::iterator<std::string::const_iterator>();
    }
}

utf8::iterator<std::string::const_iterator> GUI_UTF8Iterator::end() const{
    try {
        return utf8::iterator<std::string::const_iterator>(src_valid.end(), src_valid.begin(), src_valid.end());
    }
    catch (...) {
        return utf8::iterator<std::string::const_iterator>();
    }
}

utf8::iterator<std::string::const_reverse_iterator> GUI_UTF8Iterator::rbegin() const {
    try {
        return utf8::iterator<std::string::const_reverse_iterator>(src_valid.rbegin(), src_valid.rbegin(), src_valid.rend());
    }
    catch (...) {
        return utf8::iterator<std::string::const_reverse_iterator>();
    }
}

utf8::iterator<std::string::const_reverse_iterator> GUI_UTF8Iterator::rend() const {
    try {
        return utf8::iterator<std::string::const_reverse_iterator>(src_valid.rbegin(), src_valid.rbegin(), src_valid.rend());
    }
    catch (...) {
        return utf8::iterator<std::string::const_reverse_iterator>();
    }
}



// MARK: Source code from SDL_ttf.c
void Flush_Glyph( c_glyph* glyph )
{
    glyph->stored = 0;
    glyph->index = 0;
    if ( glyph->bitmap.buffer ) {
        SDL_free( glyph->bitmap.buffer );
        glyph->bitmap.buffer = 0;
    }
    if ( glyph->pixmap.buffer ) {
        SDL_free( glyph->pixmap.buffer );
        glyph->pixmap.buffer = 0;
    }
    glyph->cached = 0;
}

void Flush_Cache( TTF_Font* font )
{
    int i;
    int size = sizeof( font->cache ) / sizeof( font->cache[0] );
    
    for ( i = 0; i < size; ++i ) {
        if ( font->cache[i].cached ) {
            Flush_Glyph( &font->cache[i] );
        }
    }
}

FT_Error Load_Glyph( TTF_Font* font, Uint16 ch, c_glyph* cached, int want, FT_Int charIndex )
{
    FT_Face face;
    FT_Error error;
    FT_GlyphSlot glyph;
    FT_Glyph_Metrics* metrics;
    FT_Outline* outline;
    
    if ( !font || !font->face ) {
        return FT_Err_Invalid_Handle;
    }
    
    face = font->face;
    
    /* Load the glyph */
    if ( ! cached->index ) {
        if(charIndex!=-1)
            cached->index=charIndex;
        else
            cached->index = FT_Get_Char_Index( face, ch );
    }
//    error = FT_Load_Glyph( face, cached->index, FT_LOAD_DEFAULT | font->hinting);
    error = FT_Load_Glyph(face, cached->index, FT_LOAD_FORCE_AUTOHINT);
    if ( error ) {
        return error;
    }
    
    /* Get our glyph shortcuts */
    glyph = face->glyph;
//    if(glyph->num_subglyphs){
//        GUI_LogNotice("Load_Glyph", "%x, %c, num_subglyphs=%d", ch, ch, glyph->num_subglyphs);
//    }
//    else{
//        GUI_LogNotice("Load_Glyph", "%x, %c, num_subglyphs=%d", ch, ch, glyph->num_subglyphs);
//    }
    metrics = &glyph->metrics;
    outline = &glyph->outline;
    
    /* Get the glyph metrics if desired */
    if ( (want & CACHED_METRICS) && !(cached->stored & CACHED_METRICS) ) {
        if ( FT_IS_SCALABLE( face ) ) {
            /* Get the bounding box */
            cached->minx = FT_FLOOR(metrics->horiBearingX);
            cached->maxx = FT_CEIL(metrics->horiBearingX + metrics->width);
            cached->maxy = FT_FLOOR(metrics->horiBearingY);
            cached->miny = cached->maxy - FT_CEIL(metrics->height);
            cached->yoffset = font->ascent - cached->maxy;
            cached->advance = FT_CEIL(metrics->horiAdvance);
        } else {
            /* Get the bounding box for non-scalable format.
             * Again, freetype2 fills in many of the font metrics
             * with the value of 0, so some of the values we
             * need must be calculated differently with certain
             * assumptions about non-scalable formats.
             * */
            cached->minx = FT_FLOOR(metrics->horiBearingX);
            cached->maxx = FT_CEIL(metrics->horiBearingX + metrics->width);
            cached->maxy = FT_FLOOR(metrics->horiBearingY);
            cached->miny = cached->maxy - FT_CEIL(face->available_sizes[font->font_size_family].height);
            cached->yoffset = 0;
            cached->advance = FT_CEIL(metrics->horiAdvance);
        }
        
        /* Adjust for bold and italic text */
        if ( TTF_HANDLE_STYLE_BOLD(font) ) {
            cached->maxx += font->glyph_overhang;
        }
        if ( TTF_HANDLE_STYLE_ITALIC(font) ) {
            cached->maxx += (int)SDL_ceil(font->glyph_italics);
        }
        cached->stored |= CACHED_METRICS;
    }
    
    if ( ((want & CACHED_BITMAP) && !(cached->stored & CACHED_BITMAP)) ||
        ((want & CACHED_PIXMAP) && !(cached->stored & CACHED_PIXMAP)) ) {
        int mono = (want & CACHED_BITMAP);
        int i;
        FT_Bitmap* src;
        FT_Bitmap* dst;
        FT_Glyph bitmap_glyph = NULL;
        
        /* Handle the italic style */
        if ( TTF_HANDLE_STYLE_ITALIC(font) ) {
            FT_Matrix shear;
            
            shear.xx = 1 << 16;
            shear.xy = (int) ( font->glyph_italics * ( 1 << 16 ) ) / font->height;
            shear.yx = 0;
            shear.yy = 1 << 16;
            
            FT_Outline_Transform( outline, &shear );
        }
        
        /* Render as outline */
        if ( (font->outline > 0) && glyph->format != FT_GLYPH_FORMAT_BITMAP ) {
            FT_Stroker stroker;
            FT_Get_Glyph( glyph, &bitmap_glyph );
            //            error = FT_Stroker_New( library, &stroker );
            if ( error ) {
                return error;
            }
            FT_Stroker_Set( stroker, font->outline * 64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0 );
            FT_Glyph_Stroke( &bitmap_glyph, stroker, 1 /* delete the original glyph */ );
            FT_Stroker_Done( stroker );
            /* Render the glyph */
            error = FT_Glyph_To_Bitmap( &bitmap_glyph, mono ? ft_render_mode_mono : ft_render_mode_normal, 0, 1 );
            if ( error ) {
                FT_Done_Glyph( bitmap_glyph );
                return error;
            }
            src = &((FT_BitmapGlyph)bitmap_glyph)->bitmap;
        } else {
            /* Render the glyph */
            error = FT_Render_Glyph( glyph, mono ? ft_render_mode_mono : ft_render_mode_normal );
            if ( error ) {
                return error;
            }
            src = &glyph->bitmap;
        }
        /* Copy over information to cache */
        if ( mono ) {
            dst = &cached->bitmap;
        } else {
            dst = &cached->pixmap;
        }
        SDL_memcpy( dst, src, sizeof( *dst ) );
        
        /* FT_Render_Glyph() and .fon fonts always generate a
         * two-color (black and white) glyphslot surface, even
         * when rendered in ft_render_mode_normal. */
        /* FT_IS_SCALABLE() means that the font is in outline format,
         * but does not imply that outline is rendered as 8-bit
         * grayscale, because embedded bitmap/graymap is preferred
         * (see FT_LOAD_DEFAULT section of FreeType2 API Reference).
         * FT_Render_Glyph() canreturn two-color bitmap or 4/16/256-
         * color graymap according to the format of embedded bitmap/
         * graymap. */
        if ( src->pixel_mode == FT_PIXEL_MODE_MONO ) {
            dst->pitch *= 8;
        } else if ( src->pixel_mode == FT_PIXEL_MODE_GRAY2 ) {
            dst->pitch *= 4;
        } else if ( src->pixel_mode == FT_PIXEL_MODE_GRAY4 ) {
            dst->pitch *= 2;
        }
        
        /* Adjust for bold and italic text */
        if ( TTF_HANDLE_STYLE_BOLD(font) ) {
            int bump = font->glyph_overhang;
            dst->pitch += bump;
            dst->width += bump;
        }
        if ( TTF_HANDLE_STYLE_ITALIC(font) ) {
            int bump = (int)SDL_ceil(font->glyph_italics);
            dst->pitch += bump;
            dst->width += bump;
        }
        
        if (dst->rows != 0) {
            dst->buffer = (unsigned char *)SDL_malloc( dst->pitch * dst->rows );
            if ( !dst->buffer ) {
                return FT_Err_Out_Of_Memory;
            }
            SDL_memset( dst->buffer, 0, dst->pitch * dst->rows );
            
            for ( i = 0; i < src->rows; i++ ) {
                int soffset = i * src->pitch;
                int doffset = i * dst->pitch;
                if ( mono ) {
                    unsigned char *srcp = src->buffer + soffset;
                    unsigned char *dstp = dst->buffer + doffset;
                    int j;
                    if ( src->pixel_mode == FT_PIXEL_MODE_MONO ) {
                        for ( j = 0; j < src->width; j += 8 ) {
                            unsigned char c = *srcp++;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                            c <<= 1;
                            *dstp++ = (c&0x80) >> 7;
                        }
                    }  else if ( src->pixel_mode == FT_PIXEL_MODE_GRAY2 ) {
                        for ( j = 0; j < src->width; j += 4 ) {
                            unsigned char c = *srcp++;
                            *dstp++ = (((c&0xA0) >> 6) >= 0x2) ? 1 : 0;
                            c <<= 2;
                            *dstp++ = (((c&0xA0) >> 6) >= 0x2) ? 1 : 0;
                            c <<= 2;
                            *dstp++ = (((c&0xA0) >> 6) >= 0x2) ? 1 : 0;
                            c <<= 2;
                            *dstp++ = (((c&0xA0) >> 6) >= 0x2) ? 1 : 0;
                        }
                    } else if ( src->pixel_mode == FT_PIXEL_MODE_GRAY4 ) {
                        for ( j = 0; j < src->width; j += 2 ) {
                            unsigned char c = *srcp++;
                            *dstp++ = (((c&0xF0) >> 4) >= 0x8) ? 1 : 0;
                            c <<= 4;
                            *dstp++ = (((c&0xF0) >> 4) >= 0x8) ? 1 : 0;
                        }
                    } else {
                        for ( j = 0; j < src->width; j++ ) {
                            unsigned char c = *srcp++;
                            *dstp++ = (c >= 0x80) ? 1 : 0;
                        }
                    }
                } else if ( src->pixel_mode == FT_PIXEL_MODE_MONO ) {
                    /* This special case wouldn't
                     * be here if the FT_Render_Glyph()
                     * function wasn't buggy when it tried
                     * to render a .fon font with 256
                     * shades of gray.  Instead, it
                     * returns a black and white surface
                     * and we have to translate it back
                     * to a 256 gray shaded surface.
                     * */
                    unsigned char *srcp = src->buffer + soffset;
                    unsigned char *dstp = dst->buffer + doffset;
                    unsigned char c;
                    int j, k;
                    for ( j = 0; j < src->width; j += 8) {
                        c = *srcp++;
                        for (k = 0; k < 8; ++k) {
                            if ((c&0x80) >> 7) {
                                *dstp++ = NUM_GRAYS - 1;
                            } else {
                                *dstp++ = 0x00;
                            }
                            c <<= 1;
                        }
                    }
                } else if ( src->pixel_mode == FT_PIXEL_MODE_GRAY2 ) {
                    unsigned char *srcp = src->buffer + soffset;
                    unsigned char *dstp = dst->buffer + doffset;
                    unsigned char c;
                    int j, k;
                    for ( j = 0; j < src->width; j += 4 ) {
                        c = *srcp++;
                        for ( k = 0; k < 4; ++k ) {
                            if ((c&0xA0) >> 6) {
                                *dstp++ = NUM_GRAYS * ((c&0xA0) >> 6) / 3 - 1;
                            } else {
                                *dstp++ = 0x00;
                            }
                            c <<= 2;
                        }
                    }
                } else if ( src->pixel_mode == FT_PIXEL_MODE_GRAY4 ) {
                    unsigned char *srcp = src->buffer + soffset;
                    unsigned char *dstp = dst->buffer + doffset;
                    unsigned char c;
                    int j, k;
                    for ( j = 0; j < src->width; j += 2 ) {
                        c = *srcp++;
                        for ( k = 0; k < 2; ++k ) {
                            if ((c&0xF0) >> 4) {
                                *dstp++ = NUM_GRAYS * ((c&0xF0) >> 4) / 15 - 1;
                            } else {
                                *dstp++ = 0x00;
                            }
                            c <<= 4;
                        }
                    }
                } else {
                    SDL_memcpy(dst->buffer+doffset,
                               src->buffer+soffset, src->pitch);
                }
            }
        }
        
        /* Handle the bold style */
        if ( TTF_HANDLE_STYLE_BOLD(font) ) {
            int row;
            int col;
            int offset;
            int pixel;
            Uint8* pixmap;
            
            /* The pixmap is a little hard, we have to add and clamp */
            for ( row = dst->rows - 1; row >= 0; --row ) {
                pixmap = (Uint8*) dst->buffer + row * dst->pitch;
                for ( offset=1; offset <= font->glyph_overhang; ++offset ) {
                    for ( col = dst->width - 1; col > 0; --col ) {
                        if ( mono ) {
                            pixmap[col] |= pixmap[col-1];
                        } else {
                            pixel = (pixmap[col] + pixmap[col-1]);
                            if ( pixel > NUM_GRAYS - 1 ) {
                                pixel = NUM_GRAYS - 1;
                            }
                            pixmap[col] = (Uint8) pixel;
                        }
                    }
                }
            }
        }
        
        /* Mark that we rendered this format */
        if ( mono ) {
            cached->stored |= CACHED_BITMAP;
        } else {
            cached->stored |= CACHED_PIXMAP;
        }
        
        /* Free outlined glyph */
        if ( bitmap_glyph ) {
            FT_Done_Glyph( bitmap_glyph );
        }
    }
    
    /* We're done, mark this glyph cached */
    cached->cached = ch;
    
    return 0;
}

FT_Error Find_Glyph( TTF_Font* font, Uint16 ch, int want, FT_Int charIndex )
{
    int retval = 0;
    int hsize = sizeof( font->cache ) / sizeof( font->cache[0] );
    
    int h = ch % hsize;
    font->current = &font->cache[h];
    
    if (font->current->cached != ch)
        Flush_Glyph( font->current );
    
    if ( (font->current->stored & want) != want ) {
        retval = Load_Glyph( font, ch, font->current, want, charIndex);
    }
    return retval;
}

guiTTFCharacter TTF_GetCharacterAsPoints(TTF_Font *font, Uint16 ch){
//    Uint16 ucs2[2] = { ch, 0 };
//    Uint8 utf8[4];
//    UCS2_to_UTF8(ucs2, utf8);
//    
//    size_t textlen = SDL_strlen((char*)utf8);
    
    FT_Error error = Find_Glyph(font, ch, CACHED_METRICS);
    if ( error ) {
        GUI_LogError("TTF_GetCharacterAsPoints()") << "Couldn't find glyph";
        return guiTTFCharacter();
    }
    
    return makeContoursForCharacter(font->face);
}
