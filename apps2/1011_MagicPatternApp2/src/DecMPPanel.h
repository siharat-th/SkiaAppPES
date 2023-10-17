#ifndef __DEC_HORZ_SELECTOR_H__
#define __DEC_HORZ_SELECTOR_H__

#include "DecWidget.h"
#include "DecColumnContainer.h"
#include "DecPageContainer.h"

class DecMagicPattern;

class DecMPPanel : public DecContainer {
public:    
    DecMPPanel();
    virtual ~DecMPPanel() override;

    virtual std::string type() override { return "HorzSelector"; }

    void setMagicPatternWidget( DecMagicPattern *pMagicPattern );

    void setShirtType( int i );
    void setShirtLength( int i );
    void setShirtNeck( int i );
    void setShirtCollar( int i );
    void setShirtSleeve( int i );
    void setShirtHem( int i );
    void setColor( int c );

    int _shirt_mode;

    int _shirt_type;
    int _shirt_length;
    int _shirt_neck;
    int _shirt_collar;
    int _shirt_sleeve;
    int _shirt_hem;

    int _shirt_body_color;
    int _shirt_sleeve_color;
    int _shirt_hem_color;
    int _shirt_collar_color;

    void updateShirtBody();
    void updateShirtSleeve();
    void updateShirtHem();
    void updateShirtCollar();
    void updateShirtColor();

    void setShirtParams( int type, int length, int neck, int collar, int sleeve, int hem );
protected:
    DecColumnContainer *container;

    DecPageContainer *pModeWidget;
    DecPageContainer *pDesignWidget;
    DecPageContainer *pNeck;
    DecPageContainer *pCollarType;

    DecMagicPattern *_pMagicPattern;

    int _shirt_body_index;    

};

DecMPPanel &MagicPatternPanel();

#endif //__DEC_HORZ_SELECTOR_H__