//
//  LocalizedString.hpp
//  FontWorks
//
//  Created by SIHARAT THAMMAYA on 1/21/17.
//  Copyright © 2017 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef LocalizedString_h
#define LocalizedString_h

#include <stdio.h>
#include <string>
#include <locale>
#include "jsFileUtils.h"
#include "utf8.h"

// MARK: cross-platform Localizable string
// For rendering need TTF Font that contained unicode characters
// Arial Unicode MS is recommened
int LSTR_load(const std::string& locale_str = "en");

int LSTR_clear();

inline bool ends_with(std::string const & value, std::string const & ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::locale getLocale(const std::string& locale);
int LSTR_setLocale(std::string loc);

std::string LSTR(const std::string& str);

// MARK: Unicode Utility (from openframeworks)
// types
typedef char          ofChar;     // a standard signed char
typedef const char*   ofCharPtr;  //
typedef unsigned char ofUTF8Char; // a UTF8 type (unsigned char)
typedef ofCharPtr     ofUTF8Ptr;  // a pointer type used when traversing
// a UTF8 encoded std::string.
// the "beginning" of the string would be
// ofUTF8Ptr b = input.c_str();
// the "end" of the string would be
// ofUTF8Ptr e = input.c_str() + input.length();
// this is wierd.

typedef unsigned short ofUTF16Char; // a UTF16 type - stay clear of wstring. it's 4 bytes on *NIX, and 2 on Win
typedef unsigned int   ofUTF32Char; // a UTF32 type
typedef ofUTF32Char    ofUniChar;   // a unicode character type for clarity

typedef const ofUTF16Char* ofUTF16Ptr;   // a pointer to wstring, like above
typedef const ofUTF32Char* ofUTF32Ptr;   // a pointer to unicode character
typedef const ofUniChar*   ofUniCharPtr; // a pointer to unicode character

typedef string                    ofUTF8String;  //
typedef basic_string<ofUTF16Char> ofUTF16String; // std:wstring is a mess cross platform
typedef basic_string<ofUTF32Char> ofUTF32String; // aka unicode string
typedef ofUTF32String             ofUniString;   // this is a "unicode" string
// each vector entry contains one
// unicode code point.
// It is NOT UTF8 encoded.
typedef utf8::iterator<ofUTF8Ptr> ofUTF8Iterator;

class ofUTF8 {
public:
    ////////////////////////////////////////////////////////////////////
    // UTF8 MANIPULATION TOOLS /////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    // append unicode char to UTF8 string
    //static ofUTF8String   append(ofUTF8String txt, ofUniChar unicode);
    //static ofUTF8String   append(ofUTF8String txt, ofUniString unicode);
    //static ofUTF8String&  appendInPlace(ofUTF8String& txt, ofUniChar unicode);

    // check to see if a UTF8 string is valid
    static bool isValid(string txt);
    static bool isValid(ofUTF8Ptr iter, ofUTF8Ptr end);

    // check to see if a string starts with a UTF8 BOM (byte order mark)
    static bool startsWithBOM(ofUTF8String txt);
    static bool startsWithBOM(ofUTF8Ptr iter, ofUTF8Ptr end);

    // attempt to repair a broken UTF8 string
    static ofUTF8String  repair(string txt, ofUniChar replacement = -1);
    static ofUTF8String& repairInPlace(string& txt, ofUniChar replacement = -1);

    // calculate the number of UTF8 chars (string.length() returns the number of bytes)
    static int distance(ofUTF8String txt);
    static int distance(ofUTF8Ptr iter, ofUTF8Ptr end);

    // case insensitive comparison of UTF8 strings
    static int icompare(const ofUTF8String& utf8String0, const ofUTF8String& utf8String1);

    // unicode-based case conversion
    static ofUTF8String  toUpper(const ofUTF8String& str);
    static ofUTF8String& toUpperInPlace(ofUTF8String& str);
    static ofUTF8String  toLower(const ofUTF8String& str);
    static ofUTF8String& toLowerInPlace(ofUTF8String& str);

    ////////////////////////////////////////////////////////////////////
    // UTF8 ITERATION TOOLS ////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////

    // custom iterator helper
    static ofUTF8Ptr beginPtr(const ofUTF8String& input); // gets the start pointer to a string (input.c_str())
    static ofUTF8Ptr endPtr(const ofUTF8String& input);
    // custom bi-directional iterator
    static ofUTF8Iterator iterator(const ofUTF8String& input);
    static ofUTF8Iterator begin(const ofUTF8String& input);
    static ofUTF8Iterator end(const ofUTF8String& input);

    // Checked Iterators - No need to check input
    ////////////////////////////////////////////////////////////////////
    static ofUniChar getNext(ofUTF8Ptr& iter, ofUTF8Ptr end);  // will increment iter and return value
    static ofUniChar getPrior(ofUTF8Ptr& iter, ofUTF8Ptr end); // will increment iter and return value
    static ofUTF8Ptr advance(ofUTF8Ptr& iter, ofUTF8Ptr end, int numToSkip = 1); // skips N unicode chars
    //------------------------------------------------------------------
    //------------------------------------------------------------------
    static ofUniChar get(ofUTF8Ptr iter, ofUTF8Ptr end);  // reads the unichar at the current iter position
    static ofUTF8Ptr next(ofUTF8Ptr iter, ofUTF8Ptr end); // moves the iterator to the next unicode start
    static ofUTF8Ptr prior(ofUTF8Ptr iter, ofUTF8Ptr start); // moves the iterator to the previous unicode start
    //------------------------------------------------------------------
    //------------------------------------------------------------------
    static ofUniChar get(const ofUTF8String& input, ofUTF8Ptr iter = NULL);  // reads the unichar at the current iter position
    static ofUTF8Ptr next(const ofUTF8String& input, ofUTF8Ptr iter = NULL); // moves the iterator to the next unicode start
    static ofUTF8Ptr prior(const ofUTF8String& input, ofUTF8Ptr iter = NULL); // moves the iterator to the previous unicode start
    static ofUTF8Ptr advance(const ofUTF8String& input, ofUTF8Ptr iter = NULL, int numToSkip = 1); // skips N unicode chars

    // Unchecked Iterators - Should check input string for valid UTF8 first
    ////////////////////////////////////////////////////////////////////
    static ofUniChar get(ofUTF8Ptr iter);  // reads the unichar at the current iter position
    static ofUTF8Ptr next(ofUTF8Ptr iter); // moves the iterator to the next unicode start
    static ofUTF8Ptr prior(ofUTF8Ptr iter); // moves the iterator to the previous unicode start
    static ofUTF8Ptr advance(ofUTF8Ptr iter, int numToSkip = 1); // skips N unicode chars

    static ofUniChar getNext(ofUTF8Ptr& iter); // moves the iterator to the next unicode start and returns it
    static ofUniChar getPrior(ofUTF8Ptr& iter); // moves the iterator to the previous unicode start and returns it
};

class ofTextConverter {
public:
    // convert between string encodings
    //    static string        convert(const ofBuffer& buffer, ofTextEncoding inputEncoding, ofTextEncoding outputEncoding);
    //    static string        convert(const void* source, int length,  ofTextEncoding inputEncoding, ofTextEncoding outputEncoding);
    //    static string        convert(const string& input, ofTextEncoding inputEncoding, ofTextEncoding outputEncoding);

    // to UTF8
    static ofUTF8String toUTF8(const ofUTF16String& input);  // utf16-utf8

    static ofUTF8String toUTF8(const ofUniChar& input);      // utf32-utf8
    static ofUTF8String toUTF8(const ofUniString& input);    // utf32-utf8

    //    // to UTF16
    //    static ofUTF16String toUTF16(const ofUTF8String& input); // utf8->utf16
    //
    //    static ofUTF16String toUTF16(const ofUniChar& input);    // utf32->utf16
    //    static ofUTF16String toUTF16(const ofUniString& input);  // utf32->utf16

    // to UTF32 / Unicode
    static ofUniString toUTF32(const ofUTF8String& input);   // utf8->utf32
    static ofUniString toUTF32(const ofUTF16String& input);  // utf16->utf32

    // to UTF32 / Unicode
    static ofUniString toUnicode(const ofUTF8String& input);   // utf8->utf32
    static ofUniString toUnicode(const ofUTF16String& input);  // utf16->utf32

};

#endif /* LocalizedString_h */
