//
//  GUI_Preference.cpp
//  PES
//
//  Created by Jimmy on 4/15/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "GUI_Preference.h"


GUI_Preference::GUI_Preference( const char *appName )
{
    pref_path = SDL_GetPrefPath( "com.jimmysoftware", appName );
    fname = pref_path + "pref";
}

GUI_Preference::~GUI_Preference()
{
    
}

int GUI_Preference::Write() {
    int count = 0;
    if (map.empty())
        return 0;
    
    FILE *fp = fopen(fname.c_str(), "w");
    if (!fp)
        return -errno;
    
    for(std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); it++) {
        fprintf(fp, "%s=%s\n", it->first.c_str(), it->second.c_str());
        count++;
    }
    
    fclose(fp);
    return count;
}

int GUI_Preference::Read() {
    int count = 0;
    if (access(fname.c_str(), R_OK) < 0)
        return -errno;
    
    FILE *fp = fopen(fname.c_str(), "r");
    if (!fp)
        return -errno;
    
    map.clear();
    
    char *buf = 0;
    size_t buflen = 0;
    
    while(getline(&buf, &buflen, fp) > 0) {
        char *nl = strchr(buf, '\n');
        if (nl == NULL)
            continue;
        *nl = 0;
        
        char *sep = strchr(buf, '=');
        if (sep == NULL)
            continue;
        *sep = 0;
        sep++;
        
        std::string s1 = buf;
        std::string s2 = sep;
        
        map[s1] = s2;
        
        count++;
    }
    
    if (buf)
        free(buf);
    
    fclose(fp);
    return count;
}

void GUI_Preference::Set( std::string key, std::string value )
{
    map[key] = value;
}

std::string GUI_Preference::Get( std::string key )
{
    return map[key];
}

