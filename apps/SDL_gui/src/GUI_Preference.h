//
//  GUI_Preference.hpp
//  PES
//
//  Created by Jimmy on 4/15/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef GUI_Preference_hpp
#define GUI_Preference_hpp

#include <stdio.h>
#include "SDL_gui.h"
#include "jsFileUtils.h"

class GUI_Preference {
    std::string pref_path;
    std::string fname;
    
    std::map<std::string, std::string> map;
    
public:
    GUI_Preference(const char *appName);
    ~GUI_Preference();
    int Write();
    int Read();
    
    void Set( std::string key, std::string value );
    std::string Get( std::string key );
};

#endif /* GUI_Preference_hpp */
