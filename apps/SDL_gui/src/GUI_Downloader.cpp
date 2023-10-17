//
//  GUI_Downloader.cpp
//  PES
//
//  Created by Panutat Tejasen on 4/20/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "SDL_gui.h"
#include "GUI_Downloader.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/fetch.h>
#endif

void GUI_DownloadFile( const char *url, int port, const char *path, std::function<void(GUI_Downloader*)>cb );


GUI_Downloader::GUI_Downloader()
{
    http_count = 0;
#ifndef __EMSCRIPTEN__
    cnntDownload = NULL;
#endif
    callback = NULL;
    callbackProgress = NULL;
    error = false;
    errorMessage = ""; // Tooh
    pathToSave = "";
    unzip = false;
    tag = 0;
}

GUI_Downloader::~GUI_Downloader()
{
#ifndef __EMSCRIPTEN__
    if( cnntDownload )
        delete( cnntDownload );
    cnntDownload = NULL;
#endif
}

#ifndef __EMSCRIPTEN__
void GUI_Downloader::OnBegin( const happyhttp::Response* r, void* userdata )
{
    GUI_Downloader *pInstance = (GUI_Downloader *)userdata;

    if( r->getstatus() == 404 ) {
        pInstance->errorMessage = std::string(r->getreason()); // Tooh
        pInstance->error = true;
    }
    else {
        if( pInstance->pathToSave.length() > 0 ) {
            pInstance->file = jsFile(pInstance->pathToSave);
            pInstance->file.create();
            pInstance->file.open(pInstance->pathToSave, jsFile::ReadWrite, true);
        }
    }
    
    //GUI_Log( "BEGIN (%d %s)\n", r->getstatus(), r->getreason() );
    pInstance->http_count = 0;
}

// invoked to process response body data (may be called multiple times)
void GUI_Downloader::OnData( const happyhttp::Response* r, void* userdata, const unsigned char* data, int n )
{
    GUI_Downloader *pInstance = (GUI_Downloader *)userdata;
    
    if( pInstance->pathToSave.length() != 0 ) {
        //GUI_Log( "Data length: %i\n", pInstance->http_count+n );
        pInstance->file.write((const char *)data, n);
        pInstance->http_count += n;
    }
    else {
        memcpy( &pInstance->http_data[pInstance->http_count], data, n );
        pInstance->http_count += n;
    }
    
    if( pInstance->callbackProgress ) {
        pInstance->callbackProgress( pInstance );
        return;
    }
}

// invoked when response is complete
void GUI_Downloader::OnComplete( const happyhttp::Response* r, void* userdata )
{
    GUI_Downloader *pInstance = (GUI_Downloader *)userdata;
    
    if( pInstance->pathToSave.length() > 0 ) {
        pInstance->file.close();
    }
    else {
        pInstance->http_data[pInstance->http_count] = 0;
    }
    if( pInstance->callback ) {
        pInstance->callback( pInstance );
    }

}
#endif

void GUI_Downloader::downloadFile( std::string url, std::function<void(GUI_Downloader*)>cb ) {
    if( url.compare(0, 7,"http://") == 0 ) {
        url = url.substr( 7 );
        int n = (int)url.find("/");
        std::string path = url.substr(n);
        url = url.substr(0,n);
        downloadFile(url.c_str(), 80, path.c_str(), cb );
    }
}
#ifdef __EMSCRIPTEN__
void GUI_Downloader::onload(void* args, void *data, int size){
    GUI_Downloader *pInstance = (GUI_Downloader *)args;
    GUI_Log( "onload: %i bytes\n", size );
    
  
    if( pInstance->pathToSave.length() > 0 ) {
        pInstance->file = jsFile(pInstance->pathToSave);
        pInstance->file.create();
        pInstance->file.open( pInstance->pathToSave, jsFile::ReadWrite, true );
        
        memcpy( pInstance->http_data, data, size );
        pInstance->http_count = size;
        
        pInstance->file.write((const char *)data, size);
        
        pInstance->file.close();
    }
    else {
        memcpy( pInstance->http_data, data, size );
        pInstance->http_count = size;
        pInstance->http_data[pInstance->http_count] = 0;
    }
    
    if( pInstance->callback ) {
        GUI_Log( "Callback\n" );
        pInstance->callback( pInstance );
    }
    pInstance->bLoaded = true;
}

void GUI_Downloader::onerror(void* args) {
    GUI_LogError("Download Error") << args;
    GUI_Downloader *pInstance = (GUI_Downloader *)args;
    pInstance->error = true;
}
#endif

void GUI_Downloader::downloadFile( const char *url, int port, const char *path, std::function<void(GUI_Downloader*)>cb ) {
    callback = cb;
    error = false;
    errorMessage = ""; // Tooh
    
#ifndef __EMSCRIPTEN__
//    try {
        string strPath = string(path);
        downloadPath = strPath;
        guiStringReplace(strPath, " ", "%20" );
        cnntDownload = new happyhttp::Connection( url, port, timeout ); // Tooh - set send and receive timeout in seconds
        cnntDownload->setcallbacks( OnBegin, OnData, OnComplete, (void *)this );
        cnntDownload->request( "GET", strPath.c_str() );
        GUI_AddHeartbeat( this );
//    }
//    catch( happyhttp::Wobbly w ) {
//        GUI_Log( w.what() );
//        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Network ERROR", w.what(), GUI_window);
//    }
//    catch( exception e ) {
//        GUI_Log( "Exception @GUI_Downloader::downloadFile\n" );
//    }
#else
    GUI_Log( ">>> emscripten_async_wget_data: %s\n", url );
    bLoaded = false;
    GUI_AddHeartbeat( this );
    emscripten_async_wget_data( path, (void *)this, onload, onerror);
#endif
}

bool GUI_Downloader::pump() {
    if( error ) {
        
        if( callback ) {
            callback( this );
        }
        return false;
    }
#ifndef __EMSCRIPTEN__
    //GUI_Log( "Pump......\n" );

    if( cnntDownload->outstanding() ) {
//        try {
            cnntDownload->pump();
//        }
//        catch( exception e ) {
//            
//        }
//        catch( happyhttp::Wobbly e ) {
//            GUI_Log( "HTTP exception: %s\n", e.what());
//            errorMessage = std::string(e.what()); // Tooh
//            error = true;
//            if( callback ) {
//                callback( this );
//            }
//            return false;
//        }
        return true;
    }
    delete( cnntDownload );
    cnntDownload = NULL;
#else
    if( !bLoaded )
        return true;
#endif
    //GUI_Log( "End pump\n" );
    return false;
}

