#ifndef __DEC_COLUMN_CONTAINER_H__
#define __DEC_COLUMN_CONTAINER_H__

#include "DecContainer.h"

class DecColumnContainer : public DecContainer {
public:    
    DecColumnContainer();
    virtual ~DecColumnContainer() override;

    virtual std::string type() override { return "Column"; }

    virtual int contentY() override { return _padding[0]; }
    virtual int contentHeight() override;     

protected:
    virtual void _updateChildrenUI( int level ) override;
    virtual void _updateChildrenAlignment() override;
    virtual void _setGap( int g ) override;    
};

DecColumnContainer &Column();

#endif //__DEC_COLUMN_CONTAINER_H__