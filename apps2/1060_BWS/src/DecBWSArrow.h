#ifndef __DECBWSARROW_LOADER_H__
#define __DECBWSARROW_LOADER_H__

#include "DeclarativeUI.h"

class DecBWSArrow : public DecContainer {
public:    
    DecBWSArrow(int type=0);
    virtual ~DecBWSArrow() override;

    virtual void _draw_content(SkCanvas& canvas) override; 

    int _arrowType;

    bool *_pStatus;
};

DecBWSArrow &BWSArrow( int type=0 );

#endif //__DECBWSARROW_LOADER_H__