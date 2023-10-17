#ifndef __DEC_FLOW_CONTAINER_H__
#define __DEC_FLOW_CONTAINER_H__

#include "DecContainer.h"

class DecFlowContainer : public DecContainer {
public:    
    DecFlowContainer();
    virtual ~DecFlowContainer() override;

    virtual std::string type() override { return "Flow"; }

    virtual int contentX() override { return _padding[3]; }
    virtual int contentY() override { return _padding[0]; }
    virtual int contentWidth() override { return _width - (_padding[1] + _padding[3]); }
    virtual int contentHeight() override { return _height - (_padding[0] + _padding[2]); }  

protected:
    virtual void _updateChildrenUI( int level ) override;
    virtual void _updateChildrenAlignment() override {}
    virtual void _setGap( int g ) override;
};

DecFlowContainer &Flow();

#endif //__DEC_FLOW_CONTAINER_H__