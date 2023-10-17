//
//  GUI_FileDialog.cpp
//  OSX
//
//  Created by SIHARAT THAMMAYA on 9/4/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#include "GUI_FileDialog.h"
#include "GUI_Utils.h"
#include "jsFileUtils.h"

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef _WIN32
#include <direct.h>  // for _chdir(), _getcwd()
#define GetCurrentDir _getcwd
#define	chdir _chdir // pom
#include <windows.h>
#include <iostream>
#include <shlobj.h>
#else
#include <unistd.h>
#include <dirent.h> // for DIR, dirent, opendir()
#include <pwd.h>
#define GetCurrentDir getcwd
#endif

#if __IPHONEOS__
extern char __iOS_DOCUMENTS_FOLDER[512];
#endif

static char rootDir[] = "/";

extern char _docDir[512];
const char *PES_getDocumentDirectory() {
#if __IPHONEOS__
    strcpy(_docDir, __iOS_DOCUMENTS_FOLDER);
#elif __ANDROID__
    strcpy(_docDir, "/mnt/sdcard/Download");
#elif __EMSCRIPTEN__
    getcwd(_docDir, 512);
    sprintf(_docDir, "%sdata", _docDir);
#elif defined(__MACOSX__) || defined(__linux__)
	struct passwd *pwx = getpwuid(getuid());

	const char *homedir = pwx->pw_dir;

	strcpy(_docDir, homedir);
	strcat(_docDir, "/Documents");
#elif _WIN32
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, _docDir);
	if (result != S_OK)
		getcwd(_docDir, 512);
#else
    getcwd(_docDir, 512);
#endif

    return _docDir;
}

void GUI_FileDialog::setDirectory(std::string customPath) {
    dir.allowExt(fileExtension);
    if (!strcmp(fileExtension, "pen")) {
        dir.allowExt("pes");
    } 
    else if (!strcmp( fileExtension, "ppes")) {
        dir.allowExt("ppes");
//        dir.allowExt("pjob");
    }

    if (customPath.empty() == false) {
        dir.listDir(customPath);
        return;
    }

#ifdef __EMSCRIPTEN__
    dir.listDir(PES_getDocumentDirectory());
#elif __IPHONEOS__
    dir.listDir(PES_getDocumentDirectory());
#elif __ANDROID__
    dir.listDir(PES_getDocumentDirectory());
#elif __MACOSX__
    dir.listDir(PES_getDocumentDirectory());
#elif _WIN32
    dir.listDir(PES_getDocumentDirectory());
#elif __linux__
    dir.listDir(PES_getDocumentDirectory());
#else
        string cwd = GUI_GetCurrentPath();
        
        if (cwd.length() < 2) {
            cwd = jsFilePath::getUserHomeDir();
            
            if (cwd.length() < 2) {
                cwd = PES_getDocumentDirectory();
            }
        }
        
        string absolutePath = jsFilePath::getAbsolutePath(cwd);
        
        if (absolutePath.length() < 2) {
        char msg[512];
            sprintf(msg, "cwd=%s\nabs=%s\nhome=%s\ndoc=%s", cwd.c_str(), absolutePath.c_str(), jsFilePath::getUserHomeDir().c_str(), PES_getDocumentDirectory());
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Path ERROR", msg, GUI_window);
        }
        
        dir.listDir(absolutePath);
#endif
}

GUI_FileDialog::GUI_FileDialog(GUI_View *pw, bool bSave, const char *ext, const char *defaultFN, bool(*cmd)(const char *)) :
    GUI_View(pw, "", 0, 0, 480, 360),
    dirButton(NULL),
    menuDir(NULL),
    dirListBox(NULL),
    fileListBox(NULL),
    fileEdit(NULL),
    bttnOK(NULL),
    bSaveDlg(bSave),
    file_cmd(cmd),
    fileExtension(ext),
    defaultFilename(defaultFN),
    bttnDelete(NULL) {
        
    setDirectory();

    setAlignment(GUI_ALIGN_ABSOLUTE);
    setLayout(GUI_LAYOUT_VERTICAL);

    topBar = new GUI_ToolBar(this, GUI_BAR_TOP);
    topBar->tw_area.h = 35 * GUI_scale;
    topBar->bgcol = sdl_color(0x1e2638bb);
    topBar->titleColor = cWhite;


        
    backButton = new GUI_Button(topBar, " < ", 0, 5, 32, 25, cWhite, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, 1, [](GUI_Button *bt) {
        GUI_FileDialog *dlg = (GUI_FileDialog *)bt->parent->parent;

        int fc = 0;
        char cwd[512];
        strcpy(cwd, dlg->dir.path().c_str());

        char sep = '/';

#ifdef _WIN32
        sep = '\\';
#endif

        for (long i = (long)strlen(cwd) - 1; i >= 0; i--) {
            if (cwd[i] == sep) {
                dlg->dirName[fc++] = &cwd[i + 1];
                cwd[i] = 0;

                if (fc >= max_dir - 1)
                    break;
            }
        }
#if defined(__MACOSX__) || defined(__ANDROID__) || defined(__WIN32__) || defined(__linux__)
        dlg->dirName[fc++] = rootDir;
#endif

        if (fc >= 3) {
            string str = dlg->dir.path().c_str();
            str = str.substr(0, str.length() - 1);
            size_t lastSep = str.find_last_of(sep);
            str = str.substr(0, lastSep + 1);
            dlg->dir.listDir(str);
            dlg->dir.sort();
            dlg->bNeedUpdate = true;
        }
    });

    backButton->bgcol = cGrey;
    backButton->corner = 5;
    backButton->setMargin(5, 5, 5, 5);
    backButton->color_down = sdl_color(0x2D89F9FF);

    dirButton = topBar->addLabel("Test", "", GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER);
    dirButton->titleColor = cWhite;
    dirButton->bgcol = cClear;

    fileEdit = new GUI_EditText(this, "", 0, 0, -1, 30);

    if (defaultFilename) {
        fileEdit->setTitle(defaultFilename);
    }

    if (!bSaveDlg) {
        //fileEdit->disable();
        //fileEdit->titleColor = cGrey;
    }

    fileEdit->setMargin(5, 5, 5, 5);

    // dir list box
    int dir_count = 0;
    int file_count = 0;
    bool* dirDenied = (bool*)malloc(sizeof(bool) * dir.size());
    bool* fileDenied = (bool*)malloc(sizeof(bool) * dir.size());


    for( std::string d : dir.dirs ) {
        //printf( "DIR: %s\n", d.c_str() );
        dirDenied[dir_count] = true;
        dirList[dir_count++] = strdup(d.c_str());;
    }

    for (auto file = dir.begin(); file != dir.end(); file++) {
        /*
        if (file->isDirectory() && dir_count < max_dirs - 1) {
            if (file->canRead()) {
                dirDenied[dir_count] = true;
            } else {
                dirDenied[dir_count] = false;
            }

#ifdef _WIN32
            dirList[dir_count++] = _strdup(file->getFileName().c_str());
#else
            dirList[dir_count++] = strdup(file->getFileName().c_str());
#endif
        } else 
        */
        if (file->isFile() && file_count < max_dirs - 1) {
            if (file->canRead()) {
                fileDenied[file_count] = true;
            } else {
                fileDenied[file_count] = false;
            }

#ifdef _WIN32
            fileList[file_count++] = _strdup(file->getFileName().c_str());
#else
            fileList[file_count++] = strdup(file->getFileName().c_str());
#endif
        }
    }

    // dir list box
    dirList[dir_count] = NULL;
    fileList[file_count] = NULL;

    int by = 38;

    if (bSaveDlg) {
        by = 38;
    }

    GUI_View* horizontalView = new GUI_View(this, "", 0, 0, -1, 240);
    horizontalView->setLayout(GUI_LAYOUT_HORIZONTAL);
    horizontalView->setBorder(0);
    dirListBox = new GUI_List(horizontalView, (const char **)dirList, dir_count, 0, 0, 225, -1, 0, false, [=](GUI_List *l, const char *sel, int index) {
        //printf( "Select: %i\n", index );
        if (index == l->selectedIndex) {
            GUI_FileDialog *dlg = (GUI_FileDialog *)l->parent->parent;
            string path = jsFilePath::addTrailingSlash(dlg->dir.path()) + sel;
            //printf( "Select dir: %s\n", path.c_str() );

            if (1 || jsFile(path).canRead()) 
            {
                dlg->dir.reset();
                dlg->dir.listDir(path);
                dlg->dir.sort();
                //GUI_Log("Enter dir: %s\n", path.c_str());

                if (bttnDelete)
                    bttnDelete->setActive(false);
            } else {
                GUI_Log("Permission denied!\n");
            }

            dlg->bNeedUpdate = true;
        }
    }, NULL, 0);

    dirListBox->title = "Dir List Box";
    dirListBox->bgcol = cWhite;
    dirListBox->setMargin(5, 5, 5, 5);
    dirListBox->setAlignment(GUI_ALIGN_LEFT);

    for (size_t i = 0; i < dirListBox->numCells; i++) {
        //        dirListBox->setEnable(i, dirDenied[i]);
        dirListBox->setEnable(i, true);
    }

    // file list box
    if (fileListBox) {
        delete fileListBox;
    }

    fileListBox = new GUI_List(horizontalView, (const char **)fileList, file_count, 250, 0, 240, -1, 0, false, [=](GUI_List *l, const char *sel, int index) {
        if (index == l->selectedIndex) {
            GUI_FileDialog *dlg = (GUI_FileDialog *)l->parent->parent;
            string path = dlg->dir.path() + sel;

//            if (jsFile(path).canRead()) {
            if (true) {
                if (dlg->fileEdit) {
                    dlg->fileEdit->setTitle(sel);
                }

                if (bttnDelete)
                    bttnDelete->setActive(true);
            } else {
                GUI_Log("Permission denied!\n");
                if (bttnDelete)
                    bttnDelete->setActive(false);
            }

            dlg->bNeedUpdate = true;
        }
    }, NULL, 0);

    fileListBox->title = "File List Box";
    fileListBox->bgcol = cWhite;
    fileListBox->setMargin(5, 5, 5, 5);
    fileListBox->setAlignment(GUI_ALIGN_RIGHT);

    for (size_t i = 0; i < fileListBox->numCells; i++) {
        fileListBox->setEnable(i, fileDenied[i]);
    }

    bottomBar = new GUI_ToolBar(this, GUI_BAR_BOTTOM);
    bottomBar->tw_area.h = 40 * GUI_scale;
    bottomBar->bgcol = sdl_color(0x1e2638bb);
    bottomBar->titleColor = cWhite;
    bottomBar->setAlignment(GUI_ALIGN_BOTTOM);

    // button save/open
    const char *tt;
    if (bSaveDlg) {
        tt = "Save";
    } else {
        tt = "Open";
    }

    //if (bSaveDlg == false)
    {
        bttnDelete = new GUI_Button(bottomBar, "Delete", 0, 5, 80, 30, cWhite, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, 1);
        bttnDelete->bgcol = cGrey;
        bttnDelete->corner = 5;
        bttnDelete->setMargin(5, 5, 5, 5);
        bttnDelete->setAlignment(GUI_ALIGN_LEFT);
        bttnDelete->color_down = sdl_color(0x2D89F9FF);
        bttnDelete->setActive(false);

        bttnDelete->action_cmd = [=](GUI_Button* bt) {
            GUI_FileDialog *dlg = (GUI_FileDialog *)bt->parent->parent;

            if (dlg->fileEdit->title != "" && dlg->fileEdit->title[0]) {
                string path = dlg->dir.path() + dlg->fileEdit->title;
                GUI_Log("Delete File: %s\n", path.c_str());

                SDL_MessageBoxButtonData button[2];
                button[0].buttonid = 11;
                std::string text0 = string("Delete");
                button[0].text = text0.c_str();
                button[0].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;

                button[1].buttonid = 22;
                std::string text = string("Cancel");
                button[1].text = text.c_str();
                button[1].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;

                const SDL_MessageBoxColorScheme colorScheme = { /* .colors (.r, .g, .b) */
                    {   /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
                        { 0, 0, 0 },
                        /* [SDL_MESSAGEBOX_COLOR_TEXT] */
                        { 0, 255, 0 },
                        /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
                        { 255, 255, 255 },
                        /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
                        { 0, 0, 0 },
                        /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
                        { 255, 0, 255 }
                    }
                };

                SDL_MessageBoxData msg;

                msg.flags = SDL_MESSAGEBOX_INFORMATION;
                msg.title = "Confirm Delete";
                string message = guiVAArgsToString("File %s", dlg->fileEdit->title.c_str());
                msg.message = message.c_str();
                msg.numbuttons = 2;
                msg.buttons = button;
                msg.window = GUI_window;
                msg.colorScheme = &colorScheme;

                GUI_Log("Showing MessageBox\n");
                int buttonid;
                SDL_ShowMessageBox(&msg, &buttonid);

                if (buttonid == button[0].buttonid) {
                    jsFile::removeFile(path);
                    bt->setActive(false);
                    dlg->fileEdit->setText("");
                    dir.listDir();
                    dir.sort();
                    bNeedUpdate = true;
                }
            }
        };
    }

    bttnCancel = new GUI_Button(bottomBar, "Cancel", 0, 5, 80, 30, cWhite, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, 0, [](GUI_Button *bt) {
        GUI_FileDialog *dlg = (GUI_FileDialog *)bt->parent->parent;

        if (dlg) {
            dlg->close();
        }

    });

    bttnCancel->bgcol = cGrey;
    bttnCancel->corner = 5;
    bttnCancel->setMargin(5, 5, 5, 5);
    bttnCancel->setAlignment(GUI_ALIGN_RIGHT);
    bttnCancel->color_down = sdl_color(0x2D89F9FF);

    bttnOK = new GUI_Button(bottomBar, tt, 0, 5, 80, 30, cWhite, GUI_ALIGN_CENTER | GUI_ALIGN_VCENTER, 0, [](GUI_Button *bt) {
        GUI_FileDialog *dlg = (GUI_FileDialog *)bt->parent->parent;

        if (dlg->fileEdit->title != "" && dlg->fileEdit->title[0]) {
            string path;
            
            if( dlg->fileEdit->title.compare(0, 7, "http://") == 0 ) {
                path = dlg->fileEdit->title;
            }
            else {
                path = jsFilePath::addTrailingSlash(dlg->dir.path()) + dlg->fileEdit->title;
            }
            GUI_Log("File: %s\n", path.c_str());

            if (dlg->file_cmd) {
                if (dlg->file_cmd(path.c_str()))
                    dlg->close();
            }
        }
    });

    bttnOK->bgcol = cGrey;
    bttnOK->corner = 5;
    bttnOK->setMargin(5, 5, 5, 5);
    bttnOK->setAlignment(GUI_ALIGN_RIGHT);
    bttnOK->color_down = sdl_color(0x2D89F9FF);

    centerToParent();

    bNeedUpdate = true;
    free(dirDenied);
    free(fileDenied);
        
}

GUI_FileDialog::~GUI_FileDialog() {
    // noom: overided on destructor base.
    // cause: onClose is fired (on base class) after 
    // this object and its members are destroyed.
    if (onClose) {
        onClose(this);
        onClose = NULL;
    }
}

void GUI_FileDialog::draw() {
    if (bNeedUpdate) {
        disable();
        bNeedUpdate = false;

        // dir list box
        int dir_count = 0;
        int file_count = 0;
        bool* dirDenied = (bool*)malloc(sizeof(bool) * dir.size());
        bool* fileDenied = (bool*)malloc(sizeof(bool) * dir.size());

        
        for( std::string d : dir.dirs ) {
            //printf( "DIR: %s\n", d.c_str() );
            dirDenied[dir_count] = true;
            dirList[dir_count++] = strdup(d.c_str());;
        }        

        dir.sort();
        for (auto file = dir.begin(); file != dir.end(); file++) {
            /*
            if (file->isDirectory() && dir_count < max_dirs - 1) {
                if (file->canRead()) {
                    dirDenied[dir_count] = true;
                } else {
                    dirDenied[dir_count] = false;
                }

#ifdef _WIN32
                dirList[dir_count++] = _strdup(file->getFileName().c_str());
#else
                dirList[dir_count++] = strdup(file->getFileName().c_str());
#endif
            } else 
            */
            if (file->isFile() && file_count < max_dirs - 1) {
                if (file->canRead()) {
                    fileDenied[file_count] = true;
                } else {
                    fileDenied[file_count] = false;
                }

#ifdef _WIN32
                fileList[file_count++] = _strdup(file->getFileName().c_str());
#else
                fileList[file_count++] = strdup(file->getFileName().c_str());
#endif
            }
        }

        // update dir list box
        dirList[dir_count] = NULL;
        fileList[file_count] = NULL;

        if (dirListBox) {
            dirListBox->setCells((const char**)dirList, dir_count);

            for (size_t i = 0; i < dirListBox->numCells; i++) {
                //                dirListBox->setEnable(i, dirDenied[i]);
                dirListBox->setEnable(i, true);
            }
        }

        // update file list box
        if (fileListBox) {
            fileListBox->setCells((const char**)fileList, file_count);

            for (size_t i = 0; i < fileListBox->numCells; i++) {
                //                fileListBox->setEnable(i, fileDenied[i]);
                fileListBox->setEnable(i, true);
            }
        }

        // update dir button
        int fc = 0;
        char cwd[512];
        strcpy(cwd, dir.path().c_str());

        for (long i = (long)strlen(cwd) - 1; i >= 0; i--) {
#ifdef _WIN32
            if (cwd[i] == '\\') {
#else
            if (cwd[i] == '/') {
#endif
                dirName[fc++] = &cwd[i + 1];
                cwd[i] = 0;

                if (fc >= max_dir - 1)
                    break;
            }
        }

        dirName[fc] = NULL;
        if (dirButton) {
            if( !dirName[1] ) {
                dirButton->setTitle( "/", 0 );
            }
            else {
                dirButton->setTitle(dirName[1], 0);
            }
        }

#ifdef __IPHONEOS__
        if (strcmp(dirName[1], "Documents") == 0) {
            backButton->setActive(false);
        } else {
            backButton->setActive(true);
        }
#endif

        enable();
        free(dirDenied);
        free(fileDenied);
    }

    if (isVisible()) {
        clearViewport(sdl_color(0x0000006B));
        GUI_View::draw();
    }
}
    
std::string GUI_FileDialog::getDirectory() {
    return dir.getAbsolutePath();
}

void GUI_FileDialog::addAllowFileExt(const char *other_ext) {
    dir.allowExt(other_ext);
}

void GUI_FileDialog::clearViewport(SDL_Color color) {
    SDL_Rect cliprect, viewport;
    SDL_RenderGetClipRect(GUI_renderer, &cliprect);
    SDL_RenderGetViewport(GUI_renderer, &viewport);

    SDL_RenderSetViewport(GUI_renderer, GUI_MakeRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale));
    SDL_RenderSetClipRect(GUI_renderer, GUI_MakeRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale));
    GUI_FillRect(0, 0, GUI_windowWidth*GUI_scale, GUI_windowHeight*GUI_scale, color);

    SDL_RenderSetClipRect(GUI_renderer, &cliprect);
    SDL_RenderSetViewport(GUI_renderer, &viewport);
}


