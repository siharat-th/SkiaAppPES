//
//  GUI_Network.cpp
//  OSX
//
//  Created by Panutat Tejasen on 3/17/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#include "GUI_Network.h"
#include <iostream>
#ifndef __EMSCRIPTEN__
#include "happyhttp.h"
#endif
#include <errno.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>	// for gethostbyname()
#include <errno.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#ifndef __MINGW32__
#define strcasecmp stricmp
#endif
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_net.h>
#include "SDL_gui.h"

static int http_count = 0;
static unsigned char http_data[1000];
static int http_status = -1;

#ifndef __EMSCRIPTEN__
static happyhttp::Connection *cnnt = NULL;

// invoked when response headers have been received
void GUINet_OnBegin(const happyhttp::Response* r, void* userdata) {
    //GUI_Log("BEGIN (%d %s)\n", r->getstatus(), r->getreason());
    http_status = r->getstatus();
    http_count = 0;
}

// invoked to process response body data (may be called multiple times)
void GUINet_OnData(const happyhttp::Response* r, void* userdata, const unsigned char* data, int n) {
    if (http_count + n > 1000)
        n = 0;

    memcpy(&http_data[http_count], data, n);
    http_count += n;
}

// invoked when response is complete
void GUINet_OnComplete(const happyhttp::Response* r, void* userdata) {
    http_data[http_count] = 0;
}

int GUI_GetHttpStatus(const char *url, int port, const char *path) {
    cnnt = new happyhttp::Connection(url, port);
    cnnt->setcallbacks(GUINet_OnBegin, GUINet_OnData, GUINet_OnComplete, 0);

    http_status = -1;
    http_count = 0;
    cnnt->request("GET", path);

    while (cnnt->outstanding()) {
        cnnt->pump();

        if (http_status != -1)
            break;
    }

    return http_status;
}
#endif

int GUI_GetNetworkStatus() {
    char szT[128] = {0};
    IPaddress espertIP;
    int res = SDLNet_ResolveHost(&espertIP, "pinn.online", 80);

    if (!res) {
        char buffer[32] = {0};
        const char* p = inet_ntop(AF_INET, &espertIP.host, buffer, (unsigned int)32);
        sprintf(szT, "Pinn.Online %s", p);

        TCPsocket connection = SDLNet_TCP_Open(&espertIP);

        if (connection) {
            SDLNet_SocketSet sockets = SDLNet_AllocSocketSet(1);

            if (sockets == NULL) {
                return -1;
            } else {
                int status = GUI_GetHttpStatus("pinn.online", 80, "/");
                return status; // 0, or 302
            }
        } else {
            return -1; // Host not found
        }
    }

    return -1; // Offline
}
