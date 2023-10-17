#ifndef __DEC_CHECKBOX_H__
#define __DEC_CHECKBOX_H__

#include "DecWidget.h"
#include "DecLabel.h"

class DecCheckBox : public DecLabel {
public:    
    DecCheckBox( const char *title, std::function<void(DecCheckBox &cb)>callback=NULL );
    virtual ~DecCheckBox() override;

    virtual std::string type() override { return "CheckBox"; }

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;   

    DecCheckBox& check( bool b, bool update=true );
    bool isChecked(){ return  _isChecked; } // pom
protected:
    std::function<void(DecCheckBox &cb)>_callback;
    bool _isChecked;
};

DecCheckBox &CheckBox( const char *title, std::function<void(DecCheckBox &cb)>callback=NULL );

#endif //__DEC_CHECKBOX_H__