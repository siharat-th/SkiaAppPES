//
//  guiSwitch.cpp
//  VG2EMB
//
//  Created by Siharat on 25/11/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "guiSwitch.hpp"
#include "GUI_Label.h"
#include "GUI_Text.h"

namespace gui {
    
    Switch::Switch(GUI_View *parent, const char *title, int x, int y, int width, int height,
                               SDL_Color color, int align, int fontSize)
    :GUI_Label(parent, "", x, y, width, height, color, align, fontSize)
    , bSwitchOn(false)
    , action_cmd(nullptr)
    {
        setLayout(GUI_LAYOUT_HORIZONTAL);
        
        color_switchoff = cBlack;
        color_switchon = cHightLightSelected;
        
        auto l = new GUI_Label(this, title, 0, 0, 0, 0, color, GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, fontSize);
        
        btSwitch = new GUI_Button(this, "", 0, 0, 0, 0, cBlack, GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER);
        btSwitch->setPadding(0, 0, 0, 0);
        btSwitch->setIcon( GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_off, 0, 0, cWhite, 2));
        btSwitch->iconColor = color_switchoff;
        btSwitch->setAlignment(GUI_ALIGN_RIGHT|GUI_ALIGN_VCENTER);
        btSwitch->action_cmd = [=](GUI_Button* bt){
            toggleSwitch();
        };
        
        int iconWidth = btSwitch->icon_area.w / GUI_scale;
        l->setMargin(0, iconWidth+10, 0, 0);
        l->setAlignment(GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER);
        
        setSwitchOff();
    }
    
    Switch::~Switch(){
        
    }
    
    void Switch::setSwitchOn(){
        if(!bSwitchOn){
            bSwitchOn = true;
//            btSwitch->setIcon(GUI_createTextureFormUnicode(kIcon_toggle_on, 1));
            btSwitch->setIcon( GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_on, 0, 0, cWhite, 2));
            btSwitch->iconColor = color_switchon;
            if(action_cmd){
                action_cmd(this, isSwitchOn());
            }
        }
    }
    
    void Switch::setSwitchOff(){
        if(bSwitchOn){
            bSwitchOn = false;
//            btSwitch->setIcon(GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_off, NULL, NULL,color_switchoff, 1));
            btSwitch->setIcon( GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_off, 0, 0, cWhite, 2));
            btSwitch->iconColor = color_switchoff;
            if(action_cmd){
                action_cmd(this, isSwitchOn());
            }
        }
    }
    
    void Switch::toggleSwitch(){
        bSwitchOn=!bSwitchOn;
        if(isSwitchOn()){
//            btSwitch->setIcon(GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_on, NULL, NULL,color_switchon, 1));
            btSwitch->setIcon( GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_on, 0, 0, cWhite, 2));
            btSwitch->iconColor = color_switchon;
        }
        else{
//            btSwitch->setIcon(GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_off, NULL, NULL,color_switchoff, 1));
            btSwitch->setIcon( GUI_ButtonFA::createTextureFormUnicode(kIcon_toggle_off, 0, 0, cWhite, 2));
            btSwitch->iconColor = color_switchoff;
        }
        if(action_cmd){
            action_cmd(this, isSwitchOn());
        }
    }
    
    bool Switch::isSwitchOn(){
        return bSwitchOn;
    }
    
}
