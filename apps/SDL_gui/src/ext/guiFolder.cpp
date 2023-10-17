//
//  guiFolder.cpp
//  PINNPES
//
//  Created by Siharat on 8/12/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "guiFolder.hpp"

namespace gui {
    
    static SDL_Texture* texCaretRight, *texCaretDown;
    
    Folder::Folder(GUI_View* parent, std::string label, int x, int y, int w, int h)
    :Group(parent, GUI_LAYOUT_VERTICAL, x, y, w, h),
    isExpanded(false),
    header(nullptr),
    _innerView(nullptr),
    root(nullptr)
    {
        if(!texCaretRight){
            texCaretRight = GUI_createTextureFromUnicode(kIcon_caret_right);
            texCaretDown = GUI_createTextureFromUnicode(kIcon_caret_down);
        }
        
        header = addButton(label, 0, 0, -1, 0, GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER);
        header->setBorder(1);
        header->setIcon(texCaretRight);
        header->corner = 0;
        header->setMargin(0,0,0,0);
        
        _innerView = new GUI_View( this, "", 0, 0, -1, 0 );
        _innerView->setLayout( GUI_LAYOUT_VERTICAL );
        _innerView->dragable=false;
        _innerView->setBorder( 1 );
        _innerView->bgcol = gui::defaultTheme.color.inputAreaBackground;
        _innerView->borderColor = gui::defaultTheme.color.border;
        _innerView->hide();
        
        header->action_cmd = [&](Button* bt){
            toggle();
            if(isExpanded)
                bt->setToolTip("Click to collapse");
            else
                bt->setToolTip("Click to expand");
        };
    }
    
    Folder* Folder::createSubFolder(std::string label, GUI_View* parent, Folder* root, float indent){
        auto f = new Folder(parent);
        f->header = f->addButton(label, 0, 0, -1, 0, GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER);
        f->header->setBorder(1);
        f->header->setIcon(texCaretRight);
        f->header->corner = 0;
        f->header->setMargin(0,0,0,0);
        f->header->setPadding(0,10,10,(int)indent);
        f->root = root;

        f->header->action_cmd = [&](Button* bt){
            ((Folder*)bt->parent)->toggle();
        };

        return f;
    }
    
    
    Folder::~Folder(){
        if( _innerView ) {
            _innerView->remove_all_children();
        }
    }
    
    void Folder::add_child( GUI_View *child ) {
        if( _innerView ) {
            _innerView->add_child( child );
            child->parent = _innerView;
        }
        else {
            GUI_View::add_child( child );
        }
    }

    void Folder::remove_child( GUI_View *child ) {
        if( child == _innerView ) {
            GUI_View::remove_child(child);
            _innerView = NULL;
        }
        else {
            if( _innerView )
                _innerView->remove_child( child );
        }
        updateLayout();
    }

    void Folder::remove_all_children()
    {
        if( _innerView )
            _innerView->remove_all_children();
    }
    
    
    
    GUI_Label* Folder::addLabel(std::string title, int x, int y, int w, int h,
                                int align, int fontSize)
    
    {
        auto l = new GUI_Label(this, title.c_str(), x, y,-1,h, cBlack, align, fontSize);
        l->setBorder(1);
        l->borderColor = gui::defaultTheme.color.border;
        float indent = getIndent();
        l->setPadding(5, 10, 5, (int)indent);
        return l;
    }
    
    Folder* Folder::addSubFolder(std::string name, Folder* root, float indent){
        auto f = Folder::createSubFolder(name, this, root, indent);
        return f;
    }
    
    
    void Folder::expand(){
        isExpanded = true;
        reLayout();
    }
    
    void Folder::toggle(){
        isExpanded=!isExpanded;
        reLayout();
    }
    
    void Folder::collapse(){
        isExpanded=false;
        reLayout();
    }
    
    bool Folder::getIsExpanded(){
        return isExpanded;
    }
    
    float Folder::getIndent(){
        return float((header->icon_area.w) + header->icon_gap + header->padding[3]);
    }
    

    Folder::Folder(GUI_View* parent)
    :Group(parent, GUI_LAYOUT_VERTICAL, 0,0,-1,0),
    isExpanded(false),
    header(nullptr),
    root(nullptr),
    _innerView(nullptr)
    {
        if(texCaretRight==NULL){
            texCaretRight = GUI_createTextureFromUnicode(kIcon_caret_right);
            texCaretDown = GUI_createTextureFromUnicode(kIcon_caret_down);
        }
        
        _innerView = new GUI_View( this, "", 0, 0, -1, 0 );
        _innerView->setLayout( GUI_LAYOUT_VERTICAL );
        _innerView->dragable=false;
        _innerView->setBorder( 1 );
        _innerView->bgcol = gui::defaultTheme.color.inputAreaBackground;
        _innerView->borderColor = gui::defaultTheme.color.border;
        _innerView->hide();
    }
    
    void Folder::reLayout(){
        if(isExpanded){
            header->setIcon(texCaretDown, true);
            _innerView->show();
        }
        else{
            header->setIcon(texCaretRight, true);
            _innerView->hide();
        }

//        if(root){
//            if(root->parent)
//                root->parent->updateLayout();
//            else
//                root->updateLayout();
//        }
//        else if(parent)
//            parent->updateLayout();
//        else{
//             updateSubLayout();
//            updateLayout();
//        }
        
//        parent->updateLayout();
////        topView->updateLayout();
//        GUI_TopView->updateLayout();
    }
    
}
