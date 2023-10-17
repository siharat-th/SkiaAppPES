//
//  guiSwitch.hpp
//  VG2EMB
//
//  Created by Siharat on 25/11/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef guiSwitch_hpp
#define guiSwitch_hpp

#include <stdio.h>
#include "SDL_gui.h"

namespace gui {
    
    
    // NOTE: Require Font Awesome 4
    class Switch : public GUI_Label{
        bool isDown;
        bool bSwitchOn;
        
    public:
        Switch(GUI_View *parent, const char *title, int x=0, int y=0, int width=0, int height=0,
                     SDL_Color color=cBlack, int align=GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        virtual ~Switch() override;
        
        void setSwitchOn();
        void setSwitchOff();
        void toggleSwitch();
        bool isSwitchOn();
        
        std::function<void(Switch *, bool)> action_cmd;
        
        SDL_Color color_switchoff;
        SDL_Color color_switchon;
        
    private:
        GUI_Button *btSwitch;
    };
    
}

#endif /* guiSwitch_hpp */
