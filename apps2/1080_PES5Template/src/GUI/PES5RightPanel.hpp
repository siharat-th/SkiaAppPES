//
//  PES5RightPanel.hpp
//  
//
//  Created by SIHARAT THAMMAYA on 17/8/2565 BE.
//

#ifndef PES5RightPanel_hpp
#define PES5RightPanel_hpp

#include "DeclarativeUI.h"



namespace sk_ui {

DecColumnContainer & CreatePesRightPanel();
void UpdateGUIRightPanel();
DecList * GetLayerList();
DecScroller & CreateBrotherColorWidget(std::function<void(int index)> onSelected);
const sk_sp<SkTypeface> & GetDefaultTypeface();
const char* GetDefaultTypefaceName();
int GetTypefaceIndex(int nObjIndex = -1);

class DecLayerListItem : public DecRowContainer {
public:
    DecLayerListItem();
    virtual ~DecLayerListItem() override;

    virtual std::string type() override { return "DecListItem"; }
    virtual DecWidget &select( bool i ) override;

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    
    int findMyIndex();
    
protected:
    virtual void _draw_content(SkCanvas &canvas) override;
};
DecLayerListItem &LayerListItem();

class DecColorListItem : public DecRowContainer {
public:
    DecColorListItem();
    virtual ~DecColorListItem() override;

    virtual std::string type() override { return "DecListItem"; }
    virtual DecWidget &select( bool i ) override;

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;
    
    int findMyIndex();
    
protected:
    virtual void _draw_content(SkCanvas &canvas) override;
};
DecColorListItem &ColorListItem();

class DecPagesPes5 : public DecPageContainer {
public:
    DecPagesPes5() : DecPageContainer() {}
    virtual ~DecPagesPes5() override {}

    virtual std::string type() override { return "DecPagesPes5"; }

    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey modifiers) override {
        return false;
    }
};

DecPagesPes5& PagesPes5();
}

#endif /* PES5RightPanel_hpp */
