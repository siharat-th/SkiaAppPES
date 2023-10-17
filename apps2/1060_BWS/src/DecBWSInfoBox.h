#ifndef __DECBWSINFOBOX_LOADER_H__
#define __DECBWSINFOBOX_LOADER_H__

#include "DeclarativeUI.h"

class DecBWSInfoBox : public DecStackContainer {
public:    
    DecBWSInfoBox();
    virtual ~DecBWSInfoBox() override;

    DecText *_pText;

    virtual DecWidget& active(bool a) override;

    inline virtual DecWidget& text( const char *t) override {
        if( _pText ) {
            _pText->text( t );
        }
        _text = std::string( t );
        return *this;
    }
};

DecBWSInfoBox &BWSInfoBox();

#endif //__DECBWSINFOBOX_LOADER_H__