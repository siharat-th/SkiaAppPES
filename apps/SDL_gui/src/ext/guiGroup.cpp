//
//  guiGroup.cpp
//  VG2EMB
//
//  Created by Siharat on 24/11/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#include "guiGroup.hpp"
#include "GUI_Text.h"

namespace gui {
    
    // MARK: BaseGroup
    BaseGroup::BaseGroup(GUI_View* parent, int layout, int x, int y, int w, int h, int align)
    :GUI_Label(parent, "", x, y, w, h, cClear, align)
    {
        setLayout(layout);
        setPadding(0, 0, 0, 0);
        click_through = false;
    }
    
    GUI_View* BaseGroup::addView(std::string t, int x, int y, int w, int h)
    {
        return new GUI_View(this, t.c_str(), x, y, w, h,cClear);
    }
    
    
    
    
    // MARK: ButtonGroup
    ButtonGroup::ButtonGroup(GUI_View* parent, std::string label, SelectedControlType t):
    BaseGroup(parent, GUI_LAYOUT_VERTICAL, 0, 0, 0, 0, GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER),
    action_cmd(nullptr),
    hgroup(nullptr),
    type(t)
    {
        hgroup = new GUI_Label(this, "", 0, 0, 0, 0);
        hgroup->setLayout(GUI_LAYOUT_HORIZONTAL);
        hgroup->setAlignment(GUI_ALIGN_CENTER);
        hgroup->setPadding(0,0,0,0);
        if(label.empty()==false){
            GUI_Label* l = new GUI_Label(this, label.c_str(),0,0,0,0,defaultTitleColor,GUI_ALIGN_CENTER|GUI_ALIGN_TOP, GUI_FONT_MICRO);
            l->setAlignment(GUI_ALIGN_CENTER|GUI_ALIGN_TOP);
            l->setPadding(0, 0, 0, 0);
        }
    }
    
    ButtonGroup::~ButtonGroup(){
        buttons.clear();
    }
    
    Button* ButtonGroup::addButton(uint16_t icon, std::string t, int x, int y, int w, int h, int align, int fnt)
    {
        Button* b = new Button(hgroup, t, x, y, w, h, defaultTitleColor, align, fnt);
        b->action_cmd = [&](Button* bt){
            onButtonPressed(bt);
        };
#if __USE_SKIA__
        b->setIcon(GUI_createImageFromUnicode(icon));
#else
        b->setIcon(GUI_createTextureFormUnicode(icon));
#endif
        buttons.push_back(b);
        layoutAdjust();
        return buttons.back();
    }
    
    Button* ButtonGroup::addButton(std::string t, int x, int y, int w, int h, int align, int fnt)
    {
        Button* b = new Button(hgroup, t, x, y, w, h, defaultTitleColor, align, fnt);
        b->action_cmd = [&](Button* bt){
            onButtonPressed(bt);
        };
        buttons.push_back(b);
        layoutAdjust();
        return buttons.back();
    }
    
    void ButtonGroup::layoutAdjust(){
        if(size()>=2){
            size_t sz = size();
            buttons[sz-2]->corners[1] = 0;
            buttons[sz-2]->corners[2] = 0;
            buttons[sz-1]->corners[0] = 0;
            buttons[sz-1]->corners[3] = 0;
        }

        printf("layoutAdjust: \n");
        
        float maxButtonWidth  = 0;
        float maxButtonHeight = 0;
        float totalButtonsWidth = 0;
        for(auto b:buttons){
            maxButtonWidth = MAX(maxButtonWidth, b->getPhysicalWidth());
            maxButtonHeight = MAX(maxButtonHeight, b->getPhysicalHeight());
            totalButtonsWidth+=b->getPhysicalWidth();
        }
        if(size()>1){
            for(auto b:buttons){
                if(b->getPhysicalWidth()<maxButtonWidth || b->getPhysicalHeight()<maxButtonHeight){
                    float dw = maxButtonWidth - b->tw_area.w;
                    float dh = maxButtonHeight - b->tw_area.h;
                    b->tw_area.w  = (int)maxButtonWidth;
                    b->tw_area.h = (int)maxButtonHeight;
                    b->icon_area.x += (int)(dw/2);
                    b->icon_area.y += (int)(dh/2);
                    b->title_area.x += (int)(dw/2);
                    b->title_area.y += (int)(dh/2);
                }
            }
        }
    }
    
    size_t ButtonGroup::size() {
        return buttons.size();
    }
    
    void ButtonGroup::onButtonPressed(Button* bt){
        if(type==SINGLE_SELECTED){
            if(bt->getToggleState()==false && bt->toggle()){
                for(auto b:buttons){
                    if(b!=bt && b->getToggleState()){
                        b->toggle();
                    }
                }
            }
        }
        else if(type==MULTIPLE_SELECTED){
            bt->toggle();
        }
        
        if(action_cmd!=nullptr)
            action_cmd(bt);
    }
    
    void ButtonGroup::enable(){
        GUI_View::enable();
        for(auto &b:buttons){
            b->enable();
            b->setActive(true);
        }
    }
    
    void ButtonGroup::disable(){
        GUI_View::disable();
        for(auto &b:buttons){
            b->disable();
            b->setActive(false);
        }
    }
    
    
    
    
    
    
    
    // MARK: TexturedButtonGroup
    TexturedButtonGroup::TexturedButtonGroup(GUI_View* parent, int layout, bool gs, SelectedControlType t):
    BaseGroup(parent, layout, 0, 0, 0, 0, GUI_ALIGN_CENTER|GUI_ALIGN_VCENTER),
    action_cmd(nullptr),
    type(t),
    groupSegment(gs)
    {
    }
    
    TexturedButtonGroup::~TexturedButtonGroup(){
        buttons.clear();
    }
    
    TexturedButton* TexturedButtonGroup::addButton(uint16_t icon, int x, int y, int w, int h)
    {
        TexturedButton* b = new TexturedButton(this, GUI_createTextureFormUnicode(icon), false, x, y, w, h);
        if(groupSegment){
            b->borderColor = b->theme.color.button.stroke.normal = defaultTheme.color.button.stroke.hover;
        }
        
        b->action_cmd = [&](Button* bt){
            onButtonPressed((TexturedButton*)bt);
        };
        buttons.push_back(b);
        layoutAdjust();
        return buttons.back();
    }
    
    TexturedButton* TexturedButtonGroup::addButton(std::string filename, int x, int y, int w, int h)
    {
        SDL_Texture * texture = GUI_LoadTexture(filename, GUI_renderer);
        TexturedButton* b = new TexturedButton(this, texture, true, x, y, w, h);
//        if(groupSegment){
//            _theme.color.button.stroke.normal = _theme.color.button.stroke.hover;
//        }
        
        if(groupSegment){
            b->borderColor = b->theme.color.button.stroke.normal = defaultTheme.color.button.stroke.hover;
        }
        
        if(groupSegment)
            b->borderColor = defaultTheme.color.button.stroke.hover;
        b->action_cmd = [&](Button* bt){
            onButtonPressed((TexturedButton*)bt);
        };
        buttons.push_back(b);
        layoutAdjust();
        return buttons.back();
    }
    
    TexturedButton* TexturedButtonGroup::addButton(SDL_Texture* texture, bool isTextureColored, int x, int y, int w, int h){
        TexturedButton* b = new TexturedButton(this, texture, isTextureColored, x, y, w, h);
        if(groupSegment){
            b->borderColor = b->theme.color.button.stroke.normal = defaultTheme.color.button.stroke.hover;
        }
        b->action_cmd = [&](Button* bt){
            onButtonPressed((TexturedButton*)bt);
        };
        buttons.push_back(b);
        layoutAdjust();
        return buttons.back();
    }
    
    void TexturedButtonGroup::layoutAdjust(){
        if(groupSegment){
            if(size()>=2){
                int sz = size();
                if(layout==GUI_LAYOUT_VERTICAL)
                {
                    buttons[sz-2]->corners[2] = 0;
                    buttons[sz-2]->corners[3] = 0;
                    buttons[sz-1]->corners[0] = 0;
                    buttons[sz-1]->corners[1] = 0;
                }
                else if(layout==GUI_LAYOUT_HORIZONTAL){
                    buttons[sz-2]->corners[1] = 0;
                    buttons[sz-2]->corners[2] = 0;
                    buttons[sz-1]->corners[0] = 0;
                    buttons[sz-1]->corners[3] = 0;
                }
            }
        }
        
        float maxButtonWidth  = 0;
        float maxButtonHeight = 0;
        for(auto b:buttons){
            maxButtonWidth = MAX(maxButtonWidth, b->getPhysicalWidth());
            maxButtonHeight = MAX(maxButtonHeight, b->getPhysicalHeight());
        }
        if(size()>1){
            for(auto b:buttons){
                if(b->getPhysicalWidth()<maxButtonWidth || b->getPhysicalHeight()<maxButtonHeight){
                    float dw = maxButtonWidth - b->tw_area.w;
                    float dh = maxButtonHeight - b->tw_area.h;
                    b->tw_area.w  = (int)maxButtonWidth;
                    b->tw_area.h = (int)maxButtonHeight;
                    b->icon_area.x += (int)(dw/2);
                    b->icon_area.y += (int)(dh/2);
                    b->title_area.x += (int)(dw/2);
                    b->title_area.y += (int)(dh/2);
                }
            }
        }
        
        if(!groupSegment && layout==GUI_LAYOUT_VERTICAL){
            buttons.back()->setMargin(0, 0, 5, 0);
        }
    }
    
    int TexturedButtonGroup::size(){
        return (int)buttons.size();
    }
    
    void TexturedButtonGroup::onButtonPressed(TexturedButton* bt){
        if(type==SINGLE_SELECTED){
            if(bt->getToggleState()==false && bt->toggle()){
                for(auto b:buttons){
                    if(b!=bt && b->getToggleState()){
                        b->toggle();
                    }
                }
            }
        }
        else if(type==MULTIPLE_SELECTED){
            bt->toggle();
        }
        
        if(action_cmd!=nullptr)
            action_cmd(bt);
    }
    
    
    
    
    
    // MARK: Group
    Group::Group(GUI_View* p, int l, int x, int y, int w, int h)
    :gui::BaseGroup(p,l,x,y,w,h)
    {
    }
    Group::~Group(){
    }
    
    GUI_Label* Group::addLabel(std::string t, int x, int y, int w, int h, int align, int fnt)
    {
        auto l = new GUI_Label(this, t.c_str(), x, y, w, h, defaultTheme.color.label, align, fnt);
        addVerticalMargin();
        return l;
    }
    
    Button* Group::addButton(uint16_t icon, std::string t, int x, int y, int w, int h, int align, int fnt)
    {
        Button* b = addButton(t, x, y, w, h, align, fnt);
#if __USE_SKIA__
        b->setIcon(GUI_createImageFromUnicode(icon));
#else
        b->setIcon(GUI_createTextureFormUnicode(icon));
#endif
        return b;
    }
    
    Button* Group::addButton(std::string t, int x, int y, int w, int h, int align, int fnt)
    {
        Button* b = new Button(this, t, x, y, w, h, defaultTitleColor, align, fnt);
        addVerticalMargin();
        return b;
    }
    
    GUI_Dropdown* Group::addDropdown(std::string t, int x, int y, int w, int h, int align, int fnt)
    {
        GUI_Dropdown* b = new GUI_Dropdown(this, "", NULL, 0, x, y, w, h, NULL, defaultTitleColor,
                                     align, fnt);
        addVerticalMargin();
        return b;
    }

    GUI_List* Group::addList(std::string t, int x, int y, int w, int h, int cellHeight, int align, int fnt)
    {
        GUI_List* b = new GUI_List(this, NULL, 0, x, y, w, h, cellHeight, false, NULL, fnt);
        addVerticalMargin();
        return b;
    }

    GUI_EditText* Group::addEditText(int w, GUI_EditText::Type type, int align, int fnt){
        return addEditText("", type, 0, 0, w, 0, align, fnt);
    }
    
    GUI_EditText* Group::addEditText(std::string t, GUI_EditText::Type type,
                                   int x, int y, int w, int h, int align, int fnt)
    {
        auto tin = new GUI_EditText(this, t.c_str(), x, y, w, h, defaultTheme.color.editText.text, align, fnt);
        tin->setType(type);
        tin->bgcol = defaultTheme.color.inputAreaBackground;
        tin->borderColor = defaultTheme.color.border;
        addVerticalMargin();
        return tin;
    }
    
    Digit* Group::addDigit(float value, float min, float max, float inc, std::string l, std::string f){
        auto d = addDigit(l, f);
        d->set(min, max, value, inc);
        return d;
    }
    
    Digit* Group::addDigit(std::string l, std::string f, int x, int y, int w, int h, int align, int fnt)
    {
        auto d = new Digit(this, l, f, x, y, w, h, defaultTitleColor, align, fnt);
        addVerticalMargin();
        return d;
    }

    
    ButtonGroup* Group::addButtonGroup(std::string label, SelectedControlType type){
        auto bg = new ButtonGroup(this, label, type);
        addVerticalMargin();
        return bg;
    }
    
    ButtonGroup* Group::addButtonGroupSingleSelected(std::string label){
        return addButtonGroup(label, SINGLE_SELECTED);
    }
    ButtonGroup* Group::addButtonGroupMultipleSelected(std::string label){
        return addButtonGroup(label, MULTIPLE_SELECTED);
    }
    
    TexturedButtonGroup* Group::addTexturedButtonGroup(int layout, bool groupSegment, SelectedControlType type){
        auto g = new TexturedButtonGroup(this, layout, groupSegment, type);
        addVerticalMargin();
        return g;
    }
    
    TexturedButtonGroup* Group::addTexturedButtonGroupSingleSelected(int layout, bool groupSegment){
        return addTexturedButtonGroup(layout, groupSegment, SINGLE_SELECTED);
    }
    
    TexturedButtonGroup* Group::addTexturedButtonGroupMultipleSelected(int layout, bool groupSegment){
        return addTexturedButtonGroup(layout, groupSegment, MULTIPLE_SELECTED);
    }
    
    Switch* Group::addSwitch(std::string title, int x, int y, int width, int height, int align, int fontSize){
        auto sw = new Switch(this, title.c_str(), x, y, width, height, defaultTitleColor, align, fontSize);
        addVerticalMargin();
        return sw;
    }
    
    PopupButton* Group::addPopupButton(std::string default_title, int x, int y, int width, int height, int align, int fontSize){
        auto pb = new PopupButton(this, default_title, x, y, width, height, align, fontSize);
        addVerticalMargin();
        return pb;
    }
    
    Group* Group::addHorizontalGroup(int x, int y, int w, int h){
        auto g = new Group(this, GUI_LAYOUT_HORIZONTAL, x, y, w, h);
        g->bgcol = cClear;
        g->dragable = false;
        g->setMargin(0,0,0,0);
        g->setPadding(0,0,0,0);
        return g;
    }
    
    Group* Group::addVerticalGroup(int x, int y, int w, int h){
        auto g = new Group(this, GUI_LAYOUT_VERTICAL, x, y, w, h);
        g->bgcol = cClear;
        g->dragable = false;
        g->setMargin(0,0,0,0);
        g->setPadding(0,0,0,0);
        return g;
    }
    
    Group* Group::addAbsoluteGroup(int x, int y, int w, int h){
        auto g = new Group(this, GUI_LAYOUT_ABSOLUTE, x, y, w, h);
        g->bgcol = cClear;
        g->dragable = false;
        g->setMargin(0,0,0,0);
        g->setPadding(0,0,0,0);
        return g;
    }
    
    void Group::addVerticalSpace(int h) {
        if (lst_child >= 0) {
            auto c = children[lst_child];
            
            if (c) {
                c->margin[2] += (h * GUI_scale);
            }
        }
    }
    
    void Group::addSeparator(SDL_Color c) {
        auto l = new GUI_Label(this, "", 0, 0, -1, 2, cClear, GUI_ALIGN_LEFT | GUI_ALIGN_VCENTER);
        l->bgcol = c;
        int padding = (int)defaultTheme.layout.padding;
        l->setMargin(0, 5, 0, 5);
        l->align = GUI_ALIGN_CENTER;
        l->updateSubLayout();
    }
    
    void Group::addVerticalMargin(){
        if(layout==GUI_LAYOUT_VERTICAL){
            if(children[lst_child])
                children[lst_child]->setMargin((int)defaultTheme.layout.vMargin, 0, (int)defaultTheme.layout.vMargin, 0);
        }
    }
    
    
    
    
    // MARK: Base scroll view
    BaseScrollView::BaseScrollView( GUI_View *parent, int x, int y, int width, int height )
    :Group( parent, GUI_LAYOUT_VERTICAL, x, y, width, height ),
    _innerView(NULL)
    {
        bgcol = defaultTheme.color.inputAreaBackground;
        setBorder(0);
    }
    
    BaseScrollView::~BaseScrollView()
    {
        if( _innerView ) {
            _innerView->remove_all_children();
        }
    }
    
    void BaseScrollView::add_child( GUI_View *child ) {
        if( _innerView ) {
            _innerView->add_child( child );
            child->parent = _innerView;
        }
        else {
            GUI_View::add_child( child );
        }
    }
    
    void BaseScrollView::remove_child( GUI_View *child ) {
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
    
    void BaseScrollView::remove_all_children()
    {
        if( _innerView )
            _innerView->remove_all_children();
    }
    
    
    
    
    // MARK: Vertical scroll view
    VerticalScrollView::VerticalScrollView( GUI_View *parent, int x, int y, int width, int height )
    :BaseScrollView( parent, x, y, width, height )
    {
        _innerView = new GUI_View( this, "_inner", 0, 0, -1, 0 );
        _innerView->setLayout( GUI_LAYOUT_VERTICAL );
        _innerView->setBorder( 0 );
        _innerView->dragable = false;
        _innerView->bgcol = cClear;
        _innerView->display_cmd = [=](GUI_View* v){
            float dx =0, dy=0;
            
            if(v->tw_area.x!=v->parent->tw_area.x){
                dx = (float)(v->parent->tw_area.x - v->tw_area.x);
            }
            if(v->topleft.y>0){
                dy = (float)(-v->topleft.y);
            }
            
            bool scrollable = v->tw_area.h > v->parent->tw_area.h;
            if(scrollable){
                float vb = (float)(v->tw_area.y + v->tw_area.h);
                float pb = (float)(v->parent->tw_area.y + v->parent->tw_area.h);
                if(vb<pb){
                    dy = pb-vb;
                }
            }
            
            v->move((int)dx, (int)dy);
        };
        
        handle_event_cmd = [=](GUI_View* v, SDL_Event* ev)->bool{
            if(ev->type==SDL_MOUSEWHEEL){
                bool scrollable = _innerView->tw_area.h > v->tw_area.h;
                int x, y;
                SDL_GetMouseState(&x, &y);
                x = (int)(x * GUI_mouseScale);
                y = (int)(y * GUI_mouseScale);
                if(scrollable && v->hitTest(x, y)){
                    float* wx = (float*)ev->user.data1;
                    float* wy = (float*)ev->user.data2;
                    _innerView->move(0, (int)((*wy)*10));
                }
            }
            else if(ev->type==SDL_MOUSEMOTION){
                int mx = (int)(ev->motion.x*GUI_mouseScale);
                int my = (int)(ev->motion.y*GUI_mouseScale);
            }
            return false;
        };
    }
    
    VerticalScrollView::~VerticalScrollView()
    {
        if( _innerView ) {
            _innerView->remove_all_children();
        }
    }
    
//    void VerticalScrollView::draw(){
//        ofPushStyle();
//        
//        // draw clip area
//        ofPushView();
//        {
//            ofViewport(tw_area);
//            ofSetupScreen();
//            
//            ofPushMatrix();
//            ofTranslate(-tw_area.x, -tw_area.y);
//            View::draw();
//            
//            
//            _innerView->enableDraw();
//            _innerView->draw();
//            for(int i=0; i<=_innerView->lst_child;i++){
//                _innerView->children[i]->enableDraw();
//                _innerView->children[i]->draw();
//                _innerView->children[i]->disableDraw();
//            }
//            _innerView->disableDraw();
//            ofPopMatrix();
//        }
//        ofPopView();
//        
//        // draw scroll bar
//        ofSetLineWidth(2);
//        bool scrollable = _innerView->tw_area.height > tw_area.height;
//        if(scrollable){
//            int w = 12 * ofGetRenderScale();
//            ofFill();
//            ofSetColor(_theme.color.scroll.background);
//            ofDrawRectangle(tw_area.x+tw_area.width-w, tw_area.y, w, tw_area.height);
//            
//            float dh = _innerView->tw_area.height - tw_area.height;
//            float dy = abs(_innerView->tw_area.y - tw_area.y);
//            float ratio = (dy/dh);
//            
//            float bw = 8 * ofGetRenderScale();
//            float bh = tw_area.height - (_innerView->tw_area.height - tw_area.height) - 2;
//            if(bh<25 * ofGetRenderScale())
//                bh = 25 * ofGetRenderScale();
//            float y = tw_area.y + ((tw_area.height-bh) * ratio);
//            if(y>tw_area.y+tw_area.height-bh-1)
//                y = tw_area.y+tw_area.height-bh-1;
//            
//            if(y<tw_area.y+1)
//                y = tw_area.y+1;
//            
//            ofRectangle bar(tw_area.x+tw_area.width-w, y, w, bh);
//            
//            if(ofGetMousePressed()==false){
//                if(bar.inside(ofGetMouseX(), ofGetMouseY())){
//                    ofSetColor(_theme.color.scroll.barOnMouseHover);
//                    if(ofGetMousePressed()==false)
//                        _innerView->dragReverseY = true;
//                }
//                else{
//                    ofSetColor(_theme.color.scroll.bar);
//                    if(ofGetMousePressed()==false)
//                        _innerView->dragReverseY = false;
//                }
//            }
//            else{
//                if(_innerView->dragReverseY){
//                    ofSetColor(_theme.color.scroll.barOnMouseHover);
//                }
//                else{
//                    ofSetColor(_theme.color.scroll.bar);
//                }
//            }
//            
//            ofDrawRectRounded(tw_area.x+tw_area.width-bw-(w-bw)/2, y, bw, bh, bw/2-1);
//            
//            ofSetColor(_theme.color.border);
//            ofDrawLine(tw_area.x+tw_area.width-w, tw_area.y, tw_area.x+tw_area.width-w, tw_area.y+tw_area.height);
//        }
//        else{
//            _innerView->dragReverseY = false;
//        }
//        
//        ofNoFill();
//        ofSetColor(_theme.color.border);
//        ofDrawRectangle(tw_area.x, tw_area.y, tw_area.width, tw_area.height);
//        
//        ofPopStyle();
//    }
    
    
    
    // MARK: ToolBar
    ToolBar::ToolBar(GUI_View* parent, int x, int y, int w, int h)
    :Group(parent, GUI_LAYOUT_HORIZONTAL, x, y, w, h)
    {
        setBorder(1);
        setPadding(5,10,5,10);
        setMargin(0,0,0,0);
        bgcol = defaultTheme.color.guiBackground;
        borderColor = defaultTheme.color.border;
    }
    
    ToolBar::~ToolBar(){}
    
    
    // MARK: Panel
    Panel::Panel(GUI_View* parent, int layout, int x, int y, int w, int h)
    :Group(parent, layout, x, y, w, h),
    titleLabel(NULL),
    closeButton(NULL)
    {
        setBorder(1);
        setAlignment(GUI_ALIGN_ABSOLUTE);
        setPadding(5,10,5,10);
        setMargin(0,0,0,0);
        
        bgcol = defaultTheme.color.guiBackground;
        borderColor = defaultTheme.color.border;
    }
    
    Panel::~Panel(){}
    
    void Panel::createTitle(std::string title, bool closeButton){
        if(titleLabel==NULL){
            titleLabel = addLabel(title, 0, 0, -1, 0, GUI_ALIGN_LEFT|GUI_ALIGN_VCENTER, GUI_FONT_NORMAL);
            titleLabel->setPadding(2, 0, 2, 10);
            titleLabel->bgcol = cDarkGrey;
            titleLabel->titleColor=cWhite;
            titleLabel->setLayout(GUI_LAYOUT_HORIZONTAL);
        }
        if(closeButton){
            gui::TexturedButton* closeBT = new gui::TexturedButton(titleLabel, GUI_createTextureFromUnicode(kIcon_window_close), true);
            closeBT->setAlignment(GUI_ALIGN_RIGHT);
            closeBT->setPadding(2, 2, 2, 2);
            closeBT->color_down = cHightLightSelected;
            closeBT->action_cmd = [=](gui::Button* bt){
                close();
            };
        }
    }
    
    void Panel::setTitle(std::string title, int fontSize){
        if(titleLabel==NULL)
            createTitle(title, true);
        else{
            if(title!=titleLabel->title)
                titleLabel->setTitle(title);
        }
    }
    
}
