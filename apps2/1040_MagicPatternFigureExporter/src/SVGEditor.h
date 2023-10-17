#ifndef __SVGEDITOR_WIDGET_H
#define __SVGEDITOR_WIDGET_H

#include "DeclarativeUI.h"
#include "modules/svg/include/SkSVGNode.h"

class SkSVGSVG;

class DecSVGDomEditor;
class DecTreeItem;
class DecMessageBox;
class DecFileDialogBox;
class DecComboBox;

class DecSVGEditor : public DecRowContainer {
public:    
    DecSVGEditor();
    virtual ~DecSVGEditor() override;

    virtual std::string type() override { return "DecSVGEditor"; }

    int _keyMode;

    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) override;    

    virtual void init() override;
    void onLoad();

    void updateNodeTree();

    virtual void bubbleAction( int actionID, DecWidget *sender, void *data=NULL ) override; 

    void selectNode( DecTreeItem *pItem );   
    void toggleNodeVisibility( DecTreeItem *item );
    void setNodeVisibility( DecTreeItem *item, bool bVisible );
    bool getNodeVisibility( DecTreeItem *item );

    void addSubNodeToTreeItem( DecTreeItem &treeNode, SkSVGNode* node, int deep=0, std::string parentNodeName="" );

    void openSVG();

    const char *getFitPaperSize( float w, float h );


protected:  
    DecSVGDomEditor* pSVGDomEditor;
    DecText* _pTextDocumentPath;
    DecTree* pNodeTree;
    DecText *_pTextDocumentSize;
    DecFileDialogBox *_pFileDialog;

    sk_sp<SkSVGDOM> fDom;
    SkSVGSVG *root;  

    // Background
    int _nBG;
    int _nCurrentBG;
    DecTreeItem *_vBGTreeItems[5];
    DecComboBox *_pBGComboBox;
    void showBG( int bg );
    void setBG( int bg );

    // BlouseType
    int _nCurrentBlouseType;
    DecComboBox *_pBlouseTypeComboBox;   
    void showBlouseType( int bg );    
    void setType( int t ); 

    // BlouseLength
    int _nLen;
    int _nCurrentBlouseLength;
    DecTreeItem *_vBlouseLengthTreeItems[3];
    DecComboBox *_pBlouseLengthComboBox;   
    void showBlouseLength( int bg );  
    void setLength( int l, bool show=true );

    // BlouseNeck
    int _nNeck;
    int _nCurrentBlouseNeck;
    DecTreeItem *_vBlouseNeckTreeItems[4][5];
    DecComboBox *_pBlouseNeckComboBox;   
    void showBlouseNeck( int n );    
    void setNeck( int n );    

    // BlouseHem
    int _nHem;
    int _nCurrentBlouseHem;
    DecTreeItem *_vBlouseHemTreeItems[3][2][3];
    DecComboBox *_pBlouseHemComboBox;   
    void showBlouseHem( int n );    
    void setHem( int n );     
    
    // Hair
    int _nHair;
    DecTreeItem *_vHairTreeItems[6];
    DecComboBox *_pHairComboBox;
    void showHair( int bg );
    void setHair( int bg );

    // Body
    DecTreeItem *_vBodyTreeItems[2];
    DecComboBox *_pBodyComboBox;    
    void showBody( int bg );
    void setBody( int bg );

    // Pant
    DecTreeItem *_vPantTreeItems[2];
    DecComboBox *_pPantComboBox;    
    void showPant( int bg );
    void setPant( int bg );

    // Hand
    DecComboBox *_pHandComboBox;    
    DecTreeItem *_vHandTreeItems[2];
    void showHand( int bg );        
    void setHand( int bg );


    // Sleeve
    int _nSleeve;
    int _nCurrentSleeve;
    DecTreeItem *_vSleeveTreeItems[13];
    DecComboBox *_pSleeveComboBox;
    void showSleeve( int bg );
    void setSleeve( int s );


    // Collar
    int _nCollar;
    int _nCurrentCollar;
    DecTreeItem *_vCollarTreeItems[7];
    DecComboBox *_pCollarComboBox;   
    void showCollar( int c );        
    void setCollar( int c );

    void exportBG();
    int __jobExportBGIndex;
    bool __jobExportBG( DecWidget &widget );    

    void exportHem();
    int __jobExportHemLengthIndex;
    int __jobExportHemTypeIndex;
    int __jobExportHemHemIndex;
    bool __jobExportHem( DecWidget &widget );      

    void exportBody();
    void exportHand();
    void exportPant();

    void exportCollar();
    int __jobExportCollarIndex;
    bool __jobExportCollar( DecWidget &widget );       

    void exportHair();
    int __jobExportHairIndex;
    bool __jobExportHair( DecWidget &widget );     

    void exportBlouseBody();
    int __jobExportBlouseBodyLengthIndex;
    int __jobExportBlouseBodyTypeIndex;
    int __jobExportBlouseBodyNeckIndex;
    bool __jobExportBlouseBody( DecWidget &widget );    

    void exportSleeve();
    int __jobExportSleeveIndex;
    bool __jobExportSleeve( DecWidget &widget );           
};

DecSVGEditor &SVGEditor();

#endif //__SVGEDITOR_WIDGET_H