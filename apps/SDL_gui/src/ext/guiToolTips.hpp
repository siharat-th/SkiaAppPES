//
//  guiToolTips.hpp
//  VG2EMB
//
//  Created by Siharat on 25/11/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef guiToolTips_hpp
#define guiToolTips_hpp

#include "SDL_gui.h"

namespace gui {
    
    bool ToolTips_HitTest(int mouseX, int mouseY);
    bool ToolTips_IsFound();
    bool ToolTips_IsFadeOut();
    std::string ToolTips_Get();
    bool ToolTips_Render();
}

#endif /* guiToolTips_hpp */
