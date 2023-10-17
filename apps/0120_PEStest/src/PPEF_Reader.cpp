//
//  PPEF_Reader.cpp
//  PES3
//
//  Created by SIHARAT THAMMAYA on 9/11/2561 BE.
//  Copyright © 2561 Jimmy Software Co., Ltd. All rights reserved.
//

#include "PPEF_Reader.hpp"
//#include "FTSVG.hpp"
#include "UnicodeHelper.h"
//#include "PES_ToolSet.hpp"
//
//using namespace FTSVG;
//
//// MARK: ofxSatinOutline
//typedef struct ofxSatinOutline_ {
//    int length;
//    std::vector<ofPoint> side0;
//    std::vector<ofPoint> side1;
//} ofxSatinOutline;
//
//// Finds the intersection of two lines given by v1->v2 and v3->v4 and sets the value in the result variable
//void ofx_line_intersectionPoint(const ofPoint & v1, const ofPoint & v2, const ofPoint & v3, const ofPoint & v4, ofPoint* result) {
//    float A2 = v2.y - v1.y;
//    float B2 = v1.x - v2.x;
//    float C2 = A2 * v1.x + B2 * v1.y;
//    
//    float A1 = v4.y - v3.y;
//    float B1 = v3.x - v4.x;
//    float C1 = A1 * v3.x + B1 * v3.y;
//    
//    float det = A1 * B2 - A2 * B1;
//    
//    // TODO: The code below needs revised since division by zero can still occur
//    if (det < 1e-10 && det > -1e-10) {
//        result->x = -10000; /* TODO: What is significant about these numbers? Leave the point, undefined */
//        result->y = -10000; /* TODO: A better solution would be to return an unsigned char(0 parallel, 1 intersecting, 2 not-intersecting) */
//    }
//    
//    result->x = (B2 * C1 - B1 * C2) / det;
//    result->y = (A1 * C2 - A2 * C1) / det;
//}
//
//// Note: assign inset with negative value, outset with positive value
//void ofxSatinOutlineGenerate(const GUI_Polyline &contour, float zigzagWidth, float inset, ofxSatinOutline& result) {
//    auto lines = contour.getVertices();
//    if(contour.isClosed()){
//        if(guiDist(lines.front().x, lines.front().y, lines.back().x, lines.back().y)>0.01){
//            lines.push_back(lines.front());
//        }
//    }
//    int numberOfPoints = (int)lines.size();
//    float halfThickness = zigzagWidth * 0.5f;
//    int intermediateOutlineCount = 2 * numberOfPoints - 2;
//    ofxSatinOutline outline;
//    outline.side0.resize(intermediateOutlineCount);
//    outline.side1.resize(intermediateOutlineCount);
//    
//    for (int i = 1; i<numberOfPoints; i++) {
//        // Perpendicular normalized vector
//        ofVec2f v1 = lines[i] - lines[i - 1];
//        v1.perpendicular();
//        v1 = v1*-1;
//        
//        int j = (i - 1) * 2;
//        
//        ofVec2f temp = v1 * halfThickness;
//        outline.side0[j] = temp + lines[i - 1] + v1*inset;
//        outline.side0[j + 1] = temp + lines[i] + v1*inset;
//        
//        temp = v1 * -halfThickness;
//        outline.side1[j] = temp + lines[i - 1] + v1*inset;
//        outline.side1[j + 1] = temp + lines[i] + v1*inset;
//    }
//    
//    result.side0.resize(intermediateOutlineCount);
//    result.side1.resize(intermediateOutlineCount);
//    
//    result.side0[0] = outline.side0[0];
//    result.side1[0] = outline.side1[0];
//    
//    for (int i = 3; i < intermediateOutlineCount; i += 2) {
//        ofx_line_intersectionPoint(outline.side0[i - 3], outline.side0[i - 2], outline.side0[i - 1], outline.side0[i], &result.side0[(i - 1) / 2]);
//    }
//    
//    for (int i = 3; i < intermediateOutlineCount; i += 2) {
//        ofx_line_intersectionPoint(outline.side1[i - 3], outline.side1[i - 2], outline.side1[i - 1], outline.side1[i], &result.side1[(i - 1) / 2]);
//    }
//    
//    if(contour.isClosed() && contour.size()>2){
//        // Back substitue
//        ofPoint intersect1, intersect2;
//        ofx_line_intersectionPoint(outline.side0[2 * numberOfPoints - 4], outline.side0[2 * numberOfPoints - 3], outline.side0[0], outline.side0[1], &intersect1);
//        ofx_line_intersectionPoint(outline.side1[2 * numberOfPoints - 4], outline.side1[2 * numberOfPoints - 3], outline.side1[0], outline.side1[1], &intersect2);
//        result.side0[0] = result.side0[numberOfPoints - 1] = intersect1;
//        result.side1[0] = result.side1[numberOfPoints - 1] = intersect2;
//    }
//    else if(contour.isClosed()==false){
//        result.side0[numberOfPoints - 1] = outline.side0[2 * numberOfPoints - 3];
//        result.side1[numberOfPoints - 1] = outline.side1[2 * numberOfPoints - 3];
//    }
//    result.length = numberOfPoints;
//}
////std::vector<ofVec2f> ofxSatinOutlineRenderStitches(const ofxSatinOutline& result, float zigzagSpacing);
//

// MARK: UTF8Iterator
UTF8Iterator::UTF8Iterator(const string & str){
//    try{
        utf8::replace_invalid(str.begin(),str.end(),back_inserter(src_valid));
//    }catch(...){
//    }
}

utf8::iterator<std::string::const_iterator> UTF8Iterator::begin() const{
//    try {
        return utf8::iterator<std::string::const_iterator>(src_valid.begin(), src_valid.begin(), src_valid.end());
//    }
//    catch (...) {
//        return utf8::iterator<std::string::const_iterator>();
//    }
}

utf8::iterator<std::string::const_iterator> UTF8Iterator::end() const{
//    try {
        return utf8::iterator<std::string::const_iterator>(src_valid.end(), src_valid.begin(), src_valid.end());
//    }
//    catch (...) {
//        return utf8::iterator<std::string::const_iterator>();
//    }
}

utf8::iterator<std::string::const_reverse_iterator> UTF8Iterator::rbegin() const {
//    try {
        return utf8::iterator<std::string::const_reverse_iterator>(src_valid.rbegin(), src_valid.rbegin(), src_valid.rend());
//    }
//    catch (...) {
//        return utf8::iterator<std::string::const_reverse_iterator>();
//    }
}

utf8::iterator<std::string::const_reverse_iterator> UTF8Iterator::rend() const {
//    try {
        return utf8::iterator<std::string::const_reverse_iterator>(src_valid.rbegin(), src_valid.rbegin(), src_valid.rend());
//    }
//    catch (...) {
//        return utf8::iterator<std::string::const_reverse_iterator>();
//    }
}


// MARK: PPEF_Reader
PPEF_Reader::PPEF_Reader(const char * filePath)
:db(filePath, SQLite::OPEN_READONLY)
{
//    try{
        SQLite::Statement query(db, "SELECT name, value FROM Config");
        while (query.executeStep()) {
            string name = query.getColumn(0);
            if(name=="BaseGap")
                config.BaseGap = query.getColumn(1).getInt();
            else if(name=="ElephantTail")
                config.ElephantTail = query.getColumn(1).getInt();
            else if(name=="FishTail")
                config.FishTail = query.getColumn(1).getInt();
            else if(name=="KiteY")
                config.KiteY = query.getColumn(1).getInt();
            else if(name=="LetterSpace")
                config.LetterSpace = query.getColumn(1).getInt();
            else if(name=="LineHeight")
                config.LineHeight = query.getColumn(1).getInt();
            else if(name=="LowerGap")
                config.LowerGap = query.getColumn(1).getInt();
            else if(name=="OwlTail")
                config.OwlTail = query.getColumn(1).getInt();
            else if(name=="SpaceGap")
                config.SpaceGap = query.getColumn(1).getInt();
            else if(name=="UpperGap")
                config.UpperGap = query.getColumn(1).getInt();
            else if(name=="UpperVovelGap")
                config.UpperVovelGap = query.getColumn(1).getInt();
            
            //GUI_LogNotice("PPEF_Reader") << "row (" << name << ", " <<  query.getColumn(1).getInt() << ")";
        }
        
        
        
        query.reset();
//    }
//    catch (const std::exception& e)
//    {
//        GUI_LogError("Query DB") << "SQLite exception: " << e.what();
//    }
}

PPEF_Reader::~PPEF_Reader(){
    
}

vector<pesPath> PPEF_Reader::getStringAsShapes(const string & utf8, float scale, int extraLetterSpace, int extraSpace){
    
    vector<pesPath> shapes;
    
    // Check for Thai language
    bool bThaiAlphabet = false;
    for(auto c : UTF8Iterator(utf8)){
        if(th_isThaiLetter(c)){
            bThaiAlphabet = true;
            break;
        }
    }
    
    vector<uint32_t> uniString;
    uint32_t prev = -1;
    for(auto unichar: UTF8Iterator(utf8)){
        
        if(bThaiAlphabet){
            // If input utf8-string contain Thai aphabet.
            if(unichar==THAI_SARA_AM) // Decompose THAI_SARA_AM to THAI_NIKHAHIT + THAI_SARA_AA
            {
                if(prev!=-1 && th_isToneAndDiacritic(prev)){
                    uniString.back() = THAI_NIKHAHIT;
                    uniString.push_back(prev);
                    uniString.push_back(THAI_SARA_AA);
                }
                else{
                    uniString.push_back(THAI_NIKHAHIT);
                    uniString.push_back(THAI_SARA_AA);
                }
            }
            else
            {
                if(prev!=-1){
                    if(th_isBelowVowelAndDiacritic(unichar)){ // Substitue THAI_THO_THAN & THAI_YO_YING when followed by Lower vowel
                        if(prev==THAI_THO_THAN)
                            uniString.back() = THAI_THO_THAN_NONE_TAIL;
                        else if(prev==THAI_YO_YING)
                            uniString.back() = THAI_YO_YING_NONE_TAIL;
                    }
                }
                uniString.push_back(unichar);
            }
            prev = unichar;
        }
        else{
            uniString.push_back(unichar);
        }
    }
    
    const static int baseline = 150;
    int xstart = 0;
    int ystart = -baseline;
//    try{
        SQLite::Statement query(db, "SELECT data FROM Data WHERE unicode = ?");
        int num = uniString.size();
        
        uint32_t ch = uniString.front();
        uint32_t lastch = ch;
        int lastLastCh = 0;
        
        for(int i=0; i<num; i++){
            float xmin = 0;
            float advance = 0;
            float xoffset = 0;
            float yoffset = 0;
            pesPath chshape = pesPath();
            ch = uniString[i];
            query.bind(1, ch);
            
            bool bExist = false;
            if(query.executeStep()){
                bExist = true;
                SQLite::Column colBlob = query.getColumn(0);
                const void* blob = colBlob.getBlob(); // Get a pointer to the bytes.
                std::size_t size = colBlob.getBytes(); // Get the number of bytes.
                pesBuffer buffer(static_cast<const char*>(blob), size); // Create a buffer with those bytes.
                if(buffer.size()){
                    int offset;
                    char* c = buffer.getData();
                    memcpy(&xmin, c, sizeof(float)); // read xmin
                    c+=sizeof(float);
                    memcpy(&advance, c, sizeof(float)); // read advance
                    c+=sizeof(float);
                    memcpy(&offset, c, sizeof(int)); // read data offset(jump to address)
                    
                    c = buffer.getData();
                    int ptr = offset;
                    int bufferSize = (int)buffer.size();
                    while(ptr<bufferSize){
                        pesPath::Command com(pesPath::Command::_moveTo);
                        size_t byte_read = sizeof(pesPath::Command);// - sizeof(string);
                        memcpy(&com, &c[ptr], byte_read );
                        chshape.addCommand(com);
                        ptr+=byte_read;
                    }
                }
            }
            
            if(bExist){
                
                int extSpace = (ch == 0x20) ? extraSpace : extraLetterSpace;
                
                if(bThaiAlphabet){
                    if( th_isToneAndDiacritic(ch) ) { // ญี่ปุ่น
                        if( th_isBelowVowel(lastch)) {
                            lastch = lastLastCh;
                            lastLastCh = 0;
                        }
                    }
                    
                    if(th_isAboveVowelAndDiacritic(ch) || th_isToneAndDiacritic(ch)){
                        if(th_isAscenderConsonant(lastch)){ // (ป, ฝ, ฟ)
                            xoffset -= config.FishTail;
                        }
                        else if(lastch==THAI_CHO_CHANG || lastch==THAI_SO_SO || // (ช ซ ศ ส)
                                lastch==THAI_SO_SALA || lastch==THAI_SO_SUA)
                        {
                            xoffset -= config.ElephantTail;
                        }
                        else if(lastch==THAI_LO_CHULA){ // ฬ
                            yoffset -= config.KiteY;
                        }
                        else if(lastch==THAI_HO_NOKHUK){ // ฮ
                            xoffset -= config.OwlTail;
                        }
                    }
                    
                    if(th_isAboveVowelAndDiacritic(lastch)){
                        if(th_isToneAndDiacritic(ch)){
                            if(th_isAscenderConsonant(lastLastCh)){ // (ป, ฝ, ฟ)
                                xoffset -= config.FishTail;
                            }
                            else if(lastLastCh==THAI_CHO_CHANG || lastLastCh==THAI_SO_SO || // (ช ซ ศ ส)
                                    lastLastCh==THAI_SO_SALA || lastLastCh==THAI_SO_SUA)
                            {
                                xoffset -= config.ElephantTail;
                            }
                            else if(lastLastCh==THAI_LO_CHULA){ // ฬ
                                yoffset -= config.KiteY;
                            }
                            else if(lastLastCh==THAI_HO_NOKHUK){ // ฮ
                                xoffset -= config.OwlTail;
                            }
                            yoffset -= config.UpperVovelGap;
                        }
                    }
                    
                    if(th_isBelowVowelAndDiacritic(ch)){
                        if(lastch==THAI_RU || lastch==THAI_LU){
                            xoffset -= config.FishTail;
                        }
                        else if(lastch==THAI_DO_CHADA || lastch==THAI_TO_PATAK){
                            yoffset += config.BaseGap;
                        }
                    }
                    
//                    if(ch==THAI_MAI_HAN_AKAT){
//                    }
//                    else if(ch>=THAI_SARA_I && ch<=THAI_PHINTHU){
//                    }
//                    else if(ch>=THAI_MAITAIKHU && ch<=THAI_YAMAKKAN){
//                    }
                    
                    
                    if(th_isAboveVowelAndDiacritic(ch) || th_isBelowVowelAndDiacritic(ch) || th_isToneAndDiacritic(ch)){
                        xoffset -= (config.LetterSpace + extSpace);
                    }
                    
                    
                } // end if(bThaiAlphabet)
                else{

                }
            
                float tx = (float)(xstart - xmin + xoffset);
                float ty = (float)(ystart + yoffset);
                chshape.translate(tx, ty);
                
                xstart += advance;
                bool bZeroAdvance = advance==0;
                if(bZeroAdvance==false){
                    xstart += (config.LetterSpace + extSpace);
                }
                
                if(chshape.getCommands().size()){
                    chshape.setFilled(false);
                    chshape.setStrokeWidth(2);
                    chshape.setStrokeHexColor(0x0000ff);
                    shapes.push_back(chshape);
                }
                
            } // end if(bExist)
            
            if(i>0){
                lastLastCh = lastch;
                lastch = ch;
            }
            query.reset();
        }
//    }
//    catch (const std::exception& e)
//    {
//        GUI_LogError("Query DB") << "SQLite exception: " << e.what();
//    }
    
    //----- TEST
//    PPEF_Effect effect; // no effect
    
//    PPEF_EffectBorder effect; // rectangle border
    
//    PPEF_EffectCircle effect; // circle-top w/ border
//    effect.bCreateBorder = true;
    
//    PPEF_EffectCircleBottom effect; // circle-bottom w/ border
//    effect.bCreateBorder = true;
    
//    PPEF_EffectItalic effectItalic; // Italic
//    effectItalic.apply(shapes);

//    if(effect.apply!=nullptr){
//        effect.apply(shapes);
//    }
    
//    PPEF_EffectWave().apply(shapes); // wave w/ border
    
//    PPEF_EffectChevron chevron;
//    chevron.bCreateBorder = true;
//    chevron.bItalic = true;
//    chevron.apply(shapes);
    
//    PPEF_EffectCircle().apply(shapes);
//    PPEF_EffectSlant(false).apply(shapes);
    
//#ifndef PPEF_TEST_EFFECT
//    #define PPEF_TEST_EFFECT    1
//#endif
    
    //----- END TEST
    
#if !PPEF_TEST_EFFECT
//    if(effect!=nullptr && effect->apply!=nullptr){
//        effect->apply(shapes);
//    }
#endif
    
    
    if(scale!=1.0){
        for(pesPath & s : shapes){
            s.scale(scale, scale);
        }
    }
    
    return shapes;
}


