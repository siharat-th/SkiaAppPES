//
//  PPEF_Reader.hpp
//  PES3
//
//  Created by SIHARAT THAMMAYA on 9/11/2561 BE.
//  Copyright © 2561 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef PPEF_Reader_hpp
#define PPEF_Reader_hpp

#include "SQLiteCpp/SQLiteCpp.h"
#include "pesEngine.h"
#include "utf8.h"

/*
class PPEF_Effect{
public:
    PPEF_Effect()
    :apply(nullptr)
    ,bItalic(false)
    ,bCreateBorder(false)
    ,borderGap(0)
    ,borderGapY(0)
    {
        
    }
    
    std::function<void(vector<GUI_Path *> & paths)> apply;
    guiRectangle getBoundingBox(const vector<GUI_Path *> & paths);
    GUI_Path *createBorder(const GUI_Polyline &outline);
    
    bool bItalic;
    bool bCreateBorder;
    int borderGap;
    int borderGapY;
};

class PPEF_EffectNormal : public PPEF_Effect{
public:
    PPEF_EffectNormal();
};

class PPEF_EffectArchTop : public PPEF_Effect{
public:
    PPEF_EffectArchTop();
    
    float angle = -1;
    float radius = -1; // note: -1 mean calculate auto radius
    int borderGap;
    int borderGapY;
};

class PPEF_EffectArchBottom : public PPEF_Effect{
public:
    PPEF_EffectArchBottom();
    
    float angle = -1;
    float radius = -1; // note: -1 mean calculate auto radius
    int borderGap;
    int borderGapY;
};

class PPEF_EffectItalic : public PPEF_Effect{
public:
    PPEF_EffectItalic();
};

class PPEF_EffectSineWave : public PPEF_Effect{
public:
    PPEF_EffectSineWave();
    
    int magnitude = 150;
};

class PPEF_EffectChevron : public PPEF_Effect{
public:
    PPEF_EffectChevron(bool bChevronTop=false);
};

class PPEF_EffectCircle : public PPEF_Effect{
public:
    PPEF_EffectCircle();
};

class PPEF_EffectSlant : public PPEF_Effect{
public:
    PPEF_EffectSlant(bool bSlantUp=true);
    
    int angle = 30;
};

class PPEF_EffectFadeRight : public PPEF_Effect{
public:
    PPEF_EffectFadeRight();
};

class PPEF_EffectFadeLeft : public PPEF_Effect{
public:
    PPEF_EffectFadeLeft();
};

class PPEF_EffectFadeUp : public PPEF_Effect{
public:
    PPEF_EffectFadeUp();
    
    int slantFactor = 10; // unit percent
};

class PPEF_EffectFadeDown : public PPEF_Effect{
public:
    PPEF_EffectFadeDown();
    
    int slantFactor = 10; // unit percent
};

class PPEF_EffectTriangleUp : public PPEF_Effect{
public:
    PPEF_EffectTriangleUp();
};

class PPEF_EffectTriangleDown : public PPEF_Effect{
public:
    PPEF_EffectTriangleDown();
};

class PPEF_EffectInflate : public PPEF_Effect{
public:
    PPEF_EffectInflate();
};
 */

class PPEF_Reader{
public:
    PPEF_Reader(const char * filePath);
    ~PPEF_Reader();
  
    // Note: PPEF define a hight of Latin Capital Letter "M" with 300 pt.(from baseline to topmost)
    // scale with 1/3 will change it height to 100 pt.(1 cm. in PES)
//    vector<GUI_Path *> getStringAsShapes(const string & str, float scale=1/3.0, PPEF_Effect* effect=nullptr, int extraLetterSpace=0, int extraSpace=0);
    
    vector<pesPath> getStringAsShapes(const string & str,
                                        float scale=1/3.0,
                                        int extraLetterSpace=0,
                                        int extraSpace=0);
    
private:
    SQLite::Database db;
    
    struct{
        int LetterSpace = 0;
        int LineHeight = 764;
        int SpaceGap = 150;
        int LowerGap = 25;
        int BaseGap = 120;
        int UpperGap = 16;
        int UpperVovelGap = 145;
        int FishTail = 65;
        int ElephantTail = 25;
        int OwlTail = 25;
        int KiteY = 50;
    }config;
};

class UTF8Iterator{
public:

    UTF8Iterator(const std::string & str);
    utf8::iterator<std::string::const_iterator> begin() const;
    utf8::iterator<std::string::const_iterator> end() const;
    utf8::iterator<std::string::const_reverse_iterator> rbegin() const;
    utf8::iterator<std::string::const_reverse_iterator> rend() const;

private:
    /// \brief A copy of the validated UTF8-encoded std::string for validation.
    ///
    /// If the UTF8-encoded std::string passed to the constructor
    /// `ofUTF8Iterator(...)` is invalid, this variable will be empty.
    std::string src_valid;
};

#endif /* PPEF_Reader_hpp */
