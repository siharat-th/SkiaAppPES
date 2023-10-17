//
//  GUI_Log.cpp
//  Pukchue2
//
//  Created by SIHARAT THAMMAYA on 11/4/2559 BE.
//  Copyright © 2559 Jimmy Software Co., Ltd. All rights reserved.
//

#include "GUI_Log.h"
#ifdef __ANDROID__
#include <android/log.h>
#endif

static GUI_LogLevel currentLogLevel = GUI_LogLevel::GUI_LOG_NOTICE;

bool GUI_Log::bAutoSpace = false;
string GUI_Log::padding = "";

static map<string, GUI_LogLevel> & getModules() {
    static map<string, GUI_LogLevel> * modules = new map<string, GUI_LogLevel>;
    return *modules;
}

//--------------------------------------------------
void GUI_SetLogLevel(GUI_LogLevel level) {
    currentLogLevel = level;
}

//--------------------------------------------------
void GUI_SetLogLevel(string module, GUI_LogLevel level) {
    getModules()[module] = level;
}

//--------------------------------------------------
GUI_LogLevel GUI_GetLogLevel() {
    return currentLogLevel;
}

//--------------------------------------------------
GUI_Log::GUI_Log() {
    level = GUI_LogLevel::GUI_LOG_NOTICE;
    module = "";
    bPrinted = false;
}

//--------------------------------------------------
GUI_Log::GUI_Log(GUI_LogLevel _level) {
    level = _level;
    module = "";
    bPrinted = false;
}

//--------------------------------------------------
GUI_Log::GUI_Log(GUI_LogLevel level, const string & message) {
    _log(level, "", message);
    bPrinted = true;
}

//--------------------------------------------------
GUI_Log::GUI_Log(GUI_LogLevel level, const char* format, ...) {
    if (checkLog(level, "")) {
        va_list args;
        va_start(args, format);
        log(level, "", format, args);
        va_end(args);
    }

    bPrinted = true;
}

//--------------------------------------------------
void GUI_Log::setAutoSpace(bool autoSpace) {
    bAutoSpace = autoSpace;
    
    if (bAutoSpace) {
        padding = " ";
    } else {
        padding = "";
    }
}

//-------------------------------------------------------
GUI_Log::~GUI_Log() {
    // don't log if we printed in the constructor already
    if (!bPrinted) {
        _log(level, module, message.str());
    }
}

bool GUI_Log::checkLog(GUI_LogLevel level, const string & module) {
    if (getModules().find(module) == getModules().end()) {
        if (level >= currentLogLevel) return true;
    } else {
        if (level >= getModules()[module]) return true;
    }
    
    return false;
}

//-------------------------------------------------------
void GUI_Log::_log(GUI_LogLevel level, const string & module, const string & message) {
    if (checkLog(level, module)) {
        log(level, module, message);
    }
}

//--------------------------------------------------
GUI_LogVerbose::GUI_LogVerbose(const string & _module) {
    level = GUI_LogLevel::GUI_LOG_VERBOSE;
    module = _module;
    bPrinted = false;
}

GUI_LogVerbose::GUI_LogVerbose(const string & _module, const string & _message) {
    log(GUI_LogLevel::GUI_LOG_VERBOSE, _module, _message);
    bPrinted = true;
}

GUI_LogVerbose::GUI_LogVerbose(const string & module, const char* format, ...) {
    if (checkLog(GUI_LogLevel::GUI_LOG_VERBOSE, module)) {
        va_list args;
        va_start(args, format);
        log(GUI_LogLevel::GUI_LOG_VERBOSE, module, format, args);
        va_end(args);
    }
    
    bPrinted = true;
}

//--------------------------------------------------
GUI_LogNotice::GUI_LogNotice(const string & _module) {
    level = GUI_LogLevel::GUI_LOG_NOTICE;
    module = _module;
    bPrinted = false;
}

GUI_LogNotice::GUI_LogNotice(const string & _module, const string & _message) {
    log(GUI_LogLevel::GUI_LOG_NOTICE, _module, _message);
    bPrinted = true;
}

GUI_LogNotice::GUI_LogNotice(const string & module, const char* format, ...) {
    if (checkLog(GUI_LogLevel::GUI_LOG_NOTICE, module)) {
        va_list args;
        va_start(args, format);
        log(GUI_LogLevel::GUI_LOG_NOTICE, module, format, args);
        va_end(args);
    }
    
    bPrinted = true;
}

//--------------------------------------------------
GUI_LogWarning::GUI_LogWarning(const string & _module) {
    level = GUI_LogLevel::GUI_LOG_WARNING;
    module = _module;
    bPrinted = false;
}

GUI_LogWarning::GUI_LogWarning(const string & _module, const string & _message) {
    log(GUI_LogLevel::GUI_LOG_WARNING, _module, _message);
    bPrinted = true;
}

GUI_LogWarning::GUI_LogWarning(const string & module, const char* format, ...) {
    if (checkLog(GUI_LogLevel::GUI_LOG_WARNING, module)) {
        va_list args;
        va_start(args, format);
        log(GUI_LogLevel::GUI_LOG_WARNING, module, format, args);
        va_end(args);
    }
    
    bPrinted = true;
}

//--------------------------------------------------
GUI_LogError::GUI_LogError(const string & _module) {
    level = GUI_LogLevel::GUI_LOG_ERROR;
    module = _module;
    bPrinted = false;
}

GUI_LogError::GUI_LogError(const string & _module, const string & _message) {
    log(GUI_LogLevel::GUI_LOG_ERROR, _module, _message);
    bPrinted = true;
}

GUI_LogError::GUI_LogError(const string & module, const char* format, ...) {
    if (checkLog(GUI_LogLevel::GUI_LOG_ERROR, module)) {
        va_list args;
        va_start(args, format);
        log(GUI_LogLevel::GUI_LOG_ERROR, module, format, args);
        va_end(args);
    }
    
    bPrinted = true;
}

//--------------------------------------------------
GUI_LogFatalError::GUI_LogFatalError(const string & _module) {
    level = GUI_LogLevel::GUI_LOG_FATAL_ERROR;
    module = _module;
    bPrinted = false;
}

GUI_LogFatalError::GUI_LogFatalError(const string & _module, const string & _message) {
    log(GUI_LogLevel::GUI_LOG_FATAL_ERROR, _module, _message);
    bPrinted = true;
}

GUI_LogFatalError::GUI_LogFatalError(const string & module, const char* format, ...) {
    if (checkLog(GUI_LogLevel::GUI_LOG_ERROR, module)) {
        va_list args;
        va_start(args, format);
        log(GUI_LogLevel::GUI_LOG_FATAL_ERROR, module, format, args);
        va_end(args);
    }
    
    bPrinted = true;
}

//--------------------------------------------------
void log(GUI_LogLevel level, const string & module, const string & message) {
    // print to cerr for OF_LOG_ERROR and OF_LOG_FATAL_ERROR, everything else to cout
    ostream& out = level < GUI_LogLevel::GUI_LOG_ERROR ? cout : cerr;
    // only print the module name if it's not ""
    if (module != "") {
        out << module << ": ";
    }
    
    out << message << endl;
}

void log(GUI_LogLevel level, const string & module, const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(level, module, format, args);
    va_end(args);
}

void log(GUI_LogLevel level, const string & module, const char* format, va_list args) {
#ifdef __ANDROID__
    return;
    //if(GUI_GetLogLevel()<=GUI_LOG_NOTICE)
    {
        char buffer[512];
        vsprintf(buffer, format, args);

        __android_log_print(ANDROID_LOG_VERBOSE, "__GUI__", buffer, 1);
    }
#endif

    FILE* out = level < GUI_LogLevel::GUI_LOG_ERROR ? stdout : stderr;

    if (module != "") {
        fprintf(out, "%s: ", module.c_str());
    }

    vfprintf(out, format, args);
    fprintf(out, "\n");
}
