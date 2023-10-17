//
//  PesSystemUtils.cpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 19/7/2565 BE.
//

#include "PesSystemUtils.hpp"
#include <filesystem>

_Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")


//#ifdef _WIN32
//#ifdef __EMSCRIPTEN__
//#ifdef __MACOSX__
//#ifdef LINUX

using namespace std;

#ifdef _WIN32
    #include <winuser.h>
    #include <commdlg.h>
    #define _WIN32_DCOM
    #include <windows.h>
    #include <shlobj.h>
    #include <tchar.h>
    #include <stdio.h>
#endif

#ifdef __APPLE_CC__ //__MACOSX__
    #include <Cocoa/Cocoa.h>
#endif

#ifdef _WIN32
#include <locale>
#include <sstream>
#include <string>

std::string convertWideToNarrow( const wchar_t *s, char dfault = '?',
                      const std::locale& loc = std::locale() )
{
  std::ostringstream stm;

  while( *s != L'\0' ) {
    stm << std::use_facet< std::ctype<wchar_t> >( loc ).narrow( *s++, dfault );
  }
  return stm.str();
}

std::wstring convertNarrowToWide( const std::string& as ){
    // deal with trivial case of empty string
    if( as.empty() )    return std::wstring();

    // determine required length of new string
    size_t reqLength = ::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), (int)as.length(), 0, 0 );

    // construct new string of required length
    std::wstring ret( reqLength, L'\0' );

    // convert old string to new string
    ::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), (int)as.length(), &ret[0], (int)ret.length() );

    // return new string ( compiler should optimize this away )
    return ret;
}
#endif

#ifdef __APPLE_CC__ //__MACOSX__
static void restoreAppWindowFocus(){
//    NSWindow * appWindow = (NSWindow *)ofGetCocoaWindow();
//    if(appWindow) {
//        [appWindow makeKeyAndOrderFront:nil];
//    }
}
#endif

PesFileDialogResult::PesFileDialogResult(){
    filePath = "";
    fileName = "";
    bSuccess = false;
}

//------------------------------------------------------------------------------
string PesFileDialogResult::getName(){
    return fileName;
}

//------------------------------------------------------------------------------
string PesFileDialogResult::getPath(){
    return filePath;
}

PesFileDialogResult PesSystemSaveDialog(string defaultName, string messageName){

    PesFileDialogResult results;

    //----------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------       OSX
    //----------------------------------------------------------------------------------------
#ifdef __APPLE_CC__ //__MACOSX__
    @autoreleasepool {
        NSSavePanel * saveDialog = [NSSavePanel savePanel];
        NSOpenGLContext *context = [NSOpenGLContext currentContext];
        [saveDialog setMessage:[NSString stringWithUTF8String:messageName.c_str()]];
        [saveDialog setNameFieldStringValue:[NSString stringWithUTF8String:defaultName.c_str()]];

        NSInteger buttonClicked = [saveDialog runModal];
        restoreAppWindowFocus();
        [context makeCurrentContext];

        if(buttonClicked == NSModalResponseOK){
            results.filePath = string([[[saveDialog URL] path] UTF8String]);
        }
    }
#endif

    
#ifdef _WIN32
    wchar_t fileName[MAX_PATH] = L"";
    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    HWND hwnd = WindowFromDC(wglGetCurrentDC());
    ofn.hwndOwner = hwnd;
    ofn.hInstance = GetModuleHandle(0);
    ofn.nMaxFileTitle = 31;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
    ofn.lpstrDefExt = L"";    // we could do .rxml here?
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
    ofn.lpstrTitle = L"Select Output File";

    if (GetSaveFileNameW(&ofn)){
        results.filePath = convertWideToNarrow(fileName);
    }

#endif

    if( results.filePath.length() > 0 ){
        results.bSuccess = true;
//        results.fileName = PesFilePath::getFileName(results.filePath);
        results.fileName = std::filesystem::path(results.filePath).filename().string();
    }

    return results;
}

#ifdef _WIN32
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //switch(msg)
    //{
    //    case WM_CLOSE:
    //        DestroyWindow(hwnd);
    //    break;
    //    case WM_DESTROY:
    //        PostQuitMessage(0);
    //    break;
    //    default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    //}
}
#endif
