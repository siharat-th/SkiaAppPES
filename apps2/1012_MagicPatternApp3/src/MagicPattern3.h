#ifndef __MAGICPATTERN3_H__
#define __MAGICPATTERN3_H__

#include "modules/svg/include/SkSVGNode.h"
#include "DeclarativeUI.h"
#include "MagicPatternSVG.h"
#include "MPPanel.h"

class SkSVGSVG;

const int NUM_BG = 9;
const int NUM_LENGTH = 6;
const int NUM_TYPE = 7; // 1 = สวมศีรษะ, 2 = เอวผ่าหน้า, 3 = สะโพกบนผ่าหน้า.... 6 = คลุมเข่าผ่าหน้า
const int NUM_NECK = 8;
const int NUM_SLEEVE = 14;
const int NUM_HAIR = 6;
const int NUM_COLLAR = 7;
const int NUM_HEM_BODY_LENGTH = 3;    
const int NUM_HEM_BODY_TYPE = 2;    
const int NUM_HEM_LENGTH = 3;   
const int NUM_PANT = 2;
const int NUM_POCKET_LEVEL = 5;
const int NUM_POCKET_SIDE = 2;
const int NUM_POCKET_TYPE = 11;
const int NUM_POCKETCOVER_TYPE = 5;
const int NUM_HAND = 3;

extern const char *PID2SKU[];
extern int num_pid2sku;

class DecMagicPattern : public DecStackContainer {
public:    
    DecMagicPattern();
    virtual ~DecMagicPattern() override;

    virtual void init() override;
    virtual void update() override;
    virtual void onResized(int w, int h) override;

    void onLoad(DecSVGDom &svg);
    void onDeviceChanged( int index );
    void scanSubNode( SkSVGNode* node, int deep, std::string parentNodeName );

    sk_sp<SkSVGDOM> fDom;   
    SkSVGSVG *root;  

    DecContainer *pContainer;
    DecColumnContainer *pControllerContainer;
    DecComboBox *pComboDevices;
    DecMagicPatternSVG *pSVG;
    DecScroller *pButtonPanel;
    DecMPPanel *pMPPanel;
    DecMPButton *pButtonCollar;
    DecMPButton *pButtonHem;
    DecText *_pTextProductID;

    float fSVGScaleMin;
    float fSVGScaleMax;
    float fSVGScaleFit;
    float fSVGScale;
    float fSVGZoomChange;

    int nSVGHeight;

    void setNodeVisibility( SkSVGNode* node, bool bVisible );
    void setNodeColor( SkSVGNode *node, SkColor color );

    int nBackground;
    int nHair;
    int nType;
    int nLength;
    int nNeck;
    int nSleeve;
    int nCollar;
    int nHem;
    int nPant;
    int nPocket[NUM_POCKET_LEVEL][NUM_POCKET_SIDE];
    int nPocketCover[NUM_POCKET_LEVEL][NUM_POCKET_SIDE];

    int nLastNeck;
    int nLastType;
    int nLastCollar;

    int svgStartLoad;
    int svgLoaded;
    int svgScanned;
    int svgReady;

    void exportPNGs();
    void exportPNG();
    void exportSVG();
    void exportProductList();

    bool checkSKUVideo( std::string sz0, std::string sz1, std::string sz2, std::string sz3 );
    bool checkSKUVideo2( std::string sz0, std::string sz1, std::string sz2, std::string sz3, std::string sz5 );
    bool checkSKUMaterial( std::string sz0, std::string sz1, std::string sz2, std::string sz3, 
        std::string sz5, std::string sz6, std::string sz7 );

    void initNodes();  

    int __jobIndex;  
    bool __jobExportPNG(DecWidget &widget);

    void setBackground( int n );
    void setHair( int n );
    void setHand( int n );
    void setType( int n );
    void setLength( int n );
    void setNeck( int n );
    void setSleeve( int n );
    void setCollar( int n );
    void setHem( int n );
    void setPant( int n );
    void setPocket( int level, int side, int type );
    void setPocketCover( int level, int side, int type );

    void setColor( uint32_t color );

    void showBody();

    DecColumnContainer *pKeyPad;
    DecText *_pTextSearch;
    std::string searchString;  
    void addSearchChar( std::string s );   

    int nCurrentPatternID;
    std::string sku;

    void getPattern();     
    std::string getDescription();
          
    void gotoPattern( int id );
    void randomPattern();
    void randomPocket();
    bool bRandomPocket;
    void randomStyle( int pid, bool bPocket=true );

    void loadPattern( int pid );

    uint32_t currentColor;
    uint32_t currentColorCollar;
    uint32_t currentColorSleeve;
    uint32_t currentColorHem;
    uint32_t currentColorPocket0;
    uint32_t currentColorPocket1;
    uint32_t currentColorShoe;
    int currentSVGTag;

    SkSVGNode *_vBGNode[NUM_BG];
    SkSVGNode *_vLengthNode[NUM_LENGTH];
    SkSVGNode *_vNeckNode[NUM_TYPE][NUM_NECK];   
    SkSVGNode *_vSleeveNode[NUM_SLEEVE];
    SkSVGNode *_vHairNode[NUM_HAIR];
    SkSVGNode *_vCollarNode[NUM_COLLAR];
    SkSVGNode *_vHemNode[NUM_HEM_BODY_LENGTH][NUM_HEM_BODY_TYPE][NUM_HEM_LENGTH]; 
    SkSVGNode *_vPantNode[NUM_PANT];
    SkSVGNode *_vHandNode[NUM_HAND];

    SkSVGNode *_vBodyNode[2];
    SkSVGNode *_vShoeNode[2];

    SkSVGNode *_vPocketNode[NUM_POCKET_LEVEL][NUM_POCKET_SIDE][NUM_POCKET_TYPE];
    SkSVGNode *_vPocketCoverNode[NUM_POCKET_LEVEL][NUM_POCKET_SIDE][NUM_POCKETCOVER_TYPE];

    DecText *pTextPID;
    DecText *pTextSKU;

    void updateProductID();


    int svgLayerCount[15];
};

DecMagicPattern &MagicPattern();



#endif //__MAGICPATTERN3_H__