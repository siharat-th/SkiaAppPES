#ifndef __DEC_PAGE_CONTAINER_H__
#define __DEC_PAGE_CONTAINER_H__

#include "DecContainer.h"

class DecPageContainer : public DecContainer {
public:    
    DecPageContainer();
    virtual ~DecPageContainer() override;

    virtual std::string type() override { return "Pages"; }
    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) override;

    virtual DecWidget &child( DecWidget &c, bool front=false ) override;

    void next();
    void previous();
    void activate( int i ) { _activate_child( i ); }

    int getActivateIndex() { return _activate_index; }
protected:
    //virtual void _draw_children(SkCanvas &canvas) override;
    void _activate_child( int i );
    int _activate_index;

};

DecPageContainer &Pages();

#endif //__DEC_PAGE_CONTAINER_H__