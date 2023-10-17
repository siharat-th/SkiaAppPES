#include "DecBWSInfoBox.h"
   
DecBWSInfoBox::DecBWSInfoBox() {
    size( 100, 44 );
    borderWidth( 2 );
    border( true );
    active( false );
    _pText = new DecText();
    _pText->align( ALIGN_CENTER );
    _( *_pText );
}

DecBWSInfoBox::~DecBWSInfoBox() {

}

DecBWSInfoBox &BWSInfoBox() {
    DecBWSInfoBox *w = new DecBWSInfoBox();
    return *w;    
}

DecWidget& DecBWSInfoBox::active(bool a) {
    _isActive = a;
    return *this;
}