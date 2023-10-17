#ifndef __DEC_COMBOBOXITEM_H__
#define __DEC_COMBOBOXITEM_H__

#include "DecWidget.h"
#include "DecRowContainer.h"
#include "DecLabel.h"

class DecList;
class DecText;

class DecComboBoxItem : public DecListItem {
public:    
    DecComboBoxItem( const char *text ); 
    virtual ~DecComboBoxItem() override;

    virtual std::string type() override { return "DecComboBoxItem"; }

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override; 
protected:
};

DecComboBoxItem &ComboBoxItem( const char *text );

#endif //__DEC_COMBOBOXITEM_H__