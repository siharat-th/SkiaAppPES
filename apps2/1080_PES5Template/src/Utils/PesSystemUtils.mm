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

PesFileDialogResult PesSystemLoadDialog(string windowTitle, bool bFolderSelection, string defaultPath,
                                        std::vector<std::string> allowExtensions){
    PesFileDialogResult results;

#ifdef __APPLE_CC__ //__MACOSX__
    @autoreleasepool {
        NSOpenGLContext *context = [NSOpenGLContext currentContext];

        NSOpenPanel * loadDialog = [NSOpenPanel openPanel];
        [loadDialog setAllowsMultipleSelection:NO];
        [loadDialog setCanChooseDirectories:bFolderSelection];
        [loadDialog setCanChooseFiles:!bFolderSelection];
        [loadDialog setResolvesAliases:YES];
        
        if(allowExtensions.empty()==false){ // pom
            NSMutableArray *array = [NSMutableArray arrayWithCapacity:1];
            for(int i=0; i<allowExtensions.size(); i++){
                [array addObject: [NSString stringWithUTF8String:allowExtensions[i].c_str()]];
            }
            
            // NSArray* fileTypes = [NSArray arrayWithObjects:@"pes", @"ppes", @"pinn", @"svg", @"png", @"jpg", @"bmp", nil];
            NSArray* fileTypes = [array copy];
            [loadDialog setAllowedFileTypes: fileTypes];
        }

        if(!windowTitle.empty()) {
            // changed from setTitle to setMessage
            // https://stackoverflow.com/questions/36879212/title-bar-missing-in-nsopenpanel
            [loadDialog setMessage:[NSString stringWithUTF8String:windowTitle.c_str()]];
        }

        if(!defaultPath.empty()) {
            NSString * s = [NSString stringWithUTF8String:defaultPath.c_str()];
            s = [[s stringByExpandingTildeInPath] stringByResolvingSymlinksInPath];
            NSURL * defaultPathUrl = [NSURL fileURLWithPath:s];
            [loadDialog setDirectoryURL:defaultPathUrl];
        }

        NSInteger buttonClicked = [loadDialog runModal];
        [context makeCurrentContext];
        restoreAppWindowFocus();

        if(buttonClicked == NSModalResponseOK) {
            NSURL * selectedFileURL = [[loadDialog URLs] objectAtIndex:0];
            results.filePath = string([[selectedFileURL path] UTF8String]);
        }
    }
#endif

    
#ifdef _WIN32
    wstring windowTitleW;
    windowTitleW.assign(windowTitle.begin(), windowTitle.end());

    if (bFolderSelection == false){

        OPENFILENAME ofn;

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        HWND hwnd = WindowFromDC(wglGetCurrentDC());
        ofn.hwndOwner = hwnd;

        //the file name and path
        wchar_t szFileName[MAX_PATH];
        memset(szFileName, 0, sizeof(szFileName));

        //the dir, if specified
        wchar_t szDir[MAX_PATH];

        //the title if specified
        wchar_t szTitle[MAX_PATH];
        if(defaultPath!=""){
//            wcscpy(szDir,convertNarrowToWide(ofToDataPath(defaultPath)).c_str());
            wcscpy(szDir,convertNarrowToWide(defaultPath).c_str());
            ofn.lpstrInitialDir = szDir;
        }

        if (windowTitle != "") {
            wcscpy(szTitle, convertNarrowToWide(windowTitle).c_str());
            ofn.lpstrTitle = szTitle;
        } else {
            ofn.lpstrTitle = nullptr;
        }

        ofn.lpstrFilter = L"All\0";
        ofn.lpstrFile = szFileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = 0;
        ofn.lpstrTitle = windowTitleW.c_str();

        if(GetOpenFileName(&ofn)) {
            results.filePath = convertWideToNarrow(szFileName);
        }
        else {
            //this should throw an error on failure unless its just the user canceling out
            DWORD err = CommDlgExtendedError();
        }

    } else {

        BROWSEINFOW      bi;
        wchar_t         wideCharacterBuffer[MAX_PATH];
        wchar_t            wideWindowTitle[MAX_PATH];
        LPITEMIDLIST    pidl;
        LPMALLOC        lpMalloc;

        if (windowTitle != "") {
            wcscpy(wideWindowTitle, convertNarrowToWide(windowTitle).c_str());
        } else {
            wcscpy(wideWindowTitle, L"Select Directory");
        }

        // Get a pointer to the shell memory allocator
        if(SHGetMalloc(&lpMalloc) != S_OK){
            //TODO: deal with some sort of error here?
        }
        bi.hwndOwner        =   nullptr;
        bi.pidlRoot         =   nullptr;
        bi.pszDisplayName   =   wideCharacterBuffer;
        bi.lpszTitle        =   wideWindowTitle;
        bi.ulFlags          =   BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
        bi.lpfn             =   &loadDialogBrowseCallback;
        bi.lParam           =   (LPARAM) &defaultPath;
        bi.lpszTitle        =   windowTitleW.c_str();

        if(pidl = SHBrowseForFolderW(&bi)){
            // Copy the path directory to the buffer
            if(SHGetPathFromIDListW(pidl,wideCharacterBuffer)){
                results.filePath = convertWideToNarrow(wideCharacterBuffer);
            }
            lpMalloc->Free(pidl);
        }
        lpMalloc->Release();
    }
#endif

    if( results.filePath.length() > 0 ){
        results.bSuccess = true;
        results.fileName = std::filesystem::path(results.filePath).filename().string();
    }

    return results;
}
