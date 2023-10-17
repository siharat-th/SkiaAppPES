#ifndef __DEC_MODAL_H__
#define __DEC_MODAL_H__

#include "DecWidget.h"

class DecModal : public DecWidget {
public:    
    DecModal();
    virtual ~DecModal() override;

    virtual std::string type() override { return "Modal"; }

    virtual DecWidget& visible( bool v ) override;
protected:

    virtual void _draw_content(SkCanvas &canvas) override;
};

DecModal &Modal();

#endif //__DEC_MODAL_H__