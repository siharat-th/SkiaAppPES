#ifndef __DEC_COMBOBOX_H__
#define __DEC_COMBOBOX_H__

#include "DecWidget.h"
#include "DecRowContainer.h"
#include "DecColumnContainer.h"
#include "DecText.h"

class DecComboBoxItem;

class DecComboBox : public DecRowContainer {
public:    
    DecComboBox();
    virtual ~DecComboBox() override;

    virtual void focus(bool f) override;

    virtual std::string type() override { return "ComboBox"; }

    virtual void bubbleAction( int actionID, DecWidget *sender, void *data=NULL ) override;

    virtual DecWidget &_( DecWidget &c, bool front=false ) override;

    //virtual void update() override {
    //    //SkDebugf("[%d, %d] [%d, %d]\n", _width, _height, _parent->width(), _parent->height());
    //    if (_width > _parent->width()) {
    //        width(_parent->width());
    //        dirty(true, true);
    //        invalidate();
    //    }
    //}

    DecWidget &select_comboboxItem( int index, bool update=true );
    DecWidget &selectByChildValue( int value, bool update=true );   // pom
    DecWidget &selectByChildValue( float value, bool update=true ); // pom 
    int getSelectedIndex(); // pom
    float getSelectedValue(); // pom

    int count() { return _pColumn->_children.size(); }
    
    std::vector<DecWidget*> getChildren() { return _pColumn->_children; } // pom

    DecComboBoxItem *_selectedComboBoxItem;    
protected:
    DecText *_pText;
    DecScroller *_pScroller;
    DecColumnContainer *_pColumn;
};

DecComboBox &ComboBox();

#endif //__DEC_COMBOBOX_H__
