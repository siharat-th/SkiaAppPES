//
//  GUI_FontAtlas.hpp
//  GUITextField
//
//  Created by SIHARAT THAMMAYA on 4/29/17.
//  Copyright © 2017 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef GUI_FontAtlas_hpp
#define GUI_FontAtlas_hpp

#include <stdio.h>
#include <vector>
#include "SDL_gui.h"
#include "utf8.h"
#include "Shaper.hpp"
//#include <ft2build.h>
//#include FT_FREETYPE_H
//#include FT_OUTLINE_H
//#include FT_STROKER_H
//#include FT_GLYPH_H
//#include FT_TRUETYPE_IDS_H

typedef Shaper guiTTFCharacter;
guiTTFCharacter TTF_GetCharacterAsPoints(TTF_Font *font, Uint16 ch);

class GUI_Unicode{
public:
    struct range{
        std::uint32_t begin;
        std::uint32_t end;
        
        std::uint32_t getNumGlyphs() const{
            return end - begin + 1;
        }
    };
    
    static const range Space;
    static const range Latin;
    static const range Latin1Supplement;
    static const range LatinExtendedAdditional;
    
    static const range CurrencySymbols;
    static const range LetterLikeSymbols;
    static const range NumberForms;
    static const range Arrows;
    static const range MathOperators;
    
    static const range GeometricShapes;
    static const range MiscSymbols;
    
    static const range Thai;
    static const range ThaiExtended;
    
    static const range MiscSymbolsAndPictographs;
    static const range Emoticons;
    static const range TransportAndMap;
};

class GUI_Alphabet{
public:
    static const std::initializer_list<GUI_Unicode::range> Latin;
    static const std::initializer_list<GUI_Unicode::range> Thai;
    static const std::initializer_list<GUI_Unicode::range> Emoji;
};

class GUI_FontAtlasSetting{
    
    friend class GUI_FontAtlas;
    vector<GUI_Unicode::range> ranges;
    
public:
    GUI_FontAtlasSetting(const std::filesystem::path & name, int size)
    :fontName(name)
    ,fontSize(size){}
    
    std::filesystem::path fontName;
    int fontSize;
    
    void addRanges(std::initializer_list<GUI_Unicode::range> alphabet){
        ranges.insert(ranges.end(), alphabet);
    }
    
    void addRange(const GUI_Unicode::range & range){
        ranges.push_back(range);
    }
};

class GUI_UTF8Iterator{
public:
    GUI_UTF8Iterator(const string & str);
    utf8::iterator<std::string::const_iterator> begin() const;
    utf8::iterator<std::string::const_iterator> end() const;
    utf8::iterator<std::string::const_reverse_iterator> rbegin() const;
    utf8::iterator<std::string::const_reverse_iterator> rend() const;
    
private:
    std::string src_valid;
};



class GUI_FontAtlas{
    friend class GUI_TextField;
public:
    GUI_FontAtlas();
    virtual ~GUI_FontAtlas();
    
    bool load(const std::string& filename, int fontsize);
    bool load(const GUI_FontAtlasSetting & setting);
    
    bool isLoaded() const;
    std::size_t	getNumCharacters() const;
    
    int getLineHeight() const;
    void setLineHeight(int height);
    
    float getAscenderHeight() const;
    float getDescenderHeight() const;
    
    float getLetterSpacing() const;
    void setLetterSpacing(float spacing);
    
    float getSpaceSize() const;
    void setSpaceSize(float size);
    
    int getFontTextureWidth() const;
    int getFontTextureHeight() const;
    
    void setInputText(std::string utf8);
    void drawOutputText(int x, int y);
    
    void drawChar(uint32_t c, int x, int y) const;
    void drawString(const std::string& s, int x, int y) const;
    
    void drawCharAsShapes(uint32_t c, int x, int y);
    void drawStringAsShapes(const std::string& s, int x, int y);
    
    
    const SDL_Texture* getFontTexture() const;
    void setColor(SDL_Color c) const;
    
    bool isValidGlyph(uint32_t glyph) const;
    
    bool isWordBreak(unsigned int c);
    vector<string> splitLine(string c, float rectWidthLimit, float rectHeightLimit);
    std::string splitLineConcat(std::string c, float rectWidthLimit, float rectHeightLimit);
    void setPosAdjusts(vector<guiVec2f> _adjusts);
    
    SDL_Rect getStringBoundingBox(std::string c, float x, float y);
    static void stringAppendUTF8(string & str, uint32_t utf8);
    
    bool isExceedLimit();
    
    // custom character map
    void setCharMapIndex(std::map<Uint16, unsigned int> charMap);
    // outline
    void setCreateContour(bool createContour);
    
    guiTTFCharacter getCharacterAsPoints(Uint16 ch);
    vector<guiTTFCharacter> getStringAsPoints(const std::string& str);
    
protected:
    
    bool bLoadedOk;
    int lineHeight;
    int ascenderHeight;
    int descenderHeight;
    float letterSpacing;
    float spaceSize;
    int fontTextureWidth, fontTextureHeight;
    
    struct glyphProps{
        std::size_t characterIndex;
        uint32_t glyph;
        int height;
        int width;
        int bearingX, bearingY;
        int xmin, xmax, ymin, ymax;
        int advance;
        int t1,v1;
        int tW,tH;
    };
    
    struct glyph{
        glyphProps props;
        SDL_Surface *surf;
    };
    
    vector<glyphProps> cps; // properties for each character
    GUI_FontAtlasSetting setting;
    unordered_map<uint32_t,size_t> glyphIndexMap;
    
    int getKerning(uint32_t c, uint32_t prevC) const;
    glyph loadGlyph(uint32_t utf8) const;
    const glyphProps & getGlyphProperties(uint32_t glyph) const;
    size_t indexForGlyph(uint32_t glyph) const;
    
    void iterateString(const string & str, int x, int y, std::function<void(uint32_t, int x, int y)> f) const;
    
    SDL_Texture* texAtlas;
    
    std::string inputText;  // input raw text data
    std::string outputText; // text to rendering
    
    bool bResizable;
    
    vector<guiVec2f> adjusts;
    
//    void onSizeChange(int w, int h);
    
    // custom character map
    std::map<Uint16, unsigned int> charMap;
    
    std::map<Uint16, guiTTFCharacter> charOutlinesContour;
    std::map<Uint16, std::vector<guiVec2f>> keyPoints;
    
private:
    TTF_Font* font;
    static const glyphProps invalidProps;
    bool exceedLimit;
    
    bool createContour;
};

#endif /* GUI_FontAtlas_hpp */
