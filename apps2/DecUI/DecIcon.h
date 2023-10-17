#ifndef __DEC_ICON_H__
#define __DEC_ICON_H__

#include "DecText.h"

class DecIcon : public DecText {
public:
    DecIcon( const uint16_t code );
    virtual ~DecIcon() override;

    void setIcon( const uint16_t code );
};

DecIcon &Icon( const uint16_t code );

#endif //__DEC_ICON_H__