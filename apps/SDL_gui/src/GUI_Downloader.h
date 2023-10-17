//
//  GUI_Downloader.hpp
//  PES
//
//  Created by Panutat Tejasen on 4/20/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef GUI_Downloader_hpp
#define GUI_Downloader_hpp

#include <stdio.h>
#ifndef __EMSCRIPTEN__
#include "happyhttp.h"
#endif
#include "jsFileUtils.h"

class GUI_Downloader {
#ifndef __EMSCRIPTEN__
    happyhttp::Connection *cnntDownload;
#endif

public:
    GUI_Downloader();
    ~GUI_Downloader();
    bool error;
    std::string pathToSave;
    bool unzip;
    jsFile file;
    void downloadFile(std::string url, std::function<void(GUI_Downloader*)>cb = nullptr );
    void downloadFile(const char *url, int port, const char *path, std::function<void(GUI_Downloader*)>cb = nullptr);
    bool pump();

    int http_count;
    unsigned char http_data[5000000];

#ifndef __EMSCRIPTEN__
    static void OnBegin(const happyhttp::Response* r, void* userdata);
    static void OnData(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n);
    static void OnComplete(const happyhttp::Response* r, void* userdata);
#else
    static void onload(void* args, void *data, int size);
    static void onerror(void* e);
    
    bool bLoaded;
#endif

    std::function<void(GUI_Downloader*)> callback;
    std::function<void(GUI_Downloader*)> callbackProgress;

    // Tooh
    std::string downloadPath;
    std::string errorMessage;
    int tag;
    int timeout; // set send and receive timeout in seconds
};

void GUI_DownloadFile(const char *url, int port, const char *path, std::function<void(GUI_Downloader*)>cb);

#endif /* GUI_Downloader_hpp */

