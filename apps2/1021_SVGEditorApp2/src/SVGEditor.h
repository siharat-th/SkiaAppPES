#ifndef __SVGEDITOR_WIDGET_H
#define __SVGEDITOR_WIDGET_H

#include "DeclarativeUI.h"
#include "modules/svg/include/SkSVGNode.h"

class SkSVGSVG;

class DecSVGDomEditor;
class DecTreeItem;
class DecMessageBox;
class DecFileDialogBox;

class DecSVGEditor : public DecRowContainer {
public:    
    DecSVGEditor();
    virtual ~DecSVGEditor() override;

    virtual std::string type() override { return "DecSVGEditor"; }

    virtual void init() override;

    virtual void onResized(int w, int h) override;
    
    bool load(const char* filepath, uintptr_t /* uint8_t*  */ iptr = 0, size_t length = 0);
    
    virtual bool onKey(skui::Key k, skui::InputState state, skui::ModifierKey mod) override;
    virtual bool onDropFile(int ix, int iy, const char* dropped_file, uintptr_t /* uint8_t*  */ iptr = 0, size_t length = 0) override;

    void onLoad();

    void updateNodeTree();

    virtual void bubbleAction( int actionID, DecWidget *sender, void *data=NULL ) override; 

    void selectNode( DecTreeItem *pItem );   
    void toggleNodeVisibility( DecTreeItem *item );
    void setNodeVisibility( DecTreeItem *item, bool bVisible );
    bool getNodeVisibility( DecTreeItem *item );

    void addSubNodeToTreeItem( DecTreeItem &treeNode, SkSVGNode* node, int deep=0 );

    void OpenSVGFile();

    void saveSVGFile(const char* filename = "Untitled.svg");
    void exportSVGFile(const char* filename = "Untitled.svg");

    void exportPNGFile(const char* filename = "Untitled.png");
    void exportPDFFile(const char* filename = "Untitled.pdf");

    std::string _filepath;
    std::string _filename;
    std::string _filebasename;    

    const char *getFitPaperSize( float w, float h );
    DecSVGDomEditor* pSVGDomEditor;
protected:  
    DecTree* pNodeTree;
    DecText *_pTextDocumentSize;
    DecFileDialogBox *_pFileDialog;

    sk_sp<SkSVGDOM> fDom;
    SkSVGSVG *root;  

};

DecSVGEditor &SVGEditor();

#endif //__SVGEDITOR_WIDGET_H