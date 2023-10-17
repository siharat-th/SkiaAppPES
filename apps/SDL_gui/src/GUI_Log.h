//
//  GUI_Log.h
//  Pukchue2
//
//  Created by SIHARAT THAMMAYA on 11/4/2559 BE.
//  Copyright © 2559 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef GUI_Log_h
#define GUI_Log_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// core: ---------------------------
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>  //for ostringsream
#include <iomanip>  //for setprecision
#include <fstream>
#include <algorithm>
#include <cfloat>
#include <map>
#include <stack>
#include <unordered_map>
#include <memory>

#if !defined(_MSC_VER)
using namespace std;
#else
using std::basic_string;
using std::string;
using std::wstring;

using std::ios;
using std::streambuf;
using std::istream;
using std::ostream;
using std::iostream;
using std::stringbuf;
using std::istringstream;
using std::ostringstream;
using std::stringstream;
using std::filebuf;
using std::ifstream;
using std::ofstream;
using std::fstream;

using std::uppercase;
using std::nouppercase;
using std::setfill;
using std::fixed;
using std::setprecision;

using std::cout;
using std::cerr;
using std::endl;

using std::vector;
using std::list;
using std::deque;
using std::map;
using std::pair;
using std::shared_ptr;
using std::make_shared;

using std::max;
using std::min;
using std::numeric_limits;

using std::setw;
using std::hex;
#endif

//pom

/// \cond INTERNAL
/// printf annotations for automatic format checking in GCC.
#ifdef __GNUC__
#define OF_PRINTF_ATTR(x, y) __attribute__ ((format (printf, x, y)))
#else
#define OF_PRINTF_ATTR(x, y)
#endif
/// \endcond

enum class GUI_LogLevel {
    GUI_LOG_VERBOSE = 0,
    GUI_LOG_NOTICE,
    GUI_LOG_WARNING,
    GUI_LOG_ERROR,
    GUI_LOG_FATAL_ERROR,
    GUI_LOG_SILENT
};

void GUI_SetLogLevel(GUI_LogLevel level);
void GUI_SetLogLevel(std::string module, GUI_LogLevel level);
GUI_LogLevel GUI_GetLogLevel();

class GUI_Log {
public:
    GUI_Log();
    GUI_Log(GUI_LogLevel level);
    GUI_Log(GUI_LogLevel level, const std::string & message);
    GUI_Log(GUI_LogLevel level, const char* format, ...) OF_PRINTF_ATTR(3, 4);

    static void setAutoSpace(bool autoSpace);
    virtual ~GUI_Log();

    template <class T>
    GUI_Log& operator<<(const T& value) {
        message << value << padding;
        return *this;
    }

    GUI_Log& operator<<(std::ostream& (*func)(std::ostream&)) {
        func(message);
        return *this;
    }

protected:
    GUI_LogLevel level;
    bool bPrinted;
    std::string module;
    void _log(GUI_LogLevel level, const std::string & module, const std::string & message);
    bool checkLog(GUI_LogLevel level, const std::string & module);

private:
    std::ostringstream message;
    static bool bAutoSpace;

    GUI_Log(GUI_Log const&) : 
        bPrinted(false), 
        level(GUI_LogLevel::GUI_LOG_VERBOSE) {
    }

    GUI_Log& operator=(GUI_Log& from) {
        return *this;
    }

    static std::string padding; ///< The padding between std::ostream calls.
};

class GUI_LogVerbose : public GUI_Log {
public:
    GUI_LogVerbose(const string &module = "");
    GUI_LogVerbose(const string & module, const string & message);
    GUI_LogVerbose(const string & module, const char* format, ...) OF_PRINTF_ATTR(3, 4);
};

class GUI_LogNotice : public GUI_Log {
public:
    GUI_LogNotice(const string &module = "");
    GUI_LogNotice(const string & module, const string & message);
    GUI_LogNotice(const string & module, const char* format, ...) OF_PRINTF_ATTR(3, 4);
};

class GUI_LogWarning : public GUI_Log {
public:
    GUI_LogWarning(const string &module = "");
    GUI_LogWarning(const string & module, const string & message);
    GUI_LogWarning(const string & module, const char* format, ...) OF_PRINTF_ATTR(3, 4);
};

class GUI_LogError : public GUI_Log {
public:
    GUI_LogError(const string &module = "");
    GUI_LogError(const string & module, const string & message);
    GUI_LogError(const string & module, const char* format, ...) OF_PRINTF_ATTR(3, 4);
};

class GUI_LogFatalError : public GUI_Log {
public:
    GUI_LogFatalError(const string &module = "");
    GUI_LogFatalError(const string & module, const string & message);
    GUI_LogFatalError(const string & module, const char* format, ...) OF_PRINTF_ATTR(3, 4);
};

void log(GUI_LogLevel level, const string & module, const string & message);

void log(GUI_LogLevel level, const string & module, const char* format, ...);

void log(GUI_LogLevel level, const string & module, const char* format, va_list args);

#endif /* GUI_Log_h */
