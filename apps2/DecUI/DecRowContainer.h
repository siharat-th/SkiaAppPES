#ifndef __DEC_ROW_CONTAINER_H__
#define __DEC_ROW_CONTAINER_H__

#include "DecContainer.h"

class DecRowContainer : public DecContainer {
public:    
    DecRowContainer();
    virtual ~DecRowContainer() override;

    virtual std::string type() override { return "Row"; }

    virtual int contentX() override { return _padding[3]; }
    virtual int contentWidth() override; 

protected:
    virtual void _updateChildrenUI( int level ) override;
    virtual void _updateChildrenAlignment() override;
    virtual void _setGap( int g ) override;    
};

DecRowContainer &Row();

#endif //__DEC_ROW_CONTAINER_H__