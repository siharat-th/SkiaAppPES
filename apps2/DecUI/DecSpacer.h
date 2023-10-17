#ifndef __DEC_SPACER_H__
#define __DEC_SPACER_H__

#include "DecWidget.h"

class DecSpacer : public DecWidget {
public:    
    DecSpacer();
    virtual ~DecSpacer() override;

    virtual std::string type() override { return "Spacer"; }

protected:
};

DecSpacer &Spacer();

#endif //__DEC_SPACER_H__