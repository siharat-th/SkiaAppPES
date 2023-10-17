#ifndef __DEC_LIST_CONTAINER_H__
#define __DEC_LIST_CONTAINER_H__

#include "DecColumnContainer.h"
#include "DecScroller.h"
#include "DecListItem.h"
#include "DecContainer.h"

class DecList : public DecContainer {
public:    
    DecList();
    virtual ~DecList() override;

    virtual std::string type() override { return "List"; }

    virtual DecWidget &_( DecWidget &c, bool front=false ) override;

    virtual void bubbleAction( int actionID, DecWidget *sender, void *data=NULL ) override;

    virtual void clear();

    DecWidget &select_listItem( int index );
    DecListItem *getItem( int index );

    int count() { return _pColumn->_children.size(); }

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override;

    DecListItem *_selectedListItem;
    DecColumnContainer *_pColumn;
    DecScroller *_pScroller;
protected:
};

DecList &List();

#endif //__DEC_LIST_CONTAINER_H__