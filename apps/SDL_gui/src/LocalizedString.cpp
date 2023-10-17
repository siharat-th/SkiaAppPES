//
//  LocalizedString.cpp
//  FontWorks
//
//  Created by SIHARAT THAMMAYA on 1/21/17.
//  Copyright © 2017 Jimmy Software Co., Ltd. All rights reserved.
//

#include "LocalizedString.hpp"
#include "SDL_gui.h"
#include "GUI_Utils.h"

typedef std::map<std::string, ofUniString> _STRING_PAIR_;
static std::map<std::string, _STRING_PAIR_> __localizedStrings;
static std::string __strLocale = "en";

// MARK: cross-platform Localizable string
// Need TTF Font that contained unicode characters
// Arial Unicode MS is recommened
int LSTR_load(const std::string& locale_str) {
    jsBuffer buff = jsBufferFromFile(locale_str + "/Localizable.strings");
    if (buff.size()) {
        for (jsBuffer::Line it = buff.getLines().begin(), end = buff.getLines().end(); it != end; ++it) {
            string line = *it;


            if (line.empty() == false) {
                line = guiTrim(line);
                if (line.find("//") == 0) { // comment
                } else {
                    if (ends_with(line, ";")) {
                        line = line.substr(0, line.size() - 1);
                        GUI_LogVerbose("LSTR") << line;
                        std::vector<std::string> sep = guiSplitString(line, "=", true, true);
                        if (sep.size() == 2) {
                            guiStringReplace(sep[0], "\"", "");
                            guiStringReplace(sep[1], "\"", "");

                            GUI_LogVerbose("sep") << sep[0] << ", " << sep[1];

                            // ERROR: store string(utf-8) in map cause unknow error string data(non lalin) might invalid
                            // FIXED: convert/store Unicode(utf-32)
                            __localizedStrings[locale_str][sep[0]] = ofTextConverter::toUnicode(sep[1]);

                        } else if (sep.size() > 2) {
                            std::string ssep[2];
                            int idx = 0;
                            int round = 0;
                            for (auto s : sep) {
                                if (round++ > 0)
                                    ssep[idx].append("=");
                                ssep[idx].append(s);
                                if (idx == 0 && ends_with(s, "\"")) {
                                    idx = 1;
                                    round = 0;
                                }
                            }
                            GUI_LogVerbose("ssep") << ssep[0] << ", " << ssep[1];

                            // ERROR: store string(utf-8) in map cause unknow error string data(non lalin) might invalid
                            // FIXED: convert/store Unicode(utf-32)
                            __localizedStrings[locale_str][ssep[0]] = ofTextConverter::toUnicode(ssep[1]);
                        }

                    }
                }
            }
        }
    }
    return 0;
}

std::locale getLocale(const string & locale) {
    std::locale loc;
//    try {
        loc = std::locale(locale.c_str());
//    } catch (...) {
//        GUI_LogWarning("GUI_Utils") << "Couldn't create locale " << locale << " using default, " << loc.name();
//    }
    return loc;
}

int LSTR_setLocale(std::string loc) {
    __strLocale = loc;
    return 0;
}

int LSTR_clear() {
    // All clear.
    for (auto l : __localizedStrings) {
        l.second.clear();
    }
    __localizedStrings.clear();
    return 0;
}

std::string LSTR(const std::string& key) {

    if (__localizedStrings.find(__strLocale) != __localizedStrings.end()) {
        _STRING_PAIR_ lstr = __localizedStrings[__strLocale];
        if (lstr.find(key) != lstr.end()) {
            string s = ofTextConverter::toUTF8(lstr[key]);
            if (!ofUTF8::isValid(s)) {
                GUI_LogNotice() << "repair";
                ofUTF8::repairInPlace(s);
            }
            GUI_LogVerbose("LSTR") << "Found, convet utf32->utf8: " << guiToString(&lstr[key]) << "->" << s;
            return s;
        }
    }

    return key;
}


// MARK: Unicode Utility (from openframeworks)
//------------------------------------------------------------------
bool ofUTF8::isValid(ofUTF8String txt) {
//    try {
        ofUTF8String::iterator end_it = utf8::find_invalid(txt.begin(), txt.end());
        if (end_it != txt.end()) {
            ofUTF8String validPart(txt.begin(), end_it);
            GUI_LogError("ofUTF8::isValid - detected invalid UTF8 - valid part: " + validPart);
            return false;
        } else {
            return true;
        }
//    } catch (const utf8::exception& utfcpp_ex) {
//        string err = utfcpp_ex.what();
//        GUI_LogError("ofUTF8::isValid : " + err);
//        return false;
//    }
}

//------------------------------------------------------------------
bool ofUTF8::isValid(ofUTF8Ptr iter, ofUTF8Ptr end) {
//    try {
        bool isValid = utf8::is_valid(iter, end);
        if (!isValid) {
            GUI_LogError("ofUTF8::isValid - detected invalid UTF8");
            return false;
        } else {
            return true;
        }
//    } catch (const utf8::exception& utfcpp_ex) {
//        GUI_LogError("ofUTF8::isValid : " + guiToString(utfcpp_ex.what()));
//        return false;
//    }

}

//------------------------------------------------------------------
bool ofUTF8::startsWithBOM(ofUTF8String txt) {
    return startsWithBOM(beginPtr(txt), endPtr(txt));
}

//------------------------------------------------------------------
bool ofUTF8::startsWithBOM(ofUTF8Ptr iter, ofUTF8Ptr end) {
    return utf8::starts_with_bom(iter, end);
}

//------------------------------------------------------------------
ofUTF8String ofUTF8::repair(ofUTF8String txt, ofUniChar replacement) {
    ofUTF8String temp;
//    try {
        if (replacement == -1) {
            utf8::replace_invalid(txt.begin(), txt.end(), back_inserter(temp));
        } else {
            utf8::replace_invalid(txt.begin(), txt.end(), back_inserter(temp), replacement);
        }
//    } catch (const utf8::exception& utfcpp_ex) {
//        GUI_LogError("ofUTF8::repair : " + guiToString(utfcpp_ex.what()));
//    }
    return temp;
}

//------------------------------------------------------------------
ofUTF8String& ofUTF8::repairInPlace(ofUTF8String& txt, ofUniChar replacement) {
    txt = repair(txt, replacement);
    return txt;
}
//------------------------------------------------------------------
int ofUTF8::distance(ofUTF8String txt) {
//    try {
        return (int)utf8::distance(txt.begin(), txt.end());
//    } catch (const utf8::exception& utfcpp_ex) {
//        GUI_LogError("ofUTF8::distance : " + guiToString(utfcpp_ex.what()));
//    }
    return -1;
}

//------------------------------------------------------------------
int ofUTF8::distance(ofUTF8Ptr iter, ofUTF8Ptr end) {
//    try {
        return (int)utf8::distance(iter, end);
//    } catch (const utf8::exception& utfcpp_ex) {
//        GUI_LogError("ofUTF8::distance : " + guiToString(utfcpp_ex.what()));
//    }
    return -1;
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::beginPtr(const ofUTF8String& input) {
    return input.c_str();
}
//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::endPtr(const ofUTF8String& input) {
    return input.c_str() + input.length();
}
//------------------------------------------------------------------
ofUTF8Iterator ofUTF8::iterator(const ofUTF8String& input) {
    return ofUTF8Iterator(beginPtr(input), beginPtr(input), endPtr(input));
}

//------------------------------------------------------------------
ofUTF8Iterator ofUTF8::begin(const ofUTF8String& input) {
    return iterator(input);
}

//------------------------------------------------------------------
ofUTF8Iterator ofUTF8::end(const ofUTF8String& input) {
    return ofUTF8Iterator(endPtr(input), beginPtr(input), endPtr(input));
}


//------------------------------------------------------------------
ofUniChar ofUTF8::getNext(ofUTF8Ptr& iter, ofUTF8Ptr end) {
//    try {
        return utf8::next(iter, end);
//    } catch (const utf8::exception& utfcpp_ex) {
//        GUI_LogError("ofUTF8::getNext : " + guiToString(utfcpp_ex.what()));
//        return -1;
//    }
}

//------------------------------------------------------------------
ofUniChar ofUTF8::getPrior(ofUTF8Ptr& iter, ofUTF8Ptr end) {
//    try {
        return utf8::prior(iter, end);
//    } catch (const utf8::exception& utfcpp_ex) {
//        GUI_LogError("ofUTF8::getPrior : " + guiToString(utfcpp_ex.what()));
//        return -1;
//    }
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::advance(ofUTF8Ptr& iter, ofUTF8Ptr end, int numToSkip) {
//    try {
        utf8::advance(iter, numToSkip, end);
        return iter;
//    } catch (const utf8::exception& utfcpp_ex) {
//        string err = utfcpp_ex.what();
//        GUI_LogError("ofUTF8::advance : " + err);
//        return NULL;
//    }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
ofUniChar ofUTF8::get(ofUTF8Ptr iter, ofUTF8Ptr end) {
    return getNext(iter, end);
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::next(ofUTF8Ptr iter, ofUTF8Ptr end) {
    getNext(iter, end); return iter;
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::prior(ofUTF8Ptr iter, ofUTF8Ptr end) {
    getPrior(iter, end); return iter;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
ofUniChar ofUTF8::get(const ofUTF8String& input, ofUTF8Ptr iter) {
    return getNext(iter, endPtr(input));
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::next(const ofUTF8String& input, ofUTF8Ptr iter) {
    next(iter, endPtr(input));
    return iter;
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::prior(const ofUTF8String& input, ofUTF8Ptr iter) {
    prior(iter, endPtr(input));
    return iter;
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::advance(const ofUTF8String& input, ofUTF8Ptr iter, const int numToSkip) {
    return advance(iter, endPtr(input));
}

////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------
ofUniChar ofUTF8::get(ofUTF8Ptr iter) {
    return utf8::unchecked::next(iter); // same as peek
}
//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::next(ofUTF8Ptr iter) {
    utf8::unchecked::next(iter); // we don't return the unicode, just advance
    return iter; // iter is advanced
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::prior(ofUTF8Ptr iter) {
    utf8::unchecked::prior(iter); // we don't return the unicode, just move back
    return iter; // iter is advanced
}

//------------------------------------------------------------------
ofUTF8Ptr ofUTF8::advance(ofUTF8Ptr iter, const int numToSkip) {
    utf8::unchecked::advance(iter, numToSkip); // we don't return the unicode, just advance
    return iter; // iter is advanced
}

//------------------------------------------------------------------
ofUniChar ofUTF8::getNext(ofUTF8Ptr& iter) {
    return utf8::unchecked::next(iter); // increment iter and return value
}

//------------------------------------------------------------------
ofUniChar ofUTF8::getPrior(ofUTF8Ptr& iter) {
    return utf8::unchecked::prior(iter); // increment iter and return value
}

//------------------------------------------------------------------
// to UTF8
//------------------------------------------------------------------
ofUTF8String ofTextConverter::toUTF8(const ofUTF16String& input) {
    ofUTF8String utf8result;
//    try {
        utf8::utf16to8(input.begin(), input.end(), back_inserter(utf8result));
//    } catch (const utf8::exception& utfcpp_ex) {
//        GUI_LogError("ofTextConverter::toUTF8 : " + guiToString(utfcpp_ex.what()));
//    }

    return utf8result;
}

//------------------------------------------------------------------
ofUTF8String ofTextConverter::toUTF8(const ofUniChar& input) {
    ofUTF8String txt;
//    try {
        utf8::append(input, back_inserter(txt));
//    } catch (const utf8::exception& utfcpp_ex) {
//        string err = utfcpp_ex.what();
//        GUI_LogError("ofUTF8::append : " + err);
//    }
    return txt;
}
//------------------------------------------------------------------
ofUTF8String ofTextConverter::toUTF8(const ofUniString& input) {
    ofUTF8String utf8result;
//    try {
        utf8::utf32to8(input.begin(), input.end(), back_inserter(utf8result));
//    } catch (const utf8::exception& utfcpp_ex) {
//        GUI_LogError("ofTextConverter::toUniChar::toUniString : " + guiToString(utfcpp_ex.what()));
//    }
    return utf8result;
}

// to UTF32 / Unicode
//------------------------------------------------------------------
ofUniString ofTextConverter::toUTF32(const ofUTF8String& input) {
    ofUniString utf32result;
//    try {
        utf8::utf8to32(ofUTF8::beginPtr(input), ofUTF8::endPtr(input), back_inserter(utf32result));
//    } catch (const utf8::exception& utfcpp_ex) {
//        GUI_LogError("ofTextConverter::toUniChar::toUniString : " + guiToString(utfcpp_ex.what()));
//    }

    return utf32result;
}
//------------------------------------------------------------------
ofUniString ofTextConverter::toUTF32(const ofUTF16String& input) {
    return toUTF32(toUTF8(input));
}

//------------------------------------------------------------------
ofUniString ofTextConverter::toUnicode(const ofUTF8String& input) {
    return toUTF32(input);
}
//------------------------------------------------------------------
ofUniString ofTextConverter::toUnicode(const ofUTF16String& input) {
    return toUTF32(input);
}
