#ifndef __DEC_STACK_CONTAINER_H__
#define __DEC_STACK_CONTAINER_H__

#include "DecContainer.h"

class DecStackContainer : public DecContainer {
public:    
    DecStackContainer();
    virtual ~DecStackContainer() override;

    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) override;

    virtual std::string type() override { return "Stack"; }
protected:
};

DecStackContainer &Stack();

#endif //__DEC_STACK_CONTAINER_H__