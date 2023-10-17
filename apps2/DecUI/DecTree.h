#ifndef __DEC_TREE_CONTAINER_H__
#define __DEC_TREE_CONTAINER_H__

#include "DecColumnContainer.h"
#include "DecList.h"

class DecTreeItem;

class DecTree : public DecList {
public:    
    DecTree();
    virtual ~DecTree() override;

    virtual std::string type() override { return "Tree"; }

    virtual void bubbleAction( int actionID, DecWidget *sender, void *data=NULL ) override;

    void select_treeItem( DecTreeItem *item, bool bUpdateVisible=false );

    void collapseAll( int level=0 );

    virtual void clear() override; 

    DecTreeItem *_selectedTreeItem;
protected:
    //DecColumnContainer *_pColumn;
};

DecTree &Tree();

#endif //__DEC_TREE_CONTAINER_H__