#ifndef __DEC_CONTAINER_H__
#define __DEC_CONTAINER_H__

#include "DecWidget.h"

class DecContainer : public DecWidget {
public:    
    DecContainer();
    virtual ~DecContainer() override;

    virtual std::string type() override { return "Container"; }

protected:
    virtual void _draw_content(SkCanvas &canvas) override;
};

DecContainer &Container();

#endif //__DEC_CONTAINER_H__