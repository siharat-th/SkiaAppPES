#include "DeclarativeUI.h"
#include "DecSVGDom.h"
#include "DecUI.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkStream.h"
#include "src/utils/SkOSPath.h"
#include "modules/skresources/include/SkResources.h"
#include "modules/svg/include/SkSVGDOM.h"
#include "modules/svg/include/SkSVGNode.h"
#include "modules/svg/include/SkSVGSVG.h"
#include "modules/svg/include/SkSVGText.h"
#include "modules/svg/include/SkSVGG.h"
#include "modules/svg/include/SkSVGRenderContext.h"
#include "SVGDomEditor.h"
#include "SVGEditor.h"

// for SK_ARRAY_COUNT
#include "include/core/SkTypes.h"

const int paperWidth[] = { 52, 74, 105, 148, 210, 297, 420, 594, 841 };
const int paperHeight[] = { 74, 105, 148, 210, 297, 420, 594, 841, 1188 };
const char *paperName[] = { "A8", "A7", "A6", "A5", "A4", "A3", "A2", "A1", "A0", "n/a" };
const int numPaperType = 9;

const char *tagName[] = {
    "Circle",
    "ClipPath",
    "Defs",
    "Ellipse",
    "FeBlend",
    "FeColorMatrix",
    "FeComposite",
    "FeDiffuseLighting",
    "FeDisplacementMap",
    "FeDistantLight",
    "FeFlood",
    "FeGaussianBlur",
    "FeImage",
    "FeMorphology",
    "FeOffset",
    "FePointLight",
    "FeSpecularLighting",
    "FeSpotLight",
    "FeTurbulence",
    "Filter",
    "G",
    "Image",
    "Line",
    "LinearGradient",
    "Mask",
    "Path",
    "Pattern",
    "Polygon",
    "Polyline",
    "RadialGradient",
    "Rect",
    "Stop",
    "Svg",
    "Text",
    "TextLiteral",
    "TextPath",
    "TSpan",
    "Use"
};

const char *visibilityType[] = {
    "Visible",
    "Hidden",
    "Collapse",
    "Inherit"
};

DecSVGEditor::DecSVGEditor() {
    fDom = NULL;
    root = NULL;
    _pTextDocumentPath = NULL;
    _pTextDocumentSize = NULL;

    size( -1, -1 );
    border( false );
    background( false );

    _( Container()
        .size( 44, -1 )
        .border(false)
        .background(false)
    );
    //._( SVGEditor( "resources/svg/HBAN-01.svg") );
    //._( SVGEditor( "resources/svg/MagicPattern-01.svg") );
    //_(SVGDomEditor("resources/svg/Blouse-01.svg").ref((DecWidget**)&pSVGDomEditor).cache(true));
    _( SVGDomEditor().ref((DecWidget**)&pSVGDomEditor).cache(true));
    _( FileDialogBox()
        .ref( (DecWidget **)&_pFileDialog )
    );
    _( Column()
        .size( 360, -1 )
        .border(false)
        .background(false)
        .gap( 10 )
        .padding( 10, 10, 10, 10 ) 
        ._( Row()
            .size(-1, 32)
            .gap(4)
            .border(false)
            .background(false)
            ._(TextEdit("", 
                [=](DecTextEdit& textedit) {
                    SkDebugf("DecSVGEditor::DecTextEdit::callback: %s\n", textedit.text());
                })
                .size(-1, 32)
                .ref((DecWidget**)&_pTextDocumentPath)
            )
            ._( Button("", 0xf01e,
                [=](DecButton& btn) {
                    static const char* resources[] = {
                        "HBAN-A1=2-01",
                        "HBAN-A1=2-02",
                        "rect02",
                        "circle01",
                        "line01",
                        "polygon01",
                        "polyline01",
                        "ellipse01",
                        "paths-arcs01",
                        "paths-arcs02",
                        "path-cubic01",
                        "path-cubic02",
                    };
                    static const int resourcesSize = SK_ARRAY_COUNT(resources);
                    static uint8_t i = 0;
                    //pNodeTree->clear();
                    std::string strPath = std::string("resources/svg/") + resources[++i %= resourcesSize] + ".svg";
                    load(strPath.c_str());
                })
                .size(32, 32)
            )
        )
        ._( Container()
            .size( -1, 360 )
            .border_color( 0xFF808080 )
            ._( Tree()
                .border( false )
                .ref( (DecWidget **)&pNodeTree )
            )
        )
        ._( Text( "" )
            .ref( (DecWidget **)&_pTextDocumentSize )
        )
        ._( Column()
            .size( -1, 0 )
            .border( false )
            .gap( 5 )
            ._( Text( "Load: " ) )
            ._( Row()
                .size( -1, 0 )
                .gap( 10 )
                .border( false )
                ._( Button("Load SVG", 0, [=]( DecButton &btn ) {
                    SkDebugf( "Load SVG\n" );
                    openSVG();
                }))
            )
            ._( Text( "Export: " ).margin_top( 5 ) )
            ._( Row()
                .size( -1, 0 )
                .gap( 10 )
                .border( false )
                ._( Button("SVG", 0, [=]( DecButton &btn ) {
                    SkDebugf( "Export SVG\n" );
                    pSVGDomEditor->exportSVG( "_export/export.svg" );
                }))
                ._( Button("PNG 300 DPI", 0, [=]( DecButton &btn ) {
                    //SkDebugf( "Export PNG\n" );
                    pSVGDomEditor->exportPNG( "_export/test.png", 300 );
                }))
                ._( Button("PDF Document", 0, [=]( DecButton &btn ) {
                    std::string fn = exportPDF();
                    SkDebugf( "Export PDF: %s\n", fn.c_str() );
                    pSVGDomEditor->renderPDF( "Test", false );
                    pSVGDomEditor->endExportPDF();
                }))
            )
        )

    );
}

DecSVGEditor::~DecSVGEditor() {

}

DecSVGEditor &SVGEditor() {
    DecSVGEditor *w = new DecSVGEditor();
    return *w;
}

void DecSVGEditor::onLoad() {
    if (_pTextDocumentPath) {
        _pTextDocumentPath->setText(pSVGDomEditor->fPath.c_str());
        _pTextDocumentPath->dirty();
        pSVGDomEditor->dirty();
    }
    updateNodeTree();
}

void DecSVGEditor::init() {
    //SkDebugf("DecSVGEditor::init\n");
    //SkDebugf("Document path:%s\n", pSVGDomEditor ? pSVGDomEditor->fPath.c_str() : "");

    //std::filesystem::path fullpath( "resources/svg/HBAN-A1=2-01.svg"  );

    //_filepath = fullpath.string();
    //_filename = fullpath.filename().string();
    //_filebasename = fullpath.stem().string();

    //pNodeTree->clear();
    //bool ret = pSVGDomEditor->load(_filepath.c_str());

    this->load("resources/svg/HBAN-A1=2-01.svg");
    this->onLoad();
}

void DecSVGEditor::onResized(int w, int h) {
    if (pSVGDomEditor) {
        pSVGDomEditor->dirty();
    }
    this->dirty();
}

bool DecSVGEditor::load(const char* filepath, uintptr_t /* uint8_t*  */ iptr, size_t length) {
    SkDebugf("Open File: %s\n", filepath);
    bool ret = false;
    std::filesystem::path fullpath = std::filesystem::path( filepath );
    _filepath = fullpath.string();
    _filename = fullpath.filename().string();
    _filebasename = fullpath.stem().string();

    if (_pTextDocumentPath) {
        _pTextDocumentPath->setText("loading...");
        _pTextDocumentPath->dirty();
    }
    if (pNodeTree) {
        pNodeTree->clear();
    }
    if (pSVGDomEditor) {
        //pSVGDomEditor->load(filepath);
        ret = pSVGDomEditor->load(_filepath.c_str(), iptr, length); 
        pSVGDomEditor->dirty();
    }
    return ret;
}

bool DecSVGEditor::onDropFile(int ix, int iy, const char* dropped_file, uintptr_t /* uint8_t*  */ iptr, size_t length) { 
    //DecWidget::onDropFile(ix, iy, dropped_file);
    if (pSVGDomEditor && pSVGDomEditor->hitTest(ix, iy)) {
        bool success = load(dropped_file, iptr, length);
        return true;
    }
    return false;
}

void DecSVGEditor::openSVG() {
    //SkDebugf( "OpenSVG\n" );
    if( _pFileDialog ) {
        _pFileDialog->getFile([=](DecFileDialogBox& dlg, const char* filepath) { 
            //std::filesystem::path fullpath = std::filesystem::path( filepath );
            //_filepath = fullpath.string();
            //_filename = fullpath.filename().string();
            //_filebasename = fullpath.stem().string();

            this->load(filepath);
        });
    }
}

void DecSVGEditor::bubbleAction( int actionID, DecWidget *sender, void *data ) {
    if( actionID == ACTION_TREE_SELECT_CHANGED ) {
        selectNode( (DecTreeItem *)data );
    }
}

void DecSVGEditor::selectNode( DecTreeItem *pItem ) {
    if( pSVGDomEditor ) {
        if( pItem ) {
            pSVGDomEditor->selectNode( (SkSVGNode *)pItem->_externalData );
        }
        else {
            pSVGDomEditor->selectNode( NULL );
        }
    }
}

std::string DecSVGEditor::exportPDF(bool a4) {
    std::string fn = std::string("_export/") + _filebasename;
    fn = fn + ".pdf";

    pSVGDomEditor->startExportPDF(fn.c_str());      
    return fn;  
}

void DecSVGEditor::setNodeVisibility( DecTreeItem *treeItem, bool bVisible ) {
    SkSVGNode *node = (SkSVGNode *)treeItem->_externalData;
    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;
    if( bVisible ) {
        vType = SkSVGVisibility::Type::kVisible;
    }
    else {
        vType = SkSVGVisibility::Type::kHidden;
    }

    auto visible = SkSVGVisibility(vType);
    SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
    node->setVisibility(visibilityVisible);

    DecIcon *icon = (DecIcon *)treeItem->_externalWidget;
    icon->text_color( vType == SkSVGVisibility::Type::kVisible ? 0xFF000000 : 0xFFC0C0C0 );

    std::vector<DecWidget*>children = treeItem->getChildrenNodes();
    for( int i=0; i<children.size(); i++ ) {
        DecTreeItem *w = (DecTreeItem *)children[i];
        setNodeVisibility( w, bVisible );
    }
    pSVGDomEditor->dirty();
}

bool DecSVGEditor::getNodeVisibility( DecTreeItem *treeItem ) {
    SkSVGNode *node = (SkSVGNode *)treeItem->_externalData;
    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;

    const SkSVGProperty<SkSVGVisibility, true> &visibility = node->getVisibility();
    bool bVisible = true;
    if( visibility.isValue() ) {
        vType = visibility->type();
    }
    if( vType == SkSVGVisibility::Type::kVisible ) {
        bVisible = true;
    }
    else {
        bVisible = false;
    }
    return bVisible;
}

void DecSVGEditor::toggleNodeVisibility( DecTreeItem *treeItem ) {
    bool bVisible = getNodeVisibility(treeItem);

    bVisible = !bVisible;
    setNodeVisibility( treeItem, bVisible );
}

void DecSVGEditor::addSubNodeToTreeItem(DecTreeItem& treeNode, SkSVGNode* node, int deep) {
    DecTreeItem *new_node;

    char sz[128];

    SkSVGTag tag = node->tag();

    if( tag == SkSVGTag::kTextLiteral ) {
        return;
    }

    // maxdeep:50
    char chDeep[] = "--------------------------------------------------";
    chDeep[deep] = 0;

    SkSVGVisibility::Type vType = SkSVGVisibility::Type::kVisible;


    const SkSVGProperty<SkSVGVisibility, true> &visibility = node->getVisibility();
    const SkSVGProperty<SkSVGStringType, false> &propData = node->getDataName();
    const SkSVGStringType* data = propData.getMaybeNull();
    const char* cpchData = data ? data->c_str() : "";

   

    if( visibility.isValue() ) {
        vType = visibility->type();

        // can remove >>>
        /*
        int numChildren = 0;
        if (node->hasChildren()) {
            if (tag == SkSVGTag::kG) {
                SkSVGG* pG = (SkSVGG*)node;
                numChildren = (int)pG->fChildren.size();
            }
        }
        SkDebugf("%snode: %s(child:%d), has Visibility[%s], data-name:'%s' \n",
                 chDeep,
                 tagName[(int)tag],
                 numChildren,
                 visibilityType[(int)vType],
                 cpchData);
        */
        // can remove <<<
    } else {
        if (tag == SkSVGTag::kG) {
            auto visible = SkSVGVisibility(SkSVGVisibility::Type::kVisible);
            SkSVGProperty<SkSVGVisibility, true> visibilityVisible(visible);
            node->setVisibility(visibilityVisible);

            // can remove >>>
            /*
            int numChildren = 0;
            if (node->hasChildren()) {
                if (tag == SkSVGTag::kG) {
                    SkSVGG* pG = (SkSVGG*)node;
                    numChildren = (int)pG->fChildren.size();
                }
            }
            SkDebugf("%snode: %s(child:%d) set Visibility[Visible], data-name:'%s' \n",
                     chDeep,
                     tagName[(int)tag],
                     numChildren,
                     cpchData);
            */
            // can remove <<<
        }
    }

    sprintf(sz, "%s %s ", tagName[(int)tag], cpchData);

    treeNode._(TreeItem(sz).ref((DecWidget**)&new_node), true);  // add to front to display like Adobe Illustrator
    new_node->_externalData = node;
    new_node->addWidget( Icon( 0xf06e)
        .ref( (DecWidget **)&new_node->_externalWidget )
        .externalData( new_node )
        .text_color( vType == SkSVGVisibility::Type::kVisible ? 0xFF000000 : 0xFFC0C0C0 )
        .onMousePressed( [=](DecWidget &widget) -> bool {
            toggleNodeVisibility( (DecTreeItem *)widget._externalData );
            return false;
        })
    );
    if( node->hasChildren() ) {
        if( tag == SkSVGTag::kG ) {
            SkSVGG *pG = (SkSVGG *)node;
            int numChildren = (int)pG->fChildren.size();
            for( int i=0; i<numChildren; i++ ) {
                sk_sp<SkSVGNode> child_node = pG->fChildren.at( i );

                //rootItem._( TreeItem( sz ) );
                addSubNodeToTreeItem( *new_node, child_node.get(), deep+1 );
            }
        }
    }
    treeNode.collapse();
}

const char *DecSVGEditor::getFitPaperSize( float w, float h ) {
    int fW = (int)(w * 25.4);
    int fH = (int)(h * 25.4);
    if( fW > fH ) { // landscape
        fW = (int)(h * 25.4);
        fH = (int)(w * 25.4);
    }
    for( int i=0; i<numPaperType; i++ ) {
        if( paperWidth[i] >= fW && paperHeight[i] >= fH ) {
            return paperName[i];
        }
    }
    return paperName[numPaperType];
}

void DecSVGEditor::updateNodeTree() {
    if( pSVGDomEditor ) {
        pSVGDomEditor->pSVGEditor = this;

        fDom = pSVGDomEditor->fDom;
        if( fDom ) {
            root = fDom->getRoot();

            //fDom->findNodeById()

            SkSize szC = fDom->containerSize();
            //SkDebugf( "Container size: %0.2f %0.2f\n", szC.fWidth, szC.fHeight );

            SkSize szR = root->intrinsicSize(SkSVGLengthContext(SkSize::Make(0, 0), 72));
            //SkDebugf( "SVG Root Size: %0.2f %0.2f\n", szR.fWidth, szR.fHeight );
            //SkDebugf( "SVG Root Inches: %0.2f %0.2f\n", szR.fWidth/72.0, szR.fHeight/72.0 );
            if( szR.fWidth == 0 || szR.fHeight == 0 ) {
                szR.fWidth = szC.fWidth;
                szR.fHeight = szC.fHeight;
            }

            char sz[256];
            sprintf( sz, "Paper Size: %0.2f x %0.2f inches (%s)", szR.fWidth/72.0, szR.fHeight/72.0,
                getFitPaperSize( szR.fWidth/72.0, szR.fHeight/72.0 ) );
            if( _pTextDocumentSize ) {
                _pTextDocumentSize->setText( sz );
            }

            SkSVGTag rtag = root->tag();
            sprintf( sz, "%s", tagName[(int)rtag] );

            DecTreeItem &rootItem = TreeItem( sz );
            if( pNodeTree ) {
                pNodeTree->_( rootItem );
            }

            if( root->hasChildren() ) {
                int numChildren = (int)root->fChildren.size();

                if( pNodeTree ) {
                    for( int i=0; i<numChildren; i++ ) {
                        sk_sp<SkSVGNode> node = root->fChildren.at( i );

                        //rootItem._( TreeItem( sz ) );
                        addSubNodeToTreeItem( rootItem, node.get(), 1 );

                        // can remove >>>
                        /*
                        // node->getVisibility() return SkSVGProperty<SkSVGVisibility, true>
                        auto propVisibiliity = node->getVisibility();
                        // propVisibiliity.isValue() retrurn bool
                        if (propVisibiliity.isValue()) {
                            // propVisibiliity->type() return enum class SkSVGVisibility::Type
                            // can be
                            // visibility == SkSVGVisibility::Type::kHidden
                            // visibility != SkSVGVisibility::Type::kVisible
                            // visibility == SkSVGVisibility::Type::kVisible
                            // or intType = (int)visibility
                            auto visibility = propVisibiliity->type();
                            SkDebugf("visibilityType:%s\n", visibilityType[(int)visibility]);
                        } else {
                            SkDebugf("visibilityType:%s\n", "none");
                        }
                        */
                        // can remove <<<

                        /*
                        if( tag == SkSVGTag::kTextLiteral ) {
                            SkSVGTextFragment* pNode = (SkSVGTextFragment*)(node.get());
                        }
                        else if( tag == SkSVGTag::kG ) {
                            if( node->hasChildren() ) {
                                for( int ii=0; ii<numChildren; ii++ ) {
                                    sk_sp<SkSVGNode> nd = root->fChildren.at( ii );
                                    SkSVGTag tg = node->tag();
                                    SkDebugf( "Child Tag: %s\n", tagName[(int)tg] );
                                }
                            }
                        }
                        */

                    }
                }
            }
        }

    }
}
