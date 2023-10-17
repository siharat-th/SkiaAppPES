//
//  GUI_SwitchFA.cpp
//  EMBrilliance
//
//  Created by Siharat on 7/24/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "GUI_SwitchFA.hpp"

GUI_SwitchFA::GUI_SwitchFA(GUI_View *parent, const char *title, int x, int y, int width, int height,
                           SDL_Color color, int align, int fontSize)
:GUI_Label(parent, title, x, y, width, height, color, align, fontSize)
, bSwitchOn(false)
, action_cmd(nullptr)
{
    setPadding( 5, 2, 5, 5 );
    setLayout(GUI_LAYOUT_HORIZONTAL);
    
    color_switchoff = cBlack;
    color_switchon = cHightLightSelected;
    
    btSwitch = new GUI_Button(this, "", 0, 0, 0, 0, cBlack, GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER);
    btSwitch->setPadding(0, 0, 0, 0);
    btSwitch->setIcon(GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_off, NULL, NULL,color_switchoff, 1));
    btSwitch->setAlignment(GUI_ALIGN_RIGHT|GUI_ALIGN_VCENTER);
    btSwitch->action_cmd = [=](GUI_Button* bt){
        toggleSwitch();
    };
    
    setSwitchOff();
}

GUI_SwitchFA::~GUI_SwitchFA(){
    
}

void GUI_SwitchFA::setSwitchOn(){
    if(!bSwitchOn){
        bSwitchOn = true;
        btSwitch->setIcon(GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_on, NULL, NULL,color_switchon, 1));
        if(action_cmd){
            action_cmd(this, isSwitchOn());
        }
    }
}

void GUI_SwitchFA::setSwitchOff(){
    if(bSwitchOn){
        bSwitchOn = false;
        btSwitch->setIcon(GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_off, NULL, NULL,color_switchoff, 1));
        if(action_cmd){
            action_cmd(this, isSwitchOn());
        }
    }
}

void GUI_SwitchFA::toggleSwitch(){
    bSwitchOn=!bSwitchOn;
    if(isSwitchOn()){
        btSwitch->setIcon(GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_on, NULL, NULL,color_switchon, 1));
    }
    else{
        btSwitch->setIcon(GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_off, NULL, NULL,color_switchoff, 1));
    }
    if(action_cmd){
        action_cmd(this, isSwitchOn());
    }
}

bool GUI_SwitchFA::isSwitchOn(){
    return bSwitchOn;
}
