#ifndef __DEC_SWITCH_H__
#define __DEC_SWITCH_H__

#include "DecWidget.h"
#include "DecLabel.h"

class DecSwitch : public DecLabel {
public:    
    DecSwitch( const char *title, void (*callback)(DecSwitch &button)=NULL );
    virtual ~DecSwitch() override;

    virtual std::string type() override { return "RadioButton"; }

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;   
protected:
    void (*_callback)(DecSwitch &checkbox);
    bool _isChecked;
};

DecSwitch &Switch( const char *title, void (*callback)(DecSwitch &button)=NULL );

#endif //__DEC_SWITCH_H__