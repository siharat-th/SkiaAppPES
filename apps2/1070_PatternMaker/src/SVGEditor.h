#ifndef __SVGEDITOR_WIDGET_H
#define __SVGEDITOR_WIDGET_H

#include "DeclarativeUI.h"
#include "modules/svg/include/SkSVGNode.h"

class SkSVGSVG;

class DecSVGDomEditor;

class DecSVGEditor : public DecRowContainer {
public:    
    DecSVGEditor();
    virtual ~DecSVGEditor() override;

    virtual std::string type() override { return "DecSVGEditor"; }

    virtual void init() override;
    void onLoad();

    virtual void update() override;    

    void updateNodeTree();
    void updateSVGInfo();

    virtual void bubbleAction( int actionID, DecWidget *sender, void *data=NULL ) override; 

    void onPathVerbSelect( DecList *list );

    void selectNode( DecTreeItem *pItem );   
    void toggleNodeVisibility( DecTreeItem *item );
    void setNodeVisibility( DecTreeItem *item, bool bVisible, bool includeChildren=false );
    bool getNodeVisibility( DecTreeItem *item );

    std::string getDescription();

    void addSubNodeToTreeItem( DecTreeItem &treeNode, SkSVGNode* node, int deep=0, std::string parentNodeName="" );

    std::string _filepath;
    std::string _filename;
    std::string _filebasename;

    int nTargetPiece;

    void updateNodeVisibility();
    void updateAllCheckBox();
    void updateContentBounds();

    std::string getSKU( int _length, int _type, int _neck, int _hem, int _sleeve, int _collar);
    void generateProductCodeList();

    bool _isTiledExport;

    std::string startExportPDF( const char *filename, bool a4=false );
    void renderPDF( const char *patternName, bool tiled );

    //void overlaySewingLine( SkCanvas &canvas );
    void overlayPattern( SkCanvas &canvas );


    void exportPNG();  
    void exportPDFs();
    void exportPNGs();

    //SkPath *_sewingLinePath[5];
    //SkPath *_editingSewingLine;
    //std::vector<std::string>_vSewingLineInfo[5];
    //std::vector<std::string>_vEditingSewingLineInfo;
    int getSizeIndex();

    //SkPath *_SelectedSewingLinePath;
    //int _selectedSewingLineIndex;

    //void clearSewingLine();
    //void loadSewingLine();
    //void saveSewingLine();
    //void clearEditingSewingLine();

    //void createSewingLine();
    //void updateSewingLine();
    //void appendSewingLine();
    //void updateEditingSewingLine();
    //void updateSelectedSewingLine();
    //void drawSewingLine( DecSVGDomEditor *, SkCanvas &canvas );

    //void toggle01();
    //void toggle14();
    //void toggleX();

    int _product_id;
    std::string _sku;

    //void drawContentBounds( DecSVGDomEditor *dom, SkCanvas &canvas );

    void changeTargetPaper( const char *paper );
    void changeTargetPiece( const char *piece );
    void changeTargetGenerate( const char *gen );

    bool _forceBlack;

    std::vector<std::string>_sku_list;
    std::vector<std::string>_vProductFileList;
    std::vector<std::string>_vProductImageList;

    void onTreeNodeSelected( SkSVGNode *node );

    void drawPattern( DecSVGDomEditor *dom, SkCanvas &canvas );
    void drawNeckLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render=false );
    void drawWaistLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render=false );
    void drawBustLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render=false );
    void drawChestLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render=false );
    void drawCenterFrontLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render=false );
    void drawHipLine( DecSVGDomEditor *dom, SkCanvas &canvas, bool render=false );
    void drawSideSeam( DecSVGDomEditor *dom, SkCanvas &canvas, bool render=false );
    void drawArmHole( DecSVGDomEditor *dom, SkCanvas &canvas, bool render=false );
    void drawText( SkCanvas &canvas, int x, int y, std::string text );

    float fNeckLineY;
    float fWaistLineY;
    float fBustLineY;
    float fRealBustLineY;
    float fChestLineY;
    float fLowHipLineY;
    float fHighHipLineY;
    float fCenterFrontLineX;
    float fTopShoulderLineY;
    float fLowShoulderLineY;
    float fBustX;
    float fWaistX;
    float fHipX;
    float fShoulderX;
    float fChestX;
    float fBackX;

    float fNeckWidth;
    float fNeckDepth;

    int nTargetGenerate;

    bool bTargetGenerated;

    // Skirt Measure
    float fWaistLength;             // รอบเอว
    float fWaistSideDepth;          // ลึกเอว (2.5)
    float fBackDepth;               // ลึกหลัง (1.5)
    float fLowHipLength;            // สะโพกล่าง (size M 22)
    float fTightLength;             // เอวถึงหน้าขา
    float fKneeLength;              // เอวถึงเข่า
    float fAnkleLength;             // เอวถึงข้อเท้า
    float fFloorLength;             // เอวด้านข้างถึงพื้น
    float fBackFloorLength;         // เอวด้านหลังถึงพื้น
    float fFrontFloorLength;        // เอวด้านหน้าถึงพื้น
    float fSkirtLength;             // เอวถึงชายกระโปรง (ตามลูกค้าต้องการ)
    float fHipRoundLength;          // รอบสะโพกล่าง
    float fHipRoundWithBellyLength; // รอบสะโพกล่างรวมพุง
    float fHipRoundWithTightLength; // รอบสะโพกล่างรวมหน้าขา
    float fTightRoundLength;        // รอบหน้าขา

    // Skirt Defined
    float fSkirtLooseValue;         // เผื่อหลวมกระโปรง

    // Skirt Calculated
    float fSkirtWidth;              // ความกว้างกระโปรง

    float fRealBustLength;
    float fNeckRoundLength;
    float fBustRoundLength;
    float fWaistRoundLength;
    float fShoulderSeamLength;
    float fChestWidthLength;
    float fBackWidthLength;

    DecTextEdit *_pTEWaistLength;
    DecTextEdit *_pTEWaistSideDepth;
    DecTextEdit *_pTEBustLength;
    DecTextEdit *_pTELowHipLength;
    DecTextEdit *_pTENeckRoundLength;
    DecTextEdit *_pTEBustRoundLength;
    DecTextEdit *_pTEWaistRoundLength;
    DecTextEdit *_pTEHipRoundLength;
    DecTextEdit *_pTEHipRoundWithBellyLength;
    DecTextEdit *_pTEHipRoundWithTightLength;
    DecTextEdit *_pTETightRoundLength;
    DecTextEdit *_pTEShoulderSeamLength;
    DecTextEdit *_pTEChestWidthLength;
    DecTextEdit *_pTEBackWidthLength;
    DecTextEdit *_pTEThighLength;
    DecTextEdit *_pTEKneeLength;
    DecTextEdit *_pTEAnkleLength;
    DecTextEdit *_pTEFloorLength;
    DecTextEdit *_pTEBackFloorLength;
    DecTextEdit *_pTEFrontFloorLength;
    DecTextEdit *_pTESkirtLength;
    DecTextEdit *_pTEBackDepth;
    DecTextEdit *_pTESkirtLooseValue;
    
    DecPageContainer *_pPageMesurement;
    DecText *_pTextMesurement[2];

protected:  
    DecComboBox *_pComboBoxGenerate;

    DecSVGDomEditor* pSVGDomEditor;
    DecText* _pTextDocumentPath;
    DecTree* pNodeTree;
    DecText *_pTextDocumentSize;
    DecText *_pTextMinimalPaperSize;
    DecText *_pTextTargetPaperSize;
    DecText *_pTextDocumentDPI;
    DecMessageBox *_pMessageBox;
    DecFileList *_pFileList;
    DecFileList *_pImageFileList;
    DecList *_pPathNodeList;
    //DecList *_pSewingLineNodeList;
    DecComboBox *_pComboBoxTargetPaper;
    DecComboBox *_pComboBoxPiece;
    DecPageContainer *_pFilePages;
    DecPageContainer *_pImagePages;

    DecCheckBox *_pCB_All;
    DecCheckBox *_pCB_S;
    DecCheckBox *_pCB_M;
    DecCheckBox *_pCB_L;
    DecCheckBox *_pCB_XL;
    DecCheckBox *_pCB_XXL;      

    bool _is_S;
    bool _is_M;
    bool _is_L;
    bool _is_XL;
    bool _is_XXL;

    bool saveS;
    bool saveM;
    bool saveL;
    bool saveXL;
    bool saveXXL;   

    sk_sp<SkSVGDOM> fDom;
    SkSVGSVG *root;  

    // Level 1
    std::vector<DecTreeItem *>_vTreeItems;
    //std::vector<SkRect>_vNodeBounds;
    //SkRect contentBounds;

    // Level 0
    DecTreeItem *_pTreeItem0;

    bool _generatePattern;
    int _generateFrame;
    int _generateCount;    

    std::string size_name;
    int __jobProductIndex;
    int __jobSizeIndex;
    int __jobPNGFileIndex;
    std::vector<std::string> __jobProductList;
    bool __jobSizeExport( DecWidget &widget );
    bool __jobPNGSizeExport( DecWidget &widget );

};

DecSVGEditor &SVGEditor();

#endif //__SVGEDITOR_WIDGET_H