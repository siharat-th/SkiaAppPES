//
//  main.h
//  FontWorks
//
//  Created by SIHARAT THAMMAYA on 12/30/2559 BE.
//  Copyright © 2559 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef main_h
#define main_h

#ifdef _WIN32
#define _WINSOCKAPI_  // Tooh - stops Windows.h including winsock.h to prevent macro redefinition in SDL_net.h
#include <Windows.h>
#include <Commdlg.h>  // Tooh - open and save file dialog
#include <codecvt>    // Tooh - convert wchar_t to string
#include <shellapi.h> // Tooh - for CommandLineToArgvW and GetCommandLineW
#include <winsock2.h>
#include <ws2tcpip.h>
#include <SDL_syswm.h>


#include "..\_VisualC\WinApp\PES3\resource.h"

#ifndef __MINGW32__
#define strcasecmp stricmp
#endif
#endif

#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#ifndef __EMSCRIPTEN__
#include "happyhttp.h"
#include <assert.h>
#include <errno.h>
#include "PinnStick.h"
#endif

#ifndef __EMSCRIPTEN__
#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>// for gethostbyname()
#include <errno.h>
#include <unistd.h>
#endif
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#ifndef __EMSCRIPTEN__
#include <SDL_net.h>
#endif

#include "SDL_gui.h"
#include "guiExt.h"
#include "GUI_FileDialog.h"
#include "PES.h"
#include "PESStitch.hpp"
#include "GUI_ButtonFontAwesome.hpp"
#include "Command.hpp"
#include "LocalizedString.hpp"
#include "ZigZagger.hpp"
#include "Shaper.hpp"
#include "SatinOutline.hpp"
#include "GUI_Log.h"
#include "PES_PESEncoder.hpp"
#include "PES_ToolBox.hpp"

extern GUI_View *pesView;
#ifndef TARGET_CLI
extern GUI_ToolBar *topBar;
extern GUI_ToolBar *bottomBar;
extern GUI_ToolBar *layerToolbar;
#endif
extern GUI_Button *btTemplate;
extern GUI_Button *btgift;
extern GUI_Button *btAutoSewing;
extern GUI_Button *btProEmbroidery;
extern GUI_Label *layerLabel;
extern char pref_path[512];

enum ObjectAlignment {
    ALIGNMENT_LEFT = 0,
    ALIGNMENT_CENTER,
    ALIGNMENT_RIGHT
};

extern const char APP_VERSION[];

extern GUI_View *pesView;
extern GUI_View *topView;

extern gui::TexturedButton* fontTab;
extern gui::TexturedButton* shapeTab;
extern gui::TexturedButton* svgTab;
extern gui::TexturedButton* scalableFontTab;
extern gui::TexturedButton* parameterTab;

extern gui::Button* btnScalableFont;

extern void checkDataLayerCompatible();

#if defined(WIN32)
extern HWND hwnd;
#endif

extern string currentOpenFile;
extern string currentOpenPath;

extern std::vector<PESData> pesdataList;
extern PESData clipboardData;
extern bool hasClipboard;

extern SDL_Cursor* cursor_SizeNWSE;
extern SDL_Cursor* cursor_SizeNESW;
extern SDL_Cursor* cursor_SizeWE;
extern SDL_Cursor* cursor_SizeNS;

extern void flagChange();

extern Uint32 ExitCallback(Uint32 interval, void *param);

extern bool __use_font_awesome_texture;
extern int GUI_InitFontAwesomeTexture();

const int NUM_OBJECT_TYPE = 4;
extern const char* layerTypeName[NUM_OBJECT_TYPE];

extern bool showPanel;
extern bool reverse_layer_list;

extern void NewDocument();
extern void OpenFile();
extern void SaveFile();
extern void ImportFile(string extension);
extern void ImportFileCSV();
extern void ExportFileToPinnStick();
extern void ExportPES1File(int option = PES_OPTION_NONE);
extern void ExportPES5File(int option = PES_OPTION_NONE);
extern void ExportDSTFile(int option = PES_OPTION_NONE);
extern void ExportEXPFile(int option = PES_OPTION_NONE);
extern void ExportJEFFile(int option = PES_OPTION_NONE);
extern void ExportBulk(int option = PES_OPTION_NONE);
extern void ExportPENFile(int option = PES_OPTION_NONE);
extern void ExportXXXFile(int option = PES_OPTION_NONE);

extern void ImportSVGFile();
//extern void ExportSVGFile();

Uint32 SavePES(int version, int option = PES_OPTION_NONE);

extern void OpenSetting();
extern void OpenPinnStickSetting();
extern void OpenPESSendingDialog();

#ifndef TARGET_CLI
extern void updatePESInfo();
#endif

extern void updateGUIEnableState();

extern void openLaipuk2000(GUI_View* pressedButton, std::string imgDirName="_tmb");

extern void textEffect(const char* cname = "Text Effect");

extern void addNewText();
extern void align(ObjectAlignment align);

extern void breakLayer();

const SDL_Color hilightColor = {64, 70, 128, 255};
const SDL_Color borderColor = sdl_color(0xa0a0a0ff);

extern const char* defaultText;

extern GUI_List* openRecentSubmenu;
extern GUI_List* importSubmenu;
extern GUI_List* exportSubmenu;
extern GUI_List* layerSubmenu;
extern int totalStitches;

#endif /* main_h */
