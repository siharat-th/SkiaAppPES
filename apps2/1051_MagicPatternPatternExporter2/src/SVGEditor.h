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

    void clearSewingLine();
    void loadSewingLine( const char *filebasename );
    void drawSewingLine( DecSVGDomEditor *, SkCanvas &canvas );

    virtual void bubbleAction( int actionID, DecWidget *sender, void *data=NULL ) override; 

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

    void exportProductPDF( bool cover=false, bool force=false );

    std::string getSKU( int _length, int _type, int _neck, int _hem, int _sleeve, int _collar);
    void generateProductCodeList();

    bool _isTiledExport;

    std::string exportPDF( bool a4=false );
    void exportPDFs();
    void exportPNGs();
    void exportProducts();
    void exportProductsByList( const char *fn );
    void exportParts();
    void exportPartPNG();

    void setProduct( int id, const char *sku );

    int _product_id;
    std::string _sku;

    //void drawContentBounds( DecSVGDomEditor *dom, SkCanvas &canvas );

    void changeTargetPaper( const char *paper );

    bool _forceBlack;

    int getSizeIndex();

    SkPath *_sewingLinePath[5];
    std::vector<std::string>_vSewingLineInfo[5];
    void overlayLogo( SkCanvas &canvas, int w, int h );
    void overlaySewingLine( SkCanvas &canvas );
    void renderPDF( const char *patternName, bool tiled );

    std::string _sku_list[3719];
    int _product_id_list[3719];
    int _pid2index_list[3719];
    
    std::vector<std::string>_vProductFileList;
    std::vector<std::string>_vProductImageList;

    int getProductCode( int _type, int _length, int _hem, int _sleeve, int _neck, int _collar);

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
    DecList *_pProductFileList;
    DecList *_pProductImageList;
    DecList *_pSKUList;
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

    void updateProductImages();

    // Level 1
    std::vector<DecTreeItem *>_vTreeItems;
    //std::vector<SkRect>_vNodeBounds;
    //SkRect contentBounds;

    // Level 0
    DecTreeItem *_pTreeItem0;

    bool _generatePattern;
    int _generateFrame;
    int _generateCount;   

    bool _is_png_forweb;

    std::string size_name;
    int __jobProductIndex;
    int __jobSizeIndex;
    int __jobFileIndex;
    int __jobPNGFileIndex;
    std::vector<std::string> __jobProductList;
    bool __jobFileExport( DecWidget &widget );
    bool __jobPNGFileExport( DecWidget &widget );
    bool __jobSizeExport( DecWidget &widget );
    bool __jobProductExport( DecWidget &widget );
    bool __jobPartExport( DecWidget &widget );
    bool __jobPNGSizeExport( DecWidget &widget );
    bool __jobProductExportByList(DecWidget &widget);

    void overlayLayout( SkCanvas &canvas );
    void overlayImage( SkCanvas &canvas ); 

    void overlayCoverImage( SkCanvas &canvas );   
    void overlayMaterial( SkCanvas &canvas );

    bool checkSKUMaterial( std::string sz0, std::string sz1, std::string sz2, std::string sz3, 
        std::string sz5, std::string sz6, std::string sz7 );

};

DecSVGEditor &SVGEditor();

#endif //__SVGEDITOR_WIDGET_H