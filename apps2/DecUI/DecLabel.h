#ifndef __DEC_LABEL_H__
#define __DEC_LABEL_H__

#include "DecWidget.h"
#include "DecStackContainer.h"
#include "DecRowContainer.h"
#include "DecColumnContainer.h"
#include "DecIcon.h"
#include "DecText.h"

class DecLabel : public DecStackContainer {
public:    
    DecLabel( const char *title, const int icon = 0 );
    virtual ~DecLabel() override;

    virtual std::string type() override { return "Label"; }

    virtual DecWidget &fontSize( const int s ) override;

    virtual DecWidget& text_color(uint32_t c) override;

    DecIcon *_icon;
    DecText *_textWidget;
protected:
    DecRowContainer *_container;
};

DecLabel &Label( const char *title, const int icon = 0 );

#endif //__DEC_LABEL_H__