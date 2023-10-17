//
//  guiGroup.hpp
//  VG2EMB
//
//  Created by Siharat on 24/11/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef guiGroup_hpp
#define guiGroup_hpp

#include <stdio.h>
#include "guiButton.hpp"
#include "guiDigit.hpp"
#include "guiSwitch.hpp"
#include "guiTheme.h"
#include "SDL_gui.h"

namespace gui{
 
    class BaseGroup : public GUI_Label{
    public:
        
        enum SelectedControlType{
            NORMAL_SELECTED,
            SINGLE_SELECTED,
            MULTIPLE_SELECTED
        };
        
        BaseGroup(GUI_View* parent, int layout=GUI_LAYOUT_VERTICAL, int x=0, int y=0, int w=0, int h=0, int align=GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER);

        GUI_View* addView(std::string title="", int x=0, int y=0, int w=0, int h=0);
    };
    
    
    
    class ButtonGroup : public BaseGroup{
    public:
        
        ButtonGroup(GUI_View* parent, std::string label="", SelectedControlType type=NORMAL_SELECTED);
        virtual ~ButtonGroup() override;
        
        Button* addButton(uint16_t icon, std::string title="", int x=0, int y=0, int width=0, int height=0,
                          int align=GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        
        Button* addButton(std::string title="", int x=0, int y=0, int width=0, int height=0,
                          int align=GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        
        size_t size();
        void layoutAdjust();
        
        std::vector<Button*> buttons;
        std::function<void(Button*)> action_cmd;
        
        virtual void enable() override;
        virtual void disable() override;
        
    protected:
        void onButtonPressed(Button* bt);
        GUI_Label* hgroup;
        SelectedControlType type;
    };
    
    
    
    class TexturedButtonGroup : public BaseGroup{
    public:
        
        TexturedButtonGroup(GUI_View* parent, int layout, bool groupSegment=false, SelectedControlType type=NORMAL_SELECTED);
        virtual ~TexturedButtonGroup() override;
        
        TexturedButton* addButton(uint16_t icon, int x=0, int y=0, int width=0, int height=0);
        TexturedButton* addButton(std::string texture_filename, int x=0, int y=0, int width=0, int height=0);
        TexturedButton* addButton(SDL_Texture* texture, bool isTextureColored, int x=0, int y=0, int width=0, int height=0);
        
        int size();
        void layoutAdjust();
        
        std::vector<TexturedButton*> buttons;
        std::function<void(TexturedButton*)> action_cmd;
        
    protected:
        void onButtonPressed(TexturedButton* bt);
        SelectedControlType type;
        bool groupSegment;
    };
    
    class Group : public BaseGroup{
    public:
        Group(GUI_View* parent, int layout=GUI_LAYOUT_VERTICAL, int x=0, int y=0, int w=0, int h=0);
        virtual ~Group() override;
        
        GUI_Label* addLabel(std::string title="", int x=0, int y=0, int width=0, int height=0,
                        int align=GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        
        Button* addButton(std::string title="", int x=0, int y=0, int width=0, int height=0,
                          int align=GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        
        Button* addButton(uint16_t icon, std::string title="", int x=0, int y=0, int width=0, int height=0,
                          int align=GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);

        GUI_Dropdown* addDropdown(std::string title="", int x=0, int y=0, int width=0, int height=0,
                          int align=GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        GUI_List* addList(std::string title="", int x=0, int y=0, int width=0, int height=0, int cellHeight=32,
                                  int align=GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);

        GUI_EditText* addEditText(std::string title="", GUI_EditText::Type type=GUI_EditText::Type::TEXT_ANY,
                                int x=0, int y=0, int width=0, int height=0,
                                int align=GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        
        GUI_EditText* addEditText(int width, GUI_EditText::Type type=GUI_EditText::Type::TEXT_ANY, int align=GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        
        Digit* addDigit(float value, float min, float max, float inc, std::string label="", std::string format="%g");
        
        Digit* addDigit(std::string label="", std::string format="%g", int x=0, int y=0, int width=0, int height=0,
                        int align=GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        
        ButtonGroup* addButtonGroup(std::string label="", SelectedControlType type=NORMAL_SELECTED);
        ButtonGroup* addButtonGroupSingleSelected(std::string label="");
        ButtonGroup* addButtonGroupMultipleSelected(std::string label="");
        
        TexturedButtonGroup* addTexturedButtonGroup(int layout=GUI_LAYOUT_VERTICAL, bool groupSegment=false, SelectedControlType type=NORMAL_SELECTED);
        TexturedButtonGroup* addTexturedButtonGroupSingleSelected(int layout=GUI_LAYOUT_VERTICAL, bool groupSegment=false);
        TexturedButtonGroup* addTexturedButtonGroupMultipleSelected(int layout=GUI_LAYOUT_VERTICAL, bool groupSegment=false);
        
        Switch* addSwitch(std::string title="", int x=0, int y=0, int width=0, int height=0, int align=GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        
        PopupButton* addPopupButton(std::string default_title="", int x=0, int y=0, int width=0, int height=0, int align=GUI_ALIGN_RIGHT|GUI_ALIGN_VCENTER, int fontSize=GUI_FONT_NORMAL);
        
        Group* addHorizontalGroup(int x=0, int y=0, int w=0, int h=0);

        void addVerticalSpace(int h);
        void addSeparator(SDL_Color c = cGrey);
        
        Group* addVerticalGroup(int x=0, int y=0, int w=0, int h=0);
        Group* addAbsoluteGroup(int x=0, int y=0, int w=0, int h=0);
        
        void addVerticalMargin();
    protected:
        
    };
    
    class BaseScrollView : public Group{
    public:
        BaseScrollView(GUI_View* parent, int x=0, int y=0, int w=0, int h=0);
        virtual ~BaseScrollView() override;
        
        virtual void add_child(GUI_View* child) override;
        virtual void remove_child(GUI_View* child) override;
        virtual void remove_all_children() override;
        
    protected:
        GUI_View *_innerView;
    };
    
    class VerticalScrollView : public BaseScrollView{
    public:
        VerticalScrollView(GUI_View* parent, int x=0, int y=0, int w=0, int h=0);
        virtual ~VerticalScrollView() override;
//        virtual void draw();
    };
    
    // MARK: ToolBar
    class ToolBar : public Group{
    public:
        ToolBar(GUI_View* parent, int x=0, int y=0, int w=-1, int h=40);
        virtual ~ToolBar() override;
    };
    
    // MARK: Panel
    class Panel : public Group{
    public:
        Panel(GUI_View* parent, int layout=GUI_LAYOUT_VERTICAL, int x=0, int y=0, int w=0, int h=0);
        virtual ~Panel() override;
        
        void createTitle(std::string title, bool closeButton);
        virtual void setTitle(std::string title, int fontSize = -1) override;
        
    protected:
        GUI_Label* titleLabel;
        gui::Button* closeButton;

    };
    
}

#endif /* guiGroup_hpp */
