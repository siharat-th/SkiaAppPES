//
//  GUI_SwitchFA.hpp
//  EMBrilliance
//
//  Created by Siharat on 7/24/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef GUI_SwitchFA_hpp
#define GUI_SwitchFA_hpp

#include "SDL_gui.h"

// GUI_Switch-FontAwesome texture version
class GUI_SwitchFA : GUI_Label{
    bool isDown;
    bool isFocus;
    bool bSwitchOn;
    
public:
    GUI_SwitchFA(GUI_View *parent, const char *title, int x=0, int y=0, int width=0, int height=0,
                 SDL_Color color=cBlack, int align=GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, int fontSize=0);
    ~GUI_SwitchFA();
    
    void setSwitchOn();
    void setSwitchOff();
    void toggleSwitch();
    bool isSwitchOn();
    
    std::function<void(GUI_SwitchFA *, bool)> action_cmd;
    
    SDL_Color color_switchoff;
    SDL_Color color_switchon;
    
private:
    GUI_Button *btSwitch;
};

#endif /* GUI_SwitchFA_hpp */
