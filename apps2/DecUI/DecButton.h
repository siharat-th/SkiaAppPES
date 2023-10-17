#ifndef __DEC_BUTTON_H__
#define __DEC_BUTTON_H__

#include "DecWidget.h"
#include "DecLabel.h"

class DecButton : public DecLabel {
public:    
    DecButton( const char *title, const int icon = 0,std::function<void(DecButton &button)> callback=nullptr ); 
    virtual ~DecButton() override;

    virtual std::string type() override { return "Button"; }

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;   
protected:
    std::function<void(DecButton &button)> _callback;

    virtual void _draw_bg(SkCanvas &canvas) override;   
};

DecButton &Button( const char *title, const int icon = 0, std::function<void(DecButton &button)> callback=nullptr );

class DecTabButton : public DecButton {
public:    
    DecTabButton( const char *title, const int icon = 0,std::function<void(DecButton &button)> callback=nullptr ); 
    virtual ~DecTabButton() override;

    virtual std::string type() override { return "TabButton"; }    
    virtual DecWidget& select(bool a) override;

    bool selected;
protected:

    virtual void _draw_bg(SkCanvas &canvas) override; 
    virtual void _draw_border(SkCanvas& canvas) override;
};

DecTabButton &TabButton( const char *title, const int icon = 0, std::function<void(DecButton &button)> callback=nullptr );


#endif //__DEC_BUTTON_H__