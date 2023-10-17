#ifndef __DEC_MAGIC_PATTERN_H__
#define __DEC_MAGIC_PATTERN_H__

#include "DecWidget.h"
#include "DecContainer.h"
#include "DecColumnContainer.h"
#include "DecMPPanel.h"
#include "DecImage.h"
#include "DecButton.h"
#include "DecText.h"

class DecPageContainer;

class DecMagicPattern : public DecContainer {
public:    
    DecMagicPattern( std::string option );
    virtual ~DecMagicPattern() override;

    virtual std::string type() override { return "MagicPattern"; }

    void setBackground( int i );
    void setHair( int i );
    void setBody( int i );
    void setSleeve( int i );
    void setHem( int i );
    void setCollar( int collar );
    void setColor( int bodyColor, int collarColor, int sleevColor, int hemColor );

    virtual void update() override;

    void updateSKU();
    void updateProductCode();
    void updateMPPanel();

    void random();

    std::string getDescription();
    void getPattern();

    int _body;
    int _sleeve;
    int _hem;
    int _collar;
    int _type;
    int _length;
    int _neck;   
    int _hair; 

    int _queryProductID;

    char _sku[32];
    bool checkSKUMaterial( std::string sz1, std::string sz2, std::string sz5, std::string sz6 );

    std::string getSKU( int _length, int _type, int _neck, int _hem, int _sleeve, int _collar);
    std::vector<std::string>_sku_list;
    void generateProductCodeList();

    DecColumnContainer *pKeyPad;
    std::string searchString;
    void addSearchChar( std::string s );

    void gotoPattern( int pid );
protected:
    DecColumnContainer *container;

    DecMPPanel *_pMPPanel;
    DecPageContainer *_pBody;
    DecPageContainer *_pSleeve;
    DecPageContainer *_pHemType;
    DecPageContainer *_pHem0;
    DecPageContainer *_pHem1;
    DecPageContainer *_pCollarType;
    DecPageContainer *_pCollar0;
    DecPageContainer *_pCollar1;
    DecPageContainer *_pBackground;
    DecPageContainer *_pHair;

    DecStackContainer *_pMainImage;

    DecText *_pTextSearch;

    DecImage *_pHuman;
    DecImage *_pHand;

    DecText *_pTextProductID;

    DecText *_pSKU;
    DecText *_pProductCode;
    int _productCode;

    DecButton *_pRandomButton;

    bool _generateProduct;
    int _generateFrame;
    int _generateCount;

    int _generateType;
    int _generateLength;
    int _generateNeck;
    int _generateHem;
    int _generateSleeve;
    int _generateCollar;
};

DecMagicPattern &MagicPattern( std::string option );

#endif //__DEC_MAGIC_PATTERN_H__