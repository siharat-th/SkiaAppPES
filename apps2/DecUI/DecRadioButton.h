#ifndef __DEC_RADIOBUTTON_H__
#define __DEC_RADIOBUTTON_H__

#include "DecWidget.h"
#include "DecLabel.h"

class DecRadioButton : public DecLabel {
public:    
    DecRadioButton( const char *title, void (*callback)(DecRadioButton &button)=NULL );
    virtual ~DecRadioButton() override;

    virtual std::string type() override { return "RadioButton"; }

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;   
protected:
    void (*_callback)(DecRadioButton &checkbox);
    bool _isChecked;
};

DecRadioButton &RadioButton( const char *title, void (*callback)(DecRadioButton &button)=NULL );

#endif //__DEC_RADIOBUTTON_H__