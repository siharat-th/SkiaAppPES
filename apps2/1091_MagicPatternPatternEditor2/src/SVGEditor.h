#ifndef __SVGEDITOR_WIDGET_H
#define __SVGEDITOR_WIDGET_H

#include "DeclarativeUI.h"
#include "modules/svg/include/SkSVGNode.h"

class SkSVGSVG;

class DecSVGDomEditor;

enum POINT_INDEX {
    PT_SHOULDER_TIP,
    PT_SHOULDER_NECK,
    PT_CF_NECK,
    PT_CF_WAIST,
    PT_SS_BUST,
    PT_SS_WAIST,
    PT_MAX
};

enum SIZE_INDEX {
    SZ_FULL_LENGTH,
    SZ_CF_LENGTH,
    SZ_NECK_HORZ,
    SZ_NECK_VERT,
    SZ_NECK_CIRCUM,
    SZ_BUST_CIRCUM,
    SZ_WAIST_CIRCUM,
    SZ_SHOULDER_SEAM,
    SZ_SIDE_SEAM,
    SZ_MAX
};

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
    std::vector<std::string>_vSewingLineInfo[5];
    int getSizeIndex();

    SkPath *_SelectedSewingLinePath;
    int _selectedSewingLineIndex;
    SkPoint _currentPoint;

    bool _bPatternPointsSet[5][PT_MAX];
    SkPoint _patternPoints[5][PT_MAX];

    float _patternSizes[5][SZ_MAX];

    void clearPatternPoints();
    void setPatternPoint( int index );
    void clearPatternPoint( int index );

    void savePatternPoints();
    void loadPatternPoints();

    void calculateSizes();
    void updateSizes();

    void loadSewingLine();
    void updateSewingLineList();

    void clearSelectedSewingLinePath();
    void updateSelectedSewingLine();
    void drawSewingLine( DecSVGDomEditor *, SkCanvas &canvas );
    void drawGuides( SkCanvas &canvas );

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
    DecTree* pNodeTree;
    DecMessageBox *_pMessageBox;
    DecFileList *_pFileList;
    DecList *_pPathNodeList;
    DecComboBox *_pComboBoxTargetPaper;

    DecText *_pTextFullLength;
    DecText *_pTextNeck;
    DecText *_pTextCircum;
    DecText *_pTextSeam;

    DecText *_pTextMPBodySizes;

    DecTextEdit *_pTENeckCircum;

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