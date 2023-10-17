//
//  GUI_Network.hpp
//  OSX
//
//  Created by Panutat Tejasen on 3/17/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#ifndef GUI_Network_hpp
#define GUI_Network_hpp

#include <stdio.h>

int GUI_GetHttpStatus(const char *url, int port, const char *path);
int GUI_GetNetworkStatus();

#endif /* GUI_Network_hpp */
