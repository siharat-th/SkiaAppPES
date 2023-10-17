#ifndef __DEC_TREEITEM_H__
#define __DEC_TREEITEM_H__

#include "DecWidget.h"
#include "DecRowContainer.h"
#include "DecColumnContainer.h"
#include "DecLabel.h"
#include "DecText.h"

class DecTree;

class DecTreeItem : public DecColumnContainer {
public:    
    DecTreeItem( const char *text ); 
    virtual ~DecTreeItem() override;

    virtual std::string type() override { return "DecTreeItem"; }
    virtual DecWidget &select( bool i ) override;

    virtual DecWidget &_( DecWidget &c, bool front=false  ) override;      

    virtual bool onMouse(int x, int y, skui::InputState, skui::ModifierKey) override; 

    virtual void bubbleAction( int actionID, DecWidget *sender, void *data=NULL ) override;

    DecTreeItem &addWidget( DecWidget &widget );

    void toggleExpand();

    inline virtual const char *text() override { return _text.c_str(); }
    virtual DecWidget& text( const char *t) override;    

    DecTreeItem& expand();
    DecTreeItem& collapse();
    inline bool isExpand() { return _isExpand; }

    DecTreeItem& collapseAll( int level=0, int cur_level=0 );

    std::vector<DecWidget*> getChildrenNodes() { return _pChildContainer->_children; }

    DecTreeItem *_pParentTreeItem;

    DecWidget* getRootItem() { return _pContentRow; }

protected:
    DecTree *_pTree;
    DecIcon *_pExpandIcon;
    DecIcon *_pContentRow;
    DecRowContainer *_pRightIconContainer;
    DecColumnContainer *_pChildContainer;
    DecText *_pText;
    
    bool _isExpand;
};

DecTreeItem &TreeItem( const char *text );

#endif //__DEC_TREEITEM_H__