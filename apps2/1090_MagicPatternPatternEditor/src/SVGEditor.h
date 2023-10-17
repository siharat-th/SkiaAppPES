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

    virtual bool onChar(SkUnichar c, skui::ModifierKey mod) override;

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

    void updateNodeVisibility();
    void updateAllCheckBox();
    void updateContentBounds();

    std::string getSKU( int _length, int _type, int _neck, int _hem, int _sleeve, int _collar);
    void generateProductCodeList();

    bool _isTiledExport;

    std::string startExportPDF( bool a4=false );
    void renderPDF( const char *patternName, bool tiled );

    void overlaySewingLine( SkCanvas &canvas );

    void exportPNG();  
    void exportPDFs();
    void exportPNGs();

    void exportSVG();  
    void exportSVGs();  

    bool __jobSVGSizeExport( DecWidget &widget );

    SkPath *_sewingLinePath[5];
    SkPath *_editingSewingLine;
    std::vector<std::string>_vSewingLineInfo[5];
    std::vector<std::string>_vEditingSewingLineInfo;
    int getSizeIndex();

    SkPath *_SelectedSewingLinePath;
    int _selectedSewingLineIndex;

    void clearSewingLine();
    void loadSewingLine();
    void saveSewingLine();
    void rotateSewingLine();
    void clearEditingSewingLine();

    void createSewingLine();
    void appendSewingLine();
    void updateEditingSewingLine();
    void updateSelectedSewingLine();
    void drawSewingLine( DecSVGDomEditor *, SkCanvas &canvas );

    void toggle01();
    void toggle14();
    void toggleX();

    int _product_id;
    std::string _sku;

    //void drawContentBounds( DecSVGDomEditor *dom, SkCanvas &canvas );

    void changeTargetPaper( const char *paper );

    bool _forceBlack;
    bool _excludeText;
    bool _excludePocket;

    std::vector<std::string>_sku_list;
    std::vector<std::string>_vProductFileList;
    std::vector<std::string>_vProductImageList;

    void onTreeNodeSelected( SkSVGNode *node );

protected:  
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