//
//  PesSystemUtils.cpp
//  sources
//
//  Created by SIHARAT THAMMAYA on 19/7/2565 BE.
//

#include "PesSystemUtils.hpp"
#include <filesystem>
#include <functional>
#include <stdio.h>
#include <assert.h>
#include <string.h>

_Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")


//#include "include/core/SkTypes.h"

std::string initdir = "";
std::string lastdir = "";


#if defined(SK_BUILD_FOR_MAC)  //__MACOSX__
#include <Cocoa/Cocoa.h>
// init
const bool isInit = []() -> bool {
    initdir = std::filesystem::current_path().string();
    lastdir = initdir;
    return true;
}();
#endif

#ifdef _WIN32
#include <windows.h>
#include <winuser.h>
#include <commdlg.h>
#pragma comment(lib, "Comdlg32")

#include <shlobj_core.h>
#pragma comment(lib, "Shell32")
#include <shlobj.h>
#pragma comment(lib, "Ole32")


// lpstrFile:
// If lpstrFile contains a path, that path is the initial directory.
// Otherwise, lpstrInitialDir specifies the initial directory.
// Otherwise, if the application has used an Open or Save As dialog box in the past,
// the path most recently used is selected as the initial directory.
// However, if an application is not run for a long time, its saved selected path is discarded.
//
// lpstrInitialDir:
// If lpstrInitialDir is NULL and the current directory contains any files
// of the specified filter types, the initial directory is the current directory.
// Otherwise, the initial directory is the personal files directory of the current user.
// Otherwise, the initial directory is the Desktop folder.

// init
const bool isInit = []() -> bool {
    initdir = ""; //std::filesystem::current_path().string(); 
    lastdir = initdir;
    return true;
}();

#endif

#ifdef __EMSCRIPTEN__
const bool isInit = false;
#endif

#ifdef __linux__
#include <gtk/gtk.h>

// init
const bool isInit = []() -> bool {
    initdir = g_get_home_dir();
    lastdir = initdir;
    return true;
}();

static void WaitForCleanup(void) {
    while (gtk_events_pending())
        gtk_main_iteration();
}
#endif

PesFileDialogResult::PesFileDialogResult(){
    filePath = "";
    fileName = "";
    bSuccess = false;
}

//------------------------------------------------------------------------------
std::string PesFileDialogResult::getName() {
    return fileName;
}

//------------------------------------------------------------------------------
std::string PesFileDialogResult::getPath() {
    return filePath;
}

PesFileDialogResult PesSystemSaveDialog(std::string defaultName, std::string messageName) {
    std::filesystem::path cwd = std::filesystem::current_path();

    PesFileDialogResult results;
    if (!std::filesystem::exists(lastdir)) {
        lastdir = initdir;
    }

    //backward compat
    std::string defaultExt;
    int nFilterIndex;
    if (defaultName.empty()) {
        defaultName = "character.pes";
        defaultExt = ".pes";
        nFilterIndex = 1;
    }

    if (defaultExt.empty()) {
        size_t length = defaultName.length();
        size_t pos;
        if (std::string::npos != (pos = defaultName.rfind(".pes", length-4, 4))) {
            defaultExt = ".pes";
            nFilterIndex = 1;
        } else if (std::string::npos != (pos = defaultName.rfind(".ppes", length-5, 5))) {
            defaultExt = ".ppes";
            nFilterIndex = 2;
        } else {
            defaultExt = "";
            nFilterIndex = 3;
        }
        //SkDebugf("defaultName:%s, defaultExt:%s, pos:%zu\n", defaultName.c_str(), defaultExt.c_str(), pos);
    }


    if (messageName.empty()) {
        messageName = "Export PES";
    }

    char fileName[4096];
    strcpy(fileName, defaultName.c_str());
    const char *currentFolder = lastdir.c_str();

    

#if defined(SK_BUILD_FOR_MAC)  //__MACOSX__
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
    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    HWND hwnd = WindowFromDC(wglGetCurrentDC());
    ofn.hwndOwner = hwnd;
    ofn.hInstance = GetModuleHandle(0);
    ofn.nMaxFileTitle = 31;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = currentFolder;
    ofn.lpstrFilter = "Embroidery designs files (*.pes)\0*.pes\0Embroidery designs project files (*.ppes)\0*.ppes\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = nFilterIndex;
    ofn.lpstrDefExt = defaultExt.c_str();
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
    ofn.lpstrTitle = messageName.c_str();
    
    if (GetSaveFileName(&ofn)){
        results.filePath = fileName;
    }

#endif

#ifdef __EMSCRIPTEN__
#endif

#ifdef __linux__
    if ( !gtk_init_check( NULL, NULL ) ) {
        return results;
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new( "Save File",
                                          NULL,
                                          GTK_FILE_CHOOSER_ACTION_SAVE,
                                          "_Cancel", GTK_RESPONSE_CANCEL,
                                          "_Save", GTK_RESPONSE_ACCEPT,
                                          NULL );
    
    gtk_file_chooser_set_do_overwrite_confirmation( GTK_FILE_CHOOSER(dialog), TRUE );

    // file filter    
    GtkFileFilter *pes_filter = gtk_file_filter_new();
    gtk_file_filter_set_name( pes_filter, "Embroidery designs files (*.pes)" );
    gtk_file_filter_add_pattern( pes_filter, "*.pes" );
    gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(dialog), pes_filter );

    GtkFileFilter *all_filter = gtk_file_filter_new();
    gtk_file_filter_set_name( all_filter, "All files (*.*)" );
    gtk_file_filter_add_pattern( all_filter, "*" );
    gtk_file_chooser_add_filter( GTK_FILE_CHOOSER(dialog), all_filter );
    
    if (fileName[0] != 0) {
        gtk_file_chooser_set_current_name ( GTK_FILE_CHOOSER(dialog), fileName) ;
    }

    // set the default path
    // GTK+ manual recommends not specifically setting the default path.
    // We do it anyway in order to be consistent across platforms.
    // If consistency with the native OS is preferred, this is the line
    // to comment out.
    if (currentFolder[0] != 0) {
        gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER(dialog), currentFolder );
    }    
    
    if ( gtk_dialog_run( GTK_DIALOG(dialog) ) == GTK_RESPONSE_ACCEPT ) {
        char *fileName = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );   
        results.filePath = fileName;
        g_free(fileName);
    }
    
    WaitForCleanup();
    gtk_widget_destroy(dialog);
    WaitForCleanup();    

#endif

    if( results.filePath.length() > 0 ){
        results.bSuccess = true;

        std::filesystem::path _path = std::filesystem::path(results.filePath);
        //SkDebugf("_path: %s\n", _path.string().c_str());
        if (_path.has_parent_path()) {
            lastdir = _path.parent_path().string();
            //SkDebugf("str_path_dir: '%s'\n", lastdir.c_str());
        }        
        results.fileName = _path.filename().string();
    }

    std::filesystem::current_path(cwd);
    return results;
}

// should be?
bool PesSystemReallyClose(bool fileChanged, const char* currentShortname, std::function<void(const char* currentShortname)> onsave) {

#if defined(SK_BUILD_FOR_MAC)  //__MACOSX__
    return true;
#endif

#ifdef _WIN32
    return true;
#endif

#ifdef __EMSCRIPTEN__
    return true;
#endif

#ifdef __linux__
    if (fileChanged) {
        GtkWidget *dialog;
        gint response;
        dialog = gtk_message_dialog_new (NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_NONE,
            "Save the changes to '%s' before closing?", 
            currentShortname);

        gtk_dialog_add_buttons(GTK_DIALOG(dialog), 
            "Don't Save", GTK_RESPONSE_REJECT,
            "Cancel", GTK_RESPONSE_CANCEL,
            "Save", GTK_RESPONSE_ACCEPT,
            NULL);
                        
        //gtk_window_set_title(GTK_WINDOW(dialog), "There are unsaved changes.");
        
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);

        if (response == GTK_RESPONSE_CANCEL || response == GTK_RESPONSE_NONE) {
            return true;
        }
        if (response == GTK_RESPONSE_ACCEPT) {
            if(onsave)
                onsave(currentShortname);
        }
    }
    return true;
#endif
}

PesFileDialogResult PesSystemLoadDialog(std::string windowTitle, bool bFolderSelection, std::string defaultPath,
                                        std::vector<std::string> allowExtensions){
    std::filesystem::path cwd = std::filesystem::current_path();

    PesFileDialogResult results;

    lastdir = defaultPath;

    std::string defaultExt;
    int nFilterIndex;
    if (windowTitle.empty()) {
        windowTitle = "Load PPES file";
        defaultExt = ".ppes";
        nFilterIndex = 2;
    }

    if (defaultExt.empty()) {
        size_t length = windowTitle.length();
        size_t pos;
        if (std::string::npos != (pos = windowTitle.find("PPES"))) {
            defaultExt = ".ppes";
            nFilterIndex = 2;
        } else if (std::string::npos != (pos = windowTitle.find("PES"))) {
            defaultExt = ".pes";
            nFilterIndex = 1;
        } else {
            defaultExt = "";
            nFilterIndex = 3;
        }
        //SkDebugf("windowTitle:%s, defaultExt:%s, pos:%zu\n", windowTitle.c_str(), defaultExt.c_str(), pos);
    }

    char fileName[4096] = "";
    const char* currentFolder = lastdir.c_str();

#if defined(SK_BUILD_FOR_MAC)  //__MACOSX__
    @autoreleasepool {
        NSOpenGLContext *context = [NSOpenGLContext currentContext];

        NSOpenPanel * loadDialog = [NSOpenPanel openPanel];
        [loadDialog setAllowsMultipleSelection:NO];
        [loadDialog setCanChooseDirectories:bFolderSelection];
        [loadDialog setCanChooseFiles:!bFolderSelection];
        [loadDialog setResolvesAliases:YES];

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
    if (bFolderSelection == false){
        OPENFILENAME ofn;
        memset(&ofn, 0, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        HWND hwnd = WindowFromDC(wglGetCurrentDC());
        ofn.hwndOwner = hwnd;
        ofn.hInstance = GetModuleHandle(0);
        ofn.nMaxFileTitle = 31;
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrInitialDir = currentFolder;
        //ofn.lpstrFilter = "All\0";
        ofn.lpstrFilter = "Embroidery designs files (*.pes)\0*.pes\0Embroidery designs project files (*.ppes)\0*.ppes\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = nFilterIndex;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = 0;
        ofn.lpstrTitle = windowTitle.c_str();

        if(GetOpenFileName(&ofn)) {
            results.filePath = fileName;
        }
        else {
            //this should throw an error on failure unless its just the user canceling out
            DWORD err = CommDlgExtendedError();
        }

    } else {

        BROWSEINFOA      bi;
        memset(&bi, 0, sizeof(bi));

        char szBuffer[MAX_PATH];
        szBuffer[0] = 0;
        LPITEMIDLIST    pidl;
        LPMALLOC        lpMalloc;

        if (windowTitle == "") {
            windowTitle = "Select Directory";
        }

        // Get a pointer to the shell memory allocator
        if(SHGetMalloc(&lpMalloc) != S_OK){
            //TODO: deal with some sort of error here?
        }
        bi.hwndOwner        =   WindowFromDC(wglGetCurrentDC());
        bi.pidlRoot         =   nullptr;
        bi.pszDisplayName   =   szBuffer;
        bi.lpszTitle        =   windowTitle.c_str();
        bi.ulFlags          =   BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
        //bi.lpfn             =   &loadDialogBrowseCallback;
        //bi.lParam           =   (LPARAM) &defaultPath;

        if(pidl = SHBrowseForFolderA(&bi)){
            // Copy the path directory to the buffer
            if (SHGetPathFromIDListA(pidl, szBuffer)) {
                results.filePath = szBuffer;
            }
            lpMalloc->Free(pidl);
        }
        lpMalloc->Release();
    }
#endif

#ifdef __EMSCRIPTEN__
#endif

#ifdef __linux__
    if (!gtk_init_check(NULL, NULL)) {
        return results;
    }

    GtkWidget* dialog = gtk_file_chooser_dialog_new(windowTitle.c_str(),
                                                    NULL,
                                                    bFolderSelection? GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER : GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel",
                                                    GTK_RESPONSE_CANCEL,
                                                    "_Open",
                                                    GTK_RESPONSE_ACCEPT,
                                                    NULL);

    //gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    // file filter
    if(nFilterIndex == 1) {
        GtkFileFilter* pes_filter = gtk_file_filter_new();
        gtk_file_filter_set_name(pes_filter, "Embroidery designs files (*.pes)");
        gtk_file_filter_add_pattern(pes_filter, "*.pes");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), pes_filter);
    }


    if(nFilterIndex == 2) {
        GtkFileFilter* ppes_filter = gtk_file_filter_new();
        gtk_file_filter_set_name(ppes_filter, "Embroidery designs project files (*.ppes)");
        gtk_file_filter_add_pattern(ppes_filter, "*.ppes");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), ppes_filter);
    }


    GtkFileFilter* all_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(all_filter, "All files (*.*)");
    gtk_file_filter_add_pattern(all_filter, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), all_filter);

    //if (fileName[0] != 0) {
    //    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), fileName);
    //}

    // set the default path
    // GTK+ manual recommends not specifically setting the default path.
    // We do it anyway in order to be consistent across platforms.
    // If consistency with the native OS is preferred, this is the line
    // to comment out.
    if (currentFolder[0] != 0) {
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), currentFolder);
    }

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* fileName = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        results.filePath = fileName;
        g_free(fileName);
    }

    WaitForCleanup();
    gtk_widget_destroy(dialog);
    WaitForCleanup();

#endif

    if( results.filePath.length() > 0 ){
        results.bSuccess = true;
        results.fileName = std::filesystem::path(results.filePath).filename().string();
    }

    std::filesystem::current_path(cwd);
    return results;
}


