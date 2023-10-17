//
//  guiFolder.hpp
//  PINNPES
//
//  Created by Siharat on 8/12/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef guiFolder_hpp
#define guiFolder_hpp

#include <stdio.h>
#include "SDL_gui.h"
#include "guiGroup.hpp"

namespace gui{
    
    class Folder : public Group{
    public:
        
        static Folder* createSubFolder(std::string label, GUI_View* parent, Folder* root, float indent);
        
        Folder(GUI_View* parent, std::string label, int x=0, int y=0, int w=0, int h=0);
        virtual ~Folder() override;
        
        virtual GUI_Label* addLabel(std::string title="", int x=0, int y=0, int width=0, int height=0,
                                int align=GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_MICRO);
        
        Folder* addSubFolder(std::string name, Folder* root, float indent);
        
        
        virtual void add_child(GUI_View* child) override;
        virtual void remove_child(GUI_View* child) override;
        virtual void remove_all_children() override;
        
        
        void expand();
        void toggle();
        void collapse();
        
        bool getIsExpanded();
        
        float getIndent();
        
        // hidden
        Folder(GUI_View* parent);
        
    private:
        
        
        void reLayout();
        
        Button* header;
        GUI_View* _innerView;
        bool isExpanded;
        Folder* root;
    };
    
    
}

#endif /* guiFolder_hpp */
