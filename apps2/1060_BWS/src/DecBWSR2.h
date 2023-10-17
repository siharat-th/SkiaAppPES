#ifndef __DECBWSR2_LOADER_H__
#define __DECBWSR2_LOADER_H__

#include "DeclarativeUI.h"

class DecBWSR2 : public DecContainer {
public:    
    DecBWSR2();
    virtual ~DecBWSR2() override;

    virtual void update() override;

    bool bR2;
};

DecBWSR2 &BWSR2();

#endif //__DECBWSR2_LOADER_H__