#ifndef __DEC_SCROLLER_H__
#define __DEC_SCROLLER_H__

#include "DecContainer.h"
#include "DecUI.h"

class DecScroller : public DecContainer {
public:    
    DecScroller( int scroll_type );
    virtual ~DecScroller() override;

    virtual std::string type() override { return "Scroller"; }

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;  
    virtual bool onMouseWheel(int x, int y, float deltax, float delta, skui::ModifierKey) override;

    virtual void update() override;   
    virtual void reset();  
    bool moveXY( int dx, int dy );

    DecWidget* pScroll2Widget;

protected:
    int _scrollType;
    int _lastMoveX;
    int _lastMoveY;
    bool _firstMove;

};

DecScroller &Scroller( int scroll_type=SCROLL_BOTH );

#endif //__DEC_SCROLLER_H__