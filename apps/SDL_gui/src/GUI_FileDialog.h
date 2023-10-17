//
//  GUI_FileDialog.h
//  OSX
//
//  Created by SIHARAT THAMMAYA on 9/4/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#ifndef GUI_FileDialog_hpp
#define GUI_FileDialog_hpp

#include "GUI_List.h"
#include "GUI_Button.h"
#include "GUI_EditText.h"
#include "GUI_ToolBar.h"
#include "jsFileUtils.h"

const int max_dir = 64;
const int max_dirs = 2048;

class GUI_FileDialog : public GUI_View {
public:
    GUI_FileDialog(GUI_View *parent, bool bSave, const char *ext, const char *defaultFN, bool(*file_cmd)(const char *) = NULL);
    virtual ~GUI_FileDialog() override;

    const char *fileExtension;
    const char *defaultFilename;

    bool bNeedUpdate;
    char cur_wdir[512];
    char cd[512];

    char *dirName[max_dir];

    char dirs[max_dirs];
    char *dirList[max_dirs];
    char *fileList[max_dirs];

    GUI_Button *backButton;
    GUI_Label *dirButton;
    GUI_List *dirListBox;
    GUI_List *fileListBox;
    GUI_List *menuDir;
    GUI_EditText *fileEdit;
    GUI_Button *bttnOK;
    GUI_Button *bttnCancel;
    GUI_Button *bttnDelete;
    GUI_ToolBar *topBar;
    GUI_ToolBar *bottomBar;

    std::function<bool(const char*)> file_cmd;
    void setDirectory(std::string customDirectory = "");
    std::string getDirectory();
    bool bSaveDlg;
    virtual void draw() override;

    // pom
    void addAllowFileExt(const char *other_ext);

    jsDirectory dir;

private:
    void clearViewport(SDL_Color color);
};

#endif /* GUI_FileDialog_hpp */
