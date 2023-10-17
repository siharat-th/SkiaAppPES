#ifndef __DEC_LISTITEM_H__
#define __DEC_LISTITEM_H__

#include "DecWidget.h"
#include "DecRowContainer.h"
#include "DecLabel.h"

class DecList;

class DecListItem : public DecRowContainer {
public:    
    DecListItem( const char *text ); 
    virtual ~DecListItem() override;

    virtual std::string type() override { return "DecListItem"; }
    virtual DecWidget &select( bool i ) override;

    inline virtual const char *text() override { return _text.c_str(); }
    virtual DecWidget& text( const char *t) override;

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override; 

    DecText *_pText;

protected:
    //DecList *_pList;
};

DecListItem &ListItem( const char *text );

#endif //__DEC_LISTITEM_H__